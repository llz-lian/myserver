#include"socket_stuff.h"
#include"recv_send.h"
#include<iostream>
#include<stdexcept>
char buffer[256];

int main()
{
    bzero(buffer,sizeof(char)*256);
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
    while(true)
    {
        //accept connect
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
        recvMessage(client_fd,buffer,256);
        bzero(buffer,sizeof(char)*256);
        strncpy(buffer,"hellow client\0",15);
        sendMessage(client_fd,buffer,strlen(buffer));
    }
    

}