#ifndef _OSM_H
#include "osm.h"
#define _OSM_H
#endif
#include "sys/time.h"

/* calling a system call that does nothing */
#define OSM_NULLSYSCALL asm volatile( "int $0x80 " : : \
        "a" (0xffffffff) /* no such syscall */, "b" (0), "c" (0), "d" (0) /*:\
        "eax", "ebx", "ecx", "edx"*/)



#define MEASURING( Code )                          \
    struct timeval t1, t2;                          \                                           
    double elapsedTime;                             \                                  
    gettimeofday(&t1, nullptr);                        \                                   
    for ( int i = 0; i < iterations; i++)  \                                    
    {                                               \
        Code                                        \
    }                                               \
    gettimeofday(&t2, nullptr);                        \                   
    elapsedTime = (t2.tv_sec - t1.tv_sec) * 1000.0; \                                                
    return (elapsedTime + (t2.tv_usec - t1.tv_usec)) / iterations;                                     



/* Time measurement function for a simple arithmetic operation.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_operation_time(unsigned int iterations)
{
    // credit for stackoverflow. 
    
    int a = 100;
    int b = 243;
    MEASURING( a + b; )
}

void empty_function_call()
{

}


/* Time measurement function for an empty function call.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_function_time(unsigned int iterations)
{
    MEASURING(empty_function_call();)    
}
/* Time measurement function for an empty trap into the operating system.
   returns time in nano-seconds upon success,
   and -1 upon failure.
   */
double osm_syscall_time(unsigned int iterations)
{
    MEASURING(OSM_NULLSYSCALL; )
}

#include <stdlib.h>
#include <iostream>
int main( )
{
    std::cout << osm_function_time( 10000000 ) << "\n";
    return 0;
}