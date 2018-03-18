#include "game.h"

game::game()
{
    // current program starts off as executing (as indicated by this flag)
    program_executing = true;
    // current game is initially not active (but will be once player starts a new game)
    game_active = true;
}

void game::displayMappedOutLevel()
{
    for (int i = 1; i < MAPWID - 1; ++i)
    for (int j = 1; j < MAPHGT - 1; ++j)
    {
        if ((getMap()->getTerrain(point(i,j)) != WALL_INDESTRUCT && getMap()->getTerrain(point(i,j)) != WALL_DESTRUCT) ||
            numTerrainTilesAdj(getMap(),point(i,j),WALL_INDESTRUCT,1,false) + numTerrainTilesAdj(getMap(),point(i,j),WALL_DESTRUCT,1,false) < 8)
            {
                getMap()->setm(point(i,j),true);
                getMap()->setu(point(i,j),blank_ch);
            }
    }
}

void game::deallocateGameData()
{
    getPlayer()->getInv()->deleteAllItems();
    getPlayer()->getInv()->setWeapon(item_default_stats[(int)NIL_i]);
    getPlayer()->getInv()->setArmor(item_default_stats[(int)NIL_i]);
    getPlayer()->setBeing(point(1,1),being_default_stats[(int)PLAYER].max_hit_points,being_default_stats[(int)PLAYER],false,true,0,"player");
}

// initialize current game variables, pointers, etc...
void game::initGame()
{
    msgeAdd("Welcome to Spire!",colorpair_purple_black);
    msgeAdd("Press '/' for commands and info screen.",colorpair_white_black);
    getPlayer()->getInv()->addToContainer(item_default_stats[(int)PISTOL_10MM],1);
    getPlayer()->getInv()->addToContainer(item_default_stats[(int)HANDGUN_AMMO_GEN],24);
    getPlayer()->getInv()->setWeapon(0);
    // only used when I was taking screenshots; should stay commented.
    /*
    for (int i = 0; i < 5; ++i)
         getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(MAPPING_DEVICE,ROLL(10)+1),1);

    for (int i = 0; i < 5; ++i)
         getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(NPCDETECTOR_DEVICE,ROLL(10)+1),1);
    */
    playerReloadFirearm();

    getPlayer()->setCredits(500ULL);

    num_game_turns = 0;

    last_regular_level = level_number = 1;

    level_obj.setSidewaysElevatorSpawnFlag(true);
    level_obj.setBlockedArchwaySpawnFlag(true);

    level_obj.initLevel(level_number,LEV_REGULAR);
    level_obj.initLevel1SpecialRoom();

    player_has_moved = true;

    player_last_attack = false;

    summoned_agent = false;

    wayback_unlocked = false;

    debug_mode = false;

    setupPlayer();

    level_obj.addStartingAllies();

    AI_stop_counter = 0;

    player_quick_fire_spot = getPlayer()->at();
}

void game::gotoPrevLevel()
{
    if (level_number == 1)
    {
        msgeAdd("You are already on the bottom level.",colorpair_white_black);
        return;
    }

    if (getMap()->getTerrain(getPlayer()->at()) == PREV_STAIRS)
    {
        int e_lev = level_obj.getElevator(getPlayer()->at())->getLevel();

        if (e_lev == -1)
            msgeAddPromptSpace("You take the elevator going down...",colorpair_white_black);
        else
            msgeAddPromptSpace("You take the express elevator going down...",colorpair_purple_black);

        changeLevel(e_lev,LEV_REGULAR);
    }
    else if (getMap()->getTerrain(getPlayer()->at()) == SIDEWAYS_ELEVATOR)
    {
        msgeAdd("Press '>' to take the sideways elevator.",colorpair_gray_black);
    }
    else if (getMap()->getTerrain(getPlayer()->at()) == BLOCKED_ARCHWAY)
    {
        msgeAdd("Press '>' to enter the archway.",colorpair_gray_black);
    }
    else
        msgeAdd("There is no downwards elevator here!",colorpair_gray_black);
}

void game::gotoNextLevel()
{
    if (level_number == 30)
    {
        msgeAdd("You are already on the top level; you can't go anywhere from here.",colorpair_white_black);
        return;
    }

    if (getMap()->getTerrain(getPlayer()->at()) == NEXT_STAIRS)
    {
        int e_lev = level_obj.getElevator(getPlayer()->at())->getLevel();

        if (e_lev == 1)
            msgeAddPromptSpace("You take the elevator going up...",colorpair_white_black);
        else
            msgeAddPromptSpace("You take the express elevator going up...",colorpair_purple_black);

        changeLevel(e_lev,LEV_REGULAR);
    }
    else if (getMap()->getTerrain(getPlayer()->at()) == SIDEWAYS_ELEVATOR)
    {
        promptUseSidewaysElevator();
    }
    else if (getMap()->getTerrain(getPlayer()->at()) == BLOCKED_ARCHWAY)
    {
        promptEnterArchway();
    }
    else
        msgeAdd("There is nothing to enter here!",colorpair_gray_black);
}

void game::promptEnterArchway()
{
    switch (level_obj.getSpecLevVal())
    {
        case((int)LEV_REGULAR - 1):
            mbuf.clearAndDeleteAllMessages();
            msgeAdd("Enter the archway? (y = yes)",colorpair_white_black);
            updateScreen();
            if (ibuf.checkForKey('y'))
            {
                msgeAdd("You enter the archway...", colorpair_gray_black);
                changeLevel((-1*level_number)+29,LEV_LOSTLAIR);
            }
            else
                msgeAdd("You don't enter...",colorpair_gray_black);
            break;
        case((int)LEV_LOSTLAIR - 1):
            if (wayback_unlocked)
            {
                msgeAddPromptSpace("You enter the archway...",colorpair_white_black);
                changeLevel((-1*level_number) + last_regular_level,LEV_REGULAR);
            }
            else
                msgeAdd("You enter the archway... it leads to the other side.",colorpair_white_black);
            break;
        default:
            break;
    }
}

void game::promptUseSidewaysElevator()
{
    // I made it random per each use on purpose, so to add a little bit of luck involved with
    // having enough credits.
    uint credit_cost = 10000 + (uint64)ROLL(100000) + 500000ULL*(level_number - 1) + (uint64)ROLL(30000000)*(level_number - 1);
    switch (level_obj.getSpecLevVal())
    {
        case((int)LEV_REGULAR - 1):
            msgeAddPromptSpace("The terminal on the elevator seems to be heavily malfunctioning...",colorpair_white_black);
            msgeAddPromptSpace("The displayed numbers seem to be changing constantly!",colorpair_white_black);
            mbuf.clearAndDeleteAllMessages();
            msgeAdd("On the terminal it says:",colorpair_white_black);
            msgeAdd("\"HIGH SECURITY VAULT -- ENTRANCE FEE: " + int2String(credit_cost) + "\"", colorpair_orange_black);
            msgeAdd("Pay? (y = yes)",colorpair_purple_black);
            updateScreen();
            if (ibuf.checkForKey('y'))
            {
                if (getPlayer()->getCredits() >= credit_cost)
                {
                    getPlayer()->setCredits(getPlayer()->getCredits() - credit_cost);
                    mbuf.clearAndDeleteAllMessages();
                    printPlayerStats();
                    msgeAddPromptSpace("You pay the credits and enter the elevator... It suddenly starts to go somewhere!",colorpair_white_black);
                    changeLevel((-1*level_number)+29,LEV_SPECIALVAULT);
                }
                else
                    msgeAdd("The terminal remains locked due to your lack of funds.", colorpair_gray_black);
            }
            else
            {
                msgeAdd("The terminal remains locked...",colorpair_gray_black);
            }
            break;
        case((int)LEV_SPECIALVAULT - 1):
            if (wayback_unlocked)
            {
                msgeAddPromptSpace("You take the sideways elevator...",colorpair_white_black);
                changeLevel((-1*level_number) + last_regular_level,LEV_REGULAR);
            }
            else
                msgeAdd("The elevator doesn't seem to work now!",colorpair_white_black);
            break;
        default:
            break;
    }
}

void game::changeLevel(int e_lev, lev_type lt)
{
    // get rid of stray '@' from this level
    // before going to the next
    getMap()->setNPC(getPlayer()->at(),NIL_n);

    deallocateGenericData();

    // increment/decrement primary level counter
    level_number += e_lev;

    wayback_unlocked = summoned_agent = player_last_attack = false;

    level_obj.initLevel(level_number,lt);

    setupPlayer();

    setupAllies();

    player_has_moved = true;

    reDisplayAll();

    if (AI_stop_counter > 0)
        msgeAdd("Time resumes...",colorpair_gray_black);

    AI_stop_counter = 0;

    switch(lt)
    {
        case(LEV_REGULAR):
        {
          last_regular_level = level_number;
          msgeAdd("You enter a new level.",colorpair_gray_black);
          break;
        }
        case(LEV_SPECIALVAULT):
        {
          msgeAdd("The elevator suddenly shuts down!",colorpair_gray_black);
          break;
        }
        case(LEV_LOSTLAIR):
        {
          msgeAdd("You enter some kind of lair.",colorpair_gray_black);
          break;
        }
        default:
          break;
    }

    player_quick_fire_spot = getPlayer()->at();
}

// ToDo: Eliminate pathfinding here, and just make it so that an ally is within
// one of 8 adjacent starting tiles. Also, the number of allies the player can have
// at any given point should be 8 *maximum*.
void game::setupAllies()
{
    int dist_iter = 1;

    point pf_source = getPlayer()->at();

    point e = level_obj.getEntrance();

    point p;

    point bp;

    bool end_condition;

    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        if (level_obj.getBeing(i)->getAllyValue() > 0)
        {
            end_condition = false;

            dist_iter = 1;

            while(!end_condition)
            {
                for (int x = -dist_iter; x <= dist_iter; ++x)
                {
                    for (int y = -dist_iter; y <= dist_iter; ++y)
                    {
                        p.set(e.x()+x,e.y()+y);
                        if (inRange(p,1,1,MAPWID-1,MAPHGT-1))
                        if (!isAt(e,p))
                        {
                            if (getMap()->getNPC(p) == NIL_n)
                            if (!end_condition)
                            {
                                bp = pfinder.dijkstra(getMap(),pf_source,p);
                                if (!isAt(bp,pf_source))
                                {
                                    //std::cout << "x: " << bp.x() << "y: " << bp.y() << "\n";
                                    end_condition = true;
                                }
                            }
                        }
                    }
                }
                dist_iter++;
            }

            getMap()->setNPC(bp,level_obj.getBeing(i)->getType());

            level_obj.getBeing(i)->moveTo(bp);
            level_obj.getBeing(i)->setID(level_obj.getNPCIDCounter());
            level_obj.incNPCIDCounter();
            level_obj.getBeing(i)->setAttackID(-1);
            level_obj.getBeing(i)->setFleeID(-1);
            level_obj.getBeing(i)->setActivation(true);

            //pf_source = bp;
        }
    }
}

void game::calculatePlayerLOS()
{
    point p;

    int d = getPlayer()->getStats()->detect_distance;

    getMap()->setAllCellsUnvisible();

    // for all valid cells in range of player
    for (int x = getPlayer()->at().x() - d; x <= getPlayer()->at().x() + d; ++x)
    for (int y = getPlayer()->at().y() - d; y <= getPlayer()->at().y() + d; ++y)
    {
        p.set(x,y);

        if (!inRange(p,1,1,MAPWID-2,MAPHGT-2))
            continue;

        // Is path from p to getPlayer()->at() not blocked?
        // (as you can tell, the line is traversed backwards;
        if (!linetracer.isBlocking(getMap(),getPlayer()->at(),p,true,false,false))
        {
            getMap()->setm(p,true);
            getMap()->setv(p,true);
            getMap()->setu(p,blank_ch);
        }
        else
            getMap()->setv(p,false);
    }
}

bool game::confirmMelee(being *b)
{
    if (b->getStaggerCount() > 0)
        return true;
    if (b->getAttackID() != 0 && isSpecialNonProcgen(b->getType()))
    {
        msgeAdd("Do you really want to attack " + getNameToPrint(b,true,false) + "?",colorpair_white_black);
        msgeAdd("<Press 'y' to confirm.>", colorpair_white_black);
        updateScreen();
        return ibuf.checkForKey('y');
    }
    return true;
}

void game::checkForBeingStateEvent(being *provoker, being *target, bool fire_weapon)
{
    if (target->getAttackID() == -1)
        if (!isSpecialNonProcgen(target->getType()) && target->getType() != PLAYER)
            if (!beingUndetectedChance(provoker,fire_weapon))
                checkForAlertingNPCs(provoker->at(),target->at());

    if (target->getType() != PLAYER)
        checkIfTargetIsHostile(provoker,target);

    if (provoker->getType() == PLAYER)
        getPlayer()->setAttackID(target->getID());
}

void game::melee(being *attacker, being *target)
{
    int num_start_npcs = level_obj.getNumNPCs();

    checkForBeingStateEvent(attacker,target,false);

    for (int i = 0; i < attacker->getStats()->melee_per_turn; ++i)
    {
        if (num_start_npcs != level_obj.getNumNPCs())
            return;

        int total_damage;

        if (baseHitChance(attacker,target,false))
        {
            addHitMissIndicatorMessage(attacker,target,true);

            total_damage = baseMeleeDamage(attacker);

            int crit_mult = checkForCriticalHit(attacker->getInv()->getWeapon(), true);

            total_damage *= crit_mult;

            total_damage -= getArmorProtectionValue(attacker,target,true,false,false);

            if (total_damage <= 0)
            {
                crit_mult = 1;
                total_damage = 0;
            }
            else
                checkForStunEvent(attacker,target,total_damage,true);

            if (getMap()->getv(target->at()))
                damageMsgeAdd(sint2String(total_damage),attacker->getInv()->getWeapon()->getType(),getExactSymbolFromNPC(attacker),
                              getExactSymbolFromNPC(target),(total_damage >= target->getHP()),false,getMap()->getv(attacker->at()),true,crit_mult);


            damageBeing(target, target->at(), total_damage);
        }
        else
            addHitMissIndicatorMessage(attacker,target,false);
    }
}

void game::resetBeingAttackAndFleeIDs(int id)
{
    bool reset_flag;

    if (getPlayer()->getAttackID() == id)
    {
        getPlayer()->setAttackID(-1);
    }

    if (getPlayer()->getFleeID() == id)
    {
        getPlayer()->setFleeID(-1);
    }

    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        reset_flag = false;

        if (level_obj.getBeing(i)->getAttackID() == id)
        {
            reset_flag = true;
            level_obj.getBeing(i)->setAttackID(-1);
        }

        if (level_obj.getBeing(i)->getFleeID() == id)
        {
            reset_flag = true;
            level_obj.getBeing(i)->setFleeID(-1);
        }

        if (reset_flag)
            level_obj.NPCGetRandomTargetDestinationCallee(level_obj.getBeing(i));
    }
}

being *game::getCurrentNPCTurn()
{
    if (last_active_id == 0)
        return getPlayer();
    else
        return level_obj.getBeingFromID(last_active_id);
}

void game::damagePlayer(being *b, int dam)
{
    if (dam >= 10)
        msgeAdd(playerPainStatus(dam),colorpair_red_black);
    if (dam >= b->getHP())
    {
        b->killBeing();
    }
    else
    {
        b->setHP(b->getHP() - dam);
    }
}

void game::damageNPC(being *b, point p, int dam)
{
    if (dam >= b->getHP())
    {
        b->killBeing();
    }
    else
    {
        b->setHP(b->getHP() - dam);
    }
}

void game::playerEquipWeapon(STRING name, int selection)
{
    getPlayer()->getInv()->setWeapon(selection);
    msgeAdd("You are equip the " + name + ".", colorpair_white_black);
}

void game::playerWearArmor(STRING name, int selection)
{
    getPlayer()->getInv()->setArmor(selection);
    msgeAdd("You put on the " + name + ".", colorpair_white_black);
}

bool game::checkPlayerUseItem(int selection)
{
    item_stats * istats = getPlayer()->getInv()->getItem(selection)->getStats();

    switch(istats->category)
    {
        case(ITEM_ARMOR):
            playerWearArmor(istats->name,selection);
            return false;
        case(ITEM_MELEE_WEAPON):
        case(ITEM_FIREARM_WEAPON):
            playerEquipWeapon(istats->name,selection);
            return false;
        case(ITEM_WEAPON_UPGRADE):
            upgradeEquippedWeapon(getPlayer(),selection);
            return false;
        case(ITEM_STATBOOSTER):
            upgradeAttributes(getPlayer(),selection);
            return false;
        case(ITEM_MEDKIT):
            useMedkit(getPlayer(),selection);
            return false;
        case(ITEM_DEVICE):
            clearRange(0,0,GRIDWID-1,SHOWHGT);
            reDisplayAll();
            useDevice(getPlayer(),selection);
            return true;
        case(ITEM_AMMUNITION):
            msgeAdd("This item is ammunition.",colorpair_white_black);
            return false;
        default:
            break;
    }

    return false;
}

void game::playerUseDrill(int selection)
{
    msgeAdd("Drill in what direction?",colorpair_white_black);

    updateScreen();

    ibuf.setAction();

    if (ibuf.getAction() != INP_DELTA)
    {
        msgeAdd("That's not a direction!",colorpair_white_black);
        return;
    }

    point loc = addPoints(getPlayer()->at(),ibuf.getDelta());

    if (getMap()->getTerrain(loc) == WALL_INDESTRUCT)
    {
        getMap()->setTerrain(loc,FLR_GEN);
        decrementDeviceCounter(getPlayer(),selection);
        msgeAdd("You drill through the crystal wall.",colorpair_white_black);
    }
    else if (getMap()->getTerrain(loc) == WALL_DESTRUCT)
        msgeAdd("The wall is too hard to drill through!",colorpair_white_black);
    else
        msgeAdd("There's no wall there!",colorpair_white_black);
}

