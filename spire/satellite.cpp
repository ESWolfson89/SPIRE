#include "satellite.h"

satellite::satellite()
{

}

satellite::satellite(point p, point sp, int dl)
{
    loc = p;
    spawn_loc = sp;
    danger_lev = dl;
    distress_mode = true;
}

void satellite::switchMode()
{
    distress_mode = !distress_mode;
}

bool satellite::isDistressMode()
{
    return distress_mode;
}

point satellite::at()
{
    return loc;
}

point satellite::getSpawnLoc()
{
    return spawn_loc;
}

uint64 satellite::getCost()
{
    return cost;
}

void satellite::genSatelliteNPC(int NPC_id_counter)
{
    int d = rollrange(1,(int)MINIMUM(danger_lev,25));

    being_stats procgen_stats = getProcgenStats(d,1);

    chtype procgen_chtype = getProcgenChtype(d,procgen_stats.name);

    spawnable_being = being(spawn_loc,procgen_stats.max_hit_points,procgen_stats,false,false,NPC_id_counter,procgen_stats.name);

    giveBeingWeaponsAmmoArmor(&spawnable_being);

    spawnable_being.setFactoryAscii(procgen_chtype.ascii);
    spawnable_being.setFactoryColor(procgen_chtype.color.fg);

    cost = spawnable_being.getHP() * (spawnable_being.getStats()->strength +
                                      spawnable_being.getStats()->dexterity +
                                      spawnable_being.getStats()->accuracy) *
           spawnable_being.getStats()->speed;
    if (spawnable_being.getStats()->melee_per_turn > 1)
    {
        cost *= spawnable_being.getStats()->melee_per_turn;
    }
    if (spawnable_being.getStats()->is_stun_immune)
    {
        cost *= 3;
    }
    if (spawnable_being.getStats()->steal_ability > 0)
    {
        cost *= 8;
    }
    if (spawnable_being.getStats()->bulldoze_chance > 0)
    {
        cost *= 8;
    }
    if (spawnable_being.getStats()->teleport_chance > 0)
    {
        cost *= 2;
    }
    if (spawnable_being.getStats()->explode_chance > 0)
    {
        cost *= 25;
    }
}

void satellite::drawSatelliteGraphic()
{
    for (int x = 0; x < RIGHTSIDEWID; ++x)
    for (int y = 0; y < SHOWHGT; ++y)
    {
        addGFXObject(colorpair_white_black,satellite_backdrop_array[y][x],x+SHOWWID,y);
    }

    if (distress_mode)
        addGFXString(colorpair_green_black," DISTRESS ",7+SHOWWID,1);
    else
        addGFXString(colorpair_red_black," THREATEN ",7+SHOWWID,1);
}

