#include "level.h"

level::level()
{
    level_number = 1;
    NPC_id_counter = 1;
    can_spawn_sideways_elevator = true;
    can_spawn_blocked_archway = true;
}

void level::initLevel(int lev, lev_type lt)
{
    level_number = lev;
    NPC_id_counter = 1;
    spec_lev_val = -1;
    switch(lt)
    {
        case(LEV_REGULAR):
        {
            buildMap(&m,level_number);
            addDoors();
            addMachines();
            addSatellites();
            addItems();
            addVaultItems();
            addEntranceLoc();
            addBeings();
            addElevators();
            addMiscFeatures(SIDEWAYS_ELEVATOR,22,can_spawn_sideways_elevator);
            addMiscFeatures(BLOCKED_ARCHWAY,16,can_spawn_blocked_archway);
            break;
        }
        default:
        {
            initMapCells(&m,WALL_DESTRUCT);
            if (lt == LEV_LOSTLAIR)
            {
                can_spawn_blocked_archway = false;
                loadLayoutFromFile("special_room0.txt");
            }
            else
            {
                can_spawn_sideways_elevator = false;
                loadLayoutFromFile("special_room1.txt");
            }
            break;
        }
    }
}

int level::getSpecLevVal()
{
    return spec_lev_val;
}

// vault level
void level::addMiscFeatures(terrain_t t, int roller, bool f)
{
    if (ROLL(roller) > 0)
        return;

    if (!f)
        return;

    if (level_number == 30)
        return;

    point p;

    do
    {
        p = getRandWalkablePoint(1,1,MAPWID-2,MAPHGT-2);
    }
    while (isAt(p,point(0,0)));

    getMap()->setTerrain(p,t);
}

void level::addEntranceLoc()
{
    point p;

    do
    {
        p = getRandWalkablePoint(1,1,MAPWID-2,MAPHGT-2);
    }
    while (isAt(p,point(0,0)) || numTerrainTilesAdj(getMap(),p,FLR_GEN,1,false) < 8);

    entrance_loc = p;
}

void level::addElevator(int lev)
{
    if (lev == 0 || lev > 8 || lev < -8)
        return;

    if (lev != 1 && (lev + level_number < 1 || lev + level_number > 29))
        return;

    point loc;

    do
    {
        loc = getRandWalkablePoint(2,2,MAPWID-3,MAPHGT-3);
    }
    while (isAt(loc,point(0,0)));

    if (lev <= -1)
        getMap()->setTerrain(loc,PREV_STAIRS);
    else if (lev >= 1)
        getMap()->setTerrain(loc,NEXT_STAIRS);

    elevators.push_back(elevator(loc,lev));
}

void level::addElevators()
{
    if (level_number < 30)
        addElevator(1);
    if (level_number > 1)
        addElevator(-1);

    if (ROLL(5) == 0)
        if (level_number < 28)
            addElevator(ROLL(6)+2);

    if (ROLL(5) == 0)
        if (level_number > 2 && level_number < 30)
            addElevator(-(ROLL(6)+2));
}

void level::addItems()
{
    for (int x = 2; x < MAPWID - 3; ++x)
    for (int y = 2; y < MAPHGT - 3; ++y)
        if (getMap()->getTerrain(point(x,y)) == FLR_GEN ||
            getMap()->getTerrain(point(x,y)) == WALL_INDESTRUCT)
        {
            if (ROLL(14) == 0)
                addOneItem(point(x,y),level_number);
        }
    deletePileAt(point(21,8));
    m.setItem(point(21,8),NIL_i);
}

