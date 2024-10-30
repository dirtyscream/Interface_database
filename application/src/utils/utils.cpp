#include "utils.h"



std::string get_connection_string() {
    std::cout << "Connect to defualt database or connect to another?(press 1 or 2): ";
    int mode;
    std::cin >> mode;
    if (mode == 1) {
        return "host=postgres-interface.k.aivencloud.com port=14643 dbname=defaultdb user=avnadmin password=AVNS_tvo8JDom7oCDxRFUgHL";
    }
    else if (mode == 2) {
        std::string host, port, dbname, user, password;

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
        std::cout << "Invalid mode...";
        exit(-1);
    }
}
