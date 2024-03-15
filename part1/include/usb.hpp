#ifndef __USB_HPP__
#define __USB_HPP__

// File responsible for handling USB communication
#include <iostream>
#include <string>
#include <vector>

#include "pico/stdio.h"
#include "command.hpp"

#include "ThreadSafeQueue.hpp"

using msg = std::vector<std::string>;

class Reader
{
public:
    Reader(unsigned int num_max_reads, Logger *l) : num_max_reads(num_max_reads), l(l){};
    ~Reader() = default;

    void read_usb();

    std::vector<CommandInterface> process_usb_msgs();

    std::vector<msg> get_usb_msgs() { return usb_msgs; }

private:
    Logger *l;
    msg unfinished_msg;
    std::vector<msg> usb_msgs;
    unsigned int num_max_reads;
};

#endif