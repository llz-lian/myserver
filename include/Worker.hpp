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
    friend void eventHandle(Event * event,Worker * w);
    //server: threadpool.submit(run);
    //epoll wait   send event to sub_worker  check fd time out 
    Worker(HandleMap & handlemap,int sub_workers,std::string & pool_belong)
        :__handleMap(handlemap),__sub_workers(sub_workers,pool_belong),__worker_epoll(WORKER_SLEEP_TIMES)
    {
        active_fd_num = 0;
    }
    Worker(const Worker & w)
        :__handleMap(w.__handleMap),__sub_workers(w.__sub_workers),__worker_epoll(w.__worker_epoll)
    {
        active_fd_num = 0;
    }
    Worker(Worker && w)
        :__handleMap(w.__handleMap),__sub_workers(w.__sub_workers),__worker_epoll(w.__worker_epoll)
    {
        active_fd_num = 0;
    }
    ~Worker()
    {
        for(auto && [fd,event]:fd_events)
        {
            delete event;
            event = nullptr;
        }
        for(auto && [fd,mutex]:event_locks)
        {
            delete mutex;
            mutex = nullptr;
        }
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
        __handleMap.bindHandle(complete_func,"COMPLETE");
    }

    void work()
    {
        while (true)
        {
            if(active_fd_num == 0)
            {
                std::unique_lock<std::mutex> lock(__lock);
                __check_fd_num.wait(lock);
            }
            //fd num > 0
            //wait_ret = [int num_active,epoll_event* events]
            auto [num_active,ret_events] = __worker_epoll.wait();
            std::cout<<"epool return\n";
            if(num_active<0)
            {
                if(errno==EINTR)
                    continue;
                break;
            }
            for(int i = 0;i<num_active;i++)
            {
                int now_fd = ret_events[i].data.fd;
                //events send to sub-worker
                auto && now_events = fd_events[now_fd];
                if(ret_events[i].events&EPOLLRDHUP)
                {
                    #ifdef DEBUG
                    std::cout<<"fd is closed:"<<now_fd<<std::endl;
                    #endif
                    std::cout<<"fd is closed:"<<now_fd<<std::endl;
                    //close read
                    //COMPLETE => CLOSE
                    if(now_events!=nullptr&&now_events->state == Event::COMPLETE)
                    {
                        now_events->state = Event::NEED_CLOSE;
                        __sub_workers.submit(Worker::__eventHandle,now_events,this);
                    }
                }else if(!now_events)
                {
                    //create and init events
                    Event * new_event = new Event(__handleMap,now_fd,&__sub_workers);
                    fd_events[now_fd] = new_event;
                    if(event_locks[now_fd]==nullptr)
                        event_locks[now_fd] = new std::mutex();
                    __sub_workers.submit(Worker::__eventHandle,now_events,this);
                }
                else if(now_events->state==Event::COMPLETE && ret_events[i].events&EPOLLIN && ret_events[i].events&EPOLLOUT)
                {
                    __sub_workers.submit(Worker::__eventHandle,now_events,this);
                }
            }
            for(auto [fd,events]:fd_events)
            {
                //poccess events timeout
                //events must be COMPLETE
                //set CLOSE
            }
        }
    }
    void addFd(int fd)
    {
        __worker_epoll.addFd(fd);
        if(active_fd_num == 0)
            __check_fd_num.notify_all();
        active_fd_num++;
    }
    void closeFd(Event * event)
    {
        int fd = event->fd;
        __worker_epoll.removeFd(fd);
        fd_events[fd] = nullptr;
        delete event;
        // ::close(fd);
        active_fd_num--;
        #ifdef DEBUG
        std::cout<<"call closeFd:"<<fd<<std::endl;
        #endif
        //outside event not null
    }
    void completeFd(Event * event)
    {   
        int fd = event->fd;
        //do something
        //change to WAIT_READ
        event->resetFlags();
        Event::toNextState(event);
        // std::cout<<"call complete\n";
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
    std::unordered_map<int,std::mutex*> event_locks;//grow big
    //every worke has its own map
    HandleMap __handleMap;
    static void __eventHandle(Event * event,Worker * w)
    {
        if(event == nullptr)
            return;
        int fd = event->fd;
        {
            std::unique_lock<std::mutex> lock(*(w->event_locks[fd]));
            //run
            event->getHandle()();
        }
        //unlock
        if(w->fd_events[fd]==nullptr)
        {
            //removed lock
            delete w->event_locks[fd];
            w->event_locks[fd] = nullptr;
        }
    }
};