#include"socket_stuff.h"
#include"recv_send.h"
#include<iostream>
#include<stdexcept>
#include<vector>
char read_buffer[256];
char write_buffer[256];
int main()
{
    bzero(read_buffer,sizeof(char)*256);
    bzero(write_buffer,sizeof(char)*256);
    //prepare address
    //port
    int port = 8888;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = PF_INET;
    inet_pton(PF_INET,"127.0.0.1",&addr.sin_addr);

    addr.sin_port = htons(port);

    //create scoket
    int sock_fd = socket(PF_INET,SOCK_STREAM,0);
    //bind fd and addr
    int ret = bind(sock_fd,(sockaddr*)&addr,sizeof(addr));
    if(ret<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }
    ret = listen(sock_fd,5);

    fd_set read_fds;
    fd_set write_fds;
    fd_set execption_fds;
    
    std::vector<int> client_fds;

    while(true)
    {
        //accept connect
        //server socket
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&execption_fds);
        for(int client_fd:client_fds)
        {
            FD_SET(client_fd,&read_fds);
            FD_SET(client_fd,&write_fds);
            FD_SET(client_fd,&execption_fds);
        }
        FD_SET(sock_fd,&read_fds);
        FD_SET(sock_fd,&execption_fds);
        int select_ret = ::select(1000,&read_fds,&write_fds,&execption_fds,nullptr);
        if(select_ret<0)
        {
            std::cout<<"select failure\n";
            break;
        }
        if(FD_ISSET(sock_fd,&read_fds))
        {
            //sock_fd can read
            sockaddr_in client_addr;
            socklen_t client_addr_len = sizeof(client_addr);
            int client_fd = accept(sock_fd,(sockaddr * )(&client_addr),&(client_addr_len));
            if(client_fd<0)
            {
                std::cerr<<strerror(errno)<<std::endl;
                return -1;
            }
            char remote[INET_ADDRSTRLEN];
            std::cout<<"connect success with ip:"<<inet_ntop(PF_INET,&client_addr.sin_addr,remote,INET_ADDRSTRLEN)
                    <<" port:"<<ntohs(client_addr.sin_port)
                    <<" client fd:"<<client_fd<<std::endl;
            client_fds.push_back(client_fd);
        }
        else
        {
            int index = 0;
            bool closed = false;
            for(int client_fd:client_fds)
            {
                if(FD_ISSET(client_fd,&execption_fds))
                {
                    //execptinos
                    
                }
                if(FD_ISSET(client_fd,&read_fds))
                {
                    if(!recvMessage(client_fd,read_buffer,256))
                    {
                        //closed
                        closed = true;
                        client_fds.erase(client_fds.begin()+index);
                    }
                    else{
                        std::cout<<"recv message from client:"<<read_buffer<<std::endl;
                    }
                    //only read something server write something
                    //ortherwise the server try to send message to client as much as it can
                    if(FD_ISSET(client_fd,&write_fds)&&!closed)
                    {
                        bzero(write_buffer,sizeof(char)*256);
                        strncpy(write_buffer,"hellow client\0",15);
                        if(!sendMessage(client_fd,write_buffer,strlen(write_buffer)))
                        {
                            closed = false;
                            client_fds.erase(client_fds.begin()+index);
                        }
                    }
                }
                index++;
            }
        }

    }
    

}