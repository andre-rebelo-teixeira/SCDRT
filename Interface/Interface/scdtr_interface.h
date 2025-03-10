#ifndef SCDTR_INTERFACE_H
#define SCDTR_INTERFACE_H

#include <json.hpp>

#include <chrono>
#include <iostream>
#include <boost/asio.hpp>

#include <QMap>
#include <QTime>
#include <QTimer>
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QTextCodec>
#include <QMainWindow>
#include <QStringList>
#include <QTextCharFormat>


#include <QUdpSocket>
#include <QHostAddress>

using boost::asio::buffer;
using boost::asio::io_service;
using boost::asio::serial_port;
using boost::system::error_code;

using nlohmann::json;
using boost::asio::io_service;
using boost::asio::serial_port;


QT_BEGIN_NAMESPACE
namespace Ui {
class SCDTR_Interface;
}
QT_END_NAMESPACE

class SCDTR_Interface : public QMainWindow
{
    Q_OBJECT

public:
    SCDTR_Interface(QWidget *parent = nullptr);
    ~SCDTR_Interface();

private slots:
    void on_messages_check_box_clicked(bool checked);

    void on_plotjuggler_check_box_clicked(bool checked);

    void on_send_command_clicked();

    void on_command_in_currentTextChanged(const QString &arg1);

private:

    // functions
    void handleRead(const error_code& error,
                    std::size_t bytes_transferred,
                    char* data);

    bool connect_to_usb(QString dev_name );
    void startAsyncRead();

    void sendToPlotjuggler(QString msg);

    void io_poll();
    void send_presence();

    void initialize_command_in();

    // variables

    QTimer *read_timer;
    QTimer *presence_timer;
    boost::asio::io_service io;
    boost::asio::serial_port *serial;

    QVector <QString> message_vector;
    QString last_message;

    QUdpSocket _udpSocket;

    Ui::SCDTR_Interface *ui;
    bool connect_to_plotjuggler = false;
    bool show_messages = true;

    QMap <QString, QString> desired_commands;

    char data[1024]; // Buffer to hold the incoming data

};
#endif // SCDTR_INTERFACE_H
