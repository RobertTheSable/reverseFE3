#include "mystery.h"

#include <functional>
#include <cstdlib>
// #define RANDOM_THIEFS 1
// #define RANDOM_SPECIAL 2
// #define KEEP_LIMIT 4
// #define NON_PLAYER_CLASSES 8
// #define INCLUDE_TEMP 16
// #define BOOK_MATCH 32
// #define GENDER_LOCK 64
//enum Class_Type {NORMAL, THIEF, SPECIAL, ENEMY, TEMP, RESTRICTED};

using std::cout;
using std::endl;
using std::hex;

void Mystery::SetClasses(const int& mode)
{
    struct class_counter{
        uint8_t id;
        int count[2];
    };
    
    std::vector<class_counter> valid_classes;
    
    for(int i = 0; i < class_list.size(); i++)
        {
            if( (class_list[i].classType == NORMAL) ||
                    (((mode & RANDOM_THIEFS) != 0) && class_list[i].classType == THIEF) ||
                    (((mode & RANDOM_SPECIAL) != 0) && class_list[i].classType == SPECIAL) ||
                    (((mode & NON_PLAYER_CLASSES) != 0) && class_list[i].classType ==  ENEMY) ||
                    (((mode & INCLUDE_TEMP) != 0) && class_list[i].classType == TEMP) )
            {
                class_counter temp;
                int c1 = class_list[i].count_1;
                int c2 = class_list[i].count_2;
                if(i== 0x15)
                {
                    c1 = 1;
                }
                else if(class_list[i].classType ==  ENEMY || class_list[i].classType == TEMP)
                {
                    c1 = 1;
                    c2 = 1;
                }
                
                if(!(mode & KEEP_LIMIT) && class_list[i].classType != SPECIAL)
                {
                    temp = {(uint8_t)i, {-1, -1}};
                }
                else
                {
                    temp = {(uint8_t)i, {c1, c2}};
                }
                
                valid_classes.push_back(temp);
            }
        }
    for(std::vector<class_counter>::iterator it = valid_classes.begin(); it != valid_classes.end(); ++it)
    {
        cout << class_list[it->id].name << " : " << it->count[0] << " : " << it->count[1] << endl;
    }
//     for(std::map<uint8_t, MysteryCharacter>::iterator char_it = characters.begin(); char_it != characters.end(); ++char_it)
//     {
//         int index = rand() % valid_classes.size();
//         cout << hex << (int)char_it->first << " : " << char_it->second.name << ": " << (int)valid_classes[index].id << endl;
//     }
}

uint8_t Mystery::ReplaceClass(uint8_t original, uint8_t replacement, uint8_t character)
{
    uint8_t newClass = replacement;
    unsigned int offset = m_RomData.getPCAddress("PROMOTION_TABLE");
    bool treat_as_promoted = false; //(class_list[original].classType == SPECIAL);
    if((class_list[replacement].promoted == PROMOTES || 
        (class_list[replacement].promoted == NO_PROMOTION && !treat_as_promoted)  ) && 
        (class_list[original].promoted == PROMOTED || 
        (class_list[original].promoted == NO_PROMOTION && treat_as_promoted ) ))
    {
        //get promoted class
        while(romData[offset] != 0xFF)
        {
            if(romData[offset] == replacement)
            {
                newClass = romData[offset+1];
            }
            offset += 2;
        }
//         if(replacement == 0x19 && character == 0x11)
//         {
//             newClass = 0x1B;
//         }
//         else
//         {
//             
//         }
        
    }
    else if(( class_list[replacement].promoted == PROMOTED || 
        (class_list[replacement].promoted == NO_PROMOTION && treat_as_promoted) )  && 
        (class_list[original].promoted == PROMOTES ||
           (class_list[original].promoted == NO_PROMOTION && !treat_as_promoted ) ) )
    {
        //get de-promoted class
        while(romData[offset] != 0xFF)
        {
            if( (character == 0x86 || character == 0x1F ) && replacement == 0x18)
            {
                newClass = 0x17;
            }
            else if(romData[offset+1] == replacement)
            {
                newClass = romData[offset];
            }
            offset += 2;
        }
    }
    
    return newClass;
}
