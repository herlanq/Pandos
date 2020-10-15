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


/* Global Variables */
int processCount; /* number of processes on the ready queue */
int softBlockCount; /* Number of processes on the blocked queue */
pcb_t *currentProc; /* Pointer to current process */
pcb_t *readyQue; /* pointer to the ready queue */
cpu_t start_clock;
cpu_t sliceCNT;
unsigned int device_status[SEMNUM-1]; /* save device state area */
int semD[SEMNUM]; /* 49 Semaphore in the list */
#define CLOCKSEM semD[SEMNUM-1]


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

    passupvector_t *passupvector = (passupvector_t*) PASSUPVECTOR;
    passupvector->tlb_refll_handler = (memaddr) uTLB_RefillHandler;
    passupvector->tlb_refll_stackPtr = (memaddr) PASSUPVECTOR + 0x04;
    passupvector->execption_handler = (memaddr) genExceptionHandler;
    passupvector->exception_stackPtr = (memaddr) PASSUPVECTOR + 0x04;
    


    /* Init pcb and asl */
    initPcbs();
    initASL();

    /* load time onto the pseudo clock */
    LDIT(PSUEDOCLOCKTIME);
    CLOCKSEM = 0;

    /* alloc process to be set the current process, increment procCount */
    pcb_PTR proc = allocPcb();
    if(proc != NULL) {
        processCount = processCount + 1;
        memaddr ramtop = RAMBASEADDR + RAMBASESIZE;
        proc->p_s.s_sp = (memaddr) ramtop;
        proc->p_s.s_pc = (memaddr) test;
        proc->p_s.s_t9 = (memaddr) test;
        proc->p_s.s_status = (ALLOFF | IECON | IMON | TEBITON);
        

        /* insert current proc into the ready queue*/
        insertProcQ(&readyQue, proc);

        /* init current proc back to NULL */
        currentProc = NULL;

        /* Scheduler takes over the running process */
        scheduler();
    }else{
        PANIC();
    }
    return(0);
}

void genExceptionHandler(){
/*turning off the bits we don't need, and then shifting them over to make a comparison */
    int eReason;
    state_PTR oldState = (state_PTR) BIOSDATAPAGE;
    eReason = (oldState->s_cause & CAUSE) >> SHIFT;
        if(eReason == IOINTERRUPT){
            InterruptHandler();
        }
        if((eReason <= TLBEXCEPTION){
            TlbTrapHandler();
        }
        if(eReason == SYSEXCEPTION){
            sysHandler();
        }
    PrgTrapHandler();
}