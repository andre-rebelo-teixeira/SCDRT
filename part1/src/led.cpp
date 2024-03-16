#include "led.hpp"

#include "hardware/gpio.h"

#include <algorithm>

/**
 * @brief Construct a new LED::LED object
 *
 * @param pin pin number where led is connected
 * @param pwm_freq pwm frequency used to power the LED -  this is a uint16_t so smalled pwm freq is 125MHz / 65535 = 1907.3486Hz
 * @param phase_correct pwm phase correct mode - default is false - now count back to zero
 */
LED::LED(uint8_t pin, uint16_t pwm_freq, bool phase_correct) : pin(pin), pwm_freq(pwm_freq)
{
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // get pwm slice
    slice = pwm_gpio_to_slice_num(pin);
    channel = pwm_gpio_to_channel(pin);

    pwm_warp = (uint16_t)(pwm_freq / 2.0f - 1.0f);

    pwm_set_wrap(slice, pwm_warp);

    pwm_set_phase_correct(slice, phase_correct);

}

/**
 * @brief Set the pwm duty cycle
 *
 * @param duty_cycle part of the period the led is on - 0.0f to 1.0f
 */
void LED::set_pwm(float duty_cycle)
{
    // turn off pwm
    change_pwm_status(false);

    // clip duty cycle
    duty_cycle = std::min(1.0f, std::max(0.0f, duty_cycle)); // clip duty cycle
    pwm_value = duty_cycle;

    // calculate time on
    level = static_cast<uint16_t>(static_cast<float>(pwm_warp) * duty_cycle);

    // set pwm level
    pwm_set_chan_level(slice, channel, level);

    // turn on pwm
    change_pwm_status(true);
}