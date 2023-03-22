# A toy webserver
## Over All Structure
![](https://github.com/llz-lian/myserver/blob/master/imgs/server.jpg)
## Speed
![](https://github.com/llz-lian/myserver/blob/master/imgs/test.PNG)
## Compile from source code
run
```
mkdir build
cd build
cmake ..
cd ..
cmake --build build
cd bin
```
## Use Example
1. set config.hpp
2. write read, write and process functions
```
//like this
void func(Event *);
```
3. regist read write process functions 
```
int main()
{
    Server server;
    //prepare handles
    std::vector<std::function<void (Event *)>> handles = {MyRead,MyProcess,MyWrite};
    // also can
    // std::function<void (Event *)> read_fuc = MyRead;
    // std::function<void (Event *)> process_fuc = MyProcess;
    // std::function<void (Event *)> write_fuc = MyWrite;
    // handles[0] = read_fuc;
    // handles[1] = process_fuc;
    // handles[2] = write_fuc;
    //regist handles
    server.init(handles);
    //run server
    server.run();
}
```
4. compile source code
5. run server
## Project Features
* scoket_test has poll and select example
* use epoll edge trigger
* c++ 11 threadpool
* Round-Robin accept
* A Timer
## Thanks
thanks to &laquo;Linux高性能服务器编程&raquo;-游双
