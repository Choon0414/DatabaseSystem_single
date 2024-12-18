#include "table.h"
#include <stdexcept> // 예외 처리

Table::Table(const std::string &name) : tableName(name) {}

void Table::addRecord(int key, const std::vector<std::string> &fields) {
    TableRecord record{key, fields};
    records[key] = record;
}

TableRecord Table::getRecord(int key) const {
    auto it = records.find(key);
    if (it != records.end()) {
        return it->second;
    }
    throw std::out_of_range("Record not found for key: " + std::to_string(key));
}