#include <iostream>

#include "database/postgre.h"
#include "command/commandprocessor.h"
#include "template/templatetable.h"
#include "utils/utils.h"


int main() {
    try {
        #ifdef _WIN32
        system("cls");
        #else
        system("clear");
        #endif
        std::cout << "Welcome to PostgreSQL interface\n";
        PostgresDatabase db;
        std::string conn_str = get_connection_string();
        db.connect(conn_str);
        DatabaseConverter converter(conn_str);
        TableRepository repository(db);
        TableService table_service(repository, converter);
        CommandProcessor command_processor(table_service);
        std::string command;
        while (true) {
            std::cout << "Enter command (or 'exit' to quit): ";
            std::getline(std::cin, command);
            if (command == "exit") break;
            command_processor.process_command(command);
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
