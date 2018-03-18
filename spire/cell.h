#ifndef CELL_H_
#define CELL_H_

#include "globals.h"

// A cell has five characteristics
// displayed in the following priority:
// foreground:
// 1) fire_t
// 2) NPC_t
// 3) feature_t
// 4) item_t
// 5) terrain_t

class cell
{
    public:
        // init NIL values to all cell components
        cell();

        // return npc_t value of the cell
        NPC_t getNPC();

        // return item_t value of the cell
        item_t getItem();

        // return terrain_t value of the cell
        terrain_t getTerrain();

        // return feature_t value of the cell
        feature_t getFeature();

        // return fire_t value of cell
        fire_t getFire();

        // get cell visiblity
        bool getv();

        // get cell visited flag
        bool getm();

        // set npc_t value of cell
        void setNPC(NPC_t);

        // set item_t value of cell
        void setItem(item_t);

        // set terrain_t value of cell
        void setTerrain(terrain_t);

        // set feature_t value of cell
        void setFeature(feature_t);

        // set fire_t value of cell
        void setFire(fire_t);

        // set visibility of cell
        void setv(bool);

        // set cell memory (was visited)
        void setm(bool);

    private:
        // fields for cell class (described above)
        fire_t fire;
        NPC_t npc;
        feature_t feature;
        item_t item;
        terrain_t terrain;
        bool in_mem;
        bool is_vis;
};

#endif
