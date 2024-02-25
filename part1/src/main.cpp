#include <cmath>
#include <memory>
#include <stdio.h>

#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/irq.h"


#include "controller.hpp"

inline float convert_adc_value (uint16_t adc_value, int number_of_bits) {
  return adc_value * 3.3f / (1 << number_of_bits);
}
 
int main(){

  PID controller {0.01, 1, 0, 0.05  };
  const uint LED_PIN =  PICO_DEFAULT_LED_PIN;
  const uint LAMP_PIN = 15;
  const uint ADC_PIN = 26; // GPIO26
  const uint ADC_NUMBER_OF_BITS = 12;
   
  gpio_init(LED_PIN);
  gpio_set_dir(LED_PIN, GPIO_OUT);
  gpio_init(LAMP_PIN);
  gpio_set_dir(LAMP_PIN, GPIO_OUT);

  stdio_init_all();

  adc_init();
     
  adc_gpio_init(ADC_PIN);
  adc_select_input(0);
    

  // Set up PWM for the LED
  uint slice_num = pwm_gpio_to_slice_num(LAMP_PIN);
  pwm_set_wrap(slice_num, 1 << ADC_NUMBER_OF_BITS);  // PWM range is 0-255
  pwm_set_chan_level(slice_num, PWM_CHAN_A, 0);  // Start with LED off
  pwm_gpio_init(LED_PIN);

  unsigned int internal_led_state = 0;
  while(true) {
    // blink internal led
    internal_led_state = internal_led_state == 0 ? 1 : 0;
    gpio_put(LED_PIN, internal_led_state);
    gpio_put(LAMP_PIN, internal_led_state);
    
    uint16_t adc_value = adc_read(); 
    // printf("Current  ADC value read : 0x%03x bit /  Voltage :  %f V\n", adc_value, convert_adc_value(adc_value, 12));

    float u = controller.compute_control(1.5f, convert_adc_value(static_cast<float>(adc_value), ADC_NUMBER_OF_BITS ) );
    controller.housekeep(300.0f, static_cast<float>(adc_value));
    controller.anti_integral_windup();
    pwm_set_chan_level(slice_num, PWM_CHAN_A, u);

    // printf("Current Actuation : %f\n", u);

    //float u = controller.compute_control(2000.0f, static_cast<float>(convert_adc_value(result, 12)));

     
    //controller.housekeep(2000.0f, static_cast <float> (convert_adc_value(result, 12)));
  
    //printf("Raw value 0x%03x voltage : %f pwm %f V\n", result, u, static_cast<float>(convert_adc_value(result, 12)));

    sleep_ms(10);

  }
}
