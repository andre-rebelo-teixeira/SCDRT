#ifndef __MOVING_AVG_HPP__
#define __MOVING_AVG_HPP__

#include <array>
#include <algorithm>
#include <numeric>

#include "pico/mutex.h"

// Moving average class template
// This is thread safe since the mutex lock the queue acess for both the push  << and the pop operation
template <typename T, size_t size>
class MovingAvg
{
public:
    inline MovingAvg()
    {
        mutex_init(&mutex);
    }

    inline void operator<<(T value)
    {
        if (mutex_try_enter(&mutex, nullptr) == false)
        {
            return;
        }
        values[index++] = value;
        index %= size;
        mutex_exit(&mutex);
    }

    inline T get_avg()
    {
        if (mutex_try_enter(&mutex, nullptr) == false)
        {
            return 0.0;
        }

        auto val_ =  std::accumulate(values.begin(), values.end(), 0.0) / (static_cast<T>(size));
        mutex_exit(&mutex);
        return val_;
    }

    std::array<T, size> values = {0};

private:
    unsigned int index = 0;
    mutex_t mutex;
};
#endif // __MOVING_AVG_HPP__