

#include "MapReduceFramework.h"
#include <pthread.h>
#include <list>
#include <map>
#include <stdlib.h>
#include <functional>
#include "./Barrier/Barrier.h"

#include <string>
#include <iostream>
#define DEBUG

inline void log( char * str )
{
    #ifdef DEBUG
    std::cout << "[" << __FUNCTION__ <<  "]" << "-";
    std::cout << str << "\n";
    #endif
}


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
    stage_t stage = UNDEFINED_STAGE;
    
    // absoulte memmory.
    std::vector<IntermediateVec*> intermediateVecs; 


    // points on items in intermediateVecs.
    struct cmpByStringLength {
        bool operator()(const K2* a, const K2* b) const {
            return *a < *b;
        }
    };

    std::map<K2* , IntermediateVec*, cmpByStringLength > map; 
    
    float _size;

    IntermediateVec * mappop( ) {
        IntermediateVec * ret = this->map.begin()->second;
        this->map.erase(this->map.begin());
        return ret;
    }

    void emit2 (K2* key, V2* value) {
        if ( this->map.find(key) == this->map.end() )
        { 
            this->intermediateVecs.push_back( new IntermediateVec() );
            this->map[key] = this->intermediateVecs.back();
        }
        this->map[key]->push_back( IntermediatePair( key, value));
        
        log( "[emit2] i was here" );
        // std::cout
    }

    void emit3 (K3* key, V3* value) {
        this->outputVec.push_back( OutputPair( key, value ) );
        log( "[emit3] i was here" );
    }

};

GlobalEnv * globalEnv; 
pthread_mutex_t mutex;


// should executed only once.
void shuffle()
{
    globalEnv->_size = globalEnv->map.size();
}


void * map_reduce_call (void * args)
{
    void * context = NULL;
    
    globalEnv->stage=MAP_STAGE;

    // while there are still missions in the queue. 
    while ( globalEnv->inputVec.size() > 0 ){
        // pop one and execute it.
        InputPair pair = globalEnv->inputVec.back();
        globalEnv->inputVec.pop_back();
        globalEnv->client.map(pair.first, pair.second, context);
    }
    
    globalEnv->barrier.barrier();
    globalEnv->stage=SHUFFLE_STAGE;
    shuffle();
    globalEnv->stage=REDUCE_STAGE;
    globalEnv->barrier.barrier();
    std::cout <<  globalEnv->map.size() << "\n";
    while ( globalEnv->map.size() > 0 ){
        globalEnv->client.reduce(globalEnv->mappop(), context);        
    }

    for ( auto pair : globalEnv->outputVec)
    {
        std::cout<< pair.first << "\n";
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

    float p = 0;
    switch (globalEnv->stage)
    {
    case UNDEFINED_STAGE:
        p = 0;
        break;
    case MAP_STAGE:
        p = float( globalEnv->inputVec.size()/ globalEnv->_size);
        break;
    case SHUFFLE_STAGE:
        p = 0.5;
        break;
    case REDUCE_STAGE:
        p = float( globalEnv->map.size()/ globalEnv->_size);
        break;

    default:
        break;
    }
    state->percentage = 100 * (1 -p );
    state->stage = globalEnv->stage;
}
void closeJobHandle(JobHandle job) { 
 
}



void emit2 (K2* key, V2* value, void* context) {
    globalEnv->emit2( key, value );
}
void emit3 (K3* key, V3* value, void* context) {
    globalEnv->emit3(key, value);
}
