#include "partsupp_reader.h"
#include <sstream>

Partsupp parsePartsuppRecord(const std::string &line) {
    Partsupp record;
    std::istringstream stream(line);
    std::string field;

    getline(stream, field, '|');
    record.partsuppKey = stoi(field);
    getline(stream, field, '|');
    record.suppKey = stoi(field);
    getline(stream, record.avalyqty, '|');
    getline(stream, field, '|');
    record.supplyCost = stoi(field);
    getline(stream, record.comment, '|');

    return record;
}

std::vector<Partsupp> readPartsuppBlock(std::ifstream &file, int blockSize) {
    std::vector<Partsupp> block;
    std::string line;
    int count = 0;

    while (count < blockSize && getline(file, line)) {
        block.push_back(parsePartsuppRecord(line));
        count++;
    }

    return block;
}
