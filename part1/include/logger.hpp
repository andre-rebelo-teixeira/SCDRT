#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include <string>
#include <iostream>

// TODO :  Include rpi pico id into the log message for when multiple picos are being used at the same time
class Logger // Used to send messages from the pico to the Computer over serial
{
public:
    Logger() = default;
    ~Logger() = default;

    enum class LogLevel
    {
        INFO,
        WARNING,
        ERROR,
        RESPONSE,
    };

    void log(LogLevel level, std::string msg);
    void log(LogLevel level, std::string signal_name, float value);

private:
    std::string get_log_level(LogLevel level);
};

inline std::string Logger::get_log_level(LogLevel level)
{
    switch (level)
    {
    case LogLevel::INFO:
        return "[INFO]";
    case LogLevel::WARNING:
        return "[WARNING]";
    case LogLevel::ERROR:
        return "[ERROR]";
    case LogLevel::RESPONSE:
        return "[RESPONSE]";
    default:
        return "UNKNOWN";
    }
}

inline void Logger::log(LogLevel level, std::string msg)
{
    std::string log_level = get_log_level(level);

    msg = log_level + " : " + msg;
    std::cout << msg << std::endl;
}

inline void Logger::log(LogLevel level, std::string signal_name, float value)
{
    std::string log_level = get_log_level(level);

    std::string msg = log_level + " : " + signal_name + " : " + std::to_string(value);
    std::cout << msg << std::endl;
}
#endif // __LOGGER_HPP__