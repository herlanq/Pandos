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

#include "../h/libumps.h"

/* Global Variables from initial.c */
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;
extern int semD[SEMNUM];


/*Not sure what the type is of what we return on sysHandler, if anything at all*/

void sysHandler(){
	int mutex = 0;

	if(currentProc->p_s.s_a0 = 1){ /*situation of create process*/
		pcb_PTR newPcb;
		newPcb->p_s.s_a2 = currentProc->p_s.s_a2;
		newPcb->p_supportStruct = currentProc->p_s.s_a2;
		insertProcQ(newPcb, newPcb->p_next);
		insertChild(newPcb, newPcb->p_child);
		newPcb->p_time = 0;
		newPcb->p_semAdd = NULL;
		currentProc->p_s.s_pc += 4;
		scheduler();
	}
	else if(currentProc->p_s.s_a0 = 2) /*situation to terminate process*/
	{
		while (currentProc->p_child != NULL)
		{
			removeChild(currentProc->p_child);
		}
		outProcQ(&readyQue, currentProc);
		freePcb(currentProc);
		currentProc->p_s.s_pc += 4;
		scheduler();
	}
	else if(currentProc->p_s.s_a0 = 3) /*Passeren situation, dont think this is the correct syntax but this is what he put on the board in class*/
	{
		currentProc->p_s.s_pc += 4;
		mutex--;
		if(mutex < 0){
			insertBlocked(&mutex, currentProc);
			scheduler();
		}

		
	}
	else if(currentProc->p_s.s_a0 = 4) /*Verhogen situation, same notes as above */
	{
		mutex++;
		currentProc->p_s.s_pc += 4;
		if(mutex <= 0){
			int temp = removeBlocked(&mutex);
			insertProcQ(&readyQue, temp);
			scheduler();
		}
		
	}
	else if(currentProc->p_s.s_a0 = 5) /*I/O situation*/
	{
		currentProc->p_s.s_pc += 4;
		/*SYS3(PASSEREN, currentProc->p_semadd, 0,0);*/
		setSTATUS(ALLOFF | IECON | IMON | TEBITON);
		scheduler();
	}
	else if(currentProc->p_s.s_a0 = 6) /*get CPU time situation */
	{
		currentProc->p_s.s_v0 = currentProc->p_time;
		currentProc->p_s.s_pc += 4;
	}
	else if(currentProc->p_s.s_a0 = 7) /*wait clock situation*/
	{
		currentProc->p_s.s_pc += 4;
		/*SYS3(PASSEREN, currentProc->p_semadd, 0,0);*/
		setSTATUS(ALLOFF | IECON | IMON | TEBITON);
		scheduler();
	}
	else if(currentProc->p_s.s_a0 = 8) /*support pointer situation */
	{
		if(currentProc->p_supportStruct == NULL)
			currentProc->p_s.s_v0 = NULL;
		currentProc->p_s.s_v0 = currentProc->p_supportStruct;
		currentProc->p_s.s_pc += 4;
	}
	else if(currentProc->p_s.s_a0 >= 9)
		currentProc->p_s.s_pc += 4;
		PassUpOrDie();
}

void TlbTrapHandler(){
	/*need to pull the exception from the register and pass to PassUpOrDie; */
	int trigger;
	/*pcb_PTR old_state = (state_PTR) something
	trigger = old_state & EXCEPTIONS;*/
	PassUpOrDie(trigger);
}

void PrgTrapHandler(){
	/*need to pull the exception from the register and pass to PassUpOrDie; */
	int trigger;
	/*pcb_PTR old_state = (state_PTR) something
	trigger = old_state & EXCEPTIONS;*/
	PassUpOrDie(trigger);
}

/* If an exception has been encountered, it passes the error to the appropriate handler, if no exception
 * is found, it kills the process and all of its children
 * cases:
 * 1 - TLB Trap Handler
 * 2 - Program Trap Handler
 * 3 - Syscall 9+
 */

/*new passuporDIE */
void PassUpOrDie(int Excepttrigger){
	context_t context;
	support_t sup;
	context.c_stackPtr = currentProc->p_s.s_sp;
	context.c_pc = currentProc->p_s.s_pc;
	context.c_status = currentProc->p_s.s_status;
	/*sup->sup_asid = currentProc->p_somthing with process ID */
	sup.sup_exceptState = currentProc->p_s.s_cause;
	state_PTR oldState;
	state_PTR currState;
	/*sup->sup_exceptContext = currentProc->context for some sort of thing 
	state_PTR oldState = currentProc->p_oldState;
	state_PTR currState = currentProc->p_newState; */

	switch (Excepttrigger){

		case TLBRTRAP:
		if(currentProc->p_supportStruct == NULL)
			Syscall2(TERMINATETHREAD,0,0,0);
		else{
			Copy_Paste(oldState, currState);
			scheduler();
		}
		break;

		case PROGTRAP:
		if(currentProc->p_supportStruct == NULL)
			Syscall2(TERMINATETHREAD,0,0,0);
		else{
			Copy_Paste(oldState, currState);
			scheduler();
		}
		break;
		
		case SYSTRAP:
			if(currentProc->p_supportStruct == NULL)
				Syscall2(TERMINATETHREAD,0,0,0);
			else{
				Copy_Paste(oldState, currState);
				scheduler();
			}
		break;
	}
}