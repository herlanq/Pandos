/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/27
 *
 * This module processes all types of device interrupts and converts them into V operations
 * on the appropriate semaphores. Calls scheduler in order to ensure no starvation and
 * keep the flow of the program going.
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

/* Global Variables from initial.c */
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;
extern int semD[SEMNUM];


/* 2 helper functions
 * 1 to compute the device number
 * 1 to call the scheduler */
int getDevice(int lineNum);
void CallScheduler();

/* Function that determines the highest priority interrupt and gives control the to scheduler*/
void InterruptHandler(){


}
/* In charge of putting the process back on the ready queue and calling scheduler */
void CallScheduler(){
    state_t *end_state;
    end_state = (state_t*) interrupt_state;
    /* if current process is not null, put back on ready queue, call scheduler */
    if(currentProc != NULL){
        /* need to copy the state of the process */
        CopyState(end_state, &(currentProc->p_s));
        insertProcQ(&readyQue, currentProc);
    }
    /* since no current process, call scheduler to get next process*/
    scheduler();
}

void CopyState(state_t *oldState, state_t *newState){
    /*Loop through all of the registers in the old state and write them into the new state*/
    int i;
    for (i = 0; i < STATEREGNUM; i++)
    {
        newState->s_reg[i] = oldState->s_reg[i];
    }
    /*Move all of the contents from the old state into the new*/
    newState->s_status = oldState->s_status;
    newState->s_pc = oldState->s_pc;
    newState->s_cause = oldState->s_cause;
}

