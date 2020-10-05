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
    unsigned int causeLine;
    /* Variables for determining dev address and dev sema4 */
    int tempLineNum;
    int LineNum;
    int DevSema4;
    int DevNum;

    /* V operation sema4 variables */
    int* sema4;
    int* sema4Add;

    /* store device status in v0*/
    int DevStatus;

    pcb_t *blockProc;
    state_PTR caller;

    /* get state of interrupt */
    caller = (state_t*) interrupt_state;

    causeLine = caller->s_cause >> 8;
    /* if multicore is on */
    if((causeLine & MULTICORE) != 0){
        PANIC();
    }else if((causeLine & CLOCK1) != 0){
        CallScheduler(); /* process quantum is up, move to new process */
    }else if((causeLine & CLOCK2) != 0){
        /* access pseudo clock*/
        sema4Add = (int*) &(semD[SEMNUM-1]);
        /* free all currently blocked processes and insert them onto the ready queue */
        while(headBlocked(sema4Add) != NULL){
            /* remove blocked process */
            blockProc = removeBlocked(sema4Add);
            /* Put onto the ready queue */
            if(blockProc != NULL){
                insertProcQ(&readyQue, blockProc);
                /* decrement softblock count*/
                softBlockCount--;
            }
        }
        /* Set sema4 back to 0 */
        *sema4Add = 0;
        /* load pseudo clock, call scheduler for next process */
        LDIT(PSUEDOCLOCKTIME);
        CallScheduler();
    }else if((causeLine & DISKDEVICE) != 0){
        /* disk dev is on */
        causeLine = DI;
    }else if((causeLine & FLASHDEVICE) != 0){
        /* flash dev is on */
        causeLine = FI;
    }else if((causeLine & NETWORKDEVICE) != 0){
        /* network dev is on */
        causeLine = NETWORKI;
    }else if((causeLine & PRINTERDEVICE) != 0) {
        /* printer dev is on */
        causeLine = PRINTERI;
    }else if((causeLine & TERMINALDEVICE) != 0) {
        /* terminal dev is on */
        causeLine = TERMINALI;
    }else{
        PANIC();
    }
    /* call helper function to get line number */
    DevNum = getDevice(LineNum);
    /* 8 devices per line num */
    DevSema4 = tempLineNum * DEVPERINT;
    /* find device */
    DevSema4 = DevSema4 + DevNum;
    device_t *devReg;


}

/* Take in the line number of the interrupt.
 * Bit shift until we find the first device causing the interrupt */
void getDevice(int linenum){

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
    for (i = 0; i < STATEREGNUM; i++){
        newState->s_reg[i] = oldState->s_reg[i];
    }
    /*Move all of the contents from the old state into the new*/
    newState->s_entryHI = oldState->s_entryHI;
    newState->s_status = oldState->s_status;
    newState->s_pc = oldState->s_pc;
    newState->s_cause = oldState->s_cause;
}

