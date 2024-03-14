#ifndef __PID_HPP__
#define __PID_HPP__

#include <algorithm>
#include <iostream>

class PID
{
public:
    PID();
    ~PID();

    float clamp(float value);
    float compute_control(float setpoint, float current_state);

    void anti_windup(float value);
    void prepare_next_stage(float setpoint, float current_state);

private:
    float kp = 0.543f; // proportional gain
    float ki = 4.753f; // integral gai
    float kd = 0.3f;   // derivative gain

    float ti = 0.14f; // integral time constant
    float td;         // derivative time constant
    float n;
    float b = 1.0;
    float c = 1.0;

    float h = 0.01;
    float dt = 0.01f;

    float d_const = 0.0;

    const float min = 0;
    const float max = 1 << 12;

    float last_error = 0.0;
    float last_state = 0.0;

    float d_error = 0.0;

    float p_term = 0.0;
    float d_term = 0.0;
    float i_term = 0.0;
};

inline float PID::clamp(float value)
{
    return std::max(min, std::min(max, value));
}

inline void PID::prepare_next_stage(float setpoint, float current_state)
{
    last_error = setpoint - current_state;

    i_term += last_error * dt;
    i_term = clamp(i_term);
}

inline void PID::anti_windup(float value)
{
    float windup_error = this->clamp(value) - value;

    i_term -= ki * windup_error / ti;
}

#endif // __PID_HPP__