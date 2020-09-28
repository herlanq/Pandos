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

/* Variables used from initial.c */
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;

/* Global Variables for scheduler.c
 * Tracks the timing of each running process */
cpu_t compuTime; /* Keep track of time spent computing */
cpu_t QuantumStart;

void scheduler(){

}

