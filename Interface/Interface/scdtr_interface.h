#ifndef SCDTR_INTERFACE_H
#define SCDTR_INTERFACE_H

#include <json.hpp>

#include <chrono>
#include <iostream>
#include <boost/asio.hpp>

#include <QMap>
#include <QPair>
#include <QFile>
#include <QTime>
#include <QTimer>
#include <QString>
#include <QVector>
#include <QDateTime>
#include <QTextCodec>
#include <QMainWindow>
#include <QStringList>
#include <QSerialPort>
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

    void on_command_in_currentTextChanged(const QString &arg1);

    void on_send_cmd_released();

    void on_serial_port_connect_clicked(bool checked);

private:

    // functions
    void handleRead(QString msg_data);

    void sendToPlotjuggler(QString msg);

    void send_presence();

    void initialize_command_in();
    void read();

    void save_to_array(QString msg);

    void write_to_csv();
    // variables

    QTimer *read_timer;
    QTimer *presence_timer;

    QSerialPort * serial_port;
    QVector <QString> message_vector;
    QString last_message;

    QUdpSocket _udpSocket;

    Ui::SCDTR_Interface *ui;
    bool connect_to_plotjuggler = false;
    bool show_messages = true;

    QMap <QString, QString> desired_commands;

    qint64 start_time;

    QMap <QString, QVector <QPair <double, qint64>>> signal_map;

};
#endif // SCDTR_INTERFACE_H
