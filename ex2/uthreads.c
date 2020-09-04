#include "uthreads.h"
#include "p_uthreads.h"
#include "./datastructure/list.h"
#include "stdlib.h"
#include "string.h"

#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define DEBUG

#ifdef DEBUG
#include "stdio.h"

#define DEBUG_PRINT(x)\
    printf(x);

#else
#define DEBUG_PRINT(x) ;
#endif


struct {

    int size;
    int * p_quantum_usecs;
    t_list ** theards;
    t_list* current; 

} mem_manager;


struct {
    const int SUCCESS = 0;
    const int FAILURE = -1;
} CODES;



void mem_manager_main() 
{


    /*

        thread1->thread2-> [ end, change the time interval  ]


        p1 ... 

        interval -> calling 



        while ( list... ) 
        {
            while( not_timeend )
            {

            }
        }
    */
    
} 



/*
 * Description: This function initializes the thread library.
 * You may assume that this function is called before any other thread library
 * function, and that it is called exactly once. The input to the function is
 * an array of the length of a quantum in micro-seconds for each priority. 
 * It is an error to call this function with an array containing non-positive integer.
 * size - is the size of the array.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_init(int *quantum_usecs, int size)
{
    DEBUG_PRINT("uthread_init::\n")

    if ( size < 0 | quantum_usecs == NULL)
    {
        return CODES.FAILURE;
    }
    
    // threads[2j] -> blocked threads with priority j.
    mem_manager.p_quantum_usecs = (int *) malloc( size );
    

    if ( mem_manager.p_quantum_usecs == NULL)
    {
        DEBUG_PRINT("malloc::p_quantum_usecs\n")
        return CODES.FAILURE;
    }

    mem_manager.size = size;

    // char const * ptr = (char const *) quantum_usecs;
    // char * p_iter = (char *) mem_manager.p_quantum_usecs;
    
    // while ( ptr < ((char const *) quantum_usecs + size) )
    // {
    //     *p_iter = *ptr;
    //     p_iter++; ptr++;
    // }

    memcpy(mem_manager.p_quantum_usecs, quantum_usecs, size);

    
    int index = 0;
    for (int * ptr = quantum_usecs; ptr != NULL; ptr++ )
    {
        mem_manager.threads[index] = ( list  ) malloc( sizeof( list ) );
        index++;
    }

    return CODES.SUCCESS;
}

int nodelist2id( list* p_node )
{
    return (int) (( void * ) p_node );
}

list* id2nodelist (int tid)
{
    return ( list*) ((void *) tid );
}

bool priority_validity(int priority)
{
    if ( priority < 0 || priority >= mem_manager.size)
    {
        return CODES.FAILURE; 
    }
    return CODES.SUCSSES;
}


/*
 * Description: This function creates a new thread, whose entry point is the
 * function f with the signature void f(void). The thread is added to the end
 * of the READY threads list. The uthread_spawn function should fail if it
 * would cause the number of concurrent threads to exceed the limit
 * (MAX_THREAD_NUM). Each thread should be allocated with a stack of size
 * STACK_SIZE bytes.
 * priority - The priority of the new thread.
 * Return value: On success, return the ID of the created thread.
 * On failure, return -1.
*/
int uthread_spawn(void (*f)(void), int priority)
{
    if ( priority_validity(priority) == CODES.FAILURE )
    {
        return CODES.FAILURE; 
    }
    
    p_uthreads * warpper = init_p_uthreads(f, priority);    
    DEBUG_PRINT("p_uthreads initilaized: ")

    list * p_node = push(mem_manager.threads[priority], warpper); 
    #ifdef DEBUG
        printf( "%d\n" , nodelist2id(p_node ) ))
    #endif
    
    DEBUG_PRINT("p_uthreads pushed into list\n")
    return nodelist2id( p_node );    
}


/*
 * Description: This function changes the priority of the thread with ID tid.
 * If this is the current running thread, the effect should take place only the
 * next time the thread gets scheduled.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_change_priority(int tid, int priority)
{
    if ( priority_validity(priority) == CODES.FAILURE )
    {
        return CODES.FAILURE; 
    }
    
    // todo, implement a set.. and check validity. 
    list* p_list = id2nodelist(tid);
     // after poping, p_list has changed ( points to the precding node).
    list* orignal_node = pop(&p_list);  

    pushnode(mem_manager.threads[priority], orignal_node);
    return CODES.SUCSSES;
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
    return CODES.SUCSSES;
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
    if ( tid == 0 )
    {
        return CODES.FAILURE;
    }

    list* p_list = id2nodelist(tid);
    p_list->value->blocked = 1;
    return CODES.SUCSSES;
}


/*
 * Description: This function resumes a blocked thread with ID tid and moves
 * it to the READY state. Resuming a thread in a RUNNING or READY state
 * has no effect and is not considered as an error. If no thread with
 * ID tid exists it is considered an error.
 * Return value: On success, return 0. On failure, return -1.
*/
int uthread_resume(int tid)
{
    list* p_list = id2nodelist(tid);
    p_list->value->blocked = 0;
    return CODES.SUCSSES;
}


/*
 * Description: This function returns the thread ID of the calling thread.
 * Return value: The ID of the calling thread.
*/
int uthread_get_tid()
{
    return nodelist2id(mem_manager.current);
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
    return 0;
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
    return 0;
}

