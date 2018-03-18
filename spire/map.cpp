#include "map.h"

// should add a copy constructor
map::map()
{
}

cell map::getCell(point p)
{
    return grid[p.y()][p.x()];
}

NPC_t map::getNPC(point p)
{
    return grid[p.y()][p.x()].getNPC();
}

item_t map::getItem(point p)
{
    return grid[p.y()][p.x()].getItem();
}

terrain_t map::getTerrain(point p)
{
    return grid[p.y()][p.x()].getTerrain();
}

feature_t map::getFeature(point p)
{
    return grid[p.y()][p.x()].getFeature();
}

fire_t map::getFire(point p)
{
    return grid[p.y()][p.x()].getFire();
}

bool map::getv(point p)
{
    return grid[p.y()][p.x()].getv();
}

bool map::getm(point p)
{
    return grid[p.y()][p.x()].getm();
}

chtype map::getu(point p)
{
    return utiles[p.y()][p.x()];
}

void map::setNPC(point p, NPC_t n)
{
    grid[p.y()][p.x()].setNPC(n);
}

void map::setItem(point p, item_t i)
{
    grid[p.y()][p.x()].setItem(i);
}

void map::setTerrain(point p, terrain_t t)
{
    grid[p.y()][p.x()].setTerrain(t);
}

void map::setFeature(point p, feature_t r)
{
    grid[p.y()][p.x()].setFeature(r);
}

void map::setFire(point p, fire_t f)
{
    grid[p.y()][p.x()].setFire(f);
}

void map::setv(point p, bool b)
{
    grid[p.y()][p.x()].setv(b);
}

void map::setm(point p, bool b)
{
    grid[p.y()][p.x()].setm(b);
}

void map::setu(point p, chtype ct)
{
    utiles[p.y()][p.x()] = ct;
}

// terrain can never be NIL
void map::setAllValuesToNIL(point p)
{
    setv(p,false);
    setm(p,false);
    setu(p,blank_ch);
    setFire(p,NIL_f);
    setNPC(p,NIL_n);
    setFeature(p,NIL_r);
    setItem(p,NIL_i);
    setTerrain(p,WALL_INDESTRUCT);
}

void map::setAllCellsUnvisible()
{
    for (int x = 0; x < MAPWID - 1; ++x)
    for (int y = 0; y < MAPHGT - 1; ++y)
           grid[y][x].setv(false);
}

bool isCellinRange(map * m, terrain_t t, point p, int rad)
{
    point dp;

    for (int x = -rad; x <= rad; ++x)
    for (int y = -rad; y <= rad; ++y)
    if (x != 0 || y != 0)
    {
        dp.set(x + p.x(),y + p.y());
        if (m->getTerrain(dp) == t)
              return true;
    }

    return false;
}

bool NPCsCannotFight(NPC_t a, NPC_t b)
{
    return a == b && !isSpecialNonProcgen(a) && a != PROCGEN_NPC && !isRoyalNPC(a);
}

bool isRoyalNPC(NPC_t r)
{
    return (r == PRINCE_NPC || r == KING_NPC || r == EMPEROR_NPC);
}

bool isSpecialNonProcgen(NPC_t n)
{
    return n == AGENT_NPC || n == ARMSDEALER_NPC;
}

bool canAddDoorHere(map *m, point p)
{
    if (m->getTerrain(p) == FLR_GEN)
    if (numTerrainTilesAdj(m,p,WALL_INDESTRUCT,1,false) == 6)
    {
        if (m->getTerrain(point(p.x()+1,p.y())) == FLR_GEN &&
            m->getTerrain(point(p.x()-1,p.y())) == FLR_GEN)
                return true;
        if (m->getTerrain(point(p.x(),p.y()+1)) == FLR_GEN &&
            m->getTerrain(point(p.x(),p.y()-1)) == FLR_GEN)
                return true;
    }
    return false;
}

bool isWall(terrain_t t)
{
    return t == WALL_INDESTRUCT ||
           t == WALL_DESTRUCT;
}

bool isCellLevelFeature(terrain_t t)
{
    return t == CLOSED_UNLOCKED_DOOR || t == OPEN_UNLOCKED_DOOR;
}

// blocking terrain
bool isCellBlocking(terrain_t t)
{
    return isWall(t) || t == CLOSED_UNLOCKED_DOOR;
}

bool isCellBlocking(NPC_t b)
{
    return b != NIL_n;
}

bool isToggleableCell(cell *c)
{
    return c->getItem() == NIL_i && c->getNPC() == NIL_n;
}

bool unWalkableCell(cell c)
{
    return isPathBlocking(c) || c.getTerrain() == CLOSED_UNLOCKED_DOOR;
}

bool isPathBlocking(cell c)
{
    return isWall(c.getTerrain()) ||
                 isCellBlocking(c.getNPC());
}

int numTerrainTilesAdj(map *m, point p, terrain_t t, int rad, bool vn)
{
    int adj = 0;

    point ap;

    for (int x = -rad + p.x(); x <= rad + p.x(); ++x)
    for (int y = -rad + p.y(); y <= rad + p.y(); ++y)
    {
        ap.set(x,y);
        if (!isAt(ap,p))
        if (inRange(ap,0,0,MAPWID-1,MAPHGT-1))
        if (m->getTerrain(ap) == t)
        if (!vn || (p.x() == x) || (p.y() == y))
            adj++;
    }

    return adj;
}

fire_t selectFireCell(point lof, point source, item_t weap)
{
    fire_t ret_val = NIL_f;

    if (weap == COMBAT_SHOTGUN)
        return SHOTGUNPELLET_SPRITE1;
    if (weap == DOUBLEBARRELLED_SHOTGUN)
        return SHOTGUNPELLET_SPRITE2;
    if (weap == PLASMA_CANNON)
        return BLUEPLASMA_SPRITE;
    if (weap == PULSE_CANNON)
        return PURPLEPLASMA_SPRITE;
    if (weap == FUSION_CANNON)
        return REDFIRE_SPRITE;

    point dl = point(lof.x() - source.x(), lof.y() - source.y());

    if (std::abs(dl.x()) == 0)
        ret_val = BULLET_SPRITEV;
    else if (std::abs(dl.y()) == 0)
        ret_val = BULLET_SPRITEH;
    else if ((dl.x() < 0 && dl.y() < 0) || (dl.x() > 0 && dl.y() > 0))
        ret_val = BULLET_SPRITEDIAG2;
    else
        ret_val = BULLET_SPRITEDIAG1;

    switch (weap)
    {
    case(PISTOL_STEALTH):
            ret_val = (fire_t)((int)ret_val + 4);
            break;
    case(REVOLVER):
            ret_val = (fire_t)((int)ret_val + 8);
            break;
    case(ROCKET_LAUNCHER):
            ret_val = (fire_t)((int)ret_val + 12);
            break;
    case(CHAINGUN):
            ret_val = (fire_t)((int)ret_val + 16);
            break;
    case(LASER_GUN):
            ret_val = (fire_t)((int)ret_val + 20);
            break;
    case(DEATHRAY_GUN):
            ret_val = (fire_t)((int)ret_val + 24);
            break;
    default:
            break;
    }
    return ret_val;
}

bool isDestructableTerrain(terrain_t t)
{
    return t == WALL_INDESTRUCT || t == CLOSED_UNLOCKED_DOOR;
}
