#pragma once
#include<unordered_map>
#include<functional>

class Event;
class HandleMap
{
private:
    HandleMap();
    HandleMap(const HandleMap & map)=delete;
    HandleMap(const HandleMap && map)=delete;
public:
    static HandleMap & getHandleMap()
    {
        static HandleMap handle_map;
        return handle_map;
    }
    static void bindHandle(std::function<void(Event * )> handle,std::string && method);
    static std::unordered_map<std::string,std::function<void(Event *)>> handle;
};
class Handle
{
public:
    Handle(){};
    Handle(const Handle & handle){};
    Handle(const Handle && handle){};

    static std::function<void(Event *)> getHandle(const std::string & s)
    {
        return HandleMap::getHandleMap().handle.at(s);
    }
};