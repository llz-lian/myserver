#include<functional>
#include<queue>
class ThreadPool
{
private:
    using task = std::function<void>;
    std::queue<task> queue;
public:
    ThreadPool(/* args */){};
    ~ThreadPool(){};
};
