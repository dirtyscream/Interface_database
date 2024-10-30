#include "utils.h"
#include <cstdlib>

std::string get_connection_string() {
    try {
        std::cout << "Connect to default database or connect to another? (press 1 or 2): ";
        int mode;
        std::cin >> mode;
        std::cin.ignore(); 
        if (mode == 1) {
            std::string env_password = std::getenv("DB_PASSWORD");
            std::string host = std::getenv("HOST");
            std::string port = std::getenv("PORT");
            std::string user = std::getenv("USER");
            std::string db_name = std::getenv("DB_NAME");
            return "host=" + host + " port=" + port + " dbname=" + db_name + " user=" + user + " password=" + env_password;
        }
        else if (mode == 2) {
            std::string host;
            std::string port;
            std::string dbname;
            std::string user;
            std::string password;
            std::cout << "Enter host (e.g. 'localhost'): ";
            std::getline(std::cin, host);

            std::cout << "Enter port (e.g. '5432'): ";
            std::getline(std::cin, port);

            std::cout << "Enter database name (e.g. 'test'): ";
            std::getline(std::cin, dbname);

            std::cout << "Enter user (e.g. 'postgres'): ";
            std::getline(std::cin, user);

            std::cout << "Enter password (e.g. 'secret'): ";
            std::getline(std::cin, password);

            return "host=" + host + " port=" + port + " dbname=" + dbname + " user=" + user + " password=" + password;
        }
        else {
            std::cerr << "Invalid mode..." << std::endl;
            exit(-1);
        }
     } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}
