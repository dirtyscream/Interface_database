#include "commandprocessor.h"
#include <iostream>
#include <iomanip> 
#include <cstdlib>

CommandProcessor::CommandProcessor(TableService& service) 
    : table_service(service), 
    command_map{
        {"use", [this](std::istringstream& iss) { handle_use(iss); }},
        {"create", [this](std::istringstream& iss) { handle_create(iss); }},
        {"drop", [this](std::istringstream& iss) { handle_drop(iss); }},
        {"list", [this](const std::istringstream& iss) { handle_list(iss); }},
        {"help", [this](const std::istringstream& iss) { handle_help(iss); }},
        {"add", [this](std::istringstream& iss) { process_add(iss); }},
        {"remove", [this](std::istringstream& iss) { process_remove(iss); }},
        {"update", [this](std::istringstream& iss) { process_update(iss); }},
        {"show", [this](const std::istringstream&) { process_show_all(); }},
        {"find", [this](std::istringstream& iss) { process_find(iss); }},
        {"export", [this](std::istringstream& iss) { process_export_to_json(iss); }},
        {"back", [this](const std::istringstream&) { current_table.clear(); std::cout << "Exited from table context." << std::endl; }},
        {"relation", [this](std::istringstream& iss) { process_add_relation(iss); }},
        {"begin", [this](const std::istringstream&) { process_start_transaction(); }},
        {"rollback", [this](const std::istringstream&) { process_rollback_transaction(); }},
        {"end", [this](const std::istringstream&) { process_end_transaction(); }},
        {"clear", [this](const std::istringstream&) { process_clear(); }},
        {"isolation", [this](std::istringstream& iss) { process_change_isolation(iss); }},
      }
{}

void CommandProcessor::process_clear() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
    std::cout << "Terminal cleared.\n";
}

void CommandProcessor::process_drop_table(std::istringstream& iss) {
    std::string table_name;
    iss >> table_name;
    table_service.drop_table(table_name);
}

void CommandProcessor::handle_list(const std::istringstream&) const {
    table_service.list_tables();
}

void CommandProcessor::process_start_transaction() {
    table_service.start_transaction();
}

void CommandProcessor::process_end_transaction() {
    table_service.end_transaction();
}

void CommandProcessor::process_rollback_transaction() {
    table_service.rollback_transaction();
}

void CommandProcessor::handle_help(const std::istringstream&) const {
    std::cout << "Available commands:\n";
    std::cout << "Database commands:\n";
    std::cout << "  use <table_name>     - Switch to specific table context.\n";
    std::cout << "  create table <table_name> <column> <type> ... - Create a new table.\n";
    std::cout << "  drop table <table_name> - Drop a table.\n";
    std::cout << "  list tables          - List all tables.\n";
    std::cout << "Table commands (after using a table):\n";
    std::cout << "  add <column1=value1> <column2=value2> - Add a record.\n";
    std::cout << "  relation <parent_table_id> - Add a relation between tables.\n";
    std::cout << "  remove <id>       - Remove a record by id.\n";
    std::cout << "  update <id> <column=value> - Update a record by id.\n";
    std::cout << "  show all          - Show all records.\n";
    std::cout << "  find <column=value> - Find records matching a condition.\n";
    std::cout << "  export to json    - Export table data to JSON format.\n"; 
    std::cout << "Transaction commands:\n";
    std::cout << "  begin transaction  - Start a new transaction.\n";
    std::cout << "  end transaction   - Commit the current transaction.\n";
    std::cout << "  rollback transaction - Rollback the current transaction.\n";
    std::cout << "  isolation - Change isolation level (1 - READ UNCOMMITTED, 2 - READ COMMITTED, 3 - REPEATABLE READ, 4 - SERIALIZABLE).\n";
    std::cout << "Navigation:\n";
    std::cout << "  exit               - Exit current context or application.\n";
    std::cout << "  back               - Go back to database context.\n";
    std::cout << "  help               - Show this help message.\n";
}



void CommandProcessor::process_show_all() const {
    std::vector<std::string> entries = table_service.show_all_entries(current_table);
    table_service.print_entries(entries, current_table);
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
void CommandProcessor::process_add_relation(std::istringstream& iss) {
    std::string parent_id;
    iss >> parent_id;
    if (parent_id.empty()) {
        std::cerr << "Error: No parent_id provided for relation." << std::endl;
        return;
    }
    table_service.add_relation(current_table, parent_id);
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
    std::string input_line;
    std::getline(iss, input_line); 

    if (input_line.empty()) {
        std::cerr << "Error: No data provided for add command." << std::endl;
        return;
    }

    if (!table_service.add_entry(current_table, input_line)) {
        std::cerr << "Error: Failed to add entry to the table." << std::endl;
    }
}

void CommandProcessor::process_remove(std::istringstream& iss) {
    int id;
    if (iss >> id) {
        table_service.remove_entry(current_table, id);
    } else {
        std::cerr << "Error: Invalid id for removal." << std::endl;
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
        std::cerr << "Error: Invalid id for update." << std::endl;
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

void CommandProcessor::process_export_to_json(const std::istringstream& iss) {
    if (current_table.empty()) {
        std::cout << "No table selected. Use 'use <table_name>' to select a table.\n";
        return;
    }
    nlohmann::json table_data = table_service.export_table_to_json(current_table);
    std::cout << table_data.dump(4) << std::endl;
}


void CommandProcessor::process_change_isolation(std::istringstream& iss) {
    std::cout << "Select transaction isolation level:\n";
    std::cout << "1. READ UNCOMMITTED\n";
    std::cout << "2. READ COMMITTED\n";
    std::cout << "3. REPEATABLE READ\n";
    std::cout << "4. SERIALIZABLE\n";
    int choice;
    iss >> choice;
    std::string level;
    switch (choice) {
        case 1:
            level = "READ UNCOMMITTED";
            break;
        case 2:
            level = "READ COMMITTED";
            break;
        case 3:
            level = "REPEATABLE READ";
            break;
        case 4:
            level = "SERIALIZABLE";
            break;
        default:
            std::cerr << "Invalid selection. Please choose a number between 1 and 4.\n";
            return;
    }
    try {
        table_service.change_isolation_level(level);
        std::cout << "Transaction isolation level successfully set to: " << level << ".\n";
    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }
}