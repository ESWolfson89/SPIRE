#ifndef GLOBALS_H_
#define GLOBALS_H_

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <sstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#define TILEWID 16
#define TILEHGT 24

#define BITMAPROWS 16
#define BITMAPCOLS 16

#define GRIDWID 80
#define GRIDHGT 30

#define SCREENWID (TILEWID * GRIDWID)
#define SCREENHGT (TILEHGT * GRIDHGT)

#define MAPWID 56
#define MAPHGT 24

#define RIGHTSIDEWID 24

#define SHOWWID 56
#define SHOWHGT 24

#define HALFWID 12
#define HALFHGT 12

#define NUM_TOTAL_TERRAIN 19
#define NUM_TOTAL_ITEMS 78
#define NUM_TOTAL_FEATURES 1
#define NUM_TOTAL_BEINGS 28
#define NUM_TOTAL_FIRE 38

#define NUM_TOTAL_SLOT_TILES 12

#define WEAPON_OFFSET 12
#define ARMOR_OFFSET 69

#define NUM_TOTAL_WEAPONS 19

#define NUM_TOTAL_ARMORS 5

#define NUM_TOTAL_MELEE_WEAPONS 5

#define STRING std::string
#define VECTOR std::vector
#define MINIMUM std::min
#define MAXIMUM std::max
#define FINDIF std::find_if
#define FIND std::find
#define RANDOM_SHUFFLE std::random_shuffle
#define VECREMOVE std::remove
#define MINELEMENT std::min_element
#define CEILING std::ceil
#define FLOOR std::floor
#define ABSOLUTE std::abs

#define uint unsigned int
#define uint64 unsigned long long int

// color structure
struct color_type
{
    Uint8 r,g,b;
};

struct color_pair
{
    // foreground colors
    color_type fg;
    // background colors
    color_type bg;
};

struct chtype
{
    color_pair color;
    int ascii;
};

struct dice_roll
{
    uint num_dice;
    uint num_sides;
    uint num_bonus;
};

static const color_type color_red = {255,0,0};
static const color_type color_darkred = {160,0,0};
static const color_type color_orange = {255,128,0};
static const color_type color_brownorange = {140,70,0};
static const color_type color_brown = {100,50,0};
static const color_type color_orangered = {255,64,85};
static const color_type color_darkorangered = {200,64,85};
static const color_type color_primarywall = {255,245,255};
static const color_type color_orangeyellow = {200,160,0};
static const color_type color_green = {0,255,0};
static const color_type color_blue = {50,50,255};
static const color_type color_darkblue = {10,10,255};
static const color_type color_purple = {254,0,254};
static const color_type color_darkpurple = {127,0,127};
static const color_type color_cyan = {0,255,255};
static const color_type color_darkcyan = {0,128,128};
static const color_type color_wallcyan = {0,200,200};
static const color_type color_yellow = {255,255,0};
static const color_type color_gold = {189,120,32};
static const color_type color_gray = {128,128,128};
static const color_type color_darkgray = {64,64,64};
static const color_type color_verydarkgray = {45,45,45};
static const color_type color_hostileplayerred = {45,0,0};
static const color_type color_hostileNPCyellow = {45,45,0};
static const color_type color_neutralgreen = {0,45,0};
static const color_type color_black = {0,0,0};
static const color_type color_white = {255,255,255};
static const color_type color_darkgreen = {0,128,0};
static const color_type color_lightred = {255,100,100};
static const color_type color_lightgreen = {128,255,128};
static const color_type color_lightblue = {128,128,255};
static const color_type color_lightgray = {170,170,170};

