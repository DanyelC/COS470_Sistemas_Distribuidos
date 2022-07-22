/*
  Autor: https://murphypei.github.io/
  Código original: https://gist.github.com/murphypei/d59cbcdf6c8485ed98510dc1f0b3ddca
*/

#ifndef SAFE_QUEUE
#define SAFE_QUEUE

#include <condition_variable>
#include <mutex>
#include <queue>

// A threadsafe-queue.
template <class T>
class SafeQueue
{
public:
    SafeQueue() : q(), m(), c() {}

    ~SafeQueue() {}

    // Add an element to the queue.
    void enqueue(T t)
    {
        std::lock_guard<std::mutex> lock(m);
        q.push(t);
        c.notify_one();
    }

    // Get the front element.
    // If the queue is empty, wait till a element is avaiable.
    T dequeue(void)
    {
        std::unique_lock<std::mutex> lock(m);
        while (q.empty())
        {
            // release lock as long as the wait and reaquire it afterwards.
            c.wait(lock);
        }
        T val = q.front();
        q.pop();
        return val;
    }

    //Return a copy of the queue
    std::queue<T> copy_queue()
    {
        std::lock_guard<std::mutex> lock(m);
        std::queue<T> copy_q = q;
        return copy_q;    
    }

private:
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable c;
};
#endif