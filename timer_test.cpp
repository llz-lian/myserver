#include<chrono>
#include<signal.h>
#include<queue>
#include<iostream>
#include<functional>
//alarm
#include<thread>
#include<unistd.h>
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
    clock::time_point start_time;
    clock::time_point trigger_time;
    void doTask(){task();};
    Timer(std::function<void()> && task,clock::time_point start_time,ms & wait_time,long long task_times = -1)
        :task(task),start_time(start_time),trigger_time(wait_time + start_time),task_times(task_times){}
    Timer(std::function<void()> & task,clock::time_point start_time,ms & wait_time,long long task_times = -1)
        :task(task),start_time(start_time),trigger_time(wait_time + start_time),task_times(task_times){}    
    bool operator<(const Timer & t)
    {
        return trigger_time<t.trigger_time;
    }
    static auto genDurbyms(int time)
    {
        ms dur(time);
        return dur;
    }
};

class TimerList
{
private:
    // std::priority_queue<Timer> __timer_queue;
    ::sigset_t sig_alarm;
    bool is_run = true;
    std::vector<std::thread> worker;
    std::queue<std::function<void()>> task_queue;
    void __popFrontTimer()
    {
        
    }
    void __tick()
    {
        //only one thread do tick
        //set sigset and unblock sigalarm
        
        
    }
    
public:
    TimerList()
    {
        //tick thread
        worker.emplace_back(
            [this]()
            {
                this->__tick();
            }
        );
        //callback thread
        // worker.emplace_back(
        //     [this]()
        //     {

        //     }
        // );
    };
    ~TimerList(){};
    void addTimer(Timer && t)
    {

    }
};
void onetask()
{
    std::cout<<"trigger onetask\n";
}
int main()
{
    //block alram
    ::sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGALRM);
    int s = pthread_sigmask(SIG_BLOCK,&set,NULL);

    auto dur = Timer::genDurbyms(2000);

    
    TimerList list_t;

    while(1)
    {
    }
}