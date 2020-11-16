/* TLB Exception Handler for phase_3 */
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
#include "../h/pcb.h"
#include "../h/asl.h"
#include "../h/initial.h"
#include "../h/interrupts.h"
#include "../h/exceptions.h"
#include "../h/scheduler.h"
#include "../h/libumps.h"

extern pcb_t *currentProc;
swap_t swap_pool[POOLSIZE];
int swap_sem;

void pager();
void uPgmTrapHandler();
void uSysHanlder();

pcb_t uProcs[UPROCMAX]; /* Array of user processes */

HIDDEN void InitUserProc();


/*Planning on using this function to initialize all the structures needed for each process,
possibly the swap pool and backing store as well */
void test(){
    for(int i = 1; i <= 8; i++){
        /* This is where we would initialize our device semaphores, which I think there is one per process. */
    }
    /*initializing the swap pool table and semaphore is initialized above as HIDDEN */
    /*should be putting swap pool at like 0x2000.0000 plus some ambiguous number we decide */
    for(int i = 0; i < POOLSIZE; i++){
        swap_pool[i].sw_asid = -1;
        swap_pool[i].sw_pgNum = 0;
        swap_pool[i].sw_pte = NULL;
    }

    swap_sem = 1;

    /*now it is time to start initializing user processes */
    for(int i = 1, i<= UPROCMAX; i++){
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

}

/*This function is used for when there is no TLB entry found,
this function goes and searches for it within the page table */
void uTLB_RefillHandler() {
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