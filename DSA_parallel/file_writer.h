#ifndef FILE_WRITER_H
#define FILE_WRITER_H

#include <vector>
#include <string>
#include <fstream>
#include <chrono>
#include "performance_tester.h"

using namespace std;

class FileWriter {
private:
    ofstream outputStream;
    vector<string> buffer;
    size_t bufferSize;
    double outputTime;
    int outputNum = 0;

public:
    FileWriter(const string &fileName, size_t bufferSize, bool appendMode = true);
    ~FileWriter();
    double addResult(const string &result, PerformanceTester &performanceTester);
    const vector<string>& getBuffer() const;
    double flushBuffer();
    double getOutputTime() const{ return outputTime; };
    int getOutputNum() const{ return outputNum; };
};

#endif // FILE_WRITER_H