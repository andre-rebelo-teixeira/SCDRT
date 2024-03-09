#ifndef __COMMAND_HPP__
#define __COMMAND_HPP__

#include <variant>

enum class CONFIG
{
    SET,
    GET
};

enum class VAL
{
    DUTY_CYCLE,
    ILLUMINANCE_REF,
    OCCUPANCY,
    ANTI_WINDUP,
    FEEDBACK,

};

enum class SYSTEM_MONITOR
{
    EXTERNAL_ILLUMINANCE,
    INSTANTANEOUS_POWER,
    TIME_SINCE_RESTART,
    AVERAGE_ENERGY,
    VISIBILITY_ERROR,
    AVERAGE_FLICKER
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
    MonitorCmd(SYSTEM_MONITOR monitor) : CommandInterface(), monitor(monitor) {}
    void handle() override;

private:
    SYSTEM_MONITOR monitor;
};

class LuminaireConfig : public CommandInterface
{
public:
    LuminaireConfig(CONFIG type, VAL val) : CommandInterface(), type(type), val(val)
    {
    }

    void handle() override;

private:
    CONFIG type;
    VAL val;
};

void command_handle(CommandInterface &cmd);

#endif // __COMMAND_HPP__