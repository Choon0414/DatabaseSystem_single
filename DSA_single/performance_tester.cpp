#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <cmath>
#include <algorithm>
#include <sys/time.h> // 시간 측정을 위한 헤더 추가
#include <sys/resource.h> // 자원 사용량 측정을 위한 헤더 추가
#include "performance_tester.h"

using namespace std;
using namespace chrono;

string PerformanceTester::formatDouble(double value, int precision) const {
        ostringstream out;
        out << fixed << setprecision(precision) << value;
        return out.str();
}

string PerformanceTester::commas(int number) {
    string numStr = to_string(number);
    string result;

    int count = 0;
    for (auto it = numStr.rbegin(); it != numStr.rend(); ++it) {
        result.push_back(*it);
        count++;

        // 3자리마다 쉼표 추가 (마지막 자리에 추가하지 않음)
        if (count % 3 == 0 && (it + 1) != numStr.rend()) {
            result.push_back(',');
        }
    }

    reverse(result.begin(), result.end());
    return result;
}

CpuStats PerformanceTester::getCpuStats() {
    ifstream procStat("/proc/stat");
    CpuStats stats = {0};

    if (procStat.is_open()) {
        string line;
        getline(procStat, line);
        if (line.substr(0, 3) == "cpu") {
            sscanf(line.c_str(), "cpu %ld %ld %ld %ld %ld %ld %ld %ld",
                    &stats.user, &stats.nice, &stats.system, &stats.idle,
                    &stats.iowait, &stats.irq, &stats.softirq, &stats.steal);
        }
        procStat.close();
    }
    return stats;
}

double PerformanceTester::calculateCpuUsage(const CpuStats& prevStats, const CpuStats& currStats) {
    long prevIdle = prevStats.idle + prevStats.iowait;
    long currIdle = currStats.idle + currStats.iowait;

    long prevActive = prevStats.user + prevStats.nice + prevStats.system + prevStats.irq + prevStats.softirq + prevStats.steal;
    long currActive = currStats.user + currStats.nice + currStats.system + currStats.irq + currStats.softirq + currStats.steal;

    long totalPrev = prevActive + prevIdle;
    long totalCurr = currActive + currIdle;

    long totalDelta = totalCurr - totalPrev;
    long activeDelta = currActive - prevActive;

    return (double)activeDelta / totalDelta * 100.0; // CPU 사용률 (%)
}

void PerformanceTester::setMaxMemory() {
    ifstream meminfo("/proc/meminfo");
    string line;

    while (getline(meminfo, line)) {
        if (line.find("MemTotal") != string::npos) {
            stringstream ss(line);
            string label;
            ss >> label >> maxMemory; // MemTotal 값 읽기
            break;
        }
    }
    maxMemory /= 1024.0; // KB 단위로 반환
}

double PerformanceTester::setMemoryUsage() {
    std::ifstream procStatus("/proc/self/status");
    std::string line;
    size_t memory = 0;

    while (std::getline(procStatus, line)) {
        if (line.find("VmRSS:") == 0) { // Resident Set Size: 실제 메모리 사용량
            std::string memStr = line.substr(line.find_first_of("0123456789"));
            memory = std::stoul(memStr); // 단위: KB
            break;
        }
    }
    return (memory / 1000.0); // 메모리 사용량 MB
}

void PerformanceTester::printResult(string workname, double time, double cpuUsage, double memoryUsage) const {
    cout << fixed << setprecision(6);
    cout << "[ " << workname << "]" << endl;
    cout << "● 처리 시간: " << time <<  " 초" << endl;
    cout << "● CPU 사용률: " << cpuUsage << " %" << endl;
    cout << "● 메모리 사용량: " << formatDouble(memoryUsage,3) + " MB / " + formatDouble(maxMemory, 1) + " MB\n" << endl;
}

void PerformanceTester::printAvgResult() const {
    cout << fixed << setprecision(6);
    cout << "[ 읽기 + 조인 + 출력]" << endl;
    cout << "● 총 처리 시간: " << (loadTime + outputTime + joinTime) <<  " 초" << endl;
    cout << "● 평균 CPU 사용률: " << ((loadCpuUsage + outputCpuUsage + joinCpuUsage)/3) << " %" << endl;
    cout << "● 평균 메모리 사용량: " << formatDouble(((loadMemoryUsage + joinMemoryUsage + outputMemoryUsage)/3),3) + " MB\n" << endl;
}

void PerformanceTester::printStatus() {
    cout << fixed << setprecision(3);
    cout << "\n================== 설정 내용 ==================\n" << endl;
    cout << "● 블록 당 레코드 수: " << commas(block_record_num) << " 개" << endl;
    cout << "● 한 블록 당 평균 크기: " << (block_size / 1000000.0) << " MB" << endl;
    cout << "● 총 블록 수: " << block_num << " 개\n" << endl;

    cout << "● 버퍼 내 레코드 수: " << commas(buffer_record_num) << " 개" << endl;
    cout << "● 파일 쓰기 횟수: " << buffer_output_num << " 회" << endl;
    cout << "● 버퍼의 크기: " << (buffer_size / 1000000.0) << " MB" << endl;
    cout << "\n================== 최종 결과 ==================\n" << endl;

    printResult("읽기", loadTime, getLoadCPUUsage(), getLoadMemoryUsage());
    printResult("조인", joinTime, getJoinCPUUsage(), getJoinMemoryUsage());
    printResult("출력", outputTime, getOutputCPUUsage(), getOutputMemoryUsage());
    printAvgResult();

    cout << "● 전체 실제 동작 시간: " << totalTime <<  " 초" << endl;
    cout << "\n===============================================" << endl;
    cout << "...데이터 처리 완료. \n결과가 \"output.csv\"에 저장되었습니다." << endl;
}

size_t PerformanceTester::getFileSize(ifstream &fileStream) {
    // 현재 위치를 저장
    streampos currentPos = fileStream.tellg();

    // 파일 끝으로 이동 후 위치 저장
    fileStream.seekg(0, ios::end);
    size_t fileSize = fileStream.tellg();

    // 원래 위치로 복원
    fileStream.seekg(currentPos, ios::beg);

    return fileSize;
}