#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

ConfigManager::ConfigManager(const std::string& filePath)
    : configFilePath(filePath) {}

std::string ConfigManager::trim(const std::string& s) {
    auto b = s.begin();
    auto e = s.end();
    while (b != e && std::isspace((unsigned char)*b)) ++b;
    while (e != b && std::isspace((unsigned char)*(e-1))) --e;
    return std::string(b, e);
}

std::string ConfigManager::toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
    return s;
}

bool ConfigManager::load() {
    std::ifstream in(configFilePath);
    if (!in.is_open()) return false;

    kv.clear();
    std::string line;
    while (std::getline(in, line)) {
        std::string t = trim(line);
        if (t.empty() ||  t[0]=='#' ||  t[0]==';' || t[0]=='[') continue;
        auto pos = t.find('=');
        if (pos == std::string::npos) continue;
        std::string k = trim(t.substr(0,pos));
        std::string v = trim(t.substr(pos+1));
        kv[k] = v;
    }
    return true;
}

bool ConfigManager::save() const {
    std::ofstream out(configFilePath);
    if (!out.is_open()) return false;
    for (const auto& p : kv) out << p.first << "=" << p.second << "\n";
    return true;
}

std::string ConfigManager::getString(const std::string& key, const std::string& def) const {
    auto it = kv.find(key);
    return it == kv.end() ? def : it->second;
}

int ConfigManager::getInt(const std::string& key, int def) const {
    auto it = kv.find(key);
    if (it == kv.end()) return def;
    try { return std::stoi(it->second); } catch (...) { return def; }
}

double ConfigManager::getDouble(const std::string& key, double def) const {
    auto it = kv.find(key);
    if (it == kv.end()) return def;
    try { return std::stod(it->second); } catch (...) { return def; }
}

bool ConfigManager::getBool(const std::string& key, bool def) const {
    auto it = kv.find(key);
    if (it == kv.end()) return def;
    auto v = toLower(trim(it->second));
    if (v=="1" ||  v=="true" ||  v=="yes" || v=="on") return true;
    if (v=="0" ||  v=="false" ||  v=="no"  || v=="off") return false;
    return def;
}

void ConfigManager::set(const std::string& key, const std::string& value) { kv[key] = value; }
void ConfigManager::set(const std::string& key, int value) { kv[key] = std::to_string(value); }
void ConfigManager::set(const std::string& key, double value) { kv[key] = std::to_string(value); }
void ConfigManager::set(const std::string& key, bool value) { kv[key] = value ? "true" : "false"; }