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
inline uint64_t PMwrite_gate( uint64_t address, word_t val ){
    // for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
    //     if ( (PAGE_SIZE * val +i) < RAM_SIZE  ) {
    //         PMwrite( (PAGE_SIZE * val +i), 0 );        
    //     }
    // }
    PMwrite( address, val );
}
struct State {
    uint64_t virtualAddress = 0x0000;
    uint64_t address = 0x0000;
    
    // can be thought also as frame. 
    uint64_t parant = 0x0000;
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

    struct State nextState;
    nextState.parant = state.address;
    
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

            log()
            
            std::cout << "[DEBUG] " << (virtualAddress >> (current_width - step_width)) <<"\n";

    return makeStep( state,
        (virtualAddress >> (current_width - step_width)) & (PAGE_SIZE - 1) );
}
struct State firstFrame(uint64_t virtualAddress) {
    struct State root_position; 
    struct State ret = makeStep( root_position, virtualAddress,
         VIRTUAL_ADDRESS_WIDTH,FIRST_STEP_WIDTH() );
    log()
    log_state(ret);
    return ret;
}
void DFS( struct State state, uint64_t page_size,
 void (func(struct State, void *)), void * params){ 
    log()
    std::cout << page_size << "\n";

    // advanceStete(state, i, page_size) < NUM_FRAMES
    for (uint64_t i = 0;  (i < page_size) &&
         (advanceStete(state, i, page_size) < NUM_FRAMES); ++i) {
        
        struct State nextPosition = makeStep(state, i, page_size); 
        if ( nextPosition.address != 0x0000 )
            DFS( nextPosition, PAGE_SIZE, func, params);
    } 
    func(state, params);
}
void DFS(
 void (func( struct State, void *)), void * params){ 
    struct State root_position;
    DFS(root_position, FIRST_STEP(), func, params);
}
void _getMaxFrame(struct State state, void * params){
    (*((uint64_t *) params)) +=1;
    log()
    std::cout << (uint64_t) * ((uint64_t*)params) <<"\n";
}

uint64_t getMaxFrame( ){
    uint64_t maxframe = 0;
    DFS( &_getMaxFrame, (void *) &maxframe );
    return maxframe;
}


struct WeightState {
    uint64_t weight = 0x0000;
    struct State state;
};

inline uint64_t getWieght(uint64_t i) {
    return i & 1 ? WEIGHT_ODD : WEIGHT_EVEN;
}

void getMaxFrame_WEIGHT( struct WeightState state,
 struct WeightState * maxstate, uint64_t page_size ){ 
    log()
    std::cout << page_size << "\n";

    // advanceStete(state, i, page_size) < NUM_FRAMES
    for (uint64_t i = 0; i < page_size ; ++i) {
        
            struct WeightState nextPosition;
            nextPosition.state = makeStep(state.state, i, page_size); 
            nextPosition.weight = state.weight + getWieght( i );  
       
        if (advanceStete(state.state, i, page_size) < NUM_FRAMES) {
                
            if ( nextPosition.state.address != 0x0000 )
                getMaxFrame_WEIGHT( nextPosition, maxstate, PAGE_SIZE);
        }
        else {
            if ( maxstate->weight > state.weight ) {
                *maxstate = state;
            }
        }
   }
}
void getMaxFrame_WEIGHT( struct WeightState * maxstate ) {
    log();
    struct WeightState root; 
    getMaxFrame_WEIGHT( root, maxstate, FIRST_STEP() );
}

void clean_frames(struct State state) {

}

void getfreeFrame( struct State state, uint64_t & ret,  uint64_t page_size, uint64_t current_set ){ 
    log()
    std::cout << page_size << "\n";

    for (uint64_t i = 0; (i < page_size) && 
        (advanceStete(state, i, page_size) < NUM_FRAMES)  ; ++i) {        
        struct State nextPosition;
        nextPosition = makeStep(state, i, page_size); 

        if ( nextPosition.address != 0x0000 ) {
            getfreeFrame( nextPosition, ret , PAGE_SIZE, current_set);
        }
        else {             
            if ( (ret == -1)  && (current_set != advanceStete(state, i, page_size))  ) {
                ret = advanceStete(state, i, page_size); 
            }
        }
    }
} 

void getfreeFrame( uint64_t & ret, uint64_t current_set ){
    log();
    struct State root; 
    getfreeFrame( root, ret, FIRST_STEP(), current_set );
    std::cout << ret << "\n";
}


