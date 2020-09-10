/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/10
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/pcb.h"
#include "../h/asl.h"

semd_t *semdFree_h; /* pointer to head of free semaphore list */
semd_t *semd_h; /* pointer to head of active semaphore list */

/* Returns a pointer to the pcb that is at the head of the process queue associated with
 * the semaphore semAdd. Return NULL if semAdd is not found on the ASL
 * or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){
    semd_t *temp;
    temp = search(semAdd);  /* uses search helper function to find node corresponding with semAdd*/
    if(temp == NULL || emptyProcQ(temp->s_next->s_procQ)){
        return NULL;
    }else{
        return headProcQ(temp->s_next->s_procQ);
    }
}

/* Initialize the semdFree list to contain all the elements of the array
as static semd t semdTable[MAXPROC+2], the first and last nodes are used as dummy nodes */
void initASL(){
    static semd_t semdTable[MAXPROC+2]; /* includes two dummy nodes*/
    semd_h = NULL;
    semdFree_h = NULL;
    int i;
    for(i=2; i < (MAXPROC+2); i++){
        freeSemd(&(semdTable[i]));
    }
    /* initializing the first and last nodes as dummy nodes */
    semd_t *first;
    first = &(semdTable[0]);
    semd_t *last;
    last = &(semdTable[1]);
    /* init first dummy node */
    first->s_semAdd = 0;         /* set first node's semAdd = NULL*/
    first->s_procQ = NULL;
    first->s_next = last;
    /* init last dummy node */
    last->s_semAdd = (int*) MAXINT; /* set last node's semAdd = maxint*/
    last->s_procQ = NULL;
    last->s_next = NULL;

    semd_h = first;
}

/*          Additional function         */
/* Similar to allocPcb,
 * Function used to allocate space in ASL
 */
semd_t* semAlloc(){
    if(semdFree_h == NULL){
        return NULL;
    }
    semd_t *temp = semdFree_h;
    semdFree_h = semdFree_h->s_next;
    return temp;
}

/* Inserts the pcb pointed to by p at the tail of the process queue associated with the semaphore whose physical address
 * is semAdd and sets the semaphore address of p to semAdd.
 * If the semaphore is currently not active, a new descriptor from the semdFree list is allocated and
 * inserted in the ASL.
 * If a new semaphore descriptor needs to be allocated and the semdFree list is empty, return TRUE.
 * In all other cases return FALSE. */
int insertBlocked(int *semAdd, pcb_t *p) {
    semd_t *temp;
    temp = (semd_t*) search(semAdd);
    if (temp->s_next->s_semAdd != semAdd) {
        semd_t *new = semAlloc();
        if (new == NULL) {
            return TRUE;
        } else {
            new->s_next = temp->s_next;
            temp->s_next = new;
            new->s_procQ = mkEmptyProcQ();

            p->p_semAdd = semAdd;
            new->s_semAdd = semAdd;
            insertProcQ(&(new->s_procQ), p);
            return FALSE;
        }
    } else {
        p->p_semAdd = semAdd;
        insertProcQ(&(temp->s_next->s_procQ), p);
        return FALSE;
    }
}

/* Searches the ASL for a descriptor of this semaphore. If none is found, returns NULL;
 * otherwise, removes the head pcb from the process queue of the found semaphore
 * descriptor and returns a pointer to it. If the process queue for this semaphore becomes
 * empty (emptyProcQ(s procq) is TRUE), removes the semaphore descriptor from the
 * ASL and returns it to the semdFree list. */
pcb_t *removeBlocked(int *semAdd){
    semd_t *node;
    pcb_t* returnVal;
    node = search(semAdd);
    if(node->s_next->s_semAdd == semAdd) {
        returnVal = removeProcQ(&node->s_next->s_procQ); /* return value is equal to a pointer to the head pcb */
        if (emptyProcQ(node->s_next->s_procQ)) {
            semd_t *removed = node->s_next;
            node->s_next = node->s_next->s_next;
            freeSemd(removed);
            returnVal->p_semAdd = NULL;
            return returnVal;
        } else {
            returnVal->p_semAdd = NULL;
            return returnVal;
        }
    }else{
        return NULL;
    }
}

/* Remove the pcb pointed to by p from the process queue associated with p’s
 * semaphore (p→ p semAdd) on the ASL.
 * If pcb pointed to by p does not appear in the process queue associated with p’s semaphore,
 * which is an error condition, return NULL; otherwise, return p. */
pcb_t *outBlocked(pcb_t *p){
    semd_t *node;
    pcb_t* returnVal;
    node = search(p->p_semAdd);
    if(node->s_next->s_semAdd == p->p_semAdd) {
        returnVal = outProcQ(&node->s_next->s_procQ, p); /* return value is equal to a pointer to the head pcb */
        if (emptyProcQ(node->s_next->s_procQ)) {
            semd_t *removed = node->s_next;
            node->s_next = node->s_next->s_next;
            freeSemd(removed);
            return returnVal;
        } else {
            returnVal->p_semAdd = NULL;
            return returnVal;
        }
    }else{
        return NULL;
    }
}

/*                                   Other Additional Functions                                           */

/* Functions used for code writing efficiency and to make the code easier to follow */

/* Similar to freePcb, like allocASL function above
 * Function used to deallocate values in ASL
 * adds nodes to semdFree list
 */
void freeSemd(semd_t *semd) {
    if (semdFree_h == NULL) {
        semd->s_next = NULL;
        semdFree_h = semd;
        
    } else {
        semd->s_next = semdFree_h;
        semdFree_h = semd;
    }
}

/* goes through asl to determine if next node has semdAdd == parameter semAdd
 * if semAdd is found returns address of semAdd,compares whether the semAdd is greater than
 * next semAdd in order to keep the list sorted
 */
semd_t *search(int *semAdd){
    semd_t *temp = semd_h->s_next;  /*added reference to semd_t*/
    semd_t *lag_temp = semd_h;
    if(temp->s_semAdd == (int*) MAXINT){
            return semd_h;
        }
    while(semAdd >= temp->s_semAdd){
        if(temp->s_semAdd == semAdd)
        {
            return lag_temp;
        }
        lag_temp = temp;
        temp = temp->s_next;
    }
    return lag_temp;
}