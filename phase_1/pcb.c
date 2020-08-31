/* Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 8/31
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

pcb_PTR allocPcb();

pcb_PTR makeEmptyProcQ(){
    return null;
}

int emptyProcQ(pcb_PTR tp){
    return (tp==null);
}

void insertProcQ(pbc_PTR *tp, pcb_PTR p);

pcb_PTR removeProcQ(pcb_PTR *tp);

pcb_PTR outProcQ(pcb_PTR *tp, pcb_PTR p);



pcb_t* headProcQ(pcb_PTR tp){
    if(emptyProcQ(tp))
        return null;
    return(tp->p_next);
}

int emptyChild(pcb_PTR p);

void insertChild(pcb_PTR prnt, pcb_PTR p);

pcb_PTR removeChild(pcb_PTR p);

pcb_PTR outChild(pcb_PTR p);