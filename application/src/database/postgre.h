#ifndef POSTGRESDATABASE_H
#define POSTGRESDATABASE_H

#include "database.h"
#include <pqxx/pqxx>
#include <stdexcept>
#include <iostream>

class PostgresDatabase : public Database {
private:
    pqxx::connection* conn;

public:
    PostgresDatabase() : conn(nullptr) {}
    ~PostgresDatabase() { if (conn) delete conn; }

    void connect(const std::string& conn_str) override;

    pqxx::result execute_query(const std::string& query) override;
};

#endif
