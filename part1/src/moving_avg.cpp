#include "moving_avg.hpp"
#include <stdio.h>

MovingAvg::MovingAvg(unsigned int size) : size(size)
{
    this->values = std::vector<double>(size, 0.0);
    this->index = 0;
}

MovingAvg::~MovingAvg()
{
}

void MovingAvg::operator<<(double value)
{
    this->values[this->index++] = value;
    this->index %=  this->size;
}

double MovingAvg::operator--()
{
    return std::accumulate(this->values.begin(), this->values.end(), 0.0) /  (static_cast<double>(this->size);
}