void game::playerUseMap(int selection)
{
    displayMappedOutLevel();
    decrementDeviceCounter(getPlayer(),selection);
    msgeAdd("You now know the layout of this level.",colorpair_white_black);
}

void game::playerUseNPCDetector(int selection)
{
    msgeAdd("For a moment, you are able to track every being on this level.",colorpair_white_black);
    decrementDeviceCounter(getPlayer(),selection);
    displayAllNPCs();
}

void game::playerUseItemDetector(int selection)
{
    msgeAdd("You detect the locations of every item on this level.",colorpair_white_black);
    decrementDeviceCounter(getPlayer(),selection);
    displayAllItems();
}

void game::playerUseTeleport(int selection)
{
    if (level_obj.getSpecLevVal() != -1)
    {
        msgeAdd("Your teleport device doesn't seem to work here!",colorpair_white_black);
        return;
    }
    msgeAdd("Use directional keys to choose a teleport destination.",colorpair_white_black);
    decrementDeviceCounter(getPlayer(),selection);
    moveCursor(true);
    player_quick_fire_spot = getPlayer()->at();
}

void game::playerUseTimeStopper(int selection)
{
    msgeAdd("You use time stopper...",colorpair_white_black);
    setNPCAIActivation(false,getPlayer()->getInv()->getItem(selection)->getStats()->misc_val + 1);
    getPlayer()->getInv()->removeItem(selection,1);
}

void game::decrementDeviceCounter(being *b, int selection)
{
    if (b->getInv()->getItem(selection)->getStats()->misc_val > 1)
        b->getInv()->getItem(selection)->decrementMiscval();
    else
        b->getInv()->removeItem(selection,1);
}

void game::displayAllItems()
{
    for (int i = 0; i < level_obj.getNumItemPiles(); ++i)
    {
        getMap()->setm(level_obj.getPile(i)->getLoc(),true);
        getMap()->setv(level_obj.getPile(i)->getLoc(),true);
        getMap()->setu(level_obj.getPile(i)->getLoc(),blank_ch);
    }

    for (int x = 0; x < MAPWID; ++x)
    for (int y = 0; y < MAPHGT; ++y)
    {
        disp.printCell(getMap(),point(x,y),blank_ch);
        if (getMap()->getItem(point(x,y)) != NIL_i)
        {
            disp.printCell(getMap(),point(x,y),symbol_item[(int)getMap()->getItem(point(x,y))]);
        }
    }

    msgeAddPromptSpace("Number of item piles: " + sint2String(level_obj.getNumItemPiles()),colorpair_white_black);

    for (int i = 0; i < level_obj.getNumItemPiles(); ++i)
         getMap()->setv(level_obj.getPile(i)->getLoc(),false);
}

void game::displayAllNPCs()
{
    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        getMap()->setm(level_obj.getBeing(i)->at(),true);
        getMap()->setv(level_obj.getBeing(i)->at(),true);
        getMap()->setu(level_obj.getBeing(i)->at(),blank_ch);
    }

    disp.printMap(getMap());
    setCustomCellColors();

    msgeAddPromptSpace("Level population: " + sint2String(level_obj.getNumNPCs()),colorpair_white_black);

    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
         getMap()->setv(level_obj.getBeing(i)->at(),false);
}

void game::useDevice(being *b, int selection)
{
    item_t it = b->getInv()->getItem(selection)->getType();
    switch(it)
    {
    case(DRILL_DEVICE):
        playerUseDrill(selection);
        break;
    case(MAPPING_DEVICE):
        playerUseMap(selection);
        break;
    case(NPCDETECTOR_DEVICE):
        playerUseNPCDetector(selection);
        break;
    case(ITEMDETECTOR_DEVICE):
        playerUseItemDetector(selection);
        break;
    case(TELEPORT_DEVICE):
        playerUseTeleport(selection);
        break;
    case(TIMESTOPPER_DEVICE):
        playerUseTimeStopper(selection);
        break;
    default:
        break;
    }
    return;
}

void game::moveCursor(bool is_teleporting)
{
    point cursor_loc = getPlayer()->at(), temp_loc = getPlayer()->at();

    do
    {
        disp.setAndPrintFire(getMap(),cursor_loc,CURSOR_GEN);

        setCustomCellColors();

        updateScreen();

        ibuf.setAction();

        if (ibuf.getAction() == INP_DELTA)
        {
            temp_loc = addPoints(cursor_loc,ibuf.getDelta());
            if (inRange(temp_loc,1,1,MAPWID-2,MAPHGT-2))
            {
                disp.setAndPrintFire(getMap(),cursor_loc,NIL_f);
                cursor_loc = temp_loc;
            }
        }

    } while (ibuf.getAction() != INP_TOGGLE);

    if (is_teleporting)
    {
        if (!unWalkableCell(getMap()->getCell(cursor_loc)))
        {
            changeBeingTile(getPlayer()->at(),cursor_loc,PLAYER);
            getPlayer()->moveTo(cursor_loc);
            msgeAdd("You teleport.",colorpair_white_black);
        }
        else
            msgeAdd("You can't teleport there! You waste a teleport usage.",colorpair_gray_black);
    }

    disp.clearAllFireCells(getMap());
}

void game::useMedkit(being *b, int selection)
{
    msgeAdd("You apply the medkit...",colorpair_white_black);
    item_t it = b->getInv()->getItem(selection)->getType();
    if (b->getHP() < b->getStats()->max_hit_points)
    {
        if (it == SMALL_MEDKIT)
        {
            b->incHP(rollrange(1,5));
            msgeAdd("You are healed.",colorpair_white_black);
        }
        if (it == LARGE_MEDKIT)
        {
            b->incHP(rollrange(6,20));
            msgeAdd("You are healed to a large extent.",colorpair_white_black);
        }
        if (it == GIANT_MEDKIT)
        {
            b->incHP(rollrange(21,100));
            msgeAdd("You are significantly healed.",colorpair_white_black);
        }
    }
    else
        msgeAdd("Your health is about the same as before.",colorpair_white_black);
    b->getInv()->removeItem(selection,1);
    printPlayerStats();
}

void game::upgradeAttributes(being *b, int selection)
{
    msgeAdd("You apply the booster...",colorpair_gray_black);
    item_t it = b->getInv()->getItem(selection)->getType();
    if (it == HEALTH_BOOSTER)
    {
        b->setMaxHP(b->getStats()->max_hit_points + ROLL(5) + 1);
        msgeAdd("You are now tougher.",colorpair_white_black);
    }
    if (it == STRENGTH_BOOSTER)
    {
        b->incAttributes(1,0,0);
        msgeAdd("You are now stronger.",colorpair_white_black);
    }
    if (it == DEXTERITY_BOOSTER)
    {
        b->incAttributes(0,1,0);
        msgeAdd("You are now more athletic.",colorpair_white_black);
    }
    if (it == ACCURACY_BOOSTER)
    {
        b->incAttributes(0,0,1);
        msgeAdd("You are now more coordinated.",colorpair_white_black);
    }
    b->getInv()->removeItem(selection,1);
    printPlayerStats();
}

void game::upgradeEquippedWeapon(being *b, int selection)
{
    item_stats istats = *(b->getInv()->getWeapon()->getStats());

    if (istats.i_type != NIL_i)
    {
        if (istats.is_artifact)
        {
            msgeAdd("No known upgrade exists for this weapon.",colorpair_white_black);
            return;
        }

        if ((item_t)((int)istats.i_type + 25) != b->getInv()->getItem(selection)->getType())
        {
            msgeAdd("That's not the right weapon to upgrade!",colorpair_white_black);
            return;
        }

        msgeAdd("Your weapon has been upgraded.",colorpair_white_black);
        switch (b->getInv()->getItem(selection)->getStats()->misc_val)
        {
            case(0):
                if (ROLL(4) > 0)
                    istats.wepstats.dam.num_bonus += rollrange(1,6);
                else if (ROLL(3) > 0)
                    istats.wepstats.dam.num_sides += rollrange(1,4);
                else
                    istats.wepstats.dam.num_dice += rollrange(1,2);
                break;
            case(1):
                istats.wepstats.to_hit += rollrange(1,4);
                break;
            case(2):
                istats.num_subitems.max_quant += item_default_stats[(int)istats.i_type].num_subitems.max_quant;
                break;
        }
        istats.name = getWeaponNameModifier(istats.i_type,istats.wepstats.dam,istats.wepstats.to_hit) +
                      item_default_stats[(int)istats.i_type].name;
        b->getInv()->setWeapon(istats);
        b->getInv()->removeItem(selection,1);
    }
    else
        msgeAdd("You need to equip the weapon you want to upgrade.",colorpair_gray_black);
}

void game::damageBeing(being * b, point p, int dam)
{
    if (b->getType() == PLAYER)
        damagePlayer(b,dam);
    else
        damageNPC(b,p,dam);
}

void game::addHitMissIndicatorMessage(being * attacker, being * target, bool hit_success)
{
    STRING p_string, NPC_string;

    p_string = (hit_success == true) ? "hit" : "miss";

    NPC_string = (hit_success == true) ? "hits" : "misses";

    STRING end_string = ".";

    STRING wep_string;

    if (attacker->getInv()->getWeapon()->getStats()->category == ITEM_MELEE_WEAPON)
    {
        wep_string = attacker->getInv()->getWeapon()->getStats()->name;
        end_string = " with" +  getDeterminer(wep_string) + wep_string + "!";
    }

    if (attacker->getType() != PLAYER && target->getType() != PLAYER)
    if (getMap()->getv(attacker->at()) || getMap()->getv(target->at()))
    {
        msgeAdd(getNameToPrint(attacker,true,true) + " " + NPC_string + " " + getNameToPrint(target,true,false) +
                end_string,colorpair_white_black);
        return;
    }

    if (attacker->getType() == PLAYER)
        msgeAdd("You " + p_string + " " + getNameToPrint(target,true,false) + end_string,colorpair_white_black);

    if (target->getType() == PLAYER)
        msgeAdd(getNameToPrint(attacker,true,true) + " " + NPC_string + " you" + end_string,colorpair_white_black);
}

void game::setupPlayer()
{
    getPlayer()->moveTo(level_obj.getEntrance());
    getPlayer()->setAttackID(-1);
    getPlayer()->setFleeID(-1);
    // put the wondrous '@' on the map
    changeBeingTile(point(1,1),getPlayer()->at(),PLAYER);
}

// add a colored message to the buffer
void game::msgeAdd(STRING s, color_pair c)
{
    bool reached_buffer_limit = false;
    // adding a message returns a boolean that says whether
    // or not the message buffer is full after adding last
    // message
    //if (game_active)
    reached_buffer_limit = mbuf.addMessage(s,c);
    mbuf.printMessages();
    // clear buffer if full
    if (reached_buffer_limit)
    {
        reDisplayAll();
        ibuf.waitForKey(' ');
        mbuf.clearAndDeleteAllMessages();
    }
}

void game::damageMsgeAdd(STRING dam, item_t w, chtype asym, chtype tsym, bool fatal, bool explode, bool see_attacker, bool was_melee, int crit_mult)
{
    if (game_active)
        dbuf.addMessage(dam,w,asym,tsym,fatal,explode,see_attacker,was_melee,crit_mult);
}

// primary program loop
void game::run()
{
    player = being(point(1,1),being_default_stats[(int)PLAYER].max_hit_points,
                              being_default_stats[(int)PLAYER],false,true,0,"player");
    clearScreen();
    clearRange(0,0,GRIDWID-1,GRIDHGT-1);
    updateScreen();
    do
    {
        promptMainMenu();
        if (ibuf.getAction() == INP_QUIT)
            break;
        initGame();
        game_active = true;
        primaryGameLoop();
        deallocateGenericData();
        deallocateGameData();
        clearRange(0,0,GRIDWID-1,GRIDHGT-1);
    }
    while (program_executing);

    cleanup();
}

// deallocate/destroy all pointers, clear vectors, etc...
void game::cleanup()
{
    return;
}

void game::deallocateGenericData()
{
    level_obj.deleteAllPiles();
    if (game_active == false)
        level_obj.destroyAllBeings();
    else
        level_obj.destroyAllNonAllies();
    level_obj.destroyAllDoors();
    level_obj.destroyAllElevators();
    level_obj.destroyAllMachines();
    level_obj.destroyAllSatellites();
    mbuf.clearAndDeleteAllMessages();
    dbuf.clearAndDeleteAllMessages();
    clearRange(SHOWWID+1,0,GRIDWID-1,SHOWHGT);
}

// primary current game loop
void game::primaryGameLoop()
{
    do
    {
        // recalculate the player's field of view
        calculatePlayerLOS();
        // draw everything
        reDisplayAll();
        // wait for player's input
        promptInput();

        num_game_turns++;

        executeMiscPlayerTurnBasedData();
        // have every NPC take their turns
        activateAllNPCAI();
        // stop if player quits current game and goes back to main menu
        executeMiscNPCTurnBasedData();
    }
    while (game_active);

    msgeAdd("You die...",colorpair_red_black);

    mbuf.printMessages();

    reDisplayRightScreenSide();

    clearAllFireCells();

    updateScreen();

    ibuf.waitForKey(' ');
}

void game::checkForCreateVaultAgent()
{
    if (getMap()->getTerrain(getPlayer()->at()) == FLR_VAULTMARKER)
    {
        summoned_agent = true;
        level_obj.rollOneNPC(AGENT_NPC,AGENT_NPC,AGENT_NPC,false);
    }
}

void game::checkPlayerDefeatedEvents()
{
    if (getPlayer()->isDead())
    if (getPlayer()->getHP() > 0)
    {
        getPlayer()->setHP(0);
        printPlayerStats();
        resetBeingAttackAndFleeIDs(0);
        game_active = false;
    }
}

void game::checkNPCDefeatedEvents()
{
    int being_id,exp_worth,defeated_npc_ally_value,num_dead = 0;
    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        being_id = level_obj.getBeing(i)->getID();
        exp_worth = level_obj.getBeing(i)->getStats()->exp_worth;
        defeated_npc_ally_value = level_obj.getBeing(i)->getAllyValue();
        if (level_obj.getBeing(i)->isDead())
        {
            if (getMap()->getv(level_obj.getBeing(i)->at()))
            {
                msgeAdd(getNameToPrint(level_obj.getBeing(i),true,true) + " " + deathMessage(level_obj.getBeing(i)),colorpair_gray_black);
                flashNPCDeathSprite(level_obj.getBeing(i));
            }
            if (level_obj.getBeing(i)->getType() == ARMSDEALER_NPC ||
                level_obj.getBeing(i)->getType() == HOPPER_NPC ||
                level_obj.getBeing(i)->getType() == SIREN_NPC ||
                level_obj.getBeing(i)->getType() == YINAXIMAI_NPC ||
                level_obj.getBeing(i)->getAllyValue() == 2)
                beingDropAllItems(level_obj.getBeing(i),level_obj.getBeing(i)->at());
            level_obj.destroyNPC(level_obj.getBeing(i)->at());
            if (defeated_npc_ally_value == 0)
                checkExperience(getCurrentNPCTurn(),exp_worth);
            resetBeingAttackAndFleeIDs(being_id);
            num_dead++;
            i = 0;
        }
    }

    if (num_dead > 0)
    {
        delayAndUpdate(35);
    }
}

void game::flashNPCDeathSprite(being *b)
{
    chtype ct = {colorpair_white_black, (int)'%'};
    if (b->getType() == PROCGEN_NPC)
        ct.color.fg = b->getFactoryColor();
    else
        ct.color.fg = symbol_NPC[(int)b->getType()].color.fg;
    disp.printCell(getMap(),b->at(),ct);
}

void game::executeMiscNPCTurnBasedData()
{
    checkNPCDefeatedEvents();
    checkPlayerDefeatedEvents();

    if (ROLL(59 - level_number) == 0)
        level_obj.addRandomNPC();

    if (AI_stop_counter > 0)
    {
        AI_stop_counter--;
        if (AI_stop_counter == 0)
            setNPCAIActivation(true,1);
    }

    if (!summoned_agent)
        checkForCreateVaultAgent();

    checkForSpecLevelEvent();
}

void game::executeMiscPlayerTurnBasedData()
{
    if (!player_has_moved)
        return;

    checkNPCDefeatedEvents();
    checkPlayerDefeatedEvents();

    if (PERCCHANCE() <= (uint)getPlayer()->getStats()->healing_ability)
        getPlayer()->incHP(ROLL(getPlayer()->getStats()->healing_ability) + 1);

    printTerrainData();

    disp.printMap(getMap());
    setCustomCellColors();
}

void game::checkForSpecLevelEvent()
{
    point p;
    // using integers in this conditional is redundant.
    // change to "lev_type"
    if (level_obj.getSpecLevVal() == (int)LEV_SPECIALVAULT - 1)
    {
        if (!wayback_unlocked)
        {
            if (isAt(getPlayer()->at(),point(26,10)))
            {
                p.set(19,2);
                level_obj.addBeing(p,being_default_stats[(int)ELITEAGENT_NPC].max_hit_points,being_default_stats[(int)ELITEAGENT_NPC],false,true);
                level_obj.getBeing(p)->setAttackID(0);
                msgeAdd("You hear a distant *DING!*",colorpair_white_black);
                wayback_unlocked = true;
            }
        }
    }
    else if (level_obj.getSpecLevVal() == (int)LEV_LOSTLAIR - 1)
    {
        if (!wayback_unlocked)
        {
            if (level_obj.getNumNPCsOfType(YINAXIMAI_NPC) == 0)
            {
                msgeAdd("You hear a distant *buzz*",colorpair_white_black);
                wayback_unlocked = true;
            }
        }
    }
}

