#include "join.h"
#include "table.h"
#include "partsupp_reader.h"
#include "performance_tester.h"
#include "file_writer.h"

#include <sys/time.h> // 시간 측정을 위한 헤더 추가
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>

using namespace std;
using namespace chrono;

// 블록 중첩 조인 함수
void blockNestedLoopJoin(ifstream &partsuppStream, const Table supplierTable, int blockRecordNum, FileWriter &fileWriter, PerformanceTester &performanceTester) {
    performanceTester.setJoinCpuBeforeUsage();      // join 작업 전 CPU 사용량 측정
    performanceTester.setJoinMemoryBeforeUsage();   // join 작업 전 메모리 사용량 측정

    vector<Table> tables = {supplierTable};
    int blockNum = 0;
    double totalBlockSize = 0;

    while (!partsuppStream.eof()) {
        vector<Partsupp> partsupps = readPartsuppBlock(partsuppStream, blockRecordNum);
        if (partsupps.empty()) break;
        join(partsupps, tables, fileWriter, totalBlockSize, performanceTester);
        performanceTester.addBlockNum();
    }
    performanceTester.setBufferOutputNum(fileWriter.getOutputNum());
    performanceTester.setBlockSize(totalBlockSize);
    performanceTester.setJoinCpuAfterUsage();       // join 작업 후 CPU 사용량 측정
    performanceTester.setJoinMemoryAfterUsage();    // join 작업 후 메모리 사용량 측정
    performanceTester.addOutputTime(fileWriter.getOutputTime());
}

void join(const vector<Partsupp> &partsupps, const vector<Table> &tables, FileWriter &fileWriter, double &blockSize, PerformanceTester &performanceTester) {
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