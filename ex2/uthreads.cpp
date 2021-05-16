
#include "uthreads.h"
#include "Threads.h"
#include <iostream>

// #ifndef _UTHREADS_H
// #define _UTHREADS_H

/*
 * User-Level Threads Library (uthreads)
 * Author: OS, os@cs.huji.ac.il
*/

// #define MAX_THREAD_NUM 100 /* maximal number of threads */
// #define STACK_SIZE 4096 /* stack size per thread (in bytes) */

/* External interface */



Thread * queue[MAX_THREAD_NUM];
int g_quantum_usecs;
int g_currenttid;
int g_mutex = -1;
int g_total_quantums = 0;

std::list< Thread* > g_fifoQueue;
std::list< Thread* > g_blockedMutexQueue;
std::list< Thread* > g_blockedQueue;

void handleError( )
{
    std::cerr << "thread library error: " << " ERROR " << std::endl;
}

void emptyf()
{
    std::cout << "emptf\n";
}


void setSIGINT();
void SchedulerMain(int sig);

#define MACRO_BLOCK_SIG( sigsetin, sigsetout )\
    sigset_t sigsetin  =  { SIGVTALRM, SIGINT };\
    sigset_t sigsetout;\
    sigprocmask( SIG_BLOCK,  &sigsetin, &sigsetout);

#define MACRO_UNBLOCK_SIG( sigsetin, sigsetout )\
    sigprocmask( SIG_UNBLOCK,  &sigsetin, &sigsetout);


/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * the length of a quantum in micro-seconds. It is an error to call this
 * function with non-positive quantum_usecs.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int quantum_usecs)
{
    // handle errors.
    if ( quantum_usecs <= 0 )
    {
        handleError();
        return -1;
    }
    g_quantum_usecs = quantum_usecs;
    g_mutex = -1;
    for (int i = 0; i < MAX_THREAD_NUM; i++ )
    {
        queue[i] = nullptr;
    }
    setSIGINT();
    queue[0] = new Thread(&emptyf, 0);
    g_fifoQueue.push_back(queue[0]);
    g_currenttid = 0;

    if ( sigsetjmp(queue[0]->env[0], 1) != 0)
    {
        return 0;
    }
    else
    {
        SchedulerMain(0);
    }
    return 0;
}

/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void))
{
    MACRO_BLOCK_SIG( sigsetin, sigsetout )
    int tid = 1;
    for (; tid <  MAX_THREAD_NUM && \
     queue[tid] != nullptr ; tid++ ) {  }

    // handle error
    if ( tid == MAX_THREAD_NUM )
    {
        handleError();
        MACRO_UNBLOCK_SIG( sigsetin, sigsetout )
        return -1;
    }
    queue[tid] = new Thread(f, tid);
    g_fifoQueue.push_back( queue[tid]  );
    MACRO_UNBLOCK_SIG( sigsetin, sigsetout )
    return tid;
}

int contain( int tid )
{
    return (tid < MAX_THREAD_NUM) && (queue[tid] != nullptr);
}

#define CHECK_EXISTENCE \
    if (!contain(tid))\
    {                  \
        handleError();\
        return -1;\
    }


void exitfunc()
{
    exit(0);
}

/*
 * Description: This function terminates the thread with ID tid and deletes
 * it from all relevant control structures. All the resources allocated by
 * the library for this thread should be released. If no thread with ID tid
 * exists it is considered an error. Terminating the main thread
 * (tid == 0) will result in the termination of the entire process using
 * exit(0) [after releasing the assigned library memory].
 * Return value: The function returns 0 if the thread was successfully
 * terminated and -1 otherwise. If a thread terminates itself or the main
 * thread is terminated, the function does not return.
*/
int uthread_terminate(int tid)
{
    MACRO_BLOCK_SIG( sigsetin, sigsetout )
    // handle error
    CHECK_EXISTENCE
    if ( tid == 0 )
    {
        std::cout << " terminate \n";
        queue[0] = new Thread(&exitfunc, 0);
        exit(0);
        MACRO_UNBLOCK_SIG( sigsetin, sigsetout )
    }
    else
    {

        if (g_mutex == tid)
        {
            uthread_mutex_unlock();
        }
//        std::cout << " terminate " << tid << "\n";
        queue[tid] = nullptr;
    }
    MACRO_UNBLOCK_SIG( sigsetin, sigsetout )
    return 0;
}


