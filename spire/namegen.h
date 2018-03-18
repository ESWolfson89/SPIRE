#ifndef NAMEGEN_H_
#define NAMEGEN_H_

#include "misc.h"

enum being_gender
{
    BEING_GENDER_NONE,
    BEING_GENDER_MALE,
    BEING_GENDER_FEMALE
};

STRING genName(NPC_t, int);

static STRING common_consonant[12] =
{
    "j","c","d","f","h","w","l","m","v","p","x","b"
};

static STRING uncommon_consonant[7] =
{
    "g","k","s","z","r","t","qu"
};

static STRING vowel[5] =
{
    "a","e","i","o","u"
};

#endif
