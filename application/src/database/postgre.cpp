#include "databaseexception.h"
#include "postgre.h"
#include "pqxx/pqxx"
#include <iostream>

void PostgresDatabase::connect(const std::string& conn_str) {
    conn = new pqxx::connection(conn_str);
    if (conn->is_open()) {
        std::cout << "Connected to the database." << std::endl;
    } else {
        DatabaseException ex("Failed to connect to the database.");
        ex.logError();
        throw ex;
    }
}

pqxx::result PostgresDatabase::execute_query(const std::string& query) {
    if (!conn) {
        DatabaseException ex("No active database connection.");
        ex.logError();
        throw ex;
    }

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec(query);
    txn.commit();
    return result;
}
