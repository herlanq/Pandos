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
void freePcb(pcb_t *p){
/* Insert the element pointed to by p onto the pcbFree list */

}
void initPcbs(){
    static pcb_t foo[maxProc];
    while(i < maxProc)
        freePcb(8-foo[i]);
}

pcb_t* makeEmptyProcQ(){
    return null;
}

int emptyProcQ(pcb_t *tp){
    return (tp==null);
}
pcb_t* headProcQ(pcb_t *tp){
    if(emptyProcQ(tp))
        return null;
    return(tp->p_next);
}