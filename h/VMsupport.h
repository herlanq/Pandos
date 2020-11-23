#include "../h/types.h"
#include "../h/const.h"
#ifndef VMSUPPORT
#define VMSUPPORT


extern int devSem[DEVICECNT+DEVPERINT]; /* device sema4's */
extern int control_sem;

extern void InitTLB();
extern void uTLB_Pager();
extern void uTLB_RefillHandler();

extern void intsON(int on_off);


#endif