#ifndef LEVEL_H_
#define LEVEL_H_

#include "builder.h"
#include "satellite.h"
#include "door.h"
#include "elevator.h"
#include "casino.h"

enum lev_type
{
    LEV_REGULAR,
    LEV_SPECIALVAULT,
    LEV_LOSTLAIR
};

class level
{

public:
    level();
    void initLevel(int,lev_type);
    void addBeing(point,int,being_stats,bool,bool);
    void addBeingGivenInventory(being *);
    void giveDealerItems(being *);
    void NPCGetRandomTargetDestinationCallee(being *);
    void addMiscFeatures(terrain_t,int,bool);
    // NPC methods
    void addBeings();
    void addItems();
    void destroyAllBeings();
    void destroyNPC(point);
    being *getBeing(int);
    being *getBeingFromID(int);
    being *getBeing(point);
    // item and pile methods
    void createPileAt(point);
    void deletePileAt(point);
    void deleteAllPiles();
    void deleteItemFromPile(point, int, int);
    void addItem(item_stats, int, point);
    void addCreditsGreedyMethod(uint64, point);
    void rollOneNPC(NPC_t, NPC_t, NPC_t, bool);
    cell_pile* getPile(point);
    bool isPileAt(point);
    // machine methods
    void addDoors();

    int getNumItemPiles();
    cell_pile* getPile(int);

    bool addRandomNPC();

    map *getMap();

    door *getDoor(int);
    door *getDoor(point);

    elevator *getElevator(int);
    elevator *getElevator(point);

    void addDoor(point, int, STRING, door_type);
    void addElevator(int);
    void destroyDoor(point);
    void destroyAllDoors();
    void destroyAllElevators();

    void addVaultItems();
    void addOneItem(point,int);

    void addEntranceLoc();
    void addElevators();

    int getNumNPCsOfType(NPC_t n);
    int getNumNPCs();
    int getNumDoors();
    int getNumElevators(); // should be between 2 and 4 inclusive

    void procgen_NPC_factory(point,int);
    void addStartingAllies();
    void setAttackIDToRandomAlly(being *b);
    void destroyAllNonAllies();

    int getNPCIDCounter();
    void incNPCIDCounter();

    // machine methods
    slot *getSlotMachine(int);
    slot *getSlotMachine(point);
    diamond *getDiamondMachine(int);
    diamond *getDiamondMachine(point);
    void addMachines();
    point getMachineLocOfType(terrain_t, int);
    int getNumMachinesOfType(terrain_t);
    void destroyAllMachines();
    void addMachine(point,int);
    int getNumSlotMachines();
    int getNumDiamondMachines();

    // satellite methods
    satellite *getSatellite(point);
    void addSatellite(point,int);
    void addSatellites();
    void destroyAllSatellites();
    void setSidewaysElevatorSpawnFlag(bool);
    void setBlockedArchwaySpawnFlag(bool);
    int getNumSatellites();
    int getSpecLevVal();

    void loadLayoutFromFile(STRING);
    void initLevel1SpecialRoom();
    point getEntrance();

    point getRandWalkablePoint(int,int,int,int);
    point getRangeTerrainTile(point, terrain_t, int);

private:
    map m;
    VECTOR <door> door_objs;
    VECTOR <being> beings;
    VECTOR <cell_pile> piles;
    VECTOR <slot> slot_objs;
    VECTOR <diamond> diamond_objs;
    VECTOR <elevator> elevators;
    VECTOR <satellite> satellites;
    int level_number;
    int NPC_id_counter;
    int spec_lev_val;
    bool can_spawn_sideways_elevator;
    bool can_spawn_blocked_archway;
    point entrance_loc;
};

point getRandPoint();
item_stats getModifiedUpgradeCharacteristics(item_t);
item_stats getModifiedDeviceCharacteristics(item_t, int);

#endif
