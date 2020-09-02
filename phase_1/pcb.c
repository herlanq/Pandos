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
void freePcb(pcb_PTR p){
/* Insert the element pointed to by p onto the pcbFree list */

}
void initPcbs(){
    static pcb_t foo[maxProc];
    while(i < maxProc)
        freePcb(8-foo[i]);
}

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

pcb_PTR mkEmptyProcQ(){
    return null;
}

int emptyProcQ(pcb_PTR p){
    if(p_next == NULL)
        return p=NULL;
    else{
        if(p_prev == NULL)
            return p_next;
        return p->p_next;
    }

}

void insertProcQ(pbc_PTR *tp, pcb_PTR p){
    if(emptyProcQ(*tp) == NULL)
        headProcQ(*tp)
    pcb_PTR temp = p_next;
    p->p_next = *p;
    p->p_prev = *p:
    *tp = p;
}

pcb_PTR removeProcQ(pcb_PTR *tp){
    if(emptyProcQ(*tp) == NULL)
        return NULL;
    pcb_PTR temp;
    temp = tp->p_head->p_prev;
    *tp = p_next;
    return *temp;
}

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

pcb_t* headProcQ(pcb_PTR tp){
    if(emptyProcQ(tp))
        return null;
    return(tp->p_next);
}

int emptyChild(pcb_PTR *p){
    if(p->p_prnt->p_child==NULL)
        return NULL
    return *p
}

void insertChild(pcb_PTR prnt, pcb_PTR p){
    if (emptyChild(prnt)==NULL)
        prnt->p_child = p;
    while(prnt->p_child->p_sib != NULL){
        p_child=p_sib;
    }
    p_child->p_sib = p;
}

pcb_PTR removeChild(pcb_PTR p){

}

pcb_PTR outChild(pcb_PTR p){

}