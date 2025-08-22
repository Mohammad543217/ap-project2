#ifndef USER_MANAGER_H
#define USER_MANAGER_H

#include <vector>
#include <string>
#include <optional>
#include "User.h"

class UserManager {
public:
    UserManager() = default;

    bool addUser(const User& user, std::string* err = nullptr);
    bool removeUser(int id);
    bool updateUser(int id, const User& patch, std::string* err = nullptr);

    std::vector<User> searchByName(const std::string& name) const;
    std::vector<User> searchByEmail(const std::string& email) const;

    std::vector<User> sortByName(bool ascending) const;
    std::vector<User> listAll() const;

    bool addFine(int id, double amount, std::string* err = nullptr);
    bool payFine(int id, double amount, std::string* err = nullptr);

    User getByIdOrInvalid(int id) const;

private:
    std::vector<User> users_;

    static bool isValidEmail(const std::string& email);
    bool isEmailAvailable_NoFormatCheck(const std::string& email) const;
};

#endif