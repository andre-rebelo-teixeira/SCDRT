#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

// Cpp librares
#include <iostream>
#include <string>

// Pico librarie
#include <pico/flash.h>

#include "pico/mutex.h"

class Logger // Used to send messages from the pico to the Computer over serial
{
public:
  Logger(uint16_t pico_idx) : pico_idx(pico_idx) {
    // This is to make sure stdio is thread safe
  };

  ~Logger() = default;

  enum class LogLevel {
    INFO,
    WARNING,
    ERROR,
    RESPONSE,
    VALUE,
  };

  void log(LogLevel level, std::string msg);
  void log(LogLevel level, std::string signal_name, float value);

private:
  std::string get_log_level(LogLevel level);
  uint16_t pico_idx;
};

inline std::string Logger::get_log_level(LogLevel level) {
  switch (level) {
  case LogLevel::INFO:
    return "[INFO]";
  case LogLevel::WARNING:
    return "[WARNING]";
  case LogLevel::ERROR:
    return "[ERROR]";
  case LogLevel::RESPONSE:
    return "[RESPONSE]";
  case LogLevel::VALUE:
    return "[VALUE]";
  default:
    return "[UNKNOWN]";
  }
}

inline void Logger::log(LogLevel level, std::string msg) {
  std::string log_level = get_log_level(level) + " " + std::to_string(pico_idx);
  msg = log_level + " : " + msg;
  std::cout << msg << std::endl;
}

inline void Logger::log(LogLevel level, std::string signal_name, float value) {
  std::string log_level = get_log_level(level) + " " + std::to_string(pico_idx);
  std::string msg =
      log_level + " : " + signal_name + " : " + std::to_string(value);
  std::cout << msg << std::endl;
}
#endif // __LOGGER_HPP__