#include "scdtr_interface.h"
#include "ui_scdtr_interface.h"

#include <json.hpp>

SCDTR_Interface::SCDTR_Interface(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SCDTR_Interface)
{
    ui->setupUi(this);

    this->serial = new boost::asio::serial_port(this->io);

    // Connect to USB
    if (connect_to_usb("/dev/ttyACM0"))
    {
        // Start the first asynchronous read
        startAsyncRead();
    }
    else
    {
        std::cerr << "Failed to connect to USB." << std::endl;
        // Handle the failure to connect as needed
    }

    this->ui->luminaire_line_edit->setText("<empty>");
    this->ui->extra_arg_line_edit->setText("<empty>");

    this->ui->console->setReadOnly(true);
    this->ui->console->document()->setMaximumBlockCount(200);

    this->ui->command_out->setReadOnly(true);
    this->ui->command_out->setMaximumBlockCount(200);

    // poll io to get USB messages
    this->read_timer = new QTimer();
    connect(this->read_timer, &QTimer::timeout, this, &SCDTR_Interface::io_poll);
    this->read_timer->start(std::chrono::milliseconds(100));
    this->presence_timer = new QTimer();

    this->presence_timer = new QTimer();
    connect(this->presence_timer, &QTimer::timeout, this, &SCDTR_Interface::send_presence);

    this->initialize_command_in();
}

SCDTR_Interface::~SCDTR_Interface()
{
    io.stop();
    delete ui;
}

// private functions

void SCDTR_Interface::initialize_command_in() {
    this->desired_commands["d <i> <vaL>"] = "Set directly the duty cycle of luminaire i";
    this->desired_commands["g d <i>"] = "Get current duty cycle of luminaire i";
    this->desired_commands["r <i> <val>"] = "Set the illuminance reference of luminaire i";
    this->desired_commands["g r <i>"] = "Get current illuminance reference of luminaire i";
    this->desired_commands["g l <i>"] = "Measure the illuminance of luminaire i";
    this->desired_commands["o <i> <val>"]= "Set the current occupancy state of desk <i>";
    this->desired_commands["g o <i>"]= "Get the current occupancy statE of desk <i>";
    this->desired_commands["a <i> <val>"]= "Set anti-windup state of desk <i>";
    this->desired_commands["g a <i>"]= "Get anti-windup state of desk <i>";
    this->desired_commands["k <i> <val>"]= "Set feedback on/off of desk <i>";
    this->desired_commands["g k <i>"]= "Get feedback state of desk <i>";
    this->desired_commands["g x <i>"]= "Get current external illuminance of desk <i>";
    this->desired_commands["g p <i>"]= "Get instantaneous power consumption of desk <i>";
    this->desired_commands["g t <i>"]= "Get the elapsed time since the last restart";
    this->desired_commands["s <x> <i>"]= "Start the stream of the real-time variable <x> of desk <i>. <x> can be 'l' or 'd'.";
    this->desired_commands["S <x> <i>"] = "Stop the stream of the real-time variable <x> of desk <i>. <x> can be 'l' or 'd'.";
    this->desired_commands["g b <x> <i>"]= "Get the last minute buffer of the variable <x> of the desk <i>. <x> can be 'l' or 'd'.";

    this->desired_commands["g e <i>"] = "Get the average energy consumption at the desk <i> since the last system restart.";
    this->desired_commands["g v <i>"] = "Get the average visibility error at desk <i> since the last system restart";
    this->desired_commands["g f <i>" ] = "Get the average flicker error on desk <i> since the last system restart.";

    auto keys = this->desired_commands.keys();
    for (auto const & com : keys) {
        this->ui->command_in->addItem(com);
    }
}

void SCDTR_Interface::send_presence()
{
    auto clock = QDateTime::currentDateTime();

    json j;
    j["presence"] = clock.toMSecsSinceEpoch();

    std::string send_string = j.dump();

    this->_udpSocket.write((const char *)send_string.c_str(), strlen(send_string.c_str()));
}

void SCDTR_Interface::io_poll()
{
    this->io.poll_one();
}

void SCDTR_Interface::startAsyncRead()
{
    // async_read(*serial, boost::asio::buffer(data), std::bind(&SCDTR_Interface::handleRead, this, std::placeholders::_1, std::placeholders::_2, data));

    serial->async_read_some(buffer(data, sizeof(data)), std::bind(&SCDTR_Interface::handleRead, this, std::placeholders::_1, std::placeholders::_2, data));
}

