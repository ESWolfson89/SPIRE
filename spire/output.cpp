#include "output.h"

bool isTile(chtype ct1, chtype ct2)
{
    return (ct1.ascii == ct2.ascii &&
            isColor(ct1.color.fg,ct2.color.fg) &&
            isColor(ct1.color.bg,ct2.color.bg));
}

void addChar(point p, chtype ct)
{
    addGFXObject(ct.color,ct.ascii,p.x(),p.y());
}

void displayEquippedItems(being_inv *inv)
{
    clearRange(2,15,GRIDWID-2,16);

    addGFXString(colorpair_gray_black,"W: ",2,15);
    addGFXString(colorpair_gray_black,"A: ",2,16);

    if (inv->getWeapon()->getType() == NIL_i)
        addGFXString(colorpair_gray_black,"none equipped",5,15);
    else
    {
        addChar(point(5,15),symbol_item[(int)inv->getWeapon()->getType()]);
        displayWeaponStats(inv->getWeapon(),point(7,15));
    }

    if (inv->getArmor()->getType() == NIL_i)
        addGFXString(colorpair_gray_black,"none equipped",5,16);
    else
    {
        addChar(point(5,16),symbol_item[(int)inv->getArmor()->getType()]);
        displayArmorStats(inv->getArmor(),point(7,16));
    }
}

// assumes list is not empty
void printItemList(item_container *itc, int current_selection, bool disp_name)
{
    clearRange(0,1,GRIDWID-2,14);

    // page 0 is first page
    const int page = (int)(current_selection / 10);

    // top item of page
    const int first_item_index = page * 10;

    // size of page to be displayed
    const int page_size = MINIMUM(itc->getNumItems() - first_item_index, 10);

    // how much of that item to display (x NUMBER)
    int item_quantity;

    // item index
    int index = 0;

    item_category ic;

    point print_loc;

    for (int i = 0; i < page_size; ++i)
    {
        index = i + first_item_index;

        item_quantity = itc->getItem(index)->getQuant();

        ic = itc->getItem(index)->getStats()->category;

        if (index == current_selection)
            addChar(point(0,i+1),select_ch);
        else
            addChar(point(0,i+1),blank_ch);

        addChar(point(2,i+1),symbol_item[(int)itc->getItem(index)->getType()]);

        print_loc = point(4,i+1);

        if (ic == ITEM_CREDIT)
            displayCurrencyItemStats(itc->getItem(index), print_loc);
        if (ic == ITEM_MELEE_WEAPON || ic == ITEM_FIREARM_WEAPON)
            displayWeaponStats(itc->getItem(index), print_loc);
        if (ic == ITEM_ARMOR)
            displayArmorStats(itc->getItem(index), print_loc);
        if (ic == ITEM_AMMUNITION || ic == ITEM_WEAPON_UPGRADE || ic == ITEM_STATBOOSTER || ic == ITEM_MEDKIT)
            displayGenericItemStats(itc->getItem(index),print_loc);
        if (ic == ITEM_DEVICE)
            displayDeviceItemStats(itc->getItem(index),print_loc);

        if (ic != ITEM_DEVICE && ic != ITEM_FIREARM_WEAPON && ic != ITEM_MELEE_WEAPON)
            addGFXString(colorpair_gray_black,int2String((uint64)(item_quantity)),70,i+1);
    }

    addGFXString(colorpair_gray_black,"directional keys: browse list",2,18);
    addGFXString(colorpair_gray_black,"[ENTER]: select item",2,19);
    addGFXString(colorpair_gray_black,"[SPACE]: exit",2,20);

    updateScreen();
}

