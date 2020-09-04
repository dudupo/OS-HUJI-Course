#include "list.h"
#include "stdlib.h"
#include "../p_uthreads.h"



list* push(list ** p_list, p_uthreads * p_obj)
{
    if ( *p_list == NULL )
    {
        *p_list = ( list * ) malloc ( sizeof( list ) );
        (*p_list)->next = NULL;
        (*p_list)->val = p_obj;

        return *p_list;
    }
    else 
    {
        push(&((*p_list)->next), p_obj);
    }
}

list* pushnode(list ** p_list, list* node)
{
    /*
        I EXCEPT HERE TO CRITICAL PROBLEM.
        I should to ensure if the addresss of the new 
        *p_list is equal to node, in case I fail here,
        I should to insert the node as next sibling.
        *plist->next = node.  
        and then there will be another endcase.. when the list is empty.
    */


    if ( *p_list == NULL )
    {
        *p_list = node;
        (*p_list)->next = NULL;
        return *p_list;
    }
    else 
    {
        pushnode(&((*p_list)->next), node);
    }
}

list* pop( list ** p_list)
{
    if (p_list == NULL || *p_list == NULL)
    {
        return NULL;
    }
    list * ret = *p_list;
    (*p_list) = (*p_list)->next;
    return ret; 
}

p_uthreads * get(list * node, int tid)
{
    if (node == NULL)
        return NULL;
    
    return NULL;
}

