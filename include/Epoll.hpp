#pragma once
#include<sys/epoll.h>
#include<vector>
#include<string.h>
#include<iostream>
#include<errno.h>
#include<tuple>
#include"config.hpp"

class Epoll
{
public:
    Epoll()
    {
        //create epoll
        __epoll_fd = epoll_create(1);
        if(__epoll_fd<0)
        {
            std::cerr<<"Epoll create: "<<strerror(errno)<<std::endl;
        }
    }
    bool addFd(int fd)
    {
        ::epoll_event now_event;
        bzero(&now_event,sizeof(epoll_event));
        //fd should set non_block
        now_event.events = EPOLLIN|EPOLLOUT|EPOLLET|EPOLLRDHUP;
        now_event.data.fd = fd;
        return __epoll_ctl(EPOLL_CTL_ADD,fd,&now_event);
    }

    bool removeFd(int fd)
    {
        return __epoll_ctl(EPOLL_CTL_DEL,fd,nullptr);
    }
    auto wait()
    {
        int epoll_wait_ret = epoll_wait(__epoll_fd,__events,MAX_EPOLL_LISTEN_EVENTS,-1);
        if(epoll_wait<0)
        {
            std::cerr<<"Epoll wait: "<<strerror(errno)<<std::endl;
            return std::pair<int,epoll_event*>(epoll_wait_ret,__events);
        }
        return std::pair<int,epoll_event*>(epoll_wait_ret,__events);
    }
private:
    int __epoll_fd;
    ::epoll_event __events[MAX_EPOLL_LISTEN_EVENTS];
    bool __epoll_ctl(int op,int fd,::epoll_event * now_event)
    {
        int ctl_ret = epoll_ctl(__epoll_fd,op,fd,now_event);
        if(ctl_ret<0)
        {
            std::cerr<<"Epoll addFd: "<<strerror(errno)<<std::endl;
            return false;
        }
        return true;
    }
};
