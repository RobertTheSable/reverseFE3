#include "mystery.h"
#include "text.h"

//TODO: Sage fix
// 83/D3AD:	AF16447F	lda $7F4416
// 83/D3B1:	29FF00  	and #$00FF
// 83/D3B4:	C91100  	cmp #$0011
// 83/D3B7:	D026    	bne $D3DF
// 83/D3B9:	20D8D4  	jsr $D4D8
// 83/D3BC:	22FFD993	jsr $93D9FF
// 83/D3C0:	A91B00  	lda #$001B
// 83/D3C3:	8F01447F	sta $7F4401

void Mystery::ApplyFixes()
{
    uint8_t default_lord = romData[m_RomData.getPCAddress("GAME_OVER_FIX_1") + 8];
    
    unsigned int chapter_reinforcements = *((unsigned short*)&romData[m_RomData.getPCAddress("REINFORCE_TABLE") + (2 * romData[m_RomData.getPCAddress("CHAPTER_TABLE") +21])]) + 0x880001;
    uint8_t lord_class = romData[LoROMToPC(chapter_reinforcements)];
    if(!(class_list[lord_class].mounted))
    {
        unsigned int lord_mov_ptr = *((unsigned int *)&romData[ m_RomData.getPCAddress("MOV_TABLE") + (lord_class * 3) ]);
        lord_mov_ptr &= 0xFFFFFF;
        uint8_t mountain_mov = romData[LoROMToPC(lord_mov_ptr) + 7];
        if(mountain_mov == 0xFF)
        {
            romData[LoROMToPC(lord_mov_ptr) + 7] = 4;
        }
    }
    if(characters[0x2A].book2_replacement != 0xFF)
    {
        // Est has the same unit number in Books 1 and 2
        // If she's replaced, we need to change which unit the game loads at this point.
        romData[m_RomData.getPCAddress("BOOK2_EST")] = 0x9A;
    }
    
    
    if(characters[default_lord].book1_replacement != 0xFF || characters[default_lord].book2_replacement != 0xFF)
    {
        //fix check for game over. The second one is specifically for music.
        // E2 20 AF 46 44 7F 22 06 F1 DF
        uint8_t gameOverHook[] = {0xE2, 0x20, 0xAF, 0x46, 0x44, 0x7F, 0x22, 0x06, 0xF1, 0xDF};
        for(int i = 0; i < 10; i++)
        {
            romData[m_RomData.getPCAddress("GAME_OVER_FIX_1") + i] = gameOverHook[i];
            romData[m_RomData.getPCAddress("GAME_OVER_FIX_2") + i] = gameOverHook[i];
        }
        
        //Fix access to convoy.
        //to 83E2A8
        unsigned int offset =  m_RomData.getPCAddress("CONVOY_FIX_1");
        uint8_t supplyHook[] = {0x22, 0x00, 0xF1, 0xDF, 0x80, 0x01};
        for(int i = 0; i < 6; i++)
        {
            romData[offset+i] = supplyHook[i];
        }
        supplyHook[5] = 4;
        offset =  m_RomData.getPCAddress("CONVOY_FIX_2");
        for(int i = 0; i < 6; i++)
        {
            romData[offset+i] = supplyHook[i];
            romData[m_RomData.getPCAddress("CONVOY_FIX_3") + i] =  supplyHook[i];
        }
        
        //Force deployment of lord
        uint8_t deployHook[] = {0x22, 0x1A, 0xF1, 0xDF, 0xF0, 0xDC};
        offset =  m_RomData.getPCAddress("DEPLOY_FIX");
        for(int i = 0; i < 6; i++)
        {
            romData[offset+i] = deployHook[i];
        }
        
        //write algorithm to DFF100
        offset =  m_RomData.getPCAddress("FREE_SPACE_1");
        //E2 20 AF 16 44 7F 48 AF DF 07 7E C9 15 B0 05 68 
        //C9 32 80 03 68 C9 32 C2 20 6B 48 AF DF 07 7E C9 
        //15 B0 05 68 C9 32 80 03 68 C9 32 6B
        uint8_t supplyAlgortihm[] = {0xE2, 0x20, 0xAF, 0x16, 0x44, 0x7F, 0x48, 0xAF, 0xDF, 0x07, 0x7E, 0xC9, 0x15, 0xB0, 0x05, 
            0x68, 0xC9, characters[default_lord].book1_replacement, 0x80, 0x03, 0x68, 0xC9, characters[default_lord].book2_replacement, 0xC2, 0x20, 0x6B, 
            0x48, 0xAF, 0xDF, 0x07, 0x7E, 0xC9, 0x15, 0xB0, 0x05, 0x68, 0xC9, characters[default_lord].book1_replacement, 0x80, 0x03, 0x68, 0xC9, characters[default_lord].book2_replacement, 0x6B};
        for(int i = 0; i < 0x2C; i++)
        {
            romData[offset+i] = supplyAlgortihm[i];
        }
        
    }
    if(characters[0x30].book1_replacement != 0xFF)
    {
        // fix Tiki transformation freeze.
        uint8_t tikiPatch[] = {0x01, 0x00, 0x10, 0x04, 0x00, 0x10, 0x12, 0x08, 0xFF, 0xFF, 0x00};
        unsigned int offset = m_RomData.getPCAddress("TIKI_EVENT_FIX");
        for(int i = 0; i < 11; i++)
        {
            romData[offset+i] = tikiPatch[i];
        }
    }
    unsigned int offset =  m_RomData.getPCAddress("TALKS_TABLE");
    TextMap evt("events.ini");
    while(romData[offset] != 0xFF)
    {
        uint8_t chapter = romData[offset];
        uint8_t char_replacement1 = 0xFF;
        uint8_t char_replacement2 = 0xFF;
        if(memchr (&romData[m_RomData.getPCAddress("CHAPTER_TABLE")], chapter, 21) != NULL)
        {
            char_replacement1 = characters[romData[offset+1]].book1_replacement;
            char_replacement2 = characters[romData[offset+2]].book1_replacement;
        }
        else
        {
            char_replacement1 = characters[romData[offset+1]].book2_replacement;
            char_replacement2 = characters[romData[offset+2]].book2_replacement;
        }
        if(char_replacement1 != 0xFF)
        {
            romData[offset+1] = char_replacement1;
        }
        if(char_replacement2 != 0xFF)
        {
            romData[offset+2] = char_replacement2;
        }
        // Need to parse events to get text pointers
        // Which we need to swap portraits
        // Which we need to fix things like the bug with Samto/Navarre in Book 2.
        unsigned short event = *((unsigned short*)&romData[offset+4]);
        unsigned int event_offset = (*((unsigned int*)&romData[m_RomData.getPCAddress(0x8b8a93 + (event*2))]) & 0xFFFF ) + 0x8b0000;
        bool parsing = true;
        while(parsing)
        {
            unsigned int event_pc_addr = m_RomData.getPCAddress(event_offset);
            int next_index = 1;
            int event_code = romData[event_pc_addr];
            if(evt.get_length(event_code) == -1)
            {
                std::cout << "Unrecognized event command 0x" << std::hex << (int)event_code 
                            << " at offset 0x" << event_offset
                            << " at offset 0x" << offset
                            << " in index 0x" << event << std::endl;
                parsing = false;
            }
            else if(event_code == 0)
            {
                parsing = false;
            }
            else
            {
                if(event_code == 0x02)
                {
                    int text_idx = *((unsigned short*)&romData[event_pc_addr + 1]);
                    ParseText(text_idx, chapter);
                }
                next_index += evt.get_length(event_code);
            }
            event_offset += next_index;
            
        }
        
        offset += TALK_LENGTH;
    }
    offset =  m_RomData.getPCAddress("VILLAGE_TABLE");
    while(romData[offset] != 0xFF)
    {
        uint8_t chapter = romData[offset];
        uint8_t char_replacement1 = 0xFF;
        if(romData[offset+1] != 0xFF)
        {
            if(memchr (&romData[m_RomData.getPCAddress("CHAPTER_TABLE")], chapter, 21) != NULL)
            {
                char_replacement1 = characters[romData[offset+1]].book1_replacement;
            }
            else
            {
                char_replacement1 = characters[romData[offset+1]].book2_replacement;
            }
            if(char_replacement1 != 0xFF)
            {
                romData[offset+1] = char_replacement1;
            }
        }
        offset += VILLAGE_LENGTH;
    }
    //text id is offset 5, word
    offset =  m_RomData.getPCAddress("BATTLE_EVT_TABLE");
    while(romData[offset] != 0xFF)
    {
        uint8_t chapter = romData[offset];
        uint8_t char_replacement1 = 0xFF;
        if(romData[offset+1] != 0xFF && characters.count(romData[offset+1]) >  0 )
        {
            if(memchr (&romData[m_RomData.getPCAddress("CHAPTER_TABLE")], chapter, 21) != NULL)
            {
                char_replacement1 = characters[romData[offset+1]].book1_replacement;
            }
            else
            {
                char_replacement1 = characters[romData[offset+1]].book2_replacement;
            }
            
            if(char_replacement1 != 0xFF)
            {
                romData[offset+1] = char_replacement1;
                int text_idx = *((unsigned short*)&romData[offset + 5]);
                ParseText(text_idx, chapter);
            }
        }
        offset += BATTLE_EVT_LENGTH;
    }
    
    // Fix Ending statistics
    // Just Book 1 for now
    offset =  m_RomData.getPCAddress("ENDING_STATS");
    for(int unit_idx = 0; unit_idx < 46; unit_idx++)
    {
        uint8_t unit_num = romData[offset + unit_idx];
        unsigned int unit_name_offset = unit_num*2;
        uint8_t unit_name = romData[m_RomData.getPCAddress("UNIT_NAME_TABLE") + unit_name_offset];
        
        if (characters[unit_name].book1_unit != 0xFF) {
             romData[offset + unit_idx] = characters[unit_name].book1_unit;
        }
    }
    offset += 46;
    for(int unit_idx = 0; unit_idx < 45; unit_idx++)
    {
        uint8_t unit_num = romData[offset + unit_idx];
        if(unit_num == 0x2A) {
            unit_num = 0x9A;
        }
        
        unsigned int unit_name_offset = unit_num*2;
        uint8_t unit_name = romData[m_RomData.getPCAddress("UNIT_NAME_TABLE") + unit_name_offset];
        
        if (characters[unit_name].book2_unit != 0xFF) {
             romData[offset + unit_idx] = characters[unit_name].book2_unit;
        }
    }
    
    struct DeathChecks {
        int address;
        int book;
        int count;
    };
    DeathChecks deathOffsets[] = {
        {m_RomData.getPCAddress("DEATH_CHECK_B1"), 1, 2},
        {m_RomData.getPCAddress("DEATH_CHECK_B2"). 2, 6}
    };
    
    // the Caeda, Tiki, and Maalicia flags probably should be changed
    // since most likely they nly affect the ending
    // but I'm too lazy to check if they're used for anything else
    for (auto& baseOffset: deathOffsets) {
        auto address = baseOffset.address + 1;
        for (int idx = 0; idx < baseOffset.count; ++idx) {
            int oldName = romData[address];
            if (baseOffset.book == 1) {
                romData[address] = characters[oldName].book1_replacement
            } else {
                romData[address] = characters[oldName].book2_replacement
            }
            address += 5;
        }
    }
} 

