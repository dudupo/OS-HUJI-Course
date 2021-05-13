
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
        void run();
        friend class Scheduler;
        friend void mainCaller( int tid);

        int state;
        int tid;
        int total_quantums = 0;
        sigjmp_buf env[1];
        void (*f) (void);
        char stack[STACK_SIZE];
private:
};





#endif