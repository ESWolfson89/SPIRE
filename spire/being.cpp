#include "being.h"

being::being()
{
}

being::being(point p, int hp, being_stats b, bool agg, bool act, int id, STRING name)
{
    setBeing(p,hp,b,agg,act,id,name);
}

void being::setBeing(point p, int hp, being_stats b, bool agg, bool act, int id, STRING name)
{
    is_dead = false;
    is_named = false;
    factory_ascii = '@';
    factory_color = color_white;
    loc = p;
    initial_loc = p;
    stats = b;
    hit_points = hp;
    is_aggroed = agg;
    is_activated = act;
    stagger_count = 0;
    being_turn_timer = 0.0;
    pfind_dest = point(1,1);
    being_id = id;
    attack_being_id = -1;
    flee_being_id = -1;
    ally_value = 0;
    experience = 0;
    exp_level = 1;
    stats.name = name;
    gender = BEING_GENDER_MALE;
    if (stats.b_type == ADVERSARY_NPC || stats.b_type == YINAXIMAI_NPC)
    {
        is_named = true;
    }
    if (stats.b_type == SIREN_NPC || stats.b_type == YINAXIMAI_NPC)
    {
        gender = BEING_GENDER_FEMALE;
    }
    if (stats.b_type == ROGUE_NPC)
    {
        initHumanGenderAndName();
    }
    if (stats.b_type == PLAYER)
    {
        ally_value = 1;
    }
}

bool being::isDead()
{
    return is_dead;
}

void being::killBeing()
{
    is_dead = true;
}

int being::getFactoryAscii()
{
    return factory_ascii;
}

void being::setFactoryAscii(int a)
{
    factory_ascii = a;
}

color_type being::getFactoryColor()
{
    return factory_color;
}

void being::setFactoryColor(color_type c)
{
    factory_color = c;
}

void being::setName(STRING new_name)
{
    stats.name = new_name;
}

void being::setExpWorth(int exp)
{
    stats.exp_worth = exp;
}

void being::setAllyValue(int a)
{
    ally_value = a;
}

void being::initHumanGenderAndName()
{
    if (ROLL(4) == 0)
        gender = BEING_GENDER_FEMALE;
    else
        gender = BEING_GENDER_MALE;

    //if (ROLL(1) == 0)
    //    stats.name = genHumanName(gender);
}

void being::incHealing()
{
    stats.healing_ability++;
}

void being::setAttackID(int a)
{
    attack_being_id = a;
}

void being::setFleeID(int f)
{
    flee_being_id = f;
}

void being::setStats(being_stats b)
{
    stats = b;
}

void being::move(point dp)
{
    loc.set(loc.x() + dp.x(), loc.y() + dp.y());
}

void being::moveTo(point dp)
{
    loc.set(dp.x(),dp.y());
}

void being::incAttributes(int s,int d,int a)
{
    stats.strength += s;
    stats.dexterity += d;
    stats.accuracy += a;
    if (stats.strength > 99)
            stats.strength = 99;
    if (stats.dexterity > 99)
            stats.dexterity = 99;
    if (stats.accuracy > 99)
            stats.accuracy = 99;
}

void being::setMaxHP(int new_max)
{
    stats.max_hit_points = new_max;
}

void being::setHP(int new_hp)
{
    hit_points = new_hp;
}

void being::decHP(int decrement)
{
    hit_points -= decrement;
}

void being::incHP(int increment)
{
    hit_points += increment;
    if (hit_points > stats.max_hit_points)
            hit_points = stats.max_hit_points;
}

void being::setHostile(bool h)
{
    stats.is_hostile = h;
}

void being::setAggroFlag(bool a)
{
    is_aggroed = a;
}

void being::setStaggerCount(int sc)
{
    stagger_count = sc;
}

void being::decStaggerCount()
{
    stagger_count--;
}

void being::setActivation(bool a)
{
    is_activated = a;
}

void being::setMobility(bool m)
{
    stats.is_mobile = m;
}

void being::setTurnTimer(float f)
{
    being_turn_timer = f;
}

void being::decrementTurnTimer()
{
    being_turn_timer--;
}

void being::setPathfindDest(point p)
{
    pfind_dest = p;
}

float being::getTurnTimer()
{
    return being_turn_timer;
}

bool being::inTimerRange()
{
    // area of precision (assuming you dont have speeds of 1001, etc...
    return being_turn_timer < 0.0001 && being_turn_timer > -0.0001;
}

bool being::isAggroed()
{
    return is_aggroed;
}

int being::getStaggerCount()
{
    return stagger_count;
}

bool being::isActivated()
{
    return is_activated;
}

bool being::isMobile()
{
    return stats.is_mobile;
}

bool being::isNamed()
{
    return is_named;
}

int being::getHP()
{
    return hit_points;
}

int being::getMaxCarry()
{
    return stats.strength * 125;
}

STRING being::getName()
{
    return stats.name;
}

int being::getID()
{
    return being_id;
}

int being::getAttackID()
{
    return attack_being_id;
}

int being::getFleeID()
{
    return flee_being_id;
}

int being::getAllyValue()
{
    return ally_value;
}

NPC_t being::getType()
{
    return stats.b_type;
}

being_stats *being::getStats()
{
    return &stats;
}

point being::at()
{
    return loc;
}

point being::getInitLoc()
{
    return initial_loc;
}

point being::getPathfindDest()
{
    return pfind_dest;
}

uint64 being::getCredits()
{
    return credits;
}

void being::setCredits(uint64 c)
{
    credits = c;
}

being_inv *being::getInv()
{
    return &inv;
}

being_gender being::getGender()
{
    return gender;
}

void being::incExperience(uint e)
{
    experience += e;
}

void being::incLevel(uint l)
{
    exp_level += l;
}

uint being::getExperience()
{
    return experience;
}

uint being::getLevel()
{
    return exp_level;
}

void being::setID(int id)
{
    being_id = id;
}
