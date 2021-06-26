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

inline physical_addr INIT_PAGE_SIZE() {
    return  VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH ? VIRTUAL_ADDRESS_WIDTH % OFFSET_WIDTH : OFFSET_WIDTH;
}
inline physical_addr convert(physical_addr offset, physical_addr addr, int depth) {
    return depth ?  addr* PAGE_SIZE + offset : addr * (1LL << INIT_PAGE_SIZE()) + offset; 
}
inline physical_addr get_neighbors( physical_addr addr, int depth ){
    return depth ? PAGE_SIZE : (1LL << INIT_PAGE_SIZE());
} 

bool tableIsClear(uint64_t frameIndex, int depth) {
    word_t val = 0;

    for (uint64_t i = 0; i < get_neighbors(frameIndex, depth); ++i) {
        if (  convert(i ,frameIndex, depth) < RAM_SIZE )
        {
            PMread( convert(i ,frameIndex, depth), &val);
            if (val != 0) {
                return false;
            }
        }
    }
    return true;
}


void clearTable(uint64_t frameIndex, int depth) {
    for (uint64_t i = 0; i < get_neighbors(frameIndex, depth); ++i) {
        if ( convert(i ,frameIndex, depth) < RAM_SIZE )
        {
            word_t temp; 
            PMread( convert(i, frameIndex, depth), &temp);
            if (temp != 0) {
                clearTable( temp, depth+1 );
            }
            PMwrite( convert(i, frameIndex, depth), 0);
        }
    }
}

void VMinitialize() {
    clearTable(0, 0);
    for ( int i = 1; i < RAM_SIZE; i++) {
        clearTable(i, 1);
    }
}

struct Path {
    physical_addr paths[TABLES_DEPTH + 1];
};


struct Path get_path(uint64_t virtualAddress) {

    struct Path ret;
    virtualAddress >>= OFFSET_WIDTH;
    for (int i = 0; i < TABLES_DEPTH-1; virtualAddress >>= OFFSET_WIDTH) {
        ret.paths[TABLES_DEPTH - i-1] =  virtualAddress & (PAGE_SIZE - 1);
        i++;
    }
    ret.paths[0] = virtualAddress & (  (1LL << INIT_PAGE_SIZE()) - 1);
    
