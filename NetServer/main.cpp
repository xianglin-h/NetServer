// Copyright 2019, Chen Shuaihao.
//
//Author: Chen Shuaihao
//
//main

#include <signal.h>
#include "EventLoop.h"
//#include "TcpServer.h"
#include "HttpServer.h"
#include "EchoServer.h"

EventLoop *lp;
//gprof
static void sighandler1( int sig_no )   
{   
      exit(0);   
}   
static void sighandler2( int sig_no )   
{   
    lp->Quit();
}   

int main()
{
    signal(SIGUSR1, sighandler1);
    signal(SIGUSR2, sighandler2);
    //signal(SIGINT, sighandler2);
    signal(SIGPIPE, SIG_IGN);  //SIG_IGN,系统函数，忽略信号的处理程序,客户端发送RST包后，服务器还调用write会触发

    // EventLoop loop;
    // EchoServer echoserver(&loop, 8088);
    // echoserver.Start();
    // loop.loop();

    EventLoop loop;
    lp = &loop;
    HttpServer httpserver(&loop, 8088, 6);
    httpserver.Start();
    loop.loop();
    return 0;
}
