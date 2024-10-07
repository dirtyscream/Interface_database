#include "user.h"

User::User(int id, const std::string& username, const std::string& password,
           const std::string& phone_number, const std::string& email)
    : id(id),
      username(username),
      password(password),
      phone_number(phone_number),
      email(email) {}

std::string User::get_username() const { return username; }
std::string User::get_password() const { return password; }
std::string User::get_phone_number() const { return phone_number; }
std::string User::get_email() const { return email; }
int User::get_id() const { return id; }
