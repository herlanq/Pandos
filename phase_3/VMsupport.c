/* TLB Exception Handler for phase_3 */
/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 11/11
 */

#include "../h/const.h"
#include "../h/types.h"

#include "../h/initial.h"
#include "../h/uInitial.h"
#include "../h/VMsupport.h"
#include "../h/syssupport.h"
#include "../h/libumps.h"

HIDDEN swap_t swap_pool[POOLSIZE];
HIDDEN int swap_sem;
HIDDEN int get_frame();

/* initializes the TLB data structure for support paging.
 * Inits the global shared Page Table */
void InitTLB(){
    int i;
    swap_sem = 1;
    for(i=0; i < POOLSIZE; i++){
        swap_pool[i].sw_asid = -1;
    }
} /* end InitTLB */

/*This function is used for when there is no TLB entry found, thus calling a TLB exception.
The contents of the PTE are then written into the TLB */
void uTLB_RefillHandler(){
    state_PTR oldstate;
    int pg_num;

    oldstate = (state_PTR) BIOSDATAPAGE;

    pg_num = (oldstate->s_entryHI & GETPAGENUM) >> VPNSHIFT;
    pg_num = pg_num % MAXPAGES;

    setENTRYHI((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryHI);
    setENTRYLO((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryLO);
    TLBWR();
    LDST(oldstate);
}

/* This function handles only page fault TLB Management Exceptions. All other exceptions
 * will result in a terminating process. */
void uTLB_Pager(){
    int id, frame_num, pg_num, status;
    unsigned int frame_addr;
    int block;
    support_t *supStruct;
    int cause;

    supStruct = (support_t *) SYSCALL(GETSPTPTR,0,0,0);
    cause = (supStruct->sup_exceptState[PGFAULTEXCEPT].s_cause & CAUSE);
    id = supStruct->sup_asid;

    if(cause != TLBINV && cause != TLBINVS) {
        SYSCALL(TERMINATETHREAD, 0, 0, 0);
    }

    pg_num = ((supStruct->sup_exceptState[PGFAULTEXCEPT].s_entryHI) & GETPAGENUM);
    SYSCALL(PASSERN, swap_sem, 0, 0 );
    frame_num = get_frame();
    frame_addr = FRAMEPOOL + (frame_num * PAGESIZE);

    if(swap_pool[frame_num].sw_asid != -1){
        /* disable interrupts */
        intsON(OFF);
        swap_pool[frame_num].sw_pte->entryLO = swap_pool[frame_num].sw_pte->entryLO & 0xFFFFFDFF;
        TLBCLR();
        intsON(ON);

        block = swap_pool[frame_num].sw_pgNum;
        block = block % MAXPAGES;

        status = flashOP(((swap_pool[frame_num].sw_asid)-1), block, frame_addr, FLASHW);

        if(status != READY){
                SYSCALL(TERMINATETHREAD, swap_sem, 0, 0);
            }
        }

    block = pg_num;
    block = block % MAXPAGES;
    status = flashOP((id-1), block, frame_addr, FLASHR);
    if(status != READY){
        SYSCALL(TERMINATETHREAD, swap_sem, 0, 0);
    }

    intsON(OFF);
    swap_pool[frame_num].sw_pte->entryLO = frame_addr | VALIDON | DIRTYON;
    TLBCLR();
    intsON(ON);
    SYSCALL(VERHOGEN, swap_sem, 0, 0);
    LDST(&supStruct->sup_exceptState[PGFAULTEXCEPT]);
}

/* Helper function to toggle interrupts on and off */
void intsON(int on_off){
    unsigned int status;
    status = getSTATUS();

    if(on_off == OFF){
        status = status & ALLOFF;
    }else{
        status = status | 0x1;
    }
    setSTATUS(status);
}

/* This function performs a Flash I/O operation within the page fault handler.
 * This functions takes in 'flash' as the specific flash device number.
 * sect is the I/O sector number
 * buffer is the buffer address
 * 'op' specifies the types of flash operation, read or write
 * */
int flashOP(int flash, int sect, int buffer, int op){
    int status;
    devregarea_t *devreg;
    devreg = (devregarea_t *) RAMBASEADDR;
    intsON(OFF);
    devreg->devreg[flash+DEVPERINT].d_data0 = buffer;
    devreg->devreg[flash+DEVPERINT].d_command = (sect << 8) | op;
    status = SYSCALL(WAITIO, FLASH, flash, 0);
    intsON(ON);

    if(status!=READY){
        status = OFF;
    }
    return status;
}

/* this function is used to satisfy a page fault exception by finding which frame to use
 * using a round robin algorithm */
HIDDEN int get_frame(){
    int swap;
    swap = 0;
    swap = (swap+1) % POOLSIZE;
    return swap;
}
