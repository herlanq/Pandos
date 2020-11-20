/*SysSupport.c is the file that is referenced for SYS9 and above
Written by Kaleb Berry and Quinn Herlan */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/libumps.h"
#include "../h/syssupport.h"

#include "../h/uInitial.h"
#include "../h/VMsupport.h"

int zflag = 0;

/*this function is used to pull the support struct, check the exception, then determine what syscall to perform */
void SysSupport(){
	support_t *supportStruct;
	int cause;
	/*first things first, get the support struct */
	supportStruct = (support_t*) SYSCALL(GETSPTPTR,0,0,0);
	/*supportStruct->sup_exceptState[GENERALEXCEPT].s_pc += 4; */
	/* get exception cause */
	cause = (supportStruct->sup_exceptState[GENERALEXCEPT].s_cause & CAUSE) >> SHIFT;

	/* If Syscall */
	if(cause == SYSEXCEPTION) {
        uSysHandler(supportStruct);
    }

	/* Else, (for our purposes) Terminate the process */
	SYSCALL(TERMINATETHREAD,0,0,0);
}

void uSysHandler(support_t *supportStruct){
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
	else if(sysReason == PRINTERW){
		/*this is the case where we write to printer */
		int id; /*this is the asid of the process */
		int status; /*used for writing to printer and terminal */
		int error;
		int devSemNum; /*used to determine the device semapore */
		int length; /*used to determine the length of output */
		char *charAddress;
		devregarea_t *devReg; /*device register type */

		id = supportStruct->sup_asid;
		devReg = (devregarea_t*) RAMBASEADDR;
        devSemNum = ((PRINTER - DISK) * DEVPERINT) + (id-1);

		charAddress = (char*) supportStruct->sup_exceptState[GENERALEXCEPT].s_a1;
		length = supportStruct->sup_exceptState[GENERALEXCEPT].s_a2;

		if(((int)charAddress < KUSEG) || (length < 1) || (length > MAXSTRING)){
		    SYSCALL(TERMINATETHREAD, 0, 0, 0);
		}

		/* P the semaphore and get mutual exclusion */
		SYSCALL(PASSERN, devSem[devSemNum], 0, 0);

		int counter = 0; /*used for the while loop */
		error = FALSE;
		while((!error) && (counter < length)){
			/*need to work on the output for this function */
			devReg->devreg[devSemNum].d_data0 = (*charAddress);
			devReg->devreg[devSemNum].d_command = 2;
			status = SYSCALL(WAITIO, PRINTER, (id-1), 0);
			if(status != READY){
			    error = TRUE;
				/*supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = (status *-1);*/
			}else{
                counter++;
			}
            charAddress++;
		}
		/* V the semaphore and release mutual exclusion */
		SYSCALL(VERHOGEN, devSem[devSemNum], 0, 0);

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

		id = supportStruct->sup_asid;
		devReg = (devregarea_t*) RAMBASEADDR;
        devSemNum = ((PRINTER - DISK) * DEVPERINT) + (id-1);
		charAddress =(char*) supportStruct->sup_exceptState[GENERALEXCEPT].s_a1;
		length = supportStruct->sup_exceptState[GENERALEXCEPT].s_a2;

        if(((int)charAddress < KUSEG) || (length < 1)){
            SYSCALL(TERMINATETHREAD, 0, 0, 0);
        }
        /* P the semaphore and get mutual exclusion */
        SYSCALL(PASSERN, devSem[devSemNum], 0, 0);

        int counter = 0; /*used for the while loop */
        error = FALSE;
		while((!error) && (counter < length)){
		    devReg->devreg[devSemNum].t_transm_command = *charAddress;

			/* Sys 8 the terminal write */
			status = SYSCALL(WAITIO, PRINTER, (id-1), 0);

			if((status & 0xFF) != READY){
			    error = TRUE;
			}
			charAddress++;
			counter++;
		}
		/* V the semaphore to release mutual exclusion */
		SYSCALL(VERHOGEN, devSem[devSemNum], 0, 0);
		if(error){
		    counter = 0 - (status&0xFF);
		}
		supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = counter;

	} /* End Terminal Write Case */

	else if(sysReason == TERMINALR){
		/*this is the case where we read from terminal, havnt even started this yet */
	}

	else{
		zflag = 4;
		SYSCALL(TERMINATETHREAD,0,0,0);
	}
    /*supportStruct->sup_exceptState[GENERALEXCEPT].s_pc = supportStruct->sup_exceptState[]*/
    LDST(&(supportStruct->sup_exceptState[GENERALEXCEPT]));
}