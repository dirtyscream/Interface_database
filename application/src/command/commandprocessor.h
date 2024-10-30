#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>
#include "../service/tableservice.h"

class CommandProcessor {
public:
    explicit CommandProcessor(TableService& service); 
    void process_command(const std::string& command); 

private:
    TableService& table_service;
    bool exit_flag = false;
    std::string current_table; 
    std::unordered_map<std::string, std::function<void(std::istringstream&)>> command_map;
    void handle_use(std::istringstream& iss);
    void handle_create(std::istringstream& iss);
    void handle_drop(std::istringstream& iss);
    void handle_list(std::istringstream&);
    void handle_help(std::istringstream&);
    void process_create_table(std::istringstream& iss);
    void process_drop_table(std::istringstream& iss);
    void handle_contextual_commands(const std::string& command);
    void process_add(std::istringstream& iss);
    void process_remove(std::istringstream& iss);
    void process_update(std::istringstream& iss);
    void process_show_all();
    void process_find(std::istringstream& iss);
};

#endif
