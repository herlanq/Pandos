/* TLB Exception Handler for phase_3 */
/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 11/10
 */

/*REMINDERS:
One page table per user process
Pandos Page Table is an array of 32 Page Table entries. 
Each Page Table entry is a doubleword consisting of an EntryHi and an EntryLo portion
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

extern pcb_t *currentProc;
swap_t swap_pool[POOLSIZE];
int swap_sem = 1;
/*Planning on using this function to initialize all the structures needed for each process,
possibly the swap pool and backing store as well */
void initialStructs(){
    for(int i=0; i < POOLSIZE; i++){
        swap_pool[i].sw_asid = -1;
    }
}

/*this function is used for TLB invalid and modification exceptions,
it should check to make sure that the D-bit is on, and also check the valid bit. */
void uTLB_exceptionHandler(){

}

/*This function is used for when there is no TLB entry fould,
this function goes and searches for it within the page table */
void uTLB_RefillHandler(/*I think the ASID is given to us */){
    state_PTR oldstate;
    int missing_num;
    oldstate = (state_PTR) BIOSDATAPAGE;
    missing_num = (oldstate->s_entryHI & GETPAGENUM) >> VPNSHIFT;
    missing_num = missing_num % MAXPAGES; /* or % 31 ??? */
    setENTRYHI((currentProc->p_supportStruct->sup_PvtPgTable[missing_num]).entryHI);
    setENTRYLO((currentProc->p_supportStruct->sup_PvtPgTable[missing_num]).entryLO);
    TLBWR();
    LDST(oldstate);

    uVM_Hanlder(); /* page handler ? */

    SYSCALL(8,0,0,0);
    /* get cause */
    /* get asid */
    if (!pagefault){
        SYSCALL(2,0,0,0);
    }
    /* get missing page num */
    /* swap sema4 for mutex, sys 3 */

	for(int i = 0, i<= (unsigned int) RAMTOP, i++){ /*go through the page table to see where the TLB entry is */
		if(ASID == given_ASID){
			/*might have to check the valid bit as well but yeah */
			/*write it into the TLB and LDST on currentproc*/
		}
	}

}