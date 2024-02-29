#ifndef __MOVING_AVG_HPP__
#define __MOVING_AVG_HPP__

#include <vector>
#include <algorithm>
#include <numeric>

class MovingAvg
{
public:
    MovingAvg(unsigned int size);
    ~MovingAvg();

    void operator<<(double value);
    double operator--();
    double get_avg();

    std::vector<double> values;
    unsigned int size;

private:
    unsigned int index = 0;
};

#endif // __MOVING_AVG_HPP__