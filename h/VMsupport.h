#include "../h/types.h"
#include "../h/const.h"
#ifndef VMSUPPORT
#define VMSUPPORT

HIDDEN swap_t swap_pool[POOLSIZE];
HIDDEN int swap_sem;

extern void InitTLB();