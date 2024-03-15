#include "command.hpp"
#include <exception>
#include <stdexcept>

void LuminaireConfig::handle()
{
  /*
    switch (type)
    {
    case CONFIG::SET:
        // Set the value
        break;
    case CONFIG::GET:
        // Get the value
        break;
    default:
        break;
    }*/
}

void MonitorCmd::handle()
{
  /*P
    switch (monitor)
    {
    case SYSTEM_MONITOR::EXTERNAL_ILLUMINANCE:
        // Get the external illuminance
        break;
    case SYSTEM_MONITOR::INSTANTANEOUS_POWER:
        // Get the instantaneous power
        break;
    case SYSTEM_MONITOR::TIME_SINCE_RESTART:
        // Get the time since restart
        break;
    case SYSTEM_MONITOR::AVERAGE_ENERGY:
        // Get the average energy
        break;
    case SYSTEM_MONITOR::VISIBILITY_ERROR:
        // Get the visibility error
        break;
    case SYSTEM_MONITOR::AVERAGE_FLICKER:
        // Get the average flicker
        break;
    default:
        break; 
    }*/
}

void command_handle(CommandInterface &cmd)
{
    // Command must be of the type LuminaireConfig or MonitorCmd
    cmd.handle();
}
