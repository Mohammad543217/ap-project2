#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include <map>

class ConfigManager {
public:
    explicit ConfigManager(const std::string& filePath = "config/application.ini");

    bool load();
    bool save() const;

    std::string getString(const std::string& key, const std::string& def = "") const;
    int         getInt(const std::string& key, int def = 0) const;
    double      getDouble(const std::string& key, double def = 0.0) const;
    bool        getBool(const std::string& key, bool def = false) const;

    void set(const std::string& key, const std::string& value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);

    const std::string& path() const { return configFilePath; }
    void setPath(const std::string& p) { configFilePath = p; }

private:
    std::map<std::string, std::string> kv;
    std::string configFilePath;

    static std::string trim(const std::string& s);
    static std::string toLower(std::string s);
};

#endif

