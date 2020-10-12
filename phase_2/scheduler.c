/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/27
 *
 * The Nucleus guarantees finite progress (does not allow starvation) thus, every ready process will have
 * an opportunity to execute. This module implements a simple preemptive round-robin scheduling algorithm with
 * a time slice value of 5 milliseconds.
 */
#include "../h/const.h"
#include "../h/types.h"

#include "../h/asl.h"
#include "../h/pcb.h"

#include "../h/initial.h"
#include "../h/interrupts.h"
#include "../h/exceptions.h"
#include "../h/scheduler.h"

#include "../h/libumps.h"

/* Variables used from initial.c */
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;

/* 'scheduler()' uses a round Robin algorithm to schedule each process that it is going to be executed.
 */
void scheduler(){
    /* set new process block pointer */
    pcb_t *proc;

    /* remove process from head of the ready que and set it as new process */
    proc = removeProcQ(&readyQue);

    if(proc != NULL){ /* if removed process is not null*/
        Ready_Timer(proc, QUANTUM);
    }
   /*if(proc == NULL) {
        currentProc = NULL;
   }
    */

    /* check for remaining processes */
    if(processCount == 0){ /* if procCNT is equal to 0, everything finished running properly */
        HALT();
    }else{
        /* if there are still processes to be run */
        if (processCount > 0) {
            if (softBlockCount == 0){ /* have processes but not on ready queue or blocked queue */
                PANIC(); /* 'oh shit' moment */
            }
            if (softBlockCount > 0) {
                currentProc = NULL;
                /* disable timer by loading it with a large value */
                setTIMER(MAXTIME);

                /* have processes that are blocked, need to wait with interrupts and exceptions enabled
                 * "Twiddling Thumbs" */
                setSTATUS(ALLOFF | IECON | IMON | TEBITON);
                WAIT();
            }
        }
    }
}
/*                     HELPER FUNCTIONS TO DEAL WITH CONTEXT SWITCHES              */

/* Gives control over the machine to another process */
void Context_Switch(pcb_PTR this_proc){
    currentProc = this_proc;
    /* load state of the current process... switching context */
    LDST(&(currentProc->p_s));

}
/* preps the interval timer for a new process a given context switch
 * or preps the timer for an old process given a V operation or interrupt.
 * Gives control to the */
void Ready_Timer(pcb_PTR current_process, cpu_t time){
    cpu_t start_time;
    start_time = ((QUANTUM) - (current_process->p_time));
    STCK(start_time);
    /* set amount of time given for the process */
    setTIMER(time);
    /* context switch, make this process the current process */
    Context_Switch(current_process);
}

