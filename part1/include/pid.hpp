#ifndef __PID_HPP__
#define __PID_HPP__

#include <algorithm>
#include <cstdint>
#include <iostream>

/**
 * @brief PID controller class
 */
class PID {
public:
  /**
   * @brief Construct a new PID object
   *
   * @param kp proportional gain
   * @param kd derivative gain
   * @param ki integral gain
   * @param kff feed forward gain
   * @param ka anti windup gain
   * @param dt time step
   *
   * @note default values are from the paper "Design of a PID Controller for a
   * luminaire
   */
  PID(float kp = 0.43f, float kd = 0.0f, float ki = 0.853f, float kff = 0.8f,
      float ka = 0.3f, float dt = 0.01f);

  /**
   * @brief Destroy the PID object
   */
  ~PID() = default;

  float compute_control(float current_state);

  /**
   * @brief Clamp the value between min and max
   *
   * @param value value to be clamped
   */
  inline float clamp(float value) {
    return std::min(max, std::max(min, value));
  }

  /**
   * @brief Anti windup function
   *
   * @param value value to be clamped
   */
  inline void anti_windup(float value) {
    float windup_error =
        anti_windup_enabled ? this->clamp(value) - value : 0.0f;
    i_term -= windup_error * ka;
  }

  /**
   * @brief Prepare the next stage of the PID controller
   *
   * @param r setpoint
   * @param y current state
   */
  inline void prepare_next_stage(float y) {
    this->last_lumminance = y;
    // calculate integral without anti windup
    last_error = ref - y;
    i_term += last_error * dt;
  }

  // * setters

  /**
   * @brief Set the proportional gain
   *
   * @param kp proportional gain
   */
  void set_kp(float kp);

  /**
   * @brief Set the integral gain
   *
   * @param ki integral gain
   */
  void set_ki(float ki);

  /**
   * @brief Set the derivative gain
   *
   * @param kd derivative gain
   */
  void set_kd(float kd);

  /**
   * @brief Set the feed forward gain
   *
   * @param kff feed forward gain
   */
  void set_kff(float kff);

  /**
   * @brief Set the anti windup gain
   *
   * @param ka anti windup gain
   */
  void set_ka(float ka);

  /**
   * @brief Set the time step
   *
   * @param dt time step
   */
  void set_dt(float dt);

  /**
   * @brief Set the anti windup enable
   *
   * @param enable enable anti windup
   */
  void set_anti_windup(bool enable);

  /**
   * @brief Set the feed forward enable
   *
   * @param enable enable feed forward
   */
  void set_ff_enable(bool enable);

  /**
   * @brief Set the active state of the PID controller
   *
   * @param active active state of the PID controller
   */
  void set_active(bool active);

  /**
   * @brief Set the reference of the PID controller
   *
   * @param reference reference of the PID controller
   */
  void set_reference(float reference);

  /**
   * @brief Set the occupancy of the PID controller
   *
   * @param occupancy occupancy of the PID controller
   */
  void set_occupancy(bool occupancy);

  // * getters

  /**
   * @brief Get the proportional gain
   *
   * @return float proportional gain
   */

  float get_kp();

  /**
   * @brief Get the integral gain
   *
   * @return float integral gain
   */
  float get_ki();

  /**
   * @brief Get the derivative gain
   *
   * @return float derivative gain
   */
  float get_kd();

  /**
   * @brief Get the feed forward gain
   *
   * @return float feed forward gain
   */
  float get_kff();

  /**
   * @brief Get the anti windup gain
   *
   * @return float anti windup gain
   */
  float get_ka();

  /**
   * @brief Get the time step
   *
   * @return float time step
   */
  float get_dt();

  /**
   * @brief Get the anti windup enable
   *
   * @return bool anti windup enable
   */
  bool get_anti_windup();

  /**
   * @brief Get the feed forward enable
   *
   * @return bool feed forward enable
   */
  bool get_ff_enable();

  /**
   * @brief Get the active state of the PID controller
   *
   * @return bool active state of the PID controller
   */
  bool get_active();

  /**
   * @brief Get the reference of the PID controller
   *
   * @return float reference of the PID controller
   */
  float get_reference();

  /**
   * @brief Get the occupancy of the PID controller
   *
   * @return bool occupancy of the PID controller
   */
  bool get_occupancy();

  /**
   * @brief Get the last lummiance
   *
   * @return @ float last lummiance
   */
  float get_last_lummiance();

private:
  float kp = 0.543f; // proportional gain
  float ki = 4.753f; // integral gai
  float kd = 0.0f;   // derivative gain
  float kff = 0.8f;  // feed forward gain
  float ka = 0.3f;   // anti windup gain

  float b = 1.0;
  float c = 1.0;

  float dt = 0.1;

  // output will be period of duty cycle, so it must be clamp between 0 and 1
  const float min = 0.0f;
  const float max = 1.0f;

  float last_error = 0.0;
  float last_state = 0.0;

  float d_error = 0.0;

  float p_term = 0.0;
  float d_term = 0.0;
  float i_term = 0.0;

  bool anti_windup_enabled = true;
  bool ff_enable = true;
  bool active = true;
  bool occupancy = true;

  float ref = 10.0;
  float last_lumminance = 0.0;
  uint32_t start_time = 0;
};

// * setters
inline void PID::set_kp(float kp) { this->kp = kp; }
inline void PID::set_ki(float ki) { this->ki = ki; }
inline void PID::set_kd(float kd) { this->kd = kd; }
inline void PID::set_kff(float kff) { this->kff = kff; }
inline void PID::set_ka(float ka) { this->ka = ka; }
inline void PID::set_dt(float dt) { this->dt = dt; }
inline void PID::set_anti_windup(bool enable) {
  this->anti_windup_enabled = enable;
}
inline void PID::set_ff_enable(bool enable) { this->ff_enable = enable; }
inline void PID::set_active(bool active) { this->active = active; }
inline void PID::set_reference(float reference) { this->ref = reference; }
inline void PID::set_occupancy(bool occupancy) { this->occupancy = occupancy; }

// * getters
inline float PID::get_kp() { return kp; }
inline float PID::get_ki() { return ki; }
inline float PID::get_kd() { return kd; }
inline float PID::get_kff() { return kff; }
inline float PID::get_ka() { return ka; }
inline float PID::get_dt() { return dt; }
inline bool PID::get_anti_windup() { return anti_windup_enabled; }
inline bool PID::get_ff_enable() { return ff_enable; }
inline bool PID::get_active() { return active; }
inline float PID::get_reference() { return ref; }
inline bool PID::get_occupancy() { return occupancy; }
inline float PID::get_last_lummiance() { return last_lumminance; }

#endif // __PID_HPP__
