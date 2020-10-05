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
void callScheduler();

/* Function that determines the highest priority interrupt and gives control the to scheduler*/
void InterruptHandler(){

}