void level::addBeings()
{
    uint num_beings = max_NPCs_per_level[level_number - 1];

    point loc;

    for (uint i = 0; i < num_beings; ++i)
    {
        rollOneNPC(GUARD_NPC,HOPPER_NPC,NIL_n,true);
    }

    if (level_number == 30)
        rollOneNPC(ADVERSARY_NPC,ADVERSARY_NPC,ADVERSARY_NPC,true);

    for (int x = 1; x < MAPWID - 1; ++x)
    for (int y = 1; y < MAPHGT - 1; ++y)
    {
        if (m.getTerrain(point(x,y)) == FLR_DEALERMARKER)
        {
            addBeing(point(x,y),being_default_stats[(int)ARMSDEALER_NPC].max_hit_points,being_default_stats[(int)ARMSDEALER_NPC],false,true);
            getBeing(point(x,y))->setMaxHP(being_default_stats[(int)ARMSDEALER_NPC].max_hit_points + 10 * (level_number - 1));
            getBeing(point(x,y))->setHP(getBeing(point(x,y))->getStats()->max_hit_points);
            giveDealerItems(getBeing(beings.size() - 1));
        }
    }
}

void level::rollOneNPC(NPC_t start_npc, NPC_t end_npc, NPC_t specific_npc, bool initial_npc)
{
    NPC_t type;

    point p;

    bool condition_level;
    bool condition_freq;

    if (specific_npc == NIL_n)
    {
        do
        {
            type = (NPC_t)rollrange((int)start_npc,(int)end_npc);
            condition_level = being_default_stats[(int)type].min_level > level_number;
            condition_freq = PERCCHANCE() > (uint)(being_default_stats[(int)type].frequency + level_number - being_default_stats[(int)type].min_level);
            if (type == HOPPER_NPC)
                condition_freq = PERCCHANCE() > 1;
        }
        while (condition_level || condition_freq);
    }
    else
        type = specific_npc;

    do
    {
        p = getRandWalkablePoint(1,1,MAPWID-2,MAPHGT-2);
    }
    while (m.getv(p) || isAt(p,point(0,0)) || (initial_npc && pointDist(p,getEntrance()) < 4));

    switch(type)
    {
        case(PROCGEN_NPC):
             procgen_NPC_factory(p,0);
             break;
        default:
             addBeing(p,being_default_stats[(int)type].max_hit_points,being_default_stats[(int)type],false,true);
             break;
    }
}

bool level::addRandomNPC()
{
    if (spec_lev_val > -1)
        return false;

    if (getNumNPCs() >= (int)max_NPCs_per_level[level_number - 1])
        return false;

    rollOneNPC(GUARD_NPC,HOPPER_NPC,NIL_n,false);

    return true;
}

void level::addCreditsGreedyMethod(uint64 creds, point at)
{
    int credit_worth_iter = 10;
    uint64 amount = 1;
    do
    {
        if (creds >= credit_worth[credit_worth_iter])
        {
            amount = (uint64)(creds/credit_worth[credit_worth_iter]);
            addItem(item_default_stats[credit_worth_iter + 1],(int)amount,at);
            creds -= amount * credit_worth[credit_worth_iter];
        }
        credit_worth_iter--;
    }
    while (credit_worth_iter >= 0);
}

int level::getNumNPCs()
{
    return beings.size();
}

int level::getNumNPCsOfType(NPC_t n)
{
    int counter = 0;
    for (int i = 0; i < (int)beings.size(); ++i)
    {
       if (getBeing(i)->getType() == n)
       {
           counter++;
       }
    }
    return counter;
}

elevator *level::getElevator(int n)
{
    return &elevators[n];
}

door *level::getDoor(int n)
{
    return &door_objs[n];
}

being *level::getBeing(int n)
{
    return &beings[n];
}

// lambda expression
being *level::getBeingFromID(int id)
{
    VECTOR <being> :: iterator it = FINDIF(beings.begin(), beings.end(), [id](being &b)
    {
        return b.getID() == id;
    });
    return &*it;
}

being *level::getBeing(point p)
{
    VECTOR <being> :: iterator it = FINDIF(beings.begin(), beings.end(), [p](being &b)
    {
        return isAt(b.at(),p);
    });
    return &*it;
}

door *level::getDoor(point p)
{
    VECTOR <door> :: iterator it = FINDIF(door_objs.begin(), door_objs.end(), [p](door &d)
    {
        return isAt(d.at(),p);
    });
    return &*it;
}

