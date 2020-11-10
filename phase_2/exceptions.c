/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 10/26
 *
 * A Syscall exception occurs when the SYSCALL assembly instruction is executed. If the syscall request in not in
 * kernel mode (in user mode) or the syscall is not syscalls 1-8, this invokes a pass up or die scenario.
 * If the that is process making a SYSCALL request is in kernel mode and a0 contains a syscall value of 1-8 then the
 * Nucleus should perform the proper syscall given the value that is in a0.
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

/* local function declaration */
HIDDEN void blocker(int *blocking); /* helper function to block processes */
HIDDEN void PassUpOrDie(int Excepttrigger); /* function used to kill a pass up a process or terminate it and all of its children */
HIDDEN void terminate_process(pcb_PTR term_proc); /* helper function used to recursively terminate a process an all of its children */


/* If the syscall request in not in kernel mode (in user mode) or the syscall is not syscalls 1-8, this invokes a
 * pass up or die scenario. If the that is process making a SYSCALL request is in kernel mode and a0 contains a syscall
 * value of 1-8 then the Nucleus should perform the proper syscall given the value that is in a0. The sysHanlder handles
 * the request by determining the appropriate syscall based on the syscall number.
*/
void sysHandler(){
    state_PTR syscall_state; /* address of system state */
	cpu_t current_time; /* current time */
    int syscall; /* current syscall number */

	syscall_state = (state_PTR) BIOSDATAPAGE;
	syscall = syscall_state->s_a0;
	int usermode = (currentProc->p_s.s_status & UMON);

	/* check for user mode
	 * if syscall > 8, or in user mode, pass up or die */
	if(syscall >= 1 && syscall <= 8 && usermode != 0){
	    PassUpOrDie(GENERALEXCEPT);
	}

    /* store process state */
	Copy_Paste(syscall_state, &(currentProc->p_s));

	/* update PC */
	currentProc->p_s.s_pc += 4;

/*                                             Begin Syscall Handler                                                  */

	/*                  SYS 1                */
    /* situation of create process */
	if(syscall == 1){
		pcb_PTR newPcb = allocPcb();
        support_t *data;
		data = (support_t*) currentProc->p_s.s_a2;
		currentProc->p_s.s_v0 = 0;

		/* if a new process is created */
		if(newPcb != NULL) {
            processCount += 1;
            Copy_Paste((state_PTR) currentProc->p_s.s_a1, &(newPcb->p_s));
            newPcb->p_supportStruct = NULL;
            if (data != NULL || data != 0){
                newPcb->p_supportStruct = data;
            }
            currentProc->p_s.s_v0 = READY;
            insertProcQ(&readyQue, newPcb);
            insertChild(currentProc, newPcb);
        }
		/* switch context */
		Context_Switch(currentProc);
	} /* end create process case */

	/*                  SYS 2                */
	/* situation to terminate process */
	else if(syscall == 2){
	    terminate_process(currentProc);
		scheduler();
	} /* end terminate process case */

	/*                  SYS 3                */
	/* Passeren situation */
	else if(syscall == 3){
		int *mutex;
		mutex = (int*)currentProc->p_s.s_a1;
        (*mutex) -= 1;
        /* block and call scheduler or switch context */
		if((*mutex) < 0){
		    blocker(mutex);
		}else{
            Context_Switch(currentProc);
		}
	} /* end PASSEREN case */

	/*                  SYS 4                */
	/* Verhogen situation */
	else if(syscall == 4){
		int *mutex;
		mutex = (int *)currentProc->p_s.s_a1;
        pcb_PTR temp;
        (*mutex) += 1;
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
	else if(syscall == 5){
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
            softBlockCount += 1;
            blocker(&(semD[devNum]));
        }
	} /* end I/O case */

	/*                  SYS 6                */
	/* get CPU time situation */
	else if(syscall == 6){
		STCK(current_time);
		current_time = (current_time - start_clock) + currentProc->p_time;
		currentProc->p_s.s_v0 = current_time;
		Context_Switch(currentProc);
	} /* end get CPU time case */

	/*                  SYS 7                */
	/* wait for pseudo clock tick situation*/
	else if(syscall == 7){
        (semD[SEMNUM-1])--;
		if((semD[SEMNUM-1]) < 0){
		    softBlockCount +=1 ;
            blocker(&(semD[SEMNUM - 1]));
        }
		Context_Switch(currentProc);
	} /* end wait for clock case */

	/*                  SYS 8                */
	/* Get support data situation */
	else if(syscall == 8){
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
	if(currentProc->p_supportStruct != NULL){
		Copy_Paste((state_t*) BIOSDATAPAGE, &(currentProc->p_supportStruct->sup_exceptState[Excepttrigger]));
		LDCXT(currentProc->p_supportStruct->sup_exceptContext[Excepttrigger].c_stackPtr,
              currentProc->p_supportStruct->sup_exceptContext[Excepttrigger].c_status,
              currentProc->p_supportStruct->sup_exceptContext[Excepttrigger].c_pc);
	}
    terminate_process(currentProc);
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

/* Helper function used to recursively kill a process and all of its children */
HIDDEN void terminate_process(pcb_PTR term_proc){
    pcb_PTR proc; /* temp proc pointer */
    int *temp; /* temp sema4 pointer */

    /* recursively call terminate_process until the process has no more children */
    while(!emptyChild(term_proc)){
        terminate_process(removeChild(term_proc));
    }
    if(term_proc == currentProc){
        outChild(term_proc);
    }else if(term_proc->p_semAdd == NULL){
        outProcQ(&readyQue, term_proc);
    }else{
        proc = outBlocked(term_proc);
        if(proc != NULL){
            temp = proc->p_semAdd;
            if(temp >= &semD[0] && temp <= &semD[SEMNUM-1]){ /* update the softblock count */
                softBlockCount -= 1;
            }else{ /* V the semaphore */
                (*temp)++;
            }
        }
    }
    freePcb(term_proc); /* free up the terminated process's pcb */
    processCount -= 1; /* since the process is being terminated and the pcb is freed up, decrement the proc count */

    /* if there are no more processes to be run, invoke the scheduler to halt the system */
    if(processCount == 0){
        scheduler();
    }
}