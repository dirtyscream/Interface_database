#ifndef TEMPLATETABLE_H
#define TEMPLATETABLE_H

#include <vector>
#include <iostream>
template <typename T>
class TemplateTable {
private:
    std::vector<T> records;

public:
    void add_record(const T& record) {
        records.push_back(record);
    }

    void show_records() const {
        for (const auto& record : records) {
            std::cout << record << std::endl;
        }
    }

    size_t size() const {
        return records.size();
    }
};

#endif
