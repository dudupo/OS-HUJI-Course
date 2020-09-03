#include "p_uthreads.h"
#include "stdlib.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>

#define STACK_SIZE 4096

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
    asm volatile(
        "xor    %%fs:0x30,%0\n"
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
    asm volatile(
        "xor    %%gs:0x18,%0\n"
		"rol    $0x9,%0\n"
                 : "=g" (ret)
                 : "0" (addr));
    return ret;
}

#endif

struct pointer_uthreads {
    void (*func) (void);
    char stack[STACK_SIZE];
    sigjmp_buf env;
};

p_uthreads * init_p_uthreads( void (*func) (void)  )
{
    p_uthreads * p_obj = malloc( sizeof( p_uthreads ) );

    if ( p_obj == NULL )
    {
        // todo
    }

    p_obj->func = func;
    address_t sp, pc;

    sp = (address_t)p_obj->stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)p_obj->func;
    
    sigsetjmp(p_obj->env, 1);
    (p_obj->env->__jmpbuf)[JB_SP] = translate_address(sp);
    (p_obj->env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&(p_obj->env->__saved_mask));     
    
    return p_obj;
}


int gotit = 0;

void timer_handler(int sig)
{
  gotit = 1;
  printf("Timer expired\n");
}


void execute(p_uthreads * p_obj)
{
    // loading the env
    siglongjmp(p_obj->env,1);
    int ret_val = sigsetjmp(p_obj->env, 1);
    printf("SWITCH: ret_val=%d\n", ret_val); 
    if (ret_val == 1) {
        return;
    }
}




