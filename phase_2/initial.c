/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/22
 *
 * The initial.c module serves as the main for the Pandos Implementation Operating System.
 * This module initializes code from Phase 1 and also initializes variables that are maintained within the the nucleus,
 * nucleus semaphores, instances a single process in the Ready Queue, and calls scheduler.
 */

#include "../h/const.h"
#include "../h/types.h"

#include "../h/pcb.h"
#include "../h/asl.h"

#include "../phase_2/interrupts.c"
#include "../phase_2/exceptions.c"
#include "../phase_2/scheduler.c"


/* Global Variables */
int processCount; /* number of processes on the ready queue */
int softBlockCount; /* Number of processes on the blocked queue */

pcb_t *currentProc; /* Pointer to current process */
pcb_t *readyQue; /* pointer to the ready queue */

int semD[SEMNUM]; /* 49 Semaphore in the list */

/* calls test() from the p2test file */
extern void test();

/* This is the starting point, the main, of the OS. This initializes variables, sets memory addresses,
 * and declares variables that will be used throughout the phase 2 modules.
 * One the main is complete, it passes over to the scheduler */

int main(){
    /* Init pcb and asl */
    initPcbs();
    initASL();

    /* init global variables */
    processCount = 0;
    softBlockCount = 0;
    currentProc = NULL;
    readyQue = NULL;

    /* init semaphores */
    int i;
    for(i=0; i < SUMNUM; i++){
        semD[i] = 0;
    }

    /* alloc process to be set the current process, increment procCount */
    currentProc = allocPcb();
    processCount = processCount + 1;




    /* much more to be added here but ... */




    /* insert current proc into the ready queue*/
    insertProcQ(&readyQue, currentProc);

    /* init current proc back to NULL */
    currentProc = NULL;

    /* load time onto the sudo clock */
    LDIT(IOCLOCK);

    /* Scheduler takes over the running process */
    scheduler();

    return 0;
}