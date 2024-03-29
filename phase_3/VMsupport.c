/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 12/05
 *
 * The VMsupport module contains a number of functions that provide support for managing the virtual memory of
 * the Pandos operating system. The module contains functions to initialize the TLB, handle TLB refill events,
 * and handle page fault errors.
 *
 */

#include "../h/const.h"
#include "../h/types.h"

#include "../h/initial.h"
#include "../h/uInitial.h"
#include "../h/VMsupport.h"
#include "../h/syssupport.h"
#include "../h/libumps.h"

/* Modular Variable Declaration */
HIDDEN swap_t swap_pool[POOLSIZE];
HIDDEN int swap_sem;
HIDDEN int swapper = 0;

/* Modular Function Declaration */
HIDDEN int get_frame();
HIDDEN int flashOP(int flash, int blockID, int buffer, int op);

/* Initializes the TLB data structure for support paging of size UPROCMAX * 2 (= 16) */
void InitTLB(){
    int i;
    /* Init the swap pool control sema4 */
    swap_sem = 1;
    /* Init the swap pool data structure */
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

    pg_num = (((oldstate->s_entryHI) & GETPAGENUM) >> VPNSHIFT);
    pg_num = (pg_num % MAXPAGES);

    setENTRYHI((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryHI);
    setENTRYLO((currentProc->p_supportStruct->sup_PvtPgTable[pg_num]).entryLO);
    TLBWR();
    LDST(oldstate);
}

/* This function handles only page fault TLB Management Exceptions. All other exceptions
 * will result in a process termination. */
void uTLB_Pager(){
    int id, frame_num, pg_num, status; /* process id, victim frame number, requested pg number, return status */
    unsigned int frame_addr;
    int block;
    support_t *supStruct;
    int cause;

    supStruct = (support_t *) SYSCALL(GETSPTPTR,0,0,0);
    /* get exception cause and process id causing the exception */
    cause = (supStruct->sup_exceptState[PGFAULTEXCEPT].s_cause & CAUSE) >> SHIFT;
    id = supStruct->sup_asid;

    if(cause != TLBINV && cause != TLBINVS) {
        SYSCALL(TERMINATETHREAD, 0, 0, 0);
    }
    
    /* get page number of the request */
    pg_num = ((supStruct->sup_exceptState[PGFAULTEXCEPT].s_entryHI) & GETPAGENUM) >> VPNSHIFT;

    /* P the sema4 to gain mutual exclusion */
    SYSCALL(PASSERN, (int) &swap_sem, 0, 0 );

    /* call helper function to assign frame number and address */
    frame_num = get_frame();
    frame_addr = FRAMEPOOL + (frame_num * PAGESIZE);

    /* if frame is being used */
    if(swap_pool[frame_num].sw_asid != -1){
        /* disable interrupts */
        intsON(OFF);
        /* Turn off V bit */
        swap_pool[frame_num].sw_pte->entryLO &= 0xFFFFFDFF;
        TLBCLR();
        intsON(ON);

        /* update the backing store */
        block = ((swap_pool[frame_num].sw_pgNum) % MAXPAGES);
        /* write into the backing store */

        status = flashOP(((swap_pool[frame_num].sw_asid)-1), block, frame_addr, FLASHW);

        if(status != READY){
            SYSCALL(TERMINATETHREAD, (int) &swap_sem, 0, 0);
        }
    }

    /* read from backing store */
    block = (pg_num % MAXPAGES);
    status = flashOP((id-1), block, frame_addr, FLASHR);

    if(status != READY){
        SYSCALL(TERMINATETHREAD, (int) &swap_sem, 0, 0);
    }
    
    pteEntry_t* pEntry = &(supStruct->sup_PvtPgTable[block]);
    swap_pool[frame_num].sw_pte = pEntry;
    swap_pool[frame_num].sw_asid = id;
    swap_pool[frame_num].sw_pgNum = pg_num;
    /* Turn off interrupts */
    intsON(OFF);
    
    swap_pool[frame_num].sw_pte->entryLO = frame_addr | VALIDON | DIRTYON;
    
    /* Clear the TLB*/
    TLBCLR();
    /* Turn on interrupts */
    intsON(ON);
    /* V the semaphore */
    SYSCALL(VERHOGEN,(int) &swap_sem, 0, 0);
    /* Load State */
    LDST(&(supStruct->sup_exceptState[PGFAULTEXCEPT]));

} /* End uTLB_Pager */

/* Helper function to toggle interrupts on and off */
void intsON(int on_off){
    unsigned int status;
    status = getSTATUS();

    if(on_off == OFF){
        status = (status & IECON);
    }else{
        status = (status | 0x1);
    }
    setSTATUS(status);
}

/* This function performs a Flash I/O operation within the page fault handler.
 * This functions takes in 'flash' as the specific flash device number.
 * sect is the I/O sector number
 * buffer is the buffer address
 * 'op' specifies the types of flash operation, read or write
 * */
HIDDEN int flashOP(int flash, int blockID, int buffer, int op){
    int status;
    devregarea_t *devreg;
    devreg = (devregarea_t *) RAMBASEADDR;
    /* Load I/O and perform sys 8 with interrupts disabled */
    intsON(OFF); /* toggle interrupts off */
    devreg->devreg[flash+DEVPERINT].d_data0 = buffer;
    devreg->devreg[flash+DEVPERINT].d_command = (blockID << 8) | op;
    status = SYSCALL(WAITIO, FLASH, flash, 0);
    intsON(ON); /* toggle interrupts back on */

    if(status!=READY){
        status = OFF - status;
    }
    return status;
}

/* this function is used to satisfy a page fault exception by finding which frame to use
 * using a round robin algorithm */
HIDDEN int get_frame(){
    swapper = (swapper+1) % POOLSIZE;
    return swapper;
}
