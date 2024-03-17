#include <cmath>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <utility>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "hardware/adc.h"
#include "hardware/flash.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#include "ThreadSafeQueue.hpp"
#include "can.hpp"
#include "command.hpp"
#include "led.hpp"
#include "moving_avg.hpp"
#include "pid.hpp"
#include "usb.hpp"

// * Constants
constexpr uint8_t ADC_PIN = 26;
constexpr uint8_t LAMP_PIN = 15;
constexpr float INPUT_VOLTAGE = 3.3f; // 3.3V
constexpr uint8_t CanBus_IRQ_PIN = 20;
constexpr uint8_t ADC_RESOLUTION = 12;    // bit adc
constexpr uint16_t LAMP_PWM_FREQ = 60000; // 60kHz
constexpr uint8_t LED_PIN = PICO_DEFAULT_LED_PIN;
constexpr uint16_t ADC_MAX_VALUE = (1 << ADC_RESOLUTION) - 1; // 4095

// * Typedefs for this file
using rep_t = struct repeating_timer;
using AdcMovingAvg = MovingAvg<float, 20>;
using Fifo = ThreadSafeQueue<std::shared_ptr<CommandInterface>>;
using Cmds = std::vector<std::shared_ptr<CommandInterface>>;

// Reading ADC value
struct read_adc_args {
  AdcMovingAvg *avg;
};

struct ctrl_args {
  AdcMovingAvg *avg;
  PID *pid;
  LED *led;
};

struct command_handle_args {
  PID *pid;
  LED *led;
  Logger *l;
};

// * Global variables
Fifo fifo0; // * Core 0 to Core 1
Fifo fifo1; // * Core 1 to Core 0
volatile uint16_t pico_idx = 0;
CanCom * can0;

Logger *l = nullptr;
volatile bool CANbus_available = false;
volatile bool Serial_available = false;
uint8_t this_pico_flash_id[8];

void serial_receive_irq(void *params) { Serial_available = true; }

void can_receive_irq(uint gpio, uint32_t events) {
  if (gpio == CanBus_IRQ_PIN) {
    CANbus_available = true;
    can0->set_data_available();
    std::cout << "in CAN irq" << std::endl;
    gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
  }
  l->log(Logger::LogLevel::ERROR, "CAN irq");
}

inline float convert_adc_value(uint16_t adc_value) {
  return adc_value * INPUT_VOLTAGE / ADC_MAX_VALUE;
}

bool read_adc(rep_t *timer) {
  read_adc_args *args = static_cast<read_adc_args *>(timer->user_data);
  uint16_t adc_value = adc_read();
  *(args->avg) << convert_adc_value(adc_value);
  return true;
}

// Debug function to flip the internal led
// mostly used to see if the code is running
bool flip_led(rep_t *t) {
  gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
  return true;
}

inline float convert_to_lux(float adc_value) {
  float res = 10e4 * (3.3f / adc_value - 1);

  return pow(10, ((log10(res) - 6.45) / -0.7));
}

bool controller_loop(rep_t *t) {
  ctrl_args *args = static_cast<ctrl_args *>(t->user_data);

  auto avg = args->avg;
  auto pid = args->pid;
  auto led = args->led;

  float u = pid->compute_control(convert_to_lux(avg->get_avg()));
  pid->anti_windup(u);
  pid->prepare_next_stage(convert_to_lux(avg->get_avg()));

  led->set_pwm(u);

  std::cout << "[INFO] : lux : " << convert_to_lux(avg->get_avg()) << std::endl;

  return true;
}

bool command_handle(rep_t *t) {
  auto pid = static_cast<command_handle_args *>(t->user_data)->pid;
  auto led = static_cast<command_handle_args *>(t->user_data)->led;

  std::shared_ptr<CommandInterface> cmd;
  bool success = fifo1.pop(cmd);

  if (success) {
    l->log(Logger::LogLevel::WARNING, "Command handled");

    cmd->handle(pid, led);
    std::cout << "Pid ref " << pid->get_reference() << std::endl;
  } else {
    l->log(Logger::LogLevel::WARNING, "No command to handle");
  }

  return true;
}

void send_commands_to_fifo(Cmds cmds) {
  for (auto &cmd : cmds) {
    fifo1.push(cmd);
    // ! Log Command fail
  }
}

