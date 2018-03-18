#ifndef ELEVATOR_H_
#define ELEVATOR_H_

#include "point.h"
#include "misc.h"

class elevator
{
public:
    elevator();
    elevator(point, int);
    point getLoc();
    int getLevel();
private:
    point loc;
    int lev;
};

#endif
