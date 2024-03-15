#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <variant>
#include "led.hpp"
#include "pid.hpp"
#include <string>

extern LED led;
extern PID pid;

enum class CONFIG
{
    SET,
    GET,
};

enum class VAL
{
    ANTI_WINDUP,
    FEEDBACK,
    DUTY_CYCLE,
    ILLMINANCE_REF,
    OCCUPANCY,
    ELAPSED_TIME,
    AVERAGE_ENERGY,
    AVERAGE_VISIBILITY_ERROR,
    AVERAGE_FLICKER

};

enum class SYSTEM_MONITOR
{
    ILLUMINANCE,
    INSTANTANEOUS_POWER,
    EXTERNAL_ILLUMINANCE,
};

class CommandInterface
{
public:
    virtual ~CommandInterface() = 0;
    virtual void handle() = 0;

protected:
    char var;
};

class MonitorCmd : public CommandInterface
{
public:
    MonitorCmd(SYSTEM_MONITOR monitor) : CommandInterface(), monitor(monitor)
    {
    }
    void handle() override;

private:
    SYSTEM_MONITOR monitor;
};

class LuminaireConfig : public CommandInterface
{
public:
    LuminaireConfig(CONFIG type, VAL val, std::string lum, std::string param) : CommandInterface(), type(type), val(val), lum(lum), param(param)
    {
    }

    LuminaireConfig(CONFIG type, VAL val, std::string lum) : CommandInterface(), type(type), val(val), lum(lum)
    {
    }

    void handle() override;

private:
    CONFIG type;
    VAL val;
    std::string lum = "";
    std::string param = "";

    // functions
    void setter();
    void getter();
};

void command_handle(CommandInterface &cmd);

#endif // __COMMAND_HPP__