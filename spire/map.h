#ifndef MAP_H
#define MAP_H

#include "cell.h"
#include "point.h"

// a map is a two dimensional grid of cells as defined in cell.h

class map
{
    public:
        map();
        cell getCell(point);
        NPC_t getNPC(point);
        item_t getItem(point);
        terrain_t getTerrain(point);
        feature_t getFeature(point);
        fire_t getFire(point);
        bool getv(point);
        bool getm(point);
        chtype getu(point);
        void setNPC(point, NPC_t);
        void setItem(point, item_t);
        void setTerrain(point, terrain_t);
        void setFeature(point, feature_t);
        void setFire(point, fire_t);
        void setAllValuesToNIL(point);
        void setAllCellsUnvisible();
        void setv(point,bool);
        void setm(point,bool);
        void setu(point,chtype);
    private:
        // MAPHGT x MAPWID grid of cells (each cell has 5 enum values and 3 boolean fields)
        cell grid[MAPHGT][MAPWID];
        chtype utiles[MAPHGT][MAPWID];
};

bool isWall(terrain_t);

bool isCellLevelFeature(terrain_t);

bool isCellBlocking(terrain_t);

bool isCellBlocking(NPC_t);

bool isToggleableCell(cell *);

bool unWalkableCell(cell);

bool isPathBlocking(cell);

bool isCellinRange(map *, terrain_t, point, int);

bool isSpecialNonProcgen(NPC_t);

bool NPCsCannotFight(NPC_t, NPC_t);

int numTerrainTilesAdj(map *, point, terrain_t, int, bool);

bool canAddDoorHere(map *, point);

bool isDestructableTerrain(terrain_t);

bool isRoyalNPC(NPC_t);

fire_t selectFireCell(point, point, item_t);

#endif
