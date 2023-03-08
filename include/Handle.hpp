#pragma once
#include<unordered_map>
#include<functional>

class Event;
class HandleMap
{
public:
    HandleMap();
    HandleMap(const HandleMap & map);
    HandleMap(const HandleMap && map);
    void bindHandle(std::function<void(Event * )> handle,std::string && method);
    std::unordered_map<std::string,std::function<void(Event *)>> handle;
};
class Handle
{
public:
    Handle(const HandleMap & map):handle_map(map){};
    Handle(const Handle & handle):handle_map(handle.handle_map){};
    Handle(const Handle && handle):handle_map(handle.handle_map){};

    std::function<void(Event *)> getHandle(const std::string & s) const
    {
        return handle_map.handle.at(s);
    }
    const HandleMap & handle_map;
};