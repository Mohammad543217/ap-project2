#include "Logger.h"
#include <fstream>
#include <iostream>
#include <ctime>

std::string Logger::logFile = "app.log";


void Logger::log(const std::string& message, LogLevel level) {
    std::ofstream file(logFile, std::ios::app);
    if (!file.is_open()) {
        std::cerr << "Unable to open log file." << std::endl;
        return;
    }

    std::string levelStr;
    switch (level) {
        case LogLevel::INFO: levelStr = "INFO"; break;
        case LogLevel::WARNING: levelStr = "WARNING"; break;
        case LogLevel::ERROR: levelStr = "ERROR"; break;
    }

    std::time_t now = std::time(nullptr);
    char buf[80];
    std::strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    file << "[" << buf << "] [" << levelStr << "] " << message << std::endl;
}

void Logger::setLogFile(const std::string& filename) {
    logFile = filename;
}