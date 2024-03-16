#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include "led.hpp"
#include "pid.hpp"
#include <string>

enum class CONFIG { SET, GET };

enum class TYPE {
  DUTY_CYCLE,
  REF,
  ILLUMINANCE,
  OCCUPANCY,
  WINDUP,
  FEEDBACK,
  EXTERNAL_ILUMINANCE,
  INSTANTANEOUS_POWER,
  ELAPSED_TIME,
  STREAM_OF_VARIABLE
};

enum class SYSTEM_MONITOR { AVERAGE_ENERGY, VISIBILITY_ERROR, AVERAGE_FLICKER };

enum class STREAM {
  START,
  STOP,
};

class CommandInterface {
public:
  CommandInterface() = default;
  ~CommandInterface() = default;
  virtual std::string handle(PID *pid, LED *led){};
};

class MonitorCmd : public CommandInterface {
public:
  MonitorCmd(SYSTEM_MONITOR monitor, std::string lum)
      : CommandInterface(), monitor(monitor), lum(lum) {}
  std::string handle(PID *pid, LED *led) override;

private:
  SYSTEM_MONITOR monitor;
  std::string lum;
};

class LuminaireConfig : public CommandInterface {
public:
  LuminaireConfig(CONFIG cfg, TYPE type, std::string val, std::string lum)
      : CommandInterface(), cfg(cfg), type(type), val(val), lum(lum) {}

  std::string handle(PID *pid, LED *led) override;

private:
  CONFIG cfg;
  TYPE type;
  std::string val;
  std::string lum;

  // private functions
  std::string setters(PID *pid, LED *led);
  std::string getters(PID *pid, LED *led);
};

class StreamCmd : public CommandInterface {
public:
  StreamCmd(STREAM stream, std::string stream_name, std::string lum)
      : CommandInterface(), stream(stream), stream_name(stream_name), lum(lum) {
  }

  ~StreamCmd() = default;

  std::string handle(PID *pid, LED *led) override;

private:
  STREAM stream;
  std::string stream_name;
  std::string lum;
};

inline std::string return_error() { return "err"; }

inline std::string return_ack() { return "ack"; }

void command_handle(CommandInterface &cmd);

#endif // __COMMAND_HPP__