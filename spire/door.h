#ifndef DOOR_H_
#define DOOR_H_

#include "point.h"
#include "graphics.h"

enum door_type
{
    GENERIC_DOOR_TYPE,
    VAULT_DOOR_TYPE,
    CASINO_DOOR_TYPE
};

static const int closeddoor_backdrop_array[SHOWHGT][RIGHTSIDEWID] =
{
 { 0 ,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219},
 { 0 ,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,218,196,196,196,196,196,191,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 , 0 , 0 , 0 , 0 ,179,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,192,196,196,196,196,196,217,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219,219},
 { 0 ,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219},
 { 0 ,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219}
};

static const int openeddoor_backdrop_array[SHOWHGT][RIGHTSIDEWID] =
{
 { 0 ,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219},
 { 0 ,219, 0 , 0 ,219,219,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 ,218,196, 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 ,179, 0 ,196,196,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 ,192,196, 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 , 0 , 0 , 0 , 0 ,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219, 0 , 0 ,219,219,219,219, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,219},
 { 0 ,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219,219}
};

class door
{
public:
    door();
    door(point, int, STRING, door_type);
    point at();
    int getState();
    void setState(int);
    void displayDoor();
    STRING getDoorSign();
private:
    int state;
    point loc;
    door_type dtype;
    STRING door_sign;
};

#endif
