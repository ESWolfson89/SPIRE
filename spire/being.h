#ifndef BEING_H_
#define BEING_H_

#include "item.h"
#include "namegen.h"

// contains info about beings (npcs, player)

struct being_stats
{
    NPC_t b_type;

    int max_hit_points;
    int strength;
    int dexterity;
    int accuracy;
    int stealth;
    int melee_per_turn;
    int exp_worth;
    int frequency;
    int shoot_freq;
    int speed;
    int min_level;
    int danger_level;
    int flee_chance;
    int detect_distance;
    int aggressiveness; // does nothing yet
    int equip_chance;
    int talkativeness;
    int pickup_chance;
    int steal_ability;
    int healing_ability;
    int forgive_chance;
    int teleport_chance;
    int explode_chance;
    int rove_chance;
    int bulldoze_chance;
    int wallin_chance;
    int circulardeathray_chance;

    bool is_mobile;
    bool can_melee;
    bool can_summon;
    bool is_hostile;
    bool wears_armor;
    bool uses_guns;
    bool is_deactivate_immune;
    bool is_stun_immune;

    int carry_chance[19];

    int wear_chance[5];

    STRING name;
};

static const being_stats being_default_stats[NUM_TOTAL_BEINGS]=
{
    {NIL_n          ,0         ,0 ,0 ,0 ,0 ,0,0       ,0 ,0 ,0  ,0 ,0 ,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0,0 ,0,0  ,0 ,0,0,false,false,false,false,false,false,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"NULL BEING"}           ,
    {PLAYER         ,18        ,12,12,12,10,1,0       ,0 ,0 ,100,1 ,1 ,0 ,8 ,0 ,0 ,0  ,0 ,0 ,1 ,0,0 ,0,0  ,0 ,0,0,true ,true ,false,false,true ,true ,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"player"}               ,
    {AGENT_NPC      ,150       ,30,90,35,50,1,90000   ,0 ,90,150,1 ,16,3 ,28,99,12,40 ,0 ,0 ,10,0,0 ,0,0  ,0 ,0,0,true ,true ,false,false,true ,true ,false,true ,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,100},{0,0  ,0  ,0  ,25} ,"megacorporation agent"},
    {ARMSDEALER_NPC ,200       ,50,50,50,50,1,40000   ,0 ,90,100,1 ,15,15,22,50,12,40 ,0 ,0 ,5 ,0,0 ,0,0  ,0 ,0,0,false,true ,false,false,true ,true ,false,true ,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"arms dealer"}          ,
    {GUARD_NPC      ,5         ,8 ,4, 8, 5 ,1,8       ,50,30,100,1 ,1 ,30,7 ,50,20,60 ,25,0 ,3 ,0,0 ,0,10 ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{100,0  ,0  ,0  ,0  ,100,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"guard"}                ,
    {ROGUE_NPC      ,10        ,10,8, 10,10,1,25      ,20,50,100,1 ,2 ,25,15,50,35,15 ,50,0 ,1 ,0,0 ,0,5  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{3  ,3  ,3  ,3  ,3  ,100,3  ,3 ,30 ,3  ,30 ,3  ,3  ,3 ,1  ,3  ,30,3 ,2}  ,{0,0  ,0  ,0  ,100},"rogue"}                ,
    {BUGMAN_NPC     ,15        ,15,4, 6 ,3, 2,75      ,10,0 ,110,1 ,3 ,35,20,50,0, 50 ,0, 0, 6 ,0,0 ,0,30 ,1 ,0,0,true ,true ,false,true ,false,false,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"bugman"}               ,
    {GUNNER_NPC     ,70        ,40,8 ,20,5 ,1,3000    ,2 ,80,80 ,3 ,6 ,25,5 ,65,40,50 ,15,0 ,17,0,0 ,0,4  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,100,100,100,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"gunner"}               ,
    {PIRATE_NPC     ,10        ,5 ,14,20,75,1,150     ,15,75,200,2 ,4 ,90,9 ,75,25,50 ,25,0 ,20,0,0 ,0,15 ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,100,0  ,0  ,0  ,70 ,70 ,0 ,0  ,0  ,0  ,0  ,25 ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"space pirate"}         ,
    {SPIRIT_NPC     ,14        ,10,28, 6,25,1,100     ,9, 0 ,60 ,2 ,5 ,0 ,12,50,0, 50 ,0, 0, 0 ,1,0 ,0,20 ,0 ,0,0,true ,true ,false,true ,false,false,true ,true ,{0  ,0  ,50 ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"spirit"}               ,
    {GOON_NPC       ,18        ,12,12,12,10,1,250     ,9, 45,100,2 ,5 ,50,8 ,50,20,65 ,60,0, 3 ,0,0 ,0,35 ,0 ,0,0,true ,true ,false,true ,false,true, false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,100,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"space goon"}           ,
    {SIREN_NPC      ,7         ,6 ,35,35,50,1,500     ,1 ,85,120,7 ,4 ,95,12,50,5 ,100,0 ,30,10,0,0 ,0,5  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"space siren"}          ,
    {HAZMATGUY_NPC  ,35        ,25,9 ,25,8 ,1,800     ,12,60,100,5 ,6 ,20,8 ,50,15,60 ,3 ,0 ,10,0,0 ,0,5  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,100,0 ,0 ,0}  ,{0,0  ,0  ,100,0}  ,"hazmat guy"}           ,
    {EXECUTIONER_NPC,25        ,25,15,25,45,1,4000    ,4 ,25,100,12,10,3 ,8 ,50,25,40 ,0 ,0 ,20,0,0 ,0,9  ,0 ,0,0,true ,true ,false,true ,false,true ,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,100,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"executioner"}          ,
    {GENERAL_NPC    ,30        ,32,16,32,15,1,10000   ,1 ,70,100,14,12,2 ,12,50,35,25 ,5 ,0 ,5 ,0,0 ,0,1  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,100,0  ,0  ,0  ,100,100,0 ,100,0  ,75 ,0  ,0  ,50,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,100},"federation general"}   ,
    {GIANT_NPC      ,300       ,90,1 ,6 ,1 ,1,20000   ,1 ,0 ,40 ,8 ,9 ,5 ,4 ,40,0 ,20 ,0 ,0 ,20,0,0 ,0,45 ,35,0,0,true ,true ,false,true ,false,false,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"giant"}                ,
    {PRINCE_NPC     ,50        ,50,15,30,1 ,1,7500    ,3 ,15,100,15,10,25,7 ,60,5 ,35 ,10,0 ,15,0,0 ,0,3  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,100,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,100,0  ,0}  ,"war prince"}           ,
    {KING_NPC       ,100       ,50,15,40,1 ,2,30000   ,2 ,15,100,17,12,20,7 ,60,6 ,35 ,13,0 ,17,0,0 ,0,3  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,100,50 ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,100,0  ,0}  ,"war king"}             ,
    {EMPEROR_NPC    ,175       ,50,15,50,1 ,3,100000  ,1 ,15,100,19,14,15,7 ,60,7 ,35 ,16,0 ,19,0,0 ,0,3  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,100,100,50 ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,100,0  ,0}  ,"war emperor"}          ,
    {MAGICIAN_NPC   ,200       ,25,70,45,25,1,150000  ,1 ,50,100,21,16,10,20,60,25,75 ,5 ,2 ,8 ,0,4 ,0,25 ,0 ,0,0,true ,true ,true ,true ,false,true ,false,true ,{12 ,12 ,12 ,12 ,12 ,12 ,12 ,12,12 ,12 ,12 ,12 ,12 ,12,12 ,12 ,12,12,12} ,{0,0  ,0  ,0  ,0}  ,"magician"}             ,
    {JUGGERNAUT_NPC ,450       ,50,25,50,5 ,1,600000  ,1 ,80,100,25,20,0 ,17,30,25,25 ,0 ,0 ,40,0,0 ,0,0  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,100,0 ,0  ,0  ,0 ,0 ,0}  ,{0,100,0  ,0  ,0}  ,"juggernaut"}           ,
    {PROCGEN_NPC    ,1         ,1 ,1 ,1 ,0 ,1,0       ,2 ,0 ,100,1 ,1 ,0 ,0 ,0 ,0 ,0  ,0 ,0 ,0 ,0,0 ,0,0  ,0 ,0,0,true ,true ,false,true ,true ,true ,false,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"NULL PROCGEN"}         ,
    {HOPPER_NPC     ,20        ,10,10,10,10,6,15000   ,1 ,0 ,150,15,25,0 ,12,60,0 ,0  ,0 ,0 ,8 ,0,98,0,25 ,0 ,0,0,true ,true ,false,true ,false,false,true ,false,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"bidimensional hopper"} ,
    {YINAXIMAI_NPC  ,145       ,40,60,99,1 ,1,200000  ,0 ,0 ,70 ,29,25,6 ,20,90,0 ,250,0 ,0 ,7 ,0,0 ,0,20 ,0 ,0,0,true ,true ,false,true ,false,false,true ,true ,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"Yinaximai"}            ,
    {ADVERSARY_NPC  ,1600      ,60,60,60,1 ,4,10000000,0 ,0 ,100,30,30,0 ,30,90,0 ,0  ,0 ,5 ,40,0,7 ,7,100,10,1,3,true ,true ,false,true ,false,false,true ,true ,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"The Adversary"}        ,
    {ELITEGUARD_NPC ,100       ,50,50,50,1 ,1,70000   ,0 ,60,100,1 ,19,5, 10,50,20,100,25,0 ,4 ,0,0 ,0,0  ,0 ,0,0,false,true ,false,true ,true ,true ,false,false,{100,0  ,0  ,0  ,0  ,0  ,0  ,0 ,100,0  ,0  ,0  ,0  ,0 ,0  ,100,0 ,0 ,0}  ,{0,0  ,0  ,0  ,0}  ,"elite guardian"}       ,
    {ELITEKILLER_NPC,35        ,15,10,40,25,2,10000   ,0 ,85,100,29,24,0, 24,100,0,0  ,50,0 ,5 ,0,0 ,0,0  ,0 ,0,0,false,true ,false,true ,true ,true ,false,true ,{0  ,0  ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,100,0,0}  ,{0,0  ,0  ,0,100}  ,"elite killer"}         ,
    {ELITEAGENT_NPC ,160       ,30,90,35,1 ,9,180000  ,0 ,50,150,29,25,0, 30,100,15,40,0 ,0 ,10,0,0 ,0,0  ,0 ,0,0,true, true ,false,true ,true ,true ,true, true ,{0,100  ,0  ,0  ,0  ,100,0  ,0 ,0  ,0  ,0  ,0  ,0  ,0 ,0  ,0  ,0,100,0}  ,{25,0,25  ,0  ,0}  ,"elite agent"}
};

