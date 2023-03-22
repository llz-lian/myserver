#pragma once
#include"ThreadPool.hpp"
#include"Handle.hpp"
#include"Event.hpp"
#include"Epoll.hpp"
#include"shared_queue.hpp"
#include"Timer.hpp"
#include<sys/socket.h>
#include<sys/eventfd.h>
#include<unordered_map>
#include<mutex>
#include<condition_variable>
#include<unistd.h>
#include<atomic>
#include <shared_mutex> 
class Event;
class Worker
{
public:
    //server: threadpool.submit(run);
    //epoll wait   send event to sub_worker  check fd time out 
    Worker(int sub_workers,std::string & pool_belong);
    Worker(const Worker & w);
    Worker(Worker && w);
    ~Worker();
    void init();

    void work();

    bool addFd(int fd);
    void closeFd(Event * event);
    void completeFd(Event * event);
    void handleClosefd();
    void handleWaitQueue();
    void handelTimeOut();
    void run()
    {
        work();
    }
    std::atomic<int> active_fd_num = 0;
    int notify_fd = 0;
    SharedQueue<int> wait_add_queue;
    SharedQueue<int> wait_close_queue;
    SharedQueue<int> time_out_queue;
private:
    //event->read() => event->poccess(const char *) => event->write(const char *)
    std::ThreadPool __sub_workers;

    Epoll __worker_epoll;

    std::shared_mutex __map_write_lock;

    std::unordered_map<int,Event*> fd_events;
    // std::unordered_map<int,std::mutex*> event_locks;//grow big
    
    TimerList timer;


    //every worke has its own map
    static void __eventHandle(Event * event,Worker * w);
};