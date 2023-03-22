/*
what is http class?
http class can 
- analysis http request head
-- save http request body
-- save http request method
-- save request file
-- save head line

- prepare http response head

*/
#include<string>
#include<unordered_map>
#include <sys/types.h>
#include <sys/stat.h> 
#include <unistd.h>
class RequestData
{
private:
    std::string body_buffer;
public:
    RequestData(/* args */){};
    ~RequestData(){};
};
class RequestHead
{
private:
    /* data */
    bool __checkURLExists()
    {
        if(url == "/")
        {
            url = "html/index.html";
            return true;
        }
        if(url == "/favicon.ico")
        {
            url = "html/ico/favicon.ico";
            return true;
        }
        std::string path = "html" + url;
        struct stat buffer;   
        return (stat (path.c_str(), &buffer) == 0); 
    }
    bool __parseMethod(std::string & head,int & index)//meet space
    {
        while(head[index]!=' ')
            method += head[index++];
        index++;
        return method == "GET";
    }
    bool __parseURL(std::string & head,int & index)//meet space
    {
        while(head[index]!=' ')
            url += head[index++];
        index++;
        return __checkURLExists();
    }
    bool __parseHttpVersion(std::string & head,int & index)// need reach \r \n
    {
        while(head[index]!='\r')
            http_version += head[index++];
        //head[index] == '\r'
        index++;
        //head[index] == '\n'
        return head[index++]=='\n';
    }
    bool __parseHeadkv(std::string & head,int & index)
    {
        if(head[index] == '\r'&&head[index + 1]=='\n')
        {   
            index += 1000;
            return true;
        }
        std::string k;
        std::string v;
        while(head[index]!=':')
        {
            k += head[index++];
        }
        index++;
        while(head[index]!='\r')
        {
            v += head[index++];
        }
        index++;
        head_stuff[std::move(k)] = std::move(v);
        return head[index++] == '\n';
    }
public:
    enum{METHOD,URL,HTTP_V,NEXT_LINE,HEAD_OK,ERROR};
    std::string method;
    std::string url;
    std::string http_version;
    std::unordered_map<std::string,std::string> head_stuff;
    RequestHead(/* args */){};
    ~RequestHead(){};
    bool headParser(std::string & head)
    {
        int line_state = METHOD;
        //request line
        int index = 0;
        while(index<head.size())
        {
            switch (line_state)
            {
            case METHOD:
                line_state = __parseMethod(head,index)?URL:ERROR;
                break;
            case URL:
                line_state = __parseURL(head,index)?HTTP_V:ERROR;
                break;
            case HTTP_V:
                line_state = __parseHttpVersion(head,index)?HEAD_OK:ERROR;
                break;
            }
            if(line_state == HEAD_OK) break;
            if(line_state == ERROR) return false;
        }
        //request head line
        while(index<head.size())
        {
            if(!__parseHeadkv(head,index))
            {
                return false;
            }
        }
        return true;
    }
};

class ResponseHead
{
public:
    /* data */
    std::string head_buffer;
    ResponseHead(/* args */){};
    ~ResponseHead(){};
    void initBad()
    {
        head_buffer = "HTTP/1.0 404 Not Found\r\nServer: myserver\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
    }
    void initGood()
    {
        head_buffer = "HTTP/1.0 200 OK\r\nServer: myserver\r\nContent-Type: text/html;charset=utf-8\r\n\r\n";
    }
};
class ResponseBody
{
public:
    /* data */
    std::string file_path;
    ResponseBody(/* args */){};
    ~ResponseBody(){};
    void initBad()
    {
        file_path = "html/error.html";
    }
    void initGood(std::string & s)
    {
        file_path = std::move(s);
    }
};
class HTTP
{
private:
    /* data */
    RequestData __request_body;
    RequestHead __request_head;
public:
    ResponseHead response_head;
    ResponseBody response_body;
    HTTP(/* args */){};
    ~HTTP(){};
    void handleHead(std::string & head)
    {
        if(!__request_head.headParser(head))
        {
            //prepare bad response
            response_head.initBad();
            response_body.initBad();
            return;
        }
        response_head.initGood();
        response_body.initGood(__request_head.url);
    }

};