void game::printTerrainData()
{
    if (getMap()->getTerrain(getPlayer()->at()) == PREV_STAIRS ||
        getMap()->getTerrain(getPlayer()->at()) == NEXT_STAIRS)
    {
        int el = level_obj.getElevator(getPlayer()->at())->getLevel();

        STRING express_modifier = "elevator";
        STRING direction_modifier = "down";

        if (el >= 1)
            direction_modifier = "up";

        if (el < -1 || el > 1)
            express_modifier = ("express " + express_modifier);

        msgeAdd("There is an " + express_modifier + " here going " + direction_modifier + " to level " +
                sint2String(level_number + el) + ".",colorpair_white_black);
    }
    if (getMap()->getTerrain(getPlayer()->at()) == SIDEWAYS_ELEVATOR)
    {
        msgeAdd("There is a sideways elevator with a terminal here.",colorpair_white_black);
    }
    if (getMap()->getTerrain(getPlayer()->at()) == BLOCKED_ARCHWAY)
    {
        msgeAdd("There is a strange archway here.",colorpair_white_black);
    }
}

void game::checkExperience(being *b, uint e)
{
    if(b->getAllyValue() <= 0)
       return;

    if(b->getHP() <= 0)
       return;

    if(b->getLevel() == 70)
       return;

    b->incExperience(e);

    uint init_level = b->getLevel();

    int delta = 0;

    uint i = 0;

    for (i = init_level; i < 70; ++i)
    {
        if (b->getExperience() >= exp_levelup_values[i-1])
        {
            b->incLevel(1);
            b->setMaxHP(b->getStats()->max_hit_points + ROLL(b->getLevel()) + 1);
            b->setHP(b->getStats()->max_hit_points);
            b->incHealing();
            switch(ROLL(2))
            {
                case(0):
                    b->incAttributes(1,0,0);
                    break;
                case(1):
                    b->incAttributes(0,1,0);
                    break;
                case(2):
                    b->incAttributes(0,0,1);
                    break;
            }
        }
        else
            break;
    }

    delta = (int)b->getLevel() - (int)init_level;

    if (delta > 0)
    {
        if (b->getType() == PLAYER)
        {
            reDisplayRightScreenSide();
            msgeAdd("You advance to experience level " + sint2String(b->getLevel()) + "!",colorpair_purple_black);
        }
        else if (getMap()->getv(b->at()))
        {
            msgeAdd(actionToMessage(b,"looks more experienced!"),colorpair_purple_black);
        }
    }
}

// 1) clear messages, 2) print new message with space bar prompt 3) update screen,
// 4) wait indefinitely until user presses the space bar
void game::msgeAddPromptSpace(STRING s, color_pair col)
{
    mbuf.clearAndDeleteAllMessages();
    msgeAdd(s,col);
    msgeAdd(" [space]",colorpair_gray_black);
    updateScreen();
    ibuf.waitForKey(' ');
}

void game::chatWithNPC()
{
    NPC_t adj_npc;
    point dp;

    if (AI_stop_counter > 0)
    {
        msgeAdd("You can't hear yourself talk!",colorpair_gray_black);
        return;
    }

    msgeAdd("Select a direction to talk to someone or something...",colorpair_white_black);
    updateScreen();
    ibuf.setAction();

    // if user pressed a direction key...
    if (ibuf.getAction() == INP_DELTA)
    {
        dp = addPoints(getPlayer()->at(),ibuf.getDelta());
        adj_npc = getMap()->getNPC(dp);

        if (adj_npc != NIL_n)
        {
            being *b = level_obj.getBeing(dp);

            if (b->getAttackID() >= 0 || b->getFleeID() >= 0)
                NPCMessage(b,actionToMessage(b,"doesn't want to talk to you now!"),true);
            else
            {
                if (adj_npc != ARMSDEALER_NPC)
                    NPCCommunicate(b,adj_npc,true);
                else
                {
                    if (isAt(b->at(),b->getInitLoc()))
                        buyFromArmsDealer(b);
                    else
                        NPCMessage(b,speechToMessage(b,"Wait 'til I get back..."),true);
                }
            }
        }
        else
            msgeAdd("There isn't anybody there to talk to.",colorpair_gray_black);
    }
    else
        msgeAdd("You decide to not say anything.",colorpair_gray_black);
}

void game::NPCCommunicate(being *b, NPC_t n, bool player_chat)
{
     if (player_chat && b->getAllyValue() >= 2)
     {
         NPCMessage(b,speechToMessage(b,"I'm ready to fight."),false);
     }
     else
     {
         STRING comm = NPC_chat_quotes[(int)n - 2][ROLL(NUM_QUOTES_PER_NPC - 1)];
         if (comm[0] != '#')
             NPCMessage(b,speechToMessage(b,comm),false);
         else
         {
             comm = comm.substr(1,(int)comm.size()-1);
             NPCMessage(b,actionToMessage(b,comm),false);
         }
     }
}

void game::buyFromArmsDealer(being *b)
{
    examineItemList(ITEMLIST_SHOP,b->at());
}

bool game::playerAttemptStunRestrictedEvent()
{
    if (getPlayer()->getStaggerCount() == 0)
        return false;

    switch(ibuf.getAction())
    {
        case(INP_SELECT):
        case(INP_TOGGLE):
        case(INP_PICKUP):
        case(INP_QUICKPICKUP):
        case(INP_CHAT):
        case(INP_RELOAD):
        case(INP_FIRE):
        case(INP_FIREQUICK):
        case(INP_NEXTLEV):
        case(INP_PREVLEV):
             return true;
        default:
            break;
    }

    return false;
}

// ask for player input via ibuf "input" class object
void game::promptInput()
{
    // offset to next location (abs(delta.x()) and abs(delta.y()) is probably always 1)
    point delta = point(0,0);

    // set player input
    ibuf.setAction();

    last_active_id = 0;

    // by default...
    player_has_moved = player_last_attack = true;
    // clear message buffer right after player takes turn
    mbuf.clearAndDeleteAllMessages();

    if (playerAttemptStunRestrictedEvent())
    {
        mbuf.clearAndDeleteAllMessages();
        msgeAdd("You are too stunned to do that right now!",colorpair_white_black);
        updateScreen();
        player_has_moved = false;
        return;
    }

    // get action indicating what action was triggered by player
    switch(ibuf.getAction())
    {
    // if directional key was pressed
    case(INP_DELTA):
        // get delta corresponding to directional key
        delta = ibuf.getDelta();
        // move player to new location
        movePlayer(delta);
        break;
    // if player quits current game
    case(INP_QUIT):
        quitGame();
        break;
    case(INP_SELECT):
        player_has_moved = false;
    case(INP_TOGGLE):
        checkForLevelFeature();
        break;
    case(INP_QUICKPICKUP):
        checkPlayerQuickPickup();
        break;
    case(INP_PICKUP):
        examineItemList(ITEMLIST_PILE, getPlayer()->at());
        break;
    case(INP_INV):
        examineItemList(ITEMLIST_INV, getPlayer()->at());
        break;
    case(INP_CHAT):
        chatWithNPC();
        break;
    case(INP_RELOAD):
        playerReloadFirearm();
        break;
    case(INP_FIRE):
        playerFireWeaponToggle(false);
        break;
    case(INP_NEXTLEV):
        gotoNextLevel();
        break;
    case(INP_PREVLEV):
        gotoPrevLevel();
        break;
    case(INP_DEBUG):
        enterDebugMode();
        break;
    case(INP_FIREQUICK):
        playerFireWeaponToggle(true);
        break;
    case(INP_HELP):
        gotoHelpScreen();
        break;
    default:
        player_has_moved = false;
        break;
    }

    player_last_attack = false;
}

void game::gotoHelpScreen()
{
    clearRange(0,0,GRIDWID-1,GRIDHGT-1);
    displayHelpScreen();
    updateScreen();
    ibuf.waitForKey(' ');
    clearRange(0,0,GRIDWID-1,GRIDHGT-1);
    reDisplayAll();
    player_has_moved = false;
}

void game::quitGame()
{
    player_has_moved = false;
    msgeAdd("Really quit? ('y' to confirm)", colorpair_purple_black);
    updateScreen();
    if (ibuf.checkForKey('y'))
    {
        game_active = false;
    }
    else
    {
        msgeAddPromptSpace("Not quitting...",colorpair_white_black);
        mbuf.clearAndDeleteAllMessages();
        updateScreen();
    }
}

// Used to make character extremely powerful for testing purposes
void game::enterDebugMode()
{
    player_has_moved = false;
    if (!debug_mode)
    {
        msgeAdd("Really enter debug mode? (y -> confirm)", colorpair_gray_black);
        updateScreen();
        if (ibuf.checkForKey('y'))
        {
            debug_mode = true;

            // Give player unstoppable armor
            getPlayer()->getInv()->addToContainer(item_default_stats[(int)DEBUG_ARMOR],1);

            for (int i = (int)METAL_ROD; i <= (int)SWORD_GIANT; ++i)
                getPlayer()->getInv()->addToContainer(item_default_stats[i],1);

            for (int i = (int)PISTOL_10MM; i <= (int)PULSE_CANNON; ++i)
                getPlayer()->getInv()->addToContainer(item_default_stats[i],1);

            for (int i = (int)HANDGUN_AMMO_GEN; i <= (int)PLASMA_AMMO; ++i)
                getPlayer()->getInv()->addToContainer(item_default_stats[i],100);

            for (int i = 0; i < 40; ++i)
                getPlayer()->getInv()->addToContainer(getModifiedUpgradeCharacteristics(DOUBLEBARRELLED_SHOTGUN_UPGRADE),1);

            for (int i = (int)HEALTH_BOOSTER; i <= (int)GIANT_MEDKIT; ++i)
                getPlayer()->getInv()->addToContainer(item_default_stats[i],3);

            for (int i = 0; i < 5; ++i)
                getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(DRILL_DEVICE,ROLL(10)+1),1);

            for (int i = 0; i < 5; ++i)
                getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(ITEMDETECTOR_DEVICE,ROLL(10)+1),1);

            for (int i = 0; i < 5; ++i)
                getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(MAPPING_DEVICE,ROLL(10)+1),1);

            for (int i = 0; i < 5; ++i)
                getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(NPCDETECTOR_DEVICE,ROLL(10)+1),1);

            for (int i = 0; i < 5; ++i)
                getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(TELEPORT_DEVICE,ROLL(10)+1),1);

            for (int i = 0; i < 5; ++i)
                getPlayer()->getInv()->addToContainer(getModifiedDeviceCharacteristics(TIMESTOPPER_DEVICE,ROLL(10)+1),1);

            getPlayer()->setMaxHP(9999);
            getPlayer()->setHP(9999);
            getPlayer()->incAttributes(70,70,70);
            getPlayer()->setCredits(getPlayer()->getCredits() + 2000000000ULL);
        }
    }
}

void game::NPCGetRandomTargetDestination(being *b)
{
    level_obj.NPCGetRandomTargetDestinationCallee(b);
}

void game::checkNPCHeal(being *b)
{
    if (b->getHP() < b->getStats()->max_hit_points)
        if (PERCCHANCE() <= (uint)b->getStats()->healing_ability)
        {
            b->incHP(ROLL(b->getStats()->healing_ability) + 1);
            NPCMessage(b,actionToMessage(b,"heals."),false);
        }
}

void game::checkNPCNeutrality(being *b)
{
    if (b->getAttackID() >= 0)
        if (PERCCHANCE() <= (uint)b->getStats()->forgive_chance)
        {
            if (b->getAttackID() == 0)
                NPCMessage(b,actionToMessage(b,"no longer wants to attack you."),false);
            else
                NPCMessage(b,actionToMessage(b,"no longer wants to attack " +
                                             getNameToPrint(level_obj.getBeingFromID(b->getAttackID()),true,false) + "."),false);
            b->setAttackID(-1);
        }

    point last_ally_loc = getPlayer()->at();

    // have allies follow player or an NPC ally
    if (b->getAllyValue() > 0)
    if (b->getAttackID() == -1)
    {
        b->setAttackID(getPlayer()->getAttackID());
        if (b->getAttackID() == -1)
        {
            if (!isAt(pfinder.dijkstra(getMap(),b->at(),last_ally_loc),b->at()))
            {
                b->setPathfindDest(last_ally_loc);
                return;
            }
            for (int i = 0; i < level_obj.getNumNPCs(); ++i)
            {
                if (level_obj.getBeing(i)->getAllyValue() == 2 && level_obj.getBeing(i)->getID() != b->getID())
                {
                    last_ally_loc = level_obj.getBeing(i)->at();
                    if (!isAt(pfinder.dijkstra(getMap(),b->at(),last_ally_loc),b->at()))
                    {
                        b->setPathfindDest(last_ally_loc);
                        return;
                    }
                }
            }
            b->setPathfindDest(getPlayer()->at());
        }
    }
}

int game::getPotentialFleeID(int id)
{
    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        if (level_obj.getBeing(i)->getAttackID() == id)
            return level_obj.getBeing(i)->getID();
    }

    if (getPlayer()->getAttackID() == id)
        return 0;

    return -1;
}

void game::checkNPCElusion(being *b)
{
    if(!calculateFleeChance(b))
    {
        if (b->getFleeID() >= 0)
        {
            if (ROLL(4) == 0)
                NPCMessage(b,actionToMessage(b,"is no longer fleeing."),false);
            b->setFleeID(-1);
        }
        return;
    }

    int flee_from = getPotentialFleeID(b->getID());

    point flee_target;

    if (flee_from == 0)
        flee_target = getPlayer()->at();
    else if (flee_from > 0)
        flee_target = level_obj.getBeingFromID(flee_from)->at();
    else
    {
        b->setFleeID(-1);
        NPCGetRandomTargetDestination(b);
        return;
    }

    if (b->getFleeID() == -1)
    {
        b->setPathfindDest(getFleeLoc(flee_target));
        NPCMessage(b,actionToMessage(b,"turns to flee!"),false);
        b->setFleeID(flee_from);
    }
    else if (ROLL(9) == 0)
        b->setPathfindDest(getFleeLoc(flee_target));
}

void game::moveNPC(being *b, point move_to)
{
    // if we get to this point and the being wants to move to a tile that can
    // be stepped on, move the npc
    if (!unWalkableCell(getMap()->getCell(move_to)))
    {
        changeBeingTile(b->at(),move_to,b->getType());
        b->moveTo(move_to);
    }

    if (getMap()->getTerrain(move_to) == CLOSED_UNLOCKED_DOOR)
        NPCOpenDoor(b,move_to);
}

// NPC opens a door (simple as changing terrain value of cell tile at "loc")
void game::NPCOpenDoor(being * b, point loc)
{
    getMap()->setTerrain(loc,OPEN_UNLOCKED_DOOR);
    level_obj.getDoor(loc)->setState(-1);
    if (getMap()->getv(loc))
        msgeAdd(actionToMessage(b,"opens the door."),colorpair_white_black);
    else
    {
        getMap()->setu(loc,symbol_terrain[(int)CLOSED_UNLOCKED_DOOR]);
        msgeAdd("You hear a door opening...",colorpair_white_black);
    }
}

bool game::checkNPCFightEvent(being *b)
{
    if (b->getFleeID() >= 0 || b->getAttackID() == -1)
        return false;

    being *a;

    if (b->getAttackID() == 0)
        a = getPlayer();
    else
        a = level_obj.getBeingFromID(b->getAttackID());

    if(checkNPCFireWeaponEvent(b,a->at()))
        return true;
    if(pointDist(b->at(),a->at()) == 1)
    {
        if (!checkNPCMiscAttackEvent(b,a))
        if (!checkNPCStealEvent(b,a))
        if (b->getStats()->can_melee)
            melee(b,a);
        return true;
    }

    setSpecialNPCAttackMovement(b,a);

    return false;
}

void game::setSpecialNPCAttackMovement(being *attacker, being *target)
{
    switch(attacker->getType())
    {
    case(ROGUE_NPC):
        if (pointDist(attacker->at(),target->at()) <= 5)
            attacker->setPathfindDest(getFleeLoc(target->at()));
        return;
    default:
        break;
    }

    attacker->setPathfindDest(target->at());
}

bool game::checkNPCFireWeaponEvent(being *b, point dest)
{
    item_t weapon = b->getInv()->getWeapon()->getType();

    if (b->getInv()->getWeapon()->getStats()->category != ITEM_FIREARM_WEAPON)
        return false;

    if (PERCCHANCE() > (uint)b->getStats()->shoot_freq)
        return false;
    // potential crash bug with npc using fusion cannon
    if (pointDist(b->at(),dest) > firearm_ranges[(int)weapon - WEAPON_OFFSET - NUM_TOTAL_MELEE_WEAPONS] &&
        firearm_ranges[(int)weapon - WEAPON_OFFSET - NUM_TOTAL_MELEE_WEAPONS] > -1)
        return false;

    if (linetracer.isBlocking(getMap(),b->at(),dest,true,false,true))
        return false;

    uint current_load = b->getInv()->getWeapon()->getStats()->num_subitems.quant;

    if (current_load == 0)
    {
        if (!beingReloadFirearm(b))
            return false;
    }
    else
        beingFireWeapon(b,dest);

    return true;
}

bool game::checkNPCExplodeAttackEvent(being *b)
{
    if (PERCCHANCE() <= (uint)b->getStats()->explode_chance)
    {
        if (b->getAttackID() == -1 && b->getFleeID() == -1)
            return false;
        dice_roll dam = {(uint)ROLL(2) + 1,(uint)b->getStats()->danger_level,(uint)b->getStats()->danger_level*3};
        createExplosion(b,dam,b->at(),true);
        NPCMessage(b,actionToMessage(b,"gives off a wave of explosive energy!"),true);
        return true;
    }
    return false;
}

