#include "../h/types.h"
#include "../h/const.h"
#ifndef SYSSUPPORT
#define SYSSUPPORT




extern int control_sem;
extern int devSem[DEVICECNT+DEVPERINT]; /* device sema4's */

void SysSupport();
void uSysHandler(support_t *supportStruct);


#endif