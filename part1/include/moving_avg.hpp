#ifndef __MOVING_AVG_HPP__
#define __MOVING_AVG_HPP__

#include <array>
#include <algorithm>
#include <numeric>

template <typename T, size_t size>
class MovingAvg
{
public:
    inline void operator<<(T value)
    {
        values[index++] = value;
        index %= size;
    }

    inline T get_avg()
    {
        return std::accumulate(values.begin(), values.end(), 0.0) / (static_cast<T>(size));
    }

    std::array<T, size> values = {0};

private:
    unsigned int index = 0;
};
#endif // __MOVING_AVG_HPP__