static const color_pair colorpair_verydarkgray_black = {color_verydarkgray,color_black};
static const color_pair colorpair_darkgray_black = {color_darkgray,color_black};
static const color_pair colorpair_lightgray_black = {color_lightgray,color_black};
static const color_pair colorpair_white_black = {color_white,color_black};
static const color_pair colorpair_gray_black = {color_gray,color_black};
static const color_pair colorpair_red_black = {color_red,color_black};
static const color_pair colorpair_orange_black = {color_orange,color_black};
static const color_pair colorpair_green_black = {color_green,color_black};
static const color_pair colorpair_purple_black = {color_purple,color_black};
static const color_pair colorpair_cyan_black = {color_cyan,color_black};
static const color_pair colorpair_gold_black = {color_gold,color_black};

static const chtype blank_ch = {{color_black,color_black},' '};
static const chtype select_ch = {{color_white,color_verydarkgray},26};

static const chtype symbol_terrain[NUM_TOTAL_TERRAIN] =
{
    { {color_gray, color_black}, (int)'N' },
    { {color_gold, color_black}, (int)'#'},
    { {color_gold, color_black}, (int)'#'},
    { {color_gray, color_black}, (int)'.'},
    { {color_gray, color_black}, (int)'.'},
    { {color_gray, color_black}, (int)'.'},
    { {color_gray, color_black}, (int)'.'},
    { {color_gray, color_black}, (int)'.'},
    { {color_gray, color_black}, (int)'.'},
    { {color_lightgray, color_black}, 92},
    { {color_lightgray, color_black}, (int)'+'},
    { {color_white, color_verydarkgray}, 25},
    { {color_white, color_verydarkgray}, 24},
    // multicolored
    { {color_gray, color_black}, (int) '$'},
    { {color_gray, color_black}, 4},
    { {color_white, color_verydarkgray}, 21},
    { {color_white, color_black}, 239},
    { {color_orange, color_verydarkgray}, 26},
    { {color_orange, color_verydarkgray}, 239}
};

static const chtype symbol_item[NUM_TOTAL_ITEMS] =
{
    { {color_gray, color_black}, (int)'N' },
    { {color_green, color_black}, (int) '$'},
    { {color_blue, color_black}, (int) '$'},
    { {color_cyan, color_black}, (int) '$'},
    { {color_darkcyan, color_black}, (int) '$'},
    { {color_purple, color_black}, (int) '$'},
    { {color_brown, color_black}, (int) '$'},
    { {color_orange, color_black}, (int) '$'},
    { {color_orangered, color_black}, (int) '$'},
    { {color_red, color_black}, (int) '$'},
    { {color_darkred, color_black}, (int) '$'},
    { {color_white, color_black}, (int) '$'},
    { {color_darkgray, color_black}, (int) ')'},
    { {color_lightgray, color_black}, (int) ')'},
    { {color_gray, color_black}, (int) ')' },
    { {color_white, color_black}, (int) ')'},
    { {color_gray, color_black}, (int) '(' },
    { {color_gray, color_black}, 125},
    { {color_darkgray, color_black}, 125},
    { {color_brownorange, color_black}, 125},
    { {color_darkgreen, color_black}, 125},
    { {color_white, color_black}, 125},
    { {color_green, color_black}, 125},
    { {color_brown, color_black}, 125},
    { {color_red, color_black}, 125},
    { {color_orangered, color_black}, 125},
    { {color_verydarkgray, color_black}, 125},
    { {color_orange,color_black}, 125},
    { {color_cyan,color_black}, 125},
    { {color_blue, color_black}, 125},
    { {color_purple, color_black}, 125},
    { {color_gray, color_black}, 124},
    { {color_brownorange, color_black}, 124},
    { {color_darkgreen, color_black}, 124},
    { {color_white, color_black}, 124},
    { {color_green, color_black}, 124},
    { {color_red, color_black}, 124},
    { {color_orangered, color_black}, 124},
    { {color_verydarkgray, color_black}, 124},
    { {color_orange, color_black}, 124},
    { {color_cyan,color_black}, 124},
    { {color_blue, color_black}, 124},
    { {color_gray, color_black}, ';'},
    { {color_darkgray, color_black}, ';'},
    { {color_brownorange, color_black}, ';'},
    { {color_darkgreen, color_black}, ';'},
    { {color_white, color_black}, ';'},
    { {color_green, color_black}, ';'},
    { {color_brown, color_black}, ';'},
    { {color_red, color_black}, ';'},
    { {color_orangered, color_black}, ';'},
    { {color_verydarkgray, color_black}, ';'},
    { {color_orange,color_black}, ';'},
    { {color_cyan,color_black}, ';'},
    { {color_blue, color_black}, ';'},
    { {color_purple, color_black}, ';'},
    { {color_white,color_black}, 34},
    { {color_red,color_black}, 34},
    { {color_green, color_black}, 34},
    { {color_blue, color_black}, 34},
    { {color_red,color_black},'+'},
    { {color_blue,color_black},'+'},
    { {color_purple,color_black},'+'},
    { {color_white, color_black}, '?'},
    { {color_orange, color_black}, '?'},
    { {color_cyan, color_black}, '?'},
    { {color_green, color_black}, '?'},
    { {color_purple, color_black}, '?'},
    { {color_red, color_black}, '?'},
    { {color_gray, color_black}, ']'},
    { {color_darkgray,color_black}, ']'},
    { {color_white, color_black}, ']'},
    { {color_orange, color_black}, ']'},
    { {color_darkgreen, color_black}, ']'},
    { {color_darkgray, color_black}, '('},
    { {color_blue, color_black}, 125},
    { {color_purple, color_black}, ']'},
    { {color_gray, color_black}, ']'}
};

