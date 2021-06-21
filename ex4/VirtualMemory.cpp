#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#include <cstdio>

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


physical_addr& getPAddr(temp_type page){

}


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

physical_addr search(uint64_t virtualAddress) {
    physical_addr addr = 0;
    for ( int i = 0 ; i < TABLES_DEPTH  ; i++ ) {

        printf( "[DEBUG] %d\n", i);

        PMread(
            convert( virtualAddress, addr),
             (word_t *) &addr);

        if ( addr == 0 )
            return addr;            
        

        virtualAddress >>= PAGE_SIZE;
    }
    return addr;
}

int VMread(uint64_t virtualAddress, word_t* value) {
    physical_addr val = 0 , preval = 0;
    for  ( ; val  == 0 ; val =  search(virtualAddress) )  {
            
        // create, or evicate. 
    }

    // last step 
    PMread (
         convert( 0, val),
            (word_t *) value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    
    physical_addr val = 0;
    for  (  ; val  == 0 ; val =  search(virtualAddress) )  {
        // evicate 
    }    

    // last step 
    PMwrite(
        convert( 0 , val),  value);
    return 1;
}



