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

#include "/home/schlampyy/umps3/src/support/libumps.h"

/* Global Variables from initial.c */
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;
extern int semD[SEMNUM];
/* Variables for maintaining CPU time in scheduler.c*/
extern cpu_t compuTime;
extern cpu_t QuantumStart;


/*Not sure what the type is of what we return on sysHandler, if anything at all*/

void sysHandler(){
	if(currentProc->p_s.s_a0 = 1){ /*situation of create process*/
		pcb_t newPcb->p_s = a1;
		newPcb->p_supportStruct = a2;
		insertProcQ(newPcb, newPcb->p_next);
		insertChild(newPcb, newPcb->p_child);
		newPcb->p_time = 0;
		newPcb->p_semadd = NULL;
		scheduler();
		int retValue = SYSCALL(1,statet*statep, supportt*supportp, 0);
	}
	if(currentProc->p_s.s_a0 = 2) /*situation to terminate process*/
	{
		while (currentProc->p_child != NULL)
		{
			removeChild(currentProc->p_child);
		}
		outProcQ(&readyQue, currentProc);
		scheduler()
		SYSCALL(2, 0, 0, 0);
	}
	if(currentProc->p_s.s_a0 = 3) /*Passeren situation, dont think this is the correct syntax but this is what he put on the board in class*/
	{
		WAIT(mutex){
			mutex--;
			if(mutex < 0)
				WAIT();
		}
		SYSCALL (3, int*semaddr, 0, 0);
	}
	if(currentProc->p_s.s_a0 = 4) /*Verhogen situation, same notes as above */
	{
		SIGNAL(mutex);
		mutex++;
		SYSCALL (4, int*semaddr, 0, 0);
	}
	if(currentProc->p_s.s_a0 = 5) /*I/O situation*/
	{
		int ioStatus = SYSCALL (5, int intlNo,int dnum, int waitForTermRead);
	}
	if(currentProc->p_s.s_a0 = 6) /*get CPU time situation */
	{
		cpu_t cpuTime = SYSCALL (6, 0, 0, 0);
		return cpuTime;
	}
	if(currentProc->p_s.s_a0 = 7) /*wait clock situation*/
	{
		SYSCALL (7, 0, 0, 0);
	}
	if(currentProc->p_s.s_a0 = 8) /*support pointer situation */
	{
		supportt*sPtr = SYSCALL (8, 0, 0, 0);
	}
}