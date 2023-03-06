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

    void run(std::vector<Worker*> & workers)
    {
        while (true)
        {
            int ret = epoll_wait(__epoll_fd,&__server_fd_event,1,-1);
            if(ret<0)
            {
                if(errno == EINTR)
                    continue;
                std::cerr<<"acceptor epoll wait return -1:"<<strerror(errno)<<std::endl;
                throw std::runtime_error("epoll wait what?\n");
            }
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
                    //non block
                    setNonBlock(client_fd);
                    std::cout<<"arrive fd:"<<client_fd<<std::endl;
                    workers[now_choose_workers]->addFd(client_fd);
                    now_choose_workers = (now_choose_workers + 1) % __num_workers;
                }else
                {
                    std::cerr<<"???\n";
                    throw std::runtime_error("epoll listen what?\n");
                }
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
    friend void foo(Server * server,int i);
    Server():__workers_pool(NUM_WORKERS,"Server::__workers_pool"),__acceptor(SERVER_PORT,NUM_WORKERS)
    {

    };
    ~Server()
    {
        for(int i = 0;i<NUM_WORKERS;i++)
        {
            delete __workers[i];
            __workers[i] = nullptr;
        }
    }
    void init(std::vector<std::function<void (Event *)>> handles)
    {
        __map.bindHandle(handles[0],"READ");
        __map.bindHandle(handles[1],"PROCCESS");
        __map.bindHandle(handles[2],"WRITE");
        __workers.resize(NUM_WORKERS);
        for(int i = 0;i<NUM_WORKERS;i++)
        {
            std::string belong = "__workers[" + std::to_string(i) + "]";
            __workers[i] = new Worker(__map,NUM_WORKERS_THREADS,belong);
        }
        for(auto && worker:__workers)
        {
            worker->init();
        }
    }
    static void foo(Server * server,int i)
    {
        server->__workers[i]->run();
    }
    void run()
    {
        for(int i = 0;i<NUM_WORKERS;i++)
        {
            // auto run = [this,i](){
            //     this->__workers[i].run();
            // };
            __workers_pool.submit(Server::foo,this,i);
        }
        __acceptor.run(__workers);
    }
private:
    std::ThreadPool __workers_pool;
    //Worker(HandleMap & handlemap,int sub_workers)
    std::vector<Worker*> __workers;


    HandleMap __map;
    Acceptor __acceptor;

};