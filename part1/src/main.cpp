#include <cmath>
#include <memory>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

#include "moving_avg.hpp"
#include <iostream>

using rep_t = struct repeating_timer;

struct ctrl_args
{
  MovingAvg *avg;
};

inline double convert_adc_value(uint16_t adc_value, int number_of_bits)
{
  return adc_value * 3.3f / (1 << number_of_bits);
}

bool flip_led(rep_t *t)
{
  gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
  gpio_ return true;
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

int main()
{
  rep_t flip_internal_led;
  rep_t controller_loop_timer;

  MovingAvg avg(16);

  const uint LED_PIN = PICO_DEFAULT_LED_PIN;
  const uint LAMP_PIN = 15;
  const uint ADC_PIN = 26; // GPIO26
  const uint ADC_NUMBER_OF_BITS = 12;

  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);

  stdio_init_all();

  adc_init();

  adc_gpio_init(ADC_PIN);
  adc_select_input(0);

  gpio_set_function(LAMP_PIN, GPIO_FUNC_PWM);

  pwm_config config = pwm_get_default_config();

  add_repeating_timer_ms(500, flip_led, NULL, &flip_internal_led);

  ctrl_args *args = new ctrl_args();
  args->avg = &avg;

  add_repeating_timer_ms(100, controller_loop, args, &controller_loop_timer);

  int counter = 0;

  while (true)
  {
  }
}
