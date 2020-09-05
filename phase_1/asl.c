/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/4
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/pbc.h"
#include "../h/asl.h"

semd_t *semdFree_h; /* defines free semaphore list */
semd_t *semd_h /* defines active semaphore list */

/* Return a pointer to the pcb that is at the head of the process queue associated with
 * the semaphore semAdd. Return NULL if semAdd is not found on the ASL
 * or if the process queue associated with semAdd is empty. */
pcb_t *headBlocked(int *semAdd){
    semd_t *temp;
    temp = getParent(semAdd); /* gets the parent node */
    return headProcQ(temp->s_next->s_procQ);
}

/* Initialize the semdFree list to contain all the elements of the array
static semd t semdTable[MAXPROC]
This method will be only called once during data structure initialization. */
void initASL(){
    static semd_t ASLInit[MAXPROC+2]; /* includes two dummy nodes*/
    semdFree_h = NULL;
    if(int i=2; i<MAXPROC+2; i++){
        deallocASL(&ASLInit[i]);
    }
    /* setting the first and second nodes as dummy nodes */
    semd_t *first;
    first = &ASLInit[0];
    semd_t *last;
    last = &ASLInit[1];

    first->s_semAdd = NULL;
    last->s_semAdd = (int*) MAXINT;
    first->s_next = last;
    last->s_next = NULL;
    first->s_procQ = NULL;
    last->s_procQ = NULL;

    semd_h = first;
}

/* Insert the pcb pointed to by p at the tail of the process queue associated with the semaphore
 * whose physical address is semAdd and set the semaphore address of p to semAdd.
 * If the semaphore is cur- rently not active (i.e. there is no descriptor for it in the ASL),
 * allocate a new descriptor from the semdFree list, insert it in the ASL (at the appropriate position),
 * initialize all of the fields (i.e. set s semAdd to semAdd, and s procq to mkEmptyProcQ()),
 * and proceed as above.
 * If a new semaphore descriptor needs to be allocated and the semdFree list is empty,
 * return TRUE. In all other cases return FALSE. */
int insertBlocked(int *semAdd, pcb_t *p){

}

/* Search the ASL for a descriptor of this semaphore. If none is found, return NULL;
 * otherwise, remove the first (i.e. head) pcb from the process queue of the found semaphore
 * descriptor and return a pointer to it. If the process queue for this semaphore becomes
 * empty (emptyProcQ(s procq) is TRUE), remove the semaphore descriptor from the
 * ASL and return it to the semdFree list. */
pcb_t *removeBlocked(int *semAdd){

}

/* Remove the pcb pointed to by p from the process queue associated with p’s
 * semaphore (p→ p semAdd) on the ASL.
 * If pcb pointed to by p does not appear in the process queue associated with p’s semaphore,
 * which is an error condition, return NULL; otherwise, re- turn p. */
pcb_t *outBlocked(pcb_t *p){

}

/*                          Additional Functions                     */
/* Functions used for code writing efficiency and to make the code easier to follow */

/* Similar to pcb
 * Function used to allocate values in ASL
 * sets node pointer values to semd_t
 */
semd_t *allocASL(){
    semd_t *temp;

    if(semdFree_h == NULL){ /* if already free */
        return NULL;
    }
    temp = semdFree_h;
    semdFree_h = semdFree_h->s_next;
    temp->s_next = NULL;
    temp->s_semAdd = NULL;
    temp->s_procQ = NULL;
    return temp;
}

/* Similar to pcb, like allocASL function above
 * Function used to deallocate values in ASL
 * adds nodes to semdFree list */
semd_t deallocASL(semd_t *semd){
    semd->s_next = semdFree_h;
    semdFree_h = semd;
}

/* returns semAdd parent if semAdd != NULL
 * if semdAdd = NULL, return a dummy parent node */
semd_t *getParent(int *semAdd){
    semd_t *temp = (semd_t*) semd_h;
    if(semdAdd == NULL){
        semAdd = (int*) MAXINT;
    }
    while (semAdd > (temp->s_next->s_semAdd)){
        temp = temp->s_next;
    }
    return temp;
}