static const chtype symbol_feature[NUM_TOTAL_FEATURES] =
{
    { {color_gray, color_black}, (int)'N' }
};

static const chtype symbol_NPC[NUM_TOTAL_BEINGS] =
{
    { {color_gray, color_black}, (int)'N'},
    { {color_gray, color_verydarkgray}, (int)'@'},
    { {color_purple, color_black}, (int)'@'},
    { {color_darkgray, color_black}, (int)'@'},
    { {color_white, color_black}, (int)'o' },
    { {color_orangeyellow, color_black}, (int)'@' },
    { {color_darkgray, color_black}, (int)'b'},
    { {color_lightgray, color_black}, (int)'G'},
    { {color_red, color_black}, (int)'p'},
    { {color_lightgray, color_black}, (int)'s'},
    { {color_darkgreen, color_black}, (int)'O'},
    { {color_purple, color_black}, (int)'u'},
    { {color_orange, color_black}, (int)'H'},
    { {color_gray, color_black}, (int)'e'},
    { {color_white, color_black}, 1},
    { {color_cyan, color_black}, (int)'0'},
    { {color_darkpurple, color_black}, (int)'X'},
    { {color_darkred, color_black}, (int)'X'},
    { {color_darkcyan, color_black}, (int)'X'},
    { {color_purple, color_black}, 2},
    { {color_red, color_black}, (int)'&'},
    { {color_verydarkgray, color_black},  (int)'@'},
    { {color_purple, color_black}, 1},
    { {color_blue, color_black}, (int)'@'},
    { {color_darkgray, color_black}, (int)'A'},
    { {color_orange, color_black}, 2},
    { {color_darkgray, color_black}, (int)'1'},
    { {color_darkpurple, color_black}, (int)'@'}
};

