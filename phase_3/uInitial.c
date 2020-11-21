/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 11/11
 *
 * User Process Initialization module. This module initializes all of the user processes set up b
 * the test() function. All process information is being initialized such as page and segment table and backing store
 * locations. When all the processes have been created, test is blocked until all of the processes have been terminated.
 * Once all the processes have been terminated, test() will then also be terminated.
 */

/*REMINDERS:
One page table per user process
Pandos Page Table is an array of 32 Page Table entries. 
Each Page Table entry is a doubleword consisting of an EntryHi and an EntryLo portion
*/

#include "../h/const.h"
#include "../h/types.h"
#include "../h/libumps.h"
#include "../h/uInitial.h"

#include "../h/VMsupport.h"
#include "../h/syssupport.h"


int devSem[DEVICECNT+DEVPERINT]; /* list of device sema4's */
int control_sem; /* master process sema4 */
int zdiditdo = 0;

HIDDEN void InitUserProc();

/* master process that initializes all necessary data structures adn processes. Once this is complete, test() will
 * blocks itself until all of the processes have been terminated and then terminates itself. */
void test(){
    int i;
    /* initialize the TLB*/
    InitTLB();
    /* initializes the device reg semaphores */
    for(i=0; i < (DEVICECNT+DEVPERINT); i++){
        devSem[i] = 1;
    }
    /* call helper function to initialize user processes */
    InitUserProc();

    /* init master proc control sema4 */
    control_sem = 0;

    /*block master proc */
    for(i=0; i < UPROCMAX; ++i){
        SYSCALL(PASSERN, (int)&control_sem, 0, 0);
    }

    /* Au revoir */
    SYSCALL(TERMINATETHREAD, 0, 0, 0);
} /* end test(), master process */

/* helper function to clean up the code, initializes the user processes */
HIDDEN void InitUserProc(){
    int id;
    int begin;
    support_t support[UPROCMAX + 1];
    state_t start_state; /* processor state */

    /*now it is time to start initializing user processes */
    for(id = 1; id <= UPROCMAX; id++) {
        start_state.s_entryHI = id << ASIDSHIFT;
        start_state.s_sp = (int) USERSTACK;
        start_state.s_pc = start_state.s_t9 = (memaddr) USERPROCSTART;
        start_state.s_status = ALLOFF | IEPON | IMON | UMON | TEBITON;

        support[id].sup_asid = id;
        support[id].sup_exceptContext[GENERALEXCEPT].c_status = ALLOFF | IEPON | IMON | TEBITON;
        support[id].sup_exceptContext[GENERALEXCEPT].c_stackPtr = (int) &(support[id].sup_stackG[499]);
        support[id].sup_exceptContext[GENERALEXCEPT].c_pc = (memaddr) SysSupport;
        support[id].sup_exceptContext[PGFAULTEXCEPT].c_status = ALLOFF | IEPON | IMON | TEBITON;
        support[id].sup_exceptContext[PGFAULTEXCEPT].c_stackPtr = (int) &(support[id].sup_stackM[499]);
        support[id].sup_exceptContext[PGFAULTEXCEPT].c_pc = (memaddr) uTLB_Pager;

        /* Init page table */
        int i;
        for (i=0; i < MAXPAGES; i++) {
            support[id].sup_PvtPgTable[i].entryHI = ((0x80000 + i) << VPNSHIFT) | (id << ASIDSHIFT);
            support[id].sup_PvtPgTable[i].entryLO = ALLOFF | DIRTYON;
        }

        support[id].sup_PvtPgTable[MAXPAGES-1].entryHI = (0xBFFFF << VPNSHIFT) | (id << ASIDSHIFT);
        begin = SYSCALL(CREATETHREAD, (int) &start_state, (int) &(support[id]), 0);
        
        /* create the process, if it does not create, terminate B( */
        if (begin != READY) {
            SYSCALL(TERMINATETHREAD, 0, 0, 0);
        }
    }
} /* end inituserproc */