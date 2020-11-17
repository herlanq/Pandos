/* TLB Exception Handler for phase_3 */
/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 11/11
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/uInitial.h"
#include "../h/libumps.h"

HIDDEN swap_t swap_pool[POOLSIZE];
HIDDEN int swap_sem;

void InitTLB();

void InitTLB(){
    /*initializing the swap pool table and semaphore is initialized above as HIDDEN */
    /*should be putting swap pool at like 0x2000.0000 plus some ambiguous number we decide */
    swap_sem = 1;
    for(int i = 0; i < POOLSIZE; i++){
        swap_pool[i].sw_asid = -1;
    }

}
