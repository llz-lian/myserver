#pragma once
#include"ThreadPool.hpp"
#include"Event.hpp"
#include"Handle.hpp"
#include"Epoll.hpp"
#include<sys/socket.h>
#include<unordered_map>
#include<mutex>
#include<condition_variable>
#include<unistd.h>

class Worker
{
public:
    //server: threadpool.submit(run);
    //epoll wait   send event to sub_worker  check fd time out 
    Worker(HandleMap & handlemap,int sub_workers)
        :__handleMap(__handleMap),__sub_workers(sub_workers)
    {
        active_fd_num = 0;
    }

    Worker(Worker && w)
        :__handleMap(w.__handleMap),__sub_workers(w.__sub_workers)
    {
        active_fd_num = 0;
    }

    void init()
    {
        std::function<void(Event *)> close_func = [this](Event * event){
            this->closeFd(event);
        };
        __handleMap.bindHandle(close_func,"CLOSE");

        std::function<void(Event *)> complete_func = [this](Event * event)
        {
            this->completeFd(event);
        };
        __handleMap.bindHandle(close_func,"COMPLETE");
    }

    void work()
    {
        if(active_fd_num == 0)
        {
            std::unique_lock<std::mutex> lock(__lock);
            __check_fd_num.wait(lock);
        }
        //fd num > 0
        //wait_ret = [int num_active,epoll_event* events]
        auto [num_active,ret_events] = __worker_epoll.wait();
        for(int i = 0;i<num_active;i++)
        {
            int now_fd = ret_events[i].data.fd;
            //events send to sub-worker
            auto now_events = fd_events[now_fd];
            if(!now_events)
            {
                //create and init events
                Event * new_event = new Event(__handleMap,now_fd);
                fd_events[now_fd] = new_event;
                now_events = new_event;
            }
            if(ret_events[i].events&EPOLLRDHUP)
            {
                //close read
                now_events->setClose();
            }
            else if(ret_events[i].events&EPOLLIN && ret_events[i].events&EPOLLOUT)
            {
                __sub_workers.submit(now_events->getHandle());
            }
        }
        for(auto [fd,events]:fd_events)
        {
            //poccess events timeout
            //events must be COMPLETE
            //set CLOSE

        }
    }
    void addFd(int fd)
    {
        __worker_epoll.addFd(fd);
        if(active_fd_num == 0)
            __check_fd_num.notify_one();
        active_fd_num++;
    }

    void closeFd(Event * event)
    {
        int fd = event->fd;
        __worker_epoll.removeFd(fd);
        delete event;
        ::close(fd);
    }
    void completeFd(Event * event)
    {   
        int fd = event->fd;
        //do something
        //change to WAIT_READ
        event->toNextState();
        return;
    }

    void run()
    {
        work();
    }
private:
    //event->read() => event->poccess(const char *) => event->write(const char *)
    std::ThreadPool __sub_workers;

    Epoll __worker_epoll;
    int active_fd_num;

    std::condition_variable __check_fd_num;
    std::mutex __lock;

    std::unordered_map<int,Event*> fd_events;
    HandleMap & __handleMap;


};