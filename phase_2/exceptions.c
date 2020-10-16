/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 10/15
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
extern cpu_t start_clock;
extern int exception_check;
extern unsigned int device_status[SEMNUM-1];

HIDDEN void blocker(int devNum);
HIDDEN void PassUpOrDie(int Excepttrigger);

/*Not sure what the type is of what we return on sysHandler, if anything at all*/

void sysHandler(){
	currentProc->p_s.s_pc += 4;
	exception_check = currentProc->p_s.s_a0;
	if(currentProc->p_s.s_a0 == 1){ /*situation of create process*/
		pcb_PTR newPcb = allocPcb();
		if(newPcb == NULL){
			currentProc->p_s.s_v0 = -1;
			return;
		}
		Copy_Paste((state_t*) currentProc->p_s.s_a1, &(newPcb->p_s));
		if(&currentProc->p_s.s_a2 == NULL){
			newPcb->p_supportStruct = NULL;
		}
		else{
			newPcb->p_supportStruct->sup_asid = currentProc->p_s.s_a2;
		}
		insertProcQ(&readyQue, newPcb);
		insertChild(currentProc, newPcb);
		currentProc->p_s.s_v0 = 1;
		scheduler();
	}
	else if(currentProc->p_s.s_a0 == 2) /*situation to terminate process*/
	{
		while (currentProc->p_child != NULL)
		{
			removeChild(currentProc->p_child);
		}
		outProcQ(&readyQue, currentProc);
		freePcb(currentProc);
		scheduler();
	}
	else if(currentProc->p_s.s_a0 ==3) /*Passeren situation, dont think this is the correct syntax but this is what he put on the board in class*/
	{
		int *mutex = &currentProc->p_s.s_a1;
		mutex--;
		if(mutex < 0){
			cpu_t stop_clock;
			STCK(stop_clock);
			currentProc->p_time = currentProc->p_time + (stop_clock - start_clock);
			insertBlocked(mutex, currentProc);
			scheduler();
		}
		Context_Switch(currentProc);

		
	}
	else if(currentProc->p_s.s_a0 == 4) /*Verhogen situation, same notes as above */
	{
		int *mutex = &currentProc->p_s.s_a1;
		mutex++;
		if(mutex <= 0){
			pcb_PTR temp = removeBlocked(mutex);
			insertProcQ(&readyQue, temp);
		}
		Context_Switch(currentProc);
		
	}
	else if(currentProc->p_s.s_a0 == 5) /*I/O situation*/
	{
		int lineNum = currentProc->p_s.s_a1;
		int devNum = (currentProc->p_s.s_a2) + ((lineNum-3)*DEVPERINT);
		if(lineNum == TERMINT)
			devNum = devNum + DEVPERINT;
		semD[devNum]--;
		if(semD[devNum] >= 0){
			currentProc->p_s.s_v0 = device_status[devNum];
			Context_Switch(currentProc);
		}
		softBlockCount++;
		blocker(semD[devNum]);
	}
	else if(currentProc->p_s.s_a0 == 6) /*get CPU time situation */
	{
		cpu_t current_TOD;
		STCK(current_TOD);
		current_TOD = (current_TOD - start_clock) + currentProc->p_time;
		currentProc->p_s.s_v0 = current_TOD;
		scheduler();
	}
	else if(currentProc->p_s.s_a0 == 7) /*wait clock situation*/
	{
		semD[SEMNUM]--;

		if(semD[SEMNUM] >= 0)
			scheduler();
		blocker(semD[SEMNUM]);
	}
	else if(currentProc->p_s.s_a0 == 8) /*support pointer situation */
	{
		if(currentProc->p_supportStruct == NULL)
			currentProc->p_s.s_v0 = (int) NULL;
		currentProc->p_s.s_v0 = (int) currentProc->p_supportStruct;
		scheduler();
	}
	else if(currentProc->p_s.s_a0 >= 9)
		PassUpOrDie(GENERALEXCEPT);
}

void TlbTrapHandler(){
	PassUpOrDie(PGFAULTEXCEPT);
}

void PrgTrapHandler(){
	PassUpOrDie(GENERALEXCEPT);
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
	currentProc->p_s.s_pc += 4;
	support_t* supportStruct = currentProc->p_supportStruct;
	context_t context;
	context = currentProc->p_supportStruct->sup_exceptContext[Excepttrigger];
	if(supportStruct != NULL)
	{
		Copy_Paste((state_t*) BIOSDATAPAGE, &(currentProc->p_supportStruct->sup_exceptState[Excepttrigger]));
		LDCXT(context.c_stackPtr, context.c_status, context.c_pc);
	}
	SYSCALL(TERMINATETHREAD, 0, 0, 0);
	scheduler();
	
}

void blocker(int devNum){
	cpu_t TOD_stop;
	STCK(TOD_stop);
	currentProc->p_time = currentProc->p_time + (TOD_stop - start_clock);
	insertBlocked(&semD[devNum], currentProc);
	currentProc = NULL;
	scheduler();
}