#ifndef TABLESERVICE_H
#define TABLESERVICE_H

#include <vector>
#include <string>
#include "../repository/tablerepository.h"
#include <nlohmann/json.hpp>
#include "../template/templatetable.h"

class TableService {
public:
    explicit TableService(TableRepository& repository);
    void create_table(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns);
    void drop_table(const std::string& table_name);
    void add_entry(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& entry);
    void remove_entry(const std::string& table_name, int id);
    void update_entry(const std::string& table_name, int id, const std::pair<std::string, std::string>& column_value);
    void list_tables();
    TemplateTable<std::string> show_all_entries(const std::string& table_name);
    std::vector<std::string> find_entries(const std::string& table_name, const std::string& condition);
    nlohmann::json export_table_to_json(const std::string& table_name);

private:
    TableRepository& repo;
    void save_json_to_file(const nlohmann::json& json_data, const std::string& table_name);
};

#endif
