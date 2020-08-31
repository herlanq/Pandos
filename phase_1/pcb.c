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
    int                 *p_semAdd   /*which process blocked*/

    /* support layer information */
    support_t           *p_supportStruct;
                                /*ptr to support struct*/
} pcb_t;