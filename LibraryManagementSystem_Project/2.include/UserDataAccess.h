#ifndef USERDATAACCESS_H
#define USERDATAACCESS_H

#include <string>
#include <vector>
#include <memory>

class User;

class UserDataAccess {
public:
    explicit UserDataAccess(const std::string& filePath);

    bool addUser(const std::shared_ptr<User>& u);
    std::vector<std::shared_ptr<User>> getAll() const;
    std::shared_ptr<User> getById(int id) const;
    bool updateUser(const User& updated);
    bool deleteUser(int id);

private:
    std::string filePath_;
    void saveToFile(const std::vector<std::shared_ptr<User>>& users) const;
    std::vector<std::shared_ptr<User>> loadFromFile() const;
};

#endif