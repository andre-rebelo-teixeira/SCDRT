#include "scdtr_interface.h"
#include "ui_scdtr_interface.h"

#include <json.hpp>

SCDTR_Interface::SCDTR_Interface(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::SCDTR_Interface) {
  ui->setupUi(this);

  serial_port = new QSerialPort();   // this->ui->console->setReadOnly(true);
  this->ui->console->document()->setMaximumBlockCount(200);

  this->ui->command_out->setReadOnly(true);
  this->ui->command_out->setMaximumBlockCount(200);

  this->presence_timer = new QTimer();
  connect(this->presence_timer, &QTimer::timeout, this,
          &SCDTR_Interface::send_presence);

  this->initialize_command_in();
}

SCDTR_Interface::~SCDTR_Interface() {
    write_to_csv();
    delete ui;
}

// private functions

void SCDTR_Interface::write_to_csv() {

    auto clock = QDateTime::currentDateTime();
    QString date = clock.date().toString();

    foreach (auto const & sig_name , signal_map.keys()) {
        auto values = signal_map[sig_name];

        QFile file (date + sig_name +  ".csv");
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            std::cout << "Error creating file for " << sig_name.toStdString() << std::endl;
            continue;
        }

        QTextStream f_(&file);

        f_ << "timestamp , " << sig_name << '\n';

        foreach(auto const & val , values) {
            f_ << val.first << "," << val.second << '\n';
        }

        file.close();

    }
}

void SCDTR_Interface::initialize_command_in() {
    this->ui->serial_input->addItem("ttyACM0");
    this->ui->serial_input->addItem("ttyACM1");
    this->ui->serial_input->addItem("ttyACM2");
    this->ui->serial_input->addItem("ttyACM3");
    this->ui->serial_input->addItem("ttyACM4");




  this->desired_commands["d <i> <val>"] =
      "Set directly the duty cycle of luminaire i";
  this->desired_commands["g d <i>"] = "Get current duty cycle of luminaire i";
  this->desired_commands["r <i> <val>"] =
      "Set the illuminance reference of luminaire i";
  this->desired_commands["g r <i>"] =
      "Get current illuminance reference of luminaire i";
  this->desired_commands["g l <i>"] = "Measure the illuminance of luminaire i";
  this->desired_commands["o <i> <val>"] =
      "Set the current occupancy state of desk <i>";
  this->desired_commands["g o <i>"] =
      "Get the current occupancy statE of desk <i>";
  this->desired_commands["a <i> <val>"] = "Set anti-windup state of desk <i>";
  this->desired_commands["g a <i>"] = "Get anti-windup state of desk <i>";
  this->desired_commands["k <i> <val>"] = "Set feedback on/off of desk <i>";
  this->desired_commands["g k <i>"] = "Get feedback state of desk <i>";
  this->desired_commands["g x <i>"] =
      "Get current external illuminance of desk <i>";
  this->desired_commands["g p <i>"] =
      "Get instantaneous power consumption of desk <i>";
  this->desired_commands["g t <i>"] =
      "Get the elapsed time since the last restart";
  this->desired_commands["s <x> <i>"] =
      "Start the stream of the real-time variable <x> of desk <i>. <x> can be "
      "'l' or 'd'.";
  this->desired_commands["S <x> <i>"] =
      "Stop the stream of the real-time variable <x> of desk <i>. <x> can be "
      "'l' or 'd'.";
  this->desired_commands["g b <x> <i>"] =
      "Get the last minute buffer of the variable <x> of the desk <i>. <x> can "
      "be 'l' or 'd'.";

  this->desired_commands["g e <i>"] =
      "Get the average energy consumption at the desk <i> since the last "
      "system restart.";
  this->desired_commands["g v <i>"] = "Get the average visibility error at "
                                      "desk <i> since the last system restart";
  this->desired_commands["g f <i>"] = "Get the average flicker error on desk "
                                      "<i> since the last system restart.";

  auto keys = this->desired_commands.keys();
  for (auto const &com : keys) {
    this->ui->command_in->addItem(com);
  }
}

void SCDTR_Interface::send_presence() {
  auto clock = QDateTime::currentDateTime();

  json j;
  j["presence"] = clock.toMSecsSinceEpoch();

  std::string send_string = j.dump();

  this->_udpSocket.write((const char *)send_string.c_str(),
                         strlen(send_string.c_str()));
}