void SCDTR_Interface::handleRead(const error_code &error, std::size_t bytes_transferred, char *data)
{
    QTextCharFormat format_;
    if (!error)
    {
        // Handle the received data

        QString data_ = QString(data);

        // std::cout << data << std::endl;

        for (auto const ch : data_)
        {
            if (ch != '\n')
            {
                last_message.append(ch);
            }
            else
            {
                message_vector.append(last_message);
                last_message.clear();
            }
        }

        if (this->show_messages)
        {
            for (auto const &msg : message_vector)
            {

                this->ui->console->moveCursor(QTextCursor::End);
                if (msg.contains("[INFO]"))
                {
                    format_.setForeground(Qt::gray);

                    this->ui->console->setCurrentCharFormat(format_);
                    this->ui->console->append(msg);

                    if (this->connect_to_plotjuggler)
                    {
                        this->sendToPlotjuggler(QTextCodec::codecForName("UTF-8")->toUnicode(msg.toUtf8()));
                    }

                    // this->ui->console>
                }
                else if (msg.contains("[WARNING]"))
                {
                    format_.setForeground(Qt::yellow);

                    this->ui->console->setCurrentCharFormat(format_);
                    this->ui->console->append(msg);
                }
                else if (msg.contains("[ERROR]"))
                {
                    format_.setForeground(Qt::red);

                    this->ui->console->setCurrentCharFormat(format_);
                    this->ui->console->append(msg);
                }
                else if(msg.contains("[RESPONSE]")) {
                    this->ui->command_out->appendPlainText(msg);
                }
            }

        }

        message_vector.clear();

        // Set up the next asynchronous read
        this->startAsyncRead();
    }
    else
    {
        std::cerr << "Error reading data: " << error.message() << std::endl;
    }
}

void SCDTR_Interface::sendToPlotjuggler(QString msg)
{
    QStringList split_string = msg.split(":");
    auto clock = QDateTime::currentDateTime();
    json j;

    if (split_string.size() != 3)
    {
        return;
    }

    QString signal_name = split_string[1];
    QString signal_value = split_string[2];

    j[signal_name.toUtf8().constData()] = signal_value.toUtf8().toDouble();
    j["timestamp"] = clock.toMSecsSinceEpoch();

    std::string dic = j.dump();

    this->_udpSocket.write( (const char *)dic.c_str(), strlen(dic.c_str()));
}

bool SCDTR_Interface::connect_to_usb(QString dev_name)
{
    try
    {
        serial = new serial_port(io, dev_name.toStdString());
        serial->set_option(serial_port::baud_rate(115200)); // Adjust baud rate as needed
        return true;
    }
    catch (const std::exception &e)
    {
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
    if (checked == true)
    {
        // send presence message to usp socket
        this->presence_timer->start(std::chrono::milliseconds(1000));

        // connect to plotjuggler
        QString ip_ = "127.0.0.1";
        QString port_ = "9870";

        this->_udpSocket.connectToHost(ip_, port_.toUShort());
    }
    else
    {
        this->presence_timer->stop();
        this->_udpSocket.disconnectFromHost();
    }
}

void SCDTR_Interface::on_send_command_clicked()
{
    QString com = this->ui->command_in->currentText();
    QString lum = this->ui->luminaire_line_edit->text();
    QString arg = this->ui->extra_arg_line_edit->text();

    if (lum == "<empty>") {
        std::cout << "what command will the luminaire go to?" << std::endl;
        return;
    }
    if((com.contains("<val>") || com.contains("<x>"))
        && arg == "<empty>") {
        std::cout << "Missing argument, not sending command " << std::endl;
        return;
    }

    if (com.contains("<i>")) {
        com.replace("<i>", lum);
    }

    if(com.contains("<val>")) {
        com.replace("<val>", arg);
    }

    if (com.contains("<x>") && arg != "empyy") {
        com.replace("<x>", arg);
    }

    com += '\0';

    serial->write_some(buffer(com.toStdString(), com.toStdString().size()));
    std::cout << com.toStdString() << std::endl;

}

void SCDTR_Interface::on_command_in_currentTextChanged(const QString &arg1)
{
    this->ui->comment_label->setText(this->desired_commands[arg1]);
}

