#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#include <cstdio>



#define DEBUG

#ifdef DEBUG
#define log()\ 
    printf( "[DEBUG] %s \n", __FUNCTION__ );
#endif


typedef uint64_t physical_addr;
typedef uint64_t temp_type;

int toDec(uint64_t index){
    int dec_value = 0;
    int base = 1;
    uint64_t temp = index;
    while (temp > 0) {
        int last_digit = temp % 10;
        temp = temp / 10;
        dec_value += last_digit * base;
        base *= 2;
    }
    return dec_value;
}

bool tableIsClear( uint64_t frameIndex ){
    word_t* val;

    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
      PMread( (frameIndex * PAGE_SIZE) + i, val);
      if(val == 0){
        return false;
      }
    }
    return true;
}

temp_type NewFrame();




void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite((frameIndex * PAGE_SIZE) + i, 0);
    }
}
void VMinitialize() {
    //clearTable(0);
}

inline physical_addr convert(uint64_t virtualAddress, physical_addr addr )
{
    temp_type offset    = virtualAddress & (PAGE_SIZE-1);
    temp_type page      = virtualAddress >> PAGE_SIZE;
    return addr * PAGE_SIZE + offset;
} 


// tuple
struct AddressState {
    physical_addr addr; 
    physical_addr nextaddr; 
};

struct AddressState search(uint64_t virtualAddress) {
    struct AddressState ret = { 0, 0};
    for ( int i = 0 ; i < TABLES_DEPTH  ; i++ ) {

        printf( "[DEBUG] depth %d, PAGE_SIZE : %d \n", i, PAGE_SIZE);

        PMread(
            convert( virtualAddress, ret.addr),
             (word_t *) &ret.nextaddr);

        if ( ret.nextaddr == 0 )
            return ret;            
        

        virtualAddress >>= PAGE_SIZE;
    }
    return ret;
}

uint64_t inline max( uint64_t a , uint64_t b) {
    return a ? a > b : b; 
}

uint64_t getmax_frame_index_DFS( uint64_t depth, physical_addr current_addr ) {
    uint64_t _max_frame  = 0; 
    log()
    
    if ( depth < TABLES_DEPTH ) {
        physical_addr temp_addr = 0;
        for ( uint64_t i = 0 ; i < PAGE_SIZE; ++i ) {
            PMread( current_addr* PAGE_SIZE + i, (word_t *) &temp_addr);
            if ( temp_addr != 0 ) {
                _max_frame = max( getmax_frame_index_DFS( depth+1, temp_addr ) , _max_frame );                
            }
        }
    }
    return _max_frame;
}


int NewNode ( physical_addr addr, uint64_t frame ) {
    log()
    PMwrite( addr, frame);
    
    return 1;
}

struct AddressState  getPAddr(uint64_t virtualAddress){
    struct AddressState val = {0, 0};
    log()
    uint64_t max_frame_index = getmax_frame_index_DFS(0, 0 ); 
    log()
    for  ( ; val.nextaddr  == 0 ; val =  search(virtualAddress) )  {
        
        // should create new node.
        if ( max_frame_index < NUM_FRAMES ) {
            NewNode( val.addr, max_frame_index );
            max_frame_index++;
        }
        
        // evicate by weight.  
        else {
            
        }
    }
    return val;
}


int VMread(uint64_t virtualAddress, word_t* value) {
    struct AddressState val = getPAddr( virtualAddress );

    // last step 
    PMread (
         convert( 0, val.addr),
            (word_t *) value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    struct AddressState val = getPAddr( virtualAddress );

    // last step 
    PMwrite(
        convert( 0 , val.addr),  value);
    return 1;
}



