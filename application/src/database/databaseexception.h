#ifndef DATABASEEXCEPTION_H
#define DATABASEEXCEPTION_H

#include <iostream>
#include <stdexcept>
#include <string>

class BaseException {
public:
    virtual const char* what() const noexcept = 0;
    virtual ~BaseException() = default;
};

class Logger {
public:
    virtual void log(const std::string& message) const {
        std::cerr << "Log: " << message << std::endl;
    }
};

class DatabaseException : public BaseException, public Logger, public std::runtime_error {
public:
    explicit DatabaseException(const std::string& message)
        : std::runtime_error(message) {}

    const char* what() const noexcept override {
        return std::runtime_error::what();
    }

    void logError() const {
        log(what());
    }
};

#endif
