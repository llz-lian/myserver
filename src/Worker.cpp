#include"include/Worker.hpp"


Worker::Worker(HandleMap & handlemap,int sub_workers,std::string & pool_belong)
    :__handleMap(handlemap),__sub_workers(sub_workers,pool_belong),__worker_epoll(WORKER_SLEEP_TIMES)
{
    active_fd_num = 0;
}
Worker::Worker(const Worker & w)
    :__handleMap(w.__handleMap),__sub_workers(w.__sub_workers),__worker_epoll(w.__worker_epoll)
{
    active_fd_num = 0;
}
Worker::Worker(Worker && w)
    :__handleMap(w.__handleMap),__sub_workers(w.__sub_workers),__worker_epoll(w.__worker_epoll)
{
    active_fd_num = 0;
}
Worker::~Worker(){}

void Worker::init()
{
    fd_events.reserve(MAX_EPOLL_LISTEN_EVENTS);
    std::function<void(Event *)> close_func = [this](Event * event)
    {
        this->closeFd(event);
    };
    __handleMap.bindHandle(close_func,"CLOSE");
    std::function<void(Event *)> complete_func = [this](Event * event)
    {
        this->completeFd(event);
    };
    __handleMap.bindHandle(complete_func,"COMPLETE");
}
void Worker::work()
{
    while (true)
    {
        if(active_fd_num == 0)
        {
            std::unique_lock<std::mutex> lock(__lock);
            __check_fd_num.wait(lock);
        }
        //fd num > 0
        //wait_ret = [int num_active,epoll_event* events]
        auto [num_active,ret_events] = __worker_epoll.wait();
        // std::cout<<"epool return\n";
        if(num_active<0)
        {
            if(errno==EINTR)
                continue;
            break;
        }
        for(int i = 0;i<num_active;i++)
        {
            int now_fd = ret_events[i].data.fd;
            if(now_fd<=0)
                continue;
            //events send to sub-worker
            //worker can do []
            Event * now_events = nullptr;
            {
                std::unique_lock<std::mutex> lck (__map_lock);
                if(fd_events.find(now_fd)!=fd_events.end())
                    now_events = fd_events.at(now_fd);
            }
            if(ret_events[i].events&EPOLLRDHUP)
            {
                #ifdef DEBUG
                std::cout<<"fd is closed:"<<now_fd<<std::endl;
                #endif
                //close read
                //COMPLETE => CLOSE
                if(now_events!=nullptr)
                {
                    now_events->should_remove = true;
                    if(now_events->state == EventStuff::COMPLETE)
                    {
                        now_events->state = EventStuff::NEED_CLOSE;
                        __sub_workers.submit(Worker::__eventHandle,now_events,this);
                    }
                }
            }
            else if(!now_events)
            {
                //create and init events
                //lock
                std::shared_lock<std::shared_mutex> lck (__map_write_lock);
                Event * new_event = new Event(__handleMap,now_fd,this);
                fd_events[now_fd] = new_event;
                // event_locks[now_fd] = new std::mutex();
                __sub_workers.submit(Worker::__eventHandle,new_event,this);
            }
            else if(now_events->state==EventStuff::COMPLETE && ret_events[i].events&EPOLLIN && ret_events[i].events&EPOLLOUT)
            {
                now_events->fd = now_fd;
                __sub_workers.submit(Worker::__eventHandle,now_events,this);
            }
            else if (now_events->state==EventStuff::WAIT_WRITE && ret_events[i].events&EPOLLOUT)
            {
                now_events->fd = now_fd;
                __sub_workers.submit(Worker::__eventHandle,now_events,this);
            }
        }
        // for(auto [fd,events]:fd_events)
        // {
        //     //poccess events timeout
        //     //events must be COMPLETE
        //     //set CLOSE
        // }
    }
}
void Worker::addFd(int fd)
{
    if(active_fd_num>=MAX_EPOLL_LISTEN_EVENTS)
    {
        std::cout<<"reach worker max fd!\n";
        return;
    }
    __worker_epoll.addFd(fd);
    if(active_fd_num == 0)
        __check_fd_num.notify_all();
    active_fd_num++;
}

void Worker::closeFd(Event * event)
{
    std::unique_lock<std::mutex> lock(__map_lock);
    if(event == nullptr)
        return;
    int fd = event->fd;
    if(fd_events.find(fd)==fd_events.end())
        return;
    //has lock
    __worker_epoll.removeFd(fd);
    // dont change map in sub thread!
    // delete event;
    event->should_remove = true;
    ::shutdown(fd,SHUT_RDWR);
    active_fd_num--;
    #ifdef DEBUG
    std::cout<<"call closeFd:"<<fd<<std::endl;
    #endif
}

void Worker::completeFd(Event * event)
{   
    int fd = event->fd;
    //do something
    //change to WAIT_READ
    event->resetFlags();
    Event::toNextState(event);
    // std::cout<<"call complete\n";
    //now state still complete 
    return;
}
void Worker::__eventHandle(Event * event,Worker * w)
{
    if(event == nullptr)
        return;
    int fd = event->fd;
    {
        // if(w->event_locks[fd]==nullptr)
        // {
        //     w->event_locks[fd] = new std::mutex();
        // }
        //run before check
        if(w->fd_events.find(fd)!=w->fd_events.end()&&w->fd_events.at(fd)==nullptr)
        {
            // delete w->event_locks[fd];
            // w->event_locks[fd] = nullptr;
            return;
        }
        
        event->getHandle()();
    }
}