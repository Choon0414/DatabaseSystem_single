#ifndef JOIN_ALGORITHM_H
#define JOIN_ALGORITHM_H

#include <fstream>
#include <vector>
#include "table.h"
#include "partsupp_reader.h"
#include "performance_tester.h"
#include "file_writer.h"
#include <mutex> // 추가: mutex 사용
#include <chrono> // 시간 측정

using namespace std;

void blockNestedLoopJoin(ifstream &partsuppStream, const Table supplierTable, int blockSize, FileWriter &fileWriter, PerformanceTester &performanceTester);
void join(const vector<Partsupp> &partsupps, const vector<Table> &tables, FileWriter &fileWriter, double &blockSize, PerformanceTester &performanceTester, mutex &fileWriterMutex);

#endif // JOIN_ALGORITHM_H
