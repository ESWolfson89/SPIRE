#include "door.h"

door::door()
{
    state = -1;
    loc = point(0,0);
}

door::door(point p, int stt, STRING sgn, door_type dt)
{
    loc = p;
    state = stt;
    door_sign = sgn;
    dtype = dt;
}

point door::at()
{
    return loc;
}

int door::getState()
{
    return state;
}

STRING door::getDoorSign()
{
    return door_sign;
}

void door::setState(int s)
{
    state = s;
}

void door::displayDoor()
{
    int current_ascii;

    color_pair cp = colorpair_white_black;

    STRING level_indicator = "";

    color_pair level_indicator_color;

    // print machine border one block at a time
    for (int x = 0; x < RIGHTSIDEWID; ++x)
        for (int y = 0; y < SHOWHGT; ++y)
        {
            if (state <= -1)
                current_ascii = openeddoor_backdrop_array[y][x];
            else
                current_ascii = closeddoor_backdrop_array[y][x];

            addGFXObject(colorpair_gray_black,current_ascii,x+SHOWWID,y);
        }


    if (state > -1)
    {
        if (state == 0)
        {
            level_indicator_color = colorpair_white_black;
            level_indicator = "U";
        }
        else if (state > 0)
        {
            level_indicator_color = colorpair_red_black;
            level_indicator = sint2String(state);
        }

        addGFXString(level_indicator_color,level_indicator, SHOWWID+18, 11);

        if (dtype == VAULT_DOOR_TYPE)
            cp.fg = color_orange;
        if (dtype == CASINO_DOOR_TYPE)
            cp.fg = getPresetColor(ROLL(11));

        addGFXString(cp,door_sign,(int)(SHOWWID+12 - (int)(door_sign.size()/2)), 3);
    }
}
