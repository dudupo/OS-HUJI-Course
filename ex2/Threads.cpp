#include "Threads.h"
#include <iostream>
#include <list>
#include <utility>
#include <vector>


/*
    Static Private Segement. (Scheduler) 
*/
Thread * Scheduler::queue[MAX_THREAD_NUM];
int Scheduler::g_quantum_usecs;
int Scheduler::g_currenttid;
int Scheduler::g_mutex = -1;
int Scheduler::g_total_quantums = 0;
std::list< Thread* > Scheduler::g_blockedMutexQueue;

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

void emptyf()
{

}

void handleError( )
{
    std::cerr << "thread library error: " << " ERROR " << std::endl;
}


Thread::Thread( ) : state(READY), tid(0)
{
    sigemptyset(&this->env[0]->__saved_mask);
}

Thread::Thread ( void(*f)(void), int tid ) : state(READY), f(f), tid(tid)
{
    address_t sp, pc;
    sp = (address_t)this->stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)f;
    
    sigsetjmp(this->env[0], 1);
    (this->env[0]->__jmpbuf)[JB_SP] = translate_address(sp);
    (this->env[0]->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&this->env[0]->__saved_mask);
}
void setSIGINT();
int Scheduler::uthread_init(int quantum_usecs) 
{

    // handle errors.
    if ( quantum_usecs <= 0 )
    {
        handleError();
        return -1;
    }
    this->quantum_usecs = quantum_usecs;
    g_total_quantums = 0;
    g_quantum_usecs = quantum_usecs;
    g_mutex = -1;
    for (int i = 0; i < MAX_THREAD_NUM; i++ )
    {
        queue[i] = nullptr;
    }
    setSIGINT();
    queue[0] = new Thread( &emptyf, 0);
    g_currenttid = 0;
    if ( sigsetjmp(queue[0]->env[0], 1) != 0)
    {
        std::cout << "[0] __ init __" << std::endl;
        return 0;
    }
    else 
    {
        Scheduler::getInstance().main();
    }
    return 0;
}

int Scheduler::uthread_spawn(void (*f)(void))
{

    // update the number of initialztions. 
    this->thread_counter++;  

    int tid = 1;    
    for (; tid <  MAX_THREAD_NUM && \
     queue[tid] != nullptr ; tid++ ) {  }

    // handle error
    if ( tid == MAX_THREAD_NUM )
    {
        handleError();
        return -1;
    }
    queue[tid] = new Thread(f, tid);
    return tid;  
}

int Scheduler::contain( int tid )
{
    return tid < MAX_THREAD_NUM && queue[tid] != nullptr; 
} 

#define CHECK_EXISTENCE\
    if (!this->contain(tid))\
    {                  \
        handleError();\
        return -1;\
    }



void exitfunc()
{
    exit(0);
}
int Scheduler::uthread_terminate(int tid)
{
    // handle error
    CHECK_EXISTENCE

    std::cout << tid << std::endl;

    if ( tid == 0 )
    {
        queue[0] = new Thread(&exitfunc, 0);
        exit(0);
    }
    else
    {
        queue[tid] = nullptr;
    }

    return 0;
}
int Scheduler::uthread_block(int tid)
{
    // handle error
    CHECK_EXISTENCE

    if ( tid == 0)
    {
        handleError();
        return  -1;
    }

    queue[tid]->state = Thread::BLOCKED; 
    return 1;

}
int Scheduler::uthread_resume(int tid)
{
    // handle error
    CHECK_EXISTENCE
    queue[tid]->state = Thread::READY;
    return 1;
}

static jmp_buf buf;
volatile sig_atomic_t mux = false;
static struct itimerval old, newitimer;
static struct sigaction sa = {0};
static struct sigaction saINT = {0};
void mainCaller(int sig);

