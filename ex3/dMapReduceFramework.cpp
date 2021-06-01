

#include "MapReduceFramework.h"
#include <pthread.h>
#include <list>
#include <map>
#include <stdlib.h>
#include <functional>
#include "./Barrier/Barrier.h"


// absoulte position 
pthread_t ** threads;

// const MapReduceClient& client

class GlobalEnv {
    public :
    GlobalEnv(const InputVec& inputVec, OutputVec& outputVec, const MapReduceClient& client, int multiThreadLevel) : inputVec(inputVec),
     outputVec(outputVec), client(client), map(), intermediateVecs(), barrier(multiThreadLevel), _size( inputVec.size()) { }
    
    InputVec inputVec;
    OutputVec& outputVec;
    const MapReduceClient& client;
    
    Barrier barrier; 
    
    // absoulte memmory.
    std::vector<IntermediateVec> intermediateVecs; 
    // points on items in intermediateVecs.
    std::map<K2 *, IntermediateVec*> map; 
    
    float _size;

    IntermediateVec * mappop( ) {
        IntermediateVec * ret = this->map.begin()->second;
        this->map.erase(this->map.begin());
        return ret;
    }
};

GlobalEnv * globalEnv; 
pthread_mutex_t mutex;


// should executed only once.
void shuffle()
{

}


void * map_reduce_call (void * args)
{
    void * context = NULL;
    
    // while there are still missions in the queue. 
    while ( globalEnv->inputVec.size() > 0 ){
        // pop one and execute it.
        InputPair pair = globalEnv->inputVec.back();
        globalEnv->inputVec.pop_back();
        globalEnv->client.map(pair.first, pair.second, context);
    }
    
    globalEnv->barrier.barrier();
    
    shuffle();

    globalEnv->barrier.barrier();

    while ( globalEnv->map.size() > 0 ){
        globalEnv->client.reduce(globalEnv->mappop(), context);        
    }
}


JobHandle startMapReduceJob(const MapReduceClient& client,
	const InputVec& inputVec, OutputVec& outputVec,
	int multiThreadLevel) {    
    threads = (pthread_t **) malloc( sizeof(pthread_t *) * multiThreadLevel ); 

    // allocate memmory on the heap.
    for ( int i = 0 ; i < multiThreadLevel; i++ ) {
        threads[i] = (pthread_t *) malloc( sizeof(pthread_t )); 
    }

    globalEnv = new GlobalEnv (inputVec, outputVec, client, multiThreadLevel);

    for ( int i = 0 ; i < multiThreadLevel; i++ ) {
        threads[i] = (pthread_t *) malloc( sizeof(pthread_t )); 
        pthread_create( threads[i], NULL, &map_reduce_call, globalEnv );
    }

    return NULL;
}


void waitForJob(JobHandle job) {

}
void getJobState(JobHandle job, JobState* state) {
    state->percentage = 100 * (1 - float( globalEnv->inputVec.size()/ globalEnv->_size));
    
}
void closeJobHandle(JobHandle job) { 
 
}
