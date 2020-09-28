#include "../h/types.h"
#include "../h/const.h"
#ifndef INITIAL
#define INITIAL

extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;
extern int semD[SEMNUM];

#endif