unsigned int vt_alarm (int miliseconds)
{
    signal(SIGVTALRM, &mainCaller);

    newitimer.it_interval.tv_usec = 0;
    newitimer.it_interval.tv_sec = 0;
    newitimer.it_value.tv_usec = miliseconds;
    newitimer.it_value.tv_sec = 0;
  
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


#define MACRO_BLOCK_SIG( sigsetin, sigsetout )\
    sigset_t sigsetin  =  { SIGVTALRM, SIGINT };\
    sigset_t sigsetout;\
    sigprocmask( SIG_BLOCK,  &sigsetin, &sigsetout);  

#define MACRO_UNBLOCK_SIG( sigsetin, sigsetout )\
    sigprocmask( SIG_UNBLOCK,  &sigsetin, &sigsetout);  


void mainCaller(int sig)
{
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    if (sig != -1)
    {

        std::cout << "mainCaller(int sig) " <<  Scheduler::getInstance().g_currenttid << std::endl;

        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        int ret = sigsetjmp(\
         Scheduler::getInstance().queue[\
          Scheduler::getInstance().g_currenttid ]->env[0], 1);
        if (ret != 0)
        {
            return;
        }
    }
    reset_itimer();
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    siglongjmp(buf, 1);
}

int Scheduler::findreadyJob( int hist )
{
    std::cout << hist << std::endl;
    int i = hist; 
    for (; (queue[i] == nullptr) || (
        queue[i]->state != Thread::READY );
            i = (i+1)% MAX_THREAD_NUM ) { }            
    return i;
}

void Scheduler::main()
{   
    // Setting up jumping location.
    if (!sigsetjmp(buf, 1))
    {
        mainCaller(-1);
    }
    else 
    {
        MACRO_BLOCK_SIG(sigsetin, sigsetout)
        if ( queue[g_currenttid]->state == Thread::RUNNING )
        {
            queue[g_currenttid]->state = Thread::READY;
        }
        g_currenttid += 1;


        // Advances the queue.
        // Executing the function.
        g_currenttid = findreadyJob( g_currenttid );
        std::cout << "void Scheduler::main() " << g_currenttid << "," << queue[g_currenttid]->total_quantums  <<  "\n";
        queue[g_currenttid]->state  = Thread::RUNNING;
        queue[g_currenttid]->total_quantums += 1;
        g_total_quantums += 1;
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        vt_alarm(g_quantum_usecs);
        siglongjmp(queue[g_currenttid]->env[0],1);
    }
    raise(SIGINT);
}

int Scheduler::uthread_mutex_lock()
{   

    // block the signals 
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    int tid = this->uthread_get_tid();
    // handle the case where the thread is 
    // alrady locked by this thread. 
    if ( g_mutex == tid )
    {
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        handleError();
        return -1;
    }
   
    if ( g_mutex != -1 )
    {
        g_blockedMutexQueue.push_back(queue[tid]);
        queue[tid]->state = Thread::BLOCKED;
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)        
        raise( SIGINT );
    }
    else 
    {
        g_mutex = tid;
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    }
    return 1;
}
int Scheduler::uthread_mutex_unlock()
{

    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    // handle an error.
    if ( g_mutex == -1 )
    {
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        handleError();
        return -1;
    }

    int tid = this->uthread_get_tid();
    g_mutex = -1;
    // relase a witing thread. 
    if ( g_blockedMutexQueue.size() > 0 )
    {
        g_blockedMutexQueue.back()->state = Thread::READY;
        g_blockedMutexQueue.pop_back();
    }
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    return 1;
}

int Scheduler::uthread_get_tid()
{
    return g_currenttid;
}

int Scheduler::uthread_get_total_quantums()
{
    return g_total_quantums;
}

int Scheduler::uthread_get_quantums(int tid)
{   
    CHECK_EXISTENCE
    return queue[tid]->total_quantums;
}
void test()
{
    std::cout << "hi" << std::endl;
    for(;;)
    {
    
    }
}
void test1()
{
    int j =0;
    for ( ;; )
    {
        int i = 0;
        for (; i< 30000000; i++)
        {
            
        }
//        Scheduler::getInstance().uthread_mutex_lock();
        j += i;
        std::cout << "hi2 " << j << std::endl;
    }
}
void test2()
{
    struct itimerval old, newitimer;
    getitimer( ITIMER_REAL, &old );
    int j = 0;
    for ( ;; )
    {
        for (int i = 0; i< 100000000; i++)
        {   
            j = i;
        }
//        Scheduler::getInstance().uthread_mutex_lock();
        std::cout << "hi3 " << j <<  std::endl;
    } 

    Scheduler::getInstance().uthread_mutex_unlock();
}

void test4(int sig)
{
    std::cout << "test4" <<std::endl;
}

void setSIGINT()
{
    sa.sa_handler = &mainCaller;
    sa.sa_flags = 0;
    saINT.sa_handler = &mainCaller;

    if (sigaction(SIGINT, &saINT,NULL) < 0) {
		printf("sigaction error.");
	}
    signal(SIGVTALRM, &mainCaller);
}
//
#ifdef DEBUG
#define DEBUG
void threadQuantumSleep(int threadQuants)
{
    /* Note, from the thread's standpoint, it is almost impossible for two consecutive calls to
     * 'uthread_get_quantum' to yield a difference larger than 1, therefore, at some point, uthread_get_quantums(myId)
     * must obtain 'end'.
     *
     * Theoretically, it's possible that the thread will be preempted before the condition check occurs, if this happens,
     * the above won't hold, and you'll get an infinite loop. But this is unlikely, as the following operation should
     * take much less than a microsecond

     */
//    assert (threadQuants > 0);
    int myId = Scheduler::getInstance().uthread_get_tid();
    int start = Scheduler::getInstance().uthread_get_quantums(myId);
    int end = start + threadQuants;

    std::cout << end << std::endl;

    while (Scheduler::getInstance().uthread_get_quantums(myId)  <= end)
    {
    }
}

int main(int argc, char const *argv[])
{



    signal(SIGVTALRM, &mainCaller);

    Scheduler::getInstance().uthread_init(1);
    // Scheduler::getInstance().uthread_spawn(  &test);
//    Scheduler::getInstance().uthread_spawn(  &test1 );
//    Scheduler::getInstance().uthread_spawn(  &test2 );
    // Scheduler::getInstance().main();

    threadQuantumSleep(1);
    // test1();
    int j =0 ;
    for (;;)
    {
         int i = 0;
        for (; i< 100000000; i++)
        {

        }
        j += i;
        std::cout << "hi5 " << j << std::endl;
    }
    // raise(SIGINT);
    return 0;
}

#endif