static const chtype symbol_fire[NUM_TOTAL_FIRE] =
{
    { {color_gray, color_black}, (int)'N' },
    { {color_darkgray, color_black}, (int)'X'},
    { {color_darkgray, color_black}, (int)'*'},
    { {color_gray, color_black}, 179},
    { {color_gray, color_black}, 196},
    { {color_gray, color_black}, (int)'/'},
    { {color_gray, color_black}, 92},
    { {color_darkgray, color_black}, 179},
    { {color_darkgray, color_black}, 196},
    { {color_darkgray, color_black}, (int)'/'},
    { {color_darkgray, color_black}, 92},
    { {color_brownorange, color_black}, 179},
    { {color_brownorange, color_black}, 196},
    { {color_brownorange, color_black}, (int)'/'},
    { {color_brownorange, color_black}, 92},
    { {color_red, color_black}, 179},
    { {color_red, color_black}, 196},
    { {color_red, color_black}, (int)'/'},
    { {color_red, color_black}, 92},
    { {color_white, color_black}, 179},
    { {color_white, color_black}, 196},
    { {color_white, color_black}, (int)'/'},
    { {color_white, color_black}, 92},
    { {color_orangered, color_black}, 179},
    { {color_orangered, color_black}, 196},
    { {color_orangered, color_black}, (int)'/'},
    { {color_orangered, color_black}, 92},
    { {color_verydarkgray, color_black}, 179},
    { {color_verydarkgray, color_black}, 196},
    { {color_verydarkgray, color_black}, (int)'/'},
    { {color_verydarkgray, color_black}, 92},
    { {color_gray, color_black}, '*'},
    { {color_darkgray, color_black}, '*'},
    { {color_red, color_black}, '*'},
    { {color_orange, color_black}, '*'},
    { {color_verydarkgray, color_black}, '*'},
    { {color_blue, color_black}, '*'},
    { {color_purple, color_black}, '*'}
};

// terrain types
enum terrain_t
{
    // NULL value
    NIL_t,
    // Indestructable wall
    WALL_INDESTRUCT,
    // Destructable wall
    WALL_DESTRUCT,
    FLR_HOTSPOT,
    // Generic floor tile
    FLR_GEN,
    // (multicolored) slot machine cell tile
    FLR_DEALERMARKER,
    FLR_VAULTMARKER,
    FLR_CASINOMARKER,
    FLR_MACHINEMARKER,
    OPEN_UNLOCKED_DOOR,
    CLOSED_UNLOCKED_DOOR,
    PREV_STAIRS,
    NEXT_STAIRS,
    SLOT_MACHINE,
    DIAMONDS_MACHINE,
    SATELLITE_TILE,
    SATELLITE_SPAWN_TILE,
    SIDEWAYS_ELEVATOR,
    BLOCKED_ARCHWAY
};

enum item_t
{
    // NULL value
    NIL_i,

    // credits
    CREDIT,
    CREDIT_2,
    CREDIT_5,
    CREDIT_10,
    CREDIT_25,
    CREDIT_100,
    CREDIT_1K,
    CREDIT_10K,
    CREDIT_100K,
    CREDIT_1M,
    CREDIT_1B,
    // melee weapons
    METAL_ROD,
    COMBAT_KNIFE,
    SWORD_GEN,
    SWORD_DIAMOND,
    SWORD_GIANT,
    // ranged weapons
    PISTOL_10MM,
    PISTOL_STEALTH,
    REVOLVER,
    ASSAULT_RIFLE,
    CHAINGUN,
    COMBAT_SHOTGUN,
    DOUBLEBARRELLED_SHOTGUN,
    ROCKET_LAUNCHER,
    LASER_GUN,
    DEATHRAY_GUN,
    FLAMETHROWER,
    SNIPER_RIFLE,
    PLASMA_CANNON,
    PULSE_CANNON,
    // ammo
    HANDGUN_AMMO_GEN,
    REVOLVER_AMMO,
    ARIFLE_AMMO,
    CHAINGUN_AMMO,
    SHOTGUN_AMMO,
    LAUNCHER_AMMO,
    LASER_AMMO,
    DEATHRAY_AMMO,
    FLAMETHROWER_AMMO,
    SNIPER_AMMO,
    PLASMA_AMMO,
    PISTOL_10MM_UPGRADE,
    PISTOL_STEALTH_UPGRADE,
    REVOLVER_UPGRADE,
    ASSAULT_RIFLE_UPGRADE,
    CHAINGUN_UPGRADE,
    COMBAT_SHOTGUN_UPGRADE,
    DOUBLEBARRELLED_SHOTGUN_UPGRADE,
    ROCKET_LAUNCHER_UPGRADE,
    LASER_GUN_UPGRADE,
    DEATHRAY_GUN_UPGRADE,
    FLAMETHROWER_UPGRADE,
    SNIPER_RIFLE_UPGRADE,
    PLASMA_CANNON_UPGRADE,
    PULSE_CANNON_UPGRADE,
    HEALTH_BOOSTER,
    STRENGTH_BOOSTER,
    DEXTERITY_BOOSTER,
    ACCURACY_BOOSTER,
    SMALL_MEDKIT,
    LARGE_MEDKIT,
    GIANT_MEDKIT,
    DRILL_DEVICE,
    MAPPING_DEVICE,
    NPCDETECTOR_DEVICE,
    ITEMDETECTOR_DEVICE,
    TELEPORT_DEVICE,
    TIMESTOPPER_DEVICE,
    GRAPHENE_ARMOR,
    GOLIATH_ARMOR,
    DIAMOND_ARMOR,
    HAZMAT_ARMOR,
    BALLISTIC_ARMOR,
    // artifacts, non-naturally occurring items
    ANCIENT_FLAIL,
    FUSION_CANNON,
    NUCLEAR_ARMOR,
    DEBUG_ARMOR
};

