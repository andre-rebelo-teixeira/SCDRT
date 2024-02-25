#include "controller.hpp"

#include <algorithm>
#include <iostream>

#include <stdio.h>

PID::PID( float _h, float _K, float b_,
 float Ti_, float Td_, float N_, float Tt_)
 : h {_h}, K {_K}, b {b_}, Ti {Ti_}, Td {Td_},  
 N {N_}, I {0.0}, D {0.0}, y_old{0.0}, Tt {Tt_}
{
 printf("PID arguments  : %f %f %f %f %f %f", _h, _K, b_, Ti_, Td_, N_);

 this->d_aux = this->Td / ( this->Td + this->N * this->h);
} // should check arguments validity

PID::~PID() 
{}

// ref -> reference value
// y -> current value 
float PID::compute_control( float ref, float y ) {
  
 this->propotional = this->K * (this->b * ref -y);
 
 // Convert this formula to have C if we want to
 this->derivative = this->d_aux * this->derivative + \
                   this->d_aux * this->K * this->N * (y -  this->y_prev);

 this->u =  this->propotional + \
            this->derivative +  \
            this->integral;

 std::cout << "Comput controll actuation " << this->propotional << " " << \
             this->derivative << " " << this->integral << " " << this->u <<std::endl;
 return check_saturation(this->u);
}


