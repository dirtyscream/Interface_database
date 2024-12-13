#include "postgre.h"

void PostgresDatabase::connect(const std::string& conn_str) {
    try {
        conn = std::make_unique<pqxx::connection>(conn_str);
        if (!conn->is_open()) {
            throw std::runtime_error("Connection to database failed");
        }
    } catch (const std::exception& e) {
        std::cerr << "Error connecting to database: " << e.what() << std::endl;
        throw;
    }
}

pqxx::result PostgresDatabase::execute_query(const std::string& query) {
    if (!conn) {
        throw std::runtime_error("No active database connection.");
    }
    if (txn) {
        return txn->exec(query);
    } else {
        pqxx::nontransaction ntxn(*conn);
        return ntxn.exec(query);
    }
}

void PostgresDatabase::begin_transaction() {
    if (txn) {
        std::cerr << "Transaction already started!" << std::endl;
        return;
    }
    txn = std::make_unique<pqxx::work>(*conn);
}

void PostgresDatabase::end_transaction() {
    if (!txn) {
        std::cerr << "No active transaction to commit!" << std::endl;
        return;
    }

    try {
        txn->commit(); 
    } catch (const std::exception& e) {
        std::cerr << "Error during commit: " << e.what() << std::endl;
    }
    txn.reset(); 
}

void PostgresDatabase::rollback_transaction() {
    if (!txn) {
        std::cerr << "No active transaction to rollback!" << std::endl;
        return;
    }
    txn->abort();
    txn.reset();
}
