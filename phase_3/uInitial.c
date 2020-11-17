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
#include "../h/support.h"

extern pcb_t *currentProc;
int devSem[DEVICECNT+DEVPERINT]; /* device sema4's */


void Pager();
void uPgmTrapHandler();
void uSysHanlder();

pcb_t uProcs[UPROCMAX]; /* Array of user processes */

HIDDEN void InitUserProc();
/*Planning on using this function to initialize all the structures needed for each process,
possibly the swap pool and backing store as well */

void test(){
    memaddr ramtop;
    memaddr stacktop;
    state_t start_state;
    support_t support[UPROCMAX + 1];
    int devices = (DEVICECNT+DEVPERINT);

    InitTLB();

    for(int i=0; i < devices; i++){
        devSem[i] = 1;
        /* This is where we would initialize our device semaphores, which I think there is one per process. */
    }
    RAMTOP(ramtop);

    /*now it is time to start initializing user processes */
    for(int i = 1, i <= UPROCMAX; i++){
        stacktop = ramtop - (i * PAGESIZE * 2);


        /*set EntryHi to ASID
        set pc/T9 to 0x8000.00B0
        set SP to 0xC000.0000
        setStatus(Kernel off, INTEnabled, PLTenabled)
        set supplemental data strucutre in case of pass up */
        SYSCALL(CREATETHREAD,0,0,0);
    }
    /*now we wait until all the processes are finished */
    /*then we SYS2 and call it a day */
    SYSCALL(TERMINATETHREAD,0,0,0);
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