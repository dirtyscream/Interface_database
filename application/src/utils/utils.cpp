#include "utils.h"
#include <cstdlib>

std::string get_connection_string() {
    std::cout << "Connect to default database or connect to another? (press 1 or 2): ";
    int mode;
    std::cin >> mode;
    std::cin.ignore();

    const char* env_password = std::getenv("DB_PASSWORD");
    if (!env_password) {
        std::cerr << "Environment variable DB_PASSWORD is not set." << std::endl;
        exit(-1);
    }
    std::string password = env_password;

    if (mode == 1) {
        return "host=postgres-interface.k.aivencloud.com port=14643 dbname=defaultdb user=avnadmin password=" + password;
    }
    else if (mode == 2) {
        std::string host, port, dbname, user;

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
}
