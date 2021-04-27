
#ifndef THREAD 

#define THREAD

#include <vector>
#include <list>
#include <utility>

class Thread {
    
    
    public:
        static int const READY = 1;
        static int const RUNNING = 2;
        static int const BLOCKED = 3;
        Thread ( void(*f)(void) ) : state(READY), f(f) { };

        void run() { this->f(); }
    
    private:
        int state;
        void (*f) (void);
};

class Scheduler {
    
    public:
        static Scheduler& getInstance()
        {
            static Scheduler instance;
            return instance;
        }
        int uthread_init(int *quantum_usecs, int size);
        int uthread_spawn(void (*f)(void), int priority);
        void main();
    private:
        Scheduler() {}
        int contextSwitch( Thread enteringThread, Thread oldTread);
        std::vector<int> * quantum_usecs;
        std::vector<std::pair<Thread *, int>> radyQueue ; 

};

void mainScheduler( int sig );




#endif