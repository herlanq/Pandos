/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/4
 */

#include ".../h/const.h"
#include ".../h/types.h"
#include ".../h/pcb.h"

HIDDEN pcb_PTR pcbFree_h;



/*set new node's next pointer to current head, make head the newly inserted node*/
void freePcb(pcb_PTR p){
    p->p_next = pcbFree_h;
    pcbFree_h = p;
}

/*function to initialize the pcb stack with maxproc size of 20.*/
void initPcbs(){
    static pcb_t pcbInit[maxProc];
    pcbFree_h = NULL;
    int i = 0;
    while(i < maxProc){
        freePcb(&pcbInit[i]);
    }
}

/*allocating PCB space within this function*/
pcb_PTR allocPcb(){
    if(pcbFree_h == NULL){
        return NULL
    }
    pcb_PTR temp;
    temp = pcbFree_h;
    pcbFree_h = pcbFree_h->p_next;
    /* initializing new pcb */
    temp->p_next = NULL;
    temp->p_prev = NULL;
    temp->p_prnt = NULL;
    temp->p_child = NULL;
    temp->p_Lsib = NULL;
    temp->p_Rsib = NULL;

    return temp;
}

/* clears out the queue by returning null*/
pcb_PTR mkEmptyProcQ(){
    return null;
}

/* Return TRUE if the queue whose tail is pointed to by tp is empty.
Return FALSE otherwise. */
int emptyProcQ(pcb_PTR tp) {
    return (tp == NULL);
}

/*inserts a new process on to the queue, and then adjusts each pointer accordingly.*/
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
        pcb_PTR temp = *tp->p_next; /* temp head node */
        *tp->p_next = p;
        p->p_next = temp;
        p->p_prev = *tp;
        *tp = p;
    }

    *tp = p;
}

/*takes the next process off of the queue and adjusts pointers accordingly. */
pcb_PTR removeProcQ(pcb_PTR *tp) {
    if (emptyProcQ(*tp) == NULL) { /* if queue is empty, return  */
        return NULL;
    }else if (*tp->p_next == *tp){ /* if there is only one node */
        pcb_PTR *temp = *tp ;
        *tp = mkEmptyProcQ();
        return temp;
    }else{ /* queue has n nodes where n > 1 */
        pcb_PTR temp = *tp->p_next;
        *tp->p_next = temp->p_next;
        temp->p_next->p_prev = *tp;
        return temp;
    }
}
/*takes a specific pointer and removes said process from queue, and adjusts pointers accodingly. */
pcb_PTR outProcQ(pcb_PTR *tp, pcb_PTR p){
    if(headProcQ(*p) == NULL) {
        removeProcQ(*tp, p);
        return NULL;
    }else if(*tp->p_prev=p_next){
        p->p_prev = NULL;
        p->p_next = NULL;
        return p;
    }else if(*tp = p;){
        *tp = P->p_prev;
        pcb_PTR temp;
        temp = p;
        temp->p_next->p_prev = temp->p_prev;
        temp->p_prev->p_next = temp->p_next;
        return p;
    }
    pcb_PTR temp;
    temp = p;
    temp->p_next->p_prev = temp->p_prev;
    temp->p_prev->p_next = temp->p_next;
    return p;
}

/*returns null if list is empty in passed tailpointer, otherwise returns the head of the queue. */
pcb_t* headProcQ(pcb_PTR tp){
    if(emptyProcQ(tp) == NULL){
        return NULL;
    }
    return tp->p_next;
}

/*checks to see if there is a child on the tree, and returns NULL if there is not.
 return true if empty
 return false if not */
int emptyChild(pcb_PTR p){
    return (p->p_child==NULL);
}

/*inserts a child on to the tree, and then connects to the siblings, if any, on the tree.
 Make the pcb pointed to by p a child of the pcb pointed to by prnt */
void insertChild(pcb_PTR prnt, pcb_PTR p){
    if (emptyChild(prnt)){ /* case 1: no children */
        p_prnt->p_child = p;
        p->p_prnt = p_prnt;
        p->p_prevsib = NULL;
        p->p_Rsib = NULL;
    } /* case 2: n children n >= 1 */
    p->p_prnt = p_prnt;
    p_prnt->p_child->p_prevsib = p;
    p->p_sib = p_prnt->p_child;
    p->p_prevsib = NULL;
    p_prnt->p_child =p;
}


/*removes the first child pointed to by p, and adjusts siblings accordingly if any. */
pcb_PTR removeChild(pcb_PTR p){
    if(emptyChild(p)==NULL){
        return NULL;
    }else if(p->p_child->p_Rsib == NULL) {


    }else { /* children n > 1 */

    }

}
/* removes a specified child that may or may not be the first one, or could be the top parent. */
pcb_PTR outChild(pcb_PTR p){
    if(p == NULL){
        return NULL;
    }
    if(p->p_prnt == NULL){
        return NULL;
    }
    /* one child */
    if(p_Lsib == NULL && p->Rsib == NULL && p== p->prnt->p_child){
        p->p_prnt->p_child = NULL;
        p->p_prnt = NULL;
        return p;
    }
    /* first child */
    if(){

    }
    /* last child */
    if(){

    }
    /* middle */
    if(){

    }
}