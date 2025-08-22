#ifndef FILEUTILS_H
#define FILEUTILS_H

#include <string>
#include <vector>

class FileUtils {
public:
    static std::vector<std::vector<std::string>> readCSV(const std::string& filename);
    static bool writeCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data);
};

#endif