void satellite::drawSatelliteMonitor()
{
    chtype ct;

    int s_iter = 9;
    int w_iter = 0;

    for (int x = 0; x < SHOWWID; ++x)
    for (int y = 0; y < SHOWHGT; ++y)
    {
        addGFXObject(colorpair_verydarkgray_black,' ',x,y);
    }

    color_pair being_color = {spawnable_being.getFactoryColor(),color_black};

    addGFXString(colorpair_orange_black,"INTELLIGENT LIFEFORM DETECTED: ",1,1);
    addGFXString(colorpair_gray_black,"BEGIN STAT LOG-----------------",1,2);
    addGFXString(colorpair_darkgray_black,"< > " + spawnable_being.getName(),1,3);
    addGFXObject(being_color,spawnable_being.getFactoryAscii(),2,3);
    addGFXString(colorpair_lightgray_black,"HIT: " + sint2String(spawnable_being.getHP()),1,4);
    addGFXString(colorpair_gray_black,"STR: " + sint2String(spawnable_being.getStats()->strength),1,5);
    addGFXString(colorpair_gray_black,"DEX: " + sint2String(spawnable_being.getStats()->dexterity),1,6);
    addGFXString(colorpair_gray_black,"ACC: " + sint2String(spawnable_being.getStats()->accuracy),1,7);
    addGFXString(colorpair_gray_black,"SPD: " + sint2String(spawnable_being.getStats()->speed),1,8);
    if (spawnable_being.getStats()->melee_per_turn > 1)
    {
        addGFXString(colorpair_gray_black,"Multiple melee attacks: " + sint2String(spawnable_being.getStats()->melee_per_turn),1,s_iter);
        s_iter++;
    }
    if (spawnable_being.getStats()->is_stun_immune)
    {
        addGFXString(colorpair_gray_black,"Immune to stun attacks",1,s_iter);
        s_iter++;
    }
    if (spawnable_being.getStats()->steal_ability > 0)
    {
        addGFXString(colorpair_gray_black,"Steals from opponents",1,s_iter);
        s_iter++;
    }
    if (spawnable_being.getStats()->bulldoze_chance > 0)
    {
        addGFXString(colorpair_gray_black,"Tunnels through walls",1,s_iter);
        s_iter++;
    }
    if (spawnable_being.getStats()->teleport_chance > 0)
    {
        addGFXString(colorpair_gray_black,"Is able to teleport",1,s_iter);
        s_iter++;
    }
    if (spawnable_being.getStats()->explode_chance > 0)
    {
        addGFXString(colorpair_gray_black,"Gives off massive shock waves",1,s_iter);
        s_iter++;
    }
    addGFXString(colorpair_gray_black,"-------------------------------",1,s_iter);
    addGFXString(colorpair_gray_black,"ARMED WITH:",1,s_iter+1);
    if (spawnable_being.getInv()->getNumItems() == 0)
    {
        addGFXString(colorpair_darkgray_black,"nothing           ",1,s_iter+2);
    }
    else
    {
        for (int i = 0; i < spawnable_being.getInv()->getNumItems(); ++i)
        {
            if (spawnable_being.getInv()->getItem(i)->getStats()->category == ITEM_FIREARM_WEAPON ||
                spawnable_being.getInv()->getItem(i)->getStats()->category == ITEM_MELEE_WEAPON)
            {
                ct = symbol_item[(int)spawnable_being.getInv()->getItem(i)->getType()];
                addGFXObject(ct.color,ct.ascii,1+w_iter,s_iter+2);
                w_iter++;
            }
        }
    }
    addGFXString(colorpair_gray_black,"ARMOR WORN:",1,s_iter+3);
    if (spawnable_being.getInv()->getArmor()->getType() == NIL_i)
    {
        addGFXString(colorpair_darkgray_black,"none          ",1,s_iter+4);
    }
    else
    {
        ct = symbol_item[(int)spawnable_being.getInv()->getArmor()->getType()];
        addGFXString(ct.color,spawnable_being.getInv()->getArmor()->getStats()->name,1,s_iter+4);
    }
    addGFXString(colorpair_gray_black,"END STAT LOG-------------------",1,s_iter+5);
    addGFXString(colorpair_white_black,"[SPACE] -> abort",1,21);
    addGFXString(colorpair_white_black,"[ENTER] -> send signal",1,22);
    addGFXString(colorpair_white_black,"DIRECTIONAL KEYS -> toggle signal type",1,23);
}

being *satellite::getBeing()
{
    return &spawnable_being;
}

