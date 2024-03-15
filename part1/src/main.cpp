#include <cmath>
#include <memory>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"

#include "moving_avg.hpp"
#include "pid.hpp"
#include <iostream>

using rep_t = struct repeating_timer;

static int index = 0;
float ref[] = {10.0, 8.0, 6.0, 15.0, 16.0};

struct ctrl_args
{
  MovingAvg<float, 2> *avg;
  PID *pid;
};

bool change_index(rep_t *) {
  index += 1;
  if (index > 4) {
    index = 0;
  }
  return true;
}

inline float convert_adc_value(uint16_t adc_value, int number_of_bits)
{
  return adc_value * 3.3f / (1 << number_of_bits);
}

bool flip_led(rep_t *t)
{
  gpio_put(PICO_DEFAULT_LED_PIN, !gpio_get(PICO_DEFAULT_LED_PIN));
  return true;
}

void config_pwm(uint32_t pin, uint32_t period, float duty_cycle)
{
  duty_cycle = (duty_cycle > 1) ? 1 : ((duty_cycle < 0) ? 0 : duty_cycle); // Clip duty cycle
  uint32_t t_on = (uint32_t)(period * duty_cycle);

  gpio_set_function(pin, GPIO_FUNC_PWM); // Allocate pin to pwm

  // Set pwm params rfor the selected pin
  uint32_t slice_num = pwm_gpio_to_slice_num(pin);
  pwm_set_wrap(slice_num, period);
  pwm_set_chan_level(slice_num, PWM_CHAN_B, t_on);
  pwm_set_chan_level(slice_num, PWM_CHAN_A, period - t_on);

  pwm_set_enabled(slice_num, true);
}

float convert_to_lux(float adc_value)
{
  float res = 10e4 * (3.3f / adc_value - 1);

  float lux = pow(10, ((log10(res) - 6.45) / -0.7));


  return lux;
}

bool controller_loop(rep_t *t)
{
  ctrl_args *args = static_cast<ctrl_args *>(t->user_data);

  auto avg = args->avg;
  auto pid = args->pid;
  uint16_t adc_value = adc_read();
  *avg << convert_adc_value(adc_value, 12);
  
  float reference = ref[index]; 


  float u = pid->compute_control(reference, convert_to_lux(avg->get_avg()));
  pid->anti_windup(u);
  pid->prepare_next_stage(reference, convert_to_lux(avg->get_avg()));
  
  config_pwm(15, 10000, u / 2  );

  std::cout << "[INFO] : lux : " << convert_to_lux(avg->get_avg()) << std::endl;
  std::cout << "[INFO] : setpoint : " << reference << std::endl;

  return true;
}

int main()
{
  rep_t flip_internal_led;
  rep_t controller_loop_timer;
  rep_t change_index_timer;

  MovingAvg<float, 2> avg;
  PID pid = PID();

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
  args->pid = &pid;
  add_repeating_timer_ms(10, controller_loop, args, &controller_loop_timer);


  add_repeating_timer_ms(4000, change_index, NULL, &change_index_timer );

  int counter = 0;

  while (true)
  {
  }
}
