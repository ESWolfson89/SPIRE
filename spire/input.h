#ifndef INPUT_H_
#define INPUT_H_

#include "globals.h"
#include "point.h"

// input types
enum input_t
{
    INP_NONE,
    INP_EXITMENU,
    INP_SELECT,
    INP_DELTA,
    INP_TOGGLE,
    INP_PREVLEV,
    INP_NEXTLEV,
    INP_PICKUP,
    INP_QUICKPICKUP,
    INP_DROP,
    INP_DROPALL,
    INP_FULLSCREEN,
    INP_ALL,
    INP_INV,
    INP_FIRE,
    INP_RELOAD,
    INP_TARGET,
    INP_LOOK,
    INP_CHAT,
    INP_QUIT,
    INP_SAVE,
    INP_SHIFT,
    INP_REPEAT,
    INP_DEBUG,
    INP_FIREQUICK,
    INP_HELP
};

class input
{

public:
    input();

    void setAction();

    void setDelta(point);

    void waitForKey(char);

    void menuSelect(int &, int);

    bool checkForKey(char);

    void clearInputBuffer(SDL_Event *);

    void promptAction();

    point getDelta();

    input_t getAction();

private:
    SDL_Event evt;
    point delta;
    input_t action;

};

#endif