elevator *level::getElevator(point p)
{
    VECTOR <elevator> :: iterator it = FINDIF(elevators.begin(), elevators.end(), [p](elevator &e)
    {
        return isAt(e.getLoc(),p);
    });
    return &*it;
}

int level::getNumDoors()
{
    return door_objs.size();
}

int level::getNumElevators()
{
    return elevators.size();
}

void level::addDoor(point p, int state, STRING sgn, door_type dt)
{
    door_objs.push_back(door(p,state,sgn,dt));
    if (state == -1)
        m.setTerrain(p,OPEN_UNLOCKED_DOOR);
    else
        m.setTerrain(p,CLOSED_UNLOCKED_DOOR);
}

void level::addDoors()
{
    STRING door_sign = " ";
    door_type dt = GENERIC_DOOR_TYPE;

    for (int x = 2; x < MAPWID - 2; ++x)
    for (int y = 2; y < MAPHGT - 2; ++y)
    {
        door_sign = " ";
        dt = GENERIC_DOOR_TYPE;

        // terrain acts as markers for adding actual
        // doors...
        if (numTerrainTilesAdj(&m,point(x,y),FLR_VAULTMARKER,1,false) > 0)
        {
            door_sign = "DO NOT ENTER";
            dt = VAULT_DOOR_TYPE;
        }
        else if (numTerrainTilesAdj(&m,point(x,y),FLR_CASINOMARKER,1,false) > 0)
        {
            door_sign = "CASINO";
            dt = CASINO_DOOR_TYPE;
        }

        switch(m.getTerrain(point(x,y)))
        {
        case(OPEN_UNLOCKED_DOOR):
            addDoor(point(x,y),-1,door_sign,dt);
            break;
        case(CLOSED_UNLOCKED_DOOR):
            addDoor(point(x,y),0,door_sign,dt);
            break;
        default:
            break;
        }
    }
}

void level::destroyAllElevators()
{
    for (uint i = 0; i < elevators.size(); ++i)
        m.setTerrain(elevators[i].getLoc(),NIL_t);

    VECTOR<elevator>().swap(elevators);
}

void level::destroyAllDoors()
{
    for (uint i = 0; i < door_objs.size(); ++i)
        m.setTerrain(door_objs[i].at(),NIL_t);

    VECTOR<door>().swap(door_objs);
}

void level::destroyDoor(point p)
{
    for (uint i = 0; i < door_objs.size(); ++i)
        if (isAt(p,door_objs[i].at()))
        {
            door_objs.erase(door_objs.begin() + i);
            m.setTerrain(p,NIL_t);
            return;
        }
}

void level::destroyAllBeings()
{
    for (uint i = 0; i < beings.size(); ++i)
        m.setNPC(beings[i].at(),NIL_n);

    VECTOR<being>().swap(beings);
}

void level::destroyAllNonAllies()
{
    uint num_npcs = beings.size();

    for (uint i = 0; i < num_npcs; ++i)
        m.setNPC(beings[i].at(),NIL_n);

    int index = 0;

    for (uint i = 0; i < num_npcs; ++i)
    {
        if (getBeing(index)->getAllyValue() <= 0)
            beings.erase(beings.begin()+index);
        else
            index++;
    }
}

void level::destroyNPC(point p)
{
    for (uint i = 0; i < beings.size(); ++i)
        if (isAt(p,beings[i].at()))
        {
            beings.erase(beings.begin() + i);
            m.setNPC(p,NIL_n);
            return;
        }
}

point level::getEntrance()
{
    return entrance_loc;
}

void level::incNPCIDCounter()
{
    NPC_id_counter++;
}

int level::getNPCIDCounter()
{
    return NPC_id_counter;
}

