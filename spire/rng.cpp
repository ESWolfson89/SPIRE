#include "rng.h"

int ROLL(int high)
{
    return randInt(0,high);
}

int randInt(int low, int high)
{
    if (high <= low)
        return high;

    std::uniform_int_distribution<int> dist(low,high);
    return dist(random_number_generator);
}

unsigned int PERCCHANCE()
{
    return (unsigned int)randInt(1,100);
}

unsigned int TENTHPERCCHANCE()
{
    return (unsigned int)randInt(1,1000);
}
