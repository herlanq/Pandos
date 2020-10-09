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
HIDDEN int getDevice(int lineNum);
HIDDEN void CallScheduler();
HIDDEN void PLS_Interrupt(cpu_t stop);
HIDEEN void sudoClock_Interrupt();
HIDDEN void terminal_interrupt(int *device_sema4);


/* Function that determines the highest priority interrupt and gives control the to scheduler*/
void InterruptHandler(){
    unsigned int causeLine;
    /* Variables for determining dev address and dev sema4 */
    int tempLineNum;
    int LineNum;
    int DevSema4;
    int devNum;

    /* V operation sema4 variables */
    int* sema4;
    int* sema4Add;
    /* store device status in v0 */
    int DevStatus;

    pcb_t *blockProc;
    state_PTR caller;

    /* get state of interrupt */
    caller = (state_t*) INTERRUPT_OLD;

    causeLine = caller->s_cause >> 8;
    /* if multicore is on */
    if((causeLine & CLOCK1) != 0){ /* PLT */
        CallScheduler(); /* process quantum is up, move to new process */
    }else if((causeLine & CLOCK2) != 0){ /* Pseudo clock */
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
        sema4Add = 0;
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
    devNum = getDevice(LineNum);
    /* 8 devices per line num */
    DevSema4 = tempLineNum * DEVPERINT;
    tempLineNum = causeLine;
    /* find device */
    DevSema4 = DevSema4 + DevNum;
    device_t *devReg;
    /* finding specific device register */
    devReg = (device_t*)(0x10000000) + (tempLineNum * 0x80) + (devNum * 0x10);
    /* terminal, differentiate between read/write */
    if(LineNum == TERMINT){
        if((devReg->d_status & 0xF) =! READY){
            /* set dev status */
            DevStatus = devReg->d_status;
            /*ACK*/
            devReg->d_command = ACK;
        }else{
            DevSema4 = DevSema4 + DEVPERINT;
            DevStatus = devReg->d_status;
            devReg->d_command = ACK;
        }
    }else{ /* not a terminal */
        /* set status and ACK interrupt */
        DevStatus = devReg->d_status;
        devReg->d_command = ACK;
    }
    /* get sema4 for dev causing interrupt */
    sema4Add = (&(semD[DevSema4]));
    (*sema4Add) = (*sema4Add) + 1;
    if((*sema4Add) <= 0){
        /* Removed from blocked list */
        blockProc = removeBlocked(sema4Add);
        if(blockProc != NULL){
            /* set status in v0 register and decrement softblockcount
             * and insert into ready queue */
            blockProc->p_s.s_v0 = DevStatus;
            softBlockCount = softBlockCount - 1;
            insertProcQ(&readyQue, blockProc);
        }
    }
    CallScheduler();
}

/*                                              HELPER FUNCTIONS                                                    */

/* Take in the line number of the interrupt.
 * Bit shift until we find the first device causing the interrupt */
void getDevice(int linenum){
    int x;
    /* area that is causing the interrupt */
    devregarea_t *interrupt_device;
    int DevLineNum;
    unsigned int bitMapLine;
    unsigned int bipMapActive;

    int interrupt_deviceNum;
    DevLineNum = linenum;


}
/* In charge of putting the process back on the ready queue and calling scheduler */
void CallScheduler(){
    state_t *temp;
    temp = (state_t) INTERRUPT_OLD;
    /* if current process is not null, put back on ready queue, call scheduler */
    if(currentProc != NULL){
        /* need to copy the state of the process */
        CopyState(temp, &(currentProc->p_s));
        insertProcQ(&readyQue, currentProc);
    }
    /* since no current process, call scheduler to get next process*/
    scheduler();
}

void CopyPaste(state_t *copied_state, state_t *pasted_state){
    /*Loop through all of the registers in the old state and write them into the new state*/
    int i;
    for (i = 0; i < STATEREGNUM; i++){
        pasted_state->s_reg[i] = copied_state->s_reg[i];
    }
    /*Move all of the contents from the old state into the new*/
    pasted_state->s_entryHI = copied_state->s_entryHI;
    pasted_state->s_status = copied_state->s_status;
    pasted_state->s_pc = copied_state->s_pc;
    pasted_state->s_cause = copied_state->s_cause;
}

