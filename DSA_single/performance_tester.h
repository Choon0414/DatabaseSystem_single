#ifndef PERFORMANCE_TESTER_H
#define PERFORMANCE_TESTER_H

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <chrono>

using namespace std;
using namespace std::chrono;

struct CpuStats {
    long user, nice, system, idle, iowait, irq, softirq, steal;
};

class PerformanceTester {
private:
    // 공통 변수들
    int block_size = 0, block_record_num = 0, block_num = 0;
    int buffer_size = 0, buffer_record_num = 0, buffer_output_num = 0;

    // 읽기, 조인, 출력 시간 및 저장용 변수들
    double loadTime = 0, loadCpuUsage = 0, loadMemoryUsage = 0;
    double loadMemoryBeforeUsage = 0, loadMemoryAfterUsage = 0; CpuStats loadCpuBeforeUsage, loadCpuAfterUsage;
    double joinTime = 0, joinCpuUsage = 0, joinMemoryUsage = 0; 
    double joinMemoryBeforeUsage = 0, joinMemoryAfterUsage = 0; CpuStats joinCpuBeforeUsage, joinCpuAfterUsage;
    double outputTime = 0, outputCpuUsage = 0, outputMemoryUsage = 0;
    double outputMemoryBeforeUsage = 0, outputMemoryAfterUsage = 0; CpuStats outputCpuBeforeUsage, outputCpuAfterUsage;
    double totalTime = 0, avgMemory = 0, avgCpuUsage = 0, maxMemory = 80;

    string formatDouble(double value, int precision) const;
    string commas(int number);

public:
    PerformanceTester() {}
    ~PerformanceTester() {}

    // block과 buffer 사이즈 관련 Setter, Getter
    void addBlockNum() { block_num++; }
    int setBlockRecordNum(int num) { block_record_num += num; return block_record_num;}
    int setBufferRecordNum(int num) { buffer_record_num += num; return buffer_record_num; }
    void setBlockSize(double size) { block_size = size / block_num; }
    void setBufferSize() { buffer_size = (block_size / block_record_num) * buffer_record_num; }
    int setBufferOutputNum(int num) { buffer_output_num = num; return buffer_output_num; }

    // CPU 사용량 측정 관련 Setter, Getter
    CpuStats getCpuStats();
    double calculateCpuUsage(const CpuStats& prevStats, const CpuStats& currStats);
    void setLoadCpuBeforeUsage() { loadCpuBeforeUsage = getCpuStats(); }
    void setLoadCpuAfterUsage() { loadCpuAfterUsage = getCpuStats(); }
    void setJoinCpuBeforeUsage() { joinCpuBeforeUsage = getCpuStats(); }
    void setJoinCpuAfterUsage() { joinCpuAfterUsage = getCpuStats(); }
    void setOutputCPUBeforeUsage() { outputCpuBeforeUsage = getCpuStats(); }
    void setOutputCPUAfterUsage() { outputCpuAfterUsage = getCpuStats(); }

    double getLoadCPUUsage() { loadCpuUsage = calculateCpuUsage(loadCpuBeforeUsage, loadCpuAfterUsage); return loadCpuUsage; }
    double getJoinCPUUsage() { joinCpuUsage = calculateCpuUsage(joinCpuBeforeUsage, joinCpuAfterUsage); return joinCpuUsage;}
    double getOutputCPUUsage() { outputCpuUsage = calculateCpuUsage(outputCpuBeforeUsage, outputCpuAfterUsage); return outputCpuUsage;}

    // 메모리 사용량 측정 관련 Setter, Getter
    void setMaxMemory();
    double setMemoryUsage();
    void setLoadMemoryBeforeUsage() { loadMemoryBeforeUsage = setMemoryUsage(); }
    void setLoadMemoryAfterUsage() { loadMemoryAfterUsage = setMemoryUsage(); }
    void setJoinMemoryBeforeUsage() { joinMemoryBeforeUsage = setMemoryUsage(); }
    void setJoinMemoryAfterUsage() { joinMemoryAfterUsage = setMemoryUsage(); }
    double setOutputMemoryBeforeUsage() { outputMemoryBeforeUsage = setMemoryUsage(); return outputMemoryBeforeUsage; }
    double setOutputMemoryAfterUsage() { outputMemoryAfterUsage = setMemoryUsage(); return outputMemoryAfterUsage; }

    double getOutputMemoryUsage() { loadMemoryUsage = outputMemoryAfterUsage - outputMemoryBeforeUsage; return loadMemoryUsage; }
    double getJoinMemoryUsage() { joinMemoryUsage = joinMemoryAfterUsage - joinMemoryBeforeUsage; return joinMemoryUsage; }
    double getLoadMemoryUsage() { outputMemoryUsage = loadMemoryAfterUsage - loadMemoryBeforeUsage; return outputMemoryUsage; }

    // 시간 측정 관련 Setter, Getter
    void setTotalTime(double time) { totalTime += time; }
    void addLoadTime(double time) { loadTime = time; }
    void addJoinTime(double time) { joinTime += time; }
    void addOutputTime(double time) { outputTime += time; }

    double getJoinTime() const { return joinTime; }
    double getTotalTime() const { return totalTime; }

    // 출력 관련 함수
    void printResult(string workname, double time, double cpuUsage, double memoryUsage) const;
    void printAvgResult() const;
    void printStatus();
    size_t getFileSize(ifstream &fileStream); 
};

#endif // PERFORMANCE_TESTER_H