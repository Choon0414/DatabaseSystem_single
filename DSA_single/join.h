#ifndef JOIN_ALGORITHM_H
#define JOIN_ALGORITHM_H

#include <fstream>
#include <vector>
#include <sys/time.h> // 시간 측정을 위한 헤더 추가
#include "table.h"
#include "partsupp_reader.h"
#include "performance_tester.h"
#include "file_writer.h"
#include <chrono> // 시간 측정

using namespace std;
using namespace chrono;

void blockNestedLoopJoin(ifstream &partsuppStream, const Table supplierTable, int blockSize, FileWriter &fileWriter, PerformanceTester &performanceTester);
void join(const vector<Partsupp> &partsupps, const vector<Table> &tables, FileWriter &fileWriter, double &blockSize, PerformanceTester &performanceTester);

#endif // JOIN_ALGORITHM_H
