#include "pid.hpp"
#include <iostream>

PID::PID(float kp, float kd, float ki, float kff, float ka, float dt)
    : kp(kp), kd(kd), ki(ki), kff(kff), ka(ka) {
  if (dt > 0.0) {
    this->dt = dt;
  }
}

float PID::compute_control(float current_state) {

  float d_error = ref * c - current_state;

  p_term = kp * (b * ref - current_state);

  d_term = kd * (d_error - this->d_error) / dt;

  this->d_error = d_error;

  // check if feat forward is enabled
  float ff = ff_enable ? kff * ref : 0.0;

  return this->clamp((p_term + d_term + i_term * ki + ff) / 22.3);
}
