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

int semD[SEMNUM]; /* 49 Semaphore in the list */

/* declares test() from the p2test file */
extern void test();

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
    foo->tlb_refill_handler = (memaddr) uTLB_RefillHandler;
    foo->tlb_refill_stackPTR = (memaddr) 0x20001000;
    foo->exceptionHandler = (memaddr) exception_handler;
    foo->s_stackPTR = 0x20001000;


    /* Init pcb and asl */
    initPcbs();
    initASL();

    /* load time onto the sudo clock */
    LDIT(100);

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

void genExceptionHandler(/*im guessing we need to pass in the cause register*/){
currentProc->p_s.s_cause && 0x000000FC;
currentProc->p_s.s_cause << 2;          /*turning off the bits we don't need, and then shifting them over to make a comparison */
int handle = currentProc->p_s.s_cause; 
    if(handle == 0)
        InterruptHandler();
    if(handle == 1 || handle == 2 || handle == 3)
        TlbTrapHandler();
    if(handle == 4 || handle == 5 || handle == 6 || handle == 7)
        PrgTrapHandler();
    if(handle == 8)
        sysHandler();
}