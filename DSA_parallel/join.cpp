#include "join.h"
#include "table.h"
#include "partsupp_reader.h"
#include "performance_tester.h"
#include "file_writer.h"

#include <sys/time.h> // 시간 측정을 위한 헤더 추가
#include <fstream>
#include <vector>
#include <future>
#include <mutex> // 추가: mutex 사용
#include <thread>
#include <functional>
#include <iostream>
#include <iomanip>
#include <queue>
#include <condition_variable>

using namespace std;
using namespace chrono;

// 블록 중첩 조인 함수
void blockNestedLoopJoin(ifstream &partsuppStream, const Table supplierTable, int blockSize, FileWriter &fileWriter, PerformanceTester &performanceTester) {
    performanceTester.setJoinCpuBeforeUsage();      // join 작업 전 CPU 사용량 측정
    performanceTester.setJoinMemoryBeforeUsage();   // join 작업 전 메모리 사용량 측정

    vector<Table> tables = {supplierTable};
    int blockNum = 0;
    double totalBlockSize = 0;

    mutex fileWriterMutex;
    queue<function<void()>> tasks;
    mutex taskMutex;
    condition_variable cv;
    bool stop = false;

    // 스레드 풀 생성
    vector<thread> threadPool;
    for (int i = 0; i < 6; ++i) {
        threadPool.emplace_back([&]() {
            while (true) {
                function<void()> task;
                {
                    unique_lock<mutex> lock(taskMutex);
                    cv.wait(lock, [&]() { return stop || !tasks.empty(); });
                    if (stop && tasks.empty()) return;
                    task = move(tasks.front());
                    tasks.pop();
                }
                task();
            }
        });
    }

    // 작업 분배
    while (!partsuppStream.eof()) {
        vector<Partsupp> partsupps = readPartsuppBlock(partsuppStream, blockSize);

        if (partsupps.empty()) break;

        {
            lock_guard<mutex> lock(taskMutex);
            tasks.emplace([partsupps, supplierTable, &fileWriter, &totalBlockSize, &fileWriterMutex, &performanceTester]() {
                join(partsupps, {supplierTable}, fileWriter, totalBlockSize, performanceTester, fileWriterMutex);
            });
        }
        cv.notify_one();
        performanceTester.addBlockNum();
    }

    // 작업 종료 알림
    {
        lock_guard<mutex> lock(taskMutex);
        stop = true;
    }
    cv.notify_all();

    // 스레드 종료
    for (auto &t : threadPool) {
        t.join();
    }

    performanceTester.setBufferOutputNum(fileWriter.getOutputNum());
    performanceTester.setBlockSize(totalBlockSize);
    performanceTester.setJoinCpuAfterUsage();       // join 작업 후 CPU 사용량 측정
    performanceTester.setJoinMemoryAfterUsage();    // join 작업 후 메모리 사용량 측정
    performanceTester.addOutputTime(fileWriter.getOutputTime());
}

void join(const vector<Partsupp> &partsupps, const vector<Table> &tables, FileWriter &fileWriter, double &blockSize, PerformanceTester &performanceTester, mutex &fileWriterMutex) {
    for (const auto &partsupp : partsupps) {
        for (const auto &table : tables) {
            try {
                auto joinStart = high_resolution_clock::now();
                
                TableRecord record = table.getRecord(partsupp.suppKey);
                // 컬럼: partsuppKey | suppKey | avalyqty | supplyCost | supp_name | supp_address | supp_nationKey | supp_phone | supp_acctBal 
                string result = to_string(partsupp.partsuppKey) + " | " +
                                to_string(partsupp.suppKey) + " | " +
                                partsupp.avalyqty + " | " +
                                to_string(partsupp.supplyCost) + " | " +
                                record.fields[0] + " | " +
                                record.fields[1] + " | " +
                                record.fields[2] + " | " +
                                record.fields[3] + " | " +
                                record.fields[4];
                lock_guard<mutex> lock(fileWriterMutex);
                
                auto joinEnd = high_resolution_clock::now();
                performanceTester.addJoinTime(duration_cast<duration<double>>(joinEnd - joinStart).count());
                performanceTester.addOutputTime(fileWriter.addResult(result, performanceTester));
                blockSize += result.capacity();
            } catch (const out_of_range &e) {
                cerr << "Record not found for key: " << partsupp.suppKey << endl;
            }
        }
    }
}