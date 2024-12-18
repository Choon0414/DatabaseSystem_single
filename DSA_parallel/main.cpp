#include <iostream>
#include <fstream>
#include <vector>
#include <chrono> // 시간 측정
#include <iomanip> // 소수점 자릿수 설정

#include "supplier_reader.h"
#include "partsupp_reader.h"
#include "file_writer.h"
#include "join.h"
#include "table.h"
#include "performance_tester.h"

using namespace std;
using namespace chrono;

int main() {
    string supplierFile = "../tables/supplier.tbl";
    string partsuppFile = "../tables/partsupp.tbl";
    string outputFile = "output.csv";
    PerformanceTester performanceTester;

    auto start = high_resolution_clock::now();

    performanceTester.setLoadMemoryBeforeUsage();   // load 작업 전 메모리 사용량 측정
    performanceTester.setLoadCpuBeforeUsage();      // load 작업 전 CPU 사용량 측정
    auto loadStart = high_resolution_clock::now();  // load 작업 시작 시간 측정

    ifstream supplierStream(supplierFile);
    if (!supplierStream.is_open()) {
        cerr << "supplier 파일을 불러오는데 실패했습니다." << endl;
        return 1;
    }
    ifstream partsuppStream(partsuppFile);
    if (!partsuppStream.is_open()) {
        cerr << "partsupp 파일을 불러오는데 실패했습니다." << endl;
        return 1;
    }
    
    Table supplierTable("supplier");
    vector<Supplier> suppliers = readSupplierTable(supplierStream);
    for (const auto &supplier : suppliers) {
        supplierTable.addRecord(supplier.suppKey, {supplier.name, supplier.address, to_string(supplier.nationKey), supplier.phone, to_string(supplier.acctBal), supplier.comment});
    }
    performanceTester.setLoadMemoryAfterUsage();    // load 작업 후 메모리 사용량 측정
    performanceTester.setLoadCpuAfterUsage();       // load 작업 후 CPU 사용량 측정
    auto loadEnd = high_resolution_clock::now();    // load 작업 종료 시간 측정

    int bufferRecordNum = performanceTester.setBufferRecordNum(50000);
    int blockRecordNum = performanceTester.setBlockRecordNum(100000);
    FileWriter fileWriter(outputFile, bufferRecordNum);
    blockNestedLoopJoin(partsuppStream, supplierTable, blockRecordNum, fileWriter, performanceTester);

    auto end = high_resolution_clock::now();

    performanceTester.addLoadTime(duration_cast<duration<double>>(loadEnd - loadStart).count());
    performanceTester.setTotalTime(duration_cast<duration<double>>(end - start).count());
    performanceTester.setBufferSize();
    performanceTester.printStatus();

    supplierStream.close();
    partsuppStream.close();
    return 0;
}