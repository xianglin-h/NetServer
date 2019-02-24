// Copyright 2019, Chen Shuaihao.
//
//Author: Chen Shuaihao
//
//IO复用流程的抽象，等待事件，处理事件，执行其他任务

#ifndef _EVENTLOOP_H_
#define _EVENTLOOP_H_

#include <functional>
#include <vector>
#include <thread>
#include <mutex>
#include "Poller.h"
#include "Channel.h"
 
class EventLoop /*nocopyable*/
{
public:
    typedef std::function<void()> Functor;
    typedef std::vector<Channel*> ChannelList;  
    EventLoop(/* args */);
    ~EventLoop();

    void loop();
    void AddChannelToPoller(Channel *pchannel)
    {
        poller_.AddChannel(pchannel);
    }
    void RemoveChannelToPoller(Channel *pchannel)
    {
        poller_.RemoveChannel(pchannel);
    }
    void UpdateChannelToPoller(Channel *pchannel)
    {
        poller_.UpdateChannel(pchannel);
    }
    void Quit()
    {
        quit_ = true;
    }
    std::thread::id GetThreadId()
    {
        return tid_;
    }
    
    void WakeUp();
    void HandleRead();
    void HandleError();

    //Task
    void AddTask(Functor functor)
    {
        {
            std::lock_guard <std::mutex> lock(mutex_);
            functorlist_.push_back(functor);           
        }
        WakeUp();//跨线程唤醒，worker线程唤醒IO线程
    }
    void ExecuteTask()
    {
        // std::lock_guard <std::mutex> lock(mutex_);
        // for(Functor &functor : functorlist_)
        // {
        //     functor();//在加锁后执行任务，有可能导致死锁，调用sendinloop，再调用close，就死锁了
        // }
        // functorlist_.clear();

        std::vector<Functor> functorlist;
        {
            std::lock_guard <std::mutex> lock(mutex_);
            functorlist.swap(functorlist_);
        }      
        for(Functor &functor : functorlist)
        {
            functor();
        }
        //functorlist.clear();
    }

private:
    /* data */    
    std::vector<Functor> functorlist_;
    ChannelList channellist_;
    ChannelList activechannellist_;
    Poller poller_;
    bool quit_;
    std::thread::id tid_;
    std::mutex mutex_;
    int wakeupfd_;//跨线程唤醒fd
    Channel wakeupchannel_;
};


#endif