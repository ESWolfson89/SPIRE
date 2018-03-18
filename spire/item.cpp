#include "item.h"

item::item()
{
    quantity = 1;
    stats = item_default_stats[(int)NIL_i];
}

item::item(item_stats i, int q)
{
    quantity = q;
    stats = i;
}

void item::decrementMiscval()
{
    stats.misc_val--;
}

void item::setMiscVal(int m)
{
    stats.misc_val = m;
}

void item::setQuant(int q)
{
    quantity = q;
}

void item::setSubContainerAmount(int new_amount)
{
    stats.num_subitems.quant = new_amount;
}

void item::decrementSubContainerAmount()
{
    stats.num_subitems.quant--;
}

void item::setStats(item_stats new_stats)
{
    stats = new_stats;
}

int item::getQuant()
{
    return quantity;
}

item_t item::getType()
{
    return stats.i_type;
}

item_stats *item::getStats()
{
    return &stats;
}

item_container::item_container()
{
    total_weight = 0;
}

void item_container::addToContainer(item_stats istats, int quant)
{
    int item_position = getPos(&istats);

    if (item_position != -1)
        items[item_position].setQuant(items[item_position].getQuant() + quant);
    else
        items.push_back(item(istats,quant));

    updateWeight();
}

void item_container::removeItem(int i, int q)
{
    if (items[i].getQuant() > q)
        items[i].setQuant(items[i].getQuant() - q);
    else
        items.erase(items.begin() + i);
    updateWeight();
}

void item_container::updateWeight()
{
    int curr_weight = 0;
    for (uint i = 0; i < items.size(); ++i)
        curr_weight += (items[i].getStats()->weight * items[i].getQuant());
    total_weight = curr_weight;
}

void item_container::deleteAllItems()
{
    VECTOR<item>().swap(items);
    total_weight = 0;
}

void item_container::initContainer()
{
    items.clear();
    total_weight = 0;
}

bool item_container::isEmpty()
{
    return (items.size() == 0);
}

int item_container::getTotalWeight()
{
    return total_weight;
}

int item_container::getNumItems()
{
    return items.size();
}

int item_container::getPos(item_stats *s)
{
    for (uint i = 0; i < items.size(); ++i)
        if (itemsEqual(items[i].getStats(), s))
            return i;
    // item not found
    return -1;
}

item *item_container::getItem(int i)
{
    return &items[i];
}

cell_pile::cell_pile(point p)
{
    loc = p;
}

point cell_pile::getLoc()
{
    return loc;
}

being_inv::being_inv()
{
}

item *being_inv::getWeapon()
{
    return &equipped_weapon;
}

item *being_inv::getArmor()
{
    return &equipped_armor;
}

void being_inv::setWeapon(int i)
{
    if (getNumItems() >= i + 1)
    {
        if (equipped_weapon.getType() != NIL_i)
            addToContainer(*(equipped_weapon.getStats()),1);

        equipped_weapon.setStats(*(getItem(i)->getStats()));

        removeItem(i,1);
    }
}

void being_inv::setArmor(int i)
{
    if (getNumItems() >= i + 1)
    {
        if (equipped_armor.getType() != NIL_i)
            addToContainer(*(equipped_armor.getStats()),1);

        equipped_armor.setStats(*(getItem(i)->getStats()));

        removeItem(i,1);
    }
}

void being_inv::setWeapon(item_stats weap_stat)
{
    equipped_weapon.setStats(weap_stat);
}

void being_inv::setArmor(item_stats armor_stat)
{
    equipped_armor.setStats(armor_stat);
}

bool isMeleeWeaponMatchMetallicArmor(item_t wep)
{
    return wep == METAL_ROD ||
           wep == COMBAT_KNIFE ||
           wep == SWORD_GEN ||
           wep == SWORD_DIAMOND ||
           wep == SWORD_GIANT;
}

bool isFirearmMatchMetallicArmor(item_t wep)
{
    return wep == PISTOL_10MM ||
           wep == PISTOL_STEALTH ||
           wep == REVOLVER ||
           wep == COMBAT_SHOTGUN ||
           wep == DOUBLEBARRELLED_SHOTGUN ||
           wep == ASSAULT_RIFLE ||
           wep == CHAINGUN ||
           wep == SNIPER_RIFLE ||
           wep == ROCKET_LAUNCHER;
}

bool isFirearmMatchEnergyArmor(item_t wep)
{
    return wep == FLAMETHROWER ||
           wep == LASER_GUN ||
           wep == PLASMA_CANNON ||
           wep == PULSE_CANNON ||
           wep == FUSION_CANNON;
}

item_t weapon2ammo(item_t wep)
{
    switch (wep)
    {
    case(PISTOL_10MM):
    case(PISTOL_STEALTH):
        return HANDGUN_AMMO_GEN;
    case(REVOLVER):
        return REVOLVER_AMMO;
    case(ASSAULT_RIFLE):
        return ARIFLE_AMMO;
    case(COMBAT_SHOTGUN):
    case(DOUBLEBARRELLED_SHOTGUN):
        return SHOTGUN_AMMO;
    case(CHAINGUN):
        return CHAINGUN_AMMO;
    case(ROCKET_LAUNCHER):
        return LAUNCHER_AMMO;
    case(LASER_GUN):
        return LASER_AMMO;
    case(FLAMETHROWER):
        return FLAMETHROWER_AMMO;
    case(SNIPER_RIFLE):
        return SNIPER_AMMO;
    case(PLASMA_CANNON):
    case(PULSE_CANNON):
    case(FUSION_CANNON):
        return PLASMA_AMMO;
    default:
        break;
    }
    return DEATHRAY_AMMO;
}

bool itemsEqual(item_stats *a, item_stats *b)
{
    if (a->category == ITEM_DEVICE || b->category == ITEM_DEVICE)
        return false;

    if (a->category == ITEM_FIREARM_WEAPON || b->category == ITEM_FIREARM_WEAPON)
        return false;

    if (a->category == ITEM_MELEE_WEAPON || b->category == ITEM_MELEE_WEAPON)
        return false;

    return (a->i_type == b->i_type &&
            a->weight == b->weight &&
            a->name == b->name &&
            a->misc_val == b->misc_val &&
            a->num_subitems.quant == b->num_subitems.quant &&
            a->num_subitems.max_quant == b->num_subitems.max_quant &&
            a->wepstats.dam.num_bonus == b->wepstats.dam.num_bonus &&
            a->wepstats.dam.num_dice == b->wepstats.dam.num_dice &&
            a->wepstats.dam.num_sides == b->wepstats.dam.num_sides &&
            a->wepstats.to_hit == b->wepstats.to_hit &&
            a->wepstats.crit_perc == b->wepstats.crit_perc &&
            a->wepstats.crit_multiplier == b->wepstats.crit_multiplier &&
            a->wepstats.stun_chance == b->wepstats.stun_chance &&
            a->wepstats.is_stealth == b->wepstats.is_stealth &&
            a->armstats.basedv_value == b->armstats.basedv_value &&
            a->armstats.basepv_value == b->armstats.basepv_value &&
            a->armstats.metallic_bonus == b->armstats.metallic_bonus &&
            a->armstats.energy_bonus == b->armstats.energy_bonus );
}
