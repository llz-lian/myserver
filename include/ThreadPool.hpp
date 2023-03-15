#pragma once

#include<functional>
#include<thread>
#include<queue>
#include<mutex>
#include<atomic>
#include<memory>
#include<future>
#include<stdexcept>
#include<condition_variable>
#include<string>
template<class T>
class LockQueue
{
private:
    std::mutex __mutex;
    std::queue<T> __queue;
public:
    LockQueue(){}
    LockQueue(LockQueue & ){}
    LockQueue(LockQueue && ){}
    size_t size()
    {
        std::unique_lock<std::mutex> lock(__mutex);
        return __queue.size();
    }
    bool empty()
    {
        std::unique_lock<std::mutex> lock(__mutex);
        return __queue.empty();
    }
    void push(T&t)
    {
        std::lock_guard<std::mutex> lock(__mutex);
        __queue.emplace(t);
    }
    void push(T&&t)
    {
        std::lock_guard<std::mutex> lock(__mutex);
        __queue.emplace(t);
    }
    void pop()
    {
        std::unique_lock<std::mutex> lock(__mutex);
        __queue.pop();
    }
    T front()
    {
        std::unique_lock<std::mutex> lock(__mutex);
        if(__queue.empty())
            throw std::runtime_error("no next task.");
        return __queue.front();
    }
};
#ifdef DEBUG_POOL
#include<iostream>
#endif

namespace std
{
class ThreadPool
{
private:
    using Task = function<void()>;
    LockQueue<Task> __task_queue;
    vector<thread> __threads;
    mutex __lock;
    bool __isrun = false;
    condition_variable __notify_lock;
    int thread_num = 0;
    string belong;
    void __addOneThreads()
    {
        if(!__isrun)
            return;
        __threads.emplace_back(
            [this]()
            {
                while(true)
                {
                    if(!this->__isrun)
                    {
                        //out
                        break;
                    }
                    {
                        {
                            //block at cv
                            //wait_lock must RTTI
                            unique_lock<mutex> wait_lock(this->__lock);
                            #ifdef DEBUG_POOL
                            std::cout<< belong <<"block"<<std::endl;
                            #endif
                            this->__notify_lock.wait(wait_lock,[this](){
                                    return this->__isrun&&!__task_queue.empty();
                            });
                        }

                        if(this->__isrun)
                        {
                            //queue is not empty
                            if(!__task_queue.empty())
                            {
                                #ifdef DEBUG_POOL
                                cout<<"thread get task\n";
                                #endif
                                Task task;
                                {
                                    unique_lock<mutex> wait_lock(this->__lock);
                                    if(__task_queue.empty())
                                        continue;
                                    task = move(this->__task_queue.front());this->__task_queue.pop();
                                }
                                // auto && task = this->__task_queue.front();this->__task_queue.pop();

                                //do task
                                task();
                                #ifdef DEBUG_POOL
                                cout<<"thread done task\n";
                                #endif
                            }
                        }
                    }
                }
                #ifdef DEBUG_POOL
                cout<<"thread out\n";
                #endif
            }
        );
    }
public:
    ThreadPool(int num_threads,string belong):belong(belong)
    {
        thread_num = num_threads;
        __isrun = true;
        for(int i = 0;i<num_threads;i++)
            __addOneThreads();
    };
    ThreadPool(const ThreadPool & tp):belong(tp.belong)
    {
        thread_num = tp.thread_num;
        __isrun = true;
        for(int i = 0;i<thread_num;i++)
            __addOneThreads();
    };
    ThreadPool(const ThreadPool && tp):belong(tp.belong)
    {
        thread_num = tp.thread_num;
        __isrun = true;
        for(int i = 0;i<thread_num;i++)
            __addOneThreads();
    };


    ~ThreadPool(){
        #ifdef DEBUG_POOL
        cout<<"pool shutdown\n";
        #endif
        shutdown();
    };

    void shutdown(){
        __isrun = false;
        {
            unique_lock<mutex> lock(__lock);
            //pop all task
            while(__task_queue.size()>0)
            {
                __task_queue.pop();
            }   
            __isrun = false;
        }
        __notify_lock.notify_all();
        for(int i = 0;i<__threads.size();i++)
        {
            if(__threads[i].joinable())
            {
                __threads[i].join();
            }
        }

    }

    template<class Func,class ...Args>
    auto submit(Func && f,Args&&...args)
    {
        //bind function
        if(!__isrun)
            throw runtime_error("no thread pool.");
        using reture_type = decltype(f(args...));
        // auto bind_function = bind(forward<Func>(f),forward<Args>(args)...);
        //bind package
        // function<reture_type()> task_function = [&](){return bind_function();};
        auto task = make_shared<packaged_task<reture_type()>>(bind(forward<Func>(f),forward<Args>(args)...));
        Task warp_task = [task](){
            (*task)();
        };
        __task_queue.push(warp_task);
        __notify_lock.notify_one();
        return task->get_future();
    }
    // void forceNotify()
    // {
    //     __notify_lock.notify_one();
    // }
};





};