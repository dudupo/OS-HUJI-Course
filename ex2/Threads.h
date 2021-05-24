
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


/*
 *  Thread class, responsible, to initialize the thread,
 *  and hold meta data as function address, quantums counter,
 *  etc...
 */
class Thread {
    public:
        static int const READY = 1;
        static int const RUNNING = 2;
        static int const BLOCKED = 3;
        /*
         *  Thread, initilaize empty thread.
         */
        Thread( );
        /*
         *  Thread, initilaize thread, where, tid is it's id
         *  and f it's his function.
         */
        Thread ( void(*f)(void), int tid );

        int getid () { return tid; }
        int state;
        void (*f) (void);
        int tid;
        int total_quantums = 0;
        sigjmp_buf env[1];
        char stack[STACK_SIZE];
private:
};





#endif