#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<string.h>
#include<iostream>
#include<errno.h>


int main()
{
    //client
    //build clien address
    const char * ip = "192.168.1.19\0";//my ip
    sockaddr_in client_addr;
    bzero(&client_addr,sizeof(sockaddr_in));

    client_addr.sin_port = htons(9999);
    client_addr.sin_family = PF_INET;
    inet_pton(PF_INET,ip,&client_addr.sin_addr);


    //create socket
    int socket_fd = socket(PF_INET,SOCK_STREAM,0);
    //you can bind
    int bind_ret = ::bind(socket_fd,(sockaddr *)(&client_addr),sizeof(sockaddr_in));


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
    char send_buf[256] = "hi server\0";
    int send_ret = ::send(socket_fd,send_buf,strlen(send_buf),0);

    std::cout<<"send a message to server:"<<send_buf<<std::endl;

    //recv from server
    char buff[256];
    bzero(buff,sizeof(char)*256);

    int recv_ret = ::recv(socket_fd,buff,256-1,0);
    std::cout<<"recv from server:"<<buff<<std::endl;

}