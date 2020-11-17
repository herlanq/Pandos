/* TLB Exception Handler for phase_3 */
/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 11/11
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/uInitial.h"
#include "../h/libumps.h"

HIDDEN swap_t swap_pool[POOLSIZE];
HIDDEN int swap_sem;

void InitTLB(){
    swap_sem = 1;
    for(int i = 0; i < POOLSIZE; i++){
        swap_pool[i].sw_asid = -1;
    }
}

/*This function is used for when there is no TLB entry found,
this function goes and searches for it within the page table */
void uTLB_RefillHandler(){
    state_PTR oldstate;
    int pg_num;

    oldstate = (state_PTR) BIOSDATAPAGE;

    pg_num = (oldstate->s_entryHI & GETPAGENUM) >> VPNSHIFT;
    pg_num = pg_num % MAXPAGES;

    setENTRYHI((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryHI);
    setENTRYLO((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryLO);
    TLBWR();
    LDST(oldstate);
}

/*this function is used for TLB invalid and modification exceptions,
it should check to make sure that the D-bit is on, and also check the valid bit. */
void uTLB_ExceptionHandler(){
    int id, frame_num, pg_num, status;
    pteEntry_t *entry;
    unsigned int frame_addr;
    int block;
    support_t *supStruct;
    int cause;

    supStruct = (support_t *) SYSCALL(GETSPTPTR,0,0,0);
    cause = (supStruct->sup_exceptState[PGFAULTEXCEPT].s_cause & CAUSE);
    id = supStruct->sup_asid;

    if(cause == TLBINV || cause == TLBINVS){
        pg_num = ((supStruct->sup_exceptState[PGFAULTEXCEPT].s_entryHI) & GETPAGENUM);
        mutex();
        frame_num = getFrame();
        frame_addr = FRAMEPOOL + (frame_num * PAGESIZE);
        if(swap_pool[frame_num].sw_asid != -1){
            
        }


    }else{
        termUproc();
    }

    if(((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryLO >> 10) == 0)
    {
        PassUpOrDie(GENERALEXCEPT);
    }
    if(((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryLO >> 9) == 0){
        Pager();
    }
}
