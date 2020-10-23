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

/* debug variables */
extern int exception_check;
int aflag1;
int aflag11;

HIDDEN void blocker(int *blocking);
HIDDEN void PassUpOrDie(int Excepttrigger);

/*Not sure what the type is of what we return on sysHandler, if anything at all*/
void debugE(int a, int b, int c, int d){
	int i = 47;
	i++;
}

void sysHandler(){
    state_PTR syscall_state;
	cpu_t current_time;
	syscall_state = (state_PTR) BIOSDATAPAGE;
	exception_check = syscall_state->s_a0;
	/*exception_check = currentProc->p_s.s_a0;*/

    /* store process state */
	Copy_Paste(syscall_state, &(currentProc->p_s));
	/* update PC */
	currentProc->p_s.s_pc += 4;

	/*                  SYS 1                */
    /* situation of create process */
	if(exception_check == 1){
		pcb_PTR newPcb = allocPcb();
        support_t *data;
		data = (support_t*) currentProc->p_s.s_a2;
		currentProc->p_s.s_v0 = 0;

		/* if a new process is created */
		if(newPcb != NULL) {
            processCount++;
            Copy_Paste((state_PTR) currentProc->p_s.s_a1, &(newPcb->p_s));
            newPcb->p_supportStruct = NULL;
            if (data != NULL || data != 0){
                newPcb->p_supportStruct = data;
            } else {
                currentProc->p_s.s_v0 = READY;
                insertProcQ(&readyQue, newPcb);
                insertChild(currentProc, newPcb);
            } /* end inner if */
        } /* end outer if */
		Context_Switch(currentProc);
	} /* end create process case */

	/*                  SYS 2                */
	/* situation to terminate process */
	else if(exception_check == 2){
		while (currentProc->p_child != NULL){
			removeChild(currentProc->p_child);
		}
		outProcQ(&readyQue, currentProc);
		freePcb(currentProc);
		scheduler();
	} /* end terminate process case */

	/*                  SYS 3                */
	/* Passeren situation */
	else if(exception_check == 3){
		int *mutex = &currentProc->p_s.s_a1;
		mutex--;
		if(mutex < 0){
		    blocker(mutex);
		}
		Context_Switch(currentProc);
	} /* end PASSEREN case */

	/*                  SYS 4                */
	/* Verhogen situation */
	else if(exception_check == 4){
		int *mutex = (int *) &currentProc->p_s.s_a1;
        pcb_PTR temp;
        (*mutex)++;
		if((*mutex) <= 0){
			temp = removeBlocked(mutex);
			if(temp != NULL){
                insertProcQ(&readyQue, temp);
            }
		}
		Context_Switch(currentProc);
	} /* end Verhogen case */

	/*                  SYS 5                */
	/* Wait for I/O situation */
	else if(exception_check == 5){
		int lineNum = currentProc->p_s.s_a1;
		int devNum = currentProc->p_s.s_a2;
		/* get device sema4 using the device number */
		devNum = devNum + ((lineNum - DISK) * DEVPERINT);

		if((lineNum == TERMINT) && (currentProc->p_s.s_a3)) {
            devNum += DEVPERINT;
        }
		semD[devNum]--;
		if(semD[devNum] >= 0){
			Context_Switch(currentProc);
		}else{
            softBlockCount++;
            blocker(&(semD[devNum]));
        }
	} /* end I/O case */

	/*                  SYS 6                */
	/* get CPU time situation */
	else if(exception_check == 6){
		STCK(current_time);
		current_time = (current_time - start_clock) + currentProc->p_time;
		currentProc->p_s.s_v0 = current_time;
		Context_Switch(currentProc);
	} /* end get CPU time case */

	/*                  SYS 7                */
	/* wait for pseudo clock tick situation*/
	else if(exception_check == 7){
		semD[SEMNUM-1]--;
		if(semD[SEMNUM-1] < 0){
		    softBlockCount++;
            blocker(&(semD[SEMNUM - 1]));
        }
		Context_Switch(currentProc);
	} /* end wait for clock case */

	/*                  SYS 8                */
	/* Get support data situation */
	else if(exception_check == 8){
		currentProc->p_s.s_v0 = (int) currentProc->p_supportStruct;
		Context_Switch(currentProc);
	} /* end support pointer case */

    /*                  SYS 9+                */
	/* If syscall exception is >= 9, call passupordie */
    else{
		PassUpOrDie(GENERALEXCEPT);
	} /* end syscall exception >= 9 case */

} /* end sysHandler() */


/* Functions to handle Tlb and Program Traps.
 * Both situations fall under PassUpOrDie scenarios */
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
void PassUpOrDie(int Excepttrigger){
	/* currentProc->p_s.s_pc += 4; */
	support_t* supportStruct = currentProc->p_supportStruct;
	context_t context;
	context = currentProc->p_supportStruct->sup_exceptContext[Excepttrigger];
	if(supportStruct != NULL){
		Copy_Paste((state_t*) BIOSDATAPAGE, &(currentProc->p_supportStruct->sup_exceptState[Excepttrigger]));
		LDCXT(context.c_stackPtr, context.c_status, context.c_pc);
	}
	SYSCALL(TERMINATETHREAD, 0, 0, 0);
	scheduler();
}

/* Helper function used to block the current process. It stores off the amount of time that the process was running,
 * Inserts the current process onto the blocked queue, and calls the scheduler to get the next process */
HIDDEN void blocker(int *blocking){
	cpu_t TOD_stop;
	STCK(TOD_stop);
	currentProc->p_time = currentProc->p_time + (TOD_stop - start_clock);
	insertBlocked(blocking, currentProc);
	currentProc = NULL;
	scheduler();
}