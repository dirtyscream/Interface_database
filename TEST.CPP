#include <sqlite3.h>

#include <algorithm>
#include <chrono>
#include <exception>
#include <iostream>
#include <memory>
#include <ranges>
#include <stdexcept>
#include <string>
#include <vector>

class UserNotFoundException : public std::runtime_error {
   public:
    explicit UserNotFoundException(const std::string& message)
        : std::runtime_error(message) {}
};

class User {
   private:
    int id;
    std::string username;
    std::string password;
    std::string phone_number;
    std::string email;
    std::chrono::system_clock::time_point registration_time =
        std::chrono::system_clock::now();  // Инициализация в классе

   public:
    User(int id, const std::string& username, const std::string& password,
         const std::string& phone_number, const std::string& email)
        : id(id),
          username(username),
          password(password),
          phone_number(phone_number),
          email(email) {}

    std::string get_username() const { return username; }
    std::string get_password() const { return password; }
    std::string get_phone_number() const { return phone_number; }
    std::string get_email() const { return email; }

    // Заменено на std::chrono
    std::string get_registration_time() const {
        auto time = std::chrono::system_clock::to_time_t(registration_time);
        return std::ctime(&time);
    }

    int get_id() const { return id; }
    User(const User&) = delete;
    User& operator=(const User&) = delete;
};

void create_table(sqlite3* db) {
    const char* sql =
        "CREATE TABLE IF NOT EXISTS Users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "username TEXT NOT NULL UNIQUE, "
        "password TEXT NOT NULL, "
        "phone_number TEXT NOT NULL, "
        "email TEXT NOT NULL);";
    char* err_msg = nullptr;

    if (sqlite3_exec(db, sql, nullptr, nullptr, &err_msg) != SQLITE_OK) {
        std::cerr << "SQL error: " << err_msg << std::endl;
        sqlite3_free(err_msg);
    }
}

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

    void delete_user(int id) {
        auto it = std::ranges::remove_if(
            users, [id](const std::unique_ptr<User>& user) {
                return user->get_id() == id;
            });
        if (it == users.end()) {
            throw UserNotFoundException("User not found for deletion!");
        }
        users.erase(it.begin(), it.end());
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
        throw UserNotFoundException("User not found for update!");
    }

    // Теперь функция const
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

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <database_name>\n";
        return 1;
    }
    std::string username, password, phone_number, email;
    int id;

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

        switch (choice) {
            case 1:
                user_collection.print_all_users();
                break;
            case 2:
                user_collection.input_user_data();
                break;
            case 3:
                std::cout << "Enter id to update: ";
                std::cin >> id;
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
                    std::cout << "User updated successfully." << std::endl;
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;

            case 4:
                std::cout << "Enter id to delete: ";
                std::cin >> id;
                try {
                    user_collection.delete_user(id);
                    std::cout << "User deleted successfully." << std::endl;
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;

            case 5:
                user_collection.save_users_to_db(db);
                std::cout << "Users saved to database." << std::endl;
                break;
            case 6:
                std::cout << "Enter username to filter (leave empty to skip): ";
                std::cin.ignore();
                std::getline(std::cin, username);
                std::cout << "Enter email to filter (leave empty to skip): ";
                std::getline(std::cin, email);
                std::cout
                    << "Enter phone number to filter (leave empty to skip): ";
                std::getline(std::cin, phone_number);
                auto filtered_users =
                    user_collection.filter_users(username, email, phone_number);
                if (filtered_users.empty()) {
                    std::cout << "No users found matching the parameters."
                              << std::endl;
                } else {
                    for (const auto& user : filtered_users) {
                        std::cout
                            << "ID: " << user->get_id() << "\n"
                            << "Username: " << user->get_username() << "\n"
                            << "Email: " << user->get_email() << ", "
                            << "Phone: " << user->get_phone_number() << "\n"
                            << "Registration Time: "
                            << user->get_registration_time() << "\n";
                    }
                }
                break;

            case 7:
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
        }
    } while (choice != 7);

    sqlite3_close(db);
    return 0;
}