// only used safely with the satellites for now...
void level::addBeingGivenInventory(being *b)
{
    beings.push_back(being(b->at(),b->getHP(),*(b->getStats()),false,true,NPC_id_counter,b->getName()));

    int danger_level = b->getStats()->danger_level;

    beings[beings.size() - 1].setCredits(2*(ROLL(49)+1)*danger_level*danger_level*danger_level*danger_level);
    beings[beings.size() - 1].setFactoryColor(b->getFactoryColor());
    beings[beings.size() - 1].setFactoryAscii(b->getFactoryAscii());
    m.setNPC(b->at(),b->getStats()->b_type);
    NPCGetRandomTargetDestinationCallee(getBeing(b->at()));
    NPC_id_counter++;

    for (int i = 0; i < b->getInv()->getNumItems(); ++i)
    {
        getBeing(b->at())->getInv()->addToContainer(*(b->getInv()->getItem(i)->getStats()),b->getInv()->getItem(i)->getQuant());
    }
}

void level::addBeing(point p, int hp, being_stats bstat, bool agg, bool act)
{
    beings.push_back(being(p,hp,bstat,agg,act,NPC_id_counter,bstat.name));

    //int danger_level = getBeing(p)->getStats()->danger_level;

    if (bstat.b_type != HOPPER_NPC)
        beings[beings.size() - 1].setCredits(ROLL(99)+1);
    else
        beings[beings.size() - 1].setCredits(ROLL(9999999*level_number)+1);
    m.setNPC(p,bstat.b_type);
    NPCGetRandomTargetDestinationCallee(getBeing(p));
    NPC_id_counter++;

    giveBeingWeaponsAmmoArmor(getBeing(beings.size()-1));
}

void level::addVaultItems()
{
    for (int i = 1; i < MAPWID - 1; ++i)
    for (int j = 1; j < MAPHGT - 1; ++j)
    {
        if (getMap()->getTerrain(point(i,j)) == FLR_VAULTMARKER)
            addOneItem(point(i,j),level_number + 6);
    }
}

void level::addOneItem(point p, int danger_factor)
{
    item_t it;
    item_stats istats;
    int quant;
    if (ROLL(11) > 0)
    {
        do
        {
            it = (item_t)rollrange((int)METAL_ROD,(int)BALLISTIC_ARMOR);
        }
        while (PERCCHANCE() > (uint)item_default_stats[(int)it].frequency);

        switch(item_default_stats[(int)it].category)
        {
        case(ITEM_AMMUNITION):
            istats = item_default_stats[(int)it];
            quant = item_default_stats[(int)it].default_quantity*(1 + ROLL(danger_factor));
            break;
        case(ITEM_WEAPON_UPGRADE):
            istats = getModifiedUpgradeCharacteristics(it);
            quant = 1;
            break;
        case(ITEM_FIREARM_WEAPON):
        case(ITEM_MELEE_WEAPON):
            istats = getModifiedWeaponCharacteristics(it,danger_factor);
            quant = 1;
            break;
        case(ITEM_ARMOR):
            istats = getModifiedArmorCharacteristics(it,danger_factor);
            quant = 1;
            break;
        case(ITEM_DEVICE):
            istats = getModifiedDeviceCharacteristics(it,danger_factor);
            quant = 1;
            break;
        default:
            istats = item_default_stats[(int)it];
            quant = 1;
            break;
        }

        addItem(istats,quant,p);
    }
    else
    {
        if (ROLL(1) == 0)
            addCreditsGreedyMethod(1 + (uint64)((ROLL(10*danger_factor)+1)*
                                                (ROLL(11*danger_factor)+1)*
                                                (ROLL(12*danger_factor)+1)*danger_factor),p);
        else
            addCreditsGreedyMethod(1 + (uint64)ROLL(10000*danger_factor),p);
    }
}

