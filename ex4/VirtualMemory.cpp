#include "VirtualMemory.h"
#include "PhysicalMemory.h"


typdef uint64_t physical_addr;
typdef uint64_t temp_type;

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

temp_type NewFrame();


physical_addr& getPAddr(temp_type page){



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
    temp_type offset = virtualAddress & (PAGE_SIZE-1);
    temp_type page =  virtualAddress >> PAGE_SIZE;
    physical_addr addr = getPAddr(page);
    if(addr == 0){return 0;}
    PMread(addr * PAGE_SIZE+offset, value);
    return 1;
}


int VMwrite(uint64_t virtualAddress, word_t value) {
    temp_type offset = virtualAddress & (PAGE_SIZE-1);
    temp_type page =  virtualAddress >> PAGE_SIZE;
    physical_addr addr = getPAddr(page);
    if(addr == 0){return 0;}
    PMwrite(addr * PAGE_SIZE+offset, value);
    return 1;
}



