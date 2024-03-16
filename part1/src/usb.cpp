#include "usb.hpp"
#include <algorithm>
#include <memory>
#include <numeric>
#include <string>

#include "logger.hpp"

void UsbReader::read_usb() {

  char c;

  unsigned int num_reads = 0;
  bool is_reading = true;

  while (num_reads++ < num_max_reads && is_reading) {
    int read_char = getchar_timeout_us(2);

    if (read_char == PICO_ERROR_TIMEOUT) {
      is_reading = false;
      break;
    }

    char ch = static_cast<char>(read_char);
    std::string log_msg = "Read_char : " + std::to_string(get_core_num());

    for (auto &i : unfinished_msg) {
      log_msg += i;
    }

    log_msg.push_back(ch);
    this->l->log(Logger::LogLevel::INFO, log_msg);

    switch (ch) {
    // receive end of message
    case '/':
      this->l->log(Logger::LogLevel::INFO, "USB message received");
      if (this->process_usb_msg(this->unfinished_msg)) {
        this->l->log(Logger::LogLevel::INFO,
                     "USB message processed successfully");
      } else {
        this->l->log(Logger::LogLevel::ERROR,
                     "USB message not processed successfully");
      }
      this->unfinished_msg.clear();
      this->unfinished_msg.push_back("");
      break;

    // ignore these characters
    case '\r':
      break;

    // ignore these characters
    case '\0':
      break;

    // create a new empty node in the vector since a new letter will follow
    case ' ':
      this->unfinished_msg.push_back("");
      break;

    // normal letter so i just want to append it to the function
    default:
      unfinished_msg.back().push_back(static_cast<char>(read_char));
      break;
    }
  }

  return;
}

bool UsbReader::process_usb_msg(msg m) {
  if (m.size() == 0) {
    this->l->log(Logger::LogLevel::ERROR, "USB message is empty");
    return false;
  }

  if (m.size() != 3 && m.size() != 4 && m.size() != 1) {
    this->l->log(Logger::LogLevel::ERROR,
                 "USB message is not the correct size");
    return false;
  }

  std::string current_msg;
  for (auto &i : m) {
    current_msg += i;
  }

  if (m.size() == 1) {
    if (m[0] == "r") {
      // reset system
      return true;
    }
    return false;
  } else if (m.size() == 3) {
    if (m[0] == "d") {
      // set the duty cycle
      std::string lum = m[1];
      std::string duty_cycle = m[1];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::SET, TYPE::DUTY_CYCLE, duty_cycle, lum));
    } else if (m[0] == "g" && m[1] == "d") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::DUTY_CYCLE, "", lum));
    } else if (m[0] == "r") {
      std::string lum = m[1];
      std::string ref = m[2];
      usb_commands.push_back(LuminaireConfig(CONFIG::SET, TYPE::REF, ref, lum));
    } else if (m[0] == "g" && m[1] == "r") {
      std::string lum = m[2];
      usb_commands.push_back(LuminaireConfig(CONFIG::GET, TYPE::REF, "", lum));
      std::cout << " [INFO] : variable of type get ref" << std::endl;
    } else if (m[0] == "g" && m[1] == "l") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::ILLUMINANCE, "", lum));
    } else if (m[0] == "o") {
      std::string lum = m[1];
      std::string occ = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::SET, TYPE::OCCUPANCY, occ, lum));
    } else if (m[0] == "g" && m[1] == "o") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::OCCUPANCY, "", lum));

    } else if (m[0] == "a") {
      std::string lum = m[1];
      std::string state = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::SET, TYPE::WINDUP, state, lum));

    } else if (m[0] == "g" && m[1] == "a") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::WINDUP, "", lum));
    } else if (m[0] == "k") {
      std::string lum = m[1];
      std::string state = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::SET, TYPE::FEEDBACK, state, lum));

    } else if (m[0] == "g" && m[1] == "k") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::FEEDBACK, "", lum));

    } else if (m[0] == "g" && m[1] == "x") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::EXTERNAL_ILUMINANCE, "", lum));
    } else if (m[0] == "g" && m[1] == "p") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::INSTANTANEOUS_POWER, "", lum));
    } else if (m[0] == "g" && m[1] == "t") {
      std::string lum = m[2];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::ELAPSED_TIME, "", lum));
    } else if (m[0] == "s") {
      std::string lum = m[1];
      std::string state = m[2];
      usb_commands.push_back(StreamCmd(STREAM::START, lum, state));
    } else if (m[0] == "p") {
      std::string lum = m[1];
      std::string state = m[2];
      usb_commands.push_back(StreamCmd(STREAM::STOP, lum, state));
    } else if (m[0] == "g" && m[1] == "e") {
      std::string lum = m[2];
      usb_commands.push_back(MonitorCmd(SYSTEM_MONITOR::AVERAGE_ENERGY, lum));
    } else if (m[0] == "g" && m[1] == "v") {
      std::string lum = m[2];
      usb_commands.push_back(MonitorCmd(SYSTEM_MONITOR::VISIBILITY_ERROR, lum));
    } else if (m[0] == "g" && m[1] == "f") {
      std::string lum = m[2];
      usb_commands.push_back(MonitorCmd(SYSTEM_MONITOR::AVERAGE_FLICKER, lum));
    } else {
      this->l->log(Logger::LogLevel::ERROR, "USB message not recognized");
      return false;
    }
    return true;
  } else if (m.size() == 4) {
    if (m[0] == "g" && m[1] == "b") {
      std::string state = m[2];
      std::string lum = m[3];
      usb_commands.push_back(
          LuminaireConfig(CONFIG::GET, TYPE::STREAM_OF_VARIABLE, state, lum));
    } else {
      return false;
    }
    return true;
  }
  return false;
}
