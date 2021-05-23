#include "mystery.h"

using std::cout;
using std::endl;

void Mystery::Finalize()
{
    for(std::map<uint8_t, MysteryCharacter>::iterator char_it = characters.begin(); char_it != characters.end(); ++char_it)
    {
        
        uint8_t original, replacement;
        original = char_it->first;
        for(int i = 1; i <= 44; i++)
        {
            uint8_t chapter_index = romData[m_RomData.getPCAddress("CHAPTER_TABLE")+i];
            
            unsigned short chapter_reinforcements = *((unsigned short*)&romData[m_RomData.getPCAddress("REINFORCE_TABLE")+ (2 * chapter_index)]);
            unsigned short chapter_army = *((unsigned short*)&romData[m_RomData.getPCAddress("ARMY_TABLE") + (2 * chapter_index)]);
            int book;
            if(i <= 20)
            {
                book = 1;
                replacement = char_it->second.book1_replacement;
            }
            else
            {
                book = 2;
                replacement = char_it->second.book2_replacement;
            }
            bool indoor = false;
            if(indoor_maps.count(i) != 0 && (indoor_maps[i] == 0xFF || indoor_maps[i] == original ))
            {
                indoor = true;
            }
            
            SetUnits(0x880000+chapter_reinforcements, replacement, original, book, indoor);
            SetUnits(0x890000+chapter_army, replacement, original, book, indoor);
            
        }
        //book 2 astram is the only character that gets loaded form this table so we can default to book 2 here.
        SetUnits(m_RomData.getAddress("ALL_REINFORCE_TABLE"), char_it->second.book2_replacement, original, 2, false);
    }
}

