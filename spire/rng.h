#ifndef RNG_H_
#define RNG_H_

#include <random>
#include <time.h>

static std::mt19937 random_number_generator(time(0));

int ROLL(int);
unsigned int PERCCHANCE();
unsigned int TENTHPERCCHANCE();
int randInt(int,int);

#endif
