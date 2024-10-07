#include <iostream>

#include "usercollection.h"

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

void update_user(UserCollection& user_collection) {
    int id;
    std::string username;
    std::string name;
    std::string password;
    std::string phone_number;
    std::string email;

    std::cout << "Enter user ID to update: ";
    std::cin >> id;
    std::cout << "Enter new username: ";
    std::cin >> username;
    std::cout << "Enter new password: ";
    std::cin >> password;
    std::cout << "Enter new phone number: ";
    std::cin >> phone_number;
    std::cout << "Enter new email: ";
    std::cin >> email;

    if (user_collection.update_user(id, username, password, phone_number,
                                    email)) {
        std::cout << "User updated successfully.\n";
    } else {
        std::cout << "User not found.\n";
    }
}

void delete_user(UserCollection& user_collection) {
    int id;
    std::cout << "Enter user ID to delete: ";
    std::cin >> id;

    if (user_collection.delete_user(id)) {
        std::cout << "User deleted successfully.\n";
    } else {
        std::cout << "User not found.\n";
    }
}

void filter_users(const UserCollection& user_collection) {
    std::string username;
    std::string email;
    std::string phone_number;

    std::cout << "Enter username (or leave empty): ";
    std::cin >> username;
    std::cout << "Enter email (or leave empty): ";
    std::cin >> email;
    std::cout << "Enter phone number (or leave empty): ";
    std::cin >> phone_number;

    auto users = user_collection.filter_users(username, email, phone_number);
    if (!users.empty()) {
        for (const auto& user : users) {
            std::cout << "ID: " << user->get_id() << "\n"
                      << "Username: " << user->get_username() << "\n"
                      << "Email: " << user->get_email() << "\n"
                      << "Phone: " << user->get_phone_number() << "\n\n";
        }
    } else {
        std::cout << "No users found with the given criteria.\n";
    }
}

void input_data(UserCollection& user_collection) {
    std::string username;
    std::string email;
    std::string phone_number;
    std::string password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;
    std::cout << "Enter phone number: ";
    std::cin >> phone_number;
    std::cout << "Enter email: ";
    std::cin >> email;
    user_collection.input_user_data(username, password, email, phone_number);
}

void handle_choice(int choice, UserCollection& user_collection, sqlite3* db) {
    switch (choice) {
        case 1:
            user_collection.print_all_users();
            break;
        case 2:
            input_data(user_collection);
            break;
        case 3:
            update_user(user_collection);
            break;
        case 4:
            delete_user(user_collection);
            break;
        case 5:
            filter_users(user_collection);
            break;
        case 6:
            std::cout << "Exiting..." << std::endl;
            sqlite3_close(db);
            exit(0);
        default:
            std::cout << "Invalid choice." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <database_name>\n";
        return 1;
    }

    const char* db_name = argv[1];
    sqlite3* db;
    UserCollection user_collection;

    if (sqlite3_open(db_name, &db) != SQLITE_OK) {
        std::cerr << "Cannot open database: " << sqlite3_errmsg(db)
                  << std::endl;
        return 1;
    }

    create_table(db);
    user_collection.db = db;
    user_collection.load_users_from_db(db);

    int choice;
    do {
        std::cout << "Menu: \n"
                  << "1) Print all users\n"
                  << "2) Add new user\n"
                  << "3) Update user\n"
                  << "4) Delete user\n"
                  << "5) Filter users by argument\n"
                  << "6) Exit\n"
                  << "Choose an option: ";
        std::cin >> choice;
        handle_choice(choice, user_collection, db);
    } while (choice != 6);

    sqlite3_close(db);
    return 0;
}
