#include"socket_stuff.h"

#include<iostream>



int main()
{

    //prepare address
    //port
    int port = 8888;
    struct sockaddr_in addr;
    bzero(&addr,sizeof(addr));
    addr.sin_family = PF_INET;
    // inet_pton(PF_INET,INADDR_ANY,&addr.sin_addr);
    addr.sin_port = htons(port);    

    //create scoket
    int sock_fd = socket(PF_INET,SOCK_STREAM,0);
    //bind fd and addr
    int ret = bind(sock_fd,(sockaddr*)&addr,sizeof(addr));
    if(ret<0)
        std::cout<<errno<<std::endl;

    ret = listen(sock_fd,5);
    while(true)
    {
        sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int ret = accept(sock_fd,(sockaddr * )(&client_addr),&(client_addr_len));

    }


}