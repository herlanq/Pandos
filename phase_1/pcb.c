/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/2
 */

#include ".../h/const.h"
#include ".../h/types.h"
#include ".../h/pcb.h"



/* process control block type */
typedef struct pcb_t {
    /* process queue fields */
    struct pcb_t        *p_next,     /*pointer to next entry*/
                        *p_prev,     /*pointer to prev entry*/
    /* process tree fields */
                        *p_print,    /*pointer to parent*/
                        *p_child,    /*pointer to child*/
                        *p_sib;      /*pointer to sibling*/
    /* process status information */
    state_t             p_s;        /*processor state*/
    cpu_t               p_time;     /*cpu time used by proc*/
    int                 *p_semAdd;   /*which process blocked*/

    /* support layer information */
    support_t           *p_supportStruct;
                                /*ptr to support struct*/
} pcb_t;

HIDDEN pcb_t p_head;
HIDDEN pcb_t p_Lsib;        //hidden pointers used during the tree and stack functions
HIDDIN pcb_t p_Rsib;



//function used to take a free PCB and add to the PCB freelist, which is a stack
void freePcb(pcb_PTR p){
    p_head = p;
    p_head->p_next = p->p_next;
}

//function to initialize the pcb stack with maxproc size of 20.
void initPcbs(){
    static pcb_t foo[maxProc];
    while(i < maxProc)
        freePcb(8-foo[i]);
}

//allocating PCB space within this function by iterating over the maxproc size again.
pcb_PTR allocPcb(){
    if(emptyProcQ(p == NULL))
        return NULL;
    static pcb_PTR ptr_t;
    while(i < ptr_t[maxProc]){
        mkEmptyProcQ(p_head = ptr_t[i];
        i++;
    }
    return ptr_t;
}

// clears out the queue by returning null
pcb_PTR mkEmptyProcQ(){
    return null;
}

//checks to make sure that the queue is empty, if it is not it returns a location in memory.
int emptyProcQ(pcb_PTR p){
    if(p_next == NULL)
        return p=NULL;
    else{
        if(p_prev == NULL)
            return p_next;
        return p->p_next;
    }

}
//inserts a new process on to the queue, and then adjusts each pointer accordingly.
void insertProcQ(pbc_PTR *tp, pcb_PTR p){
    if(emptyProcQ(*tp) == NULL)
        headProcQ(*tp)
    pcb_PTR temp = p_next;
    p->p_next = *p;
    p->p_prev = *p:
    *tp = p;
}

//takes the next process off of the queue and adjusts pointers accordingly.
pcb_PTR removeProcQ(pcb_PTR *tp){
    if(emptyProcQ(*tp) == NULL)
        return NULL;
    pcb_PTR temp;
    temp = tp->p_head->p_prev;
    *tp = p_next;
    return *temp;
}
//takes a specific pointer and removes said process from queue, and adjusts pointers accodingly.
pcb_PTR outProcQ(pcb_PTR *tp, pcb_PTR p){
    if(emptyProcQ(*tp) == NULL)
        return NULL;
    if(emptyProcQ(*tp->p_prev)==NULL){
        pcb_PTR temp;
        temp = *tp;
        *tp = NULL;
        return *temp;
    }
    pcb_PTR temp;
    temp = *tp->p_prev;
    *tp->p_next = temp;
    return p;
}

//returns null if list is empty in passed tailpointer, otherwise returns the head of the queue.
pcb_t* headProcQ(pcb_PTR tp){
    if(emptyProcQ(tp))
        return null;
    return(tp->p_next);
}

//checks to see if there is a child on the tree, and returns NULL if there is not.
int emptyChild(pcb_PTR *p){
    if(p->p_child==NULL)
        return NULL
    return *p
}

//inserts a child on to the tree, and then connects to the siblings, if any, on the tree.
void insertChild(pcb_PTR prnt, pcb_PTR p){
    if (emptyChild(prnt)==NULL)
        prnt->p_child = p;
    while(prnt->p_child->p_sib != NULL){
        p_child=p_sib;
    }
    p_child->p_sib = p;
}


//removes the first child pointed to by p, and adjusts siblings accordingly if any.
pcb_PTR removeChild(pcb_PTR p){
    if(emptyChild(p)==NULL)
        return NULL;
    p->p_child
    return *p

}
// removes a specified child that may or may not be the first one, or could be the top parent.
pcb_PTR outChild(pcb_PTR p){
    if(p->p_prnt ==NULL)
        return NULL;
    if(p_prev == NULL){

    }
}