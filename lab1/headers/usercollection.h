#ifndef USERCOLLECTION_H
#define USERCOLLECTION_H

#include <sqlite3.h>

#include <memory>
#include <string_view>
#include <vector>

#include "user.h"

class UserCollection {
   private:
    std::vector<std::unique_ptr<User>> users;
    sqlite3* db;

   public:
    void create_user(int id, const std::string& username,
                     const std::string& password,
                     const std::string& phone_number, const std::string& email);

    void set_db(sqlite3* db);
    bool delete_user(int id);
    bool update_user(int id, const std::string& username,
                     const std::string& password,
                     const std::string& phone_number, const std::string& email);

    std::vector<User*> filter_users(
        const std::string_view username = "", const std::string_view email = "",
        const std::string_view phone_number = "") const;

    void print_all_users() const;
    void input_user_data(const std::string& username,
                         const std::string& password, const std::string& email,
                         const std::string& phone_number);
    void load_users_from_db(sqlite3* db);
    void save_users_to_db(sqlite3* db) const;
};

#endif  // USERCOLLECTION_H
