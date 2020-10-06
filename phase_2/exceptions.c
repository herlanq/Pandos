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
/* Variables for maintaining CPU time in scheduler.c*/
extern cpu_t compuTime;
extern cpu_t QuantumStart;


/*Not sure what the type is of what we return on sysHandler, if anything at all*/

void sysHandler(){
	int mutex;

	if(currentProc->p_s.s_a0 = 1){ /*situation of create process*/
		pcb_t newPcb->p_s = a1;
		newPcb->p_supportStruct = a2;
		insertProcQ(newPcb, newPcb->p_next);
		insertChild(newPcb, newPcb->p_child);
		newPcb->p_time = 0;
		newPcb->p_semadd = NULL;
		currentProc->p_s.s_pc += 4;
		LDST(currentProc->p_s);
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
			insertBlocked(&mutex, currentProc)
			scheduler();
			LDST(currentProc->p_s);
		}

		
	}
	else if(currentProc->p_s.s_a0 = 4) /*Verhogen situation, same notes as above */
	{
		mutex++;
		currentProc->p_s.s_pc += 4;
		if(mutex <= 0){
			int temp = removeBlocked(&mutex);
			insertProcQ(&readyQue, temp);
			LDST(currentProc->p_s);
		}
		
	}
	else if(currentProc->p_s.s_a0 = 5) /*I/O situation*/
	{
		currentProc->p_s.s_pc += 4;
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
		scheduler();
	}
	else if(currentProc->p_s.s_a0 = 8) /*support pointer situation */
	{
		if(p_supportStruct == NULL)
			currentProc->p_s.s_v0 = NULL;
		currentProc->p_s.s_v0 = p_supportStruct;
		currentProc->p_s.s_pc += 4;
	}
	else if(currentProc->p_s.s_a0 >= 9)
		currentProc->p_s.s_pc += 4;
		PassUpOrDie();
}

void TlbTrapHandler(){
	PassUpOrDie();
}

void PrgTrapHandler(){
	PassUpOrDie();
}

/* If an exception has been encountered, it passes the error to the appropriate handler, if no exception
 * is found, it kills the process and all of its children
 * cases:
 * 1 - TLB Trap Handler
 * 2 - Program Trap Handler
 * 3 - Syscall 9+
 */
void PassUpOrDie(state_t *caller, int trigger){
    /* what exception is triggering */
    switch (trigger){

        /*0 is TLB EXCEPTIONS*/
        case TLBTRAP:
        if((currentProc-> newTLBstate) == NULL){
            S;
        }else{
            CopyState(caller, currentProc-> oldTLBstate);
            LDST(currentProc-> newTLBstate);
        }
        break;

        /*1 is Program Trap Exceptions*/
        case PROGTRAP:
            if((currentProc-> newPRGstate) == NULL){
                Syscall2();
            }else{
                CopyState(caller, currentProc-> oldPRGstate);
                LDST(currentProc-> newPRGstate);
            }
            break;

        /*2 is SYS Exception!*/
        case SYSTRAP:
            if((currentProc->p_newState) == NULL){
                Syscall2();
            }else{
                CopyState(caller, currentProc->p_oldState);
                LDST(currentProc->p_newState);
            }
            break;
    }
}