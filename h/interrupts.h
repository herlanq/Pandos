#include "../h/types.h"
#include "../h/const.h"
#ifndef INTERRUPTS
#define INTERRUPTS

extern void InterruptHandler();
extern int Device_InterruptH(int line);
extern void Copy_Paste(state_t *copied_state, state_t *pasted_state);

#endif