being_stats getProcgenStats(int d, int level_number)
{
    being_stats procgen_stats = being_default_stats[(int)PROCGEN_NPC];

    procgen_stats.max_hit_points = ROLL(9) + d + ROLL(d*25);
    procgen_stats.strength = 6 + ROLL(14) + d + ROLL(d);
    procgen_stats.dexterity = 6 + ROLL(14) + d + ROLL(d);
    procgen_stats.accuracy = 6 + ROLL(14) + d + ROLL(d);
    procgen_stats.stealth = ROLL(100);
    procgen_stats.shoot_freq = 1 + d*2 + ROLL(48);
    procgen_stats.flee_chance = 1 + ROLL(29);
    procgen_stats.detect_distance = 5 + ROLL(d);
    procgen_stats.aggressiveness = 1 + ROLL(74) + ROLL(d);
    procgen_stats.danger_level = d;
    procgen_stats.min_level = 1;
    procgen_stats.equip_chance = MINIMUM(35,(int)(5 + ROLL(10) + ROLL(d)));
    procgen_stats.talkativeness = 0; //10 + ROLL(20) + ROLL(d*2);
    procgen_stats.pickup_chance = 1 + ROLL(48) + ROLL(d*2);
    procgen_stats.healing_ability = 5 + ROLL(d*2 + 10);
    procgen_stats.forgive_chance = 0;
    procgen_stats.rove_chance = 10 + ROLL(90);
    procgen_stats.frequency = level_number + 1;

    if (d > 3 && ROLL(199 - d) == 0)
        procgen_stats.explode_chance = 1 + ROLL(2) + ROLL(ROLL(d));
    else
        procgen_stats.explode_chance = 0;

    if (ROLL(30 - d) == 0)
        procgen_stats.teleport_chance = 1 + ROLL(4) + ROLL(d);
    else
        procgen_stats.teleport_chance = 0;

    if (ROLL(30 - d) == 0)
        procgen_stats.steal_ability = 1 + ROLL(48) + ROLL(d*2);
    else
        procgen_stats.steal_ability = 0;

    if (ROLL(149) == 0)
        procgen_stats.bulldoze_chance = 1 + ROLL(2) + ROLL(d);
    else
        procgen_stats.bulldoze_chance = 0;

    if (ROLL(179 - d) == 0)
        procgen_stats.can_summon = true;
    else
        procgen_stats.can_summon = false;

    if (ROLL(29 - d) == 0)
        procgen_stats.is_stun_immune = true;
    else
        procgen_stats.is_stun_immune = false;

    if (ROLL(159 - d) == 0)
        procgen_stats.is_deactivate_immune = true;
    else
        procgen_stats.is_deactivate_immune = false;

    if (ROLL(1) == 0)
        procgen_stats.speed = MINIMUM(300,(int)(60 + ROLL(6)*10 + ROLL(ROLL(d))*10));
    else
        procgen_stats.speed = 100;

    if (ROLL(1) == 0)
        procgen_stats.melee_per_turn = 1 + ROLL((int)(d/5));
    else
        procgen_stats.melee_per_turn = 1;

    procgen_stats.wallin_chance = procgen_stats.circulardeathray_chance = 0;

    for (int i = 0; i < NUM_TOTAL_WEAPONS; ++i)
    {
        if (ROLL(26-d) == 0)
            procgen_stats.carry_chance[i] = 100;
        else if (ROLL(9) == 0)
            procgen_stats.carry_chance[i] = d + ROLL(74) + 1;
        else
            procgen_stats.carry_chance[i] = 0;
    }

    if (ROLL(2) == 0)
    {
        if (d > 10 && ROLL(6) == 0)
            procgen_stats.wear_chance[0] = ROLL(d)*2+1;
        if (d > 8 && ROLL(7) == 0)
            procgen_stats.wear_chance[1] = ROLL(d)*3+1;
        if (d > 7 && ROLL(4) == 0)
            procgen_stats.wear_chance[2] = ROLL(d)*3+1;
        if (ROLL(1) == 0)
            procgen_stats.wear_chance[3] = d + ROLL(74) + 1;
        if (ROLL(1) == 0)
            procgen_stats.wear_chance[4] = d + ROLL(74) + 1;
    }

    procgen_stats.name = genName(PROCGEN_NPC,MINIMUM(7,3 + (int)ROLL(1) + (int)(ROLL((int)(d/2)))));

    procgen_stats.exp_worth = 10 + ROLL(90) + d*d*d*d*d;

    return procgen_stats;
}

chtype getProcgenChtype(int d, STRING name)
{
    chtype ct;

    ct.color.bg = color_black;
    ct.color.fg = getPresetColor(ROLL(2));

    if (ROLL(1) == 0)
        ct.color.fg = getPresetColor(ROLL(7)+4);

    ct.ascii = (int)'@';

    if (d <= 9)
    {
        if (ROLL(2) == 0)
            ct.ascii = '@';
        else
            ct.ascii = (int)name[0];
    }
    else if (d <= 18)
    {
        if (ROLL(2) == 0)
            ct.ascii = 1;
        else
            ct.ascii = (int)name[0] - 32;
    }
    else if (d <= 23)
    {
        if (ROLL(2) == 0)
            ct.ascii = 2;
        else
            ct.ascii = (int)rollrange((int)'1',(int)'9');
    }
    else if (d == 24)
    {
        ct.ascii = (int)'0';
    }
    else
    {
        ct.ascii = (int)'&';
    }

    return ct;
}

