#ifndef HANDLETHREAD_H
#define HANDLETHREAD_H

#include "core/thread/WorkThread.h"
#include "core/request/RequestQueue.h"
#include "core/center/type.h"
#include "base/logger/logger.h"


class ThreadPool;
class Config;
class Request;
class UpdateRequest;
class SWaitCondation;

class HandleThread : public WorkThread
{
private:
    static SMutex mutex;
    static SWaitCondation allComplete;
    RequestQueue<Request*>* p_requestQueue;
    static ThreadPool* p_threadPool;
    static Config* p_config;
    static int numOfComplete;
    static bool isWaitForRequest;

    void waitForAllComplete();
public:
    static void onRequestReady();
    static void onRequestOver();
public:
    HandleThread();
    void init(int _thID,RequestQueue<Request*>* _p_requestQueue);
    void run(); //override WorkThread
    virtual ReturnType handleRequest(Request*) = 0; //will overrided by successor
};

#endif // HANDLETHREAD_H