#ifndef QUEUE_H
#define QUEUE_H

#include <queue>
#include <mutex>
#include <iostream>
#include <condition_variable>

template<typename T>
class ConcurrentQueue {
public:
    void push(T val) {
        std::unique_lock<std::mutex> locker(mu);
        data.push(val);
        locker.unlock();
        cond.notify_one();
    }
    T pop() {
        std::unique_lock<std::mutex> locker(mu);
        if (data.empty()) {
            std::cout << "HERE!" << std::endl;
            cond.wait(locker, [&]() { return !data.empty(); });
        }
        T val = data.front();
        data.pop();
        locker.unlock();
        cond.notify_one();
        return val;
    }
    size_t size() const {return data.size();}
    bool empty() const {return data.empty();}
    void release() {cond.notify_one();}

private:
    std::queue<T> data;
    std::mutex mu;
    std::condition_variable cond;
};

#endif
