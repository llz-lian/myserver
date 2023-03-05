#include"socket_stuff.h"
#include"recv_send.h"
#include<poll.h>
#include<vector>
const size_t MAX_BUFFER_SIZE = 256;

char read_buffer[MAX_BUFFER_SIZE];
char write_buffer[MAX_BUFFER_SIZE];

int main()
{
    bzero(read_buffer,sizeof(char)*MAX_BUFFER_SIZE);
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

    std::vector<::pollfd> poll_listen;

    ::pollfd server_listen_poll_fd;
    server_listen_poll_fd.fd = server_sock_fd;
    server_listen_poll_fd.events = POLLIN|POLLERR;
    poll_listen.push_back(server_listen_poll_fd);
    while(true)
    {
        int pool_ret = ::poll(&poll_listen[0],poll_listen.size(),-1);
        int n = poll_listen.size();
        for(int i = 0;i<n;i++)
        {
            ::pollfd & pollfd = poll_listen[i];
            if(pollfd.fd == server_sock_fd && (pollfd.revents&POLLIN))
            {
                //server recive a connect
                sockaddr_in client_addr;
                socklen_t size_addr;
                bzero(&client_addr,sizeof(client_addr));
                int client_fd = ::accept(server_sock_fd,(sockaddr *)(&client_addr),&size_addr);

                char remote[INET_ADDRSTRLEN];
                std::cout<<"connect success with ip:"<<inet_ntop(PF_INET,&client_addr.sin_addr,remote,INET_ADDRSTRLEN)
                    <<" port:"<<ntohs(client_addr.sin_port)
                    <<" client fd:"<<client_fd<<std::endl;

                //create a poll fd for client_fd
                ::pollfd client_listen_poll_fd;
                client_listen_poll_fd.fd = client_fd;

                //read write close
                client_listen_poll_fd.events = POLLIN|POLLOUT|POLLRDHUP;
                poll_listen.push_back(client_listen_poll_fd);
            }
            else
            {
                //client fd read or write or hup
                if(pollfd.revents&POLLRDHUP)
                {
                    //client closed
                    //you can say goodbye ↓
                    //send(pollfd.fd,"goodbye");
                    poll_listen.erase(poll_listen.begin()+i);
                }else
                {
                    if(pollfd.revents&POLLIN)
                    {
                        //can read
                        //read a message
                        recvMessage(pollfd.fd,read_buffer,MAX_BUFFER_SIZE);
                        std::cout<<"recive message from client:"<<read_buffer<<std::endl;
                        if(pollfd.revents&POLLOUT)
                        {
                            //can write
                            strncpy(write_buffer,"hellow client",15);
                            sendMessage(pollfd.fd,write_buffer,strlen(write_buffer));
                            std::cout<<"send :\""<<write_buffer<<"\" to client"<<std::endl;
                        }
                    }
                }
            }
        }
    }



}