bool game::checkNPCMiscAttackEvent(being *attacker, being *target)
{
    STRING attack_name = getNameToPrint(attacker,true,true);
    STRING target_name = (target->getType() == PLAYER) ? "you" : getNameToPrint(target,true,false);
    switch(attacker->getType())
    {
        case(SIREN_NPC):
            if (ROLL(6) == 0)
            {
                if (getMap()->getv(attacker->at()) || getMap()->getv(target->at()))
                    msgeAdd(attack_name + " sings a song to " + target_name + "!",colorpair_purple_black);
                if (target->getStats()->is_stun_immune)
                {
                    if (getMap()->getv(attacker->at()) || getMap()->getv(target->at()))
                        msgeAdd(target_name + " couldn't care less about " + attack_name + "'s stupid voice",colorpair_white_black);
                    return true;
                }
                target->setStaggerCount(target->getStaggerCount()+5);
                if (getMap()->getv(attacker->at()) || getMap()->getv(target->at()))
                if (attacker->getType() == SIREN_NPC)
                {
                    if (target->getType() == PLAYER)
                        msgeAdd("You are mesmerized by " + getNameToPrint(attacker,true,false) + "'s voice!",colorpair_white_black);
                    else
                        msgeAdd(getNameToPrint(target,true,true) + " is mesmerized by " + getNameToPrint(attacker,true,false) + "'s voice!",colorpair_white_black);
                }
                return true;
            }
            break;
        default:
            break;
    }
    return false;
}

bool game::checkNPCStealEvent(being *attacker, being *target)
{
    if (PERCCHANCE() > (uint)attacker->getStats()->steal_ability)
        return false;

    int num_items_to_be_stolen = (int)(attacker->getStats()->steal_ability/15) + 1;

    int max_items_to_be_stolen = target->getInv()->getNumItems();

    int item_stolen;

    int sub_quantity = 1;

    item_stats istats;

    const int num_items_stolen = MINIMUM((int)ROLL(num_items_to_be_stolen),max_items_to_be_stolen);

    STRING attack_name = getNameToPrint(attacker,true,true);

    STRING target_name = (target->getType() == PLAYER) ? "you" : getNameToPrint(target,true,false);

    if (num_items_stolen == 0)
    {
        if (getMap()->getv(attacker->at()) || getMap()->getv(target->at()))
            msgeAdd(attack_name + " attempts to steal from " + target_name + ", but comes up empty.",colorpair_white_black);
        return false;
    }
    else
    {
        if (getMap()->getv(attacker->at()) || getMap()->getv(target->at()))
            msgeAdd(attack_name + " steals from " + target_name + "!",colorpair_white_black);

        for (int i = 0; i < num_items_stolen; ++i)
        {
            item_stolen = ROLL(target->getInv()->getNumItems() - 1);

            istats = *(target->getInv()->getItem(item_stolen)->getStats());

            sub_quantity = rollrange(1,target->getInv()->getItem(item_stolen)->getQuant());

            attacker->getInv()->addToContainer(istats,sub_quantity);

            target->getInv()->removeItem(item_stolen,sub_quantity);
        }
    }
    return true;
}

void game::checkNPCMiscEvent(being *b)
{
    // NPC talks randomly
    if (TENTHPERCCHANCE() <= (uint)(b->getStats()->talkativeness))
    {
        if (b->getAllyValue() <= 0)
            NPCCommunicate(b,b->getType(),false);
    }
    if (b->getFleeID() == -1)
    {
        // flee away to a random location
        if (ROLL(15) == 0)
            NPCGetRandomTargetDestination(b);
    }
    else
    {
        // NPC shouts for help
        if (TENTHPERCCHANCE() <= (uint)(b->getStats()->talkativeness))
        if (b->getAllyValue() <= 0)
            NPCShout(b);
    }

    // depending on what level player is on,
    // NPCs become more and more hostile out of the blue
    if (!isSpecialNonProcgen(b->getType()) && b->getAllyValue() <= 0)
    {
        if (ROLL(499 - level_number*10) == 0)
        {
            if (ROLL(2) == 0)
                b->setAttackID(0);
            else
                level_obj.setAttackIDToRandomAlly(b);
        }
    }
}

bool game::beingUndetectedChance(being *b, bool fire_weapon)
{
    if (fire_weapon && b->getInv()->getWeapon()->getStats()->wepstats.is_stealth)
        return true;

    return (PERCCHANCE() <= (uint)b->getStats()->stealth);
}

void game::alertNPC(being *b, int alert_id)
{
    if (isSpecialNonProcgen(b->getType()) || b->getAllyValue() > 0)
        return;

    if (alert_id > 0)
        if (NPCsCannotFight(b->getType(),level_obj.getBeingFromID(alert_id)->getType()))
            return;

    b->setAttackID(alert_id);
}

void game::checkForAlertingNPCs(point hostile_loc, point alert_loc)
{
    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        if(!isAt(level_obj.getBeing(i)->at(),alert_loc))
            if(!isAt(level_obj.getBeing(i)->at(),hostile_loc))
                if(pointDist(alert_loc,level_obj.getBeing(i)->at()) <= level_obj.getBeing(i)->getStats()->detect_distance)
                {
                    if (!isAt(getPlayer()->at(),hostile_loc))
                        alertNPC(level_obj.getBeing(i),level_obj.getBeing(hostile_loc)->getID());
                    else
                        alertNPC(level_obj.getBeing(i),0);
                }
    }
}

void game::NPCShout(being *b)
{
    int d = b->getStats()->detect_distance;

    NPCMessage(b,actionToMessage(b,"shouts for help!"),true);

    point p;

    for (int x = b->at().x() - d; x <= b->at().x() + d; ++x)
        for (int y = b->at().y() - d; y <= b->at().y() + d; ++y)
        {
            p.set(x,y);
            if (inRange(p,1,1,MAPWID-2,MAPHGT-2))
                if (!isAt(b->at(),p))
                    if ((int)getMap()->getNPC(p) > PLAYER)
                        if (level_obj.getBeing(p)->getID() != b->getFleeID())
                        {
                            alertNPC(level_obj.getBeing(p),b->getFleeID());
                        }
        }
}

void game::confrontAgent(being *b)
{
    uint64 fee = (uint64)(ROLL(100000)*level_number*level_number*level_number + 25000);

    msgeAddPromptSpace("\"The fee for entering the vault is " + int2String(fee) + " credits.\"",colorpair_white_black);

    if (fee > getPlayer()->getCredits())
    {
        msgeAdd("\"You don't have the credits? That's too bad.\"",colorpair_white_black);
        b->setAttackID(0);
    }
    else
    {
        msgeAdd("Pay the credits? (y -> confirm)",colorpair_purple_black);
        updateScreen();
        if (ibuf.checkForKey('y'))
        {
            getPlayer()->setCredits(getPlayer()->getCredits() - fee);
            msgeAdd("You pay the credits.",colorpair_gray_black);
            msgeAdd("The agent teleports away...",colorpair_gray_black);
            resetBeingAttackAndFleeIDs(b->getID());
            level_obj.destroyNPC(b->at());
        }
        else
        {
            msgeAdd("\"You will *sorely* regret that decision.\"",colorpair_white_black);
            b->setAttackID(0);
        }
    }
}

bool game::checkNPCSpecialEvent(being *b)
{
    if (b->getType() == ARMSDEALER_NPC)
        if (b->getAttackID() == -1 && b->getFleeID() == -1)
        {
            if (!isAt(b->at(),b->getInitLoc()))
                b->setPathfindDest(b->getInitLoc());
            else
                b->setMobility(false);
        }

    if (b->getType() == AGENT_NPC)
        if (b->getAttackID() == -1 && b->getFleeID() == -1)
        {
            if (pointDist(getPlayer()->at(),b->at()) > 1)
            {
                b->setPathfindDest(point(getPlayer()->at().x()+ROLL(2) - 1,getPlayer()->at().y()+ROLL(2) - 1));
            }
            else if (getMap()->getv(b->at()))
            {
                confrontAgent(b);
                return true;
            }
        }

    return false;
}

bool game::checkNPCSummonEvent(being *b)
{
    // for now, allies cannot summon other allies
    if (b->getAllyValue() == 2)
        return false;

    if (b->getStats()->can_summon == false)
        return false;

    if (ROLL(449 - b->getStats()->danger_level) > 0)
        return false;

    point summon_at;

    //NPC_t summoned_NPC;

    int num_valid_cells = 0;

    for (int x = b->at().x() - 1; x <= b->at().x() + 1; ++x)
        for (int y = b->at().y() - 1; y <= b->at().y() + 1; ++y)
            if (inRange(point(x,y),2,2,MAPWID-3,MAPHGT-3))
                if (!unWalkableCell(getMap()->getCell(point(x,y))))
                    num_valid_cells++;

    // extremely rare case where all valid cells are blocked
    if (num_valid_cells == 0)
    {
        msgeAdd(actionToMessage(b,"tried to summon, but couldn't!"),colorpair_white_black);
        return false;
    }

    summon_at = level_obj.getRandWalkablePoint(b->at().x() - 1, b->at().y() - 1, b->at().x() + 1, b->at().y() + 1);

    msgeAdd(actionToMessage(b,"opens up a portal to another dimension!"),colorpair_purple_black);

    //summoned_NPC = (NPC_t)rollrange((int)GUARD_NPC,(int)DESTROYER_NPC);
    //level_obj.addBeing(summon_at,being_default_stats[(int)summoned_NPC].max_hit_points,being_default_stats[(int)summoned_NPC],true,true);
    level_obj.procgen_NPC_factory(summon_at,0);

    // protects rare instance where NPC blows up another one when summoned in this extreme case
    if (level_obj.getNumNPCs() > 0)
    {
        if (getMap()->getv(summon_at))
            msgeAdd(actionToMessage(level_obj.getBeing(level_obj.getNumNPCs() - 1),"appears!"),colorpair_white_black);
    }

    return true;
}

bool game::checkNPCTeleportEvent(being *b)
{
    if (level_obj.getSpecLevVal() != -1)
        return false;

    if (PERCCHANCE() <= (uint)b->getStats()->teleport_chance)
    {
        point new_loc = level_obj.getRandWalkablePoint(1,1,MAPWID-2,MAPHGT-2);
        if (getMap()->getv(b->at()))
            msgeAdd(getNameToPrint(b,true,true) + " teleports away!",colorpair_purple_black);
        changeBeingTile(b->at(),new_loc,b->getType());
        b->moveTo(new_loc);
        if (getMap()->getv(new_loc))
            msgeAdd(getNameToPrint(b,true,true) + " appears!",colorpair_white_black);
        return true;
    }
    return false;
}

bool game::checkNPCStaggerEvent(being *b)
{
    if (b->getStaggerCount() > 0)
    {
        b->decStaggerCount();
        if (b->getStaggerCount() == 0)
        {
            NPCMessage(b,actionToMessage(b,"recovers."),false);
            return true;
        }
        else
            NPCMessage(b,actionToMessage(b,"staggers..."),false);
        moveNPC(b,point(b->at().x()+ROLL(2)-1,b->at().y()+ROLL(2)-1));
        return true;
    }
    return false;
}

void game::assertNPCFieldsValidity(being *b)
{
    if (b->getID() == b->getAttackID())
    {
        displayNPCErrorScreen(b,"NPC IS TRYING TO ATTACK ITSELF","ATTACK ID SET TO -1");
        b->setAttackID(-1);
    }

    if (b->getID() == b->getFleeID())
    {
        displayNPCErrorScreen(b,"NPC IS TRYING TO RUN AWAY FROM ITSELF","FLEE ID SET TO -1");
        b->setFleeID(-1);
    }
}

void game::displayNPCErrorScreen(being *b, STRING msg1, STRING msg2)
{
    clearRange(0,0,GRIDWID-1,GRIDHGT-1);
    addGFXString(colorpair_white_black,"ATTENTION -- The following NPC-related fault has occurred:",0,0);
    addGFXString(colorpair_red_black,msg1,0,3);
    addGFXString(colorpair_white_black,"Culprit: \"" + b->getName() + "\"",0,5);
    addGFXString(colorpair_gray_black,"NPC data:",0,6);
    addGFXString(colorpair_gray_black,"NPC id         -> " + sint2String(b->getID()),0,7);
    addGFXString(colorpair_gray_black,"NPC attack id  -> " + sint2String(b->getAttackID()),0,8);
    addGFXString(colorpair_gray_black,"NPC flee id    -> " + sint2String(b->getFleeID()),0,9);
    addGFXString(colorpair_gray_black,"NPC location           -> x=" + sint2String(b->at().x()) + ", y=" + sint2String(b->at().y()),0,10);
    addGFXString(colorpair_gray_black,"NPC initial location   -> x=" + sint2String(b->getInitLoc().x()) + ", y=" + sint2String(b->getInitLoc().y()),0,11);
    addGFXString(colorpair_gray_black,"NPC target destination -> x=" + sint2String(b->getPathfindDest().x()) + ", y=" + sint2String(b->getPathfindDest().y()),0,12);

    addGFXString(colorpair_white_black,"The following action is being taken in response:",0,14);
    addGFXString(colorpair_purple_black,msg2,0,15);
    addGFXString(colorpair_white_black,"If possible, please take a screenshot of (using ALT-PRTSC or whatever):",0,16);
    addGFXString(colorpair_gray_black,"1) The game window (as is with this text)",0,17);
    addGFXString(colorpair_gray_black,"2) The game window immediately upon leaving this text",0,18);
    addGFXString(colorpair_gray_black,"Post as an issue on the github repo page",0,20);
    addGFXString(colorpair_gray_black,"To attempt to RETURN (*not* recover) to the current game, press [SPACE]...",0,22);
    addGFXString(colorpair_gray_black,"Thanks,",0,24);
    addGFXString(colorpair_gray_black,"Eric",0,26);

    updateScreen();
    ibuf.waitForKey(' ');
    clearRange(0,0,GRIDWID-1,GRIDHGT-1);
    reDisplayAll();
}

// primary function/method for AI of NPC
void game::activateOneNPC(int n)
{
    being *b = level_obj.getBeing(n);

    assertNPCFieldsValidity(b);

    // will NPC heal a little bit?
    checkNPCHeal(b);

    if (checkNPCStaggerEvent(b))
        return;

    // will NPC become neutral (from hostile)?
    checkNPCNeutrality(b);

    // will NPC decide to flee or not?
    checkNPCElusion(b);

    if(checkNPCSummonEvent(b))
        return;

    if(checkNPCExplodeAttackEvent(b))
        return;

    if(checkNPCTeleportEvent(b))
        return;

    if(checkNPCSpecialEvent(b))
        return;

    if(checkNPCCircularDeathRayEvent(b))
        return;

    // check various NPC fight events that may prohibit moving on same turn
    if(checkNPCFightEvent(b))
        return;

    // check to see if NPC will equip something
    if(checkNPCEquipEvent(b))
        return;

    // if NPC is standing on an item and can pick it up, have him/her pick it up
    if(checkNPCPickupEvent(b))
        return;

    // only the boss can do these 2 things
    if(checkNPCBulldozeEvent(b))
        return;

    if(checkNPCWallinEvent(b))
        return;

    // NPC might rove to a new location
    checkNPCMiscEvent(b);

    if(!b->isMobile())
        return;

    if (pointDist(b->at(),b->getPathfindDest()) <= b->getStats()->detect_distance ||
        b->getFleeID() > -1)
    // if we get to this point, we move to the NPC's new location
        moveNPC(b,pfinder.dijkstra(getMap(),b->at(),b->getPathfindDest()));
    else if (PERCCHANCE() <= (uint)b->getStats()->rove_chance)
        moveNPC(b,point(b->at().x()+ROLL(2)-1,b->at().y()+ROLL(2)-1));
}

bool game::checkNPCWallinEvent(being *b)
{
    point loc;

    if (PERCCHANCE() <= (uint)b->getStats()->wallin_chance)
    {
        if (b->getAttackID() <= -1)
            return false;

        if (b->getAttackID() == 0)
            loc = getPlayer()->at();
        else
            loc = level_obj.getBeingFromID(b->getAttackID())->at();

        for (int x = loc.x() - 2; x <= loc.x() + 2; ++x)
        for (int y = loc.y() - 2; y <= loc.y() + 2; ++y)
        {
             if (!isAt(point(x,y),loc))
             {
                 if (inRange(point(x,y),2,2,MAPWID-3,MAPHGT-3))
                 if (getMap()->getTerrain(point(x,y)) != PREV_STAIRS)
                 if (getMap()->getNPC(point(x,y)) == NIL_n)
                 {
                     if (!getMap()->getv(point(x,y)))
                          getMap()->setu(point(x,y),disp.getSymbol(getMap(),point(x,y)));
                     getMap()->setTerrain(point(x,y),WALL_INDESTRUCT);
                 }
             }
        }

        if (getMap()->getv(b->at()))
            msgeAdd(actionToMessage(b,"creates an entrapment!"),colorpair_purple_black);
        msgeAdd("The ceiling comes crushing down partially!",colorpair_purple_black);
        if (b->getAttackID() == 0)
            msgeAdd("You are trapped!",colorpair_gray_black);
        else
            msgeAdd("You sense a cave-in!",colorpair_purple_black);
        return true;
    }
    return false;
}

