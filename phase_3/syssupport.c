/*SysSupport.c is the file that is referenced for SYS9 and above
Written by Kaleb Berry and Quinn Herlan */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/libumps.h"
#include "../h/uInitial.h"
#include "../h/VMsupport.h"
#include "../h/syssupport.h"

extern int control_sem;
void uSysHandler(support_t *supportStruct);


/*this function is used to pull the support struct, check the exception, then determine what syscall to perform */
void SysSupport(){
	support_t supportStruct;
	int cause;
	/*first things first, get the support struct */
	supportStruct = SYSCALL(GETSPTPTR,0,0,0);
	supportStruct->sup_exceptState[GENERALEXCEPT].s_pc += 4;
	cause = (supportStruct->sup_exceptState[GENERALEXCEPT].s_cause & CAUSE) >> SHIFT;

	if(cause == SYSEXCEPTION)
		uSysHandler(supportStruct);

	/*I think that we just terminate here? not sure what situation this puts us in */
	SYSCALL(TERMINATEPROCESS,0,0,0);
}

void uSysHandler(support_t *supportStruct){
    cpu_t time;
	int sysReason = supportStruct->sup_exceptState[GENERALEXCEPT].s_a0;

	if(sysReason == TERMINATE){
		/*this is the case where we terminate process */
		SYSCALL(TERMINATEPROCESS,0,0,0);
		break;
	}
	else if(sysReason == GETTOD){
		STCK(time);
		supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = time;
		break;
	}
	else if(sysReason == PRINTERW){
		/*this is the case where we write to printer */
		int id; /*this is the asid of the process */
		int status; /*used for writing to printer and terminal */
		int devSem; /*used to determine the device semapore */
		int length; /*used to determine the length of output */

		char charAddress;
		devregarea_t devReg; /*device register type */
		id = supportStruct->sup_asid;
		devReg = (devregarea_t *) RAMBASEADDR;
		devSem = ((PRINTERINT - DISKINT) * DEVPERINT) + (id -1);
		charAddress = supportStruct->sup_exceptState[GENERALEXCEPT].s_a1;
		length = supportStruct->sup_exceptState[GENERALEXCEPT].s_a2;

		int counter = 0; /*used for the while loop */
		while((status == READY) && (counter < length)){
			/*need to work on the output for this function */
			devReg->devreg[devSem].d_data0 = charAddress;
			devReg->devreg[devSem].d_command = 2;
			status = SYSCALL(WAITIO, PRINTER, (id-1), 0);
			if(status != READY){
				supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = (status *-1);
				status == status * -1;
			}
			charAddress++;
			counter++;

		}


		supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = counter;

	}
	else if(sysReason == TERMINALW){
		/*this is the case where we write to terminal */
		int id; /*this is the asid of the process */
		int status; /*used for writing to printer and terminal */
		int devSem; /*used to determine the device semapore */
		int length; /*used to determine the length of output */

		memaddr* charAddress;
		devregarea_t devReg; /*device register type */
		id = supportStruct->sup_asid;
		devReg = (devregarea_t *) RAMBASEADDR;
		devSem = ((PRINTERINT - DISKINT) * DEVPERINT) + (id -1);
		charAddress = supportStruct->sup_exceptState[GENERALEXCEPT].s_a1;
		length = supportStruct->sup_exceptState[GENERALEXCEPT].s_a2;

		int counter; /*used for the while loop */
		while((status == READY) && (counter < length)){
			/*need to work on the output for this function */
			devReg->devreg[devSem].d_data0 = charAddress;
			devReg->devreg[devSem].d_command = /*the transmat constant */ ;
			status = SYSCALL(WAITIO, PRINTER, (id-1), 0);
			if(status != READY){
				supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = (status *-1);
				status == status * -1;
			}
			charAddress++;
			counter++;
		}

		supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = counter;

	}
	else if(sysReason == TERMINALR){
		/*this is the case where we read from terminal, havnt even started this yet */
	}
	else{
		SYSCALL(TERMINATEPROCESS,0,0,0);
	}

}