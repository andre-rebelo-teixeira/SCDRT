#include <iostream>
#include <boost/asio.hpp>

// finish interface
int main(int argc, char **argv)
{
    boost::asio::io_service io;
    boost::asio::serial_port port(io, "/dev/ttyACM0");

    // Set serial port options
    port.set_option(boost ::asio::serial_port_base::baud_rate(9600));
    port.set_option(boost ::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    port.set_option(boost ::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    port.set_option(boost ::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));

    while (true)
    {
        // Simple read example
        char read_buffer[128];
        size_t bytes_read = boost::asio::read(port, boost::asio::buffer(read_buffer));

        std::cout << "Received: " << std::string(read_buffer, bytes_read) << std::endl;
    }
}
