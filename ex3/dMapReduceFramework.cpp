

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

#ifdef DEBUG
#define log(str)\
    std::cout << "[" << __FUNCTION__ <<  "]" << "-";\
    std::cout << str << "\n";
#else
    
#endif


void * map_reduce_call (void * context);

// const MapReduceClient& client

class GlobalEnv {
    public :
    GlobalEnv(const InputVec& inputVec, OutputVec& outputVec,
     const MapReduceClient& client, int multiThreadLevel) : inputVec(inputVec),
      outputVec(outputVec), client(client), map(),
       intermediateVecs(), barrier(multiThreadLevel), _size( inputVec.size()),
        mutex(PTHREAD_MUTEX_INITIALIZER) { 

        // absoulte position 
        threads = (pthread_t **) malloc( sizeof(pthread_t *) * multiThreadLevel ); 

        // allocate memmory on the heap.
        for ( int i = 0 ; i < multiThreadLevel; i++ ) {
            threads[i] = (pthread_t *) malloc( sizeof(pthread_t )); 
        }

        for ( int i = 0 ; i < multiThreadLevel; i++ ) {
            threads[i] = (pthread_t *) malloc( sizeof(pthread_t )); 
            pthread_create( threads[i], NULL, &map_reduce_call, this );
        }

    }
    
    InputVec inputVec;
    OutputVec& outputVec;
    const MapReduceClient& client;
    Barrier barrier; 
    stage_t stage = UNDEFINED_STAGE;
    
    pthread_t ** threads;
    pthread_mutex_t mutex;

    // absoulte memmory.
    std::vector<IntermediateVec*> intermediateVecs; 
    
    // points on items in intermediateVecs.
    struct cmpK2 {
        bool operator()(const K2* a, const K2* b) const {
            return *a < *b;
        }
    };

    std::map<K2* , IntermediateVec*, cmpK2> map; 
    
    float _size;
    
    void inline lock()
    {
        log("")
        if (pthread_mutex_lock(&mutex) != 0){
		    fprintf(stderr, "[[Barrier]] error on pthread_mutex_lock");
		    exit(1);
	    }
    }

    void inline unlock()
    {
        log("")
        if (pthread_mutex_unlock(&mutex) != 0) {
		    fprintf(stderr, "[[Barrier]] error on pthread_mutex_unlock");
		    exit(1);
	    }
    }

    IntermediateVec * mappop( ) {

        lock();
        IntermediateVec * ret = this->map.begin()->second;
        this->map.erase(this->map.begin());
        unlock();
        return ret;
    }

    void emit2 (K2* key, V2* value) {
        lock();
        if ( this->map.find(key) == this->map.end() )
        { 
            this->intermediateVecs.push_back( new IntermediateVec() );
            this->map[key] = this->intermediateVecs.back();
        }
        this->map[key]->push_back( IntermediatePair( key, value));
        
        log("");
        unlock();
    }

    void emit3 (K3* key, V3* value) {
        lock();
        this->outputVec.push_back( OutputPair( key, value ) );
        log("");
        unlock();
    }

};

GlobalEnv * globalEnv; 
pthread_mutex_t mutex;


// should executed only once.
void shuffle()
{
    log("");
    globalEnv->_size = globalEnv->map.size();
}


void * map_reduce_call (void * context)
{
    
    GlobalEnv * _globalEnv = (GlobalEnv *) context;
    _globalEnv->stage=MAP_STAGE;
    _globalEnv->lock();

    // while there are still missions in the queue. 
    while ( _globalEnv->inputVec.size() > 0 ){
        // pop one and execute it.
        InputPair pair = _globalEnv->inputVec.back();
        _globalEnv->inputVec.pop_back();
        _globalEnv->unlock();
        
        _globalEnv->client.map(pair.first, pair.second, context);
        _globalEnv->lock();
    }
    _globalEnv->unlock();
    
    _globalEnv->barrier.barrier();    
    _globalEnv->stage=SHUFFLE_STAGE;
    shuffle();
    _globalEnv->stage=REDUCE_STAGE;
    _globalEnv->barrier.barrier();
    
    _globalEnv->lock();
    while ( _globalEnv->map.size() > 0 ){
        _globalEnv->unlock();
        _globalEnv->client.reduce(_globalEnv->mappop(), context);        
        _globalEnv->lock();
    }
    _globalEnv->unlock();
}


JobHandle startMapReduceJob(const MapReduceClient& client,
	const InputVec& inputVec, OutputVec& outputVec,
	int multiThreadLevel) {    
    globalEnv = new GlobalEnv (inputVec, outputVec, client, multiThreadLevel);
    
    

    return (void *) globalEnv;
}


void waitForJob(JobHandle job) {

}
void getJobState(JobHandle job, JobState* state) {

    ((GlobalEnv *) job)->lock();

    float p = 0;
    switch (((GlobalEnv *) job)->stage)
    {
    case UNDEFINED_STAGE:
        p = 0;
        break;
    case MAP_STAGE:
        p = float( ((GlobalEnv *) job)->inputVec.size()/ ((GlobalEnv *) job)->_size);
        break;
    case SHUFFLE_STAGE:
        p = 0.5;
        break;
    case REDUCE_STAGE:
        p = float( ((GlobalEnv *) job)->map.size()/ ((GlobalEnv *) job)->_size);
        break;

    default:
        break;
    }
    state->percentage = 100 * (1 -p );
    state->stage = ((GlobalEnv *) job)->stage;
    ((GlobalEnv *) job)->unlock();
}
void closeJobHandle(JobHandle job) { 
 
}


void emit2 (K2* key, V2* value, void* context) {
    ((GlobalEnv *) context)->emit2( key, value );
}
void emit3 (K3* key, V3* value, void* context) {
    ((GlobalEnv *) context)->emit3(key, value);
}
