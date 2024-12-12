#ifndef TEMPLATETABLE_H
#define TEMPLATETABLE_H

#include <vector>
#include <iostream>
#include <iterator>

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
    class CustomIterator {
    private:
        typename std::vector<T>::iterator current;
    public:
        CustomIterator(typename std::vector<T>::iterator start) : current(start) {}
        T& operator*() {
            return *current;
        }
        CustomIterator& operator++() {
            ++current;
            return *this;
        }
        bool operator!=(const CustomIterator& other) const {
            return current != other.current;
        }
    };
    CustomIterator begin() {
        return CustomIterator(records.begin());
    }
    CustomIterator end() {
        return CustomIterator(records.end());
    }
};
#endif