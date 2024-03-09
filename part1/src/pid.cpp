#include "pid.hpp"
#include <iostream>

PID::PID(float kp, float ki, float kd, float b, float n, float h)
    : kp(kp), ki(ki), kd(kd), b(b), h(h), n(n)
{
    // check validity of parameters
    d_const = td / (td + n + h);
}

PID::~PID()
{
}

float PID::compute_control(float setpoint, float current_state)
{
    float error = setpoint - current_state;

    d_term = kd * (current_state - last_state) / h;

    p_term = kp * (b * setpoint - current_state);

    std::cout << "actuation_terms  " << p_term << " " << d_term << " " << i_term << std::endl;

    return this->clamp(p_term + d_term + i_term);
}