enum feature_t
{
    // NULL value
    NIL_r
};

enum NPC_t
{
    // NULL value
    NIL_n,
    // PLAYER type (user controlled)
    PLAYER,
    // NPCs
    AGENT_NPC,
    ARMSDEALER_NPC,
    GUARD_NPC,
    ROGUE_NPC,
    BUGMAN_NPC,
    GUNNER_NPC,
    PIRATE_NPC,
    SPIRIT_NPC,
    GOON_NPC,
    SIREN_NPC,
    HAZMATGUY_NPC,
    EXECUTIONER_NPC,
    GENERAL_NPC,
    GIANT_NPC,
    PRINCE_NPC,
    KING_NPC,
    EMPEROR_NPC,
    MAGICIAN_NPC,
    JUGGERNAUT_NPC,
    PROCGEN_NPC,
    HOPPER_NPC,
    YINAXIMAI_NPC,
    ADVERSARY_NPC,
    ELITEGUARD_NPC,
    ELITEKILLER_NPC,
    ELITEAGENT_NPC
};

enum fire_t
{
    // NULL value
    NIL_f,
    CURSOR_GEN,
    CURSOR_FIREPATH,
    BULLET_SPRITEV,
    BULLET_SPRITEH,
    BULLET_SPRITEDIAG1,
    BULLET_SPRITEDIAG2,
    BULLET2_SPRITEV,
    BULLET2_SPRITEH,
    BULLET2_SPRITEDIAG1,
    BULLET2_SPRITEDIAG2,
    BULLET3_SPRITEV,
    BULLET3_SPRITEH,
    BULLET3_SPRITEDIAG1,
    BULLET3_SPRITEDIAG2,
    BULLET4_SPRITEV,
    BULLET4_SPRITEH,
    BULLET4_SPRITEDIAG1,
    BULLET4_SPRITEDIAG2,
    BULLET5_SPRITEV,
    BULLET5_SPRITEH,
    BULLET5_SPRITEDIAG1,
    BULLET5_SPRITEDIAG2,
    BULLET6_SPRITEV,
    BULLET6_SPRITEH,
    BULLET6_SPRITEDIAG1,
    BULLET6_SPRITEDIAG2,
    BULLET7_SPRITEV,
    BULLET7_SPRITEH,
    BULLET7_SPRITEDIAG1,
    BULLET7_SPRITEDIAG2,
    SHOTGUNPELLET_SPRITE1,
    SHOTGUNPELLET_SPRITE2,
    REDFIRE_SPRITE,
    ORANGEFIRE_SPRITE,
    DARKFIRE_SPRITE,
    BLUEPLASMA_SPRITE,
    PURPLEPLASMA_SPRITE
};


#endif
