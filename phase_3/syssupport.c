/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 12/05
 *
 * The SysSupport module contains the portion of the OS that handles user system calls. The function SysSupport() is
 * Phase 3's exception handler. It retrieves data from the support struct in order to determine the proper action to take.
 * If the cause is a syscall exception, it calls the function uSysHandler, which is the user syscall handler, otherwise
 * it terminates the process.
 *
 */
#include "../h/const.h"
#include "../h/types.h"
#include "../h/libumps.h"
#include "../h/syssupport.h"
#include "../h/uInitial.h"
#include "../h/VMsupport.h"


/* This function is used to pull the support struct and the check the exception cause
 * If the cause is a syscall exception, the function calls uSysHandler, otherwise the exception is treated as a
 * program trap (for our purposes, this case simply terminates the process)
 */
void SysSupport(){
	support_t *supportStruct;
	int cause;
	/*first things first, get the support struct */
	supportStruct = (support_t*) SYSCALL(GETSPTPTR,0,0,0);

	/* get exception cause */
	cause = (supportStruct->sup_exceptState[GENERALEXCEPT].s_cause & CAUSE) >> SHIFT;

	/* If Syscall */
	if(cause == SYSEXCEPTION) {
        uSysHandler(supportStruct);
    }
	/* Else, (for our purposes) Terminate the process */
	else{
		SYSCALL(TERMINATETHREAD,0,0,0);
	}
	
} /* End SysSupport */

/* If a syscall > 9 is thrown, this function handles the proper user syscall that needs to be performed.
 * SYS 9 - Terminate Process
 * SYS 10 - GET TOD
 * SYS 11 - Write the Printer
 * SYS 12 - Write to Terminal
 * SYS 13 - Read from Terminal
 * Otherwise, a SYS 2 is performed and the process is terminated
 */
