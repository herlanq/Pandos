/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/27
 *
 * The initial.c module serves as the main for the Pandos Implementation Operating System.
 * This module initializes code from Phase 1 and also initializes variables that are maintained within the the nucleus,
 * nucleus semaphores, instances a single process in the Ready Queue, and calls scheduler.
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


/* Global Variables */
int processCount; /* number of processes on the ready queue */
int softBlockCount; /* Number of processes on the blocked queue */

pcb_t *currentProc; /* Pointer to current process */
pcb_t *readyQue; /* pointer to the ready queue */

cpu_t startTOD;
cpu_t sliceCNT;

int semD[SEMNUM]; /* 49 Semaphore in the list */

/* declares test() from the p2test file */
extern void test();
HIDDEN void genExceptionHandler();
extern void uTLB_RefillHandler();

/* This is the starting point, the main, of the OS. This initializes variables, sets memory addresses,
 * and declares variables that will be used throughout the phase 2 modules.
 * One the main is complete, it passes over to the scheduler */
int main(){
    /* init global variables */
    processCount = 0;
    softBlockCount = 0;
    currentProc = NULL;
    readyQue = mkEmptyProcQ();
    /* init semaphores */
    int i;
    for(i=0; i < SEMNUM; i++){
        semD[i] = 0;
    }

    passupvector foo = PASSUPVECTOR;
    foo->tlb_refill_Handler = (memaddr) uTLB_RefillHandler;
    foo->tlb_refill_stackPTR = (memaddr) RAMTOP;
    foo->exceptionHandler = (memaddr) genExceptionHandler;
    foo->s_stackPTR = RAMTOP;


    /* Init pcb and asl */
    initPcbs();
    initASL();

    /* load 100ms onto the pseudo clock */
    LDIT(IOCLOCK);

    /* alloc process to be set the current process, increment procCount */
    currentProc = allocPcb();
    currentProc->p_semAdd = NULL;
    currentProc->p_time = 0;
    currentProc->p_supportStruct = NULL;
    processCount = processCount + 1;

    /* insert current proc into the ready queue*/
    insertProcQ(&readyQue, currentProc);

    /* init current proc back to NULL */
    currentProc = NULL;

    /* Scheduler takes over the running process */
    scheduler();

    return 0;
}

void genExceptionHandler(){
/*turning off the bits we don't need, and then shifting them over to make a comparison */
    state_PTR oldState;
    int eReason;
    oldState = (state_PTR) /*this is where previous state goes, need to find syntax */
    eReason = (oldState->s_cause & CAUSE) << 2; 
        if(eReason == 0)
            InterruptHandler();
        if(eReason == 1 || eReason == 2 || eReason == 3)
            TlbTrapHandler();
        if(eReason == 4 || eReason == 5 || eReason == 6 || eReason == 7)
            PrgTraphandler();
        if(eReason == 8){
            /* should check cause register and syscall before going into sysHandler
            because if not we should hit the PrgTrapHandler() */
            if(currentProc->s_cause & UMOFF == 1)
                sysHandler();
            PrgTraphandler();
        }
}