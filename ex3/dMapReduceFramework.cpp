

#include "MapReduceFramework.h"
#include <pthread.h>
#include <list>
#include <map>
#include <stdlib.h>
#include <functional>
#include "./Barrier/Barrier.h"

#include <string>
#include <iostream>
#include <atomic>

#define DEBUG

#ifdef DEBUG
#define log(str)\
    std::cout << "[" << __FUNCTION__ <<  "]" << "-";\
    std::cout << str << " ";                       \
    std::cout << pthread_self() << "\n";
#else
    
#endif
class GlobalEnv;
class Context {
public:
    Context(int id, GlobalEnv* env ): threadID(id), env(env){}
    int threadID;
    GlobalEnv* env;
    IntermediateVec interVec;
};

void * map_reduce_call (void * context);

// const MapReduceClient& client

class GlobalEnv {
    public :
    GlobalEnv(const InputVec& inputVec, OutputVec& outputVec,
     const MapReduceClient& client, int multiThreadLevel) : inputVec(inputVec),
      outputVec(outputVec), client(client), map(), barrier(multiThreadLevel) ,hold_lock_thread(-1) ,_size( inputVec.size()),
        mutex(PTHREAD_MUTEX_INITIALIZER), mt_level(multiThreadLevel) {
//        intermediateVecs = new std::vector<IntermediateVec*>[mt_level];

        // absolute position
        threads = new pthread_t*[multiThreadLevel];
        contexts = new Context*[multiThreadLevel];

        // allocate memory on the heap.
        for ( int i = 0 ; i < multiThreadLevel; i++ ) {
            threads[i] = new pthread_t;
            contexts[i] = new Context(i, this);
            pthread_create( threads[i], nullptr, &map_reduce_call, contexts[i]);
        }

        alive = new std::atomic<bool>[ multiThreadLevel ];
        for ( int i =0 ; i < multiThreadLevel; i++)
            alive[i] = true;
    }
    ~GlobalEnv(){
        for ( int i = 0 ; i < mt_level; i++ ) {
            delete threads[i]; //TODO!11111111
            delete contexts[i];
            // should i terminate threads??
        }// TODO delete map
        delete threads;
        delete contexts;
    }
    
    InputVec inputVec;
    OutputVec& outputVec;
    const MapReduceClient& client;
    Barrier barrier;

    stage_t stage = UNDEFINED_STAGE;
    Context** contexts;
    
    pthread_t ** threads;
    std::atomic<int> hold_lock_thread;
    pthread_mutex_t mutex;
    int mt_level;

    std::atomic<bool> * alive;
    // absolute memory.
//    std::vector<IntermediateVec*> intermediateVecs;
    
    // points on items in intermediateVecs.
    struct cmpK2 {
        bool operator()(const K2* a, const K2* b) const {
            return *a < *b;
        }
    };

    std::map<K2* , IntermediateVec*, cmpK2> map;

    std::atomic<float> _size;
    
    void inline lock()
    {


//        log("")
        if (pthread_mutex_lock(&mutex) != 0){
		    fprintf(stderr, "[[Barrier]] error on pthread_mutex_lock");
		    exit(1);
	    }
    }

    void inline unlock()
    {

//        log("")
        if (pthread_mutex_unlock(&mutex) != 0) {
		    fprintf(stderr, "[[Barrier]] error on pthread_mutex_unlock");
		    exit(1);
	    }
    }

    IntermediateVec * mappop( ) {
        IntermediateVec * ret = this->map.begin()->second;
        this->map.erase(this->map.begin());
        return ret;
    }

//    void emit2 (K2* key, V2* value) {
//        lock();
//        if ( this->map.find(key) == this->map.end() )
//        {
//            this->intermediateVecs.push_back( new IntermediateVec() );
//            this->map[key] = this->intermediateVecs.back();
//        }
//        this->map[key]->push_back( IntermediatePair( key, value));
//
//        log("");
//        unlock();
//    }

    void emit3 (K3* key, V3* value) {
        this->outputVec.push_back( OutputPair( key, value ) );
    }

};



// should executed only once.
void shuffle(GlobalEnv* _globalEnv)
{
    log("");

    for(int i = 0 ; i < _globalEnv->mt_level ; i++){
        IntermediateVec vec = _globalEnv->contexts[i]->interVec;
        for (auto pair : vec){
            if ( _globalEnv->map.find(pair.first) == _globalEnv->map.end()){
                _globalEnv->map[pair.first] = new IntermediateVec();
            }
            _globalEnv->map[pair.first]->push_back(pair);
        }
    }
    _globalEnv->_size = _globalEnv->map.size();

}


void * map_reduce_call (void * context)
{


    auto* threadContext = (Context*) context;
    GlobalEnv* _globalEnv= threadContext->env;

    _globalEnv->stage=MAP_STAGE;
    _globalEnv->lock();
    // while there are still missions in the queue. 
    while ( !_globalEnv->inputVec.empty()){
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
    if(threadContext->threadID == 0){
        shuffle(_globalEnv);
    }
    _globalEnv->barrier.barrier();
    _globalEnv->stage=REDUCE_STAGE;
    _globalEnv->lock();
    while ( !_globalEnv->map.empty()){
        IntermediateVec * topop = _globalEnv->map.begin()->second;
        _globalEnv->map.erase(_globalEnv->map.begin());
        _globalEnv->unlock();
        _globalEnv->client.reduce(topop, context);
        _globalEnv->lock();
    }
    _globalEnv->unlock();
    return context;
}


JobHandle startMapReduceJob(const MapReduceClient& client,
	const InputVec& inputVec, OutputVec& outputVec,
	int multiThreadLevel) {    
    return new GlobalEnv (inputVec, outputVec, client, multiThreadLevel);
}


//void ** freethread( )
//{
//    log("")
//    if (pthread_mutex_unlock(&mutex) != 0) {
//        fprintf(stderr, "[[Barrier]] error on pthread_mutex_unlock");
//        log("pthread_mutex_unlock")
//    }
//}

void waitForJob(JobHandle job) {
    auto * _globalEnv = (GlobalEnv *) job;
    log("")
    for(int i = 0 ; i < _globalEnv->mt_level; i++) {
        std::cout << "joining the " << i << "th thread" << std::endl;
        pthread_tryjoin_np(*_globalEnv->threads[i], NULL);
//        _globalEnv->alive[i] = false;
//        if (_globalEnv->alive[i]) {
//        }
    }

    log("Done Wait")

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
    waitForJob(job);
    auto * _globalEnv = (GlobalEnv *) job;


    //    delete _globalEnv;
}


void emit2 (K2* key, V2* value, void* context) {
    auto* cont = (Context*) context;
    cont->interVec.push_back(IntermediatePair( key, value));
}
void emit3 (K3* key, V3* value, void* context) {
    auto* cont = (Context*) context;
    cont->env->emit3(key, value);
}
