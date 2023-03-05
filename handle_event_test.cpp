#include<iostream>
#include<functional>
#include<string>
#include<unordered_map>
using namespace std;


class A
{
public:
    A()
    {
        std::cout<<"default build\n";
    }
    A(int num):num(num){
        std::cout<<"build build\n";
    }
    A(A&&a)
    {
        std::cout<<"move construct\n";
        num = a.num;
    }
    A(A&a)
    {
        std::cout<<"copy copy\n";
        num = a.num;
    }
    int num = 0;
};
class Event;
class HandleMap
{
public:
    HandleMap(){};
    void bindHandle(std::function<void(Event * )> handle,std::string && method)
    {
        this->handle[method] = [handle](Event * event){
            handle(event);
        };
    }
    std::unordered_map<std::string,std::function<void(Event *)>> handle;
};

class Handle
{
public:
    Handle(HandleMap & map):handle_map(map){};
    std::function<void(Event *)> getHandle(std::string & s)
    {
        std::cout<<"get handle:"<<s<<std::endl;
        return handle_map.handle[s];
    }
    HandleMap & handle_map;
};

class Event
{
public:
    Event(HandleMap & map,int recive_fd):fd(recive_fd),handle(map),read_buffer(1),write_buffer(2){
        //create by user
        state_to_string[int(WAIT_READ)] = "READ";
        state_to_string[int(WAIT_PROCCESS)] = "PROCCESS";
        state_to_string[int(WAIT_WRITE)] = "WRITE";
        //create by worker
        state_to_string[int(NEED_CLOSE)] = "CLOSE";
    };
    enum STATE {WAIT_READ,WAIT_PROCCESS,WAIT_WRITE,NEED_CLOSE};
    int state = WAIT_READ;
    //handle
    A read_buffer;
    A write_buffer;
    int fd = 0;
    Handle handle;
    std::function<void()> getHandle()
    {
        std::function<void(Event *) > now_choose_handle = handle.getHandle(state_to_string[state]);
        return std::function<void()>([this,now_choose_handle](){now_choose_handle(this);});
    }
    void toNextState()
    {
        state = (state + 1)%(NEED_CLOSE);
    }
    std::unordered_map<int,std::string> state_to_string;
};



void read_foo(Event * event)
{
    event->read_buffer.num = 10;
    std::cout<<event->read_buffer.num<<std::endl;
}

void write_foo(Event * event)
{
    event->write_buffer.num = 100;
    std::cout<<event->write_buffer.num<<std::endl;
}

void process_foo(Event * event)
{
    int process = 0;
    process += event->read_buffer.num;
    process += 50;
    std::cout<<process<<std::endl;
}

int main()
{
    HandleMap map;
    map.bindHandle(read_foo,"READ");
    map.bindHandle(write_foo,"WRITE");
    map.bindHandle(process_foo,"PROCCESS");

    Event e(map,0);
    e.getHandle()();
    e.toNextState();

    e.getHandle()();
    e.toNextState();

    e.getHandle()();
    e.toNextState();
}