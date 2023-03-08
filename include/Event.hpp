#pragma once
#include<string>
#include<functional>
#include<unordered_map>
#include<string.h>
#include"ThreadPool.hpp"
#include"config.hpp"
#include"Worker.hpp"
#include"Handle.hpp"
class Worker;
class EventStuff
{
private:
    /* data */
    EventStuff(/* args */){};
public:
    ~EventStuff(){};
    static const EventStuff getEventStuff()
    {
        static const EventStuff eventstuff =  EventStuff();
        return eventstuff;
    }
    static const std::unordered_map<int,std::string> state_to_string;
    enum STATE {WAIT_READ,WAIT_PROCCESS,WAIT_WRITE,COMPLETE,NEED_CLOSE};
};

class Event
{
private:
    inline void __init()
    {
        // __initStateToString();
        read_buffer.resize(READ_BUFFER_DEFALUT_SIZE);read_buffer_size = READ_BUFFER_DEFALUT_SIZE;
    }
public:
    Event(const HandleMap & map,int recive_fd,Worker * w);
    Event(const Event & event);
    Event(const Event && event);
    Event(const Event * event);
    ~Event(){
        fd = -1;
    };
    std::function<void()> getHandle();
    //static makesure can return
    static void toNextState(Event * evnet);
    void resetFlags();

    //fd
    int fd = 0;
    //state
    int state = EventStuff::WAIT_READ;
    //handle
    const Handle handle;
    //read write stuff
    std::string read_buffer;
    std::string write_buffer;
    size_t read_buffer_size;


    bool read_complete_flag = false;
    bool write_complete_flag = false;
    bool process_complete_flag = false;
    
    bool should_remove = false;

    //bytes need to write
    size_t write_bytes = 0;
    size_t read_bytes = 0;
    //workers
    Worker * myMaster;    
};



