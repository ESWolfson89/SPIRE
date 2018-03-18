#include "game.h"

int main(int argc, char* argv[])
{
    if (initSDL()) {

        game mgame;

        mgame.run();

        freeSDL();
    }

    return 0;
} 
