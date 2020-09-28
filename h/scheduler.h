#include "../h/types.h"
#include "../h/const.h"
#ifndef SCHEDULER
#define SCHEDULER

extern cpu_t compuTime;
extern cpu_t QuantumStart;

extern void scheduler();

#endif