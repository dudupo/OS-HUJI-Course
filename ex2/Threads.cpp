#include "Threads.h"
#include <iostream>
#include <list>
#include <utility>
#include <vector>


#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define SECOND 1000000
#define STACK_SIZE 4096

char stack1[STACK_SIZE];
char stack2[STACK_SIZE];

sigjmp_buf env[2];

#ifdef __x86_64__
/* code for 64 bit Intel arch */

typedef unsigned long address_t;
#define JB_SP 6
#define JB_PC 7

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%fs:0x30,%0\n"
		"rol    $0x11,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#else
/* code for 32 bit Intel arch */

typedef unsigned int address_t;
#define JB_SP 4
#define JB_PC 5 

/* A translation is required when using an address of a variable.
   Use this as a black box in your code. */
address_t translate_address(address_t addr)
{
    address_t ret;
    asm volatile("xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif



int Scheduler::uthread_init(int *quantum_usecs, int size)
{
    if ( size <= 0 )
        return -1;

    this->quantum_usecs =\
     new std::vector<int>(quantum_usecs, quantum_usecs+ size*sizeof(int)); 
    return 0;
}

int Scheduler::uthread_spawn(void (*f)(void), int priority)
{
    std::pair<Thread *, int> pair ( new Thread(f) , priority);
    this->radyQueue.push_back(pair);
    return 1;  
}

static jmp_buf buf;
volatile sig_atomic_t mux = false;
static struct itimerval old, newitimer;
static struct sigaction sa = {0};
static struct sigaction saINT = {0};
void mainCaller(int sig);

unsigned int vt_alarm (int seconds)
{
    signal(SIGVTALRM, &mainCaller);

    newitimer.it_interval.tv_usec = 0;
    newitimer.it_interval.tv_sec = 0;
    newitimer.it_value.tv_usec = 0;
    newitimer.it_value.tv_sec = seconds;
  
    if (setitimer (ITIMER_VIRTUAL, &newitimer, &old) == -1)
    {
        perror("setitimer");
        return 0;
    }
    else
        return old.it_value.tv_sec;
}

void reset_itimer()
{
    if (setitimer (ITIMER_VIRTUAL, NULL, NULL) == -1)
    {
        perror("setitimer");
    }
}

void mainCaller(int sig)
{



    mux = true;
    sigset_t y  =  { SIGVTALRM, SIGINT };
    sigset_t omask;
    sigprocmask( SIG_UNBLOCK,  &y, &omask);    
    reset_itimer(); 
    siglongjmp(buf, 1);
}
void Scheduler::main()
{
    
    if (!sigsetjmp(buf, 1))
    {
        mainCaller(0);
    }
    else 
    {
        auto job = this->radyQueue.back();
        this->radyQueue.pop_back(); 
        this->radyQueue.insert( this->radyQueue.begin(), job );
        vt_alarm( this->quantum_usecs->at(job.second) );
        job.first->run();   
    }
    raise(SIGINT);
}

void test()
{
    std::cout << "hi" << std::endl;
}
void test1()
{
    std::cout << "hi2" << std::endl;
}
void test2()
{
    struct itimerval old, newitimer;
    getitimer( ITIMER_REAL, &old );
    std::cout << "hi3 " << old.it_value.tv_sec <<  std::endl;
    for ( ;; )
    {

    } 
}

void test4(int sig)
{
    std::cout << "test4" <<std::endl;
}

int main(int argc, char const *argv[])
{
    

	// struct itimerval timer;

	// Install timer_handler as the signal handler for SIGVTALRM.
	sa.sa_handler = &mainCaller;
    sa.sa_flags = 0;
    saINT.sa_handler = &mainCaller;
    // sa.sa_flags = SA_ONSTACK | SA_NOCLDWAIT;
	// if (sigaction(SIGVTALRM, &sa,NULL) < 0) {
	// 	printf("sigaction error.");
	// }

    if (sigaction(SIGINT, &saINT,NULL) < 0) {
		printf("sigaction error.");
	}

    signal(SIGVTALRM, &mainCaller);

    int quantum_usecs[3] = {1 ,1 ,3};
    Scheduler::getInstance().uthread_init( quantum_usecs, 3 );
    Scheduler::getInstance().uthread_spawn(  &test, 0 );
    Scheduler::getInstance().uthread_spawn(  &test1, 1 );
    Scheduler::getInstance().uthread_spawn(  &test2, 2 );
    

    // if (!setjmp(buf))
    // {
    //     mainCaller(0);
    // }
    // else 
    // {
    
    // }

    Scheduler::getInstance().main();
    // for (;;){
    // }
    // // mainCaller(0);
    // sa.sa_handler = &test4; 
    
    // if (sigaction(SIGVTALRM, &sa,NULL) < 0)
    // { 
	// 	printf("sigaction error.");
	// }

    // std::cout << "hiddd" << std::endl;
    // vt_alarm(1);
	// raise(SIGVTALRM);

    // raise(SIGINT);
    // raise(SIGINT);
    return 0;
}
