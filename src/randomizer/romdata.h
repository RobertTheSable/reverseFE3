#pragma once

//$88bb0d - change to $04 so that clerics can cross mountains

#include <cstdint>

// #define NAME_TABLE 0xF08000
// #define CHAPTER_TABLE 0x848C15
// #define REINFORCE_TABLE 0x888000
// #define ALL_REINFORCE_TABLE 0x889D6D
// #define ARMY_TABLE 0x898000
// #define CHARACTER_TABLE 0x889201
// #define ITEM_TABLE 0x88A6E9
// #define CLASS_TABLE 0x88906C
// #define PROMOTION_TABLE 0x83FDD1

// #define UNIT_NAME_TABLE 0x93FD00

// #define WEP_TYPE_TABLE 0x88A4B1
// #define PRF_TABLE 0x88A48E

// #define TALKS_TABLE 0x8B89C6
#define TALK_LENGTH 6
//chapter at 0, character names at 1 and 2
// #define VILLAGE_TABLE 0x8C9179
#define VILLAGE_LENGTH 0xA

#define BATTLE_EVT_LENGTH 0x07
// chapter at 0, character name at 1
// seizes are a type of village event

#define UNIT_LENGTH 0x13
#define STAT_LENGTH 0x11
#define ITEM_LENGTH 14
#define CLASS_LENGTH 9

#define BALLISTA 0x0
#define SWORD 0x1
#define LANCE 0x2
#define BOW 0x4
#define STAFF 0x08
#define MAGIC 0x10
#define ITEM 0x20
#define AXE 0x40 
#define BREATH 0x80
typedef int Weapon_Type;

#define PRF_MIN 0x80

struct Unit_Data{
    uint8_t unit;
    uint8_t unit_class;
    uint8_t level;
    uint8_t name;
    uint8_t y_pos;
    uint8_t x_pos;
    uint8_t mountedClass;
    uint8_t portrait;
    uint8_t weapon1, weapon2, weapon3, weapon4;
    uint8_t item1, item2;
    uint16_t sep2;
    uint8_t ai1, ai2, ai3;
};

struct CharData{
    uint8_t str;
    uint8_t skl;
    uint8_t spd;
    uint8_t lck;
    uint8_t def;
    uint8_t res;
    uint8_t hp;
    uint8_t wlv;
    uint8_t gen;
    uint8_t strGrowth;
    uint8_t sklGrowth;
    uint8_t spdGrowth;
    uint8_t lckGrowth;
    uint8_t defGrowth;
    uint8_t resGrowth;
    uint8_t hpGrowth;
    uint8_t wlvGrowth;
};

struct ClassData{
    uint8_t str;
    uint8_t skl;
    uint8_t spd;
    uint8_t mov;
    uint8_t def;
    uint8_t res;
    uint8_t exp;
    uint8_t sep;
    uint8_t hp;
};

struct ItemData{
    uint8_t type;
    uint8_t icon;
    uint8_t wlv;
    uint8_t atk;
    uint8_t hit;
    uint8_t crt;
    uint8_t weight;
    uint8_t sep1;
    uint8_t uses;
    uint16_t cost;
    uint8_t range;
    uint8_t type_index;
    uint8_t sep3;
};
