/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/4
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
#include "../h/asl.h"

semd_t *semdFree_h; /* pointer to head of free semaphore list */
semd_t *semd_h; /* pointer to head of active semaphore list */

/* Return a pointer to the pcb that is at the head of the process queue associated with
 * the semaphore semAdd. Return NULL if semAdd is not found on the ASL
 * or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){
    semd_t *temp;
    temp = search(semAdd);  /* uses search helper function to find corresponding node */
    if(temp == NULL || temp->s_procQ == NULL){
        return NULL;
    }else{
        return headProcQ(temp->s_procQ);
    }
}

/* Initialize the semdFree list to contain all the elements of the array
static semd t semdTable[MAXPROC] */
void initASL(){
    static semd_t semdTable[MAXPROC+2]; /* includes two dummy nodes*/
    semd_h = &semdTable[0];
    semd_h->s_next = &semdTable[1];
    semd_h ->s_semAdd =0;
    semd_h -> s_next ->s_semAdd = MAXINT;
    semd_h-> s_procQ = mkEmptyProcQ();
    semd_h ->s_next->s_procQ =mkEmptyProcQ();
    semd_h->s_next->s_next=NULL;

    semdFree_h = &semdTable[2];
    int i = 3;
    while(i < (MAXPROC+2)){
        freeSemd(&(semdTable[i]));
        i++;
    }
    return;
    /* setting the first and last nodes as dummy nodes 
    semd_t *first;
    semd_t *last;
    
    /* init last dummy node 
    last = &(semdTable[1]);
    last->s_semAdd = (int*) MAXINT; /* set last node's semAdd = maxint
    last->s_procQ = NULL;
    last->s_next = NULL;
    /* init first dummy node 
    first = &(semdTable[0]);
    first->s_semAdd = NULL;         /* set first node's semAdd = NULL
    first->s_procQ = NULL;
    first->s_next = last;
    

    semd_h = first; */
}

/* Insert the pcb pointed to by p at the tail of the process queue associated with the semaphore
 * whose physical address is semAdd and set the semaphore address of p to semAdd.
 * If the semaphore is currently not active (i.e. there is no descriptor for it in the ASL),
 * allocate a new descriptor from the semdFree list, insert it in the ASL (at the appropriate position),
 * initialize all of the fields (i.e. set s semAdd to semAdd, and s procq to mkEmptyProcQ()),
 * and proceed as above.
 *
 * If a new semaphore descriptor needs to be allocated and the semdFree list is empty,
 * return TRUE. In all other cases return FALSE. */
int insertBlocked(int *semAdd, pcb_t *p) {
    /*semd_t *temp;
    temp = search(semAdd);
    if(temp->s_next->s_semAdd != semAdd)
    {
        semd_t *alloc_sem = allocSemd();
        if(alloc_sem == NULL)
            return TRUE;
        semd_t *semdprev = search(alloc_sem->s_semAdd);
        semd_t *semdnext = semdprev->s_next;
        semdprev->s_next = alloc_sem;
        alloc_sem->s_next = semdnext;
        insertProcQ(&(alloc_sem->s_procQ), p);
        return FALSE;
    }
    insertProcQ(&(temp->s_next->s_procQ), p);
    return FALSE;
}*/







    semd_t *temp;
    p->p_semAdd = semAdd;
    if (temp->s_next->s_semAdd == semAdd){
        insertProcQ(&(temp->s_next->s_procQ), p);
        return FALSE;
    }
    else if(semdFree_h == NULL){
        return TRUE;
    }    
    else {
        semd_t *new = allocSemd();
        semd_t *semdprev = search(new->s_semAdd);
        semd_t *semdnext = semdprev->s_next;
        semdprev->s_next = new;
        new->s_next = semdnext;
        insertProcQ(&(new->s_procQ), p);
        return FALSE;
    }
    
}

