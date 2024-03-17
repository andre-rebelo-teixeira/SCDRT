#include "command.hpp"
#include <exception>
#include <stdexcept>
#include <iostream>

std::string LuminaireConfig::handle(PID *pid, LED *led) {

  std::cout << "[INFO] : Handling luminaire config" << std::endl;
  if (pid == nullptr || led == nullptr) {
    return "";
  }

  switch (cfg) {
  case CONFIG::SET:
    this->setters(pid, led);
    return "";
  case CONFIG::GET:
    return this->getters(pid, led);
    break;
  default:
    throw std::runtime_error("Unknown command type");
  }
  return "";
}

std::string LuminaireConfig::setters(PID *pid, LED *led) {
  float aux = 0.0;
  bool state = false;
  switch (type) {
  case TYPE::DUTY_CYCLE:
    try {
      aux = std::stof(val);
      led->set_duty_cycle(aux);
      pid->set_active(false);
      return return_ack();
    } catch (std::invalid_argument &e) {
    }
    break;
  case TYPE::REF:
    try {
      aux = std::stof(val);
      pid->set_reference(aux);
      return return_ack();

    } catch (std::invalid_argument &e) {
    }
    break;
  case TYPE::OCCUPANCY:
    try {
      state = std::stoi(val) == 1 ? true : false;
      pid->set_occupancy(state);
      return return_ack();
    } catch (std::invalid_argument &e) {
    }
    break;
  case TYPE::WINDUP:
    try {
      state = std::stoi(val) == 1 ? true : false;
      pid->set_anti_windup(state);
      return return_ack();
    } catch (std::invalid_argument &e) {
    }
    break;
  case TYPE::FEEDBACK:
    try {
      state = std::stoi(val) == 1 ? true : false;
      pid->set_ff_enable(state);
      return return_ack();
    } catch (std::invalid_argument &e) {
    }
    break;
  default:
    return return_error();
  }

  return return_error();
}

std::string LuminaireConfig::getters(PID *pid, LED *led) {
  switch (type) {
  case TYPE::DUTY_CYCLE:
    return std::to_string(led->get_duty_cycle());
  case TYPE::REF:
    return std::to_string(pid->get_reference());
  case TYPE::ILLUMINANCE:
    return std::to_string(pid->get_last_lummiance());
  case TYPE::OCCUPANCY:
    return pid->get_occupancy() ? "1" : "0";
  case TYPE::WINDUP:
    return pid->get_anti_windup() ? "1" : "0";
  case TYPE::FEEDBACK:
    return pid->get_ff_enable() ? "1" : "0";
  case TYPE::INSTANTANEOUS_POWER:
    return std::to_string(led->get_instanteous_power());
  case TYPE::EXTERNAL_ILUMINANCE:
    break;
  default:
    throw std::runtime_error("Unknown command type");
  }
}

std::string MonitorCmd::handle(PID *pid, LED *led) {
  switch (monitor) {
  case SYSTEM_MONITOR::AVERAGE_ENERGY:

    break;
  case SYSTEM_MONITOR::VISIBILITY_ERROR:
    break;
  case SYSTEM_MONITOR::AVERAGE_FLICKER:
    break;
  default:
    throw std::runtime_error("Unknown monitor type");
  }
}

std::string StreamCmd::handle(PID *pid, LED *led) {
  switch (stream) {
  case STREAM::START:
    // Start the stream
    break;
  case STREAM::STOP:
    // Stop the stream
    break;
  default:
    throw std::runtime_error("Unknown stream type");
  }
}

void command_handle(CommandInterface &cmd, PID *pid, LED *led) {
  // Command must be of the type LuminaireConfig or MonitorCmd
  cmd.handle(pid, led);
}
