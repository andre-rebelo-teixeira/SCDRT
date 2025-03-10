#ifndef __PID_HPP__
#define __PID_HPP__

#include <algorithm>

class PID
{
public:
    PID(float kp, float ki, float kd, float b = 1.0, float n = 10, float h = 10);
    ~PID();

    float clamp(float value);
    float compute_control(float setpoint, float current_state);

    void anti_windup(float value);
    void prepare_next_stage(float setpoint, float current_state);

private:
    float kp; // proportional gain
    float ki; // integral gain
    float kd; // derivative gain

    float ti; // integral time constant
    float td; // derivative time constant
    float n;
    float b = 1.0;
    float h = 0.01;

    float d_const = 0.0;

    const float min = 0;
    const float max = 1 << 12;

    float last_error = 0.0;
    float last_state = 0.0;

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

    i_term += ki / ti * last_error * h;
}

inline void PID::anti_windup(float value)
{
    float windup_error = this->clamp(value) - value;

    i_term -= windup_error / ti;
}

#endif // __PID_HPP__