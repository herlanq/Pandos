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
extern int CLOCKSEM;

/* separate functions for interrupt handling */
HIDDEN void Device_InterruptH(int line);
HIDDEN int terminal_interrupt(int device_sema4);


/* Function that determines the highest priority interrupt and
 * gives control the to scheduler.
 * PLS interrupts and Pseudo clock interrupts have their own separate handler functions */
void InterruptHandler(){
    cpu_t start_clock;
    cpu_t stop_clock;
    cpu_t time_left = getTIMER();
    STCK(stop_clock);
    state_PTR int_cause = ((state_PTR) BIOSDATAPAGE);

    /* BEGIN INTERRUPT HANDLING */
    /* PLT interrupt, quantum is up, time to switch to the next process */
    if((int_cause->s_cause & PLTINT) != 0) {
        if(currentProc != NULL){
            /* compute time of the running process, move the processor state, call on the next process */
            currentProc->p_time = (currentProc->p_time) + (stop_clock - start_clock);
            Copy_Paste((state_PTR) BIOSDATAPAGE, &(currentProc->p_s));
            insertProcQ(&readyQue, currentProc);
            scheduler(); /* not sure if this is correct? It has to switch to a new proc */
        }else{
            PANIC();
        }
    }
    /* Pseudo clock tick interrupt
     * Awaken all of the processes that are waiting on the pseudo clock */
    if((int_cause->s_cause & TIMERINT) != 0){
        pcb_PTR proc;
        LDIT(PSUEDOCLOCKTIME);
        proc = removeBlocked(&CLOCKSEM);
        while(proc !=NULL){
            insertProcQ(&readyQue, proc);
            softBlockCount--;
            proc = removeBlocked(&CLOCKSEM);
        }
        CLOCKSEM = 0;
        if(currentProc == NULL){
            scheduler();
        }
    }
    /* Disk interrupt */
    if((int_cause->s_cause & DISKINT) != 0){
        /* disk dev is on */
        Device_InterruptH(DISK);
    }
    /* Flash interrupt */
    if((int_cause->s_cause & FLASHINT) != 0){
        /* flash dev is on */
        Device_InterruptH(FLASH);
    }
    /* Printer interrupt */
    if((int_cause->s_cause & PRINTERINT) != 0) {
        /* printer dev is on */
        Device_InterruptH(PRINTER);
    }
    /* Terminal interrupt (special case) */
    if((int_cause->s_cause & TERMINT) != 0) {
        /* terminal dev is on */
        Device_InterruptH(TERMINAL);
    }
    if(currentProc != NULL){
        /* assign run time before the interrupt to the current process
         * and return control to the running process */
        currentProc->p_time = currentProc->p_time + (stop_clock + start_clock);
        Copy_Paste((state_PTR)BIOSDATAPAGE, &(currentProc->p_s));
        Ready_Timer(currentProc, time_left);
    }else{
        HALT();
    }
}
/* Interrupt handler for peripheral devices.
 * V's the correct device semaphore and stores the device data. */
HIDDEN void Device_InterruptH(int line){
    unsigned int bitMAP;
    volatile devregarea_t *deviceRegister;
    /* Addressing */
    deviceRegister = (devregarea_t *) RAMBASEADDR;
    bitMAP = deviceRegister->interrupt_dev[line-DISK];
    int device_number; /* interrupt device number */
    int device_semaphore; /* interrupt device semaphore */
    unsigned int status; /* status of the interrupting device */
    pcb_PTR proc;

    if((bitMAP & DEV0) != 0){
        device_number = 0;
    }
    if((bitMAP & CLOCK1) != 0){
        device_number = 1;
    }
    if((bitMAP & CLOCK2) != 0){
        device_number = 2;
    }
    if((bitMAP & DISKDEVICE) != 0){
        device_number = 3;
    }
    if((bitMAP & FLASHDEVICE) != 0){
        device_number = 4;
    }
    if((bitMAP & NETWORKDEVICE) != 0){
        device_number = 5;
    }
    if((bitMAP & PRINTERDEVICE) != 0){
        device_number = 6;
    }
    if((bitMAP & TERMINALDEVICE) != 0){
        device_number = 7;
    }
    /* get device semaphore */
    device_semaphore = ((line - DISK) * DEVPERINT) + device_number;
    /* for terminal interrupts */
    if(line == TERMINAL){
        status = terminal_interrupt(device_semaphore);
    }else{
        status = ((deviceRegister->devreg[device_semaphore]).d_status);
        /* ACK the interrupt */
        (deviceRegister->devreg[device_semaphore]).d_command = ACK;
    }
    /* V operation on the device semaphore */
    semD[device_semaphore] = semD[device_semaphore] + 1;
    /* wait for i/o */
    if(semD[device_semaphore] <= 0){
        proc = removeBlocked(&(semD[device_semaphore]));
        if(proc != NULL){
            proc->p_s.s_v0 = status; /* save the process status */
            insertProcQ(&readyQue, proc);
            softBlockCount--;
        } /* end inner IF */
    }else{
        semD[device_semaphore] = status;
    } /* end outer IF */
    /* if no process is running, call the scheduler to set the next process */
    if(currentProc == NULL){
        scheduler();
    }
}/* end Device_InterruptH */

/*                                              HELPER FUNCTIONS                                                    */

/* Loop through all of the registers in the old state(copied state)
 * and write them into the new state(pasted state) */
void Copy_Paste(state_t *copied_state, state_t *pasted_state){
    int i;
    for (i = 0; i < STATEREGNUM; i++){
        pasted_state->s_reg[i] = copied_state->s_reg[i];
    }
    /*Move all of the contents from the old state into the new state*/
    pasted_state->s_entryHI = copied_state->s_entryHI;
    pasted_state->s_status = copied_state->s_status;
    pasted_state->s_pc = copied_state->s_pc;
    pasted_state->s_cause = copied_state->s_cause;
}

/* returns the device status of a terminal interrupt
 * deciphers between terminal read/write
 * terminal write takes priority */
HIDDEN int terminal_interrupt(int device_sema4) {
    unsigned int status;
    volatile devregarea_t *devReg;
    devReg = (devregarea_t *) RAMBASEADDR;
 /*   status = devReg->devreg[(device_sema4)].t_transm_status;
    *//* handle the 'write' case *//*
    if ((status & 0x0F) != READY) {
        (devReg->devreg[(device_sema4)]).t_transm_command = ACK;

    }else{ *//* handle the 'read' case *//*
        status = ((devReg->devreg[device_sema4]).t_recv_status);
        (devReg->devreg[device_sema4]).t_recv_command = ACK;
        *//* update device sema4 *//*
        device_sema4 = device_sema4 + DEVPERINT;
    }
    */
    return (status);
}




