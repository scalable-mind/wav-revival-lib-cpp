#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>

template<typename T>
class Queue {
public:
    void push(T val) {
        std::unique_lock<std::mutex> locker(mu);
        data.push(val);
        locker.unlock();
        cond.notify_one();
    }
    T pop() {
        std::unique_lock<std::mutex> locker(mu);
        T val = data.front();
        data.pop();
        locker.unlock();
        cond.notify_one();
        return val;
    }
    size_t size() const {return data.size();}
private:
    std::queue<T> data;
    std::mutex mu;
    std::condition_variable cond;
};

#endif
