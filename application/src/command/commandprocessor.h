#ifndef COMMANDPROCESSOR_H
#define COMMANDPROCESSOR_H

#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>
#include <nlohmann/json.hpp>
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
    void process_create_table(std::istringstream& iss);
    void process_drop_table(std::istringstream& iss);
    void process_add(std::istringstream& iss);
    void process_remove(std::istringstream& iss);
    void process_update(std::istringstream& iss);
    void process_find(std::istringstream& iss);
    void handle_list(const std::istringstream& /*iss*/) const;  
    void handle_help(const std::istringstream& /*iss*/) const; 
    void process_show_all() const;
    void process_export_to_json(const std::istringstream& iss);
};

#endif 
