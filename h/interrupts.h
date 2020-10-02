#include "../h/types.h"
#include "../h/const.h"
#ifndef INTERRUPTS
#define INTERRUPTS

extern void CtrlPlusC(state_t oldState, state_t newState);
extern void IOTrapHandler();

#endif