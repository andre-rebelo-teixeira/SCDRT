#ifndef __CAN__HPP__
#define __CAN__HPP__

#include "mcp2515.h"
#include "can.h"

#include "hardware/flash.h" // for flasg_get_unique_id -> will be used to assign an Id to the pico that will then encode the CAN device ID N

#include <vector>
#include <variant>

enum class ERROR
{
    ERROR_OK,
    ERROR_BAD_MESSAGE,
    ERROR_UNKNOWN_MESSAGE,
    ERROR_NO_MESSAGE,
    ERROR_HARDWARE
};


class CanCom
{
public:
    CanCom(uint16_t pin, uint8_t CSn, uint8_t Tx, uint8_t RX, uint8_t SCK, uint32_t CLK_SPEED = 1000000);
    ~CanCom() = default;

    ERROR read_msg();

    ERROR send_msg(uint16_t msg_id, uint8_t dlc, std::vector<uint8_t> data);
    ERROR send_msg(uint32_t msg_ssid, uint8_t dlc, std::vector<uint8_t> data);
    ERROR send_msg(struct can_frame msg);

    /*
        TODO: maybe in the future include possibility to encode multiple signals in a single bit
        TODO: maybe use something like FCP
        TODO: Use yaml files to define message structure
    */
    bool send_msg(uint8_t dlc, std::vector<uint8_t> data);

    void set_data_available();

    inline struct can_frame get_message() {
        return msg;
    
    }

    MCP2515 can0;
private:
    uint32_t create_msg_ssid(uint16_t msg_id);
    uint16_t get_msg_id(uint32_t ssid);

    bool data_available = false;
    struct can_frame msg;


    uint16_t pico_id;

    uint16_t pin;
    uint16_t CSn;
    uint16_t Tx;
    uint16_t RX;
    uint16_t SCK;

    uint32_t CLK_SPEED;
};

inline void CanCom::set_data_available()
{
    data_available = true;
}

// ! This will be helper functions for when FCP similar is implemented
inline uint32_t CanCom::create_msg_ssid(uint16_t msg_id)
{
    return (pico_id << 16) + msg_id;
}

inline uint16_t CanCom::get_msg_id(uint32_t ssid)
{
    return ssid & 0xFFFF;
}

#endif // __CAN__HPP__