#pragma once
#include"Worker.hpp"
#include"config.hpp"
#include"socket_stuff.hpp"
#include<sys/socket.h>
#include<signal.h>
class Acceptor
{
public:
    Acceptor(int port,int num_workers):__num_workers(num_workers)
    {
        //set addr
        //true block false nonblock
        __server_fd = getServerFd(SERVER_PORT,false);
        // __epoll_fd = epoll_create(1);
        // if(__epoll_fd<0)
        // {
        //     std::cerr<<"Acceptor epoll err:"<<strerror(errno)<<std::endl;
        //     close(__server_fd);
        //     throw std::runtime_error("bad epoll fd\n");
        // }
        //add server_fd to listen
        // ::epoll_event server_event;
        // bzero(&server_event,sizeof(server_event));
        // server_event.data.fd = __server_fd;
        //maybe not et
        //no must et ortherwise send a null ack to client
        // server_event.events = EPOLLOUT|EPOLLET;
        // int add_epoll_listen_ret = epoll_ctl(__epoll_fd,EPOLL_CTL_ADD,__server_fd,&server_event);
        // if(add_epoll_listen_ret<0)
        // {
        //     std::cerr<<"Acceptor epoll err:"<<strerror(errno)<<std::endl;
        //     close(__server_fd);
        //     throw std::runtime_error("bad server\n");
        // }
    }

    void run(std::vector<Worker*> & workers)
    {
        while (true)
        {
            // int ret = epoll_wait(__epoll_fd,&__server_fd_event,1,-1);
            // if(ret<0)
            // {
            //     if(errno == EINTR)
            //         continue;
            //     std::cerr<<"acceptor epoll wait return -1:"<<strerror(errno)<<std::endl;
            //     throw std::runtime_error("epoll wait what?\n");
            // }
            // for(int i = 0;i<ret;i++)
            
            // int server_fd = __server_fd;//__server_fd_event.data.fd;
            sockaddr_in client_addr;
            bzero(&client_addr,sizeof(sockaddr_in));
            socklen_t addr_len = sizeof(client_addr);
            int client_fd = ::accept(__server_fd,(sockaddr * )(&client_addr),&addr_len);
            if(client_fd<0)
            {
                //accept error
                std::cout<<strerror(errno)<<std::endl;
                continue;
            }
            //non block
            setNonBlock(client_fd);
            // std::cout<<"arrive fd:"<<client_fd<<std::endl;
            // std::cout<<"send mission to worker id:"<<now_choose_workers<<std::endl;
            //maybe not do it in main thread
            // workers[now_choose_workers]->addFd(client_fd);
            //notify workers to add fd
            uint64_t u = client_fd;
            //unsafe need block
            
                int write_ret = write(workers[now_choose_workers]->notify_fd,&u,sizeof(uint64_t));
                if(write_ret<0)
                {
                    std::cerr<<strerror(errno)<<std::endl;
                }
                else{
                    workers[now_choose_workers]->wait_add_queue.push(client_fd);
                    now_choose_workers = (now_choose_workers + 1) % __num_workers;
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
        //sigalrm block
        //only timerlist can recv sigalrm
        // ::sigset_t mask;
        // ::sigemptyset(&mask);
        // ::sigaddset(&mask,SIGALRM);
        // ::pthread_sigmask(SIG_BLOCK,&mask,nullptr);
        ::signal(SIGPIPE,SIG_IGN);

        HandleMap::bindHandle(handles[0],"READ");
        HandleMap::bindHandle(handles[1],"PROCCESS");
        HandleMap::bindHandle(handles[2],"WRITE");

        __workers.resize(NUM_WORKERS);
        for(int i = 0;i<NUM_WORKERS;i++)
        {
            std::string belong = "__workers[" + std::to_string(i) + "]";
            __workers[i] = new Worker(NUM_WORKERS_THREADS,belong);
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
    Acceptor __acceptor;
};