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
uint64_t inline max( uint64_t a , uint64_t b) {
    return a > b ? a : b;
}

bool tableIsClear( uint64_t frameIndex ){
    word_t val = 0;

    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
      PMread( (frameIndex * PAGE_SIZE) + i, &val);
      if(val == 0){
        return false;
      }
    }
    return true;
}






void clearTable(uint64_t frameIndex) {
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMwrite((frameIndex * PAGE_SIZE) + i, 0);
    }
}
void VMinitialize() {
    clearTable(0);
}


physical_addr convert(uint64_t virtualAddress, physical_addr addr )
{
    temp_type offset    = virtualAddress % PAGE_SIZE ;
    temp_type page      = virtualAddress >> PAGE_SIZE;
    log()
    printf("frame: %d \n", addr);
    printf("offset: %lu \n", offset);
    printf("total: %llu \n", addr * PAGE_SIZE + offset);
    return addr * PAGE_SIZE + offset;
}


// tuple
struct AddressState {
    physical_addr addr;
    physical_addr nextaddr;
};

struct AddressState search(uint64_t virtualAddress) {
    struct AddressState ret = { 0, 0};
    word_t qal = 0;
    uint64_t temp_add = virtualAddress;
    for ( int i = 0 ; i < TABLES_DEPTH; i++ ) {

        // printf( "[DEBUG] depth %d, PAGE_SIZE : %d , TABLES_DEPTH : %d \n", i, PAGE_SIZE, TABLES_DEPTH);
        // printf( "[DEBUG] val.addr : %d, val.nextaddr  : %d \n", ret.addr, ret.nextaddr);

        PMread(convert( virtualAddress, ret.addr), &qal );

        ret.nextaddr= qal;
        if ( ret.nextaddr == 0 ) {
            log()
            // pathch, should remove.
            ret.addr = convert( virtualAddress, ret.addr);
            printf("%d \n", ret.addr);
            ret.nextaddr = -1;
            return ret;
        }

        virtualAddress /= PAGE_SIZE;

        ret.addr = ret.nextaddr;
    }
    // printf( "[DEBUG] val.addr : %d, val.nextaddr  : %d \n", ret.addr, ret.nextaddr);
    return ret;
}


#define BFS
#ifdef BFS
#include <fstream>
#include <vector>
#include <utility>
#include <iostream>
void print_BFS(std::vector< std::pair<physical_addr,  physical_addr>> queue, std::ofstream & _file ) {

    if (queue.empty())
        return;

    auto ppair = queue.front();
    queue.erase( queue.begin() );
    _file << ppair.first << " " <<  ppair.second << "\n";
    word_t temp_addr = 0;
    
    for (uint64_t i = 0; i < PAGE_SIZE ; ++i) {
         
	 if ( (ppair.second * PAGE_SIZE + i) < RAM_SIZE ){
         	PMread(ppair.second * PAGE_SIZE + i, &temp_addr);
	 	if (temp_addr != 0) {
            		queue.push_back(std::pair<physical_addr, physical_addr>( ppair.second * PAGE_SIZE + i, temp_addr));
         	}
	 }
         
    }
    print_BFS(queue, _file );
}

#endif

// not max, claculate the size of the tree.
uint64_t getmax_frame_index_DFS( uint64_t depth, physical_addr current_addr ) {
    uint64_t _max_frame  = 1;
    log()
    if ( depth < TABLES_DEPTH) {
        word_t temp_addr = 0;
        for ( uint64_t i = 0 ; i < PAGE_SIZE; ++i ) {
            PMread( current_addr* PAGE_SIZE + i, &temp_addr);
            if ( temp_addr != 0 ) {
                _max_frame += getmax_frame_index_DFS( depth+1,  (physical_addr) temp_addr );
                printf( "[DEBUG] frame : %lu, cur_addr : %d \n", _max_frame, temp_addr );
            }
        }
    }
    printf( "[DEBUG] frame : %lu, \n", _max_frame );
    return _max_frame;
}

void findFreeTable( uint64_t depth, physical_addr current_addr, temp_type & freeFrame ) {
    log()
    if ( depth >= TABLES_DEPTH)
        return;
    physical_addr temp_addr = 0;
    for (uint64_t i = 0; i < PAGE_SIZE && (freeFrame == -1); ++i) {
        PMread(current_addr * PAGE_SIZE + i, (word_t *) &temp_addr);
        if (temp_addr != 0) {
            if(tableIsClear(temp_addr)){
                freeFrame = temp_addr;
                PMwrite(current_addr * PAGE_SIZE + i, 0);
                return;
            }
            else{
            findFreeTable( depth +1, temp_addr, freeFrame );
            }
        }
    }
}


int NewNode ( physical_addr addr, uint64_t frame ) {
    log()
    printf( "[DEBUG] addr %d, frame : %d, \n", addr, frame );
    PMwrite( addr, frame);

    return 1;
}

struct AddressState  getPAddr(uint64_t virtualAddress){
    log()

    log()
    virtualAddress = virtualAddress >> OFFSET_WIDTH;
    struct AddressState val = {0,0};
    val.nextaddr =-1;
    for  ( ; val.nextaddr  == -1 ; val = search(virtualAddress) )  {

        physical_addr emptyframe = -1;
        // after initialize new node, the new should point to it's perent
        PMwrite(val.addr, val.addr / PAGE_SIZE );
        findFreeTable( 0, 0 ,emptyframe  );
        if ( emptyframe == -1 ) {
            PMwrite(val.addr, 0 );
            emptyframe = getmax_frame_index_DFS(0, 0 );
            if (emptyframe >= NUM_FRAMES) {
                //evict!
            }
        }
        NewNode(val.addr, emptyframe );
    }

    return val;
}


int VMread(uint64_t virtualAddress, word_t* value) {
    struct AddressState val = getPAddr( virtualAddress );

    printf("[DEBUG] VMread addr:  %d,\n", val.addr);
    // last step
    PMread (
         convert( 0, val.nextaddr),
            (word_t *) value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    struct AddressState val = getPAddr( virtualAddress );

    // last step
    PMwrite(
        convert( 0 , val.nextaddr),  value);
    printf("[DEBUG] write addr:  %d, the val: %d\n", val.addr, value);


#ifdef BFS

    auto u = std::pair<uint64_t, physical_addr>(0,0);
    auto V = std::vector<std::pair<uint64_t, physical_addr>>(  );
    V.push_back(u);
    std::cout << "Depth:" <<TABLES_DEPTH << " page size:"<< PAGE_SIZE<<"\n\n\n\n\n\n";
    std::ofstream _file ("test_plot.txt", std::ios_base::app);
    print_BFS(V, _file);
    std::cout << "\n\n\n\n\n\n";
#endif

    return 1;
}



