#pragma once
#include<string>
#include<functional>
#include<unordered_map>
#include<string.h>
#include"Handle.hpp"
#include"ThreadPool.hpp"
#include"config.hpp"
class HandleMap;
class Event;
void foo(){}
class HandleMap
{
public:
    HandleMap(){
        //init handle[str,func]
        std::function<void(Event *)> foo = [](Event * e){};
        handle["READ"] = foo;
        handle["WRITE"] = foo;
        handle["PROCCESS"] = foo;
        handle["CLOSE"] = foo;
        handle["COMPLETE"] = foo;
    };
    HandleMap(const HandleMap & map)
    {
        //init handle[str,func]
        for(auto && [str,func]:map.handle)
        {
            handle[str] = func;
        }
    };
    HandleMap(const HandleMap && map)
    {
        //init handle[str,func]
        for(auto && [str,func]:map.handle)
        {
            handle[str] = func;
        }
    };
    void bindHandle(std::function<void(Event * )> handle,std::string && method)
    {
        #ifdef DEBUG
        std::cout<<"bind handle:"<<method<<std::endl;
        #endif
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
    Handle(const Handle & handle):handle_map(handle.handle_map){};
    Handle(const Handle && handle):handle_map(handle.handle_map){};

    std::function<void(Event *)> getHandle(std::string & s)
    {
        return handle_map.handle[s];
    }
    HandleMap & handle_map;
};
class Event
{
private:
    void __initStateToString()
    {
        //create by user
        state_to_string[int(WAIT_READ)] = "READ";
        state_to_string[int(WAIT_PROCCESS)] = "PROCCESS";
        state_to_string[int(WAIT_WRITE)] = "WRITE";
        //create by server
        state_to_string[int(NEED_CLOSE)] = "CLOSE";
        state_to_string[int(COMPLETE)] = "COMPLETE";
    }
    void __init()
    {
        __initStateToString();
        read_buffer.resize(READ_BUFFER_DEFALUT_SIZE);read_buffer_size = READ_BUFFER_DEFALUT_SIZE;
    }
public:
    Event(HandleMap & map,int recive_fd,std::ThreadPool * worker_pool)
        :fd(recive_fd),handle(map),pool(worker_pool)
    {__init();};
    Event(Event & event)
        :fd(event.fd),handle(event.handle),pool(event.pool)
    {__init();};
    Event(Event && event)
        :fd(event.fd),handle(event.handle),pool(event.pool)
    {__init();};
    ~Event(){
        fd = -1;
    };
    //handle
    Handle handle;
    std::function<void()> getHandle()
    {
        std::function<void(Event *) > now_choose_handle = handle.getHandle(state_to_string[state]);
        #ifdef DEBUG
        std::cout<<"get handle:"<<state_to_string[state]<<std::endl;
        #endif
        if(state == NEED_CLOSE||state==COMPLETE)
        {
            return std::function<void()>([this,now_choose_handle](){now_choose_handle(this);});
        }
        return std::function<void()>([this,now_choose_handle](){
                now_choose_handle(this);
                Event::toNextState(this);
                });
    }
    //static makesure can return
    static void toNextState(Event * evnet)
    {
        if(evnet->state == NEED_CLOSE)
        {
            evnet->getHandle()();
            return;
        }
        int to_next_state = 0;
        if((evnet->read_complete_flag&&evnet->state == WAIT_READ) || 
           (evnet->write_complete_flag&&evnet->state==WAIT_WRITE)||
           (evnet->process_complete_flag&& evnet->state == WAIT_PROCCESS)||
           (evnet->state == COMPLETE))
        {
            to_next_state = 1;
        }
        evnet->state = (evnet->state + to_next_state)%(NEED_CLOSE);
        //now complete task
        if(evnet->state == COMPLETE)
        {    
            return;
        }
        evnet->getHandle()();
    }
    void resetFlags()
    {
        // std::unique_lock<std::mutex> task_lock(lock);
        read_complete_flag = false;
        write_complete_flag = false;
        process_complete_flag = false;
        write_bytes = 0;
        read_bytes = 0;
        bzero(&read_buffer[0],sizeof(char)*read_buffer_size);
        write_buffer="";
    }

    int fd = 0;
    //state
    enum STATE {WAIT_READ,WAIT_PROCCESS,WAIT_WRITE,COMPLETE,NEED_CLOSE};
    int state = WAIT_READ;
    std::unordered_map<int,std::string> state_to_string;

    //read write stuff
    std::string read_buffer;
    std::string write_buffer;
    size_t read_buffer_size;


    bool read_complete_flag = false;
    bool write_complete_flag = false;
    bool process_complete_flag = false;
    bool fd_closed = false;
    //bytes need to write
    size_t write_bytes = 0;
    size_t read_bytes = 0;
    //workers
    std::ThreadPool * pool;
    
};



