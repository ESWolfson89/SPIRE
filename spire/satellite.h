#ifndef SATELLITE_H_
#define SATELLITE_H_

#include "graphics.h"
#include "being.h"

static const int satellite_backdrop_array[SHOWHGT][RIGHTSIDEWID] =
{
 { 0 , 0 , 0,  0 , 0 , 0 ,218,196,196,196,196,196,196,196,196,196,196,191, 0 , 0 , 0 , 0 , 0 , 0 },
 { 0 , 0 , 0,  0 , 0 ,218,217, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,192,191, 0 , 0 ,248, 0 , 0 },
 { 0 , 0 , 0,  0 ,218,217, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,192,191, 47, 0 , 0 , 0 },
 { 0 , 0 , 0, 218,217, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47,191, 0 , 0 , 0 },
 { 0 , 0 ,218,217, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 45, 45, 45, 45, 45, 45, 47, 0 ,192,191, 0 , 0 },
 { 0 ,218,217, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47,124, 0 , 0 ,192,191, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47, 0 ,124, 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47, 0 , 0 ,124, 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47, 0 , 0 , 0 ,124, 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47, 0 , 0 , 0 , 0 ,124, 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47, 0 , 0 , 0 , 0 , 0 ,124, 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 47, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 },
 { 0 ,179, 0,  0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,179, 0 },
 { 0 ,192,191, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,218,217, 0 },
 { 0 , 0 ,192,191, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,218,217, 0 , 0 },
 { 0 , 0 , 0, 192,191, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,218,217, 0 , 0 , 0 },
 { 0 , 0 , 0,  0 ,192,191, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,218,217, 0 , 0 , 0 , 0 },
 { 0 , 0 , 0,  0 , 0 ,192,191, 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 , 0 ,218,217, 0 , 0 , 0 , 0 , 0 },
 { 0 , 0,  0 , 0 , 0 , 0 ,192,196,196,196,196,196,196,196,196,196,196,217, 0 , 0 , 0 , 0 , 0 , 0 }
};

class satellite
{
public:
    satellite();
    satellite(point,point,int);
    void genSatelliteNPC(int);
    void drawSatelliteMonitor();
    void drawSatelliteGraphic();
    void switchMode();
    bool isDistressMode();
    uint64 getCost();
    being *getBeing();
    point at();
    point getSpawnLoc();
private:
    point loc;
    point spawn_loc;
    int danger_lev;
    bool distress_mode;
    being spawnable_being;
    uint64 cost;
};

being_stats getProcgenStats(int,int);
chtype getProcgenChtype(int,STRING);
void giveBeingWeaponsAmmoArmor(being *);
item_stats getModifiedWeaponCharacteristics(item_t, int);
item_stats getModifiedArmorCharacteristics(item_t, int);
dice_roll getModifiedDamageCharacterisitics(item_t, int);
STRING getWeaponNameModifier(item_t,dice_roll,int);
STRING getWeaponDamagePrefix(double);
STRING getWeaponAccuracyPrefix(int);

#endif
