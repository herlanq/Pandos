#include "../h/types.h"
#include "../h/const.h"
#ifndef VMSUPPORT
#define VMSUPPORT

HIDDEN swap_t swap_pool[POOLSIZE];
HIDDEN int swap_sem;

extern void InitTLB();
extern void intsON(int on_off);
extern int flashOP(int flash, int sect, int buffer, int op)