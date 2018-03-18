// input

#include "input.h"

input::input()
{
    delta.set(0,0);
    action = INP_NONE;
}

void input::promptAction()
{
    action = INP_NONE;

    do
    {
        SDL_WaitEvent(&evt);
    }
    while (evt.type != SDL_KEYDOWN);
}

void input::menuSelect(int &selection, int num_choices)
{
    promptAction();

    switch(evt.key.keysym.sym)
    {
    case(SDLK_k):
    case(SDLK_KP_8):
    case(SDLK_UP):
        selection--;
        break;
    case(SDLK_j):
    case(SDLK_KP_2):
    case(SDLK_DOWN):
        selection++;
        break;
    case(SDLK_d):
        action = INP_DROP;
        return;
    case(SDLK_a):
        action = INP_ALL;
        return;
    case(SDLK_RETURN):
        action = INP_TOGGLE;
        break;
    case(SDLK_SPACE):
        action = INP_EXITMENU;
        break;
    }

    if (selection < 0)
        selection = num_choices - 1;
    if (selection > num_choices - 1)
        selection = 0;
}

void input::setAction()
{
    promptAction();

    switch(evt.key.keysym.sym)
    {
    case(SDLK_y):
    case(SDLK_KP_7):
        setDelta(point(-1,-1));
        break;
    case(SDLK_k):
    case(SDLK_KP_8):
    case(SDLK_UP):
        setDelta(point(0,-1));
        break;
    case(SDLK_u):
    case(SDLK_KP_9):
        setDelta(point(1,-1));
        break;
    case(SDLK_h):
    case(SDLK_KP_4):
    case(SDLK_LEFT):
        setDelta(point(-1,0));
        break;
    case(SDLK_l):
    case(SDLK_KP_6):
    case(SDLK_RIGHT):
        setDelta(point(1,0));
        break;
    case(SDLK_b):
    case(SDLK_KP_1):
        setDelta(point(-1,1));
        break;
    case(SDLK_j):
    case(SDLK_KP_2):
    case(SDLK_DOWN):
        setDelta(point(0,1));
        break;
    case(SDLK_n):
    case(SDLK_KP_3):
        setDelta(point(1,1));
        break;
    case(SDLK_q):
        action = INP_QUIT;
        break;
    case(SDLK_RETURN):
        action = INP_TOGGLE;
        break;
    case(SDLK_SPACE):
        action = INP_SELECT;
        break;
    case(SDLK_SEMICOLON):
        action = INP_QUICKPICKUP;
        break;
    case(SDLK_p):
    case(SDLK_KP_5):
        action = INP_PICKUP;
        break;
    case(SDLK_i):
        action = INP_INV;
        break;
    case(SDLK_c):
        action = INP_CHAT;
        break;
    case(SDLK_r):
        action = INP_RELOAD;
        break;
    case(SDLK_t):
        action = INP_FIRE;
        break;
    case(SDLK_z):
        action = INP_DEBUG;
        break;
    case(SDLK_COMMA):
        action = INP_PREVLEV;
        break;
    case(SDLK_PERIOD):
        action = INP_NEXTLEV;
        break;
    case(SDLK_f):
        action = INP_FIREQUICK;
        break;
    case(SDLK_SLASH):
        action = INP_HELP;
        break;
    default:
        break;
    }
}

void input::setDelta(point p)
{
    action = INP_DELTA;
    delta.set(p.x(),p.y());
}

void input::waitForKey(char c)
{
    bool hit_space = false;

    while( hit_space == false )
    {
        if( SDL_WaitEvent( &evt) )
            if( evt.type == SDL_KEYDOWN)
                if( (int)evt.key.keysym.sym == (int)c)
                {
                    hit_space = true;
                }
    }
}

bool input::checkForKey(char c)
{
    bool hit_key = false;

    while( hit_key == false )
    {
        if( SDL_WaitEvent( &evt) )
            if( evt.type == SDL_KEYDOWN)
                hit_key = true;
    }

    return evt.key.keysym.sym == (int)c;
}

point input::getDelta()
{
    return delta;
}

input_t input::getAction()
{
    return action;
}


