#include "mystery.h"

/* modes:
 * 0 - normal operation
 * 1 - force return even in case of a regalia or PRF
 */
uint8_t Mystery::ReplaceItem(uint8_t original, Weapon_Type newType, int wlv, int mode)
{
    uint8_t newItem = 0xFF;
    Weapon_Type wType = item_list[original].item_stats.type;
    int wpn_wlv = item_list[original].item_stats.wlv;
    int wpn_mt = item_list[original].item_stats.atk;
    
    
    if(wpn_wlv == 12 && wpn_mt == 20)
    {
        //regalia, don't change
        newItem = original;
    }
    else
    {
        int min_wlv = (wpn_wlv > wlv) ? wlv : wpn_wlv;
        
        if(min_wlv > 2)
        {
            if(item_list[original].item_stats.crt >= 20)
            {
                if( ( newType & SWORD) != 0 && wlv >=7 )
                {
                    newItem = 0x04;
                }
                else if( ( newType & LANCE) != 0 && wlv >= 6 )
                {
                    newItem = 0x15;
                }
                else if( ( newType & BOW) != 0  && wlv >= 6 )
                {
                    newItem = 0x1C;
                }
                else if( ( newType & MAGIC) != 0  )
                {
                    if(wlv >= 4)
                    {
                        newItem = 0x30;
                    }
                    else
                    {
                        min_wlv = 3;
                    }
                }
                else
                {
                    min_wlv = 2;
                }
            }
            else if ( original ==  0x2F || original == 0x12 || original == 0x0C ||
                original == 0x0D )
            {
                if( ( newType & SWORD) != 0 && wlv >= 3)
                {
                    newItem = 0x0C;
                }
                else if( ( newType & LANCE) != 0 && wlv >= 4)
                {
                    newItem = 0x12;
                }
                else if( ( newType & MAGIC) != 0)
                {
                    if(wlv >= 5)
                    {
                        newItem = 0x2F;
                    }
                    else
                    {
                        min_wlv = 3;
                    }
                    
                }
                else 
                {
                    min_wlv = 2;
                }
            }
            else if( original == 0x13 || original == 0x20)
            {
                if( ( newType & LANCE) != 0 && wlv >= 3)
                {
                    newItem = 0x13;
                }
                else if( ( newType & AXE) != 0 && wlv >= 3 )
                {
                    newItem = 0x20;
                }
                else
                {
                    min_wlv = 2;
                }
            }
            else if(wpn_mt >= 12)
            {
                if( ( newType & SWORD) != 0 && wlv >= 9)
                {
                    newItem = 0x02;
                }
                else if( ( newType & LANCE) != 0 && wlv >= 7)
                {
                    newItem = 0x11;
                }
                else if( ( newType & BOW) != 0 && wlv >= 7 )
                {
                    newItem = 0x1B;
                }
                else if( ( newType & AXE) != 0 && wlv >= 10 )
                {
                    newItem = 0x21;
                }
                else if( ( newType & MAGIC) != 0 )
                {
                    if(wlv >= 14)
                    {
                        newItem = 0x33;
                    }
                    else if(wlv >= 12)
                    {
                        newItem = 0x32;
                    }
                    else if(wlv >= 8)
                    {
                        newItem = 0x31;
                    }
                    else
                    {
                         min_wlv = 3;
                    }
                }
                else 
                {
                    min_wlv = 2;
                }
            }
            else if(newType == STAFF)
            {
                min_wlv = 1;
            }
                
        }
        
        if(newItem == 0xFF)
        {
            if( (( newType & MAGIC) != 0 || ( newType & BOW) != 0)
                && min_wlv > 1 && wlv >= 3)
            {
                min_wlv = 3;
            }
            else if( !(( newType & MAGIC) != 0 || ( newType & BOW) != 0) &&
                min_wlv == 3)
            {
                min_wlv = 2;
            }
            std::vector<MysteryItem>::iterator it = item_list.begin();
            while(it != item_list.end() && newItem == 0xFF)
            {
                if( (it->item_stats.type & newType  ) != 0 && it->item_stats.wlv == min_wlv)
                {
                    newItem = it->item_id;
                }
                ++it;
            }
        }
    }
    
    return newItem;
} 
