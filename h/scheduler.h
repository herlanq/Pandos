#include "../h/types.h"
#include "../h/const.h"
#ifndef SCHEDULER
#define SCHEDULER

extern cpu_t compuTime;
extern cpu_t QuantumStart;
extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;

extern void scheduler();

#endif