int could_advance( struct State state, uint64_t virtualAddress, uint64_t width, uint64_t page_size ) {
    
    return  ((virtualAddress >> (VIRTUAL_ADDRESS_WIDTH - width)) & ( page_size -1)) > 0;     
}

uint64_t newFrame(struct State state, uint64_t virtualAddress, uint64_t width){
    uint64_t getMaxFrame_t = getMaxFrame();
    std::cout << " [GET_MAX_FRAME] " << getMaxFrame_t << "\n"; 
    
    if (  getMaxFrame_t < NUM_FRAMES ) {

        uint64_t retfreme = -1;
        getfreeFrame( retfreme, state.address );
    
        uint64_t direaction =  (virtualAddress >> (VIRTUAL_ADDRESS_WIDTH - width)) & ( PAGE_SIZE -1); 
        std::cout << "[direaction]" << direaction << "\n";
        PMwrite_gate(advanceStete(state, direaction, getPAGE_SIZE()), getMaxFrame_t);
        return advanceStete(state, direaction, getPAGE_SIZE());
    }
    else {
        log();
        struct WeightState maxstate;  
        getMaxFrame_WEIGHT( &maxstate);
        PMwrite_gate(maxstate.state.parant, 0);
        PMevict( maxstate.state.parant, maxstate.state.address );
        return maxstate.state.parant;
    }
}

uint64_t getPage(struct State state0, uint64_t virtualAddress, int flag ) {
    
    uint64_t width = FIRST_STEP_WIDTH(); // + OFFSET_WIDTH; 
    
    // first step
    struct State state; 

    struct State nextState = makeStep( state,
                virtualAddress,
                VIRTUAL_ADDRESS_WIDTH,
                width);

    
    while ( width  < VIRTUAL_ADDRESS_WIDTH )  {
        std::cout << "[ WIDTH ] " << width << ", " <<  VIRTUAL_ADDRESS_WIDTH << "\n";
        
        log_state( state );
        if ( nextState.address == 0x0000  ) {
            nextState.address = newFrame( state, virtualAddress, width); 
        }
        
        
        state = makeStep( state,
                virtualAddress,
                VIRTUAL_ADDRESS_WIDTH,
                width);
        
        width += OFFSET_WIDTH;    
        
        log_state(state);

        nextState = makeStep( state,
                virtualAddress,
                VIRTUAL_ADDRESS_WIDTH,
                width);

        if ( width == VIRTUAL_ADDRESS_WIDTH ){
            if ( nextState.address == 0x0000  ) {
                nextState.address = newFrame( state, virtualAddress, width); 
            }
            std::cout << "[PMrestore]" << state.address << ", "  << (virtualAddress >> OFFSET_WIDTH) << "\n";
            PMrestore( state.address ,  virtualAddress >> OFFSET_WIDTH  );
            return state.address;
        }
        
        log_state(nextState);
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

    if ( virtualAddress >=  VIRTUAL_MEMORY_SIZE ){
        std::cout << "virtualAddress >=  VIRTUAL_MEMORY_SIZE\n";
        return 0;
    }
    log()
    // last step
    uint64_t _page = getPage(virtualAddress, 0);

    // if ( NUM_FRAMES ==  ) {
    //     return _page;
    // }

    // std::cout  << " [READ]: " ;
    // std::cout << (PAGE_SIZE * _page)  << "\n";
    // std::cout << (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1))  << "\n";
    // std::cout << " [PAGE_SIZE]: " << PAGE_SIZE << "\n";
    // std::cout << "[_page] " << _page << "\n";
    // std::cout << "[INIT_WIDTH] " << FIRST_STEP_WIDTH() << ", " << OFFSET_WIDTH << "\n";

    uint64_t addr =  (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1));

    if (  addr >= RAM_SIZE ){
        std::cout << "222 virtualAddress >=  VIRTUAL_MEMORY_SIZE\n";
        return 0; 
    }

    PMread( addr, value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {

    if ( virtualAddress  >=  VIRTUAL_MEMORY_SIZE    )
        return 0;
    log()
    uint64_t _page = getPage(virtualAddress, 0);
    std::cout  << " PAGE: " << _page << "\n";
    std::cout << (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1))  << "\n";
    
    uint64_t addr =  (PAGE_SIZE * _page) + (virtualAddress & (PAGE_SIZE-1));
    

    if ( addr >= RAM_SIZE)
        return 0; 

    PMwrite(  addr  , value); 
    return 1;
}



