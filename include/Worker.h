#include"ThreadPool.h"




class Worker
{
public:


    void run();




private:
    std::ThreadPool __sub_workers;
    


};