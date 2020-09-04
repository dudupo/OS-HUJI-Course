#ifndef OS_LIST
#define OS_LIST

#include "../p_uthreads.h"

struct struct_list
{
    struct struct_list * next;
    p_uthreads * val; 
};

typedef struct struct_list list;

list* push(list **, p_uthreads *);
list* pushnode(list **, list *);

list* pop(list ** );

// temporary implemantion.
p_uthreads * get(list*, int );



#endif
