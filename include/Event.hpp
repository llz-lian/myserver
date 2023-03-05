#pragma once
#include<string>
#include<functional>
#include<unordered_map>
#include"Handle.hpp"
#include"ThreadPool.hpp"
class HandleMap;
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
    Handle(Handle & handle):handle_map(handle.handle_map){};
    Handle(Handle && handle):handle_map(handle.handle_map){};

    std::function<void(Event *)> getHandle(std::string & s)
    {
        // std::cout<<"get handle:"<<s<<std::endl;
        return handle_map.handle[s];
    }
    HandleMap & handle_map;
};
class Event
{
public:
    Event(HandleMap & map,int recive_fd)
        :fd(recive_fd),handle(map)
    {
        //create by user
        state_to_string[int(WAIT_READ)] = "READ";
        state_to_string[int(WAIT_PROCCESS)] = "PROCCESS";
        state_to_string[int(WAIT_WRITE)] = "WRITE";
        //create by server
        state_to_string[int(NEED_CLOSE)] = "CLOSE";
        state_to_string[int(COMPLETE)] = "COMPLETE";
    };
    //handle
    Handle handle;
    std::function<void()> getHandle()
    {
        std::function<void(Event *) > now_choose_handle = handle.getHandle(state_to_string[state]);
        return std::function<void()>([this,now_choose_handle](){
            now_choose_handle(this);
            this->toNextState();
            //remember add next mission to threadpool
        });
    }
    //state
    void toNextState()
    {
        int to_next_state = 0;
        if((read_complete_flag&&state == WAIT_READ) || 
           (write_bytes == 0&&state==WAIT_WRITE)||
           (process_complete_flag&& state == WAIT_PROCCESS))
        {
            to_next_state = 1;
        }
        state = (state + 1)%(NEED_CLOSE);
        if(state == COMPLETE)
        {    
            return;
        }
        pool->submit(getHandle());
    }
    void setClose()
    {
        state = NEED_CLOSE;
    }
    int fd = 0;
    //state
    enum STATE {WAIT_READ,WAIT_PROCCESS,WAIT_WRITE,COMPLETE,NEED_CLOSE};
    int state = WAIT_READ;
    std::unordered_map<int,std::string> state_to_string;

    //read write stuff
    std::string read_buffer;
    std::string write_buffer;
    bool read_complete_flag = false;
    bool process_complete_flag = false;
    //bytes need to write
    size_t write_bytes = 256;
    //workers
    std::ThreadPool * pool;
    
};