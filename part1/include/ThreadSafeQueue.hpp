#ifndef __FIFO_HPP__
#define __FIFO_HPP__

// custom includes
#include "logger.hpp"

// C++ library headers
#include <queue>
#include <utility>

// Pico library headers
#include "pico/lock_core.h"
#include "pico/mutex.h"
// wrapper around std::queue to make it a thread safe queue

template <typename T>
class ThreadSafeQueue
{
public:
    ThreadSafeQueue(Logger *l = nullptr) : l(l)
    {
        mutex_init(&mutex);
        q = std::queue<T>();
    }

    ~ThreadSafeQueue() = default;

    inline bool push(T value)
    {
        // Try to take ownership of the mutex for 2ms outherwise give up
        if (mutex_try_enter(&mutex, &proc))
        {
            q.push(value);
            mutex_exit(&mutex);
            return true;
        }
        else
        {
            if (l != nullptr)
            {
                l->log(Logger::LogLevel::ERROR, "Failed to take ownership of the mutex");
            }
            return false;
        }
    }

    inline bool pop(T& item)
    {
        if (mutex_try_enter(&mutex, &proc))
        {
            if (q.empty())
            {
                mutex_exit(&mutex);
                return false;
            }

            item  = q.front();
            q.pop();
            mutex_exit(&mutex);
            return true; 
        }
        else
        {
            if (l != nullptr)
            {
                l->log(Logger::LogLevel::ERROR, "Failed to take ownership of the mutex");
            }
            return  false;
        }
    }

private:
    std::queue<T> q;
    mutex_t mutex;
    Logger *l;
    uint32_t proc;
};

#endif // __FIFO_HPP__