#include <sqlite3.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

#include "user.cpp"

class UserCollection {
   private:
    std::vector<std::unique_ptr<User>> users;

   public:
    void create_user(int id, const std::string& username,
                     const std::string& password,
                     const std::string& phone_number,
                     const std::string& email) {
        users.emplace_back(std::make_unique<User>(id, username, password,
                                                  phone_number, email));
    }

    std::vector<User*> filter_users(
        const std::string_view username = "", const std::string_view email = "",
        const std::string_view phone_number = "") const {
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

    void update_user(int id, const std::string& username,
                     const std::string& password,
                     const std::string& phone_number,
                     const std::string& email) {
        for (auto& user : users) {
            if (user->get_id() == id) {
                user = std::make_unique<User>(id, username, password,
                                              phone_number, email);
                return;
            }
        }
        throw std::runtime_error("User not found for update!");
    }

    void delete_user(int id) {
        auto it = std::remove_if(users.begin(), users.end(),
                                 [id](const std::unique_ptr<User>& user) {
                                     return user->get_id() == id;
                                 });
        if (it == users.end()) {
            throw std::runtime_error("User not found for deletion!");
        }
        users.erase(it, users.end());
    }

    void print_all_users() const {
        for (const auto& user : users) {
            std::cout << "ID: " << user->get_id() << "\n"
                      << "Username: " << user->get_username() << "\n"
                      << "Email: " << user->get_email() << "\n"
                      << "Phone: " << user->get_phone_number() << "\n"
                      << "Registration Time: " << user->get_registration_time()
                      << "\n";
        }
    }

    void input_user_data() {
        std::string username, password, phone_number, email;
        int id = users.size() + 1;
        std::cout << "Enter username: ";
        std::cin >> username;
        std::cout << "Enter password: ";
        std::cin >> password;
        std::cout << "Enter phone number: ";
        std::cin >> phone_number;
        std::cout << "Enter email: ";
        std::cin >> email;
        create_user(id, username, password, phone_number, email);
    }

    void load_users_from_db(sqlite3* db) {
        const char* sql =
            "SELECT id, username, password, phone_number, email FROM Users;";
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement.");
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

    void save_users_to_db(sqlite3* db) {
        const char* sql = "DELETE FROM Users;";
        sqlite3_exec(db, sql, nullptr, nullptr, nullptr);

        for (const auto& user : users) {
            const char* sql_insert =
                "INSERT INTO Users (id, username, password, phone_number, "
                "email) "
                "VALUES (?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt;

            if (sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr) !=
                SQLITE_OK) {
                throw std::runtime_error("Failed to prepare insert statement.");
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
};