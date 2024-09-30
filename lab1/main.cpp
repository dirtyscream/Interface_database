#include <sqlite3.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

#include "usercollection.cpp"

void create_table(sqlite3* db) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE, "
        "password TEXT NOT NULL, "
        "phone_number TEXT NOT NULL, "
        "email TEXT NOT NULL);";
    char* err_msg;
    int rc = sqlite3_exec(db, sql, nullptr, nullptr, &err_msg);
    if (rc != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <database_name>\n";
        return 1;
    }

    const char* db_name = argv[1];
    sqlite3* db;
    UserCollection user_collection;
    if (int rc = sqlite3_open(db_name, &db); rc != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db)
                  << std::endl;
        sqlite3_close(db);
        return 1;
    }

    create_table(db);
    user_collection.load_users_from_db(db);

    int choice;
    do {
        std::cout << "Menu: \n";
        std::cout << "1) Print all users\n";
        std::cout << "2) Add new user\n";
        std::cout << "3) Update user\n";
        std::cout << "4) Delete user\n";
        std::cout << "5) Save users to database\n";
        std::cout << "6) Filter users by argument\n";
        std::cout << "7) Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        if (choice == 1) {
            user_collection.print_all_users();
        } else if (choice == 2) {
            user_collection.input_user_data();
        } else if (choice == 3) {
            int id;
            std::cout << "Enter id to update: ";
            std::cin >> id;
            std::string username, password, phone_number, email;
            std::cout << "Enter new username: ";
            std::cin >> username;
            std::cout << "Enter new password: ";
            std::cin >> password;
            std::cout << "Enter new phone number: ";
            std::cin >> phone_number;
            std::cout << "Enter new email: ";
            std::cin >> email;

            try {
                user_collection.update_user(id, username, password,
                                            phone_number, email);
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
        } else if (choice == 4) {
            int id;
            std::cout << "Enter id to delete: ";
            std::cin >> id;

            try {
                user_collection.delete_user(id);
            } catch (const std::exception& e) {
                std::cerr << e.what() << '\n';
            }
        } else if (choice == 5) {
            user_collection.save_users_to_db(db);
        } else if (choice == 6) {
            std::string username, email, phone_number;
            std::cout << "Enter username (leave blank to skip): ";
            std::cin >> username;
            std::cout << "Enter email (leave blank to skip): ";
            std::cin >> email;
            std::cout << "Enter phone number (leave blank to skip): ";
            std::cin >> phone_number;

            auto filtered_users =
                user_collection.filter_users(username, email, phone_number);
            for (const auto& user : filtered_users) {
                std::cout << "ID: " << user->get_id() << "\n"
                          << "Username: " << user->get_username() << "\n"
                          << "Email: " << user->get_email() << "\n"
                          << "Phone: " << user->get_phone_number() << "\n"
                          << "Registration Time: "
                          << user->get_registration_time() << "\n";
            }
        }
    } while (choice != 7);

    sqlite3_close(db);
    return 0;
}