/* Search the ASL for a descriptor of this semaphore. If none is found, return NULL;
 * otherwise, remove the first (i.e. head) pcb from the process queue of the found semaphore
 * descriptor and return a pointer to it. If the process queue for this semaphore becomes
 * empty (emptyProcQ(s procq) is TRUE), remove the semaphore descriptor from the
 * ASL and return it to the semdFree list. */
pcb_t *removeBlocked(int *semAdd){
    semd_t *node;
    node = search(semAdd);
    if(node->s_next->s_semAdd != semAdd)
        return NULL;
    pcb_PTR r_pcb = removeProcQ(&node->s_next->s_procQ);
    if(emptyProcQ(&node->s_next->s_procQ)){
        semd_t *temp = node->s_next;
        node->s_next = temp->s_next;
        temp->s_next = NULL;
    }
    return r_pcb;

}







/*
    semd_t *node;
    pcb_t* returnVal;
    node = search(semAdd);
    if(node->s_next->s_semAdd == semAdd){
        returnVal = removeProcQ(&node->s_next->s_procQ);  return value is equal to a pointer to the head pcb 
        return returnVal;
    }
    else if (emptyProcQ(node->s_next->s_procQ)) {
            semd_t *removed = node->s_next;
            node->s_next = node->s_next->s_next;
            freeSemd(removed);
            removed->s_semAdd = NULL;
            removed->s_procQ = NULL;
            return NULL;
        } 
    else{
        return NULL;
    }
}*/

/* Remove the pcb pointed to by p from the process queue associated with p’s
 * semaphore (p→ p semAdd) on the ASL.
 * If pcb pointed to by p does not appear in the process queue associated with p’s semaphore,
 * which is an error condition, return NULL; otherwise, re- turn p. */
pcb_t *outBlocked(pcb_t *p){
    semd_t *node;
    node = search(p->p_semAdd);
    pcb_t *returnVal;
    if(node == NULL){
        return NULL;
    }
    if(node->s_next->s_semAdd == p->p_semAdd){
        returnVal = outProcQ(&(node->s_next->s_procQ), p);
        if(emptyProcQ(node->s_next->s_procQ)){
            semd_t *removed;
            removed = node->s_next;
            node->s_next = node->s_next->s_next;
            freeSemd(removed);
            removed->s_semAdd = NULL;
        }
        returnVal->p_semAdd = NULL;
        return returnVal;
    }else{
        return NULL;
    }

}

/*                                   Additional Functions                                           */

/* Functions used for code writing efficiency and to make the code easier to follow */

/* Similar to allocPcb
 * Function used to allocate values in ASL
 * sets node pointer values to semd_t
 */
semd_t *allocSemd(){
    if(semdFree_h == NULL){ /* if already free */
        return NULL;
    }
    semd_t *temp;
    temp = semdFree_h;
    semdFree_h = semdFree_h->s_next;
    temp->s_next = NULL;
    temp->s_semAdd = NULL;
    temp->s_procQ = mkEmptyProcQ();
    return temp;
}

/* Similar to freePcb, like allocASL function above
 * Function used to deallocate values in ASL
 * adds nodes to semdFree list
 */
void freeSemd(semd_t *semd){
    if(semdFree_h == NULL){
        semdFree_h = semd;
        return;
    }else{
        semd->s_next = semdFree_h;
        semdFree_h = semd;
    }
}

/* goes through asl to determine if next node has semdAdd == parameter semAdd
 * if semAdd is found returns address of semAdd,compares whether the semAdd is greater than
 * next semAdd in order to keep the list sorted
 */

semd_t *search(int *semAdd){
    /*
    semd_t *temp = semd_h;
    semd_t *lagtemp;
    while(temp->s_next->s_semAdd < semAdd){
        lagtemp = temp;
        temp = temp->s_next;
    }
    return lagtemp->s_next;
*/



    semd_t *temp = semd_h;
    while (semAdd > temp->s_next->s_semAdd){
        temp = temp->s_next;
    }
    return temp; 
}