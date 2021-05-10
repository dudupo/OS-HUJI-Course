#include "Threads.h"
#include <iostream>
#include <list>
#include <utility>
#include <vector>




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

void Thread::run() 
{   

    // int ret = sigsetjmp(this->env[0], 1);
    // if ( ret = 0 )
    // {}
    
    // siglongjmp( this->env[0], 1);
    
    
    // this->f();
}

int Scheduler::uthread_init(int quantum_usecs) 
{

    // handle errors.
    if ( quantum_usecs <= 0 )
    {
        return -1;
    }
    this->quantum_usecs = quantum_usecs;
    return 0;
}

int Scheduler::uthread_spawn(void (*f)(void))
{

    // update the number of initialztions. 
    this->thread_counter++;  

    int tid = 1;    
    for (; tid <  MAX_THREAD_NUM && \
     this->contain( tid ) ; tid++ ) {  }

    // handle error
    if ( tid == MAX_THREAD_NUM )
    {
        return -1;
    }
    this->radyQueue.push_back(new Thread(f, tid));
    this->thread_table.insert({  tid, this->radyQueue.back() });
    return tid;  
}


#define CHECK_EXISTENCE\
    if (!this->contain(tid))\
    {\
        return -1;\
    }


int Scheduler::uthread_terminate( int tid)
{
    // handle error
    CHECK_EXISTENCE
    return 1;
}
int Scheduler::uthread_block(int tid)
{
    // handle error
    CHECK_EXISTENCE

    this->thread_table[ tid ]->state = Thread::BLOCKED; 
    return 1;

}
int Scheduler::uthread_resume(int tid)
{
    // handle error
    CHECK_EXISTENCE

    this->thread_table[ tid ]->state = Thread::READY;
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

    if (sig != -1)
    {
        int ret = sigsetjmp( Scheduler::getInstance().vpointer()->env[0], 1);
        if (ret != 0)
        {
            return;
        }
    }

    sigset_t y  =  { SIGVTALRM, SIGINT };
    sigset_t omask;
    sigprocmask( SIG_UNBLOCK,  &y, &omask);    
    reset_itimer(); 
    siglongjmp(buf, 1);
}

void Scheduler::main()
{   
    // Setting up jumping location.
    if (!sigsetjmp(buf, 1))
    {
        this->pointer = this->radyQueue.begin();
        mainCaller(-1);
    }
    else 
    {

        // Change the status of the last Thread from RUNNING to READY 
        if ( this->vpointer()->state == Thread::RUNNING )
        {
            this->vpointer()->state
             = Thread::READY;
        }

        ++(*this);

        // Advances the queue. 
        for (; \
         this->vpointer()->state != Thread::READY; \
          ++(*this) ) { }
        
        // Executing the function. 
        this->vpointer()->state = Thread::RUNNING;

        vt_alarm(this->quantum_usecs );
        
        this->total_quantums += 1;
        this->vpointer()->total_quantums += 1;
        siglongjmp(this->vpointer()->env[0],1);
    }
    raise(SIGINT);
}

int Scheduler::uthread_mutex_lock()
{   


    int tid = this->uthread_get_tid();
    

    // handle the case where the thread is 
    // alrady locked by this thred. 
    if ( this->mutex == tid )
    {
        return -1;
    }


    /*
        "resignal" the timer.-
         -to proxy which end the mission.  
    */
   
    if ( this->mutex != -1 )
    {
        this->blockedMutexQueue.push_back(this->thread_table[tid]);
        this->thread_table[tid]->state = Thread::BLOCKED;
        raise( SIGINT );
        // while( this->mutex != -1 )
        // {
        // }
    }
    this->mutex = tid;

    
    /*
        "resignal" the timer again.
    */

    return 1;
}
int Scheduler::uthread_mutex_unlock()
{
    // handle an error.
    if ( this->mutex == -1 )
    {
        return -1;
    }

    int tid = this->uthread_get_tid();
    this->mutex = -1;

    // relase a witing thread. 
    if ( this->blockedMutexQueue.size() > 0 )
    {
        this->blockedMutexQueue.back()->state = Thread::READY;
        this->blockedMutexQueue.pop_back();
    }
    
    return 1;
}
int Scheduler::uthread_get_tid()
{
    return Scheduler::getInstance().vpointer()->tid;
}

int Scheduler::uthread_get_total_quantums()
{
    return this->total_quantums;
}

int Scheduler::uthread_get_quantums(int tid)
{   
    CHECK_EXISTENCE
    return this->thread_table[tid]->total_quantums;
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
        for (; i< 100000000; i++)
        {
            
        }
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
        std::cout << "hi3 " << j <<  std::endl;
    } 
}

void test4(int sig)
{
    std::cout << "test4" <<std::endl;
}

int main(int argc, char const *argv[])
{
    
	sa.sa_handler = &mainCaller;
    sa.sa_flags = 0;
    saINT.sa_handler = &mainCaller;

    if (sigaction(SIGINT, &saINT,NULL) < 0) {
		printf("sigaction error.");
	}

    signal(SIGVTALRM, &mainCaller);

    Scheduler::getInstance().uthread_init( 2);
    Scheduler::getInstance().uthread_spawn(  &test);
    Scheduler::getInstance().uthread_spawn(  &test1 );
    Scheduler::getInstance().uthread_spawn(  &test2 );
    Scheduler::getInstance().main();
    return 0;
}