    #ifdef PRINT_PATH 
    for (int i = 0; i < TABLES_DEPTH; ++i) {
        std::cout << ret.paths[i] << "\n";
    }
    #endif
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
uint64_t maxFrame(uint64_t depth, physical_addr current_addr) {
    uint64_t _max_frame = 1;
    if (depth < TABLES_DEPTH) {
        word_t temp_addr = 0;
        for (uint64_t i = 0; i < get_neighbors(current_addr, depth); ++i) {
            PMread( convert(i, current_addr, depth) , &temp_addr);
            if (temp_addr != 0) {
                _max_frame += maxFrame(depth + 1, (physical_addr) temp_addr);

            }
        }
    }
    return _max_frame;
}
void findFreeTable(uint64_t depth, physical_addr current_addr, temp_type &freeFrame ) {
    // log()
    if (depth >= TABLES_DEPTH)
        return;
    // PMread( max.pAddr, &t );
    // t = 0;
    
    physical_addr temp_addr = 0;
    for (uint64_t i = 0; i < get_neighbors(current_addr, depth) && (freeFrame == -1); ++i) {
        PMread(convert(i, current_addr, depth), (word_t *) &temp_addr);
        if (temp_addr != 0 && current_addr != temp_addr) { //
            if (tableIsClear(temp_addr, depth+1)) {
                freeFrame = temp_addr;
                PMwrite(convert(i, current_addr, depth), 0);
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
    uint64_t pageNum = 0;
    uint64_t weight;
    uint64_t depth = 0;
};


/* 
    still not consider the paritity of the cell. (last element).
*/
void findMaxFrameWeight(uint64_t depth,struct PageFrame cur,struct PageFrame& max ) {
    // Take care of depth thing

    if (depth == TABLES_DEPTH ){

        // cur.pageNum = cur.pageNum >> 1;
        cur.weight+= (cur.pageNum & 1)==1 ? WEIGHT_ODD : WEIGHT_EVEN;
        if(cur.weight > max.weight){
            // std::cout<<"depth: "<< depth<<" frame number: "<< cur.pAddr<<" max weight: "<< cur.weight<<" max page track: "<< cur.pageNum<< std::endl;
            max.weight = cur.weight;
            max.pageNum = cur.pageNum;
            max.pAddr = cur.pAddr;
            max.father = cur.father;
            max.depth = depth;

        }
        return;
    }

    physical_addr temp_addr = 0;
    
    for (uint64_t i = 0; i < get_neighbors(cur.pAddr, depth); ++i) {

        PMread(convert(i,cur.pAddr, depth), (word_t *) &temp_addr);
        struct PageFrame cur2;
        if (temp_addr != 0) {
            // std::cout<<"depth: "<< depth<<" frame number: "<< cur.pAddr<<" before reading: "<< temp_addr<<std::endl;            
            cur2.father = convert(i,cur.pAddr, depth); //cur2.pAddr; // convert(i,cur.pAddr, depth);
            cur2.pAddr = temp_addr;
            cur2.pageNum =  convert(i,cur.pageNum, depth) ;
            cur2.weight = cur.weight;
            cur2.weight += (((temp_addr & 1) == 1) ?  WEIGHT_ODD : WEIGHT_EVEN);
            // std::cout<<"weight after reading: "<< cur2.weight<<std::endl;
            findMaxFrameWeight(depth + 1, cur2, max);

        }
    }

}


physical_addr get_perent( physical_addr addr ) {
    if (addr < ( (1LL<< INIT_PAGE_SIZE()) * PAGE_SIZE) )
    {
        // return addr / INIT_PAGE_SIZE
    }
}

uint64_t evict() {
    // log()

    struct PageFrame cur, max ;
    cur.pAddr = 0,cur.pageNum = 0,cur.weight = WEIGHT_EVEN,max.weight = 0,max.pAddr = 0, max.pageNum = 0 ;
    findMaxFrameWeight(0, cur, max);    
    
    // word_t t;
    // PMread( max.pAddr, &t );
    // t = 0;
    
    PMwrite(max.father , 0 );
    // if ( max.pAddr < NUM_FRAMES)
    std::cout << "-- EVICATE -- " << max.pAddr << "," << max.pageNum << "\n";
    std::cout << RAM_SIZE << "\n";
    for (int i = 0; i < PAGE_SIZE; ++i)
    {
        PMwrite( max.pAddr * PAGE_SIZE + i , 0 );
    }
    // clearTable(max.pAddr, max.depth);
    PMevict(max.pAddr, max.pageNum);
    // clearTable(max.father ,max.depth);

    physical_addr temp_perent = max.father  / PAGE_SIZE;
    physical_addr temp_current = 0;
    log();

    int depth = max.depth-1; 

    while ( depth > 0 ) {
        std::cout << temp_perent << std::endl;
        std::cout << "depth = " << depth << std::endl;
        int flag = 0; 
        word_t temp_word;
        for ( int i = 0; i < PAGE_SIZE; ++i) {
            // if ( convert(i, temp_perent, depth) != temp_current ) {
            
            
            PMread( convert(i, temp_perent, depth) , &temp_word );            
            std::cout <<  temp_perent  << ", " << temp_current << ", " <<\
              convert(i, temp_perent, depth) << ", " << temp_word << "\n";
                flag = flag | temp_word;
        }
        if (!flag) {
            PMwrite(temp_perent, 0 ); 
            temp_current = temp_perent;
            
            depth--;
            temp_perent /= get_neighbors(temp_perent, depth);
            
        }
        else {
            return max.pAddr;        
        }
    }
    return max.pAddr;
}

int NewNode(physical_addr addr, uint64_t frame) {
    PMwrite(addr,  frame );
    return 1;
}


struct AddressState search(Path offsets) {
    struct AddressState ret = {0, 0};
    word_t qal = 0;
    for (int i = 0; i < TABLES_DEPTH; i++) {
        
        ret.depth = i;


        //handle the case, the user ask for illegal addresses.
        if (convert(offsets.paths[i], ret.addr, i) >= RAM_SIZE ) {
            ret.nextaddr = -1; 
            ret.addr = convert(offsets.paths[i], ret.addr, i);
            return ret;    
        }

        PMread(convert(offsets.paths[i], ret.addr, i), &qal);
        // std::cout << "next:" << qal << ", depth " <<  i <<  "\n"; 
        ret.depth = i;
        if (qal == 0) {
            // pathch, should remove.
            ret.addr = convert(offsets.paths[i], ret.addr, i);
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
    physical_addr addr;
    struct Path path = get_path(virtualAddress);
    struct AddressState val = search(path);
    
    for (; val.nextaddr == -1; val = search(path)) {
        
        // handle the case of illegal address.
        if (val.addr >= RAM_SIZE) {
            return val;
        }
    
        std::cout << "DEPTH:" <<val.depth << ", addr : " << val.addr << " \n";
        physical_addr emptyframe = -1;
        // after initialize new node, the new should point to it's perent
        
        // TODO : BUG where '(val.addr / PAGE_SIZE) = 0', excpected to infinty loop.   
        PMwrite(val.addr, val.addr / PAGE_SIZE); //TODO maybe no need?
        findFreeTable(0, 0, emptyframe);
        
        
        PMwrite(val.addr, 0); // TODO maybe no need?
        if (emptyframe == -1) {
            emptyframe = maxFrame(0, 0);
            if (emptyframe  >= NUM_FRAMES) {
                emptyframe = evict();
            }
            // NewNode(val.addr, emptyframe);
            // continue;
        }
        NewNode(val.addr, emptyframe);
        
        // else {
        //     PMwrite(val.addr, val.addr / PAGE_SIZE );
        // }
        // std::cout << "using frame: "<< emptyframe << " " << NUM_FRAMES << "!\n";
        if(val.depth == (TABLES_DEPTH -1)){
            std::cout << "val.addr " << val.addr;
            std::cout << "CALLING RESTORE for page: " << (virtualAddress >> OFFSET_WIDTH) << "\n";
            if (emptyframe >= NUM_FRAMES){
                std::cout << "CALLING RESTORE for page: jajjaaj" << "\n";
            }
            PMrestore(emptyframe, virtualAddress >> OFFSET_WIDTH);
        }
    }
    //chaya:
    return val;
}


int VMread(uint64_t virtualAddress, word_t *value) {

    if ( (virtualAddress >= ( VIRTUAL_MEMORY_SIZE << WORD_WIDTH)) )
        return 0;
    

    struct AddressState val = getPAddr(virtualAddress);
    //(virtualAddress >= VIRTUAL_MEMORY_SIZE) || 
    if ( (virtualAddress >= VIRTUAL_MEMORY_SIZE) || 
        (convert(virtualAddress & (PAGE_SIZE - 1), val.addr , val.depth) >= RAM_SIZE )){
        printf("[DEBUG] illegal address %d -> %d \n",virtualAddress, convert(virtualAddress & (PAGE_SIZE - 1), val.addr, val.depth));    
        return 0;
    }

    printf("[DEBUG] VMread addr:  %d,\n", convert(virtualAddress & (PAGE_SIZE - 1), val.addr, val.depth));

    

    // last step
    PMread(
            convert(virtualAddress & (PAGE_SIZE - 1), val.addr, val.depth),
            (word_t *) value);

    return 1;
}

int VMwrite(uint64_t virtualAddress, word_t value) {

    std::cout << value << "\n";
    if ( virtualAddress  >= ( VIRTUAL_MEMORY_SIZE << WORD_WIDTH)   )
        return 0;
    
    struct AddressState val = getPAddr(virtualAddress);

     
    if (  (virtualAddress >= VIRTUAL_MEMORY_SIZE) || 
    (convert(virtualAddress & (PAGE_SIZE - 1), val.addr, val.depth) >= RAM_SIZE )){
        printf("[DEBUG] illegal address\n");    
        return 0;
    }
    // last step
    PMwrite(
            convert(virtualAddress & (PAGE_SIZE - 1), val.addr, val.depth), value);
    printf("[DEBUG] write addr:  %d, the val: %d\n", convert(virtualAddress & (PAGE_SIZE - 1), val.addr, val.depth), value);


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



