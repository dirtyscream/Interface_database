#ifndef USER_H
#define USER_H

#include <chrono>
#include <string>

class User {
   private:
    int id;
    std::string username;
    std::string password;
    std::string phone_number;
    std::string email;

   public:
    User(int id, const std::string& username, const std::string& password,
         const std::string& phone_number, const std::string& email);

    std::string get_username() const;
    std::string get_password() const;
    std::string get_phone_number() const;
    std::string get_email() const;
    int get_id() const;

    User(const User&) = delete;
    User& operator=(const User&) = delete;
};

#endif  // USER_H
