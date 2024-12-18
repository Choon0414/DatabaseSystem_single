#ifndef SUPPLIER_READER_H
#define SUPPLIER_READER_H

#include <vector>
#include <string>
#include <fstream>

struct Supplier {
    int suppKey;         
    std::string name;   
    std::string address;
    int nationKey;        
    std::string phone;
    double acctBal;      
    std::string comment;
};

Supplier parseSupplierRecord(const std::string &line);
std::vector<Supplier> readSupplierTable(std::ifstream &file);

#endif // SUPPLIER_READER_H