void level::giveDealerItems(being *b)
{
    const int danger_factor = level_number + 2;

    const int num_items = ROLL((int)level_number/2) + 6;

    item_stats curr_istats;

    item_t it;

    int sub_quant = 1;

    // give dealer weapons
    for (int n = 0; n < num_items; ++n)
    {
        do
        {
            it = (item_t)rollrange((int)METAL_ROD,(int)BALLISTIC_ARMOR);
        }
        while (PERCCHANCE() > (uint)item_default_stats[(int)it].frequency);

        switch(item_default_stats[(int)it].category)
        {
        case(ITEM_ARMOR):
            sub_quant = 1;
            curr_istats = getModifiedArmorCharacteristics(it,danger_factor);
            break;
        case(ITEM_MELEE_WEAPON):
        case(ITEM_FIREARM_WEAPON):
            sub_quant = 1;
            curr_istats = getModifiedWeaponCharacteristics(it,danger_factor);
            break;
        case(ITEM_AMMUNITION):
            sub_quant = item_default_stats[(int)it].default_quantity * danger_factor;
            curr_istats = item_default_stats[(int)it];
            break;
        case(ITEM_WEAPON_UPGRADE):
            sub_quant = 1;
            curr_istats = getModifiedUpgradeCharacteristics(it);
            break;
        case(ITEM_MEDKIT):
        case(ITEM_STATBOOSTER):
            sub_quant = 1;
            curr_istats = item_default_stats[(int)it];
            break;
        case(ITEM_DEVICE):
            sub_quant = 1;
            curr_istats = getModifiedDeviceCharacteristics(it,danger_factor);
            break;
        default:
            break;
        }

        b->getInv()->addToContainer(curr_istats,sub_quant);
    }
}

point level::getRandWalkablePoint(int minx, int miny, int maxx, int maxy)
{
    // so that it doesn't go in an infinite loop (in a very rare case)
    int tries = 0;

    point ret_val = point(0,0);

    while ((unWalkableCell(m.getCell(ret_val)) || m.getTerrain(ret_val) != FLR_GEN) && tries < 1000)
    {
        ret_val.set(ROLL(maxx - minx) + minx,ROLL(maxy - miny) + miny);
        tries++;
    }

    if (tries == 1000)
        return getRandPoint();

    return ret_val;
}

map *level::getMap()
{
    return &m;
}

void level::NPCGetRandomTargetDestinationCallee(being *b)
{
    point goal = getRandWalkablePoint(1,1,MAPWID-2,MAPHGT-2);

    b->setPathfindDest(goal);
}

point level::getRangeTerrainTile(point p, terrain_t t, int rad)
{
    for (int x = -rad; x <= rad; ++x)
        for (int y = -rad; y <= rad; ++y)
        {
            if (x != 0 || y != 0)
                if (getMap()->getTerrain(point(p.x()+x,p.y()+y)) == t)
                    return point(p.x()+x,p.y()+y);
        }

    return point(1,1);
}

// PILE MANIPULATION METHODS

void level::createPileAt(point p)
{
    piles.push_back(cell_pile(p));
}

void level::deleteItemFromPile(point p, int selection, int select_q)
{
    cell_pile * cp = getPile(p);

    cp->removeItem(selection,select_q);

    if (cp->getNumItems() == 0)
    {
        deletePileAt(p);
        m.setItem(p,NIL_i);
    }
    else
        m.setItem(p,cp->getItem(0)->getType());
}

void level::deletePileAt(point p)
{
    int piles_index = -1;

    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            piles_index = i;

    if (piles_index != -1)
        piles.erase(piles.begin() + piles_index);
}

void level::addItem(item_stats istats, int quant, point p)
{
    int piles_index = -1;

    int num_piles = piles.size();

    for (int i = 0; i < num_piles; ++i)
        if (isAt(piles[i].getLoc(),p))
            piles_index = i;

    if (piles_index != -1)
        piles[piles_index].addToContainer(istats,quant);
    else
    {
        createPileAt(p);
        piles[num_piles].addToContainer(istats,quant);
        m.setItem(p,istats.i_type);
    }
}

bool level::isPileAt(point p)
{
    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            return true;

    return false;
}

int level::getNumItemPiles()
{
    return piles.size();
}

cell_pile *level::getPile(int i)
{
    return &piles[i];
}

cell_pile *level::getPile(point p)
{
    for (uint i = 0; i < piles.size(); ++i)
        if (isAt(piles[i].getLoc(),p))
            return &piles[i];

    // should never happen
    return &piles[0];
}

