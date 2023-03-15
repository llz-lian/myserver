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
#include<chrono>
#include<priority_queue>
#include"Event.h"
class TimerCnt
{
public:
    std::chrono::milliseconds time_left;
    TimerCnt();
    ~TimerCnt();
}



