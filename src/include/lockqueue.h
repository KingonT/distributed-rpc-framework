#pragma once
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

template<typename T>
class LockQueue
{
public:
    void push(const T& t)
    {
        std::lock_guard<std::mutex>  lock(l_mutex);
        l_queue.push(t);
        l_cond.notify_one();
    }
    
    T pop()
    {
        std::unique_lock<std::mutex>  lock(l_mutex);
        while (l_queue.empty())
        {
            l_cond.wait(lock);
        }
        T t = l_queue.front();
        l_queue.pop();
        return t;
    }

private:
    std::queue<T>  l_queue;
    std::mutex     l_mutex;
    std::condition_variable  l_cond;
};
