/* TLB Exception Handler for phase_3 */
/* Written by: Quinn Herlan, Kaleb Berry
 * CSCI 320-01 Operating Systems
 * Last modified 11/11
 */

#include "../h/const.h"
#include "../h/types.h"
#include "../h/uInitial.h"
#include "../h/VMsupport.h"
#include "../h/libumps.h"

HIDDEN swap_t swap_pool[POOLSIZE];
HIDDEN int swap_sem;

int flashOP(int flash, int sect, int buffer, int op);
void intsON(int on_off);


void InitTLB(){
    swap_sem = 1;
    for(int i = 0; i < POOLSIZE; i++){
        swap_pool[i].sw_asid = -1;
    }
}

/*This function is used for when there is no TLB entry found,
this function goes and searches for it within the page table */
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

/*this function is used for TLB invalid and modification exceptions,
it should check to make sure that the D-bit is on, and also check the valid bit. */
void uTLB_ExceptionHandler(){
    int id, frame_num, pg_num, status;
    pteEntry_t *entry;
    unsigned int frame_addr;
    int block;
    support_t *supStruct;
    int cause;

    supStruct = (support_t *) SYSCALL(GETSPTPTR,0,0,0);
    cause = (supStruct->sup_exceptState[PGFAULTEXCEPT].s_cause & CAUSE);
    id = supStruct->sup_asid;

    if(cause == TLBINV || cause == TLBINVS){
        pg_num = ((supStruct->sup_exceptState[PGFAULTEXCEPT].s_entryHI) & GETPAGENUM);
        get_mutex();

        int swap;
        swap = 0;
        swap = (swap+1) % POOLSIZE;
        frame_num = swap;
        frame_addr = FRAMEPOOL + (frame_num * PAGESIZE);
        if(swap_pool[frame_num].sw_asid != -1){
            /* disable interrupts */
            intsON(OFF);
            swap_pool[frame_num].sw_pte->entryLO &= 0xFFFFFDFF;
            TLBCLR();

            block = swap_pool[frame_num].sw_pgNum;
            block = block % MAXPAGES;
            status = flashOP((swap_pool[frame_num].sw_asid)-1, block, frame_addr, FLASHR);

            if(status != READY){
                termUproc(&swap_sem);
            }
        }
    }else{
        termUproc();
    }

    block = pg_num;
    block = block % MAXPAGES;
    status = flashOP((id-1), block, frame_addr, FLASHR);
    if(status != READY){
        termUproc(&swap_sem);
    }
}

void intsON(int on_off){
    unsigned int status;
    status = getSTATUS();

    if(on_off == OFF){
        status = status & ALLOFF;
    }else{
        status = status | 0x1
    }
    setSTATUS(status);
}

int flashOP(int flash, int sect, int buffer, int op){
    int status;
    devregarea_t *devreg;
    devreg = (devregarea_t *) RAMBASEADDR;
    intsON(OFF);
    devreg->devreg[flash+DEVPERINT].d_data0 = buffer;
    devreg->devreg[flash+DEVPERINT].d_command = (sect << 8) | op;
    status = SYSCALL(WAITIO, FLASH,flash, 0);
    intsON(ON);

    if(status!=READY){
        status = 0;
    }
    return status;
}