// core 1 main function
// * USB and CAN bus communication will be done in this cor// * USB and CAN bus
// communication will be done in this coree
void main_core1() {
  // ! I will need interrups for CAN bus reading and USB reading

  UsbReader *usb = new UsbReader(30, l);

  CanCom *can0 = new CanCom(CanBus_IRQ_PIN, 17, 19, 16, 18, 10000000);

  gpio_set_irq_enabled_with_callback(CanBus_IRQ_PIN, GPIO_IRQ_EDGE_FALL, true,
                                     &can_receive_irq);


  std::vector<uint8_t> data = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  // Code for core 1 will be looped here
  while (true) {
    //auto msg_r = can0->send_msg(static_cast<uint32_t>(0x123), 8, data);

    //if (msg_r != ERROR::ERROR_OK) {
    //  l->log(Logger::LogLevel::ERROR, "Error sending message to CAN bus");
    //} else if (msg_r == ERROR::ERROR_OK) {
    //7  l->log(Logger::LogLevel::INFO, "Message sent to CAN bus");
    //}


    if (Serial_available) {
      l->log(Logger::LogLevel::INFO, "USB commands received");

      usb->read_usb();
      Serial_available = false;

      auto commands = usb->get_usb_commands();
      usb->clear_usb_commands();

      if (commands.size() > 0) {
        send_commands_to_fifo(commands);
      }
    }

    // Can bus pooling
    struct can_frame frame;
    if (can0->can0.readMessage(&frame) == MCP2515::ERROR::ERROR_OK) {
      l->log(Logger::LogLevel::INFO, "CAN message received");
      l->log(Logger::LogLevel::INFO,
             "CAN message id: " + std::to_string(frame.can_id));
      l->log(Logger::LogLevel::INFO,
             "CAN message data: " + std::to_string(frame.data[0]));
    }
    
    /*
    if (CANbus_available) {

      l->log(Logger::LogLevel::INFO, "CAN commands received-----");
      CANbus_available = false;

      auto err = can0->read_msg();
      if (err == ERROR::ERROR_OK) {
        l->log(Logger::LogLevel::INFO, "CAN message received-----");
        auto msg = can0->get_message();
      } else {
        l->log(Logger::LogLevel::ERROR, "Error reading message from CAN bus");
      }
    } */
  }
}

// *  Core 0 main function
// * Controller and ADC reading will be done in this code
int main() {
  rep_t read_adc_timer;
  rep_t flip_internal_led;
  rep_t change_index_timer;
  rep_t command_handle_timer;
  rep_t controller_loop_timer;

  LED led = LED(LAMP_PIN, LAMP_PWM_FREQ, false);
  flash_get_unique_id(this_pico_flash_id);

  if (this_pico_flash_id[6] == 0x6D) {
    pico_idx = 1;
  } else if (this_pico_flash_id[6] == 0x62) {
    pico_idx = 2;
  } else if (this_pico_flash_id[6] == 0x61) {
    pico_idx = 3;
  }

  AdcMovingAvg avg;
  PID pid = PID();

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  stdio_init_all();

  adc_init();

  adc_gpio_init(ADC_PIN);
  adc_select_input(0);

  gpio_set_function(LAMP_PIN, GPIO_FUNC_PWM);

  pwm_config config = pwm_get_default_config();

  // * Create logger
  l = new Logger(pico_idx);

  // * USB comunication callback
  stdio_set_chars_available_callback(serial_receive_irq, NULL);

  // * CAN bus communication callback

  // * launch core 1
  multicore_launch_core1(main_core1);
  ctrl_args *args = new ctrl_args();
  args->avg = &avg;
  args->pid = &pid;
  args->led = &led;

  read_adc_args *adc_args = new read_adc_args();
  adc_args->avg = &avg;

  // add_repeating_timer_ms(1000, flip_led, NULL, &flip_internal_led);

  // * Function that will read the ADC every 1ms
  add_repeating_timer_ms(1, read_adc, adc_args, &read_adc_timer);

  // * Controll loop repeating timer
  // add_repeating_timer_ms(-10, controller_loop, args, &controller_loop_timer);

  //  add_repeating_timer_ms(8000, change_index, NULL, &change_index_timer);

  command_handle_args *command_args = new command_handle_args();
  command_args->pid = &pid;
  command_args->led = &led;
  command_args->l = l;

  // Add repeating timer to check the fifo command
  add_repeating_timer_ms(1000, command_handle, command_args,
                         &command_handle_timer);

  fifo0.set_logger(l);
  fifo1.set_logger(l);

  int counter = 0;

  while (true) {
  }
}