int Mystery::SetUnits(unsigned int offset, uint8_t replacement, uint8_t original, int book, bool indoor)
{
    int count  = 0;
    if(replacement != original)
    {
        static int transform_count = 0;
        unsigned int currentoffset = LoROMToPC(offset);
        Unit_Data *temp = (Unit_Data*)&(romData[currentoffset]);
        
        int test = 0;
        //cout << test << endl;
        while(temp->unit != 0xFF)
        {
            //cout << test << endl;
            if(temp->name == original && already_assigned.count(currentoffset) == 0)
            {
                if(replacement == 0x30)
                {
                    if(book == 1)
                    {
                        romData[m_RomData.getPCAddress("TIKI_FIX_1")] = temp->unit;
                        romData[m_RomData.getPCAddress("TIKI_FIX_2")] = temp->unit;
                    }
                    else if(book == 2)
                    {
                        romData[m_RomData.getPCAddress("TIKI_FIX_1") + 4] = temp->unit;
                        romData[m_RomData.getPCAddress("TIKI_FIX_2") + 9] = temp->unit;
                    }
                }
                uint8_t newClass;
                already_assigned[currentoffset] = true;
                bool book_2_est = (temp->unit== 0x2A && temp->name == 0x2A && book == 2);
                if(book_2_est)
                {
                    //Est has the same stats in book 1 and 2 so we need to switch her to what looks like an unused stat slot so warren's stats will be correct
                    temp->unit = 0x9A;
                }
                uint8_t unit_stats = temp->unit;
                
                unsigned int unit_offset = m_RomData.getPCAddress("CHARACTER_TABLE") + (unit_stats * STAT_LENGTH );
                CharData *unit = (CharData*)&(romData[ unit_offset ]);
                uint8_t luck_test = unit->lck;
                bool minerva_cutscene = (temp->portrait == 0x1E && temp->name != 0x1E) || (temp->portrait == 0x4c && temp->name != 0x4C);
                bool fake_navarre = (temp->unit == 0x79 && temp->name == 0x12);
//                 if( temp->unit > 0x32)
//                 {
//                     cout << characters[temp->name].name << endl;
//                         
//                 }
                        uint8_t temp_class = (temp->mountedClass == 0xFF || temp->mountedClass == 0x0) ? temp->unit_class : temp->mountedClass ;
                uint8_t target_class = (book == 1) ? characters[original].book1_class : characters[original].book2_class;
                bool class_matches = (temp_class == target_class);
                if(!class_matches)
                {
                    if(class_list[temp_class].promoted == PROMOTED)
                    {
                        class_matches = ( target_class == ReplaceClass(0x0D, temp_class, temp->name));
                    }
                    else if(class_list[temp_class].promoted == PROMOTES)
                    {
                        class_matches = ( target_class == ReplaceClass(0x0C, temp_class, temp->name));
                    }
                    
                }
                if(class_matches && luck_test != 0xFF && !minerva_cutscene)//temp_class == target_class && 
                {
                    uint8_t assign_class;
                    uint8_t original_class = !(temp->mountedClass == 0xFF || temp->mountedClass == 0x00) ? temp->mountedClass : temp->unit_class;
                    if( book == 1)
                    {
//                         if(unit_stats <= 0x32)
//                         {
//                             
//                         }
                        *unit = characters[replacement].book1_stats;
                        assign_class = ReplaceClass(original_class, characters[replacement].book1_class, replacement);
                        temp->portrait = characters[replacement].book1_portrait;
                        
                    }
                    else if( book == 2)
                    {
                        if(unit_stats > 0x32)
                        {
                            //some cutscene character in book 2 uses their book 1 unit id, so we need to check before replacing.
                            if(temp->portrait == 0x12 && temp->unit == 0x79)
                            {
                                //Samto is a specia case, need to double check to not mess up his stats
                                *unit = characters[characters[0x70].book2_replacement].book2_stats;
                            }
                            else
                            {
                                *unit = characters[replacement].book2_stats;
                            }
                        }
                        assign_class = ReplaceClass(original_class, characters[replacement].book2_class, replacement);
                        temp->portrait = characters[replacement].book2_portrait;
                    }
                    if(assign_class == 0x1A && replacement == 0x11)
                        assign_class = 0x1B;
                    
                    if((char)(unit->str + class_list[assign_class].class_bases.str) < 0)
                    {
                        unit->str = -class_list[assign_class].class_bases.str;
                    }
                    if((char)(unit->skl + class_list[assign_class].class_bases.skl) < 0)
                    {
                        unit->skl = -class_list[assign_class].class_bases.skl;
                    }
                    if((char)(unit->spd + class_list[assign_class].class_bases.spd) < 0)
                    {
                        unit->spd = -class_list[assign_class].class_bases.spd;
                    }
                    if((char)(unit->def + class_list[assign_class].class_bases.def) < 0)
                    {
                        unit->def = -class_list[assign_class].class_bases.def;
                    }
                    if((char)(unit->res + class_list[assign_class].class_bases.res) < 0)
                    {
                        unit->res = -class_list[assign_class].class_bases.res;
                    }
                    if((char)unit->lck < 0)
                    {
                        unit->lck = 0;
                    }
                    if((char)unit->wlv <= 0)
                    {
                        unit->wlv = 1;
                    }
                    temp->name = replacement;
                    if(class_list[assign_class].mounted && indoor)
                    {
                        temp->mountedClass = assign_class;
                        if(assign_class == 0x0E)
                        {
                            temp->unit_class = 0x0D;
                        }
                        else
                        {
                            if(unit->gen != 0)
                            {
                                temp->unit_class = 0x06;
                            }
                            else
                            {
                                temp->unit_class = 0x05;
                            }
                        }
                    }
                    else
                    {
                        temp->unit_class = assign_class;
                        /*
                        if(assign_class == 0x14)
                        {
                            //std::cout << std::hex << (int) assign_class << std::endl;
                            temp->mountedClass = assign_class;
                        }*/
                    }
    //                 if(assign_class == 0x14)
    //                 {
    //                     std::cout << std::hex << (int) temp->mountedClass << std::endl;
    //                 }
                    if(!class_list[assign_class].mounted && temp->mountedClass != 0xFF)
                    {
                        temp->mountedClass = 0xFF;
                    }
                    
                    if(! ( (temp->name == 0x12 && temp->unit == 0x79) || 
                        (book == 2 && temp->unit <= 0x32) ))
                    {
                        unsigned int unit_name_offset = temp->unit;
                        unit_name_offset *= 2;
                        unit_name_offset += m_RomData.getPCAddress("UNIT_NAME_TABLE");
                        uint8_t old_name = book_2_est ? 0x2A : romData[unit_name_offset];
//                         if(temp->unit == 0xc || temp->unit == 0x21)
//                         {
//                             cout << std::hex << LoROMToPC(offset) << endl;
// 
//                         }
                        if(old_name != temp->name && old_name != 0xFF && !fake_navarre)
                        {
                            if(book == 1 && characters[old_name].book1_unit == 0xFF)
                            {
                                characters[old_name].book1_unit = temp->unit;
                            }
                            else if(book == 2 && characters[old_name].book2_unit == 0xFF)
                            {
                                characters[old_name].book2_unit = temp->unit;
                            }
                            romData[unit_name_offset] = temp->name;
                            romData[unit_name_offset+1] = temp->portrait;
                        }
                        
                    }
                    
                    if(temp->unit == 0x3F || temp->unit == 0x40)
                    {
                        temp->x_pos += 4;
                    }
                    
                    
                    uint8_t* inventory = (uint8_t*) &(temp->weapon1);
                    int wlv = unit->wlv;
                    Weapon_Type newType = class_list[temp->unit_class].weaponType;
                    int i = 0;
                    while(i < 4)
                    {
                        Weapon_Type wpType = item_list[inventory[i]].item_stats.type;
                        if(inventory[i] != 0xFF)
                        {
                            
                            bool canUse = (wpType & newType) != 0 && (wlv >= item_list[inventory[i]].item_stats.wlv);
                            if(!canUse)
                            {
                                if( i > 0 && class_list[temp->unit_class].mounted && 
                                    newType == LANCE && ((wpType & SWORD) != 0) && 
                                    item_list[inventory[i-1]].item_stats.type == LANCE)
                                {
                                    canUse = true;
                                }
                            }
                            if( !canUse )
                            {
                                uint8_t newItem = inventory[i];
                                if( i < 4)
                                {
                                    newItem = ReplaceItem(inventory[i], newType, wlv );
                                    
                                }
                                if(newItem == inventory[i] && i == 0 && inventory[i+1] == 0xFF)
                                {
                                    inventory[i] = ReplaceItem(0x02, newType, wlv );
                                    inventory[i+1] = newItem;
                                }
                                else
                                {
                                    if(item_list[newItem].item_stats.type == STAFF)
                                    {
                                        int replace = 0;
                                        if(inventory[4] == 0xFF)
                                        {
                                            replace = 4;
                                        }
                                        else if(inventory[5] ==  0xFF)
                                        {
                                            replace = 5;
                                        }
                                        else
                                        {
                                            if(inventory[4] == 0x57)
                                            {
                                                replace = 4;
                                            }
                                            else if(inventory[5] == 0x57)
                                            {
                                                replace = 5;
                                            }
                                        }
                                        if(replace == 4 || replace == 5)
                                        {
                                            inventory[replace] = newItem;
                                            inventory[i] = 0xFF;
                                        }
                                    }
                                    else
                                    {
                                        inventory[i] = newItem;
                                    }
                                }   
                            }
    //                         
                        }
                        i++;
                    }
                    if(inventory[0] == 0xFF)
                    {
                        if(inventory[4] != 0xFF && item_list[inventory[4]].item_stats.type == STAFF && newType != STAFF)
                        {
                            inventory[0] = ReplaceItem(inventory[4], newType, wlv );
                        }
                        else
                        {
                            
                            if(inventory[4] == 0xFF || inventory[4] == 0x57)
                            {
                                
                            }
                            else if(inventory[5] == 0xFF || inventory[5] == 0x57)
                            {
                                
                            }
                            
                            //inventory[0] = 0xFF;
                        }
                    }
                    for(i = 4; i < 6; i++)
                    {
                        if(item_list[inventory[i]].item_stats.wlv >= PRF_MIN)
                        {
                            inventory[i] = 0xFF;
                        }
                        if(original_class == 0x13 && inventory[i] == 0xFF)
                        {
                            //compensate for losing thiefs with some chest keys
                            inventory[i] = 0x5B;
                        }
                    }
                    if(characters[temp->name].prf_item != 0)
                    {
                        unsigned short prf_item = characters[temp->name].prf_item;
                        bool thisBook = (book & ((prf_item & 0xFF00) >>8) ) == book;
    //                     if(temp->name == 0x1f && prf_item != 0 )
    //                     {
    //                         std::cout << item_list[prf_item].name << std::endl;
    //                         std::cout << std::hex << (prf_item & 0xFF) << std::endl;
    //                         std::cout << std::hex << (int)temp->name << std::endl;
    //                         std::cout << std::hex << (int)(book & ((prf_item & 0xFF00) >>8) ) << std::endl;
    //                     }
                        if(thisBook)
                        {
                            uint8_t prf = prf_item & 0xFF;
                            int max = 4;
                            if(item_list[prf].item_stats.type == STAFF)
                            {
                                i = 4;
                                max = 6;
                            }
                            else
                            {
                                i = 0;
                                max = 4;
                            }
                            while(i < max && inventory[i] != 0xFF)
                            {
                                i++;
                            }
                            
                            
                            if(i < max && inventory[i] == 0xFF)
                            {
                                
                                inventory[i] = prf;
                            }
                            else
                            {
                                if(item_list[prf].item_stats.type == STAFF)
                                {
                                    if(inventory[4] == 0x57)
                                    {
                                        inventory[4] = prf;
                                    }
                                    else if(inventory[5] == 0x57)
                                    {
                                        inventory[5] = prf;
                                    }
                                }
                            }
                        }
                    }
                    if(inventory[4] == 0x55 && temp->unit_class != 0x1C)
                    {
                        inventory[4] = 0xFF;
                    }
                    else if(temp->unit_class == 0x1C || temp->unit_class == 0x0F)
                    {
                        uint8_t dragonstone = (temp->unit_class == 0x1C) ? 0x55 : 0x4F;
                        if(inventory[4] == 0xFF)
                        {
                            inventory[4] = dragonstone;
                        }
                        else if(inventory[5] == 0xFF || inventory[5] == 0x57)
                        {
                            inventory[5] = dragonstone;
                        }
                    }
                    if( transform_count <= 30 && (temp->unit_class == 0x1C || temp->unit_class == 0x0F || temp->unit_class == 0x14) )
                    {
                        romData[m_RomData.getPCAddress("TRANSFORMERS_FIX") + transform_count] = temp->unit;
                        //std::cout << transform_count << std::endl;
                        //std::cout << std::hex << (int)temp->unit << std::endl;
                        transform_count += 5;
                        
                    }
                }
            }
            currentoffset += UNIT_LENGTH;
            temp = (Unit_Data*)&(romData[currentoffset]);
            count++;
        }
    }
    return count;
}
 