static const being_stats procgen_starthumanally[3] =
{
    {PROCGEN_NPC,   25,  12,  12,  12,  10,  1,   0,   0,   50,   100, 1,   4,   0,   8,   25,   10,   0,   50,   0,   1,   0,  0,  0, 50, 0,   0,   0,true,   true, false,   false,   true,   true,  false, false, {0,0,0,0,0,0,100,0,0,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0}, "human soldier"},
    {PROCGEN_NPC,   25,  12,  12,  12,  10,  1,   0,   0,   50,   100, 1,   4,   0,   8,   25,   10,   0,   50,   0,   1,   0,  0,  0, 50, 0,   0,   0,true,   true, false,   false,   true,   true,  false, false, {0,0,0,0,0,0,0,0,100,0,0,0,0,0,0,0,0,0,0}, {0,0,0,0,0}, "alpha III soldier"},
    {PROCGEN_NPC,   25,  12,  12,  12,  10,  1,   0,   0,   50,   100, 1,   4,   0,   8,   25,   10,   0,   50,   0,   1,   0,  0,  0, 50, 0,   0,   0,true,   true, false,   false,   true,   true,  false, false, {0,0,0,0,0,0,0,0,0,0,100,0,0,0,0,0,0,0,0}, {0,0,0,0,0}, "alpha IV soldier"}
};