void displayNPCInfo(NPC_t n, being_inv *inv, STRING name, color_type c, int ascii, STRING damage_status)
{
    clearRange(SHOWWID,0,GRIDWID-1,SHOWHGT - 1);

    chtype ct = symbol_NPC[(int)n];

    if (n == PROCGEN_NPC)
    {
        ct.color.fg = c;
        ct.ascii = ascii;
    }

    addChar(point(SHOWWID+1,1),ct);
    addGFXString(colorpair_white_black,name+":",SHOWWID+3,1);

    STRING weapon_name = "", armor_name = "";
    STRING weapon_prefix = "";

    //std::size_t space_loc;

    switch(n)
    {
        case(GUARD_NPC):
            addGFXString(colorpair_lightgray_black,"A short, one-eyed",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"cyborg. It represents",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"what is left of the",SHOWWID+1,4);
            addGFXString(colorpair_lightgray_black,"original inhabitants",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"of the Spire. What",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"was once a lowly",SHOWWID+1,7);
            addGFXString(colorpair_lightgray_black,"guard has been",SHOWWID+1,8);
            addGFXString(colorpair_lightgray_black,"reprogrammed to kill",SHOWWID+1,9);
            addGFXString(colorpair_lightgray_black,"anything except its",SHOWWID+1,10);
            addGFXString(colorpair_lightgray_black,"own kind.",SHOWWID+1,11);
            break;
        case(SPIRIT_NPC):
            addGFXString(colorpair_lightgray_black,"A strange being that",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"manifests as some sort",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"of glowing energy,",SHOWWID+1,4);
            addGFXString(colorpair_lightgray_black,"encapsulated by the",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"outline of what was",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"once an exotic alien.",SHOWWID+1,7);
            break;
        case(GOON_NPC):
            addGFXString(colorpair_lightgray_black,"Uses taunting to try",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"to throw off its",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"victims. It's already",SHOWWID+1,4);
            addGFXString(colorpair_lightgray_black,"getting into your head",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"and you just met the",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"damn thing.",SHOWWID+1,7);
            break;
        case(BUGMAN_NPC):
            addGFXString(colorpair_lightgray_black,"This is what it looks",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"like:",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"  o   o  ",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"   \\__\\  ",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"   (   ) ",SHOWWID+1,7);
            addGFXString(colorpair_lightgray_black,"   (__ )  ",SHOWWID+1,8);
            addGFXString(colorpair_lightgray_black," /((  ) )\\",SHOWWID+1,9);
            addGFXString(colorpair_lightgray_black," |( ##  )| ",SHOWWID+1,10);
            addGFXString(colorpair_lightgray_black,"   \\ _ /   ",SHOWWID+1,11);
            addGFXString(colorpair_lightgray_black,"    | |    ",SHOWWID+1,12);
            break;
        case(HAZMATGUY_NPC):
            addGFXString(colorpair_lightgray_black,"A giant yellow",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"creature in a hazmat",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"suit. Its stature",SHOWWID+1,4);
            addGFXString(colorpair_lightgray_black,"and flamethrower is",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"what makes it look",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"so intimidating.",SHOWWID+1,7);
            addGFXString(colorpair_lightgray_black,"It must be native",SHOWWID+1,8);
            addGFXString(colorpair_lightgray_black,"to a very exotic",SHOWWID+1,9);
            addGFXString(colorpair_lightgray_black,"environment, since",SHOWWID+1,10);
            addGFXString(colorpair_lightgray_black,"it cannot be exposed",SHOWWID+1,11);
            addGFXString(colorpair_lightgray_black,"to the air around",SHOWWID+1,12);
            addGFXString(colorpair_lightgray_black,"it.",SHOWWID+1,13);
            break;
        case(JUGGERNAUT_NPC):
            addGFXString(colorpair_lightgray_black,"A gargantuan creature",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"with armor that looks",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"nearly impenetrable.",SHOWWID+1,4);
            addGFXString(colorpair_lightgray_black,"It is equipped with a",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"rocket launcher that",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"could easily destroy",SHOWWID+1,7);
            addGFXString(colorpair_lightgray_black,"an entire city with a",SHOWWID+1,8);
            addGFXString(colorpair_lightgray_black,"few well-placed shots.",SHOWWID+1,9);
            addGFXString(colorpair_lightgray_black,"At the core of this",SHOWWID+1,10);
            addGFXString(colorpair_lightgray_black,"behemoth is a pulsating",SHOWWID+1,11);
            addGFXString(colorpair_lightgray_black,"red glow that appears",SHOWWID+1,12);
            addGFXString(colorpair_lightgray_black,"constitute all of its",SHOWWID+1,13);
            addGFXString(colorpair_lightgray_black,"biological components.",SHOWWID+1,14);
            addGFXString(colorpair_lightgray_black,"This glow suggests",SHOWWID+1,15);
            addGFXString(colorpair_lightgray_black,"that it is native to a",SHOWWID+1,16);
            addGFXString(colorpair_lightgray_black,"gas giant.",SHOWWID+1,17);
            break;
        case(ADVERSARY_NPC):
            addGFXString(colorpair_lightgray_black,"A tall, shadowy,",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"humanoid figure with a",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"head the shape of an",SHOWWID+1,4);
            addGFXString(colorpair_lightgray_black,"axe. It appears to lean",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"(sporadically) to the",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"left, while other times",SHOWWID+1,7);
            addGFXString(colorpair_lightgray_black,"to the right. Its body",SHOWWID+1,8);
            addGFXString(colorpair_lightgray_black,"is crackling with a",SHOWWID+1,9);
            addGFXString(colorpair_lightgray_black,"darkly colored static",SHOWWID+1,10);
            addGFXString(colorpair_lightgray_black,"discharge as it fades",SHOWWID+1,11);
            addGFXString(colorpair_lightgray_black,"in and out of reality,",SHOWWID+1,12);
            addGFXString(colorpair_lightgray_black,"like some kind of ghost",SHOWWID+1,13);
            addGFXString(colorpair_lightgray_black,"or hologram.",SHOWWID+1,14);
            break;
        case(ELITEGUARD_NPC):
            addGFXString(colorpair_lightgray_black,"A giant, armored",SHOWWID+1,2);
            addGFXString(colorpair_lightgray_black,"humanoid equipped",SHOWWID+1,3);
            addGFXString(colorpair_lightgray_black,"with a space marine's",SHOWWID+1,4);
            addGFXString(colorpair_lightgray_black,"visor helmet. Willing",SHOWWID+1,5);
            addGFXString(colorpair_lightgray_black,"to obliterate anything",SHOWWID+1,6);
            addGFXString(colorpair_lightgray_black,"that tries to sneak",SHOWWID+1,7);
            addGFXString(colorpair_lightgray_black,"its way into that",SHOWWID+1,8);
            addGFXString(colorpair_lightgray_black,"elevator over there.",SHOWWID+1,9);
            break;
        default:
            break;
    }

    addGFXString(colorpair_white_black,damage_status,SHOWWID+1,18);

    if (inv->getWeapon()->getType() != NIL_i)
    {
        weapon_name = item_default_stats[(int)inv->getWeapon()->getStats()->i_type].name;
        weapon_prefix = inv->getWeapon()->getStats()->name;
        if (weapon_prefix == weapon_name)
            addGFXString(colorpair_gray_black,"W: " + weapon_name,SHOWWID+1,19);
        else
        {
            weapon_prefix = weapon_prefix.substr(0,(int)weapon_prefix.size() - (int)weapon_name.size());
            addGFXString(colorpair_gray_black,"W: " + weapon_prefix,SHOWWID+1,19);
            addGFXString(colorpair_gray_black,"   " + weapon_name,SHOWWID+1,20);
        }
    }

    if (inv->getArmor()->getType() != NIL_i)
    {
        armor_name = item_default_stats[(int)inv->getArmor()->getStats()->i_type].name;
        addGFXString(colorpair_gray_black,"A: " + armor_name,SHOWWID+1,21);
    }

    updateScreen();
}

