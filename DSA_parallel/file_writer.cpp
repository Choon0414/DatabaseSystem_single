#include "file_writer.h"
#include "performance_tester.h"
#include <stdexcept>
#include <iostream>

using namespace std;

FileWriter::FileWriter(const string &fileName, size_t bufferSize, bool appoutputEndMode)
    : bufferSize(bufferSize) {
    if (appoutputEndMode) {
        outputStream.open(fileName, ios::app); // 추가 모드로 파일 열기
    } else {
        outputStream.open(fileName, ios::trunc); // 기존 내용 덮어쓰기
    }

    if (!outputStream.is_open()) {
        throw runtime_error("Error: Could not open output file: " + fileName);
    }

    buffer.reserve(bufferSize);
}

FileWriter::~FileWriter() {
    flushBuffer();
    outputStream.close();
}

double FileWriter::addResult(const string &result, PerformanceTester &performanceTester) {
    buffer.push_back(result);
    if (buffer.size() >= bufferSize) {
        performanceTester.setOutputCPUBeforeUsage();
        performanceTester.setOutputMemoryBeforeUsage();
        double outputTime = flushBuffer();
        performanceTester.setOutputCPUAfterUsage();
        performanceTester.setOutputMemoryAfterUsage();
        return outputTime;

    }
    return 0;
}

const vector<string>& FileWriter::getBuffer() const {
    return buffer;
};

double FileWriter::flushBuffer() {
    auto outputStart = high_resolution_clock::now();
    for (const auto &line : buffer) {
        outputStream << line << "\n";
    }
    buffer.clear();
    auto outputEnd = high_resolution_clock::now();
    outputNum++;
    return duration_cast<duration<double>>(outputEnd - outputStart).count();
}