/*
 * Description: This function blocks the thread with ID tid. The thread may
 * be resumed later using uthread_resume. If no thread with ID tid exists it
 * is considered as an error. In addition, it is an error to try blocking the
 * main thread (tid == 0). If a thread blocks itself, a scheduling decision
 * should be made. Blocking a thread in BLOCKED state has no
 * effect and is not considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_block(int tid)
{
    MACRO_BLOCK_SIG(sigsetin, sigsetout)

    // handle error
    CHECK_EXISTENCE

    if ( tid == 0)
    {
        handleError();
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        return  -1;
    }
    queue[tid]->state = Thread::BLOCKED;
    if ( tid == uthread_get_tid())
    {
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        raise(SIGINT);
    }
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)



    return 0;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state if it's not synced. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
    // handle error
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    CHECK_EXISTENCE
    queue[tid]->state = Thread::READY;
    g_fifoQueue.push_back(queue[tid]);
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    return 0;
}



static jmp_buf buf;
volatile sig_atomic_t mux = false;
static struct itimerval old, newitimer;
static struct sigaction sa = {0};
static struct sigaction saINT = {0};
void mainCaller(int sig);

unsigned int vt_alarm (int miliseconds)
{
    signal(SIGVTALRM, &SchedulerMain);

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




void mainCaller(int sig)
{

}

void SIGSEGVHandler(int sig) {
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    int tid = uthread_get_tid();
    if (contain(tid) )
        uthread_terminate(tid);
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    while (1) {
    }
}

void SchedulerMain(int sig)
{

    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    reset_itimer();

    if (sig != 0 ) {
        if (contain(g_currenttid)) {
            if ( sig == SIGVTALRM) {
                g_fifoQueue.push_back(queue[g_currenttid]);
            }
            if (sigsetjmp(queue[g_currenttid]->env[0], 1) != 0) {
                return;
            }
            else
            {
                reset_itimer();
            }
        }
    }
    sigprocmask(SIG_BLOCK ,&sigsetin, &sigsetout);
     if (contain(g_currenttid)) {
         if (queue[g_currenttid]->state == Thread::RUNNING) {
             queue[g_currenttid]->state = Thread::READY;
         }
     }
    std::list<std::list< Thread *>::iterator> del;


    if( contain(0)) {
        auto iter = g_fifoQueue.begin();

        for (; iter != g_fifoQueue.end(); ++iter) {
            if (!contain((*iter)->tid)) {
                del.push_back(iter);
            }
        }

        for (auto it : del) {
            g_fifoQueue.erase(it);
        }

        std::list<std::list<Thread *>::iterator> shift_list;

        for (iter = g_fifoQueue.begin(); iter != g_fifoQueue.end() && \
                    (*iter)->state != Thread::READY;
             ++iter) {

            if ( (*iter)->state == Thread::BLOCKED )
            {
                shift_list.push_back(iter);
            }
        }

        for ( auto& it : shift_list)
        {
            g_fifoQueue.erase( it );
//            g_fifoQueue.push_back(*it);
        }

        g_currenttid = (*iter)->tid;// findreadyJob(g_currenttid);
//        std::cout << "tid " << g_currenttid << "\n";
        queue[g_currenttid]->state = Thread::RUNNING;
        g_total_quantums += 1;
        queue[g_currenttid]->total_quantums += 1;

        g_fifoQueue.erase(iter);


        vt_alarm(g_quantum_usecs);
        signal(SIGSEGV, &SIGSEGVHandler);
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        siglongjmp(queue[g_currenttid]->env[0], 1);
        std::cout << " end function " << std::endl;
    }
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
}

/*
 * Description: This function tries to acquire a mutex. 
 * If the mutex is unlocked, it locks it and returns. 
 * If the mutex is already locked by different thread, the thread moves to BLOCK state. 
 * In the future when this thread will be back to RUNNING state, 
 * it will try again to acquire the mutex. 
 * If the mutex is already locked by this thread, it is considered an error. 
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_mutex_lock()
{
    // block the signals
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    int tid = uthread_get_tid();
    // handle the case where the thread is
    // alrady locked by this thread.
    if ( g_mutex == tid )
    {
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        handleError();
        return -1;
    }


    while ( g_mutex != -1 )
    {
        if ( queue[tid]->state != Thread::BLOCKED ) {
            g_blockedMutexQueue.push_back(queue[tid]);
            queue[tid]->state = Thread::BLOCKED;
            g_blockedMutexQueue.push_back(queue[tid]);
            raise( SIGINT );
            MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
//            uthread_block( uthread_get_tid() );
        }
        sigprocmask( SIG_BLOCK,  &sigsetin, &sigsetout);
    }

    g_mutex = tid;
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    return 0;
}


/*
 * Description: This function releases a mutex. 
 * If there are blocked threads waiting for this mutex, 
 * one of them (no matter which one) moves to READY state.
 * If the mutex is already unlocked, it is considered an error. 
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_mutex_unlock()
{
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    // handle an error.
    int tid = uthread_get_tid();
    if ( g_mutex == -1 || g_mutex != tid )
    {
        MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
        handleError();
        return -1;
    }

    g_mutex = -1;
    // relase a witing thread.

    for (auto& it : g_blockedMutexQueue) {
        it->state = Thread::READY;
        g_fifoQueue.push_back(it);
    }
    g_blockedMutexQueue.clear();
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    return 0;
}


/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid()
{
    int ret;
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    ret = g_currenttid;
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    return ret;
}


/*
 * Description: This function returns the total number of quantums since
 * the library was initialized, including the current quantum.
 * Right after the call to uthread_init, the value should be 1.
 * Each time a new quantum starts, regardless of the reason, this number
 * should be increased by 1.
 * Return value: The total number of quantums.
*/
int uthread_get_total_quantums()
{
    int ret;
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    ret = g_total_quantums;
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    return ret;
}


