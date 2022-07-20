#pragma once
#include "romdata.h"
#include "../addresser/addresser.h"

#include <fstream>
#include <iostream>
#include <cstring>
#include <cstdint>
#include <vector>
#include <map>
#include <string>
#include <optional>

#define RANDOM_THIEFS 1
#define RANDOM_SPECIAL 2
#define KEEP_LIMIT 4
#define NON_PLAYER_CLASSES 8
#define INCLUDE_TEMP 16
#define BOOK_MATCH 32
#define GENDER_LOCK 64

enum Promotion_Type {NO_PROMOTION, PROMOTES, PROMOTED};
enum Class_Type {NORMAL, THIEF, SPECIAL, DANCER, ENEMY, TEMP, DRAGON, RESTRICTED};

struct DeathQuote {
    int chapter;
    uint8_t textIndex[2];
};

struct MysteryCharacter{
        uint8_t name_id;
        std::string name;
        uint8_t book1_portrait;
        uint8_t book2_portrait;
        CharData book1_stats;
        CharData book2_stats;
        uint8_t book1_class;
        uint8_t book1_new_class;
        uint8_t book2_class;
        uint8_t book2_new_class;
        uint8_t book1_level;
        uint8_t book2_level;
        uint8_t book1_replacement;
        uint8_t book2_replacement;
        uint8_t book1_unit;
        uint8_t book2_unit;
        bool book1_set;
        bool book2_set;
        unsigned short prf_item;
        int join_point[2];
        std::optional<DeathQuote> quotes[2];
};

struct MysteryClass{
    Promotion_Type promoted;
    Class_Type classType;
    std::string name;
    bool mounted;
    Weapon_Type weaponType;
    ClassData class_bases;
    int count_1, count_2;
};

struct MysteryItem{
    uint8_t item_id;
    std::string name;
    //this is for prfs that are in character inventories, as opposed to who can use them in general
    ItemData item_stats;
};

 
class Mystery{
public:
    typedef int sortmode;
    static const sortmode sort_reverse = 1;
    static const sortmode sort_random = 2;
    static const sortmode sort_sync = 4;
    static const sortmode sort_reverse_2 = 8;
    static const sortmode sort_random_2 = 16;
    static const sortmode sort_sync_2 = 32;
    static const sortmode treat_same = 64;
    
    Mystery(RomMap& map, const bool& ntextOff = false);
    ~Mystery();
    void SortCharacters(const sortmode& mode);
    void SetClasses(const int& mode);
    //void SetItems(int mode);
    void SetStats(bool before);
    void Finalize();
    void SaveOutput(const char* output, const char* result, const char* backup = "" );
private:
     bool headered;
     int romSize;
     uint8_t *romData;
     RomMap m_RomData;

     bool noText;
     
     std::map<uint8_t, MysteryCharacter> characters;
     std::vector<uint8_t> book1Order;
     std::vector<uint8_t> book2Order;
     std::map<int, uint8_t> indoor_maps;
     
     // Gaame tracks transformers in peculiar ways
     // Need to have name values for Xane and Tiki
     // Both books needed for future stuff
     uint8_t freelancer_name1;
     uint8_t freelancer_name2;
     uint8_t manakete_name1;
     uint8_t manakete_name2;
     //uint8_t lord_name1;
     //uint8_t lord_name2;
     
     std::vector<MysteryClass> class_list;
     std::vector<MysteryItem> item_list;
     // Check if a unit struct has already been set.
     std::map<unsigned int, bool> already_assigned;
     // Don't want to replace portraits in text multiple times because that could undo the change in some cases
     // So keep track of what's been parsed and what hasn't
     std::map<int, bool> text_parsed;
     
     std::string romFile;
     
     void ChangeCharacter(uint8_t original, uint8_t replacement, int book, bool sync);
     uint8_t getReplacementUnit(uint8_t original);
     void Relevel(uint8_t replacement, uint8_t original,  int promo_level, int book);
     int SetUnits(unsigned int offset, uint8_t replacement, uint8_t original, int book , bool indoor);
     uint8_t ReplaceItem(uint8_t original, Weapon_Type newType, int wlv, int mode = 0);
     uint8_t ReplaceClass(uint8_t original, uint8_t replacement, uint8_t character);
     void ApplyFixes();
     void ParseText(int index, int chapter);
     
    
};
