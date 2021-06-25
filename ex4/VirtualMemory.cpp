#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#include <cstdio>
#include <iostream>


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

struct Path {
    physical_addr paths[TABLES_DEPTH+1]; 
};


#include <cmath>

struct Path get_path(uint64_t virtualAddress) {

    log();
    struct Path ret;
    // ret.paths[TABLES_DEPTH]  = virtualAddress & WORD_WIDTH;
    // virtualAddress >> WORD_WIDTH;
    for (int i = 0 ; i <= TABLES_DEPTH; virtualAddress >>= OFFSET_WIDTH ) {
        std::cout << "virtualAddress : " <<  virtualAddress << "\n";
        ret.paths[TABLES_DEPTH - i] = virtualAddress & (PAGE_SIZE-1);     
        i++;
    }
    for (int i = 0; i <= TABLES_DEPTH; ++i) {
        std::cout << ret.paths[i] << "\n";
    }
    return ret;
}

physical_addr convert( physical_addr offset, physical_addr addr)
{
    log()
    // printf("frame: %d \n", addr);
    // printf("offset: %lu \n", offset);
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
    
    struct Path offsets = get_path( virtualAddress );
    
    for ( int i = 0 ; i < TABLES_DEPTH; i++ ) {
        
        PMread(convert(offsets.paths[i],ret.addr), &qal );        
        if ( qal == 0 ) {
            log()
            // pathch, should remove.
            ret.addr = convert(offsets.paths[i],ret.addr);
            ret.nextaddr = -1;
            return ret;
        }
        else{
            ret.addr = qal;
        }
    }
    ret.nextaddr = qal;
    return ret;
}


// #define BFS
#ifdef BFS
#include <fstream>
#include <vector>
#include <utility>
#include <iostream>
void print_BFS(std::vector< std::pair<physical_addr,  physical_addr>> & queue, std::ofstream & _file ) {

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
                    queue.push_back(std::pair<physical_addr, physical_addr>(
                        ppair.second * PAGE_SIZE + i, temp_addr));
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
            else { 
                findFreeTable( depth +1, temp_addr, freeFrame );
                if (freeFrame != -1 ) {
                    return;
                }
            }
        }
    }
}


/* 
    still not consider the paritity of the cell. (last element).
*/
void find_max_weight_node( uint64_t depth, uint64_t addr, uint64_t & max_weight_addr , uint64_t weight , uint64_t & max_weight ) {
    if ( depth < TABLES_DEPTH ) {
        physical_addr temp_addr = 0;
        int parity = 0; 

        for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
            PMread(addr * PAGE_SIZE + i, (word_t *) &temp_addr);
            if (temp_addr != 0) {
                find_max_weight_node( depth +  1 , max_weight_addr, temp_addr, 
                    weight + (temp_addr & 1) * WEIGHT_ODD +
                        ( (temp_addr ^ 1) & 1 ) * WEIGHT_EVEN , max_weight );
            }
        }
        if ( weight > max_weight){
            max_weight = weight;
            max_weight_addr = addr;
        }
    }
}

void evicte( ) {
    log()
    uint64_t max_weight_addr, max_weight;
    find_max_weight_node( 0, 0, max_weight_addr, 0 , max_weight );
    printf( "[DEBUG] addr %d, weight : %d, \n", max_weight_addr, max_weight );
    PMwrite(max_weight_addr, 0);
}

int NewNode ( physical_addr addr, uint64_t frame ) {
    log()
    printf( "[DEBUG] addr %d, frame : %d, \n", addr, frame );

    PMwrite( addr, frame);
    return 1;
}





struct AddressState  getPAddr(uint64_t virtualAddress){
    log()



    struct AddressState val = {0,0};
    
    // should initalize to -1. BUG;
    val = search(virtualAddress);
    
    for  ( ; val.nextaddr  == -1 ; val = search(virtualAddress) )  {

        physical_addr emptyframe = -1;
        // after initialize new node, the new should point to it's perent
        findFreeTable( 0, 0 ,emptyframe);
        
        if ( emptyframe == -1 ) {
            // PMwrite(val.addr, 0 );
            emptyframe = getmax_frame_index_DFS(0, 0);
            if (emptyframe >= NUM_FRAMES) {
                evicte();
                emptyframe--;
            }
        }
        // else {
        NewNode(val.addr, emptyframe );
            
        //     PMwrite(val.addr, val.addr / PAGE_SIZE );
        // }
    }
    return val;
}


int VMread(uint64_t virtualAddress, word_t* value) {
    struct AddressState val = getPAddr( virtualAddress );

    printf("[DEBUG] VMread addr:  %d,\n", val.addr);
    // last step
    PMread (
         convert( virtualAddress &  (PAGE_SIZE-1), val.addr),
            (word_t *) value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    struct AddressState val = getPAddr( virtualAddress );
    
    PMwrite(
        convert( virtualAddress &  (PAGE_SIZE-1) , val.addr),  value);
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



