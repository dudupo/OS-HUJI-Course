
#ifndef THREAD 

#define THREAD

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
        Thread ( void(*f)(void), int tid ); 

        void run(); 
        int getid () { return tid; }

        friend class Scheduler;
        friend void mainCaller( int tid);

    private:
        int tid;
        int state;
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
        inline int contain( int tid ) {  return this->thread_table.find(tid) != this->thread_table.end(); }
        void main();
    private:
        int quantum_usecs;
        int total_quantums;
        int mutex = -1;
        
        std::vector<Thread *> radyQueue ; 
        std::map<int, Thread *> thread_table; 
        int thread_counter = 0;
        std::vector<Thread *>::iterator pointer = radyQueue.begin();
        std::vector<Thread *> blockedMutexQueue;
        
        Scheduler() : total_quantums(1) {}
        int contextSwitch( Thread enteringThread, Thread oldTread);
        
        Thread * vpointer() { return (*this->pointer);}
        void operator++( )
        {
            if ( this->pointer == this->radyQueue.end()-1 )
            {
                this->pointer = this->radyQueue.begin();
            }
            else 
            {
                this->pointer++;
            }
        }

};



void mainScheduler( int sig );




#endif