void displayCurrencyItemStats(item *it, point loc)
{
    addGFXString(colorpair_gray_black,it->getStats()->name,loc.x(),loc.y());
}

void displayGenericItemStats(item *it, point loc)
{
    STRING generic_item_name = it->getStats()->name;
    addGFXString(colorpair_gray_black,generic_item_name,loc.x(),loc.y());
}

void displayDeviceItemStats(item *it, point loc)
{
    STRING device_name = it->getStats()->name + " ";
    addGFXString(colorpair_gray_black,device_name + "(" + sint2String(it->getStats()->misc_val) + ")",loc.x(),loc.y());
}

void displayArmorStats(item *it, point loc)
{
    STRING armor_name = it->getStats()->name + " ";
    STRING values_indicator = "DV: " + sint2String(it->getStats()->armstats.basedv_value) + " PV: " +
                                      sint2String(it->getStats()->armstats.basepv_value) + " MPV: " +
                                      sint2String(it->getStats()->armstats.metallic_bonus) + " EPV: " +
                                      sint2String(it->getStats()->armstats.energy_bonus);
    addGFXString(colorpair_gray_black,armor_name + values_indicator,loc.x(),loc.y());
}

void displayWeaponStats(item *it, point loc)
{
    STRING weapon_name = it->getStats()->name + " ";
    STRING damage_indicator = int2String((uint64)it->getStats()->wepstats.dam.num_dice) + "d" +
                              int2String((uint64)it->getStats()->wepstats.dam.num_sides) + "+" +
                              int2String((uint64)it->getStats()->wepstats.dam.num_bonus) + " ";
    STRING tohit_indicator = it->getStats()->wepstats.to_hit > 0 ? ("+" + sint2String(it->getStats()->wepstats.to_hit) + ", ") :
                             (sint2String(it->getStats()->wepstats.to_hit) + ", ");
    STRING load_indicator = (it->getStats()->category == ITEM_FIREARM_WEAPON) ?
                            (int2String((uint64)it->getStats()->num_subitems.quant) + "-" +
                             int2String((uint64)it->getStats()->num_subitems.max_quant)) + " " : "";
    STRING stealth_attrib = (it->getStats()->wepstats.is_stealth == true) ? "STEALTH " : "";
    STRING stun_attrib = (it->getStats()->wepstats.stun_chance > 0) ? ("STUN(" + int2String((uint64)it->getStats()->wepstats.stun_chance) + "\%) ") : "";
    STRING crit_attrib = it->getStats()->wepstats.crit_perc > 0 ? ("CRIT(" + int2String((uint64)it->getStats()->wepstats.crit_multiplier) +
                                                                           "x:" + int2String((uint64)it->getStats()->wepstats.crit_perc) + "\%) ") : "";
    addGFXString(colorpair_gray_black,tohit_indicator + damage_indicator + weapon_name + load_indicator +
                 stealth_attrib + crit_attrib + stun_attrib,loc.x(),loc.y());
}

