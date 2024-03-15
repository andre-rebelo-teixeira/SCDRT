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

    static void read_usb_task(void *pvParameter);

    std::vector<CommandInterface> process_usb_msg(msg);

    std::vector<msg> get_usb_msgs() { return usb_msgs; }

private:
    void read_usb();
    Logger *l;
    msg unfinished_msg;
    std::vector<msg> usb_msgs;
    unsigned int num_max_reads;
};

#endif