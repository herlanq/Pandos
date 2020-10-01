/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 9/27
 *
 *
 *
 */

#include "../h/const.h"
#include "../h/types.h"

#include "../h/pcb.h"
#include "../h/asl.h"

#include "../h/initial.h"
#include "../h/interrupts.h"
#include "../h/exceptions.h"
#include "../h/scheduler.h"

/* Global Variables from initial.c */
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;
extern int semD[SEMNUM];
/* Variables for maintaining CPU time in scheduler.c*/
extern cpu_t compuTime;
extern cpu_t QuantumStart;

void sysHandler(/*passed in a0 register i think*/){
	if(/*a0 equals 1*/){ /*situation of create process*/
		pcb_t newPcb->p_s = a1;
		newPcb->p_supportStruct = a2;
		insertProcQ(newPcb, newPcb->p_next);
		intertChild(newPcb, newPcb->p_child);
		newPcb->p_time = 0;
		newPcb->p_semadd = NULL;
		int retValue = SYSCALL (1,statet*statep, supportt*supportp, 0);
	}
	if(/*a0 equals 2*/)
	{
		SYSCALL (2, 0, 0, 0);
	}
	if(/*a0 equals 3*/)
	{

		SYSCALL (3, int*semaddr, 0, 0);
	}
	if(/*a0 equals 4*/)
	{

		SYSCALL (4, int*semaddr, 0, 0);
	}
	if(/*a0 equals 5*/)
	{
		
	}
}