static const uint max_NPCs_per_level[30] =
{
    6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,25
};

class being
{
public:

    being();

    being(point, int, being_stats, bool, bool, int, STRING);

    void setBeing(point, int, being_stats, bool, bool, int, STRING);

    void killBeing();

    void setStats(being_stats);

    void setExpWorth(int);

    void incAttributes(int,int,int);

    void setAttackID(int);

    void setFleeID(int);

    void move(point);

    void moveTo(point);

    void setHP(int);

    void incHP(int);

    void decHP(int);

    void setMaxHP(int);

    void setAggroFlag(bool);

    void setStaggerCount(int);

    void decStaggerCount();

    void setActivation(bool);

    void setMobility(bool);

    void setHostile(bool);

    void setID(int);

    void setTurnTimer(float);

    void incHealing();

    void decrementTurnTimer();

    void setPathfindDest(point);

    void setCredits(uint64);

    void initHumanGenderAndName();

    void setName(STRING);

    void setAllyValue(int);

    int getFactoryAscii();

    void setFactoryAscii(int);

    color_type getFactoryColor();

    void setFactoryColor(color_type);

    float getTurnTimer();

    bool inTimerRange();

    bool isAggroed();

    bool isDead();

    int getStaggerCount();

    bool isMobile();

    bool isActivated();

    bool isNamed();

    STRING getName();

    int getHP();

    int getMaxCarry();

    int getID();

    int getAttackID();

    int getFleeID();

    int getAllyValue();

    being_gender getGender();

    NPC_t getType();

    being_stats * getStats();

    point at();

    point getInitLoc();

    point getPathfindDest();

    uint64 getCredits();

    being_inv *getInv();

    uint getExperience();

    uint getLevel();

    void incExperience(uint);

    void incLevel(uint);

private:
    point loc;
    point initial_loc;
    point pfind_dest;
    being_stats stats;
    float being_turn_timer;
    bool is_activated;
    bool is_aggroed;
    bool is_dead;
    bool is_named;
    int stagger_count;
    int hit_points;
    int attack_being_id;
    int flee_being_id;
    int being_id;
    int ally_value;
    color_type factory_color;
    int factory_ascii;
    uint experience;
    uint exp_level;
    being_gender gender;
    uint64 credits;
    being_inv inv;
};

#endif
