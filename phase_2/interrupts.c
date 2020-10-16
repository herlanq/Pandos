/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 10/15
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
extern int exception_check;
extern unsigned int device_status[SEMNUM-1];
int termChecker;
int devcheck;
/* separate functions for interrupt handling */
HIDDEN void Device_InterruptH(int line);
HIDDEN int terminal_interruptH(int *devSem);

/* Function that determines the highest priority interrupt and
 * gives control the to scheduler.
 * PLS interrupts and Pseudo clock interrupts have their own separate handler functions */
void InterruptHandler(){
    cpu_t stop_clock;
    cpu_t time_left;
    time_left = getTIMER();
    STCK(stop_clock);
    /*state_PTR int_cause = ((state_PTR) BIOSDATAPAGE);*/

    /* BEGIN INTERRUPT HANDLING */

    /* PLT interrupt, quantum is up, time to switch to the next process */
    if ((((state_PTR)BIOSDATAPAGE)->s_cause & PLTINT) != 0) {
        if(currentProc != NULL){
            /* compute time of the running process, move the processor state, call on the next process */
            currentProc->p_time = (currentProc->p_time) + (stop_clock - start_clock);
            Copy_Paste((state_PTR) BIOSDATAPAGE, &(currentProc->p_s));
            insertProcQ(&readyQue, currentProc);
            Context_Switch(currentProc); /* not sure if this is correct? It has to switch to a new proc */ /* switched to context switch */
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
            softBlockCount--;
            proc = removeBlocked(&semD[SEMNUM-1]); /*CLOCKSEM = semD[SEMNUM-1]*/
        }
        semD[SEMNUM-1] = 0; /*CLOCKSEM = semD[SEMNUM-1]*/
        if(currentProc == NULL){
            Context_Switch(currentProc);
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

    if(currentProc != NULL){
        /* assign run time before the interrupt to the current process
         * and return control to the current running process */
        currentProc->p_time = currentProc->p_time + (stop_clock + start_clock);
        Copy_Paste(((state_PTR)BIOSDATAPAGE), &(currentProc->p_s));
        Ready_Timer(currentProc, time_left);
        /* else: no current proc */
    }else{
        HALT();
    }
}

/* Interrupt handler for peripheral devices.
 * V's the correct device semaphore and stores the device data. */
void Device_InterruptH(int line){
    unsigned int bitMAP;
    volatile devregarea_t *deviceRegister;
    /* Addressing */
    /*currentProc->p_s.s_v0 = device_status[devNum]; this line was in our exceptions for SYS5, removed by mikey. */
    deviceRegister = (devregarea_t *) RAMBASEADDR;
    bitMAP = deviceRegister->interrupt_dev[line-DISK];
    int device_number; /* interrupt device number */
    int device_semaphore; /* interrupt device semaphore */
    unsigned int status; /* status of the interrupting device */
    pcb_PTR proc;

    if((bitMAP & DEV0) != 0){ /*possibly hitting this function logic every time? maybe check the bitMap logic with the constants */
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
    devcheck = 47;
    devcheck = device_number; /*I put this down here to show that we are never setting device number to any value. it remains zero. */

    /* for terminal interrupts */
    if(line == TERMINAL){ /* distinguish between read/write cases */
        status = terminal_interruptH(&device_semaphore);
    }else{
    	termChecker = 69420;
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
        device_status[device_semaphore] = status;
    } /* end outer IF */

    /* if no process is running, call the scheduler to set the next process */
    if(currentProc == NULL){
        Context_Switch(currentProc);
    }
}/* end Device_InterruptH */

/*                                          Terminal Interrupt Handler                                               */

HIDDEN int terminal_interruptH(int *devSem){
    volatile devregarea_t *deviceRegister;
    unsigned int status;
    deviceRegister = (devregarea_t *) RAMBASEADDR;
    if ((deviceRegister->devreg[*devSem].t_transm_status & 0x0F) != READY) { /* handle write */
        termChecker++; /*we are hitting this point which is correct because we are writing, but we never back out of this point, or write anymore than the P */
        status = deviceRegister->devreg[*devSem].t_transm_status;
        deviceRegister->devreg[*devSem].t_transm_command = ACK;
    }else{ /* handle read */
        termChecker--;
        status = deviceRegister->devreg[*devSem].t_recv_status;
        deviceRegister->devreg[*devSem].t_recv_command = ACK;
        *devSem = *devSem + DEVPERINT;
    }
    return(status);
}


/*                                              HELPER FUNCTIONS                                                    */

/* Loop through all of the registers in the old state(copied state)
 * and write them into the new state(pasted state) */
void Copy_Paste(state_t *copied_state, state_t *pasted_state){
    int i;
    exception_check = currentProc->p_s.s_a0;
    for (i = 0; i < STATEREGNUM; i++){
        pasted_state->s_reg[i] = copied_state->s_reg[i];
    }
    /*Move all of the contents from the old state into the new state*/
    pasted_state->s_entryHI = copied_state->s_entryHI;
    pasted_state->s_status = copied_state->s_status;
    pasted_state->s_pc = copied_state->s_pc;
    pasted_state->s_cause = copied_state->s_cause;
}





