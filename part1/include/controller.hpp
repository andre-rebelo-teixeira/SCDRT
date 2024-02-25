#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include <algorithm>
#include <iostream>

class PID {
  public:
    explicit PID( float _h, float _K = 1, float b_ = 1,
    float Ti_ = 1, float Td_ = 0, float N_ = 10,  float Tt_ = 1);
    ~PID() ;

    float compute_control( float ref, float y);

    void anti_integral_windup();
    void housekeep( float ref, float y);
    float check_saturation(float u);  

  private:
    float I, D, K, Ti, Td, b, h, y_old, N, Tt;
    float propotional = 0;
    float derivative = 0;
    float d_aux = 0;
    float integral = 0;
    float y_prev = 0;
    float u = 0;

};

// Calculate for the next itteration
inline void PID::housekeep (float ref, float  y) {
 // error = ref - y
 this->integral +=  this->K / this->Ti * (ref - y);
 std::cout << "house keep " << this->K << " " << this->Ti << " " << (ref - y) << std::endl;
}

inline void PID::anti_integral_windup() {
 // saturation error = this->check_saturation(this->u) - u 
 this->integral +=  (this->check_saturation(this->u) - this->u) / this->Tt;
 std::cout << "Anti windup " << this->check_saturation(this->u) << " " << this->u << " " << this->Tt << std::endl;
}

inline float PID::check_saturation(float u)
{
  return std::min(4095.0f , std::max (0.0f, u));
}



#endif
