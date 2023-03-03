#include"socket_stuff.h"
#include<iostream>
#include<stdexcept>
bool sendMessage(int sendFd,const char * message,const int message_len)
{
    int send_ret = ::send(sendFd,message,message_len,0);
    if(send_ret<0)
    {
            if(errno == EPIPE)
                return false;
            std::cerr<<strerror(errno)<<std::endl;
            throw std::runtime_error("send error");
    }
    std::cout<<"send "<<message_len<<" bytes\n";
    return true;
}
bool recvMessage(int recvFd, char * buf,const int buf_len)
{
    bzero(buf,buf_len*sizeof(char));
    int recv_ret = ::recv(recvFd,buf,buf_len,0);
    if(recv_ret<0)
    {
        if(errno == EINTR)
        {
            return true;
        }
        std::cerr<<strerror(errno)<<std::endl;
        throw std::runtime_error("recv error");
    }
    if(recv_ret==0)
    {
        //client closed
        return false;
    }
    return true;
}