void level::deleteAllPiles()
{
    for (uint i = 0; i < piles.size(); ++i)
        piles[i].deleteAllItems();

    VECTOR<cell_pile>().swap(piles);
}

point getRandPoint()
{
    return point(ROLL(MAPWID-3)+1,ROLL(MAPHGT-3)+1);
}

void level::setAttackIDToRandomAlly(being *b)
{
    uint rand_attack_id = 0;

    for (int n = 0; n < getNumNPCs(); ++n)
    {
        if (getBeing(n)->getAllyValue() > 0)
        {
            rand_attack_id = getBeing(n)->getID();
            break;
        }
    }

    b->setAttackID(rand_attack_id);
}

item_stats getModifiedUpgradeCharacteristics(item_t upgrader)
{
    item_stats base_upgrader_stat = item_default_stats[(int)upgrader];

    int roller = ROLL(99);

    if (roller > 49)
    {
        base_upgrader_stat.misc_val = 0;
        base_upgrader_stat.name.insert((int)base_upgrader_stat.name.size() - 7,"damage ");
    }
    if (roller >= 15 && roller <= 49)
    {
        base_upgrader_stat.misc_val = 1;
        base_upgrader_stat.name.insert((int)base_upgrader_stat.name.size() - 7,"accuracy ");
    }
    if (roller < 15)
    {
        base_upgrader_stat.misc_val = 2;
        base_upgrader_stat.name.insert((int)base_upgrader_stat.name.size() - 7,"capacity ");
    }

    return base_upgrader_stat;
}

slot *level::getSlotMachine(int n)
{
    return &slot_objs[n];
}

diamond *level::getDiamondMachine(int n)
{
    return &diamond_objs[n];
}

satellite *level::getSatellite(point p)
{
    VECTOR <satellite> :: iterator it = FINDIF(satellites.begin(), satellites.end(), [p](satellite &s)
    {
        return isAt(s.at(),p);
    });
    return &*it;
}

slot *level::getSlotMachine(point p)
{
    VECTOR <slot> :: iterator it = FINDIF(slot_objs.begin(), slot_objs.end(), [p](slot &s)
    {
        return isAt(s.at(),p);
    });
    return &*it;
}

diamond *level::getDiamondMachine(point p)
{
    VECTOR <diamond> :: iterator it = FINDIF(diamond_objs.begin(), diamond_objs.end(), [p](diamond &d)
    {
        return isAt(d.at(),p);
    });
    return &*it;
}

int level::getNumMachinesOfType(terrain_t t)
{
    switch(t)
    {
    case(SLOT_MACHINE):
        return getNumSlotMachines();
    case(DIAMONDS_MACHINE):
        return getNumDiamondMachines();
    default:
        break;
    }

    // should never happen
    return 0;
}

point level::getMachineLocOfType(terrain_t t, int i)
{
    switch(t)
    {
    case(SLOT_MACHINE):
        return getSlotMachine(i)->at();
    case(DIAMONDS_MACHINE):
        return getDiamondMachine(i)->at();
    default:
        break;
    }

    // should NEVER happen
    return point(1,1);
}

void level::addMachine(point p, int dl)
{
    if (ROLL(2) == 0)
    {
        diamond_objs.push_back(diamond(p,dl,0));
        diamond_objs[diamond_objs.size() - 1].initDiamonds();
        m.setTerrain(p,DIAMONDS_MACHINE);
    }
    else
    {
        slot_objs.push_back(slot(p,dl,0));
        slot_objs[slot_objs.size() - 1].initSlots();
        m.setTerrain(p,SLOT_MACHINE);
    }
}

