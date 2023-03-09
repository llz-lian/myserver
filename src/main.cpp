//bind test
#include<iostream>
#include"include/Server.hpp"
#include<time.h>
void MyRead(Event *event)
{
    int read_fd = event->fd;
    event->read_bytes = 0;
    if(!recvMessageNonBlock(event))
    {
        //READ => CLOSE
        event->state = EventStuff::NEED_CLOSE;
    }
    // std::cout<<"call read\n";
}
void MyProcess(Event *event)
{
    //prepare write buffer and write stuff
    
    if(event->read_bytes!=0)
        event->write_buffer = "HTTP/1.0 200 OK\r\nContent-Type: text/html;charset=utf-8\r\nContent-Length: 88\r\n\r\n<html><head><title>Wrox Homepage</title></head><body>-- body goes here --</body></html>\0";
    // std::cout<<event->write_buffer<<std::endl;
    event->process_complete_flag = true;
    // std::cout<<"call process\n";
}
void MyWrite(Event *event)
{
    event->write_bytes=0;
    // std::cout<<event->write_buffer<<std::endl;
    if(!sendMessageNonBlock(event,event->write_buffer.c_str(),event->write_buffer.size()+1))
    {
        //set event state
        //WRITE => CLOSE
        event->state = EventStuff::NEED_CLOSE;
    }
    // std::cout<<"call write\n";
    // shutdown client
    event->state = EventStuff::NEED_CLOSE;
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