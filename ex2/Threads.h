
#ifndef THREAD 

#define THREAD

#include <list>
#include <vector>
#include <map>
#include <list>
#include <utility>
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include "uthreads.h"

class Thread {
    
    
    public:
        static int const READY = 1;
        static int const RUNNING = 2;
        static int const BLOCKED = 3;

        Thread( );

        Thread ( void(*f)(void), int tid ); 
        int getid () { return tid; }

        friend class Scheduler;
        friend void mainCaller( int tid);

        int state;
    private:
        int tid;
        int total_quantums;
        sigjmp_buf env[1];
        void (*f) (void);
        char stack[STACK_SIZE];
};

class Scheduler {
    
    public:
        static Scheduler& getInstance()
        {
            static Scheduler instance;
            return instance;
        }
        int uthread_init(int quantum_usecs);
        int uthread_spawn(void (*f)(void));
        int uthread_terminate( int tid);
        int uthread_block(int tid);
        int uthread_resume(int tid);
        int uthread_mutex_lock();
        int uthread_mutex_unlock();
        int uthread_get_tid();
        int uthread_get_total_quantums();
        int uthread_get_quantums(int tid);
        friend void mainCaller( int tid);
        int contain( int tid ) ; 
        void main();
    private:
        static Thread * queue[MAX_THREAD_NUM];

        static int g_quantum_usecs;
        static int g_currenttid;
        static int g_mutex;
        static std::list< Thread* > g_blockedMutexQueue;

        int quantum_usecs;
        int total_quantums;
        int mutex = -1;        
        int thread_counter = 0;        
        int currenttid = 0;

        Scheduler() : total_quantums(1) {}
        int contextSwitch( Thread enteringThread, Thread oldTread);
        int findreadyJob( int hist );
        
};

void mainScheduler( int sig );




#endif