#include "scdtr_interface.h"
#include "ui_scdtr_interface.h"

#include <json.hpp>

using boost::asio::io_service;
using boost::asio::serial_port;
using nlohmann::json;

SCDTR_Interface::SCDTR_Interface(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::SCDTR_Interface)
{
    ui->setupUi(this);

    this->serial = new boost::asio::serial_port(this->io);

    // Connect to USB
    if (connect_to_usb("/dev/ttyACM0")) {
        // Start the first asynchronous read
        startAsyncRead();
    } else {
        std::cerr << "Failed to connect to USB." << std::endl;
        // Handle the failure to connect as needed
    }

    this->ui->console->setMaximumBlockCount(200);


    // poll io to get USB messages
    this->read_timer = new QTimer();
    connect (this->read_timer, &QTimer::timeout, this, &SCDTR_Interface::io_poll);
    this->read_timer->start(std::chrono::milliseconds(10));    this->presence_timer = new QTimer();

    this->presence_timer = new QTimer();
    connect(this->presence_timer, &QTimer::timeout, this, &SCDTR_Interface::send_presence);


}

SCDTR_Interface::~SCDTR_Interface()
{
    io.stop();
    delete ui;
}

// private functions

void SCDTR_Interface::send_presence() {
    auto clock = QDateTime::currentDateTime();

    json j;
    j["presence"] = clock.toMSecsSinceEpoch();

    std::cout << "this->clock->toMSecsSinceEpoch " << clock.toMSecsSinceEpoch() << std::endl;

    std::string send_string =  j.dump();

    this->_udpSocket.write((const char *)send_string.c_str(), strlen(send_string.c_str()));
}

void SCDTR_Interface::io_poll() {
    this->io.poll_one();
}

void SCDTR_Interface::startAsyncRead() {
   // async_read(*serial, boost::asio::buffer(data), std::bind(&SCDTR_Interface::handleRead, this, std::placeholders::_1, std::placeholders::_2, data));

    serial->async_read_some(buffer(data, sizeof(data)), std::bind(&SCDTR_Interface::handleRead, this, std::placeholders::_1, std::placeholders::_2, data));
}

void SCDTR_Interface::handleRead(const error_code& error, std::size_t bytes_transferred, char* data) {

    if (!error) {
        // Handle the received data

        std::cout << this->ui->console->blockCount() << std::endl;

        QString data_ = QString(data);

        for (auto const ch: data_){
            if (ch != '\n') {
                last_message.append(ch);
            } else {
                message_vector.append(last_message);
                last_message.clear();
            }
        }

        if (this->show_messages) {
            for (auto const & msg : message_vector) {
                this->ui->console->insertPlainText(msg);
            }
        }

        message_vector.clear();


        // Set up the next asynchronous read
        this->startAsyncRead();
    } else {
        std::cerr << "Error reading data: " << error.message() << std::endl;
    }
}

bool SCDTR_Interface::connect_to_usb(QString dev_name) {
    try {
        serial = new serial_port(io, dev_name.toStdString());
        serial->set_option(serial_port::baud_rate(115200)); // Adjust baud rate as needed
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Error connecting to USB: " << e.what() << std::endl;
        return false;
    }
}

// private slots
void SCDTR_Interface::on_messages_check_box_clicked(bool checked)
{
    this->show_messages = checked;
}

void SCDTR_Interface::on_plotjuggler_check_box_clicked(bool checked)
{
    this->connect_to_plotjuggler = checked;
    if (checked == true) {
        // send presence message to usp socket
        this->presence_timer->start(std::chrono::milliseconds(1000));

        // connect to plotjuggler
        QString ip_ = "127.0.0.1";
        QString port_ = "9870";

        this->_udpSocket.connectToHost(ip_, port_.toUShort());
    } else  {
        this->presence_timer->stop();
        this->_udpSocket.disconnectFromHost();
    }

}

void SCDTR_Interface::on_send_command_clicked()
{

}