void giveBeingWeaponsAmmoArmor(being *b)
{
    int danger_level = b->getStats()->danger_level;

    item_t weapon = NIL_i;
    item_t ammo = NIL_i;
    item_t armor = NIL_i;

    for (int i = 0; i < NUM_TOTAL_WEAPONS; ++i)
    {
        if (PERCCHANCE() <= (uint)b->getStats()->carry_chance[i])
        {
            weapon = (item_t)(i+WEAPON_OFFSET);
            ammo = weapon2ammo(weapon);
            b->getInv()->addToContainer(getModifiedWeaponCharacteristics(weapon,b->getStats()->danger_level),1);
            if (item_default_stats[(int)weapon].category == ITEM_FIREARM_WEAPON)
                b->getInv()->addToContainer(item_default_stats[(int)ammo],item_default_stats[(int)ammo].default_quantity*danger_level);
        }
    }

    for (int i = 0; i < NUM_TOTAL_ARMORS; ++i)
    {
        if (PERCCHANCE() <= (uint)b->getStats()->wear_chance[i])
        {
            armor = (item_t)(i+ARMOR_OFFSET);
            b->getInv()->setArmor(getModifiedArmorCharacteristics(armor,b->getStats()->danger_level));
            break;
        }
    }
}

dice_roll getModifiedDamageCharacterisitics(item_t weapon, int d_param)
{
    int d = MAXIMUM(1,d_param/5);

    weapon_stats base_weapon_stat = item_default_stats[(int)weapon].wepstats;

    dice_roll base_damage = base_weapon_stat.dam;

    base_damage.num_dice  = (int)MAXIMUM(1,(int)((ROLL((int)((double)base_damage.num_dice*sqrt(d)))+base_damage.num_dice)/2));
    base_damage.num_sides = 2 + (int)ROLL(base_damage.num_sides+d);
    base_damage.num_bonus = (int)rollrange((int)(base_damage.num_bonus/2),base_damage.num_bonus+(d*2));

    return base_damage;
}

item_stats getModifiedArmorCharacteristics(item_t armor, int danger_factor)
{
    item_stats base_armor_stat = item_default_stats[(int)armor];

    switch(armor)
    {
        case(GOLIATH_ARMOR):
            base_armor_stat.armstats.basedv_value -= (((int)danger_factor/2) + 1) * danger_factor * (ROLL(danger_factor) + 1);
            base_armor_stat.armstats.basepv_value += ((ROLL(danger_factor) + 1) * 2) + ROLL(15);
            break;
        case(GRAPHENE_ARMOR):
            base_armor_stat.armstats.basepv_value += ROLL(danger_factor + 5);
            base_armor_stat.armstats.metallic_bonus *= (ROLL((int)(danger_factor/2)) + 1);
            base_armor_stat.armstats.energy_bonus *= (ROLL((int)(danger_factor/2)) + 1);
            break;
        case(DIAMOND_ARMOR):
            base_armor_stat.armstats.basedv_value -= ROLL(danger_factor);
            base_armor_stat.armstats.basepv_value += ROLL(danger_factor) - 3;
            base_armor_stat.armstats.metallic_bonus += (ROLL(4) - 2);
            base_armor_stat.armstats.energy_bonus *= (ROLL((int)(danger_factor/2)) + 1);
            break;
        case(HAZMAT_ARMOR):
            base_armor_stat.armstats.energy_bonus += ROLL(danger_factor/2)*(ROLL(1)+1) + 1;
            break;
        case(BALLISTIC_ARMOR):
            base_armor_stat.armstats.basedv_value -= ROLL((int)(danger_factor/2));
            base_armor_stat.armstats.metallic_bonus += ROLL(danger_factor/2)*(ROLL(1)+1) + 1;
            break;
        default:
            break;
    }

    return base_armor_stat;
}

item_stats getModifiedWeaponCharacteristics(item_t weapon, int danger_factor)
{
    item_stats base_weapon_stat = item_default_stats[(int)weapon];

    base_weapon_stat.wepstats.dam = getModifiedDamageCharacterisitics(weapon,danger_factor);
    base_weapon_stat.wepstats.to_hit = base_weapon_stat.wepstats.to_hit - ROLL(3) + ROLL((int)(danger_factor/2)+3);

    if (base_weapon_stat.category == ITEM_FIREARM_WEAPON)
    {
        base_weapon_stat.num_subitems.max_quant = MAXIMUM(1,(int)rollrange((int)(base_weapon_stat.num_subitems.max_quant/3),
                base_weapon_stat.num_subitems.max_quant + danger_factor));
        if (base_weapon_stat.i_type == DOUBLEBARRELLED_SHOTGUN && base_weapon_stat.num_subitems.max_quant == 1)
            base_weapon_stat.num_subitems.max_quant = 2;
    }
    if (ROLL(49) == 0 && base_weapon_stat.category == ITEM_FIREARM_WEAPON)
        base_weapon_stat.wepstats.is_stealth = true;
    if (ROLL(99 - danger_factor) == 0)
        base_weapon_stat.wepstats.crit_perc += ROLL(danger_factor);
    if (base_weapon_stat.wepstats.crit_perc > 0)
    if (ROLL(199 - danger_factor) == 0)
        base_weapon_stat.wepstats.crit_multiplier += ROLL(ROLL(2)) + 1;
    if (ROLL(199 - danger_factor) == 0)
        base_weapon_stat.wepstats.stun_chance += ROLL(ROLL(danger_factor)) + 1;

    base_weapon_stat.name = getWeaponNameModifier(weapon,base_weapon_stat.wepstats.dam,base_weapon_stat.wepstats.to_hit) + base_weapon_stat.name;

    return base_weapon_stat;
}

