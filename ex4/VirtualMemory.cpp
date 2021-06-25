#include "VirtualMemory.h"
#include "PhysicalMemory.h"

#include <cstdio>

#include <cmath>

#define DEBUG

#ifdef DEBUG

#include <iostream>

#define log()\
    printf( "[DEBUG] %s \n", __FUNCTION__ );
#endif


typedef uint64_t physical_addr;
typedef uint64_t temp_type;
// tuple
struct AddressState {
    physical_addr addr;
    physical_addr nextaddr;
    uint64_t depth;
};


bool tableIsClear(uint64_t frameIndex) {
    word_t val = 0;

    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
        PMread((frameIndex * PAGE_SIZE) + i, &val);
        if (val != 0) {
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
    physical_addr paths[TABLES_DEPTH + 1];
};


struct Path get_path(uint64_t virtualAddress) {

    log();
    struct Path ret;
    // ret.paths[TABLES_DEPTH]  = virtualAddress & WORD_WIDTH;
    // virtualAddress >> WORD_WIDTH;
    virtualAddress >>= OFFSET_WIDTH;
    for (int i = 0; i < TABLES_DEPTH; virtualAddress >>= OFFSET_WIDTH) {
        std::cout << "virtualAddress : " << virtualAddress << "\n";
        ret.paths[TABLES_DEPTH - i-1] = virtualAddress & (PAGE_SIZE - 1);
        i++;
    }
    // DEBUG:
    for (int i = 0; i < TABLES_DEPTH; ++i) {
        std::cout << ret.paths[i] << "\n";
    }
    return ret;
}

inline physical_addr convert(physical_addr offset, physical_addr addr) {
    return addr * PAGE_SIZE + offset;
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
uint64_t maxFrame(uint64_t depth, physical_addr current_addr) {
    uint64_t _max_frame = 1;
    if (depth < TABLES_DEPTH) {
        word_t temp_addr = 0;
        for (uint64_t i = 0; i < PAGE_SIZE; ++i) {
            PMread(current_addr * PAGE_SIZE + i, &temp_addr);
            if (temp_addr != 0) {
                _max_frame += maxFrame(depth + 1, (physical_addr) temp_addr);

            }
        }
    }
    return _max_frame;
}
// dudu : safe frame, is the frame we should enter to it, the next node.
void findFreeTable(uint64_t depth, physical_addr current_addr, temp_type &freeFrame ) {
    if (depth >= TABLES_DEPTH)
        return;
    physical_addr temp_addr = 0;
    for (uint64_t i = 0; i < PAGE_SIZE && (freeFrame == -1); ++i) {
        PMread(convert(i, current_addr), (word_t *) &temp_addr);
        if (temp_addr != 0 && current_addr != temp_addr) { //
            if (tableIsClear(temp_addr)) {
                freeFrame = temp_addr;
                PMwrite(convert(i, current_addr), 0);
                return;
            } else {
                findFreeTable(depth + 1, temp_addr, freeFrame);
            }
        }
    }
}

struct PageFrame {
    physical_addr father ;
    uint64_t pAddr;
    uint64_t pageNum;
    uint64_t weight;
};


/* 
    still not consider the paritity of the cell. (last element).
*/
void findMaxFrameWeight(uint64_t depth,struct PageFrame cur,struct PageFrame& max ) {
    // Take care of depth thing

    if (depth == TABLES_DEPTH - 1 ){
        cur.weight+= (cur.pageNum & 1)==1 ? WEIGHT_ODD : WEIGHT_EVEN;
        if(cur.weight > max.weight){
            std::cout<<"depth: "<< depth<<" frame number: "<< cur.pAddr<<" max weight: "<< cur.weight<<" max page track: "<< cur.pageNum<< std::endl;
            max.weight = cur.weight;
            max.pageNum = cur.pageNum;
            max.pAddr = cur.pAddr;
            max.father = cur.father;

        }
        return;
    }

    physical_addr temp_addr = 0;
    for (uint64_t i = 0; i < PAGE_SIZE; ++i) {

        PMread(convert(i,cur.pAddr), (word_t *) &temp_addr);

        if (temp_addr != 0) {
            std::cout<<"depth: "<< depth<<" frame number: "<< cur.pAddr<<" before reading: "<< temp_addr<<std::endl;
            cur.father = convert(i,cur.pAddr);
            cur.pAddr = temp_addr;
            cur.pageNum = (cur.pageNum << OFFSET_WIDTH) + i;
            cur.weight += (temp_addr & 1) == 1 ?  WEIGHT_ODD : WEIGHT_EVEN;
            std::cout<<"weight after reading: "<< cur.weight<<std::endl;
            findMaxFrameWeight(depth + 1, cur, max);

        }
    }

}


uint64_t evict() {
    log()

    struct PageFrame cur, max ;
    cur.pAddr = 0,cur.pageNum = 0,cur.weight = WEIGHT_EVEN,max.weight = 0,max.pAddr = 0, max.pageNum = 0 ;
    findMaxFrameWeight(0, cur, max);
    std::cout<< "evicting " << max.pageNum <<" with weight of " << max.weight <<std::endl;
    std::cout<< "evicting " << max.pAddr <<" num of pages " << NUM_PAGES <<std::endl;
    PMevict(max.pAddr, max.pageNum);

    clearTable(max.pAddr);
    
    // uint64_t i =  (max.pageNum << OFFSET_WIDTH)
    std::cout<< " max.father " << max.father << " num of pages " << NUM_PAGES <<std::endl;
    PMwrite(max.father , 0 );
    physical_addr temp_perent = max.pAddr / PAGE_SIZE;
    physical_addr temp_current = max.pAddr;
    log();
    while ( temp_perent > 0 ) {
        int flag = 0; 
        word_t temp_word;
        printf("[DEBUG] %d\n",  temp_perent);
        for ( int i = 0; i < PAGE_SIZE; ++i) {
            if ( convert(i, temp_perent) != temp_current ) {
                PMread( convert(i, temp_perent) , &temp_word );
                flag = flag | temp_word;
            }            
        }
        if (!flag) {
            PMwrite(temp_perent, 0 ); 
            temp_current = temp_perent;
            temp_perent /= PAGE_SIZE;
        }
        else{
            temp_perent = 0;
        }
    }

    // PMwrite(max.pAddr , 0);
    return max.pAddr;
}

int NewNode(physical_addr addr, uint64_t frame) {
    log()
    printf("[DEBUG] addr %d, frame : %d, \n", addr, frame);
    PMwrite(addr, frame);
    return 1;
}


struct AddressState search(Path offsets) {
    struct AddressState ret = {0, 0};
    word_t qal = 0;
    for (int i = 0; i < TABLES_DEPTH; i++) {
        

        //handle the case, the user ask for illegal addresses.
        if (convert(offsets.paths[i], ret.addr) >= RAM_SIZE ) {
            ret.addr = convert(offsets.paths[i], ret.addr);
            std::cout << " ret.addr " << ret.addr << "\n"; 
            ret.nextaddr = -1; 
            return ret;    
        }

        PMread(convert(offsets.paths[i], ret.addr), &qal);
        ret.depth = i;
        if (qal == 0) {
            log()
            // pathch, should remove.
            ret.addr = convert(offsets.paths[i], ret.addr);
            ret.nextaddr = -1;
            return ret;
        } else {
            ret.addr = qal;
        }
    }
    ret.addr = qal;
    return ret;
}


// void clean_path_to_root( uint64_t addr, uint64_t depth) {

//     if (addr == 0 )
//         return;
    
//     if ( depth < TABLES_DEPTH && tableIsClear( addr / PAGE_SIZE) ) {
        
//     }
//     clean_path_to_root( addr / PAGE_SIZE , depth - 1);
// }


// void clean(  uint64_t addr ) {
//     uint64_t perent = addr / PAGE_SIZE;
//     uint64_t offset = addr & (PAGE_SIZE - 1);


// }


// chaya mushka's func:
struct AddressState getPAddr(uint64_t virtualAddress) {
    log()
    physical_addr addr;
    struct Path path = get_path(virtualAddress);
    struct AddressState val = search(path);
    
    // should initalize to -1. BUG;
    for (; val.nextaddr == -1; val = search(path)) {
        
        // handle the case of illegal address.
        if (val.addr >= RAM_SIZE) {
            return val;
        }
    
        std::cout << "DEPTH:" <<val.depth << "\n";
        physical_addr emptyframe = -1;
        // after initialize new node, the new should point to it's perent
        
        // TODO : BUG where '(val.addr / PAGE_SIZE) = 0', excpected to infinty loop.   
        PMwrite(val.addr, val.addr / PAGE_SIZE); //TODO maybe no need?
        findFreeTable(0, 0, emptyframe);
        
        
        
        PMwrite(val.addr, 0); // TODO maybe no need?
        if (emptyframe == -1) {
            emptyframe = maxFrame(0, 0);
            if (emptyframe >= NUM_FRAMES) {
                emptyframe = evict();
                // PMwrite(emptyframe, 0);
                // PMwrite(val.addr, val.addr / PAGE_SIZE);
                // findFreeTable(0, 0, emptyframe);
                // PMwrite(val.addr, 0); // TODO maybe no need?

            }

            // continue;
        }
        // else {
        NewNode(val.addr, emptyframe);
        //     PMwrite(val.addr, val.addr / PAGE_SIZE );
        // }
        std::cout << "using frame: "<< emptyframe << "!\n";
        if(val.depth == (TABLES_DEPTH -1)){
            std::cout << "CALLING RESTORE for page: " << (virtualAddress >> OFFSET_WIDTH) << "\n";
            PMrestore(emptyframe, virtualAddress >> OFFSET_WIDTH);
        }
    }
    //chaya:
    return val;
}


int VMread(uint64_t virtualAddress, word_t *value) {
    struct AddressState val = getPAddr(virtualAddress);
    
    if (val.addr >= RAM_SIZE ){
        printf("[DEBUG] illegal address\n");    
        return 0;
    }

    printf("[DEBUG] VMread addr:  %d,\n", convert(virtualAddress & (PAGE_SIZE - 1), val.addr));

    

    // last step
    PMread(
            convert(virtualAddress & (PAGE_SIZE - 1), val.addr),
            (word_t *) value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {
    struct AddressState val = getPAddr(virtualAddress);

     
    if (val.addr >= RAM_SIZE ){
        printf("[DEBUG] illegal address\n");    
        return 0;
    }
    // last step
    PMwrite(
            convert(virtualAddress & (PAGE_SIZE - 1), val.addr), value);
    printf("[DEBUG] write addr:  %d, the val: %d\n", convert(virtualAddress & (PAGE_SIZE - 1), val.addr), value);


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



