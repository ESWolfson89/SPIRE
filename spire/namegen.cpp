#include "namegen.h"

STRING genName(NPC_t n, int num_compounds)
{
    STRING curr_name = "";
    for (int i = 0; i <= num_compounds; ++i)
    {
        if(ROLL(8) > 0)
             curr_name.append(common_consonant[ROLL(11)]);
        else
             curr_name.append(uncommon_consonant[ROLL(6)]);

        if (ROLL(7) == 0 && i < num_compounds)
            curr_name += (char)39;

        if (i < num_compounds)
            curr_name.append(vowel[ROLL(4)]);

        if (ROLL(6) == 0)
            curr_name.append("x");
    }

    return curr_name;
}
