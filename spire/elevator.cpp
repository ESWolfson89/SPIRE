#include "elevator.h"

elevator::elevator()
{

}

elevator::elevator(point p, int l)
{
    loc = p;
    lev = l;
}

point elevator::getLoc()
{
    return loc;
}

int elevator::getLevel()
{
    return lev;
}