bool game::checkNPCCircularDeathRayEvent(being *b)
{
    if (b->getAttackID() == -1)
        return false;

    if (PERCCHANCE() <= (uint)b->getStats()->circulardeathray_chance)
    {
        point loc = b->at();

        if (getMap()->getv(loc))
            msgeAdd(actionToMessage(b,"creates an annihilating spark!"),colorpair_purple_black);
        else
            msgeAdd("You hear a giant *bzzzzzzaaaaaaaaap!*",colorpair_purple_black);

        point offset_p;

        int num_sprites = 0;

        for (int iter = 0; iter < 2; ++iter)
        {
            for (int x = loc.x()-1; x <= loc.x()+1; ++x)
            for (int y = loc.y()-1; y <= loc.y()+1; ++y)
            {
                offset_p.set(x,y);
                if (!isAt(loc,offset_p))
                {
                    if(iter == 0)
                    {
                        if(getMap()->getv(offset_p))
                        {
                            disp.setAndPrintFire(getMap(),offset_p,selectFireCell(offset_p,loc,DEATHRAY_GUN));
                            num_sprites++;
                        }
                    }
                    else
                    {
                        if (isCellBlocking(getMap()->getNPC(offset_p)))
                            damageBeingFromExplosion(b,getBeingAt(offset_p),offset_p,ROLL(270)+100,false,true);
                    }
                }
            }
        }

        if (num_sprites > 0)
        {
            updateScreen();
            SDL_Delay(150);
            clearAllFireCells();
        }

        return true;
    }
    return false;
}

bool game::checkNPCBulldozeEvent(being *b)
{
    if (PERCCHANCE() <= (uint)b->getStats()->bulldoze_chance)
    {
        point loc = point(ROLL(2)-1+b->at().x(),ROLL(2)-1+b->at().y());
        if (getMap()->getTerrain(loc) == WALL_INDESTRUCT && inRange(loc,2,2,MAPWID-3,MAPHGT-3 ))
        {
            getMap()->setTerrain(loc,FLR_GEN);
            if (getMap()->getv(loc))
                msgeAdd(actionToMessage(b,"crushes rock to pieces!"),colorpair_purple_black);
            else
            {
                getMap()->setu(loc,symbol_terrain[(int)WALL_INDESTRUCT]);
                msgeAdd("You hear a giant crushing sound!",colorpair_purple_black);
            }
            return true;
        }
    }

    return false;
}

void game::setNPCAIActivation(bool act, int amount)
{
    int num_unaffected = 0;

    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
      if (!level_obj.getBeing(i)->getStats()->is_deactivate_immune)
          level_obj.getBeing(i)->setActivation(act);
      else
          num_unaffected++;
    }

    if (!act)
    {
        if (AI_stop_counter == 0)
            msgeAdd("Everything around you freezes up!",colorpair_white_black);
        if (num_unaffected > 0)
            msgeAdd("The device indicates that one or more beings are unaffected.",colorpair_white_black);
        msgeAdd("Your ears start ringing.",colorpair_white_black);
        AI_stop_counter += amount;
    }
    else
        msgeAdd("Suddenly, everything around you starts moving again.",colorpair_gray_black);
}

// NPCs' turn. Have every enemy take
// a turn (fight,pursue player,pursue misc coordinate (gambler usually),flee,etc...)
void game::activateAllNPCAI()
{
    if (!game_active)
        return;

    if (!player_has_moved || getPlayer()->isDead())
        return;

    gbti = 0;

    float npc_speed, npc_timer;

    float reference = (float)getPlayer()->getStats()->speed;

    int initial_num_NPCs = level_obj.getNumNPCs();

    while (gbti < level_obj.getNumNPCs())
    {
        // if this being's AI is activated (no time stopping mechanism used,
        // or various other exceptions do not happen)
        if (level_obj.getBeing(gbti)->isActivated() && !level_obj.getBeing(gbti)->isDead() && !getPlayer()->isDead())
        {
            last_active_id = level_obj.getBeing(gbti)->getID();

            npc_speed = (float)level_obj.getBeing(gbti)->getStats()->speed;

            npc_timer = (float)level_obj.getBeing(gbti)->getTurnTimer();

            level_obj.getBeing(gbti)->setTurnTimer(npc_timer + (npc_speed / reference));

            if (level_obj.getBeing(gbti)->inTimerRange())
                level_obj.getBeing(gbti)->setTurnTimer(0.0);

            // ActivateOneNPC gets called a certain number of times
            // for a particular NPC -> based on how fast it moves
            // relative to player
            while(level_obj.getBeing(gbti)->getTurnTimer() > 0.0)
            {
                level_obj.getBeing(gbti)->decrementTurnTimer();
                activateOneNPC(gbti);
                if (initial_num_NPCs != level_obj.getNumNPCs())
                    return;
            }
        }
        // increment global being turn iterator
        gbti++;
    }
}

void game::printPlayerStats()
{
    addGFXString(colorpair_white_black,"                                        ",0,SHOWHGT);
    addGFXString(colorpair_white_black,"C:" + int2String((uint64)getPlayer()->getCredits()),0,SHOWHGT);
    addGFXString(colorpair_white_black,"HIT:" + sint2String(getPlayer()->getHP()) + "/" + sint2String(getPlayer()->getStats()->max_hit_points),14,SHOWHGT);
    addGFXString(colorpair_white_black," S:" + int2String((uint64)getPlayer()->getStats()->strength) +
                 " D:" + int2String((uint64)getPlayer()->getStats()->dexterity) +
                 " A:" + int2String((uint64)getPlayer()->getStats()->accuracy),29,SHOWHGT);
    addGFXString(colorpair_white_black,"E:" + sint2String(getPlayer()->getLevel()),45,SHOWHGT);
    addGFXString(colorpair_white_black,"PACK:" + sint2String(getPlayer()->getInv()->getTotalWeight()) +
                 "/" + sint2String(getPlayer()->getMaxCarry()) + "    ",50,SHOWHGT);
    if (level_obj.getSpecLevVal() == -1)
        addGFXString(colorpair_white_black,"L:" + sint2String(level_number) + " ",67,SHOWHGT);
    else
        addGFXString(colorpair_white_black,"L:?? ",67,SHOWHGT);
    addGFXString(colorpair_white_black,"T:" + sint2String(num_game_turns),72,SHOWHGT);
}

// draw everything on the SDL window except for current level feature
void game::reDisplayAll()
{
    if (ibuf.getAction() != INP_REPEAT)
    if (player_has_moved)
    {
        // print map
        disp.printMap(level_obj.getMap());
        setCustomCellColors();
        printPlayerStats();
        // print damage history (needs to be refactored into its own function
        reDisplayRightScreenSide();
        updateScreen();
    }
}

void game::reDisplayRightScreenSide()
{
    if (getMap()->getTerrain(getPlayer()->at()) == SLOT_MACHINE ||
        getMap()->getTerrain(getPlayer()->at()) == DIAMONDS_MACHINE)
        showLevelFeature(getPlayer()->at());
    else if (getMap()->getTerrain(getPlayer()->at()) == SATELLITE_TILE)
        level_obj.getSatellite(getPlayer()->at())->drawSatelliteGraphic();
    else
        dbuf.printMessages();

    for (int i = getPlayer()->at().x() - 1; i <= getPlayer()->at().x() + 1; ++i)
    for (int j = getPlayer()->at().y() - 1; j <= getPlayer()->at().y() + 1; ++j)
    {
        if (getMap()->getTerrain(point(i,j)) == CLOSED_UNLOCKED_DOOR ||
            getMap()->getTerrain(point(i,j)) == OPEN_UNLOCKED_DOOR)
            level_obj.getDoor(point(i,j))->displayDoor();
    }
}

void game::setCustomCellColors()
{
    chtype ct;

    point bp;
    point sp;
    point ip;
    point ep;

    NPC_t nt;

    int el;

    //int ai;

    uint num_machines;

    for (int i = 0; i < level_obj.getNumElevators(); ++i)
    {
        ep = level_obj.getElevator(i)->getLoc();
        el = level_obj.getElevator(i)->getLevel();

        if (el > 1 || el < -1)
        if (getMap()->getv(ep))
        if (getMap()->getFire(ep) == NIL_f &&
            getMap()->getItem(ep) == NIL_i &&
            getMap()->getNPC(ep) == NIL_n)
        {
            ct = symbol_terrain[(int)getMap()->getTerrain(ep)];
            ct.color.fg = color_purple;
            disp.printCell(getMap(),ep,ct);
        }
    }

    for (int i = 0; i < level_obj.getNumItemPiles(); ++i)
    {
        ip = level_obj.getPile(i)->getLoc();
        if (getMap()->getm(ip))
        if (getMap()->getTerrain(ip) == WALL_INDESTRUCT || getMap()->getTerrain(ip) == WALL_DESTRUCT)
        {
            ct.color.fg = color_gold;
            ct.color.bg = color_black;
            ct.ascii = (int)'#';
            disp.printCell(getMap(),ip,ct);
        }
    }

    for (uint m_type = 0; m_type < 2; ++m_type)
    {
        num_machines = (uint)level_obj.getNumMachinesOfType((terrain_t)(m_type + 13));

        for (uint i = 0; i < num_machines; ++i)
        {
            sp = level_obj.getMachineLocOfType((terrain_t)(m_type + 13),i);

            if (getMap()->getv(sp))
            if (getMap()->getFire(sp) == NIL_f &&
                getMap()->getItem(sp) == NIL_i &&
                getMap()->getNPC(sp) == NIL_n)
            {
                ct = symbol_terrain[m_type + 13];
                ct.color.fg = getPresetColor(ROLL(11));
                disp.printCell(getMap(),sp,ct);
            }
        }
    }

    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        bp = level_obj.getBeing(i)->at();
        nt = level_obj.getBeing(i)->getType();
        //ai = level_obj.getBeing(i)->getAttackID();

        if (getMap()->getv(bp))
        if (getMap()->getFire(bp) == NIL_f)
        {
            ct = symbol_NPC[(int)nt];
            // set foreground color (if needed)
            switch(nt)
            {
            case(PROCGEN_NPC):
                ct.color.fg = level_obj.getBeing(i)->getFactoryColor();
                ct.ascii = level_obj.getBeing(i)->getFactoryAscii();
                disp.printCell(getMap(),bp,ct);
                break;
            case(MAGICIAN_NPC):
                ct.color.fg = getRandomVisibleColor();
                disp.printCell(getMap(),bp,ct);
                break;
            case(SIREN_NPC):
                ct.color.fg = getPresetColor(2 + ROLL(1));
                disp.printCell(getMap(),bp,ct);
                break;
            case(YINAXIMAI_NPC):
                ct.color.fg = ROLL(1) == 0 ? color_blue : (ROLL(1) == 0 ? color_darkgray : color_white);
                disp.printCell(getMap(),bp,ct);
                break;
            case(ADVERSARY_NPC):
                ct.color.fg = getDimmedPresetColor(getPresetColor(11),2,0);
                disp.printCell(getMap(),bp,ct);
                break;
            default:
                break;
            }
        }
    }
}

void game::swapPlaces(being *a, being *b)
{
    point a_at = a->at();
    point b_at = b->at();

    getMap()->setNPC(b_at,a->getType());

    a->moveTo(b_at);

    getMap()->setNPC(a_at,b->getType());

    b->moveTo(a_at);
}

// move player to new location
void game::movePlayer(point delta)
{
    if (getPlayer()->getStaggerCount() > 0)
    {
        getPlayer()->decStaggerCount();
        if (getPlayer()->getStaggerCount() == 0)
        {
            msgeAdd("You recover.",colorpair_white_black);
            return;
        }
        else
            msgeAdd("You stagger...",colorpair_white_black);
        delta = point(ROLL(2)-1,ROLL(2)-1);
        if (isAt(delta,point(0,0)))
            return;
    }
    // first add delta to player's current position
    point new_loc = addPoints(getPlayer()->at(),delta);

    if (isCellBlocking(getMap()->getNPC(new_loc)))
    {
        if (level_obj.getBeing(new_loc)->getAllyValue() > 0)
        {
            swapPlaces(getPlayer(),level_obj.getBeing(new_loc));
            msgeAdd("You swap places.",colorpair_gray_black);
            player_quick_fire_spot = getPlayer()->at();
            return;
        }
        if (confirmMelee(level_obj.getBeing(new_loc)))
        {
            melee(getPlayer(),level_obj.getBeing(new_loc));
        }
        else
            msgeAdd("You refrain.",colorpair_gray_black);
        return;
    }
    // if "can move to this next location because it is not blocked by a WALL or something"
    if (!isCellBlocking(getMap()->getTerrain(new_loc)))
    {
        // change player tile to NULL in old location, and set new location to PLAYER tile
        changeBeingTile(getPlayer()->at(),new_loc,PLAYER);
        // set the new player coordinates stored in the player's
        getPlayer()->moveTo(new_loc);
    }
    else
    {
        if (getPlayer()->getStaggerCount() > 0)
        {
            msgeAdd("*Bonk*... OUCH!",colorpair_gray_black);
        }
    }
    player_quick_fire_spot = getPlayer()->at();
}

// change an npc or PLAYER tile to NULL in old location, and set new location that npc or PLAYER tile
void game::changeBeingTile(point from, point to, NPC_t whom)
{
    // set null to the being tile at old location
    getMap()->setNPC(from,NIL_n);
    // put being tile at new location
    getMap()->setNPC(to,whom);
    // draw old tile
    disp.printCell(getMap(),from,disp.getSymbol(getMap(),from));
    // draw new tile
    disp.printCell(getMap(),to,disp.getSymbol(getMap(),to));
}

void game::checkForLevelFeature()
{
    if (useFeatureAtPlayer(getPlayer()->at()))
        return;

    if (ibuf.getAction() == INP_SELECT)
        return;

    // number of level features adjacent to player
    int num_adj = 0;

    // dp is a delta point, last_loc is only used
    // if there is a single level feature adjacent
    // to the player
    point dp, last_loc;

    // adjacent terrain temp tile
    terrain_t adj_terrain = NIL_t;

    // check for number of adjacent "level features"
    for (int x = -1; x <= 1; ++x)
        for (int y = -1; y <= 1; ++y)
            if (x != 0 || y != 0)
            {
                // adjacent tile
                dp = addPoints(getPlayer()->at(),point(x,y));
                // adjacent terrain
                adj_terrain = getMap()->getTerrain(dp);
                // check if door, vending machine, etc...
                if (isCellLevelFeature(adj_terrain))
                {
                    num_adj++;
                    last_loc = dp;
                }
            }

    // if only one, toggle that feature
    if (num_adj == 1)
    {
        if (getMap()->getNPC(last_loc) == NIL_n && getMap()->getItem(last_loc) == NIL_i)
            useFeaturePlayer(last_loc);
        else
            msgeAdd("Someone or something is in the way!",colorpair_gray_black);

        return;
    }

    clearRange(SHOWWID,0,GRIDWID-1,SHOWHGT - 1);
}

void game::useFeaturePlayer(point loc)
{
    switch(getMap()->getTerrain(loc))
    {
    case(OPEN_UNLOCKED_DOOR):
        if (ibuf.getAction() == INP_TOGGLE)
        {
            level_obj.getDoor(loc)->setState(0);
            getMap()->setTerrain(loc,CLOSED_UNLOCKED_DOOR);
            msgeAdd("You closed the door.",colorpair_white_black);
        }
        break;
    case(CLOSED_UNLOCKED_DOOR):
        if (ibuf.getAction() == INP_TOGGLE)
        {
            level_obj.getDoor(loc)->setState(-1);
            getMap()->setTerrain(loc,OPEN_UNLOCKED_DOOR);
            msgeAdd("You opened the door.",colorpair_white_black);
        }
        break;
    default:
        break;
    }
}

void game::checkIfTargetIsHostile(being *attacker, being *target)
{
    if (NPCsCannotFight(attacker->getType(),target->getType()))
        return;

    target->setAttackID(attacker->getID());
    target->setMobility(true);
}

// get pointer to instance of the current level's map
map *game::getMap()
{
    return level_obj.getMap();
}

point game::getFleeLoc(point target)
{
    int tries = 0;

    point dest = point(1,1);

    do
    {
        tries++;
        dest = level_obj.getRandWalkablePoint(1,1,MAPWID-2,MAPHGT-2);
    }
    while (pointDist(dest,target) < 6 && tries < 3);

    return dest;
}

void game::dropItems(being *b, int s, int q)
{
    level_obj.addItem(*(b->getInv()->getItem(s)->getStats()),q,b->at());
    b->getInv()->removeItem(s,q);
}

void game::pickUpItems(being *b, int s, int q)
{
    item_stats * istats = level_obj.getPile(b->at())->getItem(s)->getStats();

    if(!getMap()->getv(b->at()))
        getMap()->setu(b->at(),symbol_item[(int)istats->i_type]);

    if (istats->category != ITEM_CREDIT)
        b->getInv()->addToContainer(*istats, q);
    else
        b->setCredits(b->getCredits() + (q*credit_worth[(int)istats->i_type - 1]));

    printPickupMessage(b,q,istats->name);

    level_obj.deleteItemFromPile(b->at(),s,q);
}

void game::printPickupMessage(being *b, int q, STRING item_name)
{
    STRING picked_up_string = item_name;

    if (q == 1)
    {
        picked_up_string = getDeterminer(item_name) + item_name;
    }
    else
    {
        picked_up_string = " " + int2String((uint64)q) + " " + item_name + "s";
    }

    if (b->getType() == PLAYER)
        msgeAdd("You pick up" + picked_up_string + ".",colorpair_gray_black);
    else
        NPCMessage(b,actionToMessage(b,"picks up" + picked_up_string + "."),false);
}

