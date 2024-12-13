#include "utils.h"
#include <iostream>
#include <fstream>
#include <string>

std::string read_connection_info_from_file(std::string& password, std::string& host, 
                                            std::string& user, 
                                            std::string& db_name) {
    std::ifstream secret_file("../application/src/utils/secret.txt");
    if (!secret_file) {
        std::cerr << "Error: Unable to open secret.txt file." << std::endl;
        exit(-1);
    }

    std::getline(secret_file, password);
    std::getline(secret_file, host);
    std::getline(secret_file, user);
    std::getline(secret_file, db_name);
    return "Connection info read from secret.txt";
}

std::string get_connection_string() {
    try {
        std::cout << "Connect to default database or connect to another? (press 1 or 2): ";
        int mode;
        std::cin >> mode;
        std::cin.ignore(); 

        if (mode == 1) {
            std::string password;
            std::string host;
            std::string port;
            std::string user;
            std::string db_name;
            read_connection_info_from_file(password, host, user, db_name);
            return "host=" + host + 
                   " dbname=" + db_name + 
                   " user=" + user + 
                   " password=" + password;
        } else if (mode == 2) {
            std::string custom_host;
            std::string custom_port;
            std::string custom_dbname;
            std::string custom_user;
            std::string custom_password;
            std::cout << "Enter host (e.g. 'localhost'): ";
            std::getline(std::cin, custom_host);

            std::cout << "Enter port (e.g. '5432'): ";
            std::getline(std::cin, custom_port);

            std::cout << "Enter database name (e.g. 'test'): ";
            std::getline(std::cin, custom_dbname);

            std::cout << "Enter user (e.g. 'postgres'): ";
            std::getline(std::cin, custom_user);

            std::cout << "Enter password (e.g. 'secret'): ";
            std::getline(std::cin, custom_password);

            return "host=" + custom_host + " port=" + custom_port + " dbname=" + custom_dbname + " user=" + custom_user + " password=" + custom_password;
        } else {
            std::cerr << "Invalid mode..." << std::endl;
            exit(-1);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        exit(-1);
    }
}
