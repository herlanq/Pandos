#include "../h/types.h"
#include "../h/const.h"
#ifndef INTERRUPTS
#define INTERRUPTS

extern void InterruptHandler();
extern void CopyState();
extern void CallScheduler();

#endif