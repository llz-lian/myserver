#include<queue>
#include<stdexcept>
#include<shared_mutex>

template<class T>
class SharedQueue
{
private:
    std::queue<T> __queue;
    std::shared_mutex __lock;
public:
    template<class I=T>
    void push(I && val)
    {
        std::unique_lock<std::shared_mutex> lock;
        __queue.push(val);
    }
    T front()
    {
        std::shared_lock<std::shared_mutex> lock;
        if(__queue.empty())
            throw std::runtime_error("no next task.");
        return __queue.front();
    }
    void pop()
    {
        std::unique_lock<std::shared_mutex> lock;
        if(__queue.empty())
            throw std::runtime_error("no next task.");
        __queue.pop();
    }
    size_t size()
    {
        std::shared_lock<std::shared_mutex> lock;
        return __queue.size();
    }
};