//bind test
#include<iostream>
#include"include/Server.hpp"

void MyRead(Event *event)
{
    int read_fd = event->fd;
    recvMessageNonBlock(event,&event->read_buffer[0],event->read_buffer_size);
    std::cout<<"call read\n";
}
void MyProcess(Event *event)
{
    //prepare write buffer and write stuff
    event->write_buffer = "hello client";
    event->write_bytes = 0;
    event->process_complete_flag = true;
    std::cout<<"call process\n";
}
void MyWrite(Event *event)
{
    sendMessageNonBlock(event,&event->write_buffer[0],event->write_buffer.size());
    std::cout<<"call write\n";
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