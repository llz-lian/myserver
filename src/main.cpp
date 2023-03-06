//bind test
#include<iostream>
#include"include/Server.hpp"
#include<time.h>
void MyRead(Event *event)
{
    // if(event->state == Event::NEED_CLOSE)
    //     return;
    // int read_fd = event->fd;
    // if(!recvMessageNonBlock(event,&event->read_buffer[0],event->read_buffer_size))
    // {
    //     event->state = Event::NEED_CLOSE;
    // }
    // std::cout<<"call read\n";
}
void MyProcess(Event *event)
{
    // if(event->state == Event::NEED_CLOSE)
    //     return;
    // //prepare write buffer and write stuff
    // event->write_buffer = "<html>hello client</html>";
    // event->write_bytes = 0;
    // event->process_complete_flag = true;
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
    // std::string http_head = "HTTP/1.0 200 OK\r\n";
    // http_head += "Server:Linux Web Server \r\n";
    // http_head += "Content-Length:2048\r\n";
    // http_head += "Content-type:text/html\r\n\r\n";
    // if(!sendMessageNonBlock(event,&http_head[0],http_head.size()))
    // {
    //     //set event state
    //     event->state = Event::NEED_CLOSE;
    // }
    // //send body
    // event->write_bytes=0;
    // if(!sendMessageNonBlock(event,&event->write_buffer[0],event->write_buffer.size()))
    // {
    //     //set event state
    //     event->state = Event::NEED_CLOSE;
    // }
    // std::cout<<"call write\n";
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