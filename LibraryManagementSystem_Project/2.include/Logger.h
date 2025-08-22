#ifndef LOGGER_H
#define LOGGER_H

#include <string>

enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    static void log(const std::string& message, LogLevel level);
    static void setLogFile(const std::string& filename);

private:
    static std::string logFile;
};

#endif