void level::addSatellite(point p, int dl)
{
    point sp;

    if (m.getTerrain(point(p.x()-2,p.y())) == SATELLITE_SPAWN_TILE)
        sp = point(p.x()-2,p.y());
    else if (m.getTerrain(point(p.x()+2,p.y())) == SATELLITE_SPAWN_TILE)
        sp = point(p.x()+2,p.y());
    else if (m.getTerrain(point(p.x(),p.y()-2)) == SATELLITE_SPAWN_TILE)
        sp = point(p.x(),p.y()-2);
    else
        sp = point(p.x(),p.y()+2);

    satellites.push_back(satellite(p,sp,dl));
    satellites[satellites.size()-1].genSatelliteNPC(NPC_id_counter);
}

void level::destroyAllSatellites()
{
    for (uint i = 0; i < satellites.size(); ++i)
        m.setTerrain(satellites[i].at(),NIL_t);

    for (uint i = 0; i < satellites.size(); ++i)
        m.setTerrain(satellites[i].getSpawnLoc(),NIL_t);

    VECTOR<satellite>().swap(satellites);
}

int level::getNumSatellites()
{
    return satellites.size();
}

void level::addSatellites()
{
    for (int x = 1; x < MAPWID - 1; ++x)
    for (int y = 1; y < MAPHGT - 1; ++y)
    {
        if (m.getTerrain(point(x,y)) == SATELLITE_TILE)
            addSatellite(point(x,y),level_number);
    }
}

void level::addMachines()
{
    for (int x = 1; x < MAPWID - 1; ++x)
        for (int y = 1; y < MAPHGT - 1; ++y)
        {
            if (m.getTerrain(point(x,y)) == FLR_MACHINEMARKER)
                addMachine(point(x,y),MINIMUM(20,(int)ROLL(5+(int)(level_number/2))+1));
        }
}

void level::destroyAllMachines()
{
    for (uint i = 0; i < slot_objs.size(); ++i)
        m.setTerrain(slot_objs[i].at(),NIL_t);

    for (uint i = 0; i < diamond_objs.size(); ++i)
        m.setTerrain(diamond_objs[i].at(),NIL_t);

    VECTOR<slot>().swap(slot_objs);
    VECTOR<diamond>().swap(diamond_objs);
}

int level::getNumSlotMachines()
{
    return slot_objs.size();
}

int level::getNumDiamondMachines()
{
    return diamond_objs.size();
}

item_stats getModifiedDeviceCharacteristics(item_t it, int danger_factor)
{
    item_stats device_stat = item_default_stats[(int)it];

    int roller = ROLL(99);

    if (roller > 49)
    {
        device_stat.misc_val = 1;
    }
    if (roller >= 15 && roller <= 49)
    {
        device_stat.misc_val = 1 + ROLL(1) + ROLL((int)(danger_factor/3));
    }
    if (roller < 15)
    {
        device_stat.misc_val = 1 + ROLL(2) + ROLL((int)(danger_factor/2));
    }

    if (it == TIMESTOPPER_DEVICE)
        device_stat.misc_val += 4;

    return device_stat;
}

void level::procgen_NPC_factory(point p, int set_danger_lev)
{
    // danger level
    int d;
    int d_max = MINIMUM(level_number,25);

    if (set_danger_lev <= 0)
        d = rollrange(1,d_max);
    else
        d = set_danger_lev;

    being_stats procgen_stats = getProcgenStats(d,level_number);

    chtype procgen_chtype = getProcgenChtype(d,procgen_stats.name);

    addBeing(p,procgen_stats.max_hit_points,procgen_stats,false,true);
    getBeing(beings.size() - 1)->setFactoryAscii(procgen_chtype.ascii);
    getBeing(beings.size() - 1)->setFactoryColor(procgen_chtype.color.fg);
}

void level::addStartingAllies()
{
    //being_stats marine_stats = being_default_stats[(int)MARINE_NPC];

    point p;

    point e = getEntrance();

    for (int i = 0; i < 1; ++i)
    {
        do
        {
            p = getRandWalkablePoint(e.x()-4,e.y()-4,e.x()+4,e.y()+4);
        } while (isAt(p,e));

        addBeing(p,procgen_starthumanally[i].max_hit_points,procgen_starthumanally[i],false,true);
        getBeing(p)->setFactoryAscii((int)'@');
        getBeing(p)->setFactoryColor(color_gray);
        getBeing(p)->setAllyValue(2);
    }
}

