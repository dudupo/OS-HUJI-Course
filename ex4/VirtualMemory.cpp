#include "VirtualMemory.h"
#include "PhysicalMemory.h"
#include "MemoryConstants.h"

#define DEBUG

#ifdef DEBUG
#include <cstdio>
#include <iostream>
#define log()\
    printf( "[DEBUG] %s \n", __FUNCTION__ );
#endif


// class Table


uint64_t getPAGE_SIZE( ) {
    return PAGE_SIZE;
}

inline uint64_t FIRST_STEP_WIDTH () {
    return VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH ? VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH : OFFSET_WIDTH;  
}
inline uint64_t FIRST_STEP() {
    return (1LL << FIRST_STEP_WIDTH());
}
inline uint64_t PMread_gate( uint64_t address, word_t * val ) {
    // log()
    // std::cout << address << "\n";
    PMread( address, val );
    return *val;
}
inline uint64_t PMread_gate( uint64_t address ) {
    word_t val;    
    return PMread_gate( address, &val );
}
inline int PMwrite_gate( uint64_t address, word_t val ){
    log()
    std::cout << address << "\n";
    PMwrite( address, val );
}
struct State {
    uint64_t virtualAddress;
    uint64_t address; 
    
    // can be thought also as frame. 
    uint64_t parant;
};

void log_state(struct State state){
    std::cout <<
     "State:\n\t >> state.virtualAddress : " <<
        state.virtualAddress <<  "\n\t >> " <<
        "state.address : " << state.address << "\n\t >> " <<
         "parant : "  <<  state.parant << "\n" ;
}

uint64_t advanceStete(
     struct State state, uint64_t direaction, uint64_t page_size ) {
    
    return page_size * state.address + direaction;
}


struct State makeStep(
    struct State state, uint64_t direaction, uint64_t page_size) {

    struct State nextState = { 
        0,
        0,
        state.address
    };
    
    nextState.address =\
     PMread_gate( advanceStete( state, direaction, page_size ) );
    
    return nextState;
}
struct State makeStep(
    struct State state, uint64_t direaction) {
    return makeStep( state, direaction, getPAGE_SIZE());
}
struct State makeStep(
    struct State state, uint64_t virtualAddress,
        uint64_t current_width, uint64_t step_width) {

    return makeStep( state,
        (virtualAddress >> (current_width - step_width)) & (PAGE_SIZE - 1) );
}
struct State firstFrame(uint64_t virtualAddress) {
    struct State root_position = { 0, 0 };
    
    struct State ret = makeStep( root_position, virtualAddress,
         VIRTUAL_ADDRESS_WIDTH,FIRST_STEP_WIDTH() );
    log()
    log_state(ret);
    return ret;
}
void DFS( struct State state, int page_size,
 void (func(struct State, void *)), void * params){ 
    log()
    std::cout << page_size << "\n";

    // advanceStete(state, i, page_size) < NUM_FRAMES
    for (uint64_t i = 0;  i < page_size &&
         (advanceStete(state, i, page_size) < NUM_FRAMES); ++i) {
        
        struct State nextPosition = makeStep(state, i, page_size); 
        if ( nextPosition.address )
            DFS( nextPosition, PAGE_SIZE, func, params);
    } 
    func(state, params);
}
void DFS(
 void (func( struct State, void *)), void * params){ 
    struct State root_position = { 0, 0 };
    DFS(root_position, FIRST_STEP(), func, params);
}
void _getMaxFrame(struct State state, void * params){
    (*((int *) params)) +=1;
    log()
    std::cout << (int) * ((int*)params) <<"\n";
}

uint64_t getMaxFrame( ){
    uint64_t maxframe = 0;
    DFS( &_getMaxFrame, (void *) &maxframe );
    return maxframe;
}

uint64_t getPage(struct State state, uint64_t virtualAddress, int flag ) {
    
    uint64_t width = FIRST_STEP_WIDTH(); // + OFFSET_WIDTH; 
    
    if (flag) {
        width += OFFSET_WIDTH;
    }
    struct State nextState; 
    int j = 0;
    while(state.address < (NUM_FRAMES / getPAGE_SIZE()) ) {
        j++;
        log()
        
        log_state( state );
        
        
        nextState = makeStep(
            state,
            virtualAddress,
            VIRTUAL_ADDRESS_WIDTH,
            width);


        log_state( nextState );
        if ( nextState.address == 0 ) {
            uint64_t getMaxFrame_t = getMaxFrame();
            uint64_t direaction =  (virtualAddress >> (VIRTUAL_ADDRESS_WIDTH - width)) & ( PAGE_SIZE -1); 
            
            std::cout << " [GET_MAX_FRAME] " << getMaxFrame_t << "\n"; 

            
            std::cout << "[direaction]" << direaction << "\n";
            // new step
            PMwrite_gate(advanceStete(state, direaction, getPAGE_SIZE()), getMaxFrame_t);
        }

        else {
            
            state = nextState;
            width += OFFSET_WIDTH;
        }
    }
    return state.address;
}


int64_t getPage(uint64_t virtualAddress, int flag ) {
    virtualAddress = virtualAddress & ( VIRTUAL_MEMORY_SIZE -1 );

    // handle the first page.
    return getPage( firstFrame(virtualAddress)  , virtualAddress, flag);
}

void VMinitialize() {

    for (uint64_t i = 0; i < NUM_FRAMES; i++) {
        for (uint64_t j = 0; j < PAGE_SIZE; j++) {
            PMwrite_gate( i * PAGE_SIZE + j, 0);
        }
    }
}

int VMread(uint64_t virtualAddress, word_t *value) {

    if ( virtualAddress >=  VIRTUAL_MEMORY_SIZE )
        return 0;

    log()
    // last step
    uint64_t _page = getPage(virtualAddress, 1);
    std::cout  << " [READ]: " ;
    std::cout << (PAGE_SIZE * _page)  << "\n";
    std::cout << (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1))  << "\n";
    PMread( (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1)), value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {

    if ( virtualAddress  >=  VIRTUAL_MEMORY_SIZE    )
        return 0;
    log()
    uint64_t _page = getPage(virtualAddress, 0);
    std::cout  << " PAGE: " << _page << "\n";
    std::cout << (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1))  << "\n";
    // PMrestore( _page, virtualAddress >> OFFSET_WIDTH  );
    PMwrite( (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1)), value); 
    return 1;
}



