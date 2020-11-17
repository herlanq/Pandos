#include "../h/types.h"
#include "../h/const.h"
#ifndef UINITIAL
#define UINITIAL


extern int devSem[DEVICECNT+DEVPERINT]; /* device sema4's */
extern int control_sem;

extern void test();
HIDDEN void InitUserProc();

#endif