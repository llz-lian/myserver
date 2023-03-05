#pragma once
#include"Handle.hpp"
#include"Worker.hpp"
#include"config.hpp"
#include"socket_stuff.hpp"
#include<sys/socket.h>
class Acceptor
{
public:
    Acceptor(int port,int num_workers):__num_workers(num_workers)
    {
        //set addr
        __server_fd = getServerFd(SERVER_PORT);
        __epoll_fd = epoll_create(1);
        if(__epoll_fd<0)
        {
            std::cerr<<"Acceptor epoll err:"<<strerror(errno)<<std::endl;
            close(__server_fd);
            throw std::runtime_error("bad epoll fd\n");
        }
        //add server_fd to listen
        ::epoll_event server_event;
        bzero(&server_event,sizeof(server_event));
        server_event.data.fd = __server_fd;
        //maybe not et
        server_event.events = EPOLLIN; // |EPOLLET;
        int add_epoll_listen_ret = epoll_ctl(__epoll_fd,EPOLL_CTL_ADD,__server_fd,&server_event);
        if(add_epoll_listen_ret<0)
        {
            std::cerr<<"Acceptor epoll err:"<<strerror(errno)<<std::endl;
            close(__server_fd);
            throw std::runtime_error("bad server\n");
        }
    }

    void run(std::vector<Worker> & workers)
    {
        int ret = epoll_wait(__epoll_fd,&__server_fd_event,1,-1);
        for(int i = 0;i<ret;i++)
        {
            int server_fd = __server_fd_event.data.fd;
            if(server_fd == __server_fd)
            {
                sockaddr_in client_addr;
                bzero(&client_addr,sizeof(sockaddr_in));
                socklen_t addr_len;
                int client_fd = ::accept(server_fd,(sockaddr * )(&client_addr),&addr_len);
                if(client_fd<0)
                {
                    //accept error
                    continue;
                }
                workers[now_choose_workers++].addFd(client_fd);
            }else
            {
                std::cerr<<"???\n";
                throw std::runtime_error("epoll listen what?\n");
            }
        }
    }
private:
    int __server_fd;
    int __epoll_fd;
    int __num_workers;
    int now_choose_workers = 0;
    ::epoll_event __server_fd_event;
};

class Server
{
public:
    Server():__workers_pool(NUM_WORKERS),__acceptor(SERVER_PORT,NUM_WORKERS)
    {

    };
    void init(std::vector<std::function<void (Event *)>> handles)
    {
        __map.bindHandle(handles[0],"READ");
        __map.bindHandle(handles[1],"WRITE");
        __map.bindHandle(handles[2],"PROCCESS");
        for(int i = 0;i<NUM_WORKERS;i++)
        {
            __workers.emplace_back(Worker(__map,NUM_WORKERS_THREADS));
        }
        for(auto && worker:__workers)
        {
            worker.init();
        }
    }
    void run()
    {
        for(auto && worker:__workers)
        {
            auto run = [&](){
                worker.run();
            };
            __workers_pool.submit(run);
        }
        __acceptor.run(__workers);
    }
private:
    std::ThreadPool __workers_pool;
    //Worker(HandleMap & handlemap,int sub_workers)
    std::vector<Worker> __workers;
    HandleMap __map;
    Acceptor __acceptor;

};