void level::loadLayoutFromFile(STRING file_name)
{
     // load room array template with '.'s
    VECTOR <char> room_vec;

    std::ifstream room_file;

    cell c;

    int dimx, dimy;
    int locx, locy;

    point p;

    room_file.open(file_name);

    room_file >> dimx;
    room_file >> dimy;
    room_file >> locx;
    room_file >> locy;
    room_file >> level_number;
    room_file >> spec_lev_val;

    for (int i = 0; i < dimx * dimy; ++i)
    {
      room_vec.push_back('#');
    }

    for (int i = 0; i < dimx * dimy; ++i)
    {
      room_file >> room_vec[i];
    }

    room_file.close();

    for (int x = 0; x < dimx; ++x)
    for (int y = 0; y < dimy; ++y)
    {
        p.set(locx + x,locy + y);

        c.setTerrain(FLR_GEN);

        switch(room_vec[(dimx * y) + x])
        {
            case('#'):
                     c.setTerrain(WALL_DESTRUCT);
                     break;
            case('.'):
                     c.setTerrain(FLR_GEN);
                     break;
            case('+'):
                     c.setTerrain(CLOSED_UNLOCKED_DOOR);
                     addDoor(p,0," ",GENERIC_DOOR_TYPE);
                     break;
            case('1'):
                     addItem(item_default_stats[(int)TIMESTOPPER_DEVICE],1,p);
                     getPile(p)->getItem(0)->setMiscVal(60);
                     break;
            case('2'):
                     if (ROLL(1) == 0)
                         addItem(item_default_stats[(int)FUSION_CANNON],1,p);
                     else
                         addItem(item_default_stats[(int)NUCLEAR_ARMOR],1,p);
                     break;
            case('@'):
                     entrance_loc = p;
                     if (spec_lev_val == (int)LEV_SPECIALVAULT - 1)
                         c.setTerrain(SIDEWAYS_ELEVATOR);
                     else
                         c.setTerrain(BLOCKED_ARCHWAY);
                     break;
            case('M'):
                     addBeing(p,being_default_stats[(int)ELITEKILLER_NPC].max_hit_points,being_default_stats[(int)ELITEKILLER_NPC],false,true);
                     getBeing(p)->getInv()->setWeapon(0);
                     getBeing(p)->setAttackID(0);
                     break;
            case('&'):
                     addBeing(p,being_default_stats[(int)JUGGERNAUT_NPC].max_hit_points,being_default_stats[(int)JUGGERNAUT_NPC],false,true);
                     getBeing(p)->setMobility(false);
                     break;
            case('G'):
                     addBeing(p,being_default_stats[(int)ELITEGUARD_NPC].max_hit_points,being_default_stats[(int)ELITEGUARD_NPC],false,true);
                     c.setTerrain(NEXT_STAIRS);
                     elevators.push_back(elevator(p,ROLL(16)+9));
                     break;
            case('Y'):
                     addBeing(p,being_default_stats[(int)YINAXIMAI_NPC].max_hit_points,being_default_stats[(int)YINAXIMAI_NPC],false,true);
                     getBeing(p)->getInv()->addToContainer(item_default_stats[(int)ANCIENT_FLAIL],1);
                     getBeing(p)->getInv()->setWeapon(0);
                     break;
            default:
                     break;
        }

        m.setTerrain(p,c.getTerrain());
    }

    VECTOR<char>().swap(room_vec);
}

void level::initLevel1SpecialRoom()
{
    loadLayoutFromFile("special_room2.txt");
    addDoor(point(21,8),0,"EXPRESS ELEVATOR",GENERIC_DOOR_TYPE);
}

void level::setBlockedArchwaySpawnFlag(bool f)
{
    can_spawn_blocked_archway = f;
}

void level::setSidewaysElevatorSpawnFlag(bool f)
{
    can_spawn_sideways_elevator = f;
}
