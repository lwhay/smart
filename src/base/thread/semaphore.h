#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <semaphore.h>
#include <time.h>

class SSemaphore
{
private:
    sem_t sem;
    timespec ts;
public:
    SSemaphore(unsigned int value){sem_init(&sem,0,value);}
    ~SSemaphore(){ sem_destroy(&sem);}
    int P(){return sem_wait(&sem);}
    int tryP(){return sem_trywait(&sem);}
    int timedP(unsigned long msecs)
    {
        ts = STime::makeTimespec(msecs / 1000, msecs % 1000 * 1000 * 1000);
        return sem_timedwait(&sem,&ts);
    }
    int V(){return sem_post(&sem);}
    int getValue(int *value) {return sem_getvalue(&sem,value);}
};

#endif // SEMAPHORE_H