/*
 * Replace portraits in text events
*/
void Mystery::ParseText(int index, int chapter)
{
    if(text_parsed.count(index) == 0)
    {
        TextMap t("text.ini");
        unsigned int text_offset = *((unsigned int*)&romData[m_RomData.getPCAddress("TEXT_TABLE") + (index*3)]) & 0xFFFFFF;
        bool text_parsing = true;
        
        while(text_parsing)
        {
            int text_index = 1;
            unsigned int pc_offset = m_RomData.getPCAddress(text_offset);
            if(romData[pc_offset] == 0)
            {
                if(t.get_length(romData[pc_offset +1]) == -1)
                {
                    text_parsing = false;
                    std::cout << "Unrecognized text command 0x" << std::hex << (int)romData[pc_offset +1] 
                            << "00 at offset 0x" << text_offset
                            << " in index 0x" << index << std::endl;
                }
                else if(romData[pc_offset +1] == 0)
                {
                    text_parsing = false;
                }
                else
                {
                    text_index += t.get_length(romData[pc_offset +1]) + 1;
                    if(romData[pc_offset +1] == 0x84 )
                    {
                        int portrait = romData[pc_offset +2];
                        
                        int table_offset = 0; //m_RomData.getPCAddress("UNIT_NAME_TABLE");
                        uint8_t old_name = 0xFF;
                        while(table_offset < 344 && old_name == 0xFF)
                        {
                            if(romData[m_RomData.getPCAddress("UNIT_NAME_TABLE") + table_offset + 1] == portrait)
                            {
                                old_name = romData[m_RomData.getPCAddress("UNIT_NAME_TABLE") + table_offset];
                            }
                            table_offset+=2;
                        }
                        if(old_name != 0xFF)
                        {
                                if(memchr (&romData[m_RomData.getPCAddress("CHAPTER_TABLE")], chapter, 21) != NULL)
                                {
                                    //book 1
                                    uint8_t new_name = characters[old_name].book1_replacement;
                                    if(new_name != 0xFF)
                                    {
                                        romData[pc_offset +2] = characters[new_name].book1_portrait;
                                    }
                                }
                                else
                                {
                                    //book 2
                                    uint8_t new_name = characters[old_name].book2_replacement;
                                    if(new_name != 0xFF)
                                    {
                                        romData[pc_offset +2] = characters[new_name].book2_portrait;
                                        
                                    }
                                }
                        }
                        
                    }
                }
            }
            text_offset += text_index;
        }
        text_parsed[index] = true;
    }
}