STRING getWeaponNameModifier(item_t wt, dice_roll dam, int to_hit)
{
    weapon_stats defwep_stats = item_default_stats[(int)wt].wepstats;

    int default_damage = (defwep_stats.dam.num_dice * defwep_stats.dam.num_sides) + defwep_stats.dam.num_bonus;
    int actual_damage = (dam.num_dice * dam.num_sides) + dam.num_bonus;

    int default_accuracy = defwep_stats.to_hit;
    int actual_accuracy = to_hit;

    int accuracy_difference = actual_accuracy - default_accuracy;
    double damage_factor = (double)actual_damage/(double)default_damage;

    STRING to_hit_name_modifier = getWeaponAccuracyPrefix(accuracy_difference);
    STRING damage_name_modifier = getWeaponDamagePrefix(damage_factor);

    if (to_hit_name_modifier != "")
        to_hit_name_modifier += " ";

    if (damage_name_modifier != "")
    {
        if (to_hit_name_modifier != "")
            damage_name_modifier += ", ";
        else
            damage_name_modifier += " ";
    }

    return damage_name_modifier + to_hit_name_modifier;
}

STRING getWeaponDamagePrefix(double damage_factor)
{
    // set damage prefix
    if (damage_factor <= 0.25)
        return weapon_modifier_strings[0][(int)DMP_USELESS];
    else if (damage_factor <= 0.4)
        return weapon_modifier_strings[0][(int)DMP_FRAIL];
    else if (damage_factor <= 0.55)
        return weapon_modifier_strings[0][(int)DMP_WEAK];
    else if (damage_factor <= 0.7)
        return weapon_modifier_strings[0][(int)DMP_SOFT];
    else if (damage_factor <= 1.5)
        return "";
    else if (damage_factor <= 1.9)
        return weapon_modifier_strings[0][(int)DMP_GREAT];
    else if (damage_factor <= 2.3)
        return weapon_modifier_strings[0][(int)DMP_DEADLY];
    else if (damage_factor <= 2.7)
        return weapon_modifier_strings[0][(int)DMP_LETHAL];
    else if (damage_factor <= 3.1)
        return weapon_modifier_strings[0][(int)DMP_PUNISHING];
    else if (damage_factor <= 3.5)
        return weapon_modifier_strings[0][(int)DMP_DEVASTATING];

    return weapon_modifier_strings[0][(int)DMP_ULTIMATE];
}

STRING getWeaponAccuracyPrefix(int accuracy_difference)
{
    // set accuracy prefix
    if (accuracy_difference <= -18)
        return weapon_modifier_strings[1][(int)AMP_DEFECTIVE];
    else if (accuracy_difference <= -14)
        return weapon_modifier_strings[1][(int)AMP_UNRELIABLE];
    else if (accuracy_difference <= -10)
        return weapon_modifier_strings[1][(int)AMP_UNWIELDY];
    else if (accuracy_difference <= -6)
        return weapon_modifier_strings[1][(int)AMP_INEXACT];
    else if (accuracy_difference <= 5)
        return "";
    else if (accuracy_difference <= 9)
        return weapon_modifier_strings[1][(int)AMP_ACCURATE];
    else if (accuracy_difference <= 13)
        return weapon_modifier_strings[1][(int)AMP_UNERRING];
    else if (accuracy_difference <= 17)
        return weapon_modifier_strings[1][(int)AMP_TARGETING];
    else if (accuracy_difference <= 21)
        return weapon_modifier_strings[1][(int)AMP_PINPOINTING];
    else if (accuracy_difference <= 25)
        return weapon_modifier_strings[1][(int)AMP_INEXORABLE];

    return weapon_modifier_strings[1][(int)AMP_UNSTOPPABLE];
}
