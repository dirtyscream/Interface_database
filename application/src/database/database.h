#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <pqxx/pqxx>

class Database {    
public:
    virtual void connect(const std::string& conn_str) = 0;
    virtual pqxx::result execute_query(const std::string& query) = 0;
};

#endif
