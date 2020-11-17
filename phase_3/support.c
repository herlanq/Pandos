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

void InitTLB();

void InitTLB(){
    /*initializing the swap pool table and semaphore is initialized above as HIDDEN */
    /*should be putting swap pool at like 0x2000.0000 plus some ambiguous number we decide */
    swap_sem = 1;
    for(int i = 0; i < POOLSIZE; i++){
        swap_pool[i].sw_asid = -1;
    }

}

/*this function is used for TLB invalid and modification exceptions,
it should check to make sure that the D-bit is on, and also check the valid bit. */
void uTLB_exceptionHandler(){
    if(((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryLO >> 10) == 0)
    {
        PassUpOrDie(GENERALEXCEPT);
    }
    if(((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryLO >> 9) == 0){
        Pager();
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
/* This is the function called for TLB invalid issues (page faults) and will be handled in here */
void Pager(){


}