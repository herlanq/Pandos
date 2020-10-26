/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 10/15
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

/* Internal and External Function Declaration*/
extern void test(); /* declares test() from the p2test file */
HIDDEN void genExceptionHandler();
extern void uTLB_RefillHandler();

/* Global Variables */
int processCount; /* number of processes on the ready queue */
int softBlockCount; /* Number of processes on the blocked queue */
pcb_t *currentProc; /* Pointer to current process */
pcb_t *readyQue; /* pointer to the ready queue */
cpu_t start_clock;
int semD[SEMNUM]; /* 49 Semaphore in the list, [0 - 48] */
#define CLOCKSEM semD[SEMNUM-1] /* clock sema4 in the device sema4 list , == semD[48] */

/* debug variables */
int exception_check;


/* This is the starting point, the main, of the OS. This initializes variables, sets memory addresses,
 * and declares variables that will be used throughout the phase 2 modules.
 * One the main is complete, it passes over to the scheduler */
int main(){
    /* init local variables */
    processCount = 0;
    softBlockCount = 0;
    pcb_PTR proc;

    /* initializing the pass up vector */
    passupvector_t *passupvector = (passupvector_t*) PASSUPVECTOR;
    passupvector->tlb_refll_handler = (memaddr) uTLB_RefillHandler;
    passupvector->tlb_refll_stackPtr = (memaddr) 0x20001000;
    passupvector->execption_handler = (memaddr) genExceptionHandler;
    passupvector->exception_stackPtr = (memaddr) 0x20001000;

    /* Init pcb and asl */
    initPcbs();
    initASL();
    /* prepare empty readyque and current process */
    currentProc = NULL;
    readyQue = mkEmptyProcQ();

    /* init semaphores on the device sema4 list */
    int i;
    for(i=0; i < (SEMNUM-1); i++){
        semD[i] = 0;
    }

    /*set the clock semaphore to 0 */
    CLOCKSEM = 0;

    /* load time onto the pseudo clock */
    LDIT(PSUEDOCLOCKTIME);

    /* alloc process to be set the current process, increment procCount */
    proc = allocPcb();
    if(proc != NULL) {
        processCount = processCount + 1;
        memaddr ramtop = *(int*)RAMBASEADDR + *(int*)RAMBASESIZE;
        proc->p_s.s_sp = (memaddr) ramtop;
        proc->p_s.s_pc = (memaddr) test;
        proc->p_s.s_t9 = (memaddr) test;
        proc->p_s.s_status = (ALLOFF | IECON | IMON | TEBITON);

        /* insert current proc into the ready queue*/
        insertProcQ(&readyQue, proc);

        /* Call the Scheduler for the next process to take over */
        scheduler();
    }else{
        PANIC();
    }
    return(0);
}

/* Calls the appropriate exception handler based on the cause */
void genExceptionHandler(){
    int eReason;
    state_PTR oldState = (state_PTR) BIOSDATAPAGE;

    /* if the current process != NULL, copy all of its register values and paste them into a new state */
    if(currentProc != NULL){
        Copy_Paste(oldState, &currentProc->p_s);
    }
    eReason = (oldState->s_cause & CAUSE) >> SHIFT;
    exception_check = eReason;
        if(eReason == IOINTERRUPT){
            InterruptHandler();
        }
        if(eReason <= TLBEXCEPTION){
            TlbTrapHandler();
        }
        if(eReason == SYSEXCEPTION){
            sysHandler();
        }
    PrgTrapHandler();
}