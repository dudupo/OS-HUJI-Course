#ifndef OS_LIST
#define OS_LIST


const int OS_LIST_POP_ERROR = -1;

typedef struct struct_list t_list;

void push(t_list * p_list, int tid);
int pop(t_list ** p_list);


#endif
