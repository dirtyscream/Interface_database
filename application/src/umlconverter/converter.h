#ifndef CONVERTER_H
#define CONVERTER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include "../template/table.h"


class DatabaseConverter {
public:
    DatabaseConverter(const std::string& connection_string);
    bool dump_database(std::string& output_file);
    void generate_uml(const std::vector<Table>&, const std::string& uml_file);
    std::string get_database_name() const;
    bool parse_connection_string(const std::string& conn_str, std::string& host, std::string& port, 
                               std::string& dbname, std::string& user, std::string& password);

private:
    std::string connection_string;
};

#endif
