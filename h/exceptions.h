#include "../h/types.h"
#include "../h/const.h"
#ifndef EXCEPTIONS
#define EXCEPTIONS

extern void CtrlPlusC(state_t *oldState, state_t *newState);
extern void PrgTrapHandler();
extern void TLBTrapHandler();
extern void SYSCALLHandler();

#endif