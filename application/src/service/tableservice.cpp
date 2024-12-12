#include "tableservice.h"
#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

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

TemplateTable<std::string> TableService::show_all_entries(const std::string& table_name) {
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

nlohmann::json TableService::export_table_to_json(const std::string& table_name) {
    auto entries = repo.get_all_entries(table_name);
    nlohmann::json j_array = nlohmann::json::array(); 
    for (const auto& entry : entries) {
        nlohmann::json j_row;  
        for (const auto& [key, value] : entry) {
            j_row[key] = value; 
        }
        j_array.push_back(j_row); 
    }
    save_json_to_file(j_array, table_name); 
    return j_array; 
}

void TableService::save_json_to_file(const nlohmann::json& json_data, const std::string& table_name) {
    fs::path json_directory = fs::current_path().parent_path() / "json";
    if (!fs::exists(json_directory)) {
        fs::create_directory(json_directory);
    }
    std::string filename = (json_directory / (table_name + ".json")).string();
    std::ofstream file(filename);
    if (file.is_open()) {
        file << json_data.dump(4);
        file.close(); 
        std::cout << "Saved successfully: " << filename << std::endl;
    } else {
        std::cerr << "Error while saving: " << filename << std::endl;
    }
}