#include "pid.hpp"
#include <iostream>

PID::PID()
{
    // check validity of parameters
    d_const = td / (td + n + h);
}

PID::~PID()
{
}

float PID::compute_control(float setpoint, float current_state)
{

    float d_error = setpoint * c - current_state;

    p_term = kp * (b * setpoint - current_state);

    d_term = kd * (d_error - this->d_error) / dt;

    this->d_error = d_error;

    // std::cout << "P term: " << p_term << std::endl;
    // std::cout << "D term: " << d_term << std::endl;
    // std::cout << "I term: " << i_term << std::endl;
    // std::cout << "P " << p_term << " I " << i_term * ki << " D " << d_term << std::endl;

    return this->clamp((p_term + d_term + i_term * ki) / 22.3f);
}