// Have an NPC or player drop all belongings
// turned off for now
void game::beingDropAllItems(being *b, point at)
{
    int item_quant;
    item_stats istats;
    item_t weapon, armor;
    weapon = b->getInv()->getWeapon()->getType();
    armor = b->getInv()->getArmor()->getType();
    chtype prev_sym = disp.getSymbol(getMap(),at);
    // drop armor
    if (armor != NIL_i)
        level_obj.addItem(*(b->getInv()->getArmor()->getStats()),1,at);
    // drop weapon
    if (weapon != NIL_i)
        level_obj.addItem(*(b->getInv()->getWeapon()->getStats()),1,at);

    // drop everything else
    for (int i = 0; i < b->getInv()->getNumItems(); ++i)
    {
        istats = *(b->getInv()->getItem(i)->getStats());
        item_quant = b->getInv()->getItem(i)->getQuant();
        level_obj.addItem(istats,item_quant,at);
    }
    if (b->getCredits() > 0)
    {
        level_obj.addCreditsGreedyMethod(b->getCredits(),at);
        b->setCredits(0);
    }

    if (getMap()->getItem(at) != NIL_i && !getMap()->getv(at))
    {
        getMap()->setu(at,prev_sym);
    }

    b->getInv()->deleteAllItems();
    b->getInv()->setWeapon(item_default_stats[(int)NIL_i]);
    b->getInv()->setArmor(item_default_stats[(int)NIL_i]);
}

void game::NPCMessage(being *b, STRING s, bool audible)
{
    if (getMap()->getv(b->at()) || audible)
        msgeAdd(s,colorpair_white_black);
}

STRING game::getNameToPrint(being *b, bool prefix, bool capital)
{
    STRING being_name = b->getStats()->name;

    STRING gender_modifier = "";

    STRING prefix_modifier = "";

    if (being_name == being_default_stats[(int)b->getType()].name || b->getType() == PROCGEN_NPC)
    {
        if (prefix && !b->isNamed())
            prefix_modifier = "the ";
        if (b->getGender() == BEING_GENDER_FEMALE && b->getType() != SIREN_NPC && b->getType() != YINAXIMAI_NPC)
            gender_modifier = "female ";
        being_name = prefix_modifier + gender_modifier + being_name;
    }

    if (!getMap()->getv(b->at()))
        being_name = "someone";

    if (capital)
        being_name[0] = toupper(being_name[0]);

    return being_name;
}

STRING game::actionToMessage(being *b, STRING msg)
{
    return (getNameToPrint(b,true,true) + " " + msg);
}

STRING game::speechToMessage(being *b, STRING msg)
{
    return (getNameToPrint(b,false,true) + ": " + "\"" + msg + "\"");
}

// will be modified as game starts needing balancing
// b => attacker
int baseMeleeDamage(being *b)
{
    int ret_val = ROLL((int)( (b->getStats()->strength * b->getStats()->strength) / 80 ) ) + 1;

    if (b->getInv()->getWeapon()->getStats()->category != ITEM_MELEE_WEAPON)
        return ret_val;

    ret_val += rollMultipleDice(b->getInv()->getWeapon()->getStats()->wepstats.dam);

    return ret_val;
}

bool baseHitChance(being *attacker, being *target, bool fire_weapon)
{
    int weapon_to_hit = 0;

    int acc = attacker->getStats()->accuracy;

    item_t weapon = attacker->getInv()->getWeapon()->getType();

    if (weapon != NIL_i)
    if (item_default_stats[(int)weapon].category == ITEM_MELEE_WEAPON || fire_weapon)
    {
        weapon_to_hit = attacker->getInv()->getWeapon()->getStats()->wepstats.to_hit;
        acc += weapon_to_hit;

        if (weapon == SNIPER_RIFLE)
            acc += pointDist(attacker->at(),target->at());
    }

    acc = (acc - target->getInv()->getArmor()->getStats()->armstats.basedv_value);

    if (acc < 1 )
        acc = 1;

    return ROLL(acc) >= ROLL(target->getStats()->dexterity);
}

// chance being will flee from player
// if flee chance is 30, then the being
// has a 30% chance of fleeing when b's hitpoints
// are lower then or equal to 30.
// so "flee_chance" is used for 2 things
// (I might split it up later, so that this is not the case)
bool calculateFleeChance(being *b)
{
    int flee_chance = b->getStats()->flee_chance;
    float percent_left = ( (float) b->getHP() / (float)b->getStats()->max_hit_points ) * (float)100.0;
    return (percent_left < (float)flee_chance && PERCCHANCE() <= (uint)flee_chance);
}

STRING damageStatusString(being *b)
{
    int cur_hp = b->getHP();
    int max_hp = b->getStats()->max_hit_points;

    if (cur_hp == max_hp)
        return "Unharmed";
    if ((float)cur_hp >= 11*(float)max_hp/12)
        return "Barely scathed";
    if ((float)cur_hp >= 8*(float)max_hp/9)
        return "Somewhat hurt";
    if ((float)cur_hp >= 3*(float)max_hp/4)
        return "Lightly wounded";
    if ((float)cur_hp >= (float)max_hp/2)
        return "Wounded";
    if ((float)cur_hp >= (float)max_hp/4)
        return "Severely wounded";
    if ((float)cur_hp >= (float)max_hp/7)
        return "Brutally wounded";

    return "Dying";
}

STRING deathMessage(being *b)
{
    switch(ROLL(7))
    {
    case(0):
        return "hit the dust.";
    case(1):
        return "is killed.";
    case(2):
        return "is toast!";
    case(3):
        return "dies!";
    case(4):
        return "was wasted.";
    case(5):
        return "is dead!";
    case(6):
        return "croaked.";
    }

    return "died.";
}

STRING playerPainStatus(int dam)
{
    if (dam > 170)
        return "You are pulverized!";
    if (dam > 100)
        return "Yikes! That wasn't pretty!";
    if (dam > 60)
        return "That was excrutiating!";
    if (dam > 20)
        return "Your vision doubles momentarily...";

    return "That really hurt!";
}

void game::checkPlayerQuickPickup()
{
    int i = 0;
    int quant = 0;
    int num_initial_items = 0;
    int num_current_items = 0;

    if (!level_obj.isPileAt(getPlayer()->at()))
        msgeAdd("There's nothing here.",colorpair_white_black);
    else
    {
        cell_pile * pile = level_obj.getPile(getPlayer()->at());

        num_initial_items = num_current_items = pile->getNumItems();

        do
        {
            quant = pile->getItem(i)->getQuant();

            if (canPickupItem(getPlayer(),pile->getItem(i),quant))
            {
                pickUpItems(getPlayer(),i,quant);
                num_current_items = (level_obj.isPileAt(getPlayer()->at()) ? pile->getNumItems() : 0);
            }
            else
                i++;

        } while (getMap()->getItem(getPlayer()->at()) != NIL_i && i < num_current_items);

        if (num_current_items == num_initial_items)
        {
            if (num_initial_items == 1)
                msgeAdd(quant == 1 ? "You cannot lift that." : "You cannot lift all of those.",colorpair_white_black);
            else
                msgeAdd("You are unable to lift this item pile.",colorpair_white_black);
        }
    }
}

void game::checkPlayerPickup(being *b, item *it, int selection, int quant, bool quick)
{
    if (canPickupItem(b,it,quant))
        pickUpItems(b,selection,quant);
    else if (!quick)
        msgeAdd("You can't carry that!",colorpair_gray_black);
}

void game::examineItemList(item_list_category ilcat, point it_loc)
{
    // which item did the user select?
    int selection = 0;

    item_container * itc;

    clearRange(0,0,GRIDWID-1,SHOWHGT-1);

    switch(ilcat)
    {
    case(ITEMLIST_PILE):
        // check if cell tile is NIL_i. If it is, show empty list indication
        // else if not empty, then a pile exists with at least one item there
        // display the list
        addGFXString(colorpair_gray_black,"Items laying here:",2,0);
        if (getMap()->getItem(it_loc) != NIL_i)
            itc = level_obj.getPile(it_loc);
        else
        {
            printMessageAndWaitForSpace("Nothing here now -> press [SPACE] to exit");
            clearRange(0,0,GRIDWID-1,2);
            return;
        }
        break;
    case(ITEMLIST_INV):
        addGFXString(colorpair_gray_black,"Inventory:",2,0);
        displayEquippedItems(getPlayer()->getInv());
        if (getPlayer()->getInv()->getNumItems() > 0)
            itc = getPlayer()->getInv();
        else
        {
            printMessageAndWaitForSpace("No items -> press [SPACE] to exit");
            clearRange(0,0,GRIDWID-1,SHOWHGT);
            return;
        }
        break;
    case(ITEMLIST_SHOP):
        addGFXString(colorpair_gray_black,"Arms Dealer's Inventory:",2,0);
        if (level_obj.getBeing(it_loc)->getInv()->getNumItems() > 0)
            itc = level_obj.getBeing(it_loc)->getInv();
        else
        {
            printMessageAndWaitForSpace("Empty -> press [SPACE] to exit");
            clearRange(0,0,GRIDWID-1,SHOWHGT);
            return;
        }
        break;
    }

    do
    {
        printItemList(itc, selection, true);

        ibuf.menuSelect(selection,itc->getNumItems());

        mbuf.clearAndDeleteAllMessages();

        if (ilcat == ITEMLIST_PILE)
        {
            if (ibuf.getAction() == INP_TOGGLE)
                checkPlayerPickup(getPlayer(),itc->getItem(selection),selection,1,false);
            if (ibuf.getAction() == INP_ALL)
                checkPlayerPickup(getPlayer(),itc->getItem(selection),selection,itc->getItem(selection)->getQuant(),false);
            if (!level_obj.isPileAt(getPlayer()->at()))
            {
                clearRange(1,1,GRIDWID-2,14);
                clearRange(2,18,GRIDWID-2,20);
                printPlayerStats();
                printMessageAndWaitForSpace("Nothing here anymore -> press [SPACE] to exit");
                break;
            }
        }

        if (ilcat == ITEMLIST_INV)
        {
            if (ibuf.getAction() == INP_DROP)
                dropItems(getPlayer(),selection,1);
            if (ibuf.getAction() == INP_ALL)
                dropItems(getPlayer(),selection,itc->getItem(selection)->getQuant());
            if (ibuf.getAction() == INP_TOGGLE)
            {
                if(checkPlayerUseItem(selection))
                   break;
                displayEquippedItems(getPlayer()->getInv());
            }
            if (getPlayer()->getInv()->getNumItems() == 0)
            {
                clearRange(1,1,GRIDWID-2,14);
                clearRange(2,18,GRIDWID-2,20);
                printPlayerStats();
                printMessageAndWaitForSpace("No items left -> press [SPACE] to exit");
                break;
            }
        }

        if (ilcat == ITEMLIST_SHOP)
        {
            if (ibuf.getAction() == INP_TOGGLE)
                examineItemFromArmsDealer(1,selection,level_obj.getBeing(it_loc));
            if (ibuf.getAction() == INP_ALL)
                examineItemFromArmsDealer(itc->getItem(selection)->getQuant(),selection,level_obj.getBeing(it_loc));
            if (itc->getNumItems() == 0)
            {
                clearRange(1,1,GRIDWID-2,14);
                clearRange(2,18,GRIDWID-2,20);
                printPlayerStats();
                printMessageAndWaitForSpace("Cleaned out... ->press [SPACE] to exit");
                break;
            }
        }

        printPlayerStats();

        selection = MINIMUM(selection,itc->getNumItems() - 1);

    }
    while (ibuf.getAction() != INP_EXITMENU);

    clearRange(0,0,GRIDWID-1,SHOWHGT);
}

void game::promptMainMenu()
{
    printMainMenu();
    updateScreen();
    ibuf.setAction();
    clearRange(0,0,GRIDWID-1,GRIDHGT-1);
    updateScreen();
}

void game::examineItemFromArmsDealer(int quant, int index, being *b)
{
    item_stats *i_stats = b->getInv()->getItem(index)->getStats();

    uint64 item_cost = getItemCost(i_stats,quant,level_number);

    if (quant == 1)
        msgeAdd("\"I'll sell that to you for " + int2String(item_cost) + " credits.\"",colorpair_white_black);
    else
        msgeAdd("\"I'll sell em' all to you for " + int2String(item_cost) + " credits.\"",colorpair_white_black);


    if (item_cost > getPlayer()->getCredits())
        msgeAdd("\"...when you get the cash!\"",colorpair_white_black);
    else
    {
        msgeAdd("Really purchase? ('p' > purchase)",colorpair_white_black);
        updateScreen();
        if (ibuf.checkForKey('p'))
        {
            getPlayer()->setCredits(getPlayer()->getCredits() - item_cost);
            level_obj.addItem(*i_stats,quant,getPlayer()->at());
            b->getInv()->removeItem(index,quant);
            msgeAdd("You purchase the item.",colorpair_white_black);
        }
        else
            msgeAdd("You decide to not make the purchase...",colorpair_white_black);
    }

    printPlayerStats();
}

void game::printMessageAndWaitForSpace(STRING s)
{
    addGFXString(colorpair_gray_black,s,2,1);
    updateScreen();
    ibuf.waitForKey(' ');
}

bool canPickupItem(being *b, item *it, int quant)
{
    if ((int)it->getType() >= (int)CREDIT && (int)it->getType() <= (int)CREDIT_1B)
        return true;

    const int curr_weight = b->getInv()->getTotalWeight();
    const int item_weight = it->getStats()->weight * quant;

    return curr_weight + item_weight <= b->getMaxCarry();
}

bool game::checkNPCPickupEvent(being * b)
{
    if (PERCCHANCE() <= (uint)b->getStats()->pickup_chance)
    if (level_obj.isPileAt(b->at()))
    {
        cell_pile * pile = level_obj.getPile(b->at());
        item_category itcat;
        for (int i = 0; i < pile->getNumItems(); ++i)
        {
            itcat = pile->getItem(i)->getStats()->category;
            if (canPickupItem(b,pile->getItem(i),pile->getItem(i)->getQuant()))
                switch(itcat)
                {
                    case(ITEM_CREDIT):
                    case(ITEM_MELEE_WEAPON):
                    case(ITEM_FIREARM_WEAPON):
                    case(ITEM_AMMUNITION):
                        pickUpItems(b,i,pile->getItem(i)->getQuant());
                        return true;
                    default:
                        break;
                }
        }
    }

    return false;
}

bool game::checkNPCEquipEvent(being *b)
{
    if (b->getAttackID() == -1)
        return false;

    int num_items = b->getInv()->getNumItems();

    if (num_items == 0)
        return false;

    int which_item = ROLL(num_items - 1);

    item_stats * istats = b->getInv()->getItem(which_item)->getStats();

    if (PERCCHANCE() <= (uint)b->getStats()->equip_chance)
    {
        if (istats->category == ITEM_MELEE_WEAPON ||
            istats->category == ITEM_FIREARM_WEAPON)
        {
            NPCMessage(b,actionToMessage(b,"equips" + getDeterminer(istats->name) + istats->name + "."),false);
            b->getInv()->setWeapon(which_item);
        }
        else
        {
            NPCMessage(b,actionToMessage(b,"is searching " + getGenderPronoun(b->getGender()) + " pack for a weapon."),false);
        }

        return true;
    }

    return false;
}


bool game::beingReloadFirearm(being * b)
{
    // helpful locals
    STRING weapon_name = b->getInv()->getWeapon()->getStats()->name;

    item_t weapon_type = b->getInv()->getWeapon()->getType();

    item_t ammo_type = weapon2ammo(weapon_type);

    item_stats istats = item_default_stats[(int)ammo_type];

    int current_load = b->getInv()->getWeapon()->getStats()->num_subitems.quant;

    int ammo_type_inv_loc = b->getInv()->getPos(&istats);

    int num_ammo_reload = 0;

    int max_num_reload_amount = 1;
    // getPos returns -1 if item not found
    if (ammo_type_inv_loc > -1)
    {
        if (b->getType() == PLAYER)
            msgeAdd("You reload the " + weapon_name + ".",colorpair_gray_black);
        else
            NPCMessage(b,actionToMessage(b,"reloads" + getDeterminer(weapon_name) + weapon_name + "."), false);

        // code that deals with reloading
        max_num_reload_amount = b->getInv()->getWeapon()->getStats()->num_subitems.max_quant;
        num_ammo_reload = MINIMUM(b->getInv()->getItem(ammo_type_inv_loc)->getQuant(),max_num_reload_amount - current_load);
        // reload...
        b->getInv()->getWeapon()->setSubContainerAmount(num_ammo_reload + current_load);
        b->getInv()->removeItem(ammo_type_inv_loc,num_ammo_reload);
    }
    else
    {
        if (b->getType() == PLAYER)
            msgeAdd("You have no ammo for your " + weapon_name + " in your pack!",colorpair_gray_black);
        else
        {
            NPCMessage(b,actionToMessage(b,"is looking around for ammo."),false);
            return false;
        }
    }

    return true;
}

void game::playerReloadFirearm()
{
    const uint num_subitems = getPlayer()->getInv()->getWeapon()->getStats()->num_subitems.quant;
    const uint max_num_subitems = getPlayer()->getInv()->getWeapon()->getStats()->num_subitems.max_quant;

    if (getPlayer()->getInv()->getWeapon()->getStats()->category == ITEM_FIREARM_WEAPON)
    {
        if (num_subitems < max_num_subitems)
            beingReloadFirearm(getPlayer());
        else
            msgeAdd("Your weapon is already fully loaded.",colorpair_gray_black);
    }
    else
        msgeAdd("Reload what?",colorpair_gray_black);
}

