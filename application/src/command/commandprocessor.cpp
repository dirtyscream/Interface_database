#include "commandprocessor.h"
#include <iostream>

CommandProcessor::CommandProcessor(TableService& service) 
    : table_service(service) {

    command_map = {
        {"use", [this](std::istringstream& iss) { handle_use(iss); }},
        {"create", [this](std::istringstream& iss) { handle_create(iss); }},
        {"drop", [this](std::istringstream& iss) { handle_drop(iss); }},
        {"list", [this](const std::istringstream& iss) { handle_list(iss); }},
        {"help", [this](const std::istringstream& iss) { handle_help(iss); }},
        {"add", [this](std::istringstream& iss) { process_add(iss); }},
        {"remove", [this](std::istringstream& iss) { process_remove(iss); }},
        {"update", [this](std::istringstream& iss) { process_update(iss); }},
        {"show", [this](const std::istringstream&) { process_show_all(); }},
        {"find", [this](std::istringstream& iss) { process_find(iss); }}
    };
}

void CommandProcessor::process_drop_table(std::istringstream& iss) {
    std::string table_name;
    iss >> table_name;
    table_service.drop_table(table_name);
}

void CommandProcessor::handle_list(const std::istringstream&) const {
    table_service.list_tables();
}

void CommandProcessor::handle_help(const std::istringstream&) const {
    std::cout << "Available commands:\n";
    std::cout << "Database commands:\n";
    std::cout << "  use <table_name>     - Switch to specific table context.\n";
    std::cout << "  create table <table_name> - Create a new table.\n";
    std::cout << "  drop table <table_name> - Drop a table.\n";
    std::cout << "  list tables          - List all tables.\n";
    std::cout << "Table commands (after using a table):\n";
    std::cout << "  add <column1=value1> <column2=value2> - Add a record.\n";
    std::cout << "  remove <id>       - Remove a record by ID.\n";
    std::cout << "  update <id> <column=value> - Update a record by ID.\n";
    std::cout << "  show all          - Show all records.\n";
    std::cout << "  find <column=value> - Find records matching a condition.\n";
    std::cout << "Navigation:\n";
    std::cout << "  exit               - Exit current context or application.\n";
    std::cout << "  back               - Go back to database context.\n";
    std::cout << "  help               - Show this help message.\n";
}

void CommandProcessor::process_show_all() const {
    std::vector<std::string> entries = table_service.show_all_entries(current_table);

    if (entries.empty()) {
        std::cout << "No entries found or table does not exist.\n";
        return;
    }

    std::cout << "Entries in table " << current_table << ":\n";
    for (const auto& entry : entries) {
        std::cout << entry << '\n';
    }
}

void CommandProcessor::process_command(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;

    auto it = command_map.find(cmd);
    if (it != command_map.end()) {
        it->second(iss);
    } else {
        std::cerr << "Error: Unknown command." << std::endl;
    }
}

void CommandProcessor::handle_use(std::istringstream& iss) {
    iss >> current_table;
    if (!current_table.empty()) {
        std::cout << "Using table: " << current_table << std::endl;
    } else {
        std::cerr << "Error: Table name is required." << std::endl;
    }
}

void CommandProcessor::handle_create(std::istringstream& iss) {
    std::string subcmd;
    iss >> subcmd;

    if (subcmd == "table") {
        process_create_table(iss);
    } else {
        std::cerr << "Error: Unknown subcommand for 'create'." << std::endl;
    }
}

void CommandProcessor::handle_drop(std::istringstream& iss) {
    std::string subcmd;
    iss >> subcmd;

    if (subcmd == "table") {
        process_drop_table(iss);
    } else {
        std::cerr << "Error: Unknown subcommand for 'drop'." << std::endl;
    }
}

void CommandProcessor::process_create_table(std::istringstream& iss) {
    std::string table_name;
    iss >> table_name;

    std::vector<std::pair<std::string, std::string>> columns;
    std::string column_name;
    std::string column_type;

    while (iss >> column_name >> column_type) {
        columns.emplace_back(column_name, column_type);
    }

    if (!columns.empty()) {
        table_service.create_table(table_name, columns);
    } else {
        std::cerr << "Error: No columns provided for table." << std::endl;
    }
}

void CommandProcessor::process_add(std::istringstream& iss) {
    std::vector<std::pair<std::string, std::string>> entry;
    std::string column_value;
    while (iss >> column_value) {
        size_t pos = column_value.find('=');
        if (pos != std::string::npos) {
            std::string column = column_value.substr(0, pos);
            std::string value = column_value.substr(pos + 1);
            entry.emplace_back(column, value);
        }
    }
    
    if (!entry.empty()) {
        table_service.add_entry(current_table, entry);
    } else {
        std::cerr << "Error: No column=value pairs provided for adding entry." << std::endl;
    }
}

void CommandProcessor::process_remove(std::istringstream& iss) {
    int id;
    if (iss >> id) {
        table_service.remove_entry(current_table, id);
    } else {
        std::cerr << "Error: Invalid ID for removal." << std::endl;
    }
}

void CommandProcessor::process_update(std::istringstream& iss) {
    int id;
    std::string column_value;
    if (iss >> id >> column_value) {
        size_t pos = column_value.find('=');
        if (pos != std::string::npos) {
            std::string column = column_value.substr(0, pos);
            std::string value = column_value.substr(pos + 1);
            std::pair<std::string, std::string> column_value_pair(column, value);
            table_service.update_entry(current_table, id, column_value_pair);
        } else {
            std::cerr << "Error: Invalid column=value format for update." << std::endl;
        }
    } else {
        std::cerr << "Error: Invalid ID for update." << std::endl;
    }
}

void CommandProcessor::process_find(std::istringstream& iss) {
    std::string condition;
    if (iss >> condition) {
        table_service.find_entries(current_table, condition);
    } else {
        std::cerr << "Error: No condition provided for find." << std::endl;
    }
}
