#ifndef TABLESERVICE_H
#define TABLESERVICE_H

#include <vector>
#include <string>
#include "../repository/tablerepository.h"

class TableService {
public:
    TableService(TableRepository& repository);
    void create_table(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns);
    void drop_table(const std::string& table_name);
    void add_entry(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& entry);
    void remove_entry(const std::string& table_name, int id);
    void update_entry(const std::string& table_name, int id, const std::pair<std::string, std::string>& column_value);
    void list_tables();
    std::vector<std::string> show_all_entries(const std::string& table_name);
    std::vector<std::string> find_entries(const std::string& table_name, const std::string& condition);

private:
    TableRepository& repo;
};

#endif
