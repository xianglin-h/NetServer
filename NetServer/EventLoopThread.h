// Copyright 2019, Chen Shuaihao.
//
//Author: Chen Shuaihao
//
//EventLoopThread类，表示IO线程,执行特定任务的,线程池的是通用任务线程

#ifndef _EVENTLOOP_THREAD_H_
#define _EVENTLOOP_THREAD_H_

#include <iostream>
#include <string>
#include <thread> 
//#include <queue>
//#include <mutex>  
//#include <condition_variable>
#include "EventLoop.h"

class EventLoopThread
{
private:
    std::thread th_;
    std::thread::id threadid_;
    std::string threadname_;
    EventLoop *loop_;

public:
    EventLoopThread();
    ~EventLoopThread();

    EventLoop* GetLoop();
    void Start();
    void ThreadFunc();//线程真正执行的函数
};

#endif


