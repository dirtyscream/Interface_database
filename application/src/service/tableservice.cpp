#include "tableservice.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <regex>

namespace fs = std::filesystem;

std::vector<Table> parse_dump(const std::string& dumpFilePath) {
    std::ifstream file(dumpFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open the dump file: " + dumpFilePath);
    }
    std::vector<Table> tables;
    std::string line;
    Table currentTable;
    std::regex tableRegex(R"(CREATE TABLE (\w+)\.(\w+))");
    std::regex columnRegex(R"(^\s*(\w+)\s+(\w+))");
    std::regex primaryKeyRegex(R"(CONSTRAINT \w+ PRIMARY KEY \(([^)]+)\))");
    std::regex foreignKeyRegex(R"(FOREIGN KEY \(([^)]+)\) REFERENCES (\w+)\.(\w+))");

    while (std::getline(file, line)) {
        std::smatch match;
        if (line.find("ALTER") != std::string::npos || 
            line.find("CREATE SEQUENCE") != std::string::npos || 
            line.find("DEFAULT PRIVILEGES") != std::string::npos ||
            line.find("NO") != std::string::npos || 
            line.find("CACHE") != std::string::npos || 
            line.find("AS") != std::string::npos || 
            line.find("START") != std::string::npos || 
            line.find("INCREMENT") != std::string::npos || 
            line.find("ADD") != std::string::npos) {
            continue;
        }
        if (std::regex_search(line, match, tableRegex)) {
            if (!currentTable.name.empty()) {
                tables.push_back(currentTable);
                currentTable = Table{};
            }
            currentTable.name = match[2];  
        } 
        else if (currentTable.name.empty()) {
            continue;
        } 
        else if (std::regex_search(line, match, columnRegex)) {
            Column col;
            col.name = match[1];
            col.type = match[2];  
            currentTable.columns.push_back(col);
        } 
        else if (std::regex_search(line, match, primaryKeyRegex)) {
            std::stringstream pkStream(match[1]);
            std::string pk;
            while (std::getline(pkStream, pk, ',')) {
                pk.erase(pk.find_last_not_of(" \t\n\r\f\v") + 1); 
                pk.erase(0, pk.find_first_not_of(" \t\n\r\f\v")); 
                currentTable.primary_keys.push_back(pk);
            }
        } 
        else if (std::regex_search(line, match, foreignKeyRegex)) {
            std::string fkColumn = match[1];    
            std::string referencedTable = match[2]; 
            currentTable.foreign_keys.emplace_back(fkColumn, referencedTable);
        }
    }
    if (!currentTable.name.empty()) {
        tables.push_back(currentTable);
    }

    file.close();
    return tables;
}


void TableService::convert_to_diagram() {
    try {
        std::string output_file;  
        if (!converter.dump_database(output_file)) {
            throw std::runtime_error("Error while exporting database");
        }
        if (output_file.empty()) {
            throw std::runtime_error("Schemas file was not generated.");
        }
        std::filesystem::path output_path = output_file;
        if (!std::filesystem::exists(output_path)) {
            throw std::runtime_error("File of shema is not exists.");
        }
        std::vector<Table> tables = parse_dump(output_path);
        if (std::filesystem::file_size(output_path) == 0) {
            throw std::runtime_error("Generated file is empty.");
        }
        std::cout << "Generated database schema file was succesfully imported into file: " << output_file << std::endl;
        std::string uml_file = output_file + ".uml";
        converter.generate_uml(tables, uml_file);
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << std::endl;
    }
}


TableService::TableService(TableRepository& repository, DatabaseConverter& converter)
    : repo(repository), converter(converter) {
}

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

bool TableService::add_entry(const std::string& table_name, const std::string& input_line) {
    if (table_name.empty() || input_line.empty()) {
        std::cerr << "Error: Invalid table name or input data." << std::endl;
        return false;
    }
    std::vector<std::pair<std::string, std::string>> entry;
    std::istringstream line_stream(input_line);
    std::string token;
    std::string current_column;
    std::string current_value;
    while (std::getline(line_stream, token, ' ')) {
        size_t pos = token.find('=');
        if (pos != std::string::npos) {
            if (!current_column.empty()) {
                entry.emplace_back(current_column, current_value);
                current_column.clear();
                current_value.clear();
            }

            current_column = token.substr(0, pos);
            current_value = token.substr(pos + 1);
        } else if (!current_column.empty()) {
            current_value += " " + token;
        }
    }
    if (!current_column.empty()) {
        entry.emplace_back(current_column, current_value);
    }

    if (entry.empty()) {
        std::cerr << "Error: No valid column=value pairs found." << std::endl;
        return false;
    }
    repo.add_entry(table_name, entry);
    return true;
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
void TableService::print_entries(const std::vector<std::string>& entries, const std::string& current_table) {
    if (entries.empty()) {
        std::cout << "No entries found or table does not exist.\n";
        return;
    }
    auto column_names = repo.get_column_names(current_table);
    if (column_names.empty()) {
        std::cerr << "Error: Failed to retrieve column names for table " << current_table << ".\n";
        return;
    }
    std::cout << "\nEntries in table: " << current_table << "\n";
    for (const auto& column_name : column_names) {
        std::cout << "| " << std::setw(15) << std::left << column_name << " ";
    }
    std::cout << "|\n" << std::string(column_names.size() * 18, '-') << '\n';
    for (const auto& entry : entries) {
        std::istringstream entry_stream(entry);
        std::string value;
        for (size_t i = 0; i < column_names.size(); ++i) {
            if (std::getline(entry_stream, value, '\t')) {
                if (value.front() == '"' && value.back() == '"') {
                    value = value.substr(1, value.length() - 2);
                }
                std::cout << "| " << std::setw(15) << std::left << value << " ";
            } else {
                std::cout << "| " << std::setw(15) << std::left << "NULL" << " ";
            }
        }
        std::cout << "|\n";
    }
    std::cout << std::string(column_names.size() * 18, '-') << '\n';
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

void TableService::add_relation(const std::string& table_name, const std::string& column_name) {
    auto pos = column_name.find("_id");
    if (pos == std::string::npos) {
        std::cerr << "Error: Invalid relation column name. It must end with '_id'.\n";
        return;
    }
    std::string parent_table = column_name.substr(0, pos);
    repo.add_relation(table_name, parent_table, column_name);
}

std::vector<std::string> TableService::get_column_names(const std::string& table_name) {
    if (table_name.empty()) {
        std::cerr << "Error: Invalid table name." << std::endl;
        return {};
    }
    return repo.get_column_names(table_name);
}


void TableService::start_transaction() {
    repo.start_transaction();
}

void TableService::end_transaction() {
    repo.end_transaction();
}

void TableService::rollback_transaction() {
    repo.rollback_transaction();
}

void TableService::change_isolation_level(const std::string& level) {
    static const std::vector<std::string> valid_levels = {
        "READ UNCOMMITTED", "READ COMMITTED", "REPEATABLE READ", "SERIALIZABLE"
    };

    if (std::find(valid_levels.begin(), valid_levels.end(), level) == valid_levels.end()) {
        throw std::invalid_argument("Invalid isolation level: " + level);
    }
    repo.set_isolation_level(level);
}