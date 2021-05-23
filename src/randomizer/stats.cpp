#include "mystery.h"

void Mystery::SetStats(bool before)
{
    for(std::map<uint8_t, MysteryCharacter>::iterator char_it = characters.begin(); char_it != characters.end(); ++char_it)
    {
        if(char_it->second.book1_replacement != 0xFF)
        {
            Relevel(char_it->first, char_it->second.book1_replacement, 10, 1);
        }
        if(char_it->second.book2_replacement != 0xFF)
        {
            Relevel(char_it->first, char_it->second.book2_replacement, 10, 2);
        }
    }
}
void Mystery::Relevel(uint8_t replacement, uint8_t original, int promo_level, int book)
{
    
    int original_level = ( book == 2) ? characters[original].book2_level : characters[original].book1_level;
    uint8_t original_class = ( book == 2) ? characters[original].book2_class : characters[original].book1_class;
    bool treat_as_promoted = false; //(class_list[original_class].classType == SPECIAL);
    if(class_list[original_class].promoted == PROMOTED ||
        (class_list[original_class].promoted == NO_PROMOTION && treat_as_promoted ) )
    {
        original_level += promo_level - 1;
    }
    
    int replacement_level = ( book == 2) ? characters[replacement].book2_level : characters[replacement].book1_level;
    uint8_t replacement_class = ( book == 2) ? characters[replacement].book2_class : characters[replacement].book1_class;
    if(class_list[replacement_class].promoted == PROMOTED
    )
    {
        replacement_level += promo_level - 1;
    }
    
    int difference = original_level - replacement_level;
    
    if(difference != 0)
    {
        uint8_t *adjusted;
        if( book == 2)
        {
            adjusted = (uint8_t*)&(characters[replacement].book2_stats);
        }
        else
        {
            adjusted = (uint8_t*)&(characters[replacement].book1_stats);
        }
        for(int i = 0; i < 8; i++)
        {
            int stat = adjusted[i] * 100;
            int growth = adjusted[i+9];
            if(difference >= 0)
            {
                for(int j = 0; j < difference; j++)
                {
                    stat = stat + growth;
                }
                if(i != 6 && stat >= 2000)
                {
                    stat = 2000;
                }
                else if(i != 6 && stat >= 5200)
                {
                    stat = 5200;
                }
            }
            else
            {
                for(int j = 0; j < -difference; j++)
                {
                    stat = stat - growth;
                }
                if(i != 6 && stat <= -2000)
                {
                    stat = -2000;
                }
                
            }
            char stat2 = stat/100;
            
            if(stat % 100 > 50)
            {
                stat2 = 1 + stat2;
            }
            
            adjusted[i] = stat2;
        }
        if(replacement == 0x30)
        {
            //tiki gets class base speed because going lower can cause issues after transforming.
            char stat2 = adjusted[2];
            adjusted[2] = (stat2 < 0) ? 0 : stat2;
        }
//         if(original == 0x2A)
//         {
//             std::cout << (int)adjusted[2] << std::endl;
//         }
    }
}
