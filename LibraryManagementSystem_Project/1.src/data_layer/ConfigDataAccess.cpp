#include "ConfigDataAccess.h"

#include <fstream>
#include <sstream>
#include <unordered_map>
#include <algorithm>

using namespace std;

namespace {
    static inline void ltrim(string& s) {
        s.erase(s.begin(), find_if(s.begin(), s.end(), [](int ch){ return !isspace(ch); }));
    }
    static inline void rtrim(string& s) {
        s.erase(find_if(s.rbegin(), s.rend(), [](int ch){ return !isspace(ch); }).base(), s.end());
    }
    static inline void trim(string& s) {
        ltrim(s); rtrim(s);
    }
    static inline void stripBOM(string& s) {
        if (s.size() >= 3 &&
            static_cast<unsigned char>(s[0]) == 0xEF &&
            static_cast<unsigned char>(s[1]) == 0xBB &&
            static_cast<unsigned char>(s[2]) == 0xBF) {
            s.erase(0, 3);
        }
    }
}

ConfigDataAccess::ConfigDataAccess(const std::string& filename)
: filename_(filename) {}

std::unordered_map<std::string, std::string> ConfigDataAccess::load() const {
    unordered_map<string,string> kv;

    ifstream in(filename_);
    if (!in) return kv;

    string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        stripBOM(line);
        trim(line);
        if (line.empty()) continue;
        if (line[0] == ';' || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos == string::npos) continue;

        string key = line.substr(0, pos);
        string val = line.substr(pos + 1);
        trim(key); trim(val);
        if (!key.empty()) kv[key] = val;
    }
    return kv;
}

bool ConfigDataAccess::save(const std::unordered_map<std::string, std::string>& kv) const {
    ofstream out(filename_, ios::trunc);
    if (!out) return false;

    for (const auto& p : kv) {
        out << p.first << '=' << p.second << '\n';
    }
    return true;
}