void game::beingFireWeapon(being *b, point p)
{
    item_t weapon_in_hand = b->getInv()->getWeapon()->getType();

    STRING weapon_name = b->getInv()->getWeapon()->getStats()->name;

    color_pair weapon_color = symbol_item[(int)b->getInv()->getWeapon()->getType()].color;

    if (b->getType() == PLAYER)
        msgeAdd("You fire your " + weapon_name + ".",colorpair_white_black);
    else
        NPCMessage(b,actionToMessage(b,"fires" + getDeterminer(weapon_name) + weapon_name + "."),false);

    switch(weapon_in_hand)
    {
    case(PISTOL_10MM):
        msgeAdd("*BANG!*",weapon_color);
        shootProjectile(b,p,weapon_in_hand,2,-1,1);
        break;
    case(PISTOL_STEALTH):
        msgeAdd("*pfft*",weapon_color);
        shootProjectile(b,p,weapon_in_hand,2,-1,1);
        break;
    case(REVOLVER):
        msgeAdd("*KAPOW!*",weapon_color);
        shootProjectile(b,p,weapon_in_hand,2,-1,1);
        break;
    case(ASSAULT_RIFLE):
        msgeAdd("*B-r-r-r-r-r-r-r-APPPP*",weapon_color);
        for (uint i = 0; i < 3; ++i)
            if (b->getInv()->getWeapon()->getStats()->num_subitems.quant > 0)
            {
                shootProjectile(b,p,weapon_in_hand,2,-1,1);
            }
        break;
    case(CHAINGUN):
        msgeAdd("*PING-PING-PING-PING-PING-PING*",weapon_color);
        for (uint i = 0; i < 6; ++i)
            if (b->getInv()->getWeapon()->getStats()->num_subitems.quant > 0)
            {
                shootProjectile(b,p,weapon_in_hand,2,9,1);
            }
        break;
    case(DOUBLEBARRELLED_SHOTGUN):
        if (b->getInv()->getWeapon()->getStats()->num_subitems.quant > 1)
        {
            msgeAdd("*BOOOOOOOOM!*",weapon_color);
            b->getInv()->getWeapon()->decrementSubContainerAmount();
            shootProjectile(b,p,weapon_in_hand,1,5,2);
            break;
        }
    case(COMBAT_SHOTGUN):
        msgeAdd("*BOOOM!*",weapon_color);
        shootProjectile(b,p,COMBAT_SHOTGUN,1,5,1);
        break;
    case(ROCKET_LAUNCHER):
        msgeAdd("*cccchhhhhhhUUUUUG*",colorpair_verydarkgray_black);
        shootProjectile(b,p,weapon_in_hand,1,-1,1);
        break;
    case(LASER_GUN):
        msgeAdd("*bzzzzzzz*",weapon_color);
        shootProjectile(b,p,weapon_in_hand,1,-1,1);
        delayAndUpdate(1);
        clearAllFireCells();
        break;
    case(DEATHRAY_GUN):
        msgeAdd("*bzzzaaaaap!*",weapon_color);
        shootProjectile(b,p,weapon_in_hand,1,1,1);
        break;
    case(FLAMETHROWER):
        msgeAdd("*FWWWWOOOOOOSHHHHH*",weapon_color);
        shootBurst(b,p,weapon_in_hand,8);
        break;
    case(SNIPER_RIFLE):
        msgeAdd("*BAAAAAMMMMMM!*",weapon_color);
        shootSniper(b,p);
        break;
    case(PLASMA_CANNON):
        msgeAdd("*CHOOOOOM*",weapon_color);
        shootProjectile(b,p,weapon_in_hand,1,3,1);
        break;
    case(PULSE_CANNON):
        msgeAdd("*v-r-r-r-r-EEEEEEEEEE!*",weapon_color);
        shootPulse(b,p,weapon_in_hand);
        break;
    case(FUSION_CANNON):
        msgeAdd("*CHOOOOOOOOOOOOM*",colorpair_red_black);
        shootProjectile(b,p,weapon_in_hand,1,6,1);
        break;
    default:
        break;
    }
}

void game::shootSniper(being *b, point lof)
{
    b->getInv()->getWeapon()->decrementSubContainerAmount();
    if (getMap()->getv(b->at()) || isAt(lof,getPlayer()->at()))
    {
        disp.setAndPrintFire(getMap(),lof,REDFIRE_SPRITE);
        delayAndUpdate(1);
        clearAllFireCells();
        updateScreen();
    }

    if (isCellBlocking(getMap()->getNPC(lof)))
        checkForBeingInLOF(1,lof,b);
}

bool game::isTargetableEnemy(int e)
{
    return  !isSpecialNonProcgen(level_obj.getBeing(e)->getType()) &&
            level_obj.getBeing(e)->getAllyValue() <= 0 &&
            getMap()->getv(level_obj.getBeing(e)->at());
}

point game::getNextTargetedEnemy(int &counter)
{
    bool found_enemy = false;

    int iter = 0;

    point loc = getPlayer()->at();

    for (int i = 0; i < level_obj.getNumNPCs(); ++i)
    {
        if (isTargetableEnemy(i))
            iter++;
    }

    if (iter == 0)
        return loc;

    do
    {
        if (isTargetableEnemy(counter))
        {
            loc = level_obj.getBeing(counter)->at();
            found_enemy = true;
        }

        counter = (counter + 1) % level_obj.getNumNPCs();

    }
    while (!found_enemy);

    return loc;
}

void game::playerFireWeaponToggle(bool quick_fire)
{
    item_category ic = getPlayer()->getInv()->getWeapon()->getStats()->category;

    point fire_at = getPlayer()->at();

    point temp_loc;

    int target_counter = 0;

    fire_at = getNextTargetedEnemy(target_counter);

    if (ic != ITEM_FIREARM_WEAPON)
    {
        msgeAdd("You need to equip a firearm first!",colorpair_gray_black);
        return;
    }

    if (getPlayer()->getInv()->getWeapon()->getStats()->num_subitems.quant > 0)
    {
        if (!quick_fire)
        {
            msgeAdd("Fire your weapon where?",colorpair_white_black);

            do
            {
                printPlayerFirePath(getPlayer()->at(),fire_at);

                if ((int)getMap()->getNPC(fire_at) > (int)PLAYER)
                {
                    displayNPCInfo(getMap()->getNPC(fire_at),level_obj.getBeing(fire_at)->getInv(),
                                   level_obj.getBeing(fire_at)->getStats()->name,level_obj.getBeing(fire_at)->getFactoryColor(),
                                   level_obj.getBeing(fire_at)->getFactoryAscii(),damageStatusString(level_obj.getBeing(fire_at)));
                }

                ibuf.setAction();

                if (ibuf.getAction() == INP_DELTA)
                {
                    temp_loc = addPoints(fire_at,ibuf.getDelta());
                    if (inRange(temp_loc,0,0,MAPWID-1,MAPHGT-1))
                        if (getMap()->getv(temp_loc))
                            fire_at = temp_loc;
                }

                if (isAt(temp_loc,fire_at))
                    clearAllFireCells();

                if (ibuf.getAction() == INP_SELECT)
                {
                    msgeAdd("You hold your fire.",colorpair_gray_black);
                    clearAllFireCells();
                    return;
                }

                if (ibuf.getAction() == INP_TOGGLE)
                {
                    clearAllFireCells();
                    fire_at = getNextTargetedEnemy(target_counter);
                }
            }
            while (ibuf.getAction() != INP_FIRE);

            clearAllFireCells();
        }
        else
            fire_at = player_quick_fire_spot;

        if (!isAt(fire_at,getPlayer()->at()))
            beingFireWeapon(getPlayer(),fire_at);
        else
            msgeAdd("Please select a proper target.",colorpair_gray_black);

        player_quick_fire_spot = fire_at;
    }
    else
        playerReloadFirearm();
}

void game::printPlayerFirePath(point a, point b)
{
    linetracer.bresenham(a,b);

    getMap()->setFire(b,CURSOR_GEN);

    for (int i = 1; i < linetracer.getLineSize(); ++i)
        getMap()->setFire(linetracer.getLinePoint(i),CURSOR_FIREPATH);

    if (!isAt(a,b))
        getMap()->setFire(a,NIL_f);

    disp.printMap(getMap());

    setCustomCellColors();

    updateScreen();
}

void game::shootProjectile(being *b, point dest, item_t weap, int disp_mod, int range, int num_ammo)
{
    b->getInv()->getWeapon()->decrementSubContainerAmount();
    // location of shooter --- line of fire --- extrapolated
    // target location (extending beyond intended target)
    point source = b->at(), lof = point(1,1);
    // initialize projectile line and line iter
    int point_iter = 0;
    int primary_iter = 0;
    bool is_visible = (getMap()->getv(b->at()) || isAt(dest,getPlayer()->at()));
    bool exploding = b->getInv()->getWeapon()->getStats()->wepstats.exploding;
    linetracer.bresenham(source,dest);
    do
    {
        // ERROR
        if (linetracer.getLineSize() <= 1)
        {
            displayNPCErrorScreen(b,"BAD PROJECTILE SRC-TARGET LINE DISTANCE: " + int2String(linetracer.getLineSize()),"shootProjectile() RETURNING IMMEDIATELY");
            return;
        }
        point_iter = linetracer.getLineSize() - 2;
        do
        {
            // update line of fire coord
            lof = linetracer.getLinePoint(point_iter);
            // print lof to map display
            if (primary_iter % disp_mod == 0)
            if (is_visible)
                outputLOFTransition(disp_mod,weap,lof,b->at(),dest);
            // keep looping until reached
            // selected target destination
            if (isCellBlocking(getMap()->getNPC(lof)))
            if (checkForBeingInLOF(num_ammo,lof,b))
            {
                projectileHit(lof,b,weap,is_visible,exploding,disp_mod);
                return;
            }
            if (isCellBlocking(getMap()->getTerrain(lof)) ||
                primary_iter == range - 1)
            {
                projectileHit(lof,b,weap,is_visible,exploding,disp_mod);
                return;
            }
            point_iter--;
            primary_iter++;
        }
        while (point_iter >= 0);
        // this should not loop forever, as long as the
        // level boundaries are filled with walls
        // (else most likely crash)
        extrapolateLine(source,dest);
    }
    while (1);
}

// cause solid terrain hit
// if rocket launcher cause explosion
void game::projectileHit(point lof, being *b, item_t weap, bool is_visible, bool exploding, int disp_mod)
{
    if (is_visible)
    if (weap != LASER_GUN)
        clearFireInRange(lof,disp_mod);
    if (exploding)
    {
        createExplosion(b,b->getInv()->getWeapon()->getStats()->wepstats.dam,lof,false);
        checkForDestroyingTerrain(lof);
    }
}

void game::checkForDestroyingTerrain(point lof)
{
    if (isDestructableTerrain(getMap()->getTerrain(lof)))
    {
        if (!getMap()->getv(lof))
            getMap()->setu(lof,symbol_terrain[(int)getMap()->getTerrain(lof)]);
        if (getMap()->getTerrain(lof) == CLOSED_UNLOCKED_DOOR)
            level_obj.destroyDoor(lof);
        getMap()->setTerrain(lof,FLR_GEN);
    }
}

void game::extrapolateLine(point &source, point &dest)
{
    point extrap = dest;
    dest.setx(2*dest.x() - source.x());
    dest.sety(2*dest.y() - source.y());
    // source is now former target
    source = extrap;

    linetracer.bresenham(source,dest);
}

// create fire sprite symbol, then delay and surround it with
// null fire sprite symbols, and repeat in direction weapon was shot.
// If laser gun, do not clear fire until solid terrain was hit.
void game::outputLOFTransition(int disp_mod, item_t weap, point lof, point source, point dest)
{
    if (weap != LASER_GUN)
        clearFireInRange(lof,disp_mod);
    disp.setAndPrintFire(getMap(),lof,selectFireCell(dest,source,weap));
    if (weap != LASER_GUN)
        delayAndUpdate(1);
}

void game::delayAndUpdate(int ms)
{
    setCustomCellColors();
    updateScreen();
    SDL_Delay(ms);
}

void game::clearFireInRange(point lof, int r)
{
    point dxy = point(1,1);
    for (int x = lof.x() - r; x <= lof.x() + r; ++x)
        for (int y = lof.y() - r; y <= lof.y() + r; ++y)
        {
            dxy.set(x,y);
            if (inRange(dxy,0,0,MAPWID-1,MAPHGT-1))
                disp.setAndPrintFire(getMap(),dxy,NIL_f);
        }
}

void game::clearAllFireCells()
{
    disp.clearAllFireCells(getMap());
    disp.printMap(getMap());
    setCustomCellColors();
}

// Called in stream, burst, and projectile
// functions to check if an NPC is in the line of fire (lof)
// of the weapon that was fired. If so, return true, else false.
// if return true, end line of fire, else do not
bool game::checkForBeingInLOF(int num_ammo, point lof, being *a)
{
    being *being_in_lof;

    if (getMap()->getNPC(lof) != PLAYER)
        being_in_lof = level_obj.getBeing(lof);
    else
        being_in_lof = getPlayer();

    if (being_in_lof->isDead())
        return false;

    if (a->getAllyValue() > 0 && being_in_lof->getAllyValue() > 0)
        return false;

    bool travel_through = a->getInv()->getWeapon()->getStats()->wepstats.travel_through;

    checkForBeingStateEvent(a,being_in_lof,true);

    // base hit chance determines if it was a hit or a miss
    // flamethrower LOF cannot be dodged. It is always a hit.
    if (baseHitChance(a,being_in_lof,true))
    {
        damageBeingFromWeapon(num_ammo,lof,a,being_in_lof);

        if (!travel_through)
            return true;
    }

    return false;
}

void game::damageBeingFromWeapon(int num_ammo, point lof, being *attacker, being *target)
{
    if (target->getType() == PLAYER && !getMap()->getv(attacker->at()))
        msgeAdd("You are hit! (enemy unseen)",colorpair_gray_black);

    int total_damage = rollMultipleDice(attacker->getInv()->getWeapon()->getStats()->wepstats.dam);

    if (num_ammo == 1 && attacker->getInv()->getWeapon()->getType() == DOUBLEBARRELLED_SHOTGUN)
        total_damage = (int)(total_damage / 2);

    int crit_mult = checkForCriticalHit(attacker->getInv()->getWeapon(), false);

    total_damage *= crit_mult;

    total_damage -= getArmorProtectionValue(attacker,target,false,false,false);

    if (total_damage <= 0)
    {
        crit_mult = 1;
        total_damage = 0;
    }
    else
        checkForStunEvent(attacker,target,total_damage,false);

    if (getMap()->getv(target->at()))
        damageMsgeAdd(sint2String(total_damage),attacker->getInv()->getWeapon()->getType(),getExactSymbolFromNPC(attacker),
                      getExactSymbolFromNPC(target),(total_damage >= target->getHP()),false,getMap()->getv(attacker->at()),false,crit_mult);

    damageBeing(target, lof, total_damage);
}

int game::checkForCriticalHit(item * it, bool is_melee)
{
    if (it->getType() == NIL_i)
        return 1;

    if (it->getStats()->category == ITEM_FIREARM_WEAPON &&
        is_melee)
        return 1;

    if (PERCCHANCE() <= (uint)it->getStats()->wepstats.crit_perc)
        return (int)it->getStats()->wepstats.crit_multiplier;

    return 1;
}

// create an explosion based on input parameter base (in terms
// of damage and blast radius)
void game::createExplosion(being *creator, dice_roll base, point p, bool incinerate)
{
    msgeAdd("kaBOOOOOOOOOOOOOOM!",colorpair_orange_black);

    // damage is halved from base. This was put in primarly so that a direct rocket hit is more
    // damaging then the blast radius.
    dice_roll modified = {(uint)(base.num_dice/2),(uint)(base.num_sides),(uint)(base.num_bonus)};

    // formula for determining size of blast radius (1-7) (8 for incinerate powers)
    int radius = MAXIMUM(1,MINIMUM(6,(int) ( ( (base.num_dice * base.num_sides) + base.num_bonus ) / 100 ) ) );

    if (incinerate)
        radius = MAXIMUM(2,(int)(creator->getStats()->danger_level/2));

    int x_range;

    fire_t fire_sprite = NIL_f;

    point dp;

    uint num_sprites = 0;

    // first iteration displays explosion,
    // second causes the damage...
    // This was done so that message buffer does not block
    // explosion and cause it to partially display when
    // it asks for "press [SPACE] to continue"
    // if message buffer does block explosion, the full circle of
    // explosion sprites is still displayed

    // disp_iter == 0 -> print circle of explosion sprites
    // disp_iter == 1 -> do damage
    for (int disp_iter = 0; disp_iter < 2; ++disp_iter)
        for (int y = -radius; y <= radius; ++y)
        {
            x_range = (int)std::sqrt(((double)((radius * radius) - (y * y))));
            for (int x = -x_range; x <= x_range; ++x)
            {
                dp.set(p.x() + x, p.y() + y);
                if (inRange(dp,0,0,MAPWID-1,MAPHGT-1))
                {
                    if (disp_iter == 0)
                    {
                        if (getMap()->getv(dp))
                        {
                            fire_sprite = incinerate == true ? REDFIRE_SPRITE : (fire_t)(ROLL(2) + (int)REDFIRE_SPRITE);
                            disp.setAndPrintFire(getMap(),dp,fire_sprite);
                            num_sprites++;
                        }
                    }

                    else
                    {
                        if (x != 0 || y != 0)
                            if (isCellBlocking(getMap()->getNPC(dp)))
                                damageBeingFromExplosion(creator,getBeingAt(dp),dp,rollMultipleDice(modified),incinerate,false);
                    }
                }
            }
        }

    if (num_sprites > 0)
    {
        setCustomCellColors();
        updateScreen();
        SDL_Delay(1);
        clearAllFireCells();
    }
}