// create black rectangle on screen (in TILEWID * TILEHGT units)
void clearRange(int x1, int y1, int x2, int y2)
{
    for (int x = x1; x <= x2; ++x)
        for (int y = y1; y <= y2; ++y)
            addChar(point(x,y),blank_ch);
}

mapdisplay::mapdisplay()
{
    dummy = 0;
}

void mapdisplay::printMap(map *m)
{
    point p;

    for (int x = 0; x < MAPWID; ++x)
    for (int y = 0; y < MAPHGT; ++y)
    {
        p.set(x,y);
        printCell(m,p,getSymbol(m,p));
    }
}

void mapdisplay::printCell(map *m, point p, chtype ct)
{
    chtype added;
    chtype dimmed;

    if (!m->getm(p) && m->getFire(p) != CURSOR_GEN)
        added = blank_ch;
    else if (isTile(m->getu(p),blank_ch))
        added = ct;
    else
        added = m->getu(p);

    dimmed = added;
    dimmed.color.fg = getDimmedPresetColor(added.color.fg,2,0);
    addChar(p,(m->getv(p) ? added : dimmed));
}

chtype mapdisplay::getSymbol(map *m, point p)
{
    // print in priority depicted in cell.h
    if (m->getFire(p) != NIL_f)
        return symbol_fire[(int)m->getFire(p)];

    // ...Do NOT print NPCs if not in field of view
    else if (m->getNPC(p) != NIL_n && m->getv(p))
        return symbol_NPC[(int)m->getNPC(p)];

    // feature,items,terrain...
    else if (m->getFeature(p) != NIL_r)
        return symbol_feature[(int)m->getFeature(p)];

    else if (m->getItem(p) != NIL_i)
        return symbol_item[(int)m->getItem(p)];

    return symbol_terrain[m->getTerrain(p)];
}

