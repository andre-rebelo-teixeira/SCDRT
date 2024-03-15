#include "usb.hpp"
#include <algorithm>
#include <numeric>

#include "logger.hpp"
#include <iostream>


void Reader::read_usb_task(void *pvParameter){
  Reader* reader = static_cast<Reader *> (pvParameter);
  reader->read_usb();
}

void Reader::read_usb()
{
    char c;

    unsigned int num_reads = 0;
    bool is_reading = true;
    while (num_max_reads < num_max_reads && is_reading)
    {
        int read_char = getchar_timeout_us(0);

        if (read_char == PICO_ERROR_TIMEOUT)
        {
            is_reading = false;
            continue;
        }

        switch (static_cast<char>(read_char))
        {
        case ' ':
            // push empty string to the unfinished_msgs
            this->unfinished_msg.push_back("");
            break;

        case '\n':
            this->usb_msgs.push_back(this->unfinished_msg);
            this->unfinished_msg.clear();
            break;

        default:
            this->unfinished_msg.back().push_back(static_cast<char>(read_char));
            break;
        }
    }
}
/*
std::vector<CommandInterface> Reader::process_usb_msgs()
{
    std::vector<CommandInterface> cmds_;

    for (auto &msg : usb_msgs)
    {
        CommandInterface *cmd_;
        if (msg.size() != 3 || msg.size() != 4)
        {
            this->l->log(Logger::LogLevel::ERROR, "Invalid message size");
        }

        if (msg[0] == "d")
        {
            // only command that starts with d has 3 elements and is setting duty cycle
            if (msg.size() != 3)
            {
                this->l->log(Logger::LogLevel::ERROR, "Invalid message size for setting duty cycle");
            }

            std::string lum = msg[1];
            std::string duty_cycle = msg[2];

            cmd_ = new LuminaireConfig(CONFIG::SET, VAL::DUTY_CYCLE, lum, duty_cycle);
        }
        else if (msg[0] == "g")
        {
            if (msg.size() == 3)
            {
                if (msg[1] == "d")
                {
                    // get current duty cycle
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::DUTY_CYCLE, lum);
                }
                else if (msg[1] == "r")
                {
                    // get illuminance reference
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::ILLMINANCE_REF, lum);
                }
                else if (msg[1] == "l")
                {
                    std::string lum = msg[2];
                    cmd_ = new MonitorCmd(SYSTEM_MONITOR::ILLUMINANCE);
                    // measure the illumincance
                }
                else if (msg[1] == "o")
                {
                    // get current occupancy
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::OCCUPANCY, lum);
                }
                else if (msg[1] == "a")
                {
                    // get anti windup
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::ANTI_WINDUP, lum);
                }
                else if (msg[1] == "k")
                {
                    // get feedback state
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::FEEDBACK, lum);
                }
                else if (msg[1] == "x")
                {
                    // get external illuminance
                    std::string lum = msg[2];
                    cmd_ = new MonitorCmd(SYSTEM_MONITOR::EXTERNAL_ILLUMINANCE);
                }
                else if (msg[1] == "p")
                {
                    // get instantaneous power
                    std::string lum = msg[2];
                    cmd_ = new MonitorCmd(SYSTEM_MONITOR::INSTANTANEOUS_POWER);
                }
                else if (msg[1] == "t")
                {
                    // get time since restart
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::ELAPSED_TIME, lum);
                }
                else if (msg[1] == "e")
                {
                    // get average energy since last restart
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::AVERAGE_ENERGY, lum);
                }
                else if (msg[1] == "v")
                {
                    // get visibility error
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::AVERAGE_VISIBILITY_ERROR, lum);
                }
                else if (msg[1] == "f")
                {
                    // get average flicker
                    std::string lum = msg[2];
                    cmd_ = new LuminaireConfig(CONFIG::GET, VAL::AVERAGE_FLICKER, lum);
                }
            }
            else if (msg.size() == 4)
            {
                if (msg[1] == "b")
                {
                    // get last minute buffer of a variable in a desk
                    // ! TODO -> implement buffer variables
                }
            }
        }
        else if (msg[0] == "r")
        {
            if (msg.size() != 3)
            {
                this->l->log(Logger::LogLevel::ERROR, "Invalid message size for setting reference");
            }

            std::string lum = msg[1];
            std::string reference = msg[2];

            cmd_ = new LuminaireConfig(CONFIG::SET, VAL::ILLMINANCE_REF, lum, reference);
        }
        else if (msg[0] == "o")
        {
            // set occupancy

            if (msg.size() != 3)
            {
                this->l->log(Logger::LogLevel::ERROR, "Invalid message size for setting occupancy");
            }

            std::string lum = msg[1];
            std::string occupancy = msg[2];

            cmd_ = new LuminaireConfig(CONFIG::SET, VAL::OCCUPANCY, lum, occupancy);
        }
        else if (msg[0] == "a")
        {
            // set anti windup of desk
            if (msg.size() != 3)
            {
                this->l->log(Logger::LogLevel::ERROR, "Invalid message size for setting anti windup");
            }

            std::string lum = msg[1];
            std::string anti_windup = msg[2];

            cmd_ = new LuminaireConfig(CONFIG::SET, VAL::ANTI_WINDUP, lum, anti_windup);
        }
        else if (msg[0] == "k")
        {
            // set feedback of desk
            if (msg.size() != 3)
            {
                this->l->log(Logger::LogLevel::ERROR, "Invalid message size for setting feedback");
            }

            std::string lum = msg[1];
            std::string feedback = msg[2];

            cmd_ = new LuminaireConfig(CONFIG::SET, VAL::FEEDBACK, lum, feedback);
        }
        else if (msg[0] == "s")
        {
            // implement stram varibales
            // ! TODO
        }
        else if (msg[0] == "S")
        {
            // implement stram variables
            // ! TODO
        }
        else
        {
            this->l->log(Logger::LogLevel::ERROR, "Invalid command type");
        }

        cmds_.push_back(*cmd_);
    }

    return cmds_;
}
*/