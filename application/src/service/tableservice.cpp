#include "tableservice.h"
#include <iostream>

TableService::TableService(TableRepository& repository) : repo(repository) {}

void TableService::create_table(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns) {
    if (table_name.empty() || columns.empty()) {
        std::cerr << "Error: Invalid table name or columns." << std::endl;
        return;
    }
    repo.create_table(table_name, columns);
}

void TableService::drop_table(const std::string& table_name) {
    if (table_name.empty()) {
        std::cerr << "Error: Invalid table name." << std::endl;
        return;
    }
    repo.drop_table(table_name);
}

void TableService::add_entry(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& entry) {
    if (table_name.empty() || entry.empty()) {
        std::cerr << "Error: Invalid table name or entry." << std::endl;
        return;
    }
    repo.add_entry(table_name, entry);
}

void TableService::remove_entry(const std::string& table_name, int id) {
    if (table_name.empty()) {
        std::cerr << "Error: Invalid table name." << std::endl;
        return;
    }
    repo.remove_entry(table_name, id);
}

void TableService::update_entry(const std::string& table_name, int id, const std::pair<std::string, std::string>& column_value) {
    if (table_name.empty() || column_value.first.empty()) {
        std::cerr << "Error: Invalid table name or column value." << std::endl;
        return;
    }
    repo.update_entry(table_name, id, column_value);
}

std::vector<std::string> TableService::show_all_entries(const std::string& table_name) {
    if (table_name.empty()) {
        std::cerr << "Error: Invalid table name." << std::endl;
        return {};
    }
    return repo.show_all_entries(table_name);
}

std::vector<std::string> TableService::find_entries(const std::string& table_name, const std::string& condition) {
    if (table_name.empty() || condition.empty()) {
        std::cerr << "Error: Invalid table name or condition." << std::endl;
        return {};
    }
    return repo.find_entries(table_name, condition);
}

void TableService::list_tables() {
    std::vector<std::string> tables = repo.list_tables();
    if (tables.empty()) {
        std::cout << "No tables found." << std::endl;
    } else {
        std::cout << "Tables in the database:" << std::endl;
        for (const auto& table : tables) {
            std::cout << "- " << table << std::endl;
        }
    }
}