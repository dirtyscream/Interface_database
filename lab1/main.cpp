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

void print_all_users(UserCollection& user_collection) {
    user_collection.print_all_users();
}

void input_user_data(UserCollection& user_collection) {
    user_collection.input_user_data();
}

void update_user(UserCollection& user_collection) {
    int id;
    std::cout << "Enter id to update: ";
    std::cin >> id;
    std::string username;
    std::string password;
    std::string phone_number;
    std::string email;
    std::cout << "Enter new username: ";
    std::cin >> username;
    std::cout << "Enter new password: ";
    std::cin >> password;
    std::cout << "Enter new phone number: ";
    std::cin >> phone_number;
    std::cout << "Enter new email: ";
    std::cin >> email;

    if (!user_collection.update_user(id, username, password, phone_number,
                                     email)) {
        std::cerr << "Error: Unable to update user with ID " << id << std::endl;
    } else {
        std::cout << "User updated successfully." << std::endl;
    }
}

void delete_user(UserCollection& user_collection) {
    int id;
    std::cout << "Enter id to delete: ";
    std::cin >> id;

    if (!user_collection.delete_user(id)) {
        std::cerr << "Error: Unable to delete user with ID " << id << std::endl;
    } else {
        std::cout << "User deleted successfully." << std::endl;
    }
}

void save_users_to_db(UserCollection& user_collection, sqlite3* db) {
    user_collection.save_users_to_db(db);
    std::cout << "Users saved to database." << std::endl;
}

void filter_users(UserCollection& user_collection) {
    std::string username;
    std::string email;
    std::string phone_number;
    std::cout << "Enter username to filter (leave empty to skip): ";
    std::cin.ignore();
    std::getline(std::cin, username);
    std::cout << "Enter email to filter (leave empty to skip): ";
    std::getline(std::cin, email);
    std::cout << "Enter phone number to filter (leave empty to skip): ";
    std::getline(std::cin, phone_number);

    auto filtered_users =
        user_collection.filter_users(username, email, phone_number);

    if (filtered_users.empty()) {
        std::cout << "No users found matching the parameters." << std::endl;
    } else {
        for (const auto& user : filtered_users) {
            std::cout << "ID: " << user->get_id() << "\n"
                      << "Username: " << user->get_username() << "\n"
                      << "Email: " << user->get_email() << ", "
                      << "Phone: " << user->get_phone_number() << "\n"
                      << "Registration Time: " << user->get_registration_time()
                      << "\n";
        }
    }
}

void handle_choice(int choice, UserCollection& user_collection, sqlite3* db) {
    switch (choice) {
        case 1:
            print_all_users(user_collection);
            break;
        case 2:
            input_user_data(user_collection);
            break;
        case 3:
            update_user(user_collection);
            break;
        case 4:
            delete_user(user_collection);
            break;
        case 5:
            save_users_to_db(user_collection, db);
            break;
        case 6:
            filter_users(user_collection);
            break;
        case 7:
            std::cout << "Exiting..." << std::endl;
            break;
        default:
            std::cout << "Invalid choice." << std::endl;
            break;
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

    if (sqlite3_open(db_name, &db) != SQLITE_OK) {
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
        handle_choice(choice, user_collection, db);
    } while (choice != 7);

    sqlite3_close(db);
    return 0;
}
