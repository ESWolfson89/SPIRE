#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include "misc.h"

bool initSDL();

bool isColor(color_type, color_type);

color_type getRandomVisibleColor();

color_type getPresetColor(int);

color_type getDimmedPresetColor(color_type, Uint8, Uint8);

color_pair getInvertedColorpair(color_pair);

void setUpdateFlag(bool);

void freeSDL();

void addGFXString(color_pair, STRING, int, int);

void addGFXAltString(color_pair, STRING, int, int, int);

void addGFXObject(color_pair, int, int, int);

void clearScreen();

void updateScreen();

#endif
