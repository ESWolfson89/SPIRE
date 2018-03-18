#include "misc.h"

STRING int2String(uint64 i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

STRING sint2String(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}

uint rollrange(int s, int e)
{
    if (e <= s)
            return s;

    return ROLL(e-s) + s;
}

int rollMultipleDice(dice_roll d)
{
    int ret_val = 0;
    for (uint i = 0; i < d.num_dice; ++i)
    {
        ret_val += ROLL(d.num_sides - 1) + 1;
    }
    ret_val += d.num_bonus;
    return ret_val;
}

STRING int2TimerString(int t)
{
    uint ut = (uint)ABSOLUTE(t);

    STRING ret_val = "";

    uint64 days = (uint64)(ut/86400);

    uint64 hours = (ut % 86400) / 3600;

    uint64 minutes = (ut % 3600) / 60;

    uint64 seconds = (ut % 60);

    if (t < 0)
            ret_val += "-";

    if (ut >= 86400)
          ret_val += int2String(days) + ":";

    if (ut >= 3600)
    {
        if (hours <= 9)
                ret_val += "0";
        ret_val += int2String(hours) + ":";
    }

    if (minutes <= 9)
            ret_val += "0";

    ret_val += int2String(minutes) + ":";

    if (seconds <= 9)
            ret_val += "0";

    ret_val += int2String(seconds);

    return ret_val;
}
