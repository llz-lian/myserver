#include<queue>
#include<stdexcept>
#include<shared_mutex>
#include<condition_variable>
#include<mutex>
#include<iostream>
template<class T>
class SharedQueue
{
private:
    std::queue<T> __queue;
    std::shared_mutex __lock;
    // std::condition_variable __empty_lock;
public:
    SharedQueue(){}
    ~SharedQueue(){}

    template<class I=T>
    void push(I && val)
    {
        std::lock_guard<std::shared_mutex> lock(__lock);
        __queue.push(val);
    }
    T front() const
    {
        std::unique_lock<std::shared_mutex> lock(__lock);
        if(__queue.empty())
            throw std::runtime_error("no next task.");
        return __queue.front();
    }
    T pop()
    {
        std::unique_lock<std::shared_mutex> lock(__lock);
        if(__queue.empty())
            throw std::runtime_error("no next task.");
        T ret(std::move(__queue.front()));
        __queue.pop();
        return ret;
    }
    size_t size()
    {
        std::shared_lock<std::shared_mutex> lock(__lock);
        return __queue.size();
    }
};