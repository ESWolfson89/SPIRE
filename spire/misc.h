#ifndef MISC_H_
#define MISC_H_

#include "globals.h"
#include "rng.h"

STRING int2String(uint64);

STRING sint2String(int);

uint rollrange(int, int);

int rollMultipleDice(dice_roll);

STRING int2TimerString(int);

#endif

