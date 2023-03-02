//bind test
#include<iostream>
#include<functional>
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
int main()
{
    auto bind_a = std::bind(aFunc,1,2,3);
    auto bind_b = std::bind(aFunc,2,3,4);
    

    auto afunc = [&](){
        std::cout<<"lambda a"<<std::endl;
        bind_a();};

    std::function<void()> func = afunc;
    std::cout<<"function<void()>"<<std::endl;
    func();

}