void SCDTR_Interface::handleRead(QString msg_data) {
  QTextCharFormat format_;

  for (auto const ch : msg_data) {
    if (ch != '\n') {
      last_message.append(ch);
    } else {
      message_vector.append(last_message);
      last_message.clear();
    }
  }

  if (this->show_messages) {
    for (auto const &msg : message_vector) {

      this->ui->console->moveCursor(QTextCursor::End);
      if (msg.contains("[INFO]")) {
        format_.setForeground(Qt::gray);

        this->ui->console->setCurrentCharFormat(format_);
        this->ui->console->append(msg);

        if (this->connect_to_plotjuggler) {
          this->sendToPlotjuggler(
              QTextCodec::codecForName("UTF-8")->toUnicode(msg.toUtf8()));
        }

        this->save_to_array(msg);

        // this->ui->console>
      } else if (msg.contains("[WARNING]")) {
        format_.setForeground(Qt::yellow);

        this->ui->console->setCurrentCharFormat(format_);
        this->ui->console->append(msg);
      } else if (msg.contains("[ERROR]")) {
        format_.setForeground(Qt::red);

        this->ui->console->setCurrentCharFormat(format_);
        this->ui->console->append(msg);
      } else if (msg.contains("[RESPONSE]")) {
        this->ui->command_out->appendPlainText(msg);
      }
      else if (msg.contains("[VALUE]")) {
        format_.setForeground(Qt::green);
        if (this->connect_to_plotjuggler) {
          this->sendToPlotjuggler(
              QTextCodec::codecForName("UTF-8")->toUnicode(msg.toUtf8()));
        }
      }
    }
  }

  message_vector.clear();
}

void SCDTR_Interface::sendToPlotjuggler(QString msg) {
  QStringList split_string = msg.split(":");
  auto clock = QDateTime::currentDateTime();
  json j;

  if (split_string.size() != 3) {
    return;
  }

  QString signal_name = split_string[1];
  QString signal_value = split_string[2];

  j[signal_name.toUtf8().constData()] = signal_value.toUtf8().toDouble();
  j["timestamp"] = clock.toMSecsSinceEpoch();

  std::string dic = j.dump();

  this->_udpSocket.write((const char *)dic.c_str(), strlen(dic.c_str()));
}

// private slots
void SCDTR_Interface::on_messages_check_box_clicked(bool checked) {
  this->show_messages = checked;
}

void SCDTR_Interface::on_plotjuggler_check_box_clicked(bool checked) {
  this->connect_to_plotjuggler = checked;
  if (checked == true) {
    // send presence message to usp socket
    this->presence_timer->start(std::chrono::milliseconds(1000));

    // connect to plotjuggler
    QString ip_ = "127.0.0.1";
    QString port_ = "9870";

    this->_udpSocket.connectToHost(ip_, port_.toUShort());
  } else {
    this->presence_timer->stop();
    this->_udpSocket.disconnectFromHost();
  }
}

void SCDTR_Interface::on_send_cmd_released() {
  QString com = this->ui->command_in->currentText();
  QString lum = this->ui->luminaire_line_edit->text();
  QString arg = this->ui->extra_arg_line_edit->text();

  if (lum == "<empty>") {
    std::cout << "what command will the luminaire go to?" << std::endl;
    return;
  }
  if ((com.contains("<val>") || com.contains("<x>")) && arg == "<empty>") {
    std::cout << "Missing argument, not sending command " << std::endl;
    return;
  }

  if (com.contains("<i>")) {
    com.replace("<i>", lum);
  }

  if (com.contains("<val>")) {
    com.replace("<val>", arg);
  }

  if (com.contains("<x>") && arg != "empty") {
    com.replace("<x>", arg);
  }

  com += '\n';

  auto msg_for_serial_port = com.toStdString().c_str();
  serial_port->write(msg_for_serial_port);

  std::cout << com.toStdString() << " " << com.toStdString().size()
            << std::endl;
}

void SCDTR_Interface::on_command_in_currentTextChanged(const QString &arg1) {
  this->ui->comment_label->setText(this->desired_commands[arg1]);
}

void SCDTR_Interface::read() {
  auto data = serial_port->readAll();

  std::cout << data.toStdString() << std::endl;

  handleRead(data);
}

void SCDTR_Interface::on_serial_port_connect_clicked(bool checked)
{
    if (checked) {
        if (serial_port->isOpen()) {
            serial_port->close();
        }


        auto clock = QDateTime::currentDateTime();
        this->start_time = clock.toMSecsSinceEpoch();
        connect(serial_port, &QSerialPort::readyRead, this, &SCDTR_Interface::read);

        QString portname = "/dev/";
        QString input_value = this->ui->serial_input->currentText();

        if (input_value == "") {
            input_value = "ttyACM0";
        }

        portname += input_value;
        serial_port->setPortName(portname);

        serial_port->setBaudRate(115200);

        auto output = serial_port->open(QIODevice::ReadWrite);
        if (!output) {
            std::cout << "Serial port did not open find out why " << std::endl;
            exit(EXIT_FAILURE);
        }
    }
    else {
        if (serial_port->isOpen()) {
            serial_port->close();
        }
        write_to_csv();
        signal_map.clear();
    }

}

void SCDTR_Interface::save_to_array(QString msg) {
    auto clock = QDateTime::currentDateTime();
    QStringList split_string =  msg.split(":");
    if (split_string.size() != 3) {
        return;
    }

    QString signal_name = split_string[1];
    QString signal_value = split_string[2];

    std::cout << "clock.to MS" << clock.toMSecsSinceEpoch() << std::endl;

    auto pair = qMakePair( clock.toMSecsSinceEpoch() - start_time, signal_value.toDouble());

    if (signal_map.contains(signal_name)) {
        signal_map[signal_name].push_back(pair);
    } else {
        signal_map[signal_name] = QVector<QPair<double, qint64>>();
        signal_map[signal_name].push_back(pair);
    }
}

