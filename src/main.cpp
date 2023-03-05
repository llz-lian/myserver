//bind test
#include<iostream>
#include"include/Server.hpp"

void MyRead(Event *event)
{
    int read_fd = event->fd;
    event->read_buffer.resize(256);
    recvMessageNonBlock(event,&event->read_buffer[0],256);
}
void MyProcess(Event *event)
{
    //pass
}
void MyWrite(Event *event)
{
    int write_fd = event->fd;
    event->write_buffer = "hello client";
    sendMessageNonBlock(event,&event->write_buffer[0],event->write_buffer.size());
}
int main()
{
    Server server;
    std::vector<std::function<void (Event *)>> handles;
    std::function<void (Event *)> read_fuc = MyRead;
    std::function<void (Event *)> process_fuc = MyProcess;
    std::function<void (Event *)> write_fuc = MyWrite;
    handles[0] = read_fuc;
    handles[1] = process_fuc;
    handles[2] = write_fuc;
    server.init(handles);
}