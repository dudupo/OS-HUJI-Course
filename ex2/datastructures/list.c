#include "list.h"
#include "stdlib.h"




struct struct_list
{
    struct struct_list * next;
    int val; 
};

void push(t_list * p_list, int tid)
{
    if ( p_list == NULL )
    {
        p_list = ( t_list * ) malloc ( sizeof( t_list ) );
        p_list->next = NULL;
        p_list->val = tid;
    }
    else 
    {
        push(p_list->next, tid);
    }
}

int pop(t_list ** p_list)
{
    if (p_list == NULL || *p_list == NULL)
    {
        return OS_LIST_POP_ERROR;
    }
    int ret = (*p_list)->val;
    (*p_list) = (*p_list)->next;
    return ret; 
}
