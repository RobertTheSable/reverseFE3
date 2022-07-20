#include "mystery.h"

using std::cout;
using std::endl;

//TODO: identify character by portrait and name?
//some cutscene characters such as Gotoh in Chapter 17 use the name of a player character

Mystery::Mystery(RomMap& map, const bool& textOff): noText(textOff)
{
    //do initial configuration to assign names and set character order
    std::ifstream charCfg("characters.ini");
    std::string temp;
    int mode = 0;
    int book = 0;
    while(charCfg >> temp)
    {
        if( temp == "[Book1]" )
        {
            book = 1;
        }
        else if( temp == "[Book2]")
        {
            book = 2;
        }
        else if( temp == "[Classes]" )
        {
            mode = 1;
        }
        else if( temp == "[Items]" )
        {
            mode = 2;
        }
        else if( temp == "[Chapters]" )
        {
            mode = 3;
        }
        else 
        {
            if(mode == 0)
            {
                unsigned name_id = 0xFF;
                sscanf(temp.c_str(), "%x", &name_id);
                //std::cout << "0x" << std::hex << name_id << std::endl;
                if( charCfg >> temp && characters.count(name_id) == 0)
                {
                    //int join_pt;
                    MysteryCharacter tempChar;
                    tempChar.name_id = name_id;
                    tempChar.name = temp;
                    tempChar.book1_portrait = 0xFF;
                    tempChar.book2_portrait = 0xFF;
                    tempChar.book1_class = 0xFF;
                    tempChar.book2_class = 0xFF;
                    tempChar.book1_new_class = 0xFF;
                    tempChar.book2_new_class = 0xFF;
                    tempChar.book1_level = 0xFF;
                    tempChar.book2_level = 0xFF;
                    tempChar.book1_stats = {0,0,0,0xFF,0,0,0,0xFF,0,0,0,0,0,0,0,0,0};
                    tempChar.book2_stats = {0,0,0,0xFF,0,0,0,0xFF,0,0,0,0,0,0,0,0,0};
                    tempChar.book1_replacement = name_id;
                    tempChar.book2_replacement = name_id;
                    tempChar.book1_unit = 0xFF;
                    tempChar.book2_unit = 0xFF;
                    tempChar.book1_set = false;
                    tempChar.book2_set = false;
                    tempChar.prf_item = 0;
                    characters[name_id] = tempChar;
                    
                }
                if( book == 1)
                {
                    book1Order.push_back(name_id);
                }
                else if( book == 2 )
                {
                    book2Order.push_back(name_id);
                }
                charCfg >> characters[name_id].join_point[book - 1];
            }
            else if(mode == 1)
            {
                charCfg.get();
                if( getline(charCfg, temp) )
                {
                    MysteryClass tClass;
                    tClass.name = temp;
                    class_list.push_back(tClass);
                }
            }
            else if(mode == 2)
            {
                unsigned item_id = 0xFF;
                sscanf(temp.c_str(), "%x", &item_id);
                charCfg.get();
                if( getline(charCfg, temp) )
                {
                    MysteryItem tItem;
                    tItem.item_id = item_id;
                    tItem.name = temp;
                    item_list.push_back(tItem);
                }
            }
            else if(mode == 3)
            {
                int chapter_num;
                unsigned int char_id;
                sscanf(temp.c_str(), "%d", &chapter_num);
                charCfg >> std::hex >> char_id;
                indoor_maps[chapter_num] = (uint8_t) char_id;
            }
        }
    }
    
    charCfg.close();
    
    freelancer_name1 = 0x28;
    manakete_name1 = 0x1C;
    //lord_name1 = 0;
    freelancer_name2 = 0x28;
    manakete_name2 = 0x1C;
    //lord_name2 = 0;
    
//     romFile = rfile;
//     std::ifstream rom(romFile.c_str(), std::ios::binary | std::ios::ate);
    m_RomData = map;
    romData = &(m_RomData[0x808000]);
    headered =  m_RomData.isHeadered();
    romSize = m_RomData.getSize();
    
//     headered = (rom.tellg() % 1024) == 512 ;
//     romSize = rom.tellg() - (rom.tellg() % 1024);
//     rom.seekg(LoROMToPC(RomMap::REVISION, headered));
//     revision = rom.get();
//     m_RomMap = RomMap("");

//     rom.seekg(m_RomMap.getPCAddress("ITEM_TABLE", headered));
    // load items
    int temp_offset = m_RomData.getAddress("ITEM_TABLE");
    for(int i = 0; i < 128; i++)
    {
        ItemData tempItem;
        m_RomData.read((uint8_t*)&tempItem, temp_offset, ITEM_LENGTH);
        temp_offset += ITEM_LENGTH;
        //rom.read((char*)&tempItem, ITEM_LENGTH );
        item_list[i].item_stats = tempItem;
    }
    
    
    // load classes
    for(int i = 0; i < 45; i++)
    {
        temp_offset = m_RomData.getAddress("CLASS_TABLE") + (i * CLASS_LENGTH);
        ClassData tempClass;
        //rom.read((char*)&tempClass, CLASS_LENGTH );
        m_RomData.read((uint8_t*)&tempClass, temp_offset, CLASS_LENGTH);
        MysteryClass tClass = class_list[i];
        tClass.class_bases = tempClass;
        // Determine if class promotes.
        tClass.promoted = NO_PROMOTION;
        if(i == 0x1B)
        {
            tClass.promoted = PROMOTED;
        }
        else
        {
            temp_offset = m_RomData.getAddress("PROMOTION_TABLE");
            while( m_RomData[temp_offset] != 0xFF)
            {
                uint8_t promo_entry =  m_RomData[temp_offset];
                temp_offset++;
                if(promo_entry == i)
                {
                    tClass.promoted = PROMOTES;
                }
                promo_entry =  m_RomData[temp_offset];
                temp_offset++;
                if(promo_entry == i)
                {
                    tClass.promoted = PROMOTED;
                }
            }
        }
        if(i >= 0x26 || i == 0x10 )
        {
            tClass.classType = DRAGON;
        }
        else if(i == 0x23 || i == 0x05 || i == 0x06)
        {
            tClass.classType = RESTRICTED;
        }
        else if(i >= 0x1D)
        {
            tClass.classType = TEMP;
        }
        else if(i == 0x02)
        {
            tClass.classType = ENEMY;
        }
        else if(i == 0x13)
        {
            tClass.classType = THIEF;
        }
        else if(i == 0x0F || i == 0x14  || i == 0x1C)
        {
            tClass.classType = SPECIAL;
        }
        else if(i == 0x15)
        {
            tClass.classType = DANCER;
        }
        else
        {
            tClass.classType = NORMAL;
        }
        // is mov is over 8 and it's not a wyvern, then the class is mounted
        tClass.mounted = (tempClass.mov >= 8 && i != 0x28);
        tClass.weaponType = BALLISTA;
        //determine weapon type
        int wepTypes[] = {0x0, 0x0F, 0x18, 0x40, 0x2D, 0x57, 0x1E, 0x29};
        for(int j = 0; j < 8; j++)
        {
            unsigned int weapon_address = 0;
            temp_offset = m_RomData.getAddress("WEP_TYPE_TABLE") + (wepTypes[j] * 2);
            //rom.read((char*)&weapon_address, 2);
            m_RomData.read((uint8_t*)&weapon_address, temp_offset, 2);
            temp_offset = weapon_address + 0x880000;
            //rom.seekg(LoROMToPC(weapon_address + 0x880000, headered));
            while(m_RomData[temp_offset] != 0xFF)
            {
                if(m_RomData[temp_offset] == i)
                {
                    int test = 1;
                    test <<= j;
                    tClass.weaponType |= test;
                }
                temp_offset++;
            }
            tClass.weaponType &= 0xDF;
        }
        tClass.count_1 = 0;
        tClass.count_2 = 0;
        class_list[i] = tClass;
    }
    
    // load units
    for(int i = 1; i <= 44; i++)
    {
         
        int count = 0;
        uint8_t chapter_index = 0x00;
        //rom.seekg(m_RomData.getPCAddress("CHAPTER_TABLE", headered)+i);
        temp_offset = m_RomData.getAddress("CHAPTER_TABLE")+i;
        m_RomData.read((uint8_t*)&chapter_index, temp_offset , 1 );
        unsigned short chapter_reinforcements = 0;
        //rom.seekg(m_RomData.getPCAddress("REINFORCE_TABLE", headered) + (chapter_index * 2));
        temp_offset = m_RomData.getAddress("REINFORCE_TABLE") + (chapter_index * 2);
        m_RomData.read((uint8_t*)&chapter_reinforcements, temp_offset, 2 );
        //rom.seekg( LoROMToPC(0x880000 + chapter_reinforcements, headered) );
        Unit_Data tempUnit;
        temp_offset = 0x880000 + chapter_reinforcements;
        m_RomData.read((uint8_t*)&tempUnit, temp_offset, UNIT_LENGTH );
        
        while( tempUnit.unit != 0xFF )
        {
            count++;
            unsigned int currentAddr = temp_offset + UNIT_LENGTH;
            uint8_t current_unit = tempUnit.unit;
            uint8_t current_name = tempUnit.name;
            if(characters.count(current_name) != 0)
            {
                bool minerva_cutscene = (tempUnit.portrait == 0x1E && current_name != 0x1E) || (tempUnit.portrait == 0x4c && current_name != 0x4C);
                bool fake_navarre = (current_name == 0x1E && current_unit == 0x79);
                //some book 2 cutscenes use PC character names with the wrong class, could mess up class assignment
                CharData tempChar;
                temp_offset = m_RomData.getAddress("CHARACTER_TABLE")  + (current_unit * STAT_LENGTH);
                //rom.seekg(m_RomData.getPCAddress("CHARACTER_TABLE", headered)  + (current_unit * STAT_LENGTH) );
                m_RomData.read((uint8_t*)&tempChar, temp_offset, STAT_LENGTH );
                //rom.read((char*)&tempChar, STAT_LENGTH );
                if( tempChar.lck != 0xFF )
                {
                    uint8_t unit_class = (tempUnit.mountedClass == 0xFF) ? tempUnit.unit_class : tempUnit.mountedClass;
                    if(i <= 20)
                    {
                        characters[current_name].book1_stats = tempChar;
                        characters[current_name].book1_class = unit_class;
                        characters[current_name].book1_level = tempUnit.level;
                        characters[current_name].book1_portrait = tempUnit.portrait; 
//                         if(current_name == 0x30)
//                             std::cout << std::hex << (int)unit_class << std::endl;
                        if(!characters[current_name].book1_set && unit_class < 0x26)
                        {
                            class_list[unit_class].count_1++;
                            characters[current_name].book1_set = true;
                        }
                    }
                    else if(i > 20 && !(tempUnit.mountedClass == 0) && !minerva_cutscene && !fake_navarre)
                    {
                        characters[current_name].book2_stats = tempChar;
                        characters[current_name].book2_class = unit_class;
                        characters[current_name].book2_level = tempUnit.level;
                        characters[current_name].book2_portrait = tempUnit.portrait; 
                        if(!characters[current_name].book2_set && unit_class < 0x26)
                        {
                            class_list[unit_class].count_2++;
                            characters[current_name].book2_set = true;
                        }
                    }
                    uint8_t *inventory = &tempUnit.weapon1;
                    for(int j = 0; j < 6; j++)
                    {
                        if(inventory[j] != 0xFF && item_list[inventory[j]].item_stats.wlv >= PRF_MIN)
                        {
                            characters[current_name].prf_item |= inventory[j];
                            if(i > 20)
                            {
                                characters[current_name].prf_item |= 0x0200;
                            }
                            else
                            {
                                characters[current_name].prf_item |= 0x0100;
                            }
                        }
                    }
                }
                
                //rom.seekg(currentAddr);
            }
            m_RomData.read((uint8_t*)&tempUnit, currentAddr, UNIT_LENGTH );
            temp_offset = currentAddr;
            //rom.read((char*)&tempUnit, UNIT_LENGTH );
            
        }
    }
    
//     int startoffset  = ((headered) ? 512 : 0);
    //rom.seekg(startoffset, rom.beg);
    //romData = new uint8_t[romSize];
    //rom.read((char*)romData, romSize );
    
    
    //rom.close();
    
}

Mystery::~Mystery(){
    delete []romData;
}

uint8_t Mystery::getReplacementUnit(uint8_t original)
{
    return 0;
}
