#include "tablerepository.h"

TableRepository::TableRepository(PostgresDatabase& database) : db(database) {}

void TableRepository::create_table(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& columns) {
    std::string query = "CREATE TABLE " + table_name + " (";
    for (const auto& column : columns) {
        query += column.first + " " + column.second + ", ";
    }
    query = query.substr(0, query.size() - 2) + ");";
    db.execute_query(query);
}

void TableRepository::drop_table(const std::string& table_name) {
    std::string query = "DROP TABLE IF EXISTS " + table_name + ";";
    db.execute_query(query);
}

void TableRepository::add_entry(const std::string& table_name, const std::vector<std::pair<std::string, std::string>>& entry) {
    std::string query = "INSERT INTO " + table_name + " (";
    std::string values = "VALUES (";
    for (const auto& [key, value] : entry) {
        query += key + ", ";
        values += "'" + value + "', ";
    }
    query = query.substr(0, query.size() - 2) + ") " + values.substr(0, values.size() - 2) + ");";
    db.execute_query(query);
}


void TableRepository::remove_entry(const std::string& table_name, int id) {
    std::string query = "DELETE FROM " + table_name + " WHERE id = " + std::to_string(id) + ";";
    db.execute_query(query);
}

void TableRepository::update_entry(const std::string& table_name, int id, const std::pair<std::string, std::string>& column_value) {
    std::string query = "UPDATE " + table_name + " SET " + column_value.first + " = '" + column_value.second + "' WHERE id = " + std::to_string(id) + ";";
    db.execute_query(query);
}

std::vector<std::string> TableRepository::show_all_entries(const std::string& table_name) {
    std::vector<std::string> entries;
    auto column_names = get_column_names(table_name);
    if (column_names.empty()) {
        std::cerr << "Error: Failed to retrieve columns for table " << table_name << "." << std::endl;
        return entries;
    }
    std::string header;
    for (const auto& col : column_names) {
        header += col + "\t";
    }
    entries.push_back(header);
    std::string query = "SELECT * FROM " + table_name + ";";
    auto result = db.execute_query(query);
    if (result.empty()) {
        std::cerr << "Error: Query returned no results or failed." << std::endl;
        return entries;
    }
    for (const auto& row : result) {
        std::string entry;
        for (const auto& field : row) {
            entry += field.as<std::string>() + "\t";
        }
        entries.push_back(entry);
    }
    return entries;
}


std::vector<std::string> TableRepository::find_entries(const std::string& table_name, const std::string& condition) {
    std::string query = "SELECT * FROM " + table_name + " WHERE " + condition + ";";
    auto result = db.execute_query(query);

    std::vector<std::string> entries;
    for (const auto& row : result) {
        std::string entry;
        for (const auto& field : row) {
            entry += field.as<std::string>() + " ";
        }
        entries.push_back(entry);
    }
    return entries;
}

std::vector<std::string> TableRepository::list_tables() {
    std::vector<std::string> tables; 
    try {
        std::string query = "SELECT table_name FROM information_schema.tables WHERE table_schema='public';";
        auto result = db.execute_query(query);
        for (const auto& row : result) {
            tables.push_back(row["table_name"].as<std::string>());
        }
    } catch (const std::exception& e) {
        std::cerr << "Error retrieving tables: " << e.what() << std::endl;
    }
    return tables;
}

std::vector<std::string> TableRepository::get_column_names(const std::string& table_name) {
    std::vector<std::string> column_names;
    std::string query = "SELECT column_name FROM information_schema.columns WHERE table_name = '" + table_name + "';";
    
    auto result = db.execute_query(query);
    for (const auto& row : result) {
        column_names.push_back(row[0].as<std::string>());
    }
    return column_names;
}

