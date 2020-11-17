/*SysSupport.c is the file that is referenced for SYS9 and above
Written by Kaleb Berry and Quinn Herlan */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/libumps.h"
#include "../h/support.h"

/*this function is used to pull the support struct, check the exception, then determine what syscall to perform */
void SysSupport(){
	support_t supportStruct;
	int cause;
	/*first things first, get the support struct */
	supportStruct = SYSCALL(8,0,0,0);
	cause = (supportStruct->sup_exceptState[GENERALEXCEPT].s_cause & CAUSE) >> SHIFT;

	if(cause == SYSEXCEPTION)
		uSysHandler();

	/*I think that we just terminate here? not sure what situation this puts us in */
	SYSCALL(TERMINATEPROCESS,0,0,0);
}

void uSysHandler(support_t supportStruct){
	
	
	int sysReason = supportStruct->sup_exceptState[GENERALEXCEPT].s_a0;
	if(sysReason == 9){
		/*this is the case where we terminate process */
		SYSCALL(TERMINATEPROCESS,0,0,0);
	}
	else if(sysReason == 10){
		cpu_t time;
		STCK(time);
		supportStruct->sup_exceptState[GENERALEXCEPT].s_v0 = time;
	}
	else if(sysReason == 11){
		/*this is the case where we write to printer */
		int id; /*this is the asid of the process */
		int status; /*used for writing to printer and terminal */
		int devSem; /*used to determine the device semapore */
		int length; /*used to determine the length of output */
		devregarea_t devReg; /*device register type */
		id = supportStruct->sup_asid;
		devReg = 
	}
	else if(sysReason == 12){
		/*this is the case where we write to terminal */
		int id; /*this is the asid of the process */
		int status; /*used for writing to printer and terminal */
		int devSem; /*used to determine the device semapore */
		int length; /*used to determine the length of output */
		devregarea_t devReg; /*device register type */
		id = supportStruct->sup_asid;
	}
	else if(sysReason == 13){
		/*this is the case where we read from terminal, havnt even started this yet */
	}

}