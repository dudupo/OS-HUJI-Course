#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#include "uthreads.h"
#include "p_uthreads.h"
#include "datastructures/list.h"
#include "stdlib.h"
#include "string.h"


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
    list ** threads;
    list* current; 
    int current_priority;
    int runnerid;
	int runners;
    
} mem_manager;

struct itimerval timer;

struct sigaction sa;


struct {
    const int SUCCESS ;
    const int FAILURE ;
} CODES = { 0 , -1};



int gotit = 0;


void update_current_thread( )
{
	if ( 	mem_manager.current != NULL &&
	 		mem_manager.current->next != NULL  )
  	{
		mem_manager.current = mem_manager.current->next;
	}
	else
 	{
		mem_manager.current_priority = ( mem_manager.current_priority + 1 ) % mem_manager.size; 
		
		
		// int last_priority = mem_manager.current_priority;  
		// do {
			
		// } while( mem_manager.runners > 0 && 
		// 	( 	mem_manager.threads[mem_manager.current_priority] == NULL ||
		// 		mem_manager.threads[mem_manager.current_priority]->val == NULL ) );
	}
}


void timer_handler(int sig)
{

	DEBUG_PRINT("timer_handler\n")
	update_current_thread();
	if ( 	mem_manager.threads[mem_manager.current_priority] != NULL &&
	  		mem_manager.threads[mem_manager.current_priority]->val != NULL  )
	{
		timer.it_interval.tv_sec = mem_manager.p_quantum_usecs[mem_manager.current_priority];
		mem_manager.current = mem_manager.threads[mem_manager.current_priority];
		
	
		execute( mem_manager.current->val );
	}

	// if (setitimer (ITIMER_VIRTUAL, &timer, NULL)) {
	// 	printf("setitimer error.");
	// }
}

void mem_manager_main() 
{

	DEBUG_PRINT("mem_manager_main\n")

	// Install timer_handler as the signal handler for SIGVTALRM.
	sa.sa_handler = &timer_handler;

	if (sigaction(SIGVTALRM, &sa,NULL) < 0) {
		printf("sigaction error.");
	}

    timer.it_value.tv_sec =  0;		// first time interval, seconds part
	timer.it_value.tv_usec = 1;		// first time interval, microseconds part
	timer.it_interval.tv_sec = 1;	// following time intervals, seconds part
	timer.it_interval.tv_usec = 0;	// following time intervals, microseconds part

	// Start a virtual timer. It counts down whenever this process is executing.
	if (setitimer (ITIMER_VIRTUAL, &timer, NULL)) {
		printf("setitimer error.");
	}
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

    if ( size < 0 | quantum_usecs == NULL | (sizeof(int) * size != sizeof(quantum_usecs)) )
    {
        return CODES.FAILURE;
    }

	for ( int index = 0; index < size; index++)
	{
		if ( quantum_usecs[index] <= 0 )
		{
			return CODES.FAILURE;
		}
	}
    mem_manager.p_quantum_usecs = (int *) malloc( size );
    if ( mem_manager.p_quantum_usecs == NULL)
    {
        DEBUG_PRINT("malloc::p_quantum_usecs\n")
        return CODES.FAILURE;
    }

    mem_manager.size = size;
	mem_manager.runners = 0;
    memcpy(mem_manager.p_quantum_usecs, quantum_usecs, size);
    mem_manager.threads = (list ** ) malloc( sizeof( list **) * size );
    mem_manager.current_priority = 0;

    for (int index = 0; index < size; index++ )
    {
        mem_manager.threads[index] = ( list * ) malloc( sizeof( list ) );
    }

	mem_manager_main(); 
    return CODES.SUCCESS;
}

int nodelist2id( list* p_node )
{
    return (int) (( void * ) p_node - ((void *) mem_manager.threads[0]));
}

list* id2adrress(int tid)
{
    return ( list*) ( ( (void *) mem_manager.threads[0]) + tid);
}

list* id2nodelist(int tid)
{
    list* p_list = id2adrress(tid);

    if ( p_list == NULL || (is_p_uthreads(p_list->val) != 0) )
    {
        return NULL;
    }

    return p_list;
}

int priority_validity(int priority)
{
    if ( priority < 0 || priority >= mem_manager.size)
    {
        return CODES.FAILURE; 
    }
    return CODES.SUCCESS;
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

    list * p_node = push(&mem_manager.threads[priority], warpper); 
    #ifdef DEBUG
        printf( "%d\n" , nodelist2id(p_node ) );
    #endif
    
    DEBUG_PRINT("p_uthreads pushed into list\n")
	mem_manager.runners += 1; 
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
    
    list* p_list = id2nodelist(tid);

    if ( p_list == NULL )
    {
        return CODES.FAILURE;
    }

     // after poping, p_list has changed ( points to the precding node).
    list* orignal_node = pop(&p_list);

    if ( orignal_node == NULL )
    {
        return CODES.FAILURE;
    }

    pushnode(&mem_manager.threads[priority], orignal_node);
    return CODES.SUCCESS;
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
    list* p_list = id2nodelist(tid);
    
    if ( p_list == NULL )
    {
        return CODES.FAILURE;
    }

    list* orignal_node = pop(&p_list);

    if ( orignal_node == NULL )
    {
        return CODES.FAILURE;
    }

    orignal_node->val->signature[0] = '\0';
    free_p_uthreads(orignal_node->val);
    return CODES.SUCCESS;
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

    if ( p_list == NULL )
    {
        return CODES.FAILURE;
    }

    p_list->val->blocked = 1;
    return CODES.SUCCESS;
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

    if ( p_list == NULL )
    {
        return CODES.FAILURE;
    }

    p_list->val->blocked = 0;
    return CODES.SUCCESS;
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

