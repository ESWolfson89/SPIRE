#include "cell.h"

// see .h file for more info
// NIL values are enum 0 for NPC_t, item_t,
// terrain_t and fire_t enums
cell::cell()
{
    npc = NIL_n;
    item = NIL_i;
    terrain = NIL_t;
    feature = NIL_r;
    fire = NIL_f;
    in_mem = false;
    is_vis = false;
}

NPC_t cell::getNPC()
{
    return npc;
}

item_t cell::getItem()
{
    return item;
}

terrain_t cell::getTerrain()
{
    return terrain;
}

feature_t cell::getFeature()
{
    return feature;
}

fire_t cell::getFire()
{
    return fire;
}

bool cell::getv()
{
    return is_vis;
}

bool cell::getm()
{
    return in_mem;
}

void cell::setNPC(NPC_t n)
{
    npc = n;
}

void cell::setItem(item_t i)
{
    item = i;
}

void cell::setTerrain(terrain_t t)
{
    terrain = t;
}

void cell::setFeature(feature_t f)
{
    feature = f;
}

void cell::setFire(fire_t f)
{
    fire = f;
}

void cell::setv(bool v)
{
    is_vis = v;
}

void cell::setm(bool m)
{
    in_mem = m;
}
