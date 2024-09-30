#include <sqlite3.h>

#include <chrono>
#include <iostream>
#include <stdexcept>
#include <string>

class User {
   private:
    int id;
    std::string username;
    std::string password;
    std::string phone_number;
    std::string email;
    std::chrono::system_clock::time_point registration_time;

   public:
    User(int id, std::string username, std::string password,
         std::string phone_number, std::string email)
        : id(id),
          username(username),
          password(password),
          phone_number(phone_number),
          email(email) {
        this->registration_time = std::chrono::system_clock::now();
    }
    std::string get_username() const { return username; }
    std::string get_password() const { return password; }
    std::string get_phone_number() const { return phone_number; }
    std::string get_email() const { return email; }
    std::string get_registration_time() const {
        std::time_t time =
            std::chrono::system_clock::to_time_t(registration_time);
        return std::ctime(&time);
    }
    int get_id() const { return id; }
};

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

class UserCollection {
   private:
    User** users;
    int user_count;
    int capacity;
    void resize() {
        if (capacity == 0) {
            capacity = 1;
        } else {
            capacity *= 2;
        }
        users = static_cast<User**>(realloc(users, capacity * sizeof(User*)));
        if (!users) {
            throw std::runtime_error("Memory allocation failed!");
        }
    }

   public:
    UserCollection() : users(nullptr), user_count(0), capacity(0) {}

    void create_user(int id, const std::string& username,
                     const std::string& password,
                     const std::string& phone_number,
                     const std::string& email) {
        if (user_count >= capacity) {
            resize();
        }
        users[user_count++] =
            new User(id, username, password, phone_number, email);
    }

    std::vector<User*> filter_users(const std::string& username = "",
                                    const std::string& email = "",
                                    const std::string& phone_number = "") {
        std::vector<User*> result;
        for (int i = 0; i < user_count; ++i) {
            bool matches = true;
            if (!username.empty() && users[i]->get_username() != username) {
                matches = false;
            }
            if (!email.empty() && users[i]->get_email() != email) {
                matches = false;
            }
            if (!phone_number.empty() &&
                users[i]->get_phone_number() != phone_number) {
                matches = false;
            }
            if (matches) {
                result.push_back(users[i]);
            }
        }
        return result;
    }

    void update_user(int id, const std::string& username,
                     const std::string& password,
                     const std::string& phone_number,
                     const std::string& email) {
        for (int i = 0; i < user_count; ++i) {
            if (users[i]->get_id() == id) {
                delete users[i];
                users[i] =
                    new User(id, username, password, phone_number, email);
                return;
            }
        }
        throw std::runtime_error("User not found for update!");
    }

    void delete_user(int id) {
        for (int i = 0; i < user_count; ++i) {
            if (users[i]->get_id() == id) {
                delete users[i];
                users[i] = users[--user_count];
                return;
            }
        }
        throw std::runtime_error("User not found for deletion!");
    }

    void print_all_users() const {
        for (int i = 0; i < user_count; ++i) {
            std::cout << "ID: " << users[i]->get_id() << "\n"
                      << "Username: " << users[i]->get_username() << "\n"
                      << "Email: " << users[i]->get_email() << ", "
                      << "Phone: " << users[i]->get_phone_number() << "\n"
                      << "Registration Time: "
                      << users[i]->get_registration_time() << "\n";
        }
    }

