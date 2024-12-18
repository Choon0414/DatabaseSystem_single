#ifndef PARTSUPP_READER_H
#define PARTSUPP_READER_H

#include <vector>
#include <string>
#include <fstream>

struct Partsupp {
    int partsuppKey;
    int suppKey;
    std::string avalyqty;
    int supplyCost;
    std::string comment;
};

Partsupp parsePartsuppRecord(const std::string &line);
std::vector<Partsupp> readPartsuppBlock(std::ifstream &file, int blockSize);

#endif // NATION_READER_H
