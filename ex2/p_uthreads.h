#ifndef OS_P_UTHREADS
#define OS_P_UTHREADS

#include <setjmp.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>


extern const char POINTER_UTHREADS_SIGNATURE[17];

struct pointer_uthreads {
    char signature[17];
    void (*func) (void);
    char * stack; 
    sigjmp_buf env;
    int priority;
    int blocked;

};

typedef struct pointer_uthreads p_uthreads;

p_uthreads * init_p_uthreads( void (*func) (void) ,int);
void execute(p_uthreads * );
int is_p_uthreads(p_uthreads * );

#endif 