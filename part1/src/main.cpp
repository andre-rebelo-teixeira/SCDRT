#include <cmath>
#include <iostream>
#include <memory>
#include <stdio.h>
#include <utility>

#include "pico/multicore.h"
#include "pico/stdlib.h"

#include "hardware/adc.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "hardware/pwm.h"

#include "ThreadSafeQueue.hpp"
#include "command.hpp"
#include "led.hpp"
#include "moving_avg.hpp"
#include "pid.hpp"
#include "usb.hpp"

// * Constants
constexpr uint8_t ADC_PIN = 26;
constexpr uint8_t LAMP_PIN = 15;
constexpr float INPUT_VOLTAGE = 3.3f;     // 3.3V
constexpr uint8_t ADC_RESOLUTION = 12;    // bit adc
constexpr uint16_t LAMP_PWM_FREQ = 60000; // 60kHz
constexpr uint8_t LED_PIN = PICO_DEFAULT_LED_PIN;
constexpr uint16_t ADC_MAX_VALUE = (1 << ADC_RESOLUTION) - 1; // 4095

// * Typedefs for this file
using rep_t = struct repeating_timer;
using AdcMovingAvg = MovingAvg<float, 20>;
using Fifo = ThreadSafeQueue<CommandInterface>;

// Reading ADC value
struct read_adc_args {
  AdcMovingAvg *avg;
};

struct ctrl_args {
  AdcMovingAvg *avg;
  PID *pid;
  LED *led;
};

// For now i only want to send this
struct fifo_coms {
  Fifo *fifo0; // * Thread safe queue to comunicate from core 0 to core 1
  Fifo *fifo1; // * Thread safe queue to comunicate from core 1 to core 0
};

struct command_handle_args {
  PID *pid;
  LED *led;
  Logger *l;
};

// * Global variables
fifo_coms *coms = nullptr;
Logger *l = nullptr;
bool CANbus_available = false;

static int index = 0;
float ref[] = {10.0, 8.0, 6.0, 15.0, 16.0};

inline float convert_adc_value(uint16_t adc_value) {
  return adc_value * INPUT_VOLTAGE / ADC_MAX_VALUE;
  ;
}

bool read_adc(rep_t *timer) {
  read_adc_args *args = static_cast<read_adc_args *>(timer->user_data);
  uint16_t adc_value = adc_read();
  *(args->avg) << convert_adc_value(adc_value);
  return true;
}

// this is used to change the referece while i dont have the usb communication
bool change_index(rep_t *) {
  index += 1;
  if (index > 4) {
    index = 0;
  }
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

  float reference = ref[index];

  float u = pid->compute_control(convert_to_lux(avg->get_avg()));
  pid->anti_windup(u);
  pid->prepare_next_stage( convert_to_lux(avg->get_avg()));

  led->set_pwm(u);

  std::cout << "[INFO] : lux : " << convert_to_lux(avg->get_avg()) << std::endl;
  std::cout << "[INFO] : setpoint : " << reference << std::endl;

  return true;
}

bool command_handle(rep_t *t) {
  auto fifo1 = coms->fifo1;
  auto pid = static_cast<command_handle_args *>(t->user_data)->pid;
  auto led = static_cast<command_handle_args *>(t->user_data)->led;

  CommandInterface cmd;
  bool success = fifo1->pop(cmd);

  if (success) {
    cmd.handle(pid, led);
     
    l->log(Logger::LogLevel::WARNING, "Command handled");
  }

  return true;
}

// core 1 main function
// * USB and CAN bus communication will be done in this cor// * USB and CAN bus
// communication will be done in this coree
void main_core1() {
  // ! I will need interrups for CAN bus reading and USB reading

  UsbReader *usb = new UsbReader(10, l);

  // Code for core 1 will be looped here
  while (true) {
    usb->read_usb();
    sleep_ms(1000);
    //auto commands = usb->get_usb_commands();
    //usb->clear_usb_commands();

    l->log(Logger::LogLevel::INFO, "USB commands received-----");
    // Send commands to the other fifo
    //if (commands.size() > 0) {
    //  l->log(Logger::LogLevel::INFO, "Sending commands to core 0");
    //  for (auto &cmd : commands) {
    //    coms->fifo1->push(cmd);
    //  }
    //}
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
  l = new Logger();

  // * Create the comunication between cores
  Fifo *fifo0 = new Fifo();
  Fifo *fifo1 = new Fifo();
  coms = new fifo_coms();
  coms->fifo0 = fifo0;
  coms->fifo1 = fifo1;

  // * launch core 1
  multicore_launch_core1(main_core1);

  ctrl_args *args = new ctrl_args();
  args->avg = &avg;
  args->pid = &pid;
  args->led = &led;

  read_adc_args *adc_args = new read_adc_args();
  adc_args->avg = &avg;

  add_repeating_timer_ms(1000, flip_led, NULL, &flip_internal_led);

  // * Function that will read the ADC every 1ms
   // add_repeating_timer_ms(1, read_adc, adc_args, &read_adc_timer);

  // * Controll loop repeating timer
 //  add_repeating_timer_ms(-10, controller_loop, args, &controller_loop_timer);

//  add_repeating_timer_ms(8000, change_index, NULL, &change_index_timer);

  command_handle_args *command_args = new command_handle_args();
  command_args->pid = &pid;
  command_args->led = &led;
  command_args->l = l;

  // Add repeating timer to check the fifo command
 // add_repeating_timer_ms(1000, command_handle, command_args,
       //                  &command_handle_timer);

  int counter = 0;

  while (true) {
  }
}