void printMainMenu()
{
    clearScreen();
    addGFXAltString(colorpair_gold_black,"   #######           ##### ##           #####  #     ##### ###      ##### ##",  1,1,219);
    addGFXAltString(colorpair_gold_black," #       ###      ######  ####       ######  #    ######  # ##   ######  ####", 1,2,219);
    addGFXAltString(colorpair_gold_black,"#         ##     ##   #  #  ###     ##   #  #    ##   #  #  ##  ##   #  # ####",1,3,219);
    addGFXAltString(colorpair_gold_black,"##        #     #    #  #    ###   #    #  #    #    #  #   ## #    #  #   ##", 1,4,219);
    addGFXAltString(colorpair_gold_black," ###                #  #      ##       #  #         #  #    #      #  #",       1,5,219);
    addGFXAltString(colorpair_gold_black,"## ###             ## ##      ##      ## ##        ## ##   #      ## ##",       1,6,219);
    addGFXAltString(colorpair_gold_black," ### ###           ## ##      ##      ## ##        ## ##  #       ## ##",       1,7,219);
    addGFXAltString(colorpair_gold_black,"   ### ###       #### ##      #     #### ##        ## ####        ## ######",   1,8,219);
    addGFXAltString(colorpair_gold_black,"     ### ###    # ### ##     #     # ### ##        ## ##  ###     ## #####",    1,9,219);
    addGFXAltString(colorpair_gold_black,"       ## ###      ## #######         ## ##        ## ##    ##    ## ##",       1,10,219);
    addGFXAltString(colorpair_gold_black,"        ## ##      ## ######     ##   ## ##        #  ##    ##    #  ##",       1,11,219);
    addGFXAltString(colorpair_gold_black,"         # #       ## ##        ###   #  #            #     ##       #",        1,12,219);
    addGFXAltString(colorpair_gold_black,"##        #        ## ##         ###    #         ####      ###  ####",         1,13,219);
    addGFXAltString(colorpair_gold_black," #########         ## ##          ######         #  ####    ##  #  ###########",1,14,219);
    addGFXAltString(colorpair_gold_black,"   #####      ##   ## ##            ###         #    ##     #  #     ######",   1,15,219);
    addGFXAltString(colorpair_gold_black,"             ###   #  #                         #              #",              1,16,219);
    addGFXAltString(colorpair_gold_black,"              ###    #                           ##             ##",            1,17,219);
    addGFXAltString(colorpair_gold_black,"               ######",                                                         1,18,219);
    addGFXAltString(colorpair_gold_black,"                 ###",                                                          1,19,219);
    addGFXString(colorpair_darkgray_black,"                              _  ___",                                         1,20);
    addGFXString(colorpair_darkgray_black,"             |__| _ || _   _ (_   | |_  _   /\\  _|   _ _ _ _  _",             1,21);
    addGFXString(colorpair_darkgray_black,"             |  |(_|||_)  (_)|    | | )(-  /--\\(_|\\/(-| _)(_|| \\/",         1,22);
    addGFXString(colorpair_darkgray_black,"                                                               /",             1,23);
    addGFXString(colorpair_white_black,"                         www.github.com/ESWolfson89/spire         ",              1,24);
    addGFXString(colorpair_white_black,"                                                                  ",              1,25);
    addGFXString(colorpair_white_black,"                    Press a key to start a new game (q to quit)   ",              1,26);
    addGFXString(colorpair_white_black,"                                                                  ",              1,27);
    addGFXString(colorpair_white_black,"                           Copyright Eric Wolfson 2018            ",              1,28);

}

void displayHelpScreen()
{
    addGFXString(colorpair_white_black,"This game is incomplete. Notable unfinished features, etc...:",1,1);
    addGFXString(colorpair_white_black,"1) save/load",1,2);
    addGFXString(colorpair_white_black,"2) win condition",1,3);
    addGFXString(colorpair_white_black,"3) NPC related bug fixes (can lead to instability and crashes",1,4);
    addGFXString(colorpair_white_black,"4) Completed NPC descriptions",1,5);
    addGFXString(colorpair_white_black,"5) Completed NPC dialog",1,6);
    addGFXString(colorpair_white_black,"6) Code cleanup",1,7);
    addGFXString(colorpair_white_black,"7) More enemies and items",1,8);
    addGFXString(colorpair_white_black,"8) Revamped ally mechanism (limit, starting allies, etc..)",1,9);
    addGFXString(colorpair_white_black,"9) Finished casinos",1,10);
    addGFXString(colorpair_white_black,"10) Additional special levels",1,11);
    addGFXString(colorpair_white_black,"11) Other misc bugs and unimplemented features",1,12);
    addGFXString(colorpair_green_black,"Key commands:",1,14);
    addGFXString(colorpair_green_black,"Move - arrows, keypad arrows, yuhjkbn",1,15);
    addGFXString(colorpair_green_black,"attack: t - target, f - quick fire, r - reload",1,16);
    addGFXString(colorpair_green_black,"inventory: i",1,17);
    addGFXString(colorpair_green_black,"casino: space and enter",1,18);
    addGFXString(colorpair_green_black,"buy from dealer (chat): c, then directional key",1,19);
    addGFXString(colorpair_green_black,"elevators: period, comma",1,20);
    addGFXString(colorpair_green_black,"Undocumented in generic item list: a -> select all",1,21);
    addGFXString(colorpair_green_black,"Undocumented in inventory: a -> drop all, d -> drop one",1,22);
    addGFXString(colorpair_green_black,"p - pickup, semicolon - quick pickup",1,23);
    addGFXString(colorpair_green_black,"q - prompt quit, z - prompt debug mode",1,24);
    addGFXString(colorpair_green_black,"\".\" - down elevator, \",\" - up elevator",1,25);
    addGFXString(colorpair_gray_black,"Press [space] to resume",1,27);
}

void mapdisplay::clearAllFireCells(map *m)
{
    for (int x = 0; x < MAPWID; ++x)
        for (int y = 0; y < MAPHGT; ++y)
            m->setFire(point(x,y),NIL_f);
}

void mapdisplay::setAndPrintFire(map *m, point p, fire_t f)
{
    m->setFire(p,f);
    printCell(m,p,getSymbol(m,p));
}
