#include "../h/types.h"
#include "../h/const.h"
#ifndef SCHEDULER
#define SCHEDULER

extern int processCount;
extern int softBlockCount;
extern pcb_t *currentProc;
extern pcb_t *readyQue;

extern void scheduler();
extern void Ready_Timer(pcb_PTR current_process, cpu_t time);
extern void Context_Switch(pcb_PTR this_proc);


#endif