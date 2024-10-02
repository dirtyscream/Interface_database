#include <chrono>
#include <ctime>
#include <iostream>

class User {
   private:
    int id;
    std::string username;
    std::string password;
    std::string phone_number;
    std::string email;
    std::chrono::system_clock::time_point registration_time =
        std::chrono::system_clock::now();

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

    std::string get_registration_time() const {
        auto time = std::chrono::system_clock::to_time_t(registration_time);
        return std::ctime(&time);
    }

    int get_id() const { return id; }
    User(const User&) = delete;
    User& operator=(const User&) = delete;
};