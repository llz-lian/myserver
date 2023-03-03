#include"socket_stuff.h"
#include"recv_send.h"

const size_t MAX_BUFFER_SIZE = 256;

char send_buffer[MAX_BUFFER_SIZE];
char write_buffer[MAX_BUFFER_SIZE];

int main()
{
    bzero(send_buffer,sizeof(char)*MAX_BUFFER_SIZE);
    bzero(write_buffer,sizeof(char)*MAX_BUFFER_SIZE);
    //set server address
    int port =  8888;
    sockaddr_in server_addr;
    bzero(&server_addr,sizeof(sockaddr_in));

    server_addr.sin_port = htons(port);
    server_addr.sin_family = PF_INET;
    //set ip if u want
    //inet_pton(PF_INET,your_ip,&server_addr.sin_addr);
    
    //create scoket
    int server_sock_fd = socket(PF_INET,SOCK_STREAM,0);
    if(server_sock_fd<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }
    int ret_bind = ::bind(server_sock_fd,(const sockaddr*)(&server_addr),sizeof(sockaddr_in));
    if(ret_bind<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }
    int listen_ret = ::listen(server_sock_fd,10);
    if(listen_ret<0)
    {
        std::cerr<<strerror(errno)<<std::endl;
        return -1;
    }
    //poll
    



}