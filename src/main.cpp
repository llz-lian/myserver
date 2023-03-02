//bind test
#include<include/ThreadPool.h>
#include<iostream>
class A
{
public:
    A(int num):num(num){
        std::cout<<"build build\n";
    }
    A(A&&a)
    {
        std::cout<<"move construct\n";
        num = a.num;
    }
    A(A&a)
    {
        std::cout<<"copy copy\n";
        num = a.num;
    }
    int num = 0;
};
int aFunc(int a,int b,int c)
{
    std::cout<<"aFunc"<<std::endl;
    return a+b+c;
}
int bFunc(int a,int b,int c)
{
    std::cout<<"bFunc"<<std::endl;
    return a*b*c;
}
int sumab(A & a,A& b)
{

    return a.num + b.num;
}
int sumslow(A&a,A&b)
{
    int num = 0;
    for(int i=0;i<1000;i++)
    {
        num = a.num/b.num + a.num * b.num;
        num /=2;
    }
    return num;
}

int main()
{
    // auto bind_a = std::bind(aFunc,1,2,3);
    // auto bind_b = std::bind(aFunc,2,3,4);
    

    // auto afunc = [&](){
    //     std::cout<<"lambda a"<<std::endl;
    //     bind_a();};

    // std::function<void()> func = afunc;
    // std::cout<<"function<void()>"<<std::endl;
    // func();
    std::ThreadPool pool(64);
    A a = A(5);
    A b = A(10);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
        pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
        pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
        pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
        pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
        pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
        pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
        pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumab,a,b);
    pool.submit(sumslow,a,b);
    pool.submit(sumab,a,b);
    auto ret = pool.submit(sumab,a,b);
    int res = ret.get();
    std::cout<<res;
    
}