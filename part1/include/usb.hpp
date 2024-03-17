#ifndef __USB_HPP__
#define __USB_HPP__

// File responsible for handling USB communication
#include <iostream>
#include <string>
#include <vector>
#include <memory>

#include "pico/stdio.h"

#include "command.hpp"
#include "ThreadSafeQueue.hpp"

// * Typedefs for this file
using msg = std::vector<std::string>;

/**
 * @brief Class to handle USB communication
*/
class UsbReader
{
public:
    
    /**
     * @brief Construct a new UsbReader object
     * 
     * @param num_max_reads max number of times we will try to read from the USB fin each itteration
     * @param l pointer to the logger object
     * 
     * @return USBReader object
    */
    inline UsbReader(unsigned int num_max_reads, Logger *l) : 
        num_max_reads(num_max_reads), l(l) 
    {
        unfinished_msg.push_back("");
    };

    /**
     * @brief Destroy the UsbReader object
    */
    ~UsbReader() = default;

    /**
     * @brief Read from the USB
    */
    void read_usb();

    /**
     * @brief Get the vector of usb commands
     * 
     * @return std::vector<CommandInterface> vector of commands to be executed
    */
    inline std::vector <std::shared_ptr<CommandInterface >> get_usb_commands() {
        return usb_commands;
    }


    /**
     * @brief Clear the vector of usb commands
     * 
     * @note this function is used to clear the vector of usb commands after they have been executed
    */
    inline void clear_usb_commands() {
        usb_commands.clear();
    }

private:
    Logger *l;
    msg unfinished_msg;
    unsigned int num_max_reads;
    std::vector<std::shared_ptr<CommandInterface>>  usb_commands;


    /**
     * @brief Process the messages from the USB
     *  
     * @param m vector of strings representing the message
     * 
     * @note if the message is not processed successfully, the function will log an error
     * @note if the message if not processed succefully, the message will be erased 
     *  
     * @return bool true if the message was processed successfully
     * @return bool false if the message was not processed successfully 
    */
    bool process_usb_msg(msg m);
};

#endif