void uSysHandler(support_t *supportStruct){
    supportStruct->sup_exceptState[GENERALEXCEPT].s_pc += 4;
	int sysReason = supportStruct->sup_exceptState[GENERALEXCEPT].s_a0;


    /* Begin Terminate Case */
	if(sysReason == TERMINATE){
		/*this is the case where we terminate process */
		SYSCALL(TERMINATETHREAD,0,0,0);
	} /* End Terminate Case */


	/* Begin Get TOD Case */
	else if(sysReason == GETTOD){
		cpu_t time;
		STCK(time);
		supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = time;
	} /* End Get TOD Case */


	/* Begin Write to Printer Case */
	/* Our Printer Write is broken, we are only able to print out one character from the beginning line in the test */
	else if(sysReason == PRINTERW){
		/*this is the case where we write to printer */
		int id; /*this is the asid of the process */
		int status = READY; /*used for writing to printer and terminal */
		int error;
		int devSemNum; /*used to determine the device semapore */
		int length; /*used to determine the length of output */
		char* charAddress;
		devregarea_t *devReg; /*device register type */

		id = (supportStruct->sup_asid) - 1;
		devReg = (devregarea_t*) RAMBASEADDR;
        devSemNum = ((PRINTER - DISK) * DEVPERINT) + (id);

		charAddress = (char*) supportStruct->sup_exceptState[GENERALEXCEPT].s_a1;
		length = supportStruct->sup_exceptState[GENERALEXCEPT].s_a2;

		if(((int)charAddress < KUSEG) || (length < 0) || (length > MAXSTRING)){
		    SYSCALL(TERMINATETHREAD, 0, 0, 0);
		}

		/* P the semaphore and get mutual exclusion */
		SYSCALL(PASSERN, (int) &devSem[devSemNum], 0, 0);
		int counter = 0; /*used for the while loop */
		error = FALSE;
		while((error == FALSE) && (counter < length)){
			/*need to work on the output for this function */
			devReg->devreg[devSemNum].d_data0 = *charAddress;
			devReg->devreg[devSemNum].d_command = 2;
			status = SYSCALL(WAITIO, PRINTER, id, 0);
			if(status == READY){
				counter++;
			}
			else{
				error = TRUE;
			}
            charAddress++;
		}
		/* V the semaphore and release mutual exclusion */
		SYSCALL(VERHOGEN, (int) &devSem[devSemNum], 0, 0);
		/* assign the number of characters to the process */
		supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = counter;
	} /* End Write to Printer Case */


	/* Begin Terminal Write Case */
	else if(sysReason == TERMINALW){
		/*this is the case where we write to terminal */
		int id; /*this is the asid of the process */
		int status; /*used for writing to printer and terminal */
		int devSemNum; /*used to determine the device semapore */
		int error;
		int length; /*used to determine the length of output */
		char* charAddress;
		devregarea_t* devReg; /*device register type */

		id = supportStruct->sup_asid-1;
		devReg = (devregarea_t*) RAMBASEADDR;
        devSemNum = ((TERMINAL - DISK) * DEVPERINT) + id;
		charAddress =(char*) supportStruct->sup_exceptState[GENERALEXCEPT].s_a1;
		length = supportStruct->sup_exceptState[GENERALEXCEPT].s_a2;

        if(((int)charAddress < KUSEG) || (length < 1) || length > MAXSTRING){
            SYSCALL(TERMINATETHREAD, 0, 0, 0);
        }
        /* P the semaphore and get mutual exclusion */
        SYSCALL(PASSERN, (int) &devSem[devSemNum], 0, 0);

        int counter = 0; /*used for the while loop */
        error = FALSE;

		while((error == FALSE) && (counter < length)){
		    devReg->devreg[devSemNum].t_transm_command = *charAddress << BYTE_LENGTH | C_TRANSMIT;

			/* Sys 8 the terminal write */
			status = SYSCALL(WAITIO, TERMINAL, id, 0);

			if((status & 0xFF) != C_TRANSOK){
			    error = TRUE;
			}else{
                counter++;
            }
            charAddress++;
		}
		/* V the semaphore to release mutual exclusion */
		SYSCALL(VERHOGEN, (int) &devSem[devSemNum], 0, 0);

		/* Terminal status or assign number of characters printed */
		if(error){
		    counter = 0 - (status & 0xFF);
		}
        supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = counter;
	} /* End Terminal Write Case */


	/* Begin Terminal Read Case */
    /*We never were able to get this working */
	else if(sysReason == TERMINALR){
        /*this is the case where we write to terminal */
        int id; /*this is the asid of the process */
        int status; /*used for writing to printer and terminal */
        int devSemNum; /*used to determine the device semapore */
        int error; /* status variables */
        int complete;
        char* charAddress;
        devregarea_t* devReg; /*device register type */

        id = supportStruct->sup_asid-1;
        devReg = (devregarea_t*) RAMBASEADDR;
        devSemNum = ((TERMINAL - DISK) * DEVPERINT) + id;
        charAddress =(char*) supportStruct->sup_exceptState[GENERALEXCEPT].s_a1;

        if((int)charAddress < KUSEG){
            SYSCALL(TERMINATETHREAD, 0, 0, 0);
        }
        /* P the semaphore and get mutual exclusion */
        SYSCALL(PASSERN, (int) &devSem[(devSemNum+DEVPERINT)], 0, 0);

        int counter = 0; /*used for the while loop */
        error = FALSE;
        complete = FALSE;

        while((error == FALSE) && (complete == FALSE)){
            devReg->devreg[devSemNum].t_recv_command = C_TRANSMIT;

            /* Sys 8 the terminal write */
            status = SYSCALL(WAITIO, TERMINAL, id, 1);

            if((status & 0xFF) != C_TRANSOK){
                error = TRUE;
            }else{
                counter++;
                *charAddress = status >> 8;
                charAddress++;
                if((status >> 8) == 0x0A){
                    complete = TRUE;
                }
            }
        }
        /* V the semaphore to release mutual exclusion */
        SYSCALL(VERHOGEN, (int) &devSem[(devSemNum+DEVPERINT)], 0, 0);

        /* Terminal status or assign number of characters printed */
        if(error){
            counter = 0 - (status & 0xFF);
        }
        supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = counter;

	} /* End Terminal Read Case */


	/* Else, Terminate the Process */
	else{
		SYSCALL(TERMINATETHREAD,0,0,0);
	}
	
    /* Perform a load state to return to the user process */
    LDST(&(supportStruct->sup_exceptState[GENERALEXCEPT]));
}