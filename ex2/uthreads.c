#include <stdio.h>
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


// char mainstack[STACK_SIZE*10];

struct {

    int size;
    int * p_quantum_usecs;
    list ** threads;
    list* current; 
    int current_priority;
    int runnerid;
	int runners;
    int totalprocsses;
    p_uthreads * main;
    
} mem_manager;

struct itimerval timer;


struct {
    const int SUCCESS ;
    const int FAILURE ;
} CODES = { 0 , -1};



int gotit = 0;

void timer_handler(int);


struct itimerval timerr;
void execute(int sig)
{
}
void mem_manager_main(int sig);
int lunch_timer(  void (*_handler) (int), size_t tv_sec, size_t tv_usec );

sigjmp_buf * get_current_env()
{
    return mem_manager.current->val->env;
}

int current_is_blocked()
{
    return mem_manager.current->val->blocked;
}
// struct sigaction saa= { &execsrart_current_taskute };


int perform_step( )
{
    if ( mem_manager.current != NULL &&
            mem_manager.current->val != NULL &&
                mem_manager.current->next != NULL ) 
    {
        mem_manager.current =  mem_manager.current->next;        
    }

    else 
    {
        mem_manager.current_priority += 1;
        mem_manager.current_priority %= mem_manager.size;
        mem_manager.current = mem_manager.threads[mem_manager.current_priority];
        // mem_manager.current = mem_manager.threads[0];
    }
    return mem_manager.current != NULL && mem_manager.current->val != NULL;
}

void timer_handler(int sig)
{
    
}

void stop_task(int sig)
{

    printf("stop_task\n");
    if ( mem_manager.current != NULL &&
        mem_manager.current->val != NULL ) 
    {

        int ret_val = sigsetjmp( *get_current_env(), 1);
        mem_manager.current->val->blocked = 1;
        printf("SWITCH: ret_val=%d\n", ret_val); 
        if (ret_val == 1) 
        {
            DEBUG_PRINT("switch_current_task, task has been fail\n")    
        }

    }
}

int uthread_get_tid();
sigset_t set;

void _blockAlarm() {
    if (sigemptyset(&set) < 0) {
        // print_error(SYSTEM_ERROR, "sigemptyset error");
    }
    if (sigaddset(&set, SIGVTALRM) < 0) {
        // print_error(SYSTEM_ERROR, "sigaddset error");
    }
    if (sigprocmask(SIG_BLOCK, &set, NULL) < 0) {
        // print_error(SYSTEM_ERROR, "sigprocmask error");
    }
}

/**
 * block SIGVTALRM
 */
void _unblockAlarm() {
    if (sigemptyset(&set) < 0) {
        // print_error(SYSTEM_ERROR, "sigemptyset error");
    }
    if (sigaddset(&set, SIGVTALRM) < 0) {
        // print_error(SYSTEM_ERROR, "sigaddset error");
    }
    if (sigprocmask(SIG_UNBLOCK, &set, NULL) < 0) {
        // print_error(SYSTEM_ERROR, "sigprocmask error");
    }
}


void srart_current_task()
{

    _blockAlarm();

    printf("\nblblblbblb\n");
    if ( mem_manager.current != NULL &&
        mem_manager.current->val != NULL ) 
    {
        printf("\naaaaaa\n");
        printf("\n %d \n" ,uthread_get_tid());
 
        // if ( current_is_blocked())
        // {
        mem_manager.current->val->blocked = 0;
        int ret_val = sigsetjmp(*get_current_env(), 1);
        if (ret_val == 1) {
            siglongjmp(*get_current_env(),1);
        }
        
        printf("\ncccccc\n");

        // }
    }

    _unblockAlarm();
}

void station ( int sig );

#define SYSTEM_ERROR 1

void mem_manager_main_void(void)
{
    mem_manager_main(0);
}


static jmp_buf mainbuf;
void mem_manager_main(int sig) 
{  
    _blockAlarm();
    // int ret_val = sigsetjmp( *get_current_env(), 1);
    DEBUG_PRINT("mem_manager_main\n")
    lunch_timer(station, 1, 400);    

    
    // while ( mem_manager.current != NULL &&
    //     mem_manager.current->val != NULL )
    // {
    //     perform_step();
    // }
    
    if ( mem_manager.current != NULL &&
        mem_manager.current->val != NULL ) 
    {   
        printf("\n %d \n" ,uthread_get_tid());
        mem_manager.current->val->blocked = 0;
        // sig_t prev_sigint_handler1 = signal(SIGHUP, catch1);
        int ret_val = setjmp(mainbuf);
        if (!ret_val)
            longjmp(mem_manager.current->val->env, 1);

        // if (ret_val != 1) 
        // {
        //     // int flag = perform_step();
        //     // lunch_timer(station, 1, 400);    
        //     // if(flag)
        //     siglongjmp(mem_manager.current->val->env,1);
        // }
        // else
        // {
        //     DEBUG_PRINT("srart_current_task\n")
        //     perform_step();
        //     lunch_timer(station, 1, 400);    
        // }
    }
    else 
    {
        perform_step();
        lunch_timer(station, 1, 400);    
    }
    _unblockAlarm();     
} 

