#include"include/Worker.hpp"
#include<stdexcept>

Worker::Worker(int sub_workers,std::string & pool_belong)
    :__sub_workers(sub_workers,pool_belong),__worker_epoll(WORKER_SLEEP_TIMES)
{
    active_fd_num = 0;
}
Worker::Worker(const Worker & w)
    :__sub_workers(w.__sub_workers),__worker_epoll(w.__worker_epoll)
{
    active_fd_num = 0;
}
Worker::Worker(Worker && w)
    :__sub_workers(w.__sub_workers),__worker_epoll(w.__worker_epoll)
{
    active_fd_num = 0;
}
Worker::~Worker(){
    for(auto [fd,event]:fd_events)
    {
        delete event;
        event = nullptr;
    }
}

void Worker::init()
{
    notify_fd = eventfd(0,0);
    if(!__worker_epoll.addFd(notify_fd))
    {
        throw std::runtime_error("Worker init eventfd failed!");
    }
    if(!__worker_epoll.modFd(notify_fd,EPOLLIN|EPOLLET))
    {
        throw std::runtime_error("Worker init eventfd failed!");
    }
    fd_events.reserve(MAX_EPOLL_LISTEN_EVENTS);
    std::function<void(Event *)> close_func = [this](Event * event)
    {
        this->closeFd(event);
    };
    HandleMap::bindHandle(close_func,"CLOSE");
    std::function<void(Event *)> complete_func = [this](Event * event)
    {
        this->completeFd(event);
    };
    HandleMap::bindHandle(complete_func,"COMPLETE");
}
void Worker::work()
{
    while (true)
    {
        // if(active_fd_num == 0)
        // {
        //     std::unique_lock<std::mutex> lock(__lock);
        //     __check_fd_num.wait(lock);
        // }
        //fd num > 0
        //wait_ret = [int num_active,epoll_event* events]
        __worker_epoll.setSleepTime(timer.lateset_trigger_ms);
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
            //can add eventfd

            if(now_fd == notify_fd)
            {
                //handle notify
                uint64_t u;
                int read_ret = read(notify_fd,&u,sizeof(uint64_t));
                handleWaitQueue();
                handleClosefd();
                continue;
            }
            //events send to sub-worker
            //worker can do []
            Event * now_events = nullptr;
            {
                std::shared_lock<std::shared_mutex> lck (__map_write_lock);
                if(fd_events.find(now_fd)!=fd_events.end())
                    now_events = fd_events.at(now_fd);
                else
                    continue;
            }
            // std::cout<<"EPOLL EVENTS:"<<ret_events[i].events<<std::endl;
            if(ret_events[i].events&EPOLLRDHUP)
            {
                #ifdef DEBUG
                std::cout<<"fd is closed:"<<now_fd<<std::endl;
                #endif
                //close read
                //COMPLETE => CLOSE
                if(now_events!=nullptr)
                {
                    if(!now_events->is_running)
                    {
                        now_events->state = EventStuff::NEED_CLOSE;
                        __sub_workers.submit(Worker::__eventHandle,now_events,this);
                    }
                }
            }
            else if(now_events->state==EventStuff::WAIT_READ && (ret_events[i].events&EPOLLIN || ret_events[i].events&EPOLLPRI))
            {
                __sub_workers.submit(Worker::__eventHandle,now_events,this);
            }
            else if(now_events->state==EventStuff::COMPLETE && (ret_events[i].events&EPOLLIN || ret_events[i].events&EPOLLPRI))
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
        //handle time out
        timer.tick();
        handelTimeOut();
    }
}

void Worker::handelTimeOut()
{
    std::unique_lock<std::shared_mutex> lock(__map_write_lock);
    int n = time_out_queue.size();
    while(n-->0)
    {
        //time out event
        int fd = time_out_queue.pop();
        if(fd_events.find(fd) ==fd_events.end())
            continue;
        auto event = fd_events[fd];
        //event is complete
        if(event->is_running == false && (event->state == EventStuff::COMPLETE||event->state == EventStuff::WAIT_READ||event->state == EventStuff::CLOSED))
        {
            event->state = EventStuff::CLOSED;
            wait_close_queue.push(fd);
            uint64_t u = 1;
            int write_ret = write(event->myMaster->notify_fd,&u,sizeof(uint64_t));
        }
        else
        {
            time_out_queue.push(fd);
        }
    }
}

