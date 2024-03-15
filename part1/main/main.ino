#include <cmath>
#include <memory>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

#include "moving_avg.hpp"
#include "ThreadSafeQueue.hpp"
#include "command.hpp"
#include "usb.hpp"
#include "pid.hpp"
#include "logger.hpp"
#include "can.hpp"
#include <iostream>

#include <FreeRTOS.h>
#include <task.h>

using rep_t = struct repeating_timer;

struct ctrl_args
{
  MovingAvg<float, 16> *avg;
};

inline double convert_adc_value(uint16_t adc_value, int number_of_bits)
{
  return adc_value * 3.3f / (1 << number_of_bits);
}

bool flip_led(rep_t *t)
{
  gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
  return true;
}

bool controller_loop(rep_t *t)
{
  ctrl_args *args = static_cast<ctrl_args *>(t->user_data);

  auto avg = args->avg;
  uint16_t adc_value = adc_read();
  *avg << convert_adc_value(adc_value, 12);
  std::cout << avg-- << std::endl;

  return true;
}




static void test(void *pvParameter) {
  Serial.println("teste");

  gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
  vTaskDelay(100);
}
// setup function for core 0
void setup()
{
  //Logger *logger = new Logger();
  //Reader *usb_reader = new Reader(10, logger);

 // ThreadSafeQueue <CommandInterface> fifo0; 
  //ThreadSafeQueue <CommandInterface> fifo1;

  //CanCom *can = new CanCom(spi0, 17, 19, 16, 18, 10000000);
  xTaskCreate(test, "usb_reader", 2048, NULL, 1, NULL);

//  xTaskCreate(Reader::read_usb_task, "usb_reader", 2048, usb_reader, 1, NULL);

  // TODO: finish processing usb messages

  // xTaskCreate(usb_reader.process_usb_msgs, "usb_processor", 2048, NULL, 1, NULL);

  //xTaskCreateAffinitySet(can->read_msg, "can_reader", 2048, NULL, 1, 0b01, NULL);
  //xTaskCreateAffinitySet(can.process_messages, "can_processor", 2048, NULL, 1, 0b01, NULL);

  // task for second core
  //PID controller = new PID();

  //xTaskCreateAffinitySet(controller->control_loop, "controller", 2048, NULL, 0, 0b10, NULL);
}

// setup function for code 1
void setup1()
{
}

// loop function for core 0
void loop()
{
  Serial.printf("Send a message in one core");
  gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
  vTaskDelay(100);
}

// loop function for core 1
void loop1()
{
  Serial.printf("Send a message from another core");
  vTaskDelay(100);
}