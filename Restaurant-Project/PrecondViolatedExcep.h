#pragma once
#include <exception>
#include <string>
class PrecondViolatedExcep : public std::exception {
private:
    std::string message;
public:
    explicit PrecondViolatedExcep(const std::string& msg) : message(msg) {}

    virtual const char* what() const noexcept {
        return message.c_str();
    }
};
