#include "list.h"
#include "stdlib.h"
#include "../p_uthreads.h"



list* push(list ** p_list, p_uthreads * p_obj)
{

    list* new_p_list = ( list * ) malloc ( sizeof( list ) );
    new_p_list->next = *p_list ;
    new_p_list->val = p_obj;

    *p_list = new_p_list;

    return *p_list;
    
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
    node->next = *p_list;
    *p_list = node;
    return *p_list;
    
}

list* pop( list ** p_list)
{
    if (p_list == NULL || *p_list == NULL)
    {
        return NULL;
    }
    list * ret = *p_list;

    if ( (*p_list)->next == NULL )
    {
        *p_list = NULL; 
    }
    else
    {
        *p_list = (*p_list)->next;
    }
    return ret; 
}

p_uthreads * get(list * node, int tid)
{
    if (node == NULL)
        return NULL;
    
    return NULL;
}

