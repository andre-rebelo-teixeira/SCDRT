#ifndef __LED_HPP__
#define __LED_HPP__

#include <stdint.h>

#include "hardware/pwm.h"

class LED
{

public:
    LED(uint8_t pin, uint16_t pwm_freq = 60e3, bool phase_correct = false);

    ~LED() = default;

    void set_pwm(float duty_cycle);
    void set_duty_cycle(float duty_cycle);
    void set_luminosity_percentage(float percentage);
    float get_duty_cycle();

private:
    void change_pwm_status(bool status);

    uint8_t pin;
    uint8_t slice;
    uint8_t channel;

    uint16_t pwm_freq; // 60kHz -> limit switch noise for LDR
    uint16_t pwm_warp;
    uint16_t level;
    const uint16_t DAC_RANGE = 1 << 12;

    float pwm_value;

    bool phase_correct;
    bool pwm_enabled = false;
};

inline void LED::set_duty_cycle(float duty_cycle)
{
    // set_pwm_range(static_cast<uint8_t>((float)DAC_RANGE * duty_cycle));
}

inline void LED::set_luminosity_percentage(float percentage)
{
    set_duty_cycle(percentage / 100.0);
}

inline float LED::get_duty_cycle()
{
    return pwm_value;
}

inline void LED::change_pwm_status(bool status)
{
    pwm_set_enabled(slice, status);
}

#endif // __LED_HPP__