void Worker::handleClosefd()
{
    std::unique_lock<std::shared_mutex> lck (__map_write_lock);
    int n = wait_close_queue.size();
    while(n-->0)
    {
        int fd = wait_close_queue.pop();
        if(fd_events.find(fd) ==fd_events.end())
            continue;
        auto event = fd_events[fd];
        delete event;
        fd_events.erase(fd);
        active_fd_num--;
        __worker_epoll.removeFd(fd);
        ::shutdown(fd,SHUT_RDWR);
        ::linger l;
        l.l_linger = 1;
        l.l_onoff = 1;
        ::setsockopt(fd,SOL_SOCKET,SO_LINGER,&l,sizeof(::linger));
        ::close(fd);
        // std::cout<<"closed fd:"<<fd<<std::endl;
    }
}

void Worker::handleWaitQueue()
{
    std::unique_lock<std::shared_mutex> lock(__map_write_lock);
    int n = wait_add_queue.size();//noly pick now visible fd
    while(n-->0)
    {
        int fd = wait_add_queue.pop();
        if(fd_events.find(fd)!=fd_events.end())
            __worker_epoll.modFd(fd,EPOLLIN|EPOLLET|EPOLLONESHOT|EPOLLRDHUP|EPOLLPRI);
        else
        {   
            if(!addFd(int(fd)))
                continue;
            // Event * new_event = nullptr;
            // new_event = new Event(__handleMap,fd,this);
            fd_events[fd] = new Event(fd,this);
            auto time_out_task = Timer::bindTask([this](int fd){
                std::unique_lock<std::shared_mutex> lck (this->__map_write_lock);
                if(this->fd_events.find(fd)==this->fd_events.end())
                    return;
                fd_events.at(fd)->is_running = false;
                this->time_out_queue.push(fd);
            },fd);
            timer.addTimer(Timer(time_out_task,std::chrono::steady_clock::now(),Timer::genDurbyms(FD_TIMEOUT),1));
        }
    }
}
bool Worker::addFd(int fd)
{
    if(active_fd_num>=MAX_EPOLL_LISTEN_EVENTS-1)
    {
        std::cout<<"reach worker max fd! event_size:"<<fd_events.size()<<"\n";
        close(fd);
        return false;
    }

    if(__worker_epoll.addFd(fd))
    {
        // std::cout<<"fd added:"<<fd<<std::endl;
        // if(active_fd_num == 0)
        //     __check_fd_num.notify_all();
        active_fd_num++;
        return true;
    }
    return false;
}

void Worker::closeFd(Event * event)
{
    // std::shared_lock<std::shared_mutex> lock(__map_write_lock);
    if(event == nullptr)
        return;
    int fd = event->fd;
    //has lock
    uint64_t u = fd;
    //if put these below write may cause invalid memory access
    event->fd = 0;
    // event->is_running = false;
    event->state = EventStuff::CLOSED;
    //if ret<0 we also set closed and push
    event->myMaster->wait_close_queue.push(fd);
    
    int write_ret = write(event->myMaster->notify_fd,&u,sizeof(uint64_t));
    // std::cout<<"fd ready to close:"<<fd<<std::endl;
}

void Worker::completeFd(Event * event)
{   
    int fd = event->fd;
    //do something
    //change to WAIT_READ
    event->resetFlags();
    Event::toNextState(event);
    #ifdef DEBUG
    std::cout<<"call complete\n";
    #endif
    //now state still complete 
    return;
}
void Worker::__eventHandle(Event * event,Worker * w)
{
    if(event == nullptr)
        return;
    int fd = event->fd;
    {
        std::shared_lock<std::shared_mutex> lock(w->__map_write_lock);
        if(w->fd_events.find(fd)!=w->fd_events.end()&&w->fd_events.at(fd)==nullptr)
        {
            return;
        }
    }
    event->is_running.store(true);
    event->getHandle()();
    event->is_running.store(false);
}