void disable_maintimer()
{

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

    if ( p_list == NULL ) // is_p_uthreads 
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


void free_entire_list( list * root)
{
    if ( root->next != NULL )
    {
        free_entire_list( root->next );
    }
    free_p_uthreads( root->val );
}

void station (  int sig )
{
    printf("_sigaction\n");
    lunch_timer(&mem_manager_main, 0, 100);
}


//struct sigaction _sigaction = {0}; // { 1, mem_manager_main };
//struct itimerval _itimerval;
// _sigaction.sa_handler = mem_manager_main;

struct sigaction * _sigaction;
struct itimerval * _itimerval;
int lunch_timer(  void (*_handler)(int sig), size_t tv_sec, size_t tv_usec ) 
{

    printf("lunch_timer\n");
    // free(_sigaction);
    // free(_itimerval);
    _sigaction = malloc(sizeof(struct sigaction));  // { 1, mem_manager_main };
    _itimerval = malloc(sizeof(struct itimerval));
	_sigaction->sa_handler = _handler;
    if (sigaction(SIGVTALRM, _sigaction,NULL) < 0)
    {
		printf("sigaction error.");
	}
	_itimerval->it_value.tv_sec = tv_sec;
	_itimerval->it_value.tv_usec =  tv_usec;		
	_itimerval->it_interval.tv_sec = 0;	
	_itimerval->it_interval.tv_usec = 0;	
	if (setitimer (ITIMER_VIRTUAL, _itimerval, NULL)) 
    {
		printf("setitimer error.");
	}
    
    return 0;
    
}

// ------------------------- API  ---------------------------- // 

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
    // (sizeof(int) * size != sizeof(quantum_usecs))
    if ( size < 0 | quantum_usecs == NULL )
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
	mem_manager.runners = 1;
    mem_manager.totalprocsses = 1;
    memcpy(mem_manager.p_quantum_usecs, quantum_usecs, size);
    mem_manager.threads = (list ** ) malloc( sizeof( list **) * size );
    mem_manager.current_priority = 0;

    for (int index = 0; index < size; index++ )
    {
        mem_manager.threads[index] = ( list * ) malloc( sizeof( list ) );
    }

    mem_manager.main = init_p_uthreads( mem_manager_main_void, 0, 0);

	lunch_timer(&mem_manager_main, 0, 100); 
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
        DEBUG_PRINT("attemption to initalize unrecognaized prtorty type\n")
        return CODES.FAILURE; 
    }

    if ( mem_manager.runners >= MAX_THREAD_NUM )
    {
        DEBUG_PRINT("the number of conccurrent will exceed the limit\n")
        return CODES.FAILURE;
    }
    
    p_uthreads * warpper = init_p_uthreads(f, priority, mem_manager.totalprocsses);
    list * p_node = push(&mem_manager.threads[priority], warpper); 


    // DEBUG_PRINT("p_uthreads initilaized: ")
    // #ifdef DEBUG
    //     printf( "%d\n" , nodelist2id(p_node ) );
    // #endif
    // DEBUG_PRINT("p_uthreads pushed into list\n")
	mem_manager.totalprocsses += 1;
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
        DEBUG_PRINT("attemption to initalize unrecognaized prtorty type\n")
        return CODES.FAILURE; 
    }
    
    list* p_list = id2nodelist(tid);

    if ( p_list == NULL )
    {
        DEBUG_PRINT("the head of the process list is empty\n")
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


    if ( tid == 0 )
    {
        for (int index = 0; index < mem_manager.size; index++ )
        {
            // free_entire_list( mem_manager.threads[index] );
            // free( mem_manager.threads[index]);
        }        
        // free( mem_manager.threads );
        disable_maintimer();
        printf("end\n");
        exit(0);
    }


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
    if (orignal_node->val->blocked == 0)
    {
        mem_manager.runners -= 1; 
    }
    mem_manager.totalprocsses -= 1;
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
    mem_manager.runners -= 1;
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
    return mem_manager.totalprocsses;
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
    list* p_list = id2nodelist(tid);
    return p_list->val->times_was_in_running_state;
}

