#include "moving_avg.hpp"
#include <stdio.h>

// Constructor
MovingAvg::MovingAvg(unsigned int size) : size(size)
{
    this->values = std::vector<double>(size, 0.0);
    this->index = 0;
}

// Destructor
MovingAvg::~MovingAvg()
{
}

// Insert data into the moving average
void MovingAvg::operator<<(double value)
{
    this->values[this->index++] = value;
    this->index %= this->size;
}

// Get the average of the moving average
float MovingAvg::get_avg()
{
    return std::accumulate(this->values.begin(), this->values.end(), 0.0) / (static_cast<float>(this->size));
}
