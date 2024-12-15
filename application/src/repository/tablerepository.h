#ifndef TABLEREPOSITORY_H
#define TABLEREPOSITORY_H

#include <vector>
#include <string>
#include <utility>
#include "../database/postgre.h"    
#include "../template/templatetable.h"

class TableRepository {
public:
    explicit TableRepository(PostgresDatabase& database);
    void create_table(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns);
    void drop_table(const std::string& table_name);
    void add_entry(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& entry);
    void remove_entry(const std::string& table_name, int id);
    void update_entry(const std::string& table_name, int id, const std::pair<std::string, std::string>& column_value);
    void add_relation(const std::string& table_name, const std::string& parent_table, const std::string& column_name);
    void start_transaction();
    void end_transaction();
    void rollback_transaction();
    void set_isolation_level(const std::string& level);
    std::vector<std::string> show_all_entries(const std::string& table_name);
    std::vector<std::string> find_entries(const std::string& table_name, const std::string& condition);
    std::vector<std::string> list_tables();
    std::vector<std::string> get_column_names(const std::string& table_name);
    std::vector<std::unordered_map<std::string, std::string>> get_all_entries(const std::string& table_name);
private:
    PostgresDatabase& db;
};

#endif 
