//bind test
#include<iostream>
#include"include/Server.hpp"
#include<time.h>
void MyRead(Event *event)
{
    int read_fd = event->fd;
    event->read_bytes = 0;
    if(!recvMessageNonBlock(event,&event->read_buffer[0],event->read_buffer_size))
    {
        //READ => CLOSE
        event->state = Event::NEED_CLOSE;
    }
    // std::cout<<"call read\n";
}
void MyProcess(Event *event)
{
    //prepare write buffer and write stuff
    // event->write_buffer = "HTTP/1.0 200 OK\r\nConten-Type: text/html; charset=utf-8\r\nContent-Length: 17\r\n\r\n<html>html</html>";
    if(event->read_bytes!=0)
        event->write_buffer = "HTTP/1.0 200 OK\r\nContent-Type: text/html;charset=utf-8\r\nContent-Length: 122\r\n\r\n<html><head><title>Wrox Homepage</title></head><body><!-- body goes here --></body></html>\0";
    // std::cout<<event->write_buffer<<std::endl;
    event->process_complete_flag = true;
    // std::cout<<"call process\n";
}
void MyWrite(Event *event)
{
    // if(event->state == Event::NEED_CLOSE)
    //     return;
    //if http 
    //buil head here
    //string http_head = buildHttpHead();
    //and send head
    // std::string http_head = "HTTP1.0 200 OK\r\n";
    // http_head += "Server: linux\r\n";
    // http_head += "\r\n";
    // if(!sendMessageNonBlock(event,http_head.c_str(),http_head.size()))
    // {
    //     //set event state
    //     event->state = Event::NEED_CLOSE;
    // }
    // //send body
    event->write_bytes=0;
    if(!sendMessageNonBlock(event,event->write_buffer.c_str(),event->write_buffer.size()+1))
    {
        //set event state
        //WRITE => CLOSE
        event->state = Event::NEED_CLOSE;
    }
    // std::cout<<"call write\n";
    // shutdown client
    event->state = Event::NEED_CLOSE;
}
int main()
{
    Server server;
    //prepare handles
    std::vector<std::function<void (Event *)>> handles = {MyRead,MyProcess,MyWrite};
    // also can
    // std::function<void (Event *)> read_fuc = MyRead;
    // std::function<void (Event *)> process_fuc = MyProcess;
    // std::function<void (Event *)> write_fuc = MyWrite;
    // handles[0] = read_fuc;
    // handles[1] = process_fuc;
    // handles[2] = write_fuc;
    //regist handles
    server.init(handles);
    //run server
    server.run();
}