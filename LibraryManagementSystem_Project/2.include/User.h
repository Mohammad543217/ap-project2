#ifndef USER_H
#define USER_H

#include <string>
#include <cctype>

enum class UserRole {
    Admin = 0,
    Librarian = 1,
    Member = 2,
    Regular = Member
};

inline std::string to_string(UserRole r) {
    switch (r) {
        case UserRole::Admin:     return "Admin";
        case UserRole::Librarian: return "Librarian";
        case UserRole::Member:    return "Member";
    }
    return "Member";
}

inline UserRole userRoleFromString(std::string s) {
    for (auto& c : s) c = static_cast<char>(::tolower(static_cast<unsigned char>(c)));
    if (s == "admin")     return UserRole::Admin;
    if (s == "librarian") return UserRole::Librarian;
    if (s == "member")    return UserRole::Member;
    if (s == "regular")   return UserRole::Regular;
    return UserRole::Member;
}

class User {
public:
    User(int id, const std::string& name, const std::string& email, UserRole role)
    : id_(id), name_(name), email_(email), role_(role) {}

    int getId() const noexcept { return id_; }
    const std::string& getName() const noexcept { return name_; }
    const std::string& getEmail() const noexcept { return email_; }
    UserRole getRole() const noexcept { return role_; }

    void setName(const std::string& n)  noexcept { name_ = n; }
    void setEmail(const std::string& e) noexcept { email_ = e; }
    void setRole(UserRole r)            noexcept { role_ = r; }


    double getFines() const noexcept { return fines_; }
    void   setFines(double f) noexcept { fines_ = f; }
    void   addFine(double f) noexcept { fines_ += f; }
    void   clearFines() noexcept { fines_ = 0.0; }

private:
    int id_{};
    std::string name_;
    std::string email_;
    UserRole role_{UserRole::Member};
    double fines_{0.0};
};

#endif