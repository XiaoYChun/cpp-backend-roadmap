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
        std::cerr << "File Open Error!" << std::endl;
        return;
    }
}

Logger::~Logger() {
    if (m_filePtr != nullptr) {
        fclose(m_filePtr);
    }
}

void Logger::Log(const std::string& logInfo) {
    Log(Level::INFO, logInfo);
}

void Logger::Log(const Level level, const std::string& logInfo) {
     if (level < m_loggerConfig.minLevel) {
        return;
     }

    std::string completeStr = FormatLogStr(level, logInfo);

    if (m_loggerConfig.consoleAllowed) {
        std::cout << completeStr;
    }

    if (m_filePtr != nullptr && m_loggerConfig.fileAllowed) {
        fwrite(completeStr.data(), 1, completeStr.size(), m_filePtr);
    } // 还需要考虑检查写入是否正确完整的执行完了
}

std::tm Logger::GetCurrentTimeStruct() {
    auto currentTimePoint = std::chrono::system_clock::now();
    time_t currentTime = std::chrono::system_clock::to_time_t(currentTimePoint);
    std::tm* currentTimeStruct = std::localtime(&currentTime);

    return *currentTimeStruct;
}

// 这个函数当前的写法也没有考虑线程安全
// 分析当前职责：1. 获取当前时间； 2. 转换类型； 3. 格式化时间。
std::string Logger::CurrentTimeToStr() {
    std::tm currentTimeStruct = GetCurrentTimeStruct();

    char timeBuffer[32];
    std::snprintf(timeBuffer, sizeof(timeBuffer), "%04d-%02d-%02d %02d:%02d:%02d",
              currentTimeStruct.tm_year + 1900, // tm_year 是从1900年开始的
              currentTimeStruct.tm_mon + 1,     // tm_mon 是 0-11
              currentTimeStruct.tm_mday,
              currentTimeStruct.tm_hour,
              currentTimeStruct.tm_min,
              currentTimeStruct.tm_sec);

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

std::string Logger::FormatLogStr(const Level level, const std::string& logInfo) {
    std::string timeStr = CurrentTimeToStr();
    std::string levelStr = LevelToStr(level);
    std::string completeStr = "[ " + levelStr + "]"
                              + "[" + timeStr + "] "
                              + logInfo
                              + "\n";

    return completeStr;
}