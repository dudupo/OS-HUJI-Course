#include "VirtualMemory.h"
#include "PhysicalMemory.h"


typdef uint64_t physical_addr;

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

bool tableIsClear(){
    word_t* val;
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
      PMread(frameIndex * PAGE_SIZE) + i, val);
      if(val == 0){
        return false;
      }
    }
    return true;
}

uint64_t NewFrame();
    // look in tree for 0 frames (and remove ref from father
    // next frame in the order
    //

physical_addr& getPAddr(uint64_t frameIndex){
    uint64_t frame = 0;
    for(int i = 0 ; i < TABLES_DEPTH ; ++i)

}


void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite((frameIndex * PAGE_SIZE) + i, 0);
    }
}

void VMinitialize() {
    clearTable(0);
}


int VMread(uint64_t virtualAddress, word_t* value) {
    physical_addr addr = getPAddr(virtualAddress);
    if(addr == 0){return 0;}
    PMread(addr, value);
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    physical_addr addr = getPAddr(virtualAddress);
    if(addr == 0){return 0;}
    PMwrite(addr, value);
    return 1;
}



