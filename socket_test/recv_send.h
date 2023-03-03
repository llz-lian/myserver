#include"socket_stuff.h"
#include<iostream>
#include<stdexcept>
void sendMessage(int sendFd,const char * message,const int message_len)
{
    int send_ret = ::send(sendFd,message,message_len,0);
    if(send_ret<0)
    {
            std::cerr<<strerror(errno)<<std::endl;
            throw std::runtime_error("send error");
    }
}
void recvMessage(int recvFd, char * buf,const int buf_len)
{
    bzero(buf,buf_len*sizeof(char));
    int recv_ret = ::recv(recvFd,buf,buf_len,0);
    if(recv_ret<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        throw std::runtime_error("recv error");
    }
}