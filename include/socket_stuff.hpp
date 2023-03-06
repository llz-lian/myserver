#pragma once
#include<iostream>
//socket socketaddress
#include<sys/socket.h>

#include<unistd.h>
//htol htos ntohl ntohs
#include<netinet/in.h>
//inet_addr inet_aton inet_ntoa
#include<arpa/inet.h>
//errno
#include<errno.h>
//types
#include<sys/types.h>

//bzero memset
#include<string.h>
#include <unistd.h>
#include <fcntl.h> 
int setNonBlock(int fd)
{
    int now_flags = fcntl(fd,F_GETFL);
    int new_flags = now_flags|O_NONBLOCK;
    fcntl(fd,F_SETFL,new_flags);
    return now_flags;
}


int getServerFd(int port)
{
    sockaddr_in server_addr;
    bzero(&server_addr,sizeof(sockaddr_in));

    server_addr.sin_port = htons(port);
    server_addr.sin_family = PF_INET;
    //set ip if u want
    //inet_pton(PF_INET,your_ip,&server_addr.sin_addr);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //create scoket non block
    int server_sock_fd = socket(PF_INET,SOCK_STREAM|SOCK_NONBLOCK,0);
    if(server_sock_fd<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        throw std::runtime_error("bad server sockfd\n");
    }
    int ret_bind = ::bind(server_sock_fd,(const sockaddr*)(&server_addr),sizeof(sockaddr_in));
    if(ret_bind<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        throw std::runtime_error("bad bind\n");
    }
    int listen_ret = ::listen(server_sock_fd,10);
    if(listen_ret<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        throw std::runtime_error("bad listen\n");
    }

    return server_sock_fd;
}





bool sendMessageNonBlock(Event* event,const char * message,const int message_len)
{
    int sendFd = event->fd;
    int send_ret = ::send(sendFd,message+event->write_bytes,message_len-event->write_bytes,0);
    //send return how many bits it send.
    if(send_ret<0)
    {
            if(errno == EPIPE)
            {
                //client shutdown socket
                return false;
            }
            if(errno == EAGAIN)
            {
                //system buffer is full
                //must wait
                return true;
            }
            std::cerr<<strerror(errno)<<std::endl;
            // throw std::runtime_error("send error");
            return false;
    }
    event->write_bytes += send_ret;
    if(send_ret == 0||event->write_bytes == message_len)
    {
        //nothing to send
        event->write_complete_flag = true;
        #ifdef DEBUG
        std::cout<<"write complete"<<std::endl;
        #endif
    }
    // std::cout<<"send "<<message_len<<" bytes\n";
    return true;
}

bool recvMessageNonBlock(Event* event, char * buf,const int buf_len)
{
    int recvFd = event->fd;
    bzero(buf+event->read_bytes,(buf_len - event->read_bytes)*sizeof(char));
    int recv_ret = ::recv(recvFd,buf + event->read_bytes,buf_len - event->read_bytes,0);
    if(recv_ret<0)
    {
        if(errno == EINTR)
        {
            return true;
        }
        //client closed or read_complete
        if((errno==EAGAIN)||(errno==EWOULDBLOCK))
        {
            //read complete
            #ifdef DEBUG
            std::cout<<"read complete"<<std::endl;
            #endif
            event->read_complete_flag = true;
            return true;
        }
        std::cerr<<strerror(errno)<<std::endl;
        throw std::runtime_error("recv error");
    }
    event->read_bytes += recv_ret;
    #ifdef DEBUG
    std::cout<<event->read_buffer.size()<<std::endl;
    std::cout<<event->read_buffer<<std::endl;
    #endif
    if(event->read_bytes<event->read_buffer_size||recv_ret==0)
    {
        event->read_complete_flag = true;
        return true;
    }
    if(event->read_bytes == event->read_buffer_size)
    {
        event->read_buffer_size = event->read_buffer_size * 2;
        event->read_buffer.resize(event->read_buffer_size);
    }
    return true;
}