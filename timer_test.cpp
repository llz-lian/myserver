#include<chrono>
#include<signal.h>
#include<queue>
#include<iostream>
#include<functional>
//alarm
#include<thread>
#include<unistd.h>
#include<sys/epoll.h>
#include<type_traits>
/*
Timer
    //need a timer list
    Timer_list l;

    //need a timer
    Timer t;
    //t need a callback and timeout
    Timer t(callback,timeout)
    //callback should be like this -> void(data *){//change data};

    //put t to timer list
    Timer_list.add(t);
    Timer_list.add(std::move(t));
    Time_list.add(Timer(callback,timeout))    

    //timer list can calculate time and use heap, the mimimal left time timer should be front
    //timer list can processs callback function in timer
    //timer list should have threads
    //one thread can recive sigalrm, after recive sigalrm it can process timer and submit callback to other theards and wait sigalrm
    //timer should have function tick that recive sigalrm and submit callback
*/
struct Timer
{
    using ms = std::chrono::milliseconds;
    using clock = std::chrono::steady_clock;
    std::function<void()> task;
    long long task_times = -1;//<0 means do not stop
    bool stop = false;
    clock::time_point start_time;
    clock::time_point trigger_time;
    ms wait_time;
    void doTask(clock::time_point & now_time){
        task();
        task_times -= 1;
        if(task_times == 0)
        {    
            stop = true;
        }
        else
        {
            //reset start_time and trigger_time
            start_time = now_time;
            trigger_time = now_time + wait_time;
        }
    };
    Timer(std::function<void()> && task,clock::time_point start_time,ms & wait_time,long long task_times = -1)
        :task(task),start_time(start_time),trigger_time(wait_time + start_time),wait_time(wait_time),task_times(task_times){}
    Timer(std::function<void()> & task,clock::time_point start_time,ms & wait_time,long long task_times = -1)
        :task(task),start_time(start_time),trigger_time(wait_time + start_time),wait_time(wait_time),task_times(task_times){}


    Timer(const Timer && t):task(t.task),start_time(t.start_time),trigger_time(t.trigger_time),wait_time(t.wait_time),task_times(t.task_times),stop(t.stop){}
    Timer& operator=(const Timer&t)
    {
        task = t.task;
        start_time = t.start_time;
        trigger_time = t.trigger_time;
        wait_time = t.wait_time;
        task_times = t.task_times;
        stop = t.stop;
        return *this;
    }
    bool operator<(const Timer & t)
    {
        return trigger_time<t.trigger_time;
    }
    friend bool operator<(const Timer & now_timer,const Timer & cmp_timer)
    {
        return now_timer.trigger_time<cmp_timer.trigger_time;
    }

    static auto genDurbyms(int time)
    {
        ms dur(time);
        return dur;
    }
    template<class F,class ...Args>
    static auto bindTask(F && f,Args && ...args)
    {
        using return_type = decltype(f(args...));
        if(!std::is_same<return_type,void>::value)
        {
            std::cout<<"ignored return value!\n";
        }
        auto bind_task = std::function<void()>(std::bind(std::forward<F>(f),std::forward<Args>(args)...));
        return bind_task;
    }
    bool missionComplete()
    {
        return task_times == 0;
    }
};

class TimerList
{
private:
    std::priority_queue<Timer> __timer_queue;
    bool is_run = true;
    std::vector<std::thread> worker;
    std::queue<std::function<void()>> task_queue;
    int lateset_trigger_ms = -1;
public:
    TimerList()
    {

    };
    ~TimerList(){};
    template <class T>
    void addTimer(T && t)
    {
        __timer_queue.push(std::forward<T>(t));
    }
    bool empty()
    {
        return __timer_queue.empty();
    }
    void tick()
    {
        //only one thread do tick
        //set sigset and unblock sigalarm
        if(__timer_queue.empty())
            return;
        auto now_time = std::chrono::steady_clock::now();
        while(!__timer_queue.empty())
        {
            //const T & top() const
            Timer timer(std::move(__timer_queue.top()));__timer_queue.pop();
            if(timer.trigger_time>now_time)
            {
                __timer_queue.emplace(std::move(timer));
                break;
            }
            //need trigger
            timer.doTask(now_time);
            if(!timer.stop)
            {
                __timer_queue.emplace(std::move(timer));
            }
        }
        if(!__timer_queue.empty())
        {
            now_time = std::chrono::steady_clock::now();
            auto dur = __timer_queue.top().trigger_time - now_time;
            lateset_trigger_ms = std::chrono::duration_cast<std::chrono::milliseconds>(dur).count();
        }
        else
            lateset_trigger_ms = -1;
    }
};

int rettask()
{
    std::cout<<"ret task\n";
    return 1;
}

void onetask()
{
    std::cout<<"trigger onetask\n";
}
int main()
{
    auto dur = Timer::genDurbyms(2000);
    TimerList list_t;

    list_t.addTimer(Timer(Timer::bindTask(onetask),std::chrono::steady_clock::now(),dur,-1));
    sleep(1);
    list_t.addTimer(Timer(std::function<void()>(onetask),std::chrono::steady_clock::now(),dur,-1));
    int epfd = ::epoll_create1(1);

    while(true)
    {
        int ret = epoll_wait(epfd,nullptr,1,1000);
        list_t.tick();
    }
}