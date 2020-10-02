/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/27
 *
 * The Nucleus guarantees finite progress (does not allow starvation) thus, every ready process will have
 * an opportunity to execute. The Nucleus implements a simple preemptive round-robin scheduling algorithm with
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

/* Global Variables for scheduler.c
 * Tracks the timing of each running process */
cpu_t compuTime; /* Keep track of time spent computing */
cpu_t QuantumStart;

/* Round Robin algorithm that schedules each process that it is going to be executed.
 * Under certain conditions, it PANICS or performs the appropriate function call.
 */
void scheduler(){
    if(currentProc != NULL){ /* if the current process is null */
        STCK(compuTime); /* get amount of time the process was running */
        /* set current proc time to how long the process was running in cpu */
        currentProc->p_time = (currentProc->p_time) + (compuTime - QuantumStart);
    }
    /* set new process block pointer */
    pcb_t *proc;
    /* remove process from head of the ready que and set it as new process */
    proc = removeProcQ(&readyQue);

    if(proc != NULL){ /* if removed process is not null*/
        currentProc = proc;
        STCK(QuantumStart);
        setTimer(QUANTUM);
        LDST(&(currentProc->p_s));
    }
    if(proc == NULL){ /* if removed process is null */
        currentProc = NULL;
        if(processCount == 0){ /* if procCNT is equal to 0, correct */
            HALT();
        }
        if(processCount > 0){ /* if there are still processes to be executed */
            if(softBlockCount == 0){
                PANIC();
            }
            /* need to wait */
            setSTATUS();
            WAIT();
        }
    }
}

