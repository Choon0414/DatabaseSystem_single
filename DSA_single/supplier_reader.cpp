#include "supplier_reader.h"
#include <sstream>

Supplier parseSupplierRecord(const std::string &line) {
    Supplier record;
    std::istringstream stream(line);
    std::string field;

    getline(stream, field, '|');
    record.suppKey = stoi(field);
    getline(stream, record.name, '|');
    getline(stream, record.address, '|');
    getline(stream, field, '|');
    record.nationKey = stoi(field);
    getline(stream, record.phone, '|');
    getline(stream, field, '|');
    record.acctBal = stod(field);
    getline(stream, record.comment, '|');

    return record;
}

std::vector<Supplier> readSupplierTable(std::ifstream &file) {
    std::vector<Supplier> block;
    std::string line;

    while (getline(file, line)) {
        block.push_back(parseSupplierRecord(line));
    }

    return block;
}
