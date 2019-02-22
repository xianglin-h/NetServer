// Copyright 2019, Chen Shuaihao.
//
//Author: Chen Shuaihao
//
//

#include <iostream>
#include <functional>
#include "HttpServer.h"


HttpServer::HttpServer(EventLoop *loop, int port, int threadnum)
    : tcpserver_(loop, port, threadnum),
    cnt(0)
{
    tcpserver_.SetNewConnCallback(std::bind(&HttpServer::HandleNewConnection, this, std::placeholders::_1));
    tcpserver_.SetMessageCallback(std::bind(&HttpServer::HandleMessage, this, std::placeholders::_1, std::placeholders::_2));
    tcpserver_.SetSendCompleteCallback(std::bind(&HttpServer::HandleSendComplete, this, std::placeholders::_1));
    tcpserver_.SetCloseCallback(std::bind(&HttpServer::HandleClose, this, std::placeholders::_1));
    tcpserver_.SetErrorCallback(std::bind(&HttpServer::HandleError, this, std::placeholders::_1));
}

HttpServer::~HttpServer()
{

}

void HttpServer::HandleNewConnection(TcpConnection *ptcpconn)
{
    //std::string msg(s);
    HttpSession *phttpsession = new HttpSession();
    //可以优化成无锁，放入conn里面就行
    {
        std::lock_guard <std::mutex> lock(mutex_);
        httpsessionnlist_[ptcpconn] = phttpsession;//LOCK
    }
}

void HttpServer::HandleMessage(TcpConnection *ptcpconn, std::string &s)
{
    //std::cout << "http num is:" << ++cnt << std::endl;  
    HttpSession *phttpsession = NULL;
    {
        std::lock_guard <std::mutex> lock(mutex_);
        phttpsession =  httpsessionnlist_[ptcpconn];
    }    
    //可以改造成线程池处理业务，处理完后投递回本IO线程执行send
    phttpsession->PraseHttpRequest(s);
    phttpsession->HttpProcess();
    std::string msg;
    phttpsession->AddToBuf(msg);
    ptcpconn->Send(msg);
    if(!phttpsession->KeepAlive())
    {
        //短连接，可以告诉框架层数据发完就可以关掉TCP连接，不过这里注释掉，还是交给客户端主动关闭吧
        //ptcpconn->HandleClose();
    }
}

void HttpServer::HandleSendComplete(TcpConnection *ptcpconn)
{

}

void HttpServer::HandleClose(TcpConnection *ptcpconn)
{
    HttpSession *phttpsession = NULL;
    {
        std::lock_guard <std::mutex> lock(mutex_);
        phttpsession = httpsessionnlist_[ptcpconn];
        httpsessionnlist_.erase(ptcpconn);//LOCK
    }
    delete phttpsession;
}

void HttpServer::HandleError(TcpConnection *ptcpconn)
{
    HttpSession *phttpsession = NULL;
    {
        std::lock_guard <std::mutex> lock(mutex_);
        phttpsession = httpsessionnlist_[ptcpconn];
        httpsessionnlist_.erase(ptcpconn);//LOCK
    }
    delete phttpsession;
}

void HttpServer::Start()
{
    tcpserver_.Start();
}
