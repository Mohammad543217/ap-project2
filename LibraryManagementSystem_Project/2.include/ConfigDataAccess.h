#ifndef CONFIG_DATA_ACCESS_H
#define CONFIG_DATA_ACCESS_H

#include <string>
#include <unordered_map>

class ConfigDataAccess {
public:
    explicit ConfigDataAccess(const std::string& filename);


    std::unordered_map<std::string, std::string> load() const;


    bool save(const std::unordered_map<std::string, std::string>& kv) const;

private:
    std::string filename_;
};

#endif 