    void input_user_data() {
        std::string username, password, phone_number, email;
        int id = user_count + 1;
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
        int rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        if (rc != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement.");
        }

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const char* username =
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            const char* password =
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            const char* phone_number =
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3));
            const char* email =
                reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4));
            create_user(id, username, password, phone_number, email);
        }
        sqlite3_finalize(stmt);
    }

    void save_users_to_db(sqlite3* db) {
        const char* sql = "DELETE FROM Users;";
        sqlite3_exec(db, sql, nullptr, nullptr, nullptr);

        for (int i = 0; i < user_count; ++i) {
            const char* sql_insert =
                "INSERT INTO Users (id, username, password, phone_number, "
                "email) "
                "VALUES (?, ?, ?, ?, ?);";
            sqlite3_stmt* stmt;
            sqlite3_prepare_v2(db, sql_insert, -1, &stmt, nullptr);
            sqlite3_bind_int(stmt, 1, users[i]->get_id());
            sqlite3_bind_text(stmt, 2, users[i]->get_username().c_str(), -1,
                              SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, users[i]->get_password().c_str(), -1,
                              SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, users[i]->get_phone_number().c_str(), -1,
                              SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, users[i]->get_email().c_str(), -1,
                              SQLITE_STATIC);
            sqlite3_step(stmt);
            sqlite3_finalize(stmt);
        }
    }

    ~UserCollection() {
        for (int i = 0; i < user_count; ++i) {
            delete users[i];
        }
        free(users);
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cout << "Usage: " << argv[0] << " <database_name>\n";
        return 1;
    }

    const char* db_name = argv[1];
    sqlite3* db;
    UserCollection user_collection;
    int rc = sqlite3_open(db_name, &db);
    if (rc != SQLITE_OK) {
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
            case 3: {
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
                    std::cout << "User updated successfully." << std::endl;
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 4: {
                int id;
                std::cout << "Enter id to delete: ";
                std::cin >> id;
                try {
                    user_collection.delete_user(id);
                    std::cout << "User deleted successfully." << std::endl;
                } catch (const std::runtime_error& e) {
                    std::cerr << "Error: " << e.what() << std::endl;
                }
                break;
            }
            case 5:
                user_collection.save_users_to_db(db);
                std::cout << "Users saved to database." << std::endl;
                break;
            case 6: {
                std::string username, email, phone_number;
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
            }
            case 7:
                break;
            default:
                std::cout << "Invalid choice." << std::endl;
        }
    } while (choice != 7);

    sqlite3_close(db);
    return 0;
}

Исправь все issues из sonarcloud в этом коде.
Use the init-statement to declare "rc" inside the if statement.
Define each identifier in a dedicated statement.
Define each identifier in a dedicated statement.
Use the init-statement to declare "filtered_users" inside the if statement.
Refactor this code to not nest more than 3 if|for|do|while|switch statements.
Pass expensive to copy object "username" by reference to const.
Pass expensive to copy object "password" by reference to const.
Pass expensive to copy object "phone_number" by reference to const.
Pass expensive to copy object "email" by reference to const.
Replace this call to the non reentrant function "ctime" by a call to "ctime_r".
Remove use of this obsolete "ctime" function. Replace it by a call to "strftime".
Replace this use of "ctime" with equivalent code using "std::chrono".
Customize this class' copy constructor to participate in resource management. Customize or delete its copy assignment operator. Also consider whether move operations should be customized.
Remove this use of "realloc".
Define and throw a dedicated exception instead of using a generic one.
Do not use the constructor's initializer list for data member "capacity". Use the in-class initializer instead.
Do not use the constructor's initializer list for data member "users". Use the in-class initializer instead.
Do not use the constructor's initializer list for data member "user_count". Use the in-class initializer instead.
Access of the heap area at index 2, while it holds only a single 'class User *' element
Access of the heap area at an overflowing index, while it holds only a single 'class User *' element
Access of the heap area at index 1, while it holds only a single 'class User *' element
Access of the heap area at index 3, while it holds only a single 'class User *' element
Replace the use of "new" with an operation that automatically manages the memory.
This function should be declared "const"
Replace this const reference to "std::string" by a "std::string_view"
Replace this const reference to "std::string" by a "std::string_view".
Rewrite the code so that you no longer need this "delete".
Replace the use of "new" with an operation that automatically manages the memory
Define and throw a dedicated exception instead of using a generic one.
Define and throw a dedicated exception instead of using a generic one.  
Define each identifier in a dedicated statement.
Use the init-statement to declare "rc" inside the if statement.
Define and throw a dedicated exception instead of using a generic one.
Replace the redundant type with "auto". 
Remove this use of "free".
