#ifndef OUTPUT_H
#define OUTPUT_H

#include "map.h"
#include "graphics.h"
#include "item.h"

bool isTile(chtype,chtype);

void addChar(point, chtype);

void clearRange(int,int,int,int);

void printMainMenu();

void printItemList(item_container *, int, bool);

void displayGenericItemStats(item *, point);

void displayCurrencyItemStats(item *, point);

void displayArmorStats(item *, point);

void displayWeaponStats(item *, point);

void displayEquippedItems(being_inv *);

void displayNPCInfo(NPC_t, being_inv *,STRING, color_type, int, STRING);

void displayDeviceItemStats(item *, point);

void displayHelpScreen();

class mapdisplay
{

    public:
        mapdisplay();
        void printCell(map *, point, chtype);
        void printMap(map *);
        void clearAllFireCells(map *);
        void setAndPrintFire(map *, point, fire_t);
        chtype getSymbol(map *,point);
    private:
        int dummy;

};

#endif
