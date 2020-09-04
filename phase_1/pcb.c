/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/3
 */

#include ".../h/const.h"
#include ".../h/types.h"
#include ".../h/pcb.h"

HIDDEN pcb_PTR pcbFree_h;

/* Insert the element pointed to by p onto the pcbFree list. */
void freePcb(pcb_PTR p){
    p->p_next = pcbFree_h;
    pcbFree_h = p;
}

/* Initialize the pcbFree list to contain all the elements of the static array of MAXPROC pcbs.
 * This method will be called only once during data structure initialization. */
void initPcbs(){
    static pcb_t pcbInit[maxProc];
    pcbFree_h = NULL;
    int i = 0;
    while(i < maxProc){
        freePcb(&pcbInit[i]);
    }
}

/* Return NULL if the pcbFree list is empty.
 * Otherwise, remove an element from the pcbFree list, provide initial values for ALL of the pcbs fields
 * (i.e. NULL and/or 0) and then return a pointer to the removed element.
 * pcbs get reused, so it is important that no previous value persist in a pcb when it gets reallocated. */
pcb_PTR allocPcb(){
    if(pcbFree_h == NULL){
        return NULL
    }
    pcb_PTR temp;
    temp = pcbFree_h;
    pcbFree_h = pcbFree_h->p_next;
    // initializing new pcb
    temp->p_next = NULL;
    temp->p_prev = NULL;
    temp->p_prnt = NULL;
    temp->p_child = NULL;
    temp->p_Lsib = NULL;
    temp->p_Rsib = NULL;

    return temp;
}

/* This method is used to initialize a variable to be tail pointer to a process queue.
Return a pointer to the tail of an empty process queue; i.e. NULL. */
pcb_PTR mkEmptyProcQ(){
    return null;
}

/* Return TRUE if the queue whose tail is pointed to by tp is empty.
Return FALSE otherwise. */
int emptyProcQ(pcb_PTR tp) {
    return (tp == NULL);
}

/* Insert the pcb pointed to by p into the process queue whose
 * tail- pointer is pointed to by tp.
 * Note the double indirection through tp to allow for
 * the possible updating of the tail pointer as well. */

void insertProcQ(pbc_PTR *tp, pcb_PTR p){
    /* case 1: empty */
    if(emptyProcQ(*tp) == NULL){
        p->p_next = p;
        p-p_prev = p;
    }else if(*tp->p_next == *tp){ /* case 2: only one node*/
        /* new node is assigned to the tail pointer and prev pointer
         * new previous and next is the new node */
        p->p_next = *tp;
        p->p_prev = *tp;
        *tp->p_prev = p;
        *tp->p_next = p;

    }else{ /* case 3: n nodes where n > 1 */
        /* new node is the head node
         * next pointer of the new node is the head
         * previous pointer of the head is the new node
         * p_prev of the new node points to the tail of the initial node*/
        pcb_PTR temp = *tp->p_next; //temp head node
        *tp->p_next = p;
        p->p_next = temp;
        p->p_prev = *tp;
        *tp = p;
    }

    *tp = p;
}

/* Remove the first (i.e. head) element from the process queue whose
 * tail-pointer is pointed to by tp. Return NULL if the process queue
 * was initially empty; otherwise return the pointer to the removed element.
 * Update the process queue’s tail pointer if necessary. */

pcb_PTR removeProcQ(pcb_PTR *tp) {
    if (emptyProcQ(*tp) == NULL) { // if queue is empty, return null
        return NULL;
    }else if (*tp->p_next == *tp){ // if there is only one node
        pcb_PTR *temp = *tp ;
        *tp = mkEmptyProcQ();
        return temp;
    }else{ // queue has n nodes where n > 1
        pcb_PTR temp = *tp->p_next;
        *tp->p_next = temp->p_next;
        temp->p_next->p_prev = *tp;
        return temp;
    }
}
/* Remove the pcb pointed to by p from the process queue whose tail- pointer is pointed to by tp.
 * Update the process queue’s tail pointer if necessary.
 * If the desired entry is not in the indicated queue (an error condition),
 * return NULL; otherwise, return p. Note that p can point to any element of the process queue. */
pcb_PTR outProcQ(pcb_PTR *tp, pcb_PTR p){
    if(emptyProcQ(*tp) == NULL) {
        return NULL;
    } else if(emptyProcQ(*tp->p_prev)==NULL){
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

/* Return a pointer to the first pcb from the process queue whose tail is pointed to by tp.
 * Do not remove this pcbfrom the process queue. Return NULL if the process queue is empty. */

pcb_t* headProcQ(pcb_PTR tp){
    if(emptyProcQ(tp) == NULL){
        return NULL;
    }
    return tp->p_next;
}

/* Return TRUE if the pcb pointed to by p has no children. Return
FALSE otherwise. */
int emptyChild(pcb_PTR p){
    return (p->p_child==NULL);
}

/* Make the pcb pointed to by p a child of the pcb pointed to by prnt.
*/
void insertChild(pcb_PTR prnt, pcb_PTR p){
    // p_sib = next, p_prevsib = prev
    if (emptyChild(prnt)){ // case 1: no children
        p_prnt->p_child = p;
        p->p_prnt = p_prnt;
        p->p_prevsib = NULL;
        p->p_sib = NULL;
    } // case 2: n children n >= 1
    p->p_prnt = p_prnt;
    p_prnt->p_child->p_prevsib = p;
    p->p_sib = p_prnt->p_child;
    p->p_prevsib = NULL;
    p_prnt->p_child =p;
}


/* Make the first child of the pcb pointed to by p no longer a child of p.
 * Return NULL if initially there were no children of p.
 * Otherwise, return a pointer to this removed first child pcb. */
pcb_PTR removeChild(pcb_PTR p){
    if(emptyChild(p)==NULL){ // no children
        return NULL;
    }else if(p->p_child->p_sib == NULL) { // one child


    }else { // children n > 1

    }

}
/* Make the pcb pointed to by p no longer the child of its parent.
 * If the pcb pointed to by p has no parent, return NULL; otherwise, return p.
 * Note that the element pointed to by p need not be the first child of its parent. */
pcb_PTR outChild(pcb_PTR p){
    if(p == NULL){
        return NULL;
    }
    if(p->p_prnt == NULL){
        return NULL;
    }
    // one child
    if(p_prevsib == NULL && p->p_sib == NULL && p== p->prnt->p_child){
        p->p_prnt->p_child = NULL;
        p->p_prnt = NULL;
        return p;
    }
    // first child
    if(){

    }
    // last child
    if(){

    }
    // middle
    if(){

    }
}