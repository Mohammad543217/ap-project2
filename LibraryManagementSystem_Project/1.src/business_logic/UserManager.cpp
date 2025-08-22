#include "UserManager.h"
#include <algorithm>
#include <cctype>
#include <iostream>

// ===== Helpers =====
bool UserManager::isValidEmail(const std::string& email) {
    size_t at  = email.find('@');
    size_t dot = email.find_last_of('.');
    if (at == std::string::npos || dot == std::string::npos) return false;
    if (at == 0) return false;
    if (dot <= at + 1) return false;
    if (dot == email.size() - 1) return false;
    return true;
}

bool UserManager::isEmailAvailable_NoFormatCheck(const std::string& email) const {
    for (const auto& u : users_) if (u.getEmail() == email) return false;
    return true;
}

// ===== CRUD / Mutations =====
bool UserManager::addUser(const User& user, std::string* err) {
    if (user.getId() <= 0) { if (err) *err = "شناسه‌ی کاربر باید مثبت باشد."; return false; }
    if (user.getName().empty()) { if (err) *err = "نام خالی است."; return false; }
    if (!isValidEmail(user.getEmail())) { if (err) *err = "ایمیل نامعتبر است."; return false; }
    if (!isEmailAvailable_NoFormatCheck(user.getEmail())) { if (err) *err = "این ایمیل قبلاً ثبت شده است."; return false; }
    // عدم تکرار id
    if (std::any_of(users_.begin(), users_.end(), [&](const User& u){return u.getId()==user.getId();})) {
        if (err) *err = "شناسه‌ی کاربر تکراری است."; return false;
    }
    users_.push_back(user);
    return true;
}

bool UserManager::removeUser(int id) {
    auto it = std::remove_if(users_.begin(), users_.end(), [&](const User& u){return u.getId()==id;});
    if (it == users_.end()) return false;
    users_.erase(it, users_.end());
    return true;
}

bool UserManager::updateUser(int id, const User& patch, std::string* err) {
    auto it = std::find_if(users_.begin(), users_.end(), [&](const User& u){return u.getId()==id;});
    if (it == users_.end()) { if (err) *err = "کاربر یافت نشد."; return false; }

    // قانون: id را تغییر نمی‌دهیم. بقیه اگر مقدار معتبر داشت، اعمال می‌کنیم.
    if (!patch.getName().empty()) it->setName(patch.getName());
    if (!patch.getEmail().empty()) {
        if (!isValidEmail(patch.getEmail())) { if (err) *err = "ایمیل نامعتبر است."; return false; }
        if (patch.getEmail() != it->getEmail() && !isEmailAvailable_NoFormatCheck(patch.getEmail())) {
            if (err) *err = "این ایمیل قبلاً ثبت شده است."; return false;
        }
        it->setEmail(patch.getEmail());
    }
    // نقش (Role) را اگر patch نقشی غیر از مقدار پیش‌فرض دارد تنظیم کن
    it->setRole(patch.getRole());
    // جرایم اگر در patch مقداردهی شد (>=0) می‌توانی در صورت نیاز اعمال کنی (اختیاری)
    return true;
}

// ===== Queries =====
std::vector<User> UserManager::searchByName(const std::string& name) const {
    std::vector<User> out;
    out.reserve(users_.size());
    for (const auto& u : users_) {
        if (u.getName().find(name) != std::string::npos) out.push_back(u);
    }
    return out;
}

std::vector<User> UserManager::searchByEmail(const std::string& email) const {
    std::vector<User> out;
    out.reserve(users_.size());
    for (const auto& u : users_) {
        if (u.getEmail().find(email) != std::string::npos) out.push_back(u);
    }
    return out;
}

std::vector<User> UserManager::sortByName(bool ascending) const {
    std::vector<User> v = users_;
    std::sort(v.begin(), v.end(), [&](const User& a, const User& b){
        if (ascending) return a.getName() < b.getName();
        return a.getName() > b.getName();
    });
    return v;
}

std::vector<User> UserManager::listAll() const {
    return users_;
}

// ===== Fines =====
bool UserManager::addFine(int id, double amount, std::string* err) {
    if (amount <= 0) { if (err) *err = "مبلغ جریمه باید مثبت باشد."; return false; }
    auto it = std::find_if(users_.begin(), users_.end(), [&](const User& u){return u.getId()==id;});
    if (it == users_.end()) { if (err) *err = "کاربر یافت نشد."; return false; }
    it->setFines(it->getFines() + amount);
    return true;
}

bool UserManager::payFine(int id, double amount, std::string* err) {
    if (amount <= 0) { if (err) *err = "مبلغ پرداخت باید مثبت باشد."; return false; }
    auto it = std::find_if(users_.begin(), users_.end(), [&](const User& u){return u.getId()==id;});
    if (it == users_.end()) { if (err) *err = "کاربر یافت نشد."; return false; }
    double cur = it->getFines();
    if (amount > cur) amount = cur;
    it->setFines(cur - amount);
    return true;
}

// ===== Utility =====
User UserManager::getByIdOrInvalid(int id) const {
    auto it = std::find_if(users_.begin(), users_.end(), [&](const User& u){return u.getId()==id;});
    if (it != users_.end()) return *it;
    // برگرداندن یک یوزر «نامعتبر» با role پیش‌فرض (برای اینکه main.getRole() کرش نکند)
    return User(-1, "INVALID", "invalid@example.com", UserRole::Regular);
}