#include "ThreadPool.h"
#include "core/center/Config.h"
#include "core/thread/KNNQueryThread.h"
#include "core/thread/RangeQueryThread.h"
#include "core/thread/AssignThread.h"
#include "core/thread/UpdateThread.h"
#include "core/index/DynamicIndex.h"
#include "core/index/OnlineTuning.h"


namespace smart {

ThreadPool::ThreadPool()
{
    p_periodTimer = NULL;
}

ThreadPool::~ThreadPool()
{
    delete p_periodTimer;

    for(unsigned int i = 0;i < assignThreads.size();++i)
    {
        delete assignThreads[i];
    }
    for(unsigned int i = 0;i < updateThreads.size();++i)
    {
        delete updateThreads[i];
    }
    for(unsigned int i = 0;i < queryThreads.size();++i)
    {
        delete queryThreads[i];
    }

    for(unsigned int i = 0;i < updateQueues.size();++i)
    {
        delete updateQueues[i];
    }
    for(unsigned int i = 0;i < queryQueues.size();++i)
    {
        delete queryQueues[i];
    }
}

void ThreadPool::init(Index* p_index)
{
    Config* p_config = Config::getObject();
    AssignThread* p_assignThread = NULL;
    for(int i =0;i<p_config->numOfAssignThreads;++i)
    {
        p_assignThread = new AssignThread();
        p_assignThread->init(i);
        assignThreads.push_back(p_assignThread);
    }
    RequestQueue<Request*> *p_queue = NULL;
    HandleThread* p_handleThread = NULL;
    for(int i=0;i<p_config->numOfUpdateThreads;++i)
    {
        p_handleThread = new UpdateThread();
        p_queue = new RequestQueue<Request*>();
        updateQueues.push_back(p_queue);
        p_handleThread->init(i,p_queue,p_index);
        updateThreads.push_back(p_handleThread);
    }
    if(p_config->queryType == QUERY_RANGE)
    {
        for(int i=0;i<p_config->numOfQueryThreads;++i)
        {
            p_handleThread = new RangeQueryThread();
            p_queue = new RequestQueue<Request*>();
            queryQueues.push_back(p_queue);
            p_handleThread->init(i,p_queue,p_index);
            queryThreads.push_back(p_handleThread);
        }
    }
    else
    {
        for(int i=0;i<p_config->numOfQueryThreads;++i)
        {
            p_handleThread = new KNNQueryThread();
            p_queue = new RequestQueue<Request*>();
            queryQueues.push_back(p_queue);
            p_handleThread->init(i,p_queue,p_index);
            queryThreads.push_back(p_handleThread);
        }
    }

    //创建周期定时器
    p_periodTimer = new PeriodTimer;
    p_periodTimer->init(static_cast<unsigned long>(p_config->maxUpdateTime*100));
    //周期到来时，先暂停所有处理线程，然后打印线程信息，然后调整索引，最后更新周期并唤醒所有线程
    p_periodTimer->addListener(HandleThread::pauseForPeriodCome);
    p_periodTimer->addListener(this,&ThreadPool::printThreadStatus);
    //如果使用动态索引，需要调整索引
    if(dynamic_cast<DynamicIndex*>(p_index))
        p_periodTimer->addListener(dynamic_cast<DynamicIndex*>(p_index),&DynamicIndex::tune);
    p_periodTimer->addListener(WorkThread::increasePeriod);
    p_periodTimer->addListener(HandleThread::onPeriodCome);

    SConnectGG(AssignThread::requestReady,HandleThread::onRequestReady);
    SConnectGG(AssignThread::requestOver,HandleThread::onRequestOver);
}

void ThreadPool::startAll()
{
    for(unsigned int i = 0;i < assignThreads.size();++i)
    {
        assignThreads[i]->start();
    }
    //启动定时器
    p_periodTimer->start();

    for(unsigned int i = 0;i < updateThreads.size();++i)
    {
        updateThreads[i]->start();
    }
    for(unsigned int i = 0;i < queryThreads.size();++i)
    {
        queryThreads[i]->start();
    }
}

void ThreadPool::stopAll()
{
    for(unsigned int i = 0;i < assignThreads.size();++i)
    {
        assignThreads[i]->stop();
    }
    for(unsigned int i = 0;i < updateThreads.size();++i)
    {
        updateThreads[i]->stop();
    }
    for(unsigned int i = 0;i < queryThreads.size();++i)
    {
        queryThreads[i]->stop();
    }
}

void ThreadPool::waitForAllOver()
{
    for(unsigned int i = 0;i < assignThreads.size();++i)
    {
        assignThreads[i]->over();
    }
    for(unsigned int i = 0;i < updateThreads.size();++i)
    {
        updateThreads[i]->over();
    }
    for(unsigned int i = 0;i < queryThreads.size();++i)
    {
        queryThreads[i]->over();
    }
}

void ThreadPool::printThreadStatus()
{
    int sumOfDiscard = 0;
    int sumOfSuccess = 0;
    int tmpDiscard = 0;
    int tmpSuccess = 0;
    for(unsigned int i = 0;i < assignThreads.size();++i)
    {
        assignThreads[i]->getStatus(tmpDiscard,tmpSuccess);
        sumOfDiscard += tmpDiscard;
        sumOfSuccess += tmpSuccess;
    }

    SInfo("period:%d assignDiscard:%d assignSuccess:%d",WorkThread::period,sumOfDiscard,sumOfSuccess);
    sumOfDiscard = 0;
    sumOfSuccess = 0;
    for(unsigned int i = 0;i < updateThreads.size();++i)
    {
        updateThreads[i]->getStatus(tmpDiscard,tmpSuccess);
        sumOfDiscard += tmpDiscard;
        sumOfSuccess += tmpSuccess;
    }
    SInfo("period:%d updateDiscard:%d updateSuccess:%d",WorkThread::period,sumOfDiscard,sumOfSuccess);
    sumOfDiscard = 0;
    sumOfSuccess = 0;
    for(unsigned int i = 0;i < queryThreads.size();++i)
    {
        queryThreads[i]->getStatus(tmpDiscard,tmpSuccess);
        sumOfDiscard += tmpDiscard;
        sumOfSuccess += tmpSuccess;
    }
    SInfo("period:%d queryDiscard:%d querySuccess:%d",WorkThread::period,sumOfDiscard,sumOfSuccess);
}

}
