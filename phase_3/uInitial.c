/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 11/11
 */

/*REMINDERS:
One page table per user process
Pandos Page Table is an array of 32 Page Table entries. 
Each Page Table entry is a doubleword consisting of an EntryHi and an EntryLo portion
*/

#include "../h/const.h"
#include "../h/types.h"
#include "../h/libumps.h"
#include "../h/VMsupport.h"
#include "../h/syssupport.h"


int devSem[DEVICECNT+DEVPERINT]; /* device sema4's */
int control_sem;

HIDDEN void InitUserProc();
/*Planning on using this function to initialize all the structures needed for each process,
possibly the swap pool and backing store as well */

void test(){
    int devices = (DEVICECNT+DEVPERINT);

    InitTLB();
    for(int i=0; i < devices; i++){
        devSem[i] = 1;
        /* This is where we would initialize our device semaphores, which I think there is one per process. */
    }

    /* initialize user processes */
    InitUserProc();

    control_sem = 0;

    for(int i = 0; i < UPROCMAX; ++i){
        SYSCALL(PASSERN, (int)&control_sem, 0, 0);
    }

    SYSCALL(TERMINATETHREAD, 0, 0, 0);

} /* end test() */

/* helper function to clean up the code, initializes the user processes */
HIDDEN void InitUserProc(){
    int id;
    int begin;
    int devices = (DEVICECNT+DEVPERINT);
    memaddr ramtop;
    memaddr stacktop;
    support_t support[UPROCMAX + 1];
    state_t start_state;

    RAMTOP(ramtop);
    /*now it is time to start initializing user processes */
    for(id = 1; id <= UPROCMAX; id++) {
        stacktop = ramtop - (id * PAGESIZE * 2);
        start_state.s_entryHI = id << ASIDSHIFT;
        start_state.s_sp = (int) USERSTACK;
        start_state.s_pc = start_state.s_t9;
        start_state.s_pc = (memaddr) USERPROCSTART;
        start_state.s_status = ALLOFF | IEPON | IMON | UMON | TEBITON;

        support[id].sup_asid = id;
        support[id].sup_exceptContext[GENERALEXCEPT].c_status = ALLOFF | IEPON | IMON | TEBITON;
        support[id].sup_exceptContext[GENERALEXCEPT].c_stackPtr = stacktop;
        support[id].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) uGeneralExcept;
        support[id].sup_exceptContext[PGFAULTEXCEPT].c_status = ALLOFF | IEPON | IMON | TEBITON;
        support[id].sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = stacktop + PAGESIZE;
        support[id].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) uVM_Handler;

        /* Init page table */
        for (int i = 0; i < MAXPAGES; i++) {
            support[i].sup_PvtPgTable->entryHI = ((0x80000 + i) << VPNSHIFT) | (id << ASIDSHIFT);
            support[i].sup_PvtPgTable->entryLO = ALLOFF | DIRTYON;
        }

        support[id].sup_PvtPgTable[MAXPAGES - 1].entryHI = (0xBFFFF << VPNSHIFT) | (id << ASIDSHIFT);

        begin = SYSCALL(CREATETHREAD, (int) &start_state, (int) &(support[id]), 0);
        if (begin == 0) {
            SYSCALL(TERMINATETHREAD, 0, 0, 0);
        }
    }
} /* end inituserproc */