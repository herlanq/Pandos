/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 10/26
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
extern cpu_t start_clock;

/* separate functions for interrupt handling */
HIDDEN void Device_InterruptH(int line);
HIDDEN int terminal_interruptH(int *devSem);


/* Function that determines the highest priority interrupt and
 * gives control the to scheduler.
 * PLS interrupts and Pseudo clock interrupts have their own separate handler functions */
void InterruptHandler(){
    cpu_t stop_clock;
    cpu_t time_left;

    STCK(stop_clock);
    time_left = getTIMER();

    /* BEGIN INTERRUPT HANDLING */

    /* PLT interrupt, quantum is up, time to switch to the next process */
    if ((((state_PTR)BIOSDATAPAGE)->s_cause & PLTINT) != 0) {
        if(currentProc != NULL){
            /* compute time of the running process, move the processor state, call on the next process */
            currentProc->p_time = (currentProc->p_time) + (stop_clock - start_clock);
            /* store processor state */
            Copy_Paste((state_PTR) BIOSDATAPAGE, &(currentProc->p_s));
            /* add process back on to the ready que and switch to another process */
            insertProcQ(&readyQue, currentProc);
            scheduler();
            /* else: no current proc */
        }else{
            PANIC();
        }
    }

    /* Pseudo clock tick interrupt
     * Awaken all of the processes that are waiting on the pseudo clock */
    if((((state_PTR)BIOSDATAPAGE)->s_cause & TIMERINT) != 0){
        pcb_PTR proc;
        LDIT(PSUEDOCLOCKTIME);
        proc = removeBlocked(&semD[SEMNUM-1]); /*CLOCKSEM = semD[SEMNUM-1]*/
        while(proc !=NULL){
            insertProcQ(&readyQue, proc);
            softBlockCount -= 1;
            proc = removeBlocked(&semD[SEMNUM-1]); /*CLOCKSEM = semD[SEMNUM-1]*/
        }
        /* set the semaphore to = 0 */
        semD[SEMNUM-1] = 0; /*CLOCKSEM = semD[SEMNUM-1]*/
        if(currentProc == NULL){
            scheduler();
        }
    }

    /* Disk interrupt */
    if((((state_PTR)BIOSDATAPAGE)->s_cause & DISKINT) != 0){
        /* disk dev is on */
        Device_InterruptH(DISK);
    }

    /* Flash interrupt */
    if((((state_PTR)BIOSDATAPAGE)->s_cause & FLASHINT) != 0){
        /* flash dev is on */
        Device_InterruptH(FLASH);
    }

    /* Printer interrupt */
    if((((state_PTR)BIOSDATAPAGE)->s_cause & PRINTERINT) != 0) {
        /* printer dev is on */
        Device_InterruptH(PRINTER);
    }

    /* Terminal interrupt (special case) */
    if((((state_PTR)BIOSDATAPAGE)->s_cause & TERMINT) != 0) {
        /* terminal dev is on */
        Device_InterruptH(TERMINAL);
    }

    /* assign run time before the interrupt to the current process
     * and return control to the current running process */
    if(currentProc != NULL){
        currentProc->p_time = currentProc->p_time + (stop_clock - start_clock);
        Copy_Paste(((state_PTR)BIOSDATAPAGE), &(currentProc->p_s));
        Ready_Timer(currentProc, time_left);
        /* else: no current proc */
    }else{
        HALT();
    }
} /* end InterruptHandler */

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
    unsigned int intstatus; /* register status of the interrupting device */
    pcb_PTR p;

    /* logic to determine which specific device number is causing an interrupt */
    if((bitMAP & DEV0) != 0){
        device_number = 0;
    }else if((bitMAP & DEV1) != 0){
        device_number = 1;
    }else if((bitMAP & DEV2) != 0){
        device_number = 2;
    }else if((bitMAP & DEV3) != 0){
        device_number = 3;
    }else if((bitMAP & DEV4) != 0){
        device_number = 4;
    }else if((bitMAP & DEV5) != 0){
        device_number = 5;
    }else if((bitMAP & DEV6) != 0){
        device_number = 6;
    }else{
        device_number = 7;
    }

    /* get device semaphore */
    device_semaphore = ((line - DISK) * DEVPERINT) + device_number;

    /* For terminal interrupts */
    if(line == TERMINAL){
        intstatus = terminal_interruptH(&device_semaphore); /* call function for handling terminal interrupts */

    /* if not a terminal device interrupt */
    }else{
        intstatus = ((deviceRegister->devreg[device_semaphore]).d_status);
        /* ACK the interrupt */
        (deviceRegister->devreg[device_semaphore]).d_command = ACK;
    }

    /* V operation on the device semaphore */
    semD[device_semaphore] = semD[device_semaphore] + 1;

    /* if already waited for i/o */
    if(semD[device_semaphore] <= 0) {
        p = removeBlocked(&(semD[device_semaphore]));
        if (p != NULL) {
            p->p_s.s_v0 = intstatus; /* save status */
            insertProcQ(&readyQue, p); /* insert the process onto the ready queue */
            softBlockCount -= 1; /* update SBC*/
        }
    }
    /* if no process is running, call the scheduler to set the next process */
    if(currentProc == NULL){
        scheduler();
    }
}/* end Device_InterruptH */

/*                                          Terminal Interrupt Handler                                               */
/* Returns the device status specified by a terminal read or terminal write case */
HIDDEN int terminal_interruptH(int *devSem){
    volatile devregarea_t *deviceRegister;
    unsigned int status;
    deviceRegister = (devregarea_t *) RAMBASEADDR;

    /* terminal write case takes priority over terminal read case */
    if ((deviceRegister->devreg[(*devSem)].t_transm_status & 0x0F) != READY) { /* handle write casse */
        status = deviceRegister->devreg[(*devSem)].t_transm_status;
        deviceRegister->devreg[(*devSem)].t_transm_command = ACK;

    }else{ /* handle read case if not write case */
        status = deviceRegister->devreg[(*devSem)].t_recv_status;
        deviceRegister->devreg[(*devSem)].t_recv_command = ACK;
        /* update dev sema4 for the terminal read situation */
        (*devSem) = (*devSem) + DEVPERINT;
    }
    return(status);
}


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







