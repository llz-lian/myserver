
#include"include/Event.hpp"

const std::unordered_map<int,std::string> EventStuff::state_to_string{
    {EventStuff::WAIT_READ,"READ"},
    {EventStuff::WAIT_PROCCESS,"PROCCESS"},
    {EventStuff::WAIT_WRITE,"WRITE"},
    {EventStuff::COMPLETE,"COMPLETE"},
    {EventStuff::NEED_CLOSE,"CLOSE"},
    {EventStuff::CLOSED,"CLOSED"}
};


HandleMap::HandleMap(){
    //init handle[str,func]
    std::function<void(Event *)> foo = [](Event * e){};
    handle["READ"] = foo;
    handle["WRITE"] = foo;
    handle["PROCCESS"] = foo;
    handle["CLOSE"] = foo;
    handle["COMPLETE"] = foo;
    handle["CLOSED"] = foo;
};

HandleMap::HandleMap(const HandleMap & map)
{
    //init handle[str,func]
    for(auto && [str,func]:map.handle)
    {
        handle[str] = func;
    }
};
HandleMap::HandleMap(const HandleMap && map)
{
    //init handle[str,func]
    for(auto && [str,func]:map.handle)
    {
        handle[str] = func;
    }
};
void HandleMap::bindHandle(std::function<void(Event * )> handle,std::string && method)
{
    #ifdef DEBUG
    std::cout<<"bind handle:"<<method<<std::endl;
    #endif
    this->handle[method] = [handle](Event * event){
        handle(event);
    };
}



Event::Event(const HandleMap & map,int recive_fd,Worker * w)
    :fd(recive_fd),handle(map),myMaster(w)
{__init();};
Event::Event(const Event & event)
    :fd(event.fd),handle(event.handle),myMaster(event.myMaster)
{__init();};
Event::Event(const Event && event)
    :fd(event.fd),handle(event.handle),myMaster(event.myMaster)
{__init();};
Event::Event(const Event * event)
    :fd(event->fd),handle(event->handle),state(event->state),myMaster(event->myMaster)
{
    __init();
}


std::function<void()> Event::getHandle()
{
    std::function<void(Event *) > now_choose_handle = handle.getHandle(EventStuff::getEventStuff().state_to_string.at(state));
    #ifdef DEBUG
    std::cout<<"get handle:"<<state_to_string[state]<<std::endl;
    #endif
    if(state ==EventStuff::NEED_CLOSE||state==EventStuff::COMPLETE)
    {
        return std::function<void()>([this,now_choose_handle](){now_choose_handle(this);});
    }
    return std::function<void()>([this,now_choose_handle](){
            now_choose_handle(this);
            Event::toNextState(this);
            });
}
void Event::toNextState(Event * evnet)
{
    if(evnet->state == EventStuff::NEED_CLOSE)
    {
        evnet->getHandle()();
        return;
    }
    int to_next_state = 0;
    if((evnet->read_complete_flag&&evnet->state == EventStuff::WAIT_READ) || 
       (evnet->write_complete_flag&&evnet->state==EventStuff::WAIT_WRITE)||
       (evnet->process_complete_flag&& evnet->state == EventStuff::WAIT_PROCCESS)||
       (evnet->state == EventStuff::COMPLETE))
    {
        to_next_state = 1;
    }
    evnet->state = (evnet->state + to_next_state)%(EventStuff::NEED_CLOSE);
    //now complete task
    if(evnet->state == EventStuff::COMPLETE)
    {
        //add fd
        //has lock
        // evnet->myMaster->addFd(evnet->fd);
        //worker add fd
        evnet->is_running = false;
        uint64_t u = evnet->fd;
        int write_ret = write(evnet->myMaster->notify_fd,&u,sizeof(uint64_t));
        evnet->myMaster->wait_add_queue.push(evnet->fd);
        return;
    }
    evnet->getHandle()();
}
void Event::resetFlags()
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