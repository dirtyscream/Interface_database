#ifndef TABLE_H
#define TABLE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <utility> 

struct Column {
    std::string name;
    std::string type;
};

struct Table {
    std::string name;
    std::vector<Column> columns;
    std::vector<std::string> primary_keys;
    std::vector<std::pair<std::string, std::string>> foreign_keys;
};

#endif
