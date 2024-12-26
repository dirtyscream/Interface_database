#include "converter.h"
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <regex>
#include <iostream>
#include <filesystem>
#include <ctime>

DatabaseConverter::DatabaseConverter(const std::string& connection_string)
    : connection_string(connection_string) {}

bool DatabaseConverter::parse_connection_string(const std::string& conn_str, std::string& host, std::string& port,
                                           std::string& dbname, std::string& user, std::string& password) {
    std::regex conn_pattern(R"(host=([^ ]+) (?:port=([^ ]+) )?dbname=([^ ]+) user=([^ ]+) password=([^ ]+))");
    std::smatch match;
    if (std::regex_search(conn_str, match, conn_pattern)) {
        host = match[1].str();
        port = match[2].matched ? match[2].str() : "";
        dbname = match[3].str();
        user = match[4].str();
        password = match[5].str();
        return true;
    }
    return false;
}

bool DatabaseConverter::dump_database(std::string& output_file) {
    std::string host, port, dbname, user, password;
    if (!parse_connection_string(connection_string, host, port, dbname, user, password)) {
        std::cerr << "Error while connecting." << std::endl;
        return false;
    }

    if (output_file.empty()) {
        std::time_t now = std::time(nullptr);
        std::tm* tm_info = std::localtime(&now);
        std::ostringstream filename_stream;
        filename_stream << "db_schema_"
                        << (tm_info->tm_year + 1900) << "_"
                        << (tm_info->tm_mon + 1) << "_"
                        << tm_info->tm_mday << "_"
                        << tm_info->tm_hour << "_"
                        << tm_info->tm_min << "_"
                        << tm_info->tm_sec
                        << ".sql";
        output_file = filename_stream.str();
    }

    std::string command = "PGPASSWORD='" + password + "' pg_dump --no-password --file=" + output_file +
                          " --schema-only --host=" + host + " --port=" + port + " --dbname=" + dbname +
                          " --username=" + user;
    int result = system(command.c_str());

    if (result != 0) {
        std::cerr << "Error while exporting database schema." << std::endl;
        return false;
    }

    std::cout << "Exporting database schema wass succesfully. File: " << output_file << std::endl;
    return true;
}

std::string DatabaseConverter::get_database_name() const {
    std::regex dbname_pattern(R"(dbname=([\w\d_]+))");
    std::smatch match;
    if (std::regex_search(connection_string, match, dbname_pattern)) {
        return match[1];
    }
    return "";
}

void DatabaseConverter::generate_uml(const std::vector<Table>& tables, const std::string& outputFilePath) {
    std::ofstream file(outputFilePath);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open the output file: " + outputFilePath);
    }

    file << "@startuml\n";
    for (const auto& table : tables) {
        file << "class " << table.name << " {\n";

        for (const auto& column : table.columns) {
            file << "  + " << column.name << " : " << column.type << "\n";
        }

        if (!table.primary_keys.empty()) {
            file << "  -- Primary Keys --\n";
            for (const auto& pk : table.primary_keys) {
                file << "  + " << pk << "\n";
            }
        }

        file << "}\n";
    }
    for (const auto& table : tables) {
        for (const auto& column : table.columns) {
            if (column.name.size() > 3 && column.name.substr(column.name.size() - 3) == "_id") {
                std::string referencedTable = column.name.substr(0, column.name.size() - 3);
                auto it = std::find_if(tables.begin(), tables.end(), [&referencedTable](const Table& t) {
                    return t.name == referencedTable;
                });

                if (it != tables.end()) {
                    file << table.name << " --> " << referencedTable << " : " << column.name << "\n";
                }
            }
        }
    }
    file << "@enduml\n";
    file.close();
}





