#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>
#include <unordered_map>

struct TableRecord {
    int key;
    std::vector<std::string> fields;
};

class Table {
private:
    std::string tableName;
    std::unordered_map<int, TableRecord> records;

public:
    Table(const std::string &name);
    void addRecord(int key, const std::vector<std::string> &fields);
    TableRecord getRecord(int key) const;
    std::string getTableName() const;
};

#endif // TABLE_H
