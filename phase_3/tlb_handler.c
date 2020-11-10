/* TLB Exception Handler for phase_3 */
/* Written by Kaleb Berry and Quinn Herlan */

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


void initialStructs(){
	/*Planning on using this function to initialize all the structures needed for each process,
	possibly the swap pool and backing store as well */
}

void uTLB_exceptionHandler(){
	/*this function is used for TLB invalid and modification exceptions,
	it should check to make sure that the D-bit is on, and also check the valid bit. */
}

void uTLB_RefillHandler(/*I think the ASID is given to us */){
	/*This function is used for when there is no TLB entry fould,
	 this function goes and searches for it within the page table */
	for(int i = 0, i<= (unsigned int) RAMTOP, i++){ /*go through the page table to see where the TLB entry is */
		if(ASID == given_ASID){
			/*might have to check the valid bit as well but yeah */
			/*write it into the TLB and LDST on currentproc*/
		} 
	}
}