#include "postgre.h"
#include "pqxx/pqxx"

void PostgresDatabase::connect(const std::string& conn_str) {
    conn = new pqxx::connection(conn_str);
    if (conn->is_open()) {
        std::cout << "Connected to the database." << std::endl;
    } else {
        throw std::runtime_error("Failed to connect to the database.");
    }
}

pqxx::result PostgresDatabase::execute_query(const std::string& query) {
    if (!conn) throw std::runtime_error("No active database connection.");

    pqxx::work txn(*conn);
    pqxx::result result = txn.exec(query);
    txn.commit();
    return result;
}