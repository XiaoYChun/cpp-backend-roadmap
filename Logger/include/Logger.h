#pragma once

#include <cstdio>
#include <string>

class Logger {
public:
    // Level 的位置还可以进一步考虑
    enum class Level {
        DEBUG,
        INFO,
        WARN,
        ERROR
    };

    // 通过 Config 结构体避免大量重载构造函数
    struct LoggerConfig {
        bool consoleAllowed = true;
        bool fileAllowed = true;
        Level minLevel = Level::INFO;
        std::string filePath = "./Log/test.log";
    };


    Logger();
    Logger(const LoggerConfig loggerConfig);
    ~Logger();

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void Log(const std::string& logInfo);
    void Log(const Level level, const std::string& logInfo);

private:
    std::string CurrentTimeToStr();
    std::string LevelToStr(Level level);

    LoggerConfig m_loggerConfig;

    FILE* m_filePtr;
};