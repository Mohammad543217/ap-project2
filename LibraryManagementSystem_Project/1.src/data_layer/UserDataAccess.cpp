#include "UserDataAccess.h"
#include "User.h"

#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stdexcept>
#include <memory>

namespace {

inline std::string trim(std::string s) {
    auto notSpace = [](int ch){ return !std::isspace(ch); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), notSpace));
    s.erase(std::find_if(s.rbegin(), s.rend(), notSpace).base(), s.end());
    return s;
}

inline bool toIntSafe(const std::string& s, int& out) {
    try {
        size_t idx = 0;
        int v = std::stoi(s, &idx);
        if (idx != s.size()) return false;
        out = v;
        return true;
    } catch (...) { return false; }
}

static std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> cells;
    std::string cur;
    bool inQuotes = false;

    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i+1] == '"') {
                    cur.push_back('"'); ++i;
                } else {
                    inQuotes = false;
                }
            } else cur.push_back(c);
        } else {
            if (c == ',') { cells.push_back(trim(cur)); cur.clear(); }
            else if (c == '"') { inQuotes = true; }
            else cur.push_back(c);
        }
    }
    cells.push_back(trim(cur));
    return cells;
}

static std::string escapeCSV(const std::string& s) {
    bool needQuotes = s.find_first_of(",\"\n\r") != std::string::npos;
    if (!needQuotes) return s;
    std::string out = "\"";
    for (char c : s) {
        if (c == '"') out += "\"\"";
        else out.push_back(c);
    }
    out += "\"";
    return out;
}

static std::string joinCSV(const std::vector<std::string>& cells) {
    std::ostringstream oss;
    for (size_t i = 0; i < cells.size(); ++i) {
        if (i) oss << ',';
        oss << escapeCSV(cells[i]);
    }
    return oss.str();
}

static std::string roleToString(UserRole r) {
    switch (r) {
        case UserRole::Admin:     return "Admin";
        case UserRole::Librarian: return "Librarian";
        case UserRole::Member:    return "Member";
        default:                  return "Member";
    }
}

static UserRole stringToRole(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    if (s == "admin")     return UserRole::Admin;
    if (s == "librarian") return UserRole::Librarian;
    if (s == "member")    return UserRole::Member;

    int v;
    if (toIntSafe(s, v)) {
        switch (v) {
            case 0: return UserRole::Admin;
            case 1: return UserRole::Librarian;
            case 2: return UserRole::Member;
        }
    }
    return UserRole::Member;
}

}

UserDataAccess::UserDataAccess(const std::string& filePath)
: filePath_(filePath) {}

bool UserDataAccess::addUser(const std::shared_ptr<User>& u) {
    auto users = loadFromFile();
    auto it = std::find_if(users.begin(), users.end(),
        [&](const std::shared_ptr<User>& x){ return x->getId() == u->getId(); });
    if (it != users.end()) return false;
    users.push_back(u);
    saveToFile(users);
    return true;
}

std::vector<std::shared_ptr<User>> UserDataAccess::getAll() const {
    return loadFromFile();
}

std::shared_ptr<User> UserDataAccess::getById(int id) const {
    auto users = loadFromFile();
    auto it = std::find_if(users.begin(), users.end(),
        [&](const std::shared_ptr<User>& x){ return x->getId() == id; });
    return it == users.end() ? nullptr : *it;
}

bool UserDataAccess::updateUser(const User& updated) {
    auto users = loadFromFile();
    bool found = false;
    for (auto& ptr : users) {
        if (ptr->getId() == updated.getId()) {
            ptr->setName(updated.getName());
            ptr->setEmail(updated.getEmail());
            ptr->setRole(updated.getRole());
            found = true;
            break;
        }
    }
    if (!found) return false;
    saveToFile(users);
    return true;
}

bool UserDataAccess::deleteUser(int id) {
    auto users = loadFromFile();
    auto before = users.size();
    users.erase(std::remove_if(users.begin(), users.end(),
        [&](const std::shared_ptr<User>& x){ return x->getId() == id; }),
        users.end());
    if (users.size() == before) return false;
    saveToFile(users);
    return true;
}

void UserDataAccess::saveToFile(const std::vector<std::shared_ptr<User>>& users) const {
    std::ofstream file(filePath_, std::ios::trunc);
    if (!file) throw std::runtime_error("Cannot open users CSV for writing: " + filePath_);


    file << "id,name,email,role\n";

    for (const auto& u : users) {
        std::vector<std::string> row = {
            std::to_string(u->getId()),
            u->getName(),
            u->getEmail(),
            roleToString(u->getRole())
        };
        file << joinCSV(row) << "\n";
    }
}

std::vector<std::shared_ptr<User>> UserDataAccess::loadFromFile() const {
    std::vector<std::shared_ptr<User>> users;
    std::ifstream file(filePath_);
    if (!file) {

        return users;
    }

    std::string line;
    bool first = true;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

    
        if (first) {
            first = false;
            auto head = splitCSV(line);
            if (!head.empty() && (head[0] == "id" || head[0] == "#id")) {
                continue;
            }
        }

        auto cells = splitCSV(line);
        if (cells.size() < 4) continue;

        int id = -1;
        if (!toIntSafe(cells[0], id)) continue;

        std::string name  = cells[1];
        std::string email = cells[2];
        UserRole role     = stringToRole(cells[3]);

        auto u = std::make_shared<User>(id, name, email, role);
        users.push_back(u);
    }

    return users;
}