// helper function that causes damage from explosion to being "target"
// as a result of attack by being "creator"
void game::damageBeingFromExplosion(being *creator, being *target, point p, int base_damage, bool incinerate, bool circ_deathray)
{
    if (creator->getID() != target->getID())
    if (creator->getAllyValue() > 0 && target->getAllyValue() > 0)
         return;

    if (creator->getID() != target->getID())
        checkForBeingStateEvent(creator,target,false);

    base_damage -= getArmorProtectionValue(creator,target,false,true,circ_deathray);

    if (!circ_deathray)
    {
        if (base_damage <= 0)
            base_damage = 0;
        else if (!incinerate)
            checkForStunEvent(creator,target,base_damage,false);
    }

    if (getMap()->getv(target->at()))
        damageMsgeAdd(sint2String(base_damage),NIL_i,getExactSymbolFromNPC(creator),
                      getExactSymbolFromNPC(target),(base_damage >= target->getHP()),true,getMap()->getv(creator->at()),false,1);

    damageBeing(target,p,base_damage);
}

// get being at P
// Same as getBeing(p) in level class
// except extended for player or NPC
being *game::getBeingAt(point p)
{
    if (isAt(p,getPlayer()->at()))
        return getPlayer();
    else
        return level_obj.getBeing(p);
}

being *game::getPlayer()
{
    return &player;
}

// shoot flame burst sprite that is 3 tiles wide (only flamethrower for now)
void game::shootBurst(being *b, point dest, item_t weap, int length)
{
    b->getInv()->getWeapon()->decrementSubContainerAmount();

    int dist_counter = 0;

    point source = b->at(), lof[3], adjp;

    point dl = point(dest.x() - source.x(), dest.y() - source.y());

    bool block[3] = {false,false,false};

    bool is_visible = (getMap()->getv(b->at()) || isAt(dest,getPlayer()->at()));

    int point_iter = 0;

    if (abs(dl.y()) > abs(dl.x()))
        adjp = point(1,0);
    else
        adjp = point(0,1);

    linetracer.bresenham(source,dest);

    point_iter = linetracer.getLineSize() - 2;

    do
    {
        dest.set(linetracer.getLinePoint(point_iter).x(),
                 linetracer.getLinePoint(point_iter).y());

        lof[0].set(dest.x(),dest.y());
        lof[1].set(dest.x() + adjp.x(), dest.y() + adjp.y());
        lof[2].set(dest.x() - adjp.x(), dest.y() - adjp.y());

        for (int i = 0; i < 3; ++i)
            if (!block[i])
            {
                if (isCellBlocking(getMap()->getTerrain(lof[i])))
                    block[i] = true;
                else
                {
                    if (is_visible)
                        disp.setAndPrintFire(getMap(),lof[i],ORANGEFIRE_SPRITE);
                    if (isCellBlocking(getMap()->getNPC(lof[i])))
                        checkForBeingInLOF(1,lof[i],b);
                }
            }

        if (is_visible)
            delayAndUpdate(1);

        dist_counter++;

        point_iter--;

        if (point_iter == -1)
        {
            extrapolateLine(source,dest);
            //dist_counter--;
            point_iter = linetracer.getLineSize() - 2;
        }

    }
    while ((dist_counter < length) && (!block[0] || !block[1] || !block[2]));

    if (is_visible)
        clearAllFireCells();
}

// shoot a stream of fire sprites (pulse cannon only for now)
void game::shootPulse(being *b, point dest, item_t weap)
{
    b->getInv()->getWeapon()->decrementSubContainerAmount();

    bool end_hit = false;
    bool hit_being = false;

    point lof, lof_hit, source = b->at();

    //TCODLine::init(source.x(),source.y(),dest.x(),dest.y());
    linetracer.bresenham(source,dest);

    int point_iter = linetracer.getLineSize() - 1;

    bool is_visible = (getMap()->getv(b->at()) || isAt(dest,getPlayer()->at()));

    do
    {
        // condition: fire_t has NOT hit something
        if (!end_hit)
        {
            if (point_iter == 0)
            {
                extrapolateLine(source,dest);
                point_iter = linetracer.getLineSize() - 1;
            }
            point_iter--;
            lof.set(linetracer.getLinePoint(point_iter).x(),
                    linetracer.getLinePoint(point_iter).y());
            if (unWalkableCell(getMap()->getCell(lof)))
            {
                if (isCellBlocking(getMap()->getNPC(lof)))
                    if (checkForBeingInLOF(1,lof,b))
                        hit_being = true;

                if (hit_being || isCellBlocking(getMap()->getTerrain(lof)))
                {
                    lof_hit = lof;
                    linetracer.bresenham(b->at(),lof_hit);
                    point_iter = linetracer.getLineSize() - 1;
                    end_hit = true;
                }
            }
            else if (is_visible)
            {
                disp.setAndPrintFire(getMap(),lof,PURPLEPLASMA_SPRITE);
                delayAndUpdate(1);
            }
        }
        // it has hit something
        else
        {
            if (point_iter <= 0)
                break;

            lof.set(linetracer.getLinePoint(point_iter).x(),
                    linetracer.getLinePoint(point_iter).y());

            //set farthest cell from enemy/player with "t" fire_t value to NIL
            if (is_visible)
            {
                clearFireInRange(lof,1);
                delayAndUpdate(1);
            }

            if (hit_being && isCellBlocking(getMap()->getNPC(lof_hit)))
                damageBeingFromWeapon(1,lof_hit,b,getBeingAt(lof_hit));

            point_iter--;
        }
    }
    while (1);

    if (is_visible)
        disp.clearAllFireCells(getMap());
}

double getArmorCost(item_stats *istats)
{
    double total_cost = (double)istats->base_cost;

    double basepv_ratio = (double)(istats->armstats.basepv_value + 1.0) / (double)(item_default_stats[(int)istats->i_type].armstats.basepv_value + 1.0);
    double metallic_ratio = (double)(istats->armstats.metallic_bonus + 1.0) / (double)(item_default_stats[(int)istats->i_type].armstats.metallic_bonus + 1.0);
    double energy_ratio = (double)(istats->armstats.energy_bonus + 1.0) / (double)(item_default_stats[(int)istats->i_type].armstats.energy_bonus + 1.0);

    total_cost = (total_cost * basepv_ratio * metallic_ratio * energy_ratio);

    if (total_cost <= 0.0)
        return 1.0;

    return total_cost;
}

double getWeaponCost(item_stats *istats)
{
    double total_cost = (double)istats->base_cost;

    double damage_numdice_ratio = (double)istats->wepstats.dam.num_dice / ((double)item_default_stats[(int)istats->i_type].wepstats.dam.num_dice + 1.0);
    double damage_numsides_ratio = (double)istats->wepstats.dam.num_sides / ((double)item_default_stats[(int)istats->i_type].wepstats.dam.num_sides + 1.0);
    double damage_numbonus_ratio = (double)istats->wepstats.dam.num_bonus / ((double)item_default_stats[(int)istats->i_type].wepstats.dam.num_bonus + 1.0);

    double crit_mult = (double)istats->wepstats.crit_multiplier - (double)item_default_stats[(int)istats->i_type].wepstats.crit_multiplier;
    double crit_factor = (double)istats->wepstats.crit_perc - (double)item_default_stats[(int)istats->i_type].wepstats.crit_perc;
    double stun_factor = (double)istats->wepstats.stun_chance - (double)item_default_stats[(int)istats->i_type].wepstats.stun_chance;
    double tohit_factor = (double)istats->wepstats.to_hit - (double)item_default_stats[(int)istats->i_type].wepstats.to_hit;

    total_cost = ((double)total_cost * damage_numdice_ratio * damage_numsides_ratio * damage_numbonus_ratio);

    if (istats->wepstats.is_stealth)
        total_cost *= 2.0;

    // may be removed later (probably)
    if (crit_factor < -4.0)
        crit_factor = 4.0;
    // ------

    crit_factor *= (double)total_cost/5.0;

    total_cost += crit_factor;

    if (crit_factor > 0.0)
    {
        crit_mult *= (double)total_cost;

        total_cost += crit_mult;
    }

    stun_factor *= (double)total_cost/3.0;

    total_cost += stun_factor;

    if (tohit_factor < -99.0)
        tohit_factor = -99.0;

    tohit_factor *= (double)total_cost/100.0;

    total_cost += tohit_factor;

    // must not go in if statement for melee or will divide by 0
    if (istats->category == ITEM_FIREARM_WEAPON)
    {
        total_cost *= ((double)istats->num_subitems.max_quant / (double)item_default_stats[(int)istats->i_type].num_subitems.max_quant);
    }

    if (total_cost <= 0.0)
        return 1.0;

    return total_cost;
}

uint64 getItemCost(item_stats *istats, int quant, int level_number)
{
    uint64 ret_val = 1ULL;

    switch(istats->category)
    {
    case(ITEM_MELEE_WEAPON):
    case(ITEM_FIREARM_WEAPON):
    {
        ret_val = (uint64)getWeaponCost(istats);
        break;
    }
    case(ITEM_AMMUNITION):
    {
        ret_val = istats->base_cost;
        break;
    }
    case(ITEM_WEAPON_UPGRADE):
    {
        if (istats->misc_val == 0)
            ret_val = istats->base_cost;
        if (istats->misc_val == 1)
            ret_val = istats->base_cost * 10;
        if (istats->misc_val == 2)
            ret_val = istats->base_cost * 100;
        break;
    }
    case(ITEM_STATBOOSTER):
    {
        ret_val = istats->base_cost * level_number;
        break;
    }
    case(ITEM_MEDKIT):
    {
        ret_val = istats->base_cost * level_number;
        break;
    }
    case(ITEM_DEVICE):
    {
        ret_val = istats->base_cost * istats->misc_val;
        break;
    }
    case(ITEM_ARMOR):
    {
        ret_val = (uint64)getArmorCost(istats);
        break;
    }
    default:
        break;
    }

    ret_val *= (uint64)quant;

    return ret_val;
}

template <typename M>
void game::featurePlayerSelect(M *mach_obj)
{
    mach_obj->gotoNextState();
    mach_obj->drawMachine();
    updateScreen();
}

template <typename M>
void game::featurePlayerToggle(M *mach_obj)
{
    uint64 machine_retval = 0;

    msgeAdd("You press a button.",colorpair_gray_black);

    machine_retval = mach_obj->useMachine(getPlayer()->getCredits());

    machine_state ms = mach_obj->getState();

    if (ms == MSTATE_SLOTCASHOUT || ms == MSTATE_DIAMONDCASHOUT)
        getPlayer()->setCredits(getPlayer()->getCredits() + machine_retval);
    if (ms == MSTATE_SLOTINSERT || ms == MSTATE_DIAMONDINSERT)
        getPlayer()->setCredits(0);
}

template <typename M>
void game::useMachinePlayer(M *mach_obj)
{
    if (ibuf.getAction() == INP_TOGGLE)
        featurePlayerToggle(mach_obj);
    else
        featurePlayerSelect(mach_obj);

    printPlayerStats();
}

bool game::sendSatelliteSignal(satellite *s)
{
    uint64 player_credits = getPlayer()->getCredits();

    mbuf.clearAndDeleteAllMessages();
    if (s->isDistressMode())
    {
        msgeAdd("The " + s->getBeing()->getName() + " demand " + int2String(s->getCost()) + " credits to send one of their own as aid. Accept? (y -> yes)",colorpair_purple_black);
        updateScreen();
        if (ibuf.checkForKey('y'))
        {
            if (s->getCost() <= player_credits)
            {
                if (getMap()->getNPC(s->getSpawnLoc()) == NIL_n)
                {
                    getPlayer()->setCredits(player_credits - s->getCost());
                    msgeAdd("Help has arrived!",colorpair_green_black);
                    level_obj.addBeingGivenInventory(s->getBeing());
                    level_obj.getBeing(level_obj.getNumNPCs()-1)->setAllyValue(2);
                    s->genSatelliteNPC(level_obj.getNPCIDCounter());
                    msgeAdd(speechToMessage(level_obj.getBeing(s->getSpawnLoc()),"At your service."),colorpair_white_black);
                }
                else
                {
                    msgeAdd("Something went wrong with the transaction!",colorpair_purple_black);
                    msgeAdd("The archway glows momentarily...",colorpair_gray_black);
                }
            }
            else
            {
                msgeAdd("The " + s->getBeing()->getName() + " decide to ignore your request due to your lack of funds.",colorpair_purple_black);
            }
            return true;
        }
        msgeAdd("The " + s->getBeing()->getName() + " request that you stop wasting their time.",colorpair_white_black);
        updateScreen();
    }
    else
    {
        msgeAdd("Really send a threatening signal?",colorpair_purple_black);
        updateScreen();
        if (ibuf.checkForKey('y'))
        {
            if (getMap()->getNPC(s->getSpawnLoc()) == NIL_n)
            {
                level_obj.addBeingGivenInventory(s->getBeing());
                level_obj.getBeing(level_obj.getNumNPCs()-1)->setAttackID(0);
                s->genSatelliteNPC(level_obj.getNPCIDCounter());
                msgeAdd("A hostile " + level_obj.getBeing(s->getSpawnLoc())->getName() + " suddenly appears!",colorpair_red_black);
            }
            else
            {
                msgeAdd("The archway glows momentarily...",colorpair_gray_black);
            }
            return true;
        }
        msgeAdd("Aborting...",colorpair_gray_black);
        updateScreen();
    }

    return false;
}

void game::useSatellitePlayer(satellite *s)
{
    s->drawSatelliteMonitor();
    do
    {
            updateScreen();
            ibuf.setAction();
            switch(ibuf.getAction())
            {
            case(INP_TOGGLE):
                if(sendSatelliteSignal(s))
                {
                   player_has_moved = true;
                   reDisplayAll();
                   player_has_moved = false;
                   return;
                }
                break;
            case(INP_DELTA):
                s->switchMode();
                s->drawSatelliteGraphic();
                break;
            case(INP_SELECT):
                break;
            default:
                msgeAdd("Invalid command.",colorpair_gray_black);
                break;
            }

    }while(ibuf.getAction() != INP_SELECT);
    player_has_moved = true;
    reDisplayAll();
    player_has_moved = false;
}

bool game::useFeatureAtPlayer(point loc)
{
    if (getMap()->getTerrain(loc) == SLOT_MACHINE)
        useMachinePlayer(level_obj.getSlotMachine(loc));
    else if (getMap()->getTerrain(loc) == DIAMONDS_MACHINE)
        useMachinePlayer(level_obj.getDiamondMachine(loc));
    else if (getMap()->getTerrain(loc) == SATELLITE_TILE)
        useSatellitePlayer(level_obj.getSatellite(loc));
    else
        return false;

    return true;
}

void game::showLevelFeature(point loc)
{
    switch(getMap()->getTerrain(loc))
    {
    case(SLOT_MACHINE):
        level_obj.getSlotMachine(loc)->drawMachine();
        break;
    case(DIAMONDS_MACHINE):
        level_obj.getDiamondMachine(loc)->drawMachine();
        break;
    default:
        break;
    }
}

chtype getExactSymbolFromNPC(being *b)
{
    if (b->getType() != PROCGEN_NPC)
        return symbol_NPC[(int)b->getType()];

    chtype custom_npc_sym = {{b->getFactoryColor(),color_black},b->getFactoryAscii()};

    return custom_npc_sym;
}

void game::checkForStunEvent(being *attacker, being *target, int dam, bool is_melee)
{
    if (attacker->getInv()->getWeapon()->getType() == NIL_i)
        return;

    if (attacker->getInv()->getWeapon()->getStats()->category == ITEM_FIREARM_WEAPON &&
        is_melee)
        return;

    if (PERCCHANCE() > attacker->getInv()->getWeapon()->getStats()->wepstats.stun_chance ||
        target->getStats()->is_stun_immune)
        return;

    int stagger_count = 4 + (int)ROLL(2) + (int)(dam/10);

    target->setStaggerCount(stagger_count+target->getStaggerCount());

    if (target->getType() == PLAYER)
    {
        player_quick_fire_spot = getPlayer()->at();
        msgeAdd("You are stunned!",colorpair_white_black);
    }
    else
        NPCMessage(target,actionToMessage(target,"is stunned!"),false);
}

int getArmorProtectionValue(being * attacker, being * target, bool is_melee, bool is_explosion, bool is_circ_deathray)
{
    int ret_val = 0;

    if (is_circ_deathray)
        return ret_val;

    ret_val += target->getInv()->getArmor()->getStats()->armstats.basepv_value;

    item_t wep = attacker->getInv()->getWeapon()->getType();

    int metal_val = target->getInv()->getArmor()->getStats()->armstats.metallic_bonus;
    int energy_val = target->getInv()->getArmor()->getStats()->armstats.energy_bonus;

    if (!is_melee)
    {
        if (!is_explosion)
        {
            if (isFirearmMatchMetallicArmor(wep))
                ret_val += metal_val;

            if (isFirearmMatchEnergyArmor(wep))
                ret_val += energy_val;
        }
        else
            ret_val += energy_val;
    }
    else
    {
        if (attacker->getInv()->getWeapon()->getStats()->category == ITEM_MELEE_WEAPON)
        if (isMeleeWeaponMatchMetallicArmor(wep))
            ret_val += metal_val;
    }

    return ret_val;
}

STRING getGenderPronoun(being_gender gen)
{
    switch(gen)
    {
        case(BEING_GENDER_MALE):
            return "his";
        case(BEING_GENDER_FEMALE):
            return "her";
        default:
            return "its";
    }

    return "his";
}

STRING getDeterminer(STRING name)
{
    // check string is not empty
    char c = ((int)name.size() > 0 ? name[0] : ' ');
    if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u')
        return " an ";
    return " a ";
}
