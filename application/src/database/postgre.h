#ifndef POSTGRESDATABASE_H
#define POSTGRESDATABASE_H

#include "database.h"
#include <pqxx/pqxx>
#include <stdexcept>
#include <iostream>
#include <memory>

class PostgresDatabase : public Database {
private:
    std::unique_ptr<pqxx::connection> conn;
public:
    PostgresDatabase() = default;
    void connect(const std::string& conn_str) override;
    pqxx::result execute_query(const std::string& query) override;
};

#endif
