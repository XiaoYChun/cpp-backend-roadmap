#include "Logger.h"

#include <iostream>
#include <chrono>
#include <ctime>

Logger::Logger()
        : Logger(LoggerConfig{}) {
}

Logger::Logger(const LoggerConfig loggerConfig) 
        : m_loggerConfig(std::move(loggerConfig)) {
    // 这里的将 string 转为 char* 还需要仔细考虑
    m_filePtr = std::fopen(m_loggerConfig.filePath.c_str(), "a");

    if (m_filePtr == nullptr) {
        std::cout << "File Open Error!" << std::endl;
        return;
    }
}

Logger::~Logger() {
    fclose(m_filePtr);
}

void Logger::Log(const std::string& logInfo) {
    Log(Level::INFO, logInfo);
}

// 职责也太多了，后续拆分
void Logger::Log(const Level level, const std::string& logInfo) {
    std::string timeStr = CurrentTimeToStr();
    std::string levelStr = LevelToStr(level);
    std::string completeStr = "[ " + levelStr + "]"
                              + "[" + timeStr + "] "
                              + logInfo
                              + "\n";

    if (m_loggerConfig.consoleAllowed) {
        std::cout << completeStr;
    }

    if (m_loggerConfig.fileAllowed) {
        fwrite(completeStr.data(), 1, completeStr.size(), m_filePtr);
    }
}

// 这个函数当前的写法也没有考虑线程安全
// 而且感觉写的很差，写的又麻烦又乱...
// 职责太多了，后续拆分
std::string Logger::CurrentTimeToStr() {
    auto currentTimePoint = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(currentTimePoint);
    std::tm* currentTimeStruct = std::localtime(&currentTime);

    char timeBuffer[32];
    std::snprintf(timeBuffer, sizeof(timeBuffer), "%04d-%02d-%02d %02d:%02d:%02d",
              currentTimeStruct->tm_year + 1900, // tm_year 是从1900年开始的
              currentTimeStruct->tm_mon + 1,     // tm_mon 是 0-11
              currentTimeStruct->tm_mday,
              currentTimeStruct->tm_hour,
              currentTimeStruct->tm_min,
              currentTimeStruct->tm_sec);

    return std::string(timeBuffer);
}

std::string Logger::LevelToStr(Level level) {
    switch(level) {
        case Level::DEBUG:
            return "DEBUG";
        case Level::INFO:
            return "INFO";
        case Level::WARN:
            return "WARN";
        case Level::ERROR:
            return "ERROR";

        default:
            return "LOGERROR";
    }
}