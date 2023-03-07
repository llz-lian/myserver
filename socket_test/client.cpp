#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<iostream>
#include<errno.h>
#include<unistd.h>
#include<time.h>
#include"recv_send.h"
char read_buffer[256];
char write_buffer[256];
int main()
{
    bzero(read_buffer,sizeof(char)*256);
    bzero(write_buffer,sizeof(char)*256);

    //client
    //build clien address
    const char * ip = "192.168.1.19\0";//my ip
    sockaddr_in client_addr;
    bzero(&client_addr,sizeof(sockaddr_in));

    client_addr.sin_port = htons(1234);
    client_addr.sin_family = PF_INET;
    inet_pton(PF_INET,ip,&client_addr.sin_addr);


    //create socket
    int socket_fd = socket(PF_INET,SOCK_STREAM,0);
    //you can bind
    // int bind_ret = ::bind(socket_fd,(sockaddr *)(&client_addr),sizeof(sockaddr_in));


    //prepare sercer address
    sockaddr_in server_addr;
    bzero(&server_addr,sizeof(sockaddr_in));
    server_addr.sin_port = htons(8888);
    server_addr.sin_family = PF_INET;
    inet_pton(PF_INET,"127.0.0.1\0",&server_addr.sin_addr);

    //now connect

    int connect_ret = ::connect(socket_fd,(sockaddr*)(&server_addr),sizeof(sockaddr_in));
    //if you not start server
    //connect return -1
    if(connect_ret<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }

    //connect complete
    //socket_fd <------> server
    //send something
    bzero(write_buffer,sizeof(char)*256);
    strncpy(write_buffer,"GET / HTTP/1.0\r\nUser-Agent: client 1.0\r\nHost: 127.0.0.1\r\n\r\n",60);    
    sendMessage(socket_fd,write_buffer,strlen(write_buffer));
    std::cout<<"send a message to server:"<<write_buffer<<std::endl;

    recvMessage(socket_fd,read_buffer,256);
    std::cout<<"recv from server:"<<read_buffer<<std::endl;
    sleep(5);

    bzero(write_buffer,sizeof(char)*256);
    strncpy(write_buffer,"GET / HTTP/1.0\r\nUser-Agent: client 1.0\r\nHost: 127.0.0.1\r\n\r\n",60);

    sendMessage(socket_fd,write_buffer,strlen(write_buffer));
    std::cout<<"send a message to server:"<<write_buffer<<std::endl;
    recvMessage(socket_fd,read_buffer,256);
    std::cout<<"recv from server:"<<read_buffer<<std::endl;

    // sendMessage(socket_fd,send_buf,strlen(send_buf));
    // std::cout<<"send a message to server:"<<send_buf<<std::endl;
    // //recv from server
    // recvMessage(socket_fd,buff,256);
    // std::cout<<"recv from server:"<<buff<<std::endl;
    sleep(50000);
    //close write but can read
    ::shutdown(socket_fd,SHUT_WR);
    // recvMessage(socket_fd,read_buffer,256);
    // std::cout<<"recv from server:"<<read_buffer<<std::endl;
}