/*
 * Description: This function returns the number of quantums the thread with
 * ID tid was in RUNNING state. On the first time a thread runs, the function
 * should return 1. Every additional quantum that the thread starts should
 * increase this value by 1 (so if the thread with ID tid is in RUNNING state
 * when this function is called, include also the current quantum). If no
 * thread with ID tid exists it is considered an error.
 * Return value: On success, return the number of quantums of the thread with ID tid.
 * 			     On failure, return -1.
*/
int uthread_get_quantums(int tid)
{

    int ret;
    MACRO_BLOCK_SIG(sigsetin, sigsetout)
    CHECK_EXISTENCE
    ret = queue[tid]->total_quantums;
    MACRO_UNBLOCK_SIG(sigsetin, sigsetout)
    return ret;
}




void test()
{
    std::cout << "hi" << std::endl;
//    for(;;)
//    {
//
//    }
//    asm("int $3");
}
void test1()
{
    int j =0;
    for (int k=0 ; k < 100; k++ )
    {
        int i = 0;
        for (; i< 300000; i++)
        {

        }
//        uthread_mutex_lock();
        j += i;
        std::cout << "hi2 " << j << std::endl;
    }
    uthread_terminate(3);
}
void test2()
{
    struct itimerval old, newitimer;
    getitimer( ITIMER_REAL, &old );
    int j = 0;
    for ( ;; )
    {
        for (int i = 0; i< 1000000; i++)
        {
            j = i;
        }
//       uthread_mutex_lock();
        std::cout << "hi3 " << j <<  std::endl;
    }

//    uthread_mutex_unlock();
}

void test4(int sig)
{
    std::cout << "test4" <<std::endl;
}

void setSIGINT()
{
    sa.sa_handler = &SchedulerMain;
    sa.sa_flags = 0;
    saINT.sa_handler = &SchedulerMain;

    if (sigaction(SIGINT, &saINT,NULL) < 0) {
        printf("sigaction error.");
    }
    signal(SIGVTALRM, &SchedulerMain);
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
    int myId = uthread_get_tid();
    int start = uthread_get_quantums(myId);
    int end = start + threadQuants;

    std::cout << end << std::endl;

    while (uthread_get_quantums(myId)  <= end)
    {
    }
}

int main(int argc, char const *argv[])
{

    signal(SIGVTALRM, &SchedulerMain);

    uthread_init(100);
    uthread_spawn(  &test);
    uthread_spawn(  &test1 );
    uthread_spawn(  &test2 );
//     Scheduler::getInstance().main();

//    threadQuantumSleep(1);
//     test1();
    int j =0 ;
    for (int k = 0; k < 10; k++)
    {
        int i = 0;
        for (; i< 200000000; i++)
        {

        }
        j += i;
        std::cout << "hi5 " << j << std::endl;
    }
    // raise(SIGINT);
    return 0;
}

#endif
