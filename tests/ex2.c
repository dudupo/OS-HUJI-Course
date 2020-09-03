#include "stdio.h"
#include "uthreads.h"

/*

    heartbeat tests :


*/

#define TEST( code ) \
    if (code) {\
        printf( "[v] " #code " pass \n" );\ 
    }\
    else {\
        printf( "[x] " #code " fail \n");\
    }


int test_uthread_init( )
{
    TEST( uthread_init( (int *) { 1, 2, 5 }, 3) == 0 ) 
    TEST( uthread_init( (int *) { 1, 8 }, 2) == 0 )
    TEST( uthread_init( (int *) { 1, 8 }, -1) == -1 )
    TEST( uthread_init( (int *) { 1, -8 }, 2) == -1 )
    TEST( uthread_init( (int *) { 1, 8, -3}, 3) == -1 )
    TEST( uthread_init( (int *) { 1, -8, 3}, 3) == -1 )
    TEST( uthread_init( (int *) NULL, 3) == -1 )
    TEST( uthread_init( (int *) {1 , 2}, 3) == -1 )

    return 1;
}

void f_test_uthread_spawn()
{
    printf("f_test_uthread_spawn");
}

int test_uthread_spawn ()
{
    uthread_init( (int *) { 1, 2, 5 }, 3);

    TEST( uthread_spawn( &f_test_uthread_spawn, 1) == 1 )
    TEST( uthread_spawn( &f_test_uthread_spawn, 3) == 2 )
    
    for (int i = 0; i < MAX_THREAD_NUM; i++)
    {
        uthread_spawn( &f_test_uthread_spawn, 1);
    }

    TEST( uthread_spawn( &f_test_uthread_spawn, 1) == -1 )
    
}

void f_test_uthread_change_priority()
{
    printf("f_test_uthread_change_priority");
} 

int test_uthread_change_priority()
{
    uthread_init( (int *) { 1, 2, 5 }, 3);
    int tid = uthread_spawn(&f_test_uthread_change_priority, 2);
    TEST( uthread_change_priority(tid, 1) == 0 ) 
    TEST( uthread_change_priority(tid, 2) == 0 ) 
    TEST( uthread_change_priority(tid, 2) == 0 )
    TEST( uthread_change_priority(tid, 3) == 0 )
    TEST( uthread_change_priority(tid, 4) == -1 )

    // passing id of thread which not exist. 
    TEST( uthread_change_priority(tid+1, 2) == -1 )  
 
    TEST( uthread_change_priority(tid, -1) == -1 )
    
    TEST( uthread_change_priority(MAX_THREAD_NUM+1, 2) == -1 )

}

void f_test_uthread_terminate()
{
    printf( "f_test_uthread_terminate");
}

int test_uthread_terminate()
{

}

int test_uthread_block()
{

}

int test_uthread_resume()
{

}

int test_uthread_get_tid()
{

}

int test_uthread_get_total_quantums()
{

}

int test_uthread_get_quantums()
{

}
int main(void)
{
    test_uthread_init();
    test_uthread_spawn();
    test_uthread_change_priority();
    test_uthread_terminate();
    test_uthread_block();
    test_uthread_resume();
    test_uthread_get_tid();
    test_uthread_get_total_quantums();
    test_uthread_get_quantums();

    return 0;
}
