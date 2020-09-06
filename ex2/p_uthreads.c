#include "p_uthreads.h"
#include "stdlib.h"
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <wait.h>
#include "string.h"

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

const char POINTER_UTHREADS_SIGNATURE[17] = "pointer_uthreads\0";


p_uthreads * init_p_uthreads( void (*func) (void), int priority)
{
    p_uthreads * p_obj = malloc( sizeof( p_uthreads ) );

    if ( p_obj == NULL )
    {
        // todo
    }

    memcpy(p_obj->signature, POINTER_UTHREADS_SIGNATURE, sizeof(char) * 17);

    p_obj->stack = malloc ( STACK_SIZE * sizeof(char));
    p_obj->func = func;
    p_obj->priority = priority; 
    p_obj->blocked = 0;
    address_t sp, pc;

    sp = (address_t)p_obj->stack + STACK_SIZE - sizeof(address_t);
    pc = (address_t)p_obj->func;
    
    sigsetjmp(p_obj->env, 1);
    (p_obj->env->__jmpbuf)[JB_SP] = translate_address(sp);
    (p_obj->env->__jmpbuf)[JB_PC] = translate_address(pc);
    sigemptyset(&(p_obj->env->__saved_mask));     
    
    return p_obj;
}





void execute(p_uthreads * p_obj)
{
    // loading the env

    printf( "execute\n" );

    if ( p_obj->blocked == 1)
        return;

    siglongjmp(p_obj->env,1);
    int ret_val = sigsetjmp(p_obj->env, 1);
    printf("SWITCH: ret_val=%d\n", ret_val); 
    if (ret_val == 1) {
        return;
    }
}


void catch_int(int sigNum) 
{
	// Install catch_int as the signal handler for SIGINT.
	printf(" Cannot access memory \n");
    exit(1);
}


int strcmp_eq(char * str1, const char * str2)
{

    struct sigaction sa;
    sa.sa_handler = &catch_int;

    int ret;

    if (sigaction(SIGSEGV, &sa, NULL) < 0) 
    {
		printf("sigaction error.");
        return 1;
	}

    int pid = fork();

    if ( pid == 0 ) 
    {
        for(int index = 0 ;  str2[index]  != '\0' ; index++ )
        {
            if ( (str1 == NULL) || (*str1 != str2[index]) )
            {
                exit(1);
            }
            str1++;
        }
        exit(0);
    }
    else
    {
        waitpid(pid, &ret, WUNTRACED | WCONTINUED);     
        if (sigaction(SIGSEGV, SIG_DFL, NULL) < 0) 
        {
            printf("sigaction error.");
            return 1;
        }
        printf("exited, status=%d\n", WEXITSTATUS(ret));
        return WEXITSTATUS(ret);
    }
}

int is_p_uthreads(p_uthreads * p_obj)
{
    int ret = strcmp_eq(p_obj->signature, POINTER_UTHREADS_SIGNATURE);
    return ret;
}

void free_p_uthreads(p_uthreads * p_obj)
{
    free( p_obj->stack);
    free( p_obj );
}

