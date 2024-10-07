#include "usercollection.h"

#include <iostream>

void UserCollection::create_user(int id, const std::string& username,
                                 const std::string& password,
                                 const std::string& phone_number,
                                 const std::string& email) {
    users.emplace_back(
        std::make_unique<User>(id, username, password, phone_number, email));
    save_users_to_db(db);
}

void UserCollection::set_database(sqlite3* db) { this->db = db; }

bool UserCollection::delete_user(int id) {
    for (size_t i = 0; i < users.size(); ++i) {
        if (users[i]->get_id() == id) {
            users.erase(users.begin() + i);
            std::cout << "User deleted successfully.\n";
            save_users_to_db(db);
            return true;
        }
    }
    std::cout << "User not found\n";
    return false;
}

bool UserCollection::update_user(int id, const std::string& username,
                                 const std::string& password,
                                 const std::string& phone_number,
                                 const std::string& email) {
    for (auto& user : users) {
        if (user->get_id() == id) {
            user = std::make_unique<User>(id, username, password, phone_number,
                                          email);
            save_users_to_db(db);
            return true;
        }
    }
    return false;
}

std::vector<User*> UserCollection::filter_users(
    const std::string_view username, const std::string_view email,
    const std::string_view phone_number) const {
    std::vector<User*> result;
    for (const auto& user : users) {
        if ((username.empty() || user->get_username() == username) &&
            (email.empty() || user->get_email() == email) &&
            (phone_number.empty() ||
             user->get_phone_number() == phone_number)) {
            result.push_back(user.get());
        }
    }
    return result;
}

void UserCollection::print_all_users() const {
    for (const auto& user : users) {
        std::cout << "ID: " << user->get_id() << "\n"
                  << "Username: " << user->get_username() << "\n"
                  << "Email: " << user->get_email() << "\n"
                  << "Phone: " << user->get_phone_number() << "\n\n";
    }
}

void UserCollection::input_user_data(std::string username, std::string password,
                                     std::string email,
                                     std::string phone_number) {
    int id = users.size() + 1;
    create_user(id, username, password, phone_number, email);
}

void UserCollection::load_users_from_db(sqlite3* db) {
    const char* sql =
        "SELECT id, username, password, phone_number, email FROM Users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        exit(-1);
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int id = sqlite3_column_int(stmt, 0);
        std::string username =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        std::string password =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
        std::string phone_number =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
        std::string email =
            reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));

        create_user(id, username, password, phone_number, email);
    }
    sqlite3_finalize(stmt);
}

void UserCollection::save_users_to_db(sqlite3* db) const {
    for (const auto& user : users) {
        const char* sql_insert_or_update =
            "INSERT INTO Users (id, username, password, phone_number, email) "
            "VALUES (?, ?, ?, ?, ?) "
            "ON CONFLICT(id) DO UPDATE SET "
            "username = excluded.username, "
            "password = excluded.password, "
            "phone_number = excluded.phone_number, "
            "email = excluded.email;";

        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db, sql_insert_or_update, -1, &stmt, nullptr) !=
            SQLITE_OK) {
            exit(-2);
        }

        sqlite3_bind_int(stmt, 1, user->get_id());
        sqlite3_bind_text(stmt, 2, user->get_username().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, 3, user->get_password().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, 4, user->get_phone_number().c_str(), -1,
                          SQLITE_STATIC);
        sqlite3_bind_text(stmt, 5, user->get_email().c_str(), -1,
                          SQLITE_STATIC);

        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }
}
