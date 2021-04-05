#include "stdio.h"
#include "uthreads.h"
#include <unistd.h>
#include <sys/time.h>

/*

    heartbeat tests :


*/
#define SECOND 1

#define TEST( code ) \
    if (code) {\
        printf( "[v] " #code " pass \n" );\ 
    }\
    else {\
        printf( "[x] " #code " fail \n");\
    }



int test_uthread_init( )
{
    int A[][3] = {
                    { 1, 2, 5 },
                    { 1, 8, 0 },
                    { 1, -8,0 },
                    { 1, 8, -3},
                    { 1, -8, 3},
                    {1 , 2, 0}
                };
    // fail
    TEST( uthread_init( A[0], 3 ) == 0 ) 
    
    TEST( uthread_init( A[1], 2 ) == 0 ) // pass
    TEST( uthread_init( A[1], -1 ) == -1 ) // pass
    TEST( uthread_init( A[2], 2 ) == -1 ) // pass
    TEST( uthread_init( A[3], 3 ) == -1 ) // pass
    TEST( uthread_init( A[4], 3 ) == -1 ) // pass
    TEST( uthread_init( NULL, 3 ) == -1 ) // pass

    // int B[] = { 1,2};
    // TEST( uthread_init(B, 3) == -1 ) //pass
    TEST( uthread_init( A[0], 3 ) == 0 ) 

    return 1;
}

void f_test_uthread_spawn()
{
    // char space[1000]; // Reserve enough space for main to run
    // space[999] = 1;  
    printf("f_test_uthread_spawn\n");
    for(int j =0 ; j < 4; j++)
    {
        printf("f_test_uthread_spawnn\n");
        
        // usleep(SECOND);

    }
}

int test_uthread_spawn ()
{
    int A[] = { 1, 2, 5 };
    uthread_init(A, 3);

    TEST( uthread_spawn( &f_test_uthread_spawn, 1) != -1 ) //pass 
    TEST( uthread_spawn( &f_test_uthread_spawn, 3) == -1 ) //pass 
    
    for (int i = 0; i <= MAX_THREAD_NUM; i++)
    {
        uthread_spawn( &f_test_uthread_spawn, 1);
    }

    TEST( uthread_spawn( &f_test_uthread_spawn, 1) == -1 ) // pass
    
}

void f_test_uthread_change_priority()
{
    printf("f_test_uthread_change_priority");
} 

int test_uthread_change_priority()
{
    int A[] = { 1, 2, 5 };
    uthread_init(A , 3);
    int tid = uthread_spawn(&f_test_uthread_change_priority, 1);
    TEST( uthread_change_priority(tid, 1) == 0 ) //fail
    TEST( uthread_change_priority(tid, 2) == 0 ) //fail
    TEST( uthread_change_priority(tid, 2) == 0 ) //fail
    TEST( uthread_change_priority(tid, 3) == -1 )
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
    int A[] = { 1, 2, 5 };
    uthread_init(A , 3);
    int tid = uthread_spawn(&f_test_uthread_terminate, 2);
    // terminate an unexsit process
    TEST( uthread_terminate( 4 ) == -1 )
    TEST( uthread_terminate( tid ) == 0 )

    tid = uthread_spawn(&f_test_uthread_terminate, 2);
    // changing priority and then terminate
    TEST( uthread_change_priority(tid, 1) == 0 )     
    TEST( uthread_terminate( 4 ) == -1 )
    TEST( uthread_terminate( tid ) == 0 )

    // multiple process, 
    tid = uthread_spawn(&f_test_uthread_terminate, 2);
    int tid2 = uthread_spawn(&f_test_uthread_terminate, 1);
    int tid3 = uthread_spawn(&f_test_uthread_terminate, 1);
    TEST( uthread_terminate( tid2 ) == 0 )
    TEST( uthread_terminate( tid3 ) == 0 )
    TEST( uthread_terminate( tid ) == 0 )

    // todo add test for terminate the main thread.

}

void f_test_uthread_block()
{
    printf("f_test_uthread_block");
}

int test_uthread_block()
{
    int A[] = { 1, 2, 5 };
    uthread_init(A , 3);
    int tid = uthread_spawn(&f_test_uthread_block, 2);
    
    TEST( uthread_block( 8 ) == -1 )
    TEST( uthread_block( tid ) == 0 )
    TEST( uthread_block( tid ) == 0 )

    int tid2 = uthread_spawn(&f_test_uthread_block, 2);
    TEST( uthread_terminate( tid2 ) == 0 )
    TEST( uthread_block( tid2 ) == -1 )
    
    // blocking main thread 
    TEST( uthread_block( 0 ) == -1 )
}

void f_test_uthread_resume()
{
    printf("f_test_uthread_resume");
}

int test_uthread_resume()
{
    int A[] = { 1, 2, 5 };
    uthread_init(A , 3);
    int tid = uthread_spawn(&f_test_uthread_resume, 2);
    TEST( uthread_resume(8) == -1 )
    TEST( uthread_resume(tid) == 0 )
    TEST( uthread_block( tid ) == 0 )
    TEST( uthread_resume(tid) == 0 )
    
}

void f_test_uthread_get_tid()
{
    printf("f_test_uthread_get_tid");
}

int test_uthread_get_tid()
{
    int A[] = { 1, 2, 5 };
    uthread_init(A , 3);
    int tid = uthread_spawn(&f_test_uthread_get_tid, 2);
    printf(" WARRING NO TESTS FOR get_tid");
}

void f_test_uthread_get_total_quantums()
{
    printf("f_test_uthread_get_total_quantums");
}

int test_uthread_get_total_quantums()
{
    int A[] = { 1, 2, 5 };
    uthread_init(A , 3);
    TEST( uthread_get_total_quantums() == 1 )
    int tid = uthread_spawn(&f_test_uthread_get_total_quantums, 2);
    TEST( uthread_get_total_quantums() == 2 )
    int tid2 = uthread_spawn(&f_test_uthread_get_total_quantums, 2);
    TEST( uthread_get_total_quantums() == 3 )
    TEST( uthread_block( tid ) == 0 )
    TEST( uthread_get_total_quantums() == 3 )
}

void f_test_uthread_get_quantums()
{
    printf("f_test_uthread_get_quantums");
}

int test_uthread_get_quantums()
{
    int A[] = { 1, 2, 5 };
    uthread_init(A , 3);
    TEST( uthread_get_quantums(0) == 1 )
    int tid = uthread_spawn(&f_test_uthread_get_quantums, 2);
    TEST( uthread_get_quantums(tid) == 1 )

}

int test_terminaion_with( )
{
    int A[][3] = {
                    { 1, 2, 5 },
                    { 1, 8, 0 },
                    { 1, -8,0 },
                    { 1, 8, -3},
                    { 1, -8, 3},
                    {1 , 2, 0}
                };
    TEST( uthread_init( A[0], 3 ) == 0 ) 
    TEST( uthread_spawn( &f_test_uthread_spawn, 1) != -1 ) 
    TEST( uthread_spawn( &f_test_uthread_spawn, 1) != -1 ) 
    printf("%d\n" ,  uthread_spawn( &f_test_uthread_spawn, 1));
    // printf("%d\n" ,  uthread_spawn( &f_test_uthread_spawn, 1));
    // printf("%d\n" ,  uthread_spawn( &f_test_uthread_spawn, 1));
    // printf("%d\n" ,  uthread_spawn( &f_test_uthread_spawn, 1));

    return 1;
}

int main(void)
{
    test_terminaion_with();


    // test_uthread_init();
    // test_uthread_spawn();
    // test_uthread_change_priority();
    // test_uthread_terminate();
    // test_uthread_block();
    // test_uthread_resume();
    // test_uthread_get_tid();
    // test_uthread_get_total_quantums();
    // test_uthread_get_quantums();

    // uthread_terminate(0);

    for (;;)
    {

    }
    return 0;
}
