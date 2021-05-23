#include "mystery.h"

void Mystery::SaveOutput(const char* output, const char* backup, const char* result)
{
    if(strlen(output) != 0)
    {
        std::cout << "Saving to " << output << std::endl;
        std::cout << "File size: " << romSize << std::endl;
        std::ofstream outputFile(output, std::ios_base::out|std::ios_base::trunc);
        ApplyFixes();
        if(headered)
        {
            for(int i = 0; i < 0x200; i++)
            {
                outputFile.put(0);
            }
        }
        outputFile.write((char *)&romData[0], romSize);
    }
    if(strlen(result) != 0)
    {
        std::ofstream resultfile(result, std::ios_base::out|std::ios_base::trunc);
        std::map<uint8_t, uint8_t> stats; //key is the unit, etry is the name.
        resultfile << "<!DOCTYPE html>" << std::endl;
        resultfile << "<html><body><h1>Results</h1><br>" << std::endl;
        
        //table of contents
        resultfile  << "<a href=\"#replacements1\">Book 1 Replacements</a><br>"
                    << "<a href=\"#replacements2\">Book 2 Replacements</a><br>"
                    << "<a href=\"#book1\">Book 1 Unit Data</a><br>"
                    << "<a href=\"#book2\">Book 2 Unit Data</a><br>"
                    << "<a href=\"#stats\">Character Stats</a><br>" 
                    << "<a href=\"#classes\">Class Stats</a><br>" 
//                 << "<a href=\"#statsdebug\">Character Stats</a><br>" 
                    << std::endl;
                
        resultfile << "<h2><span id=\"replacements1\">Book 1 Replacements</span></h2><table border=\"1\">" << std::endl
                   << "<tr><th>Original</th><th>Replacement</th></tr>" << std::endl;
        for(std::vector<uint8_t>::iterator it = book1Order.begin(); it != book1Order.end(); ++it)
        {
             resultfile << "<tr><td>" << characters[characters[*it].book1_replacement].name << "</td><td>" << characters[*it].name  << "</td></tr>" << std::endl;
        }
        resultfile << "</table><br>" << std::endl;
        
        resultfile << "<h2><span id=\"replacements2\">Book 2 Replacements</span></h2><table border=\"1\">" << std::endl
                   << "<tr><th>Original</th><th>Replacement</th></tr>" << std::endl;
        for(std::vector<uint8_t>::iterator it = book2Order.begin(); it != book2Order.end(); ++it)
        {
             resultfile << "<tr><td>" << characters[characters[*it].book2_replacement].name << "</td><td>" << characters[*it].name  << "</td></tr>" << std::endl;
        }
        resultfile << "</table><br>" << std::endl;
        
        
        resultfile << "<h2><span id=\"book1\">Book 1 Unit Data</span></h2>" << std::endl;
        
        resultfile << "<table border=\"1\">" << std::endl;
        for(int i = 1; i <= 44; i++)
        {
            uint8_t chapter_index = romData[m_RomData.getPCAddress("CHAPTER_TABLE")+i];
            unsigned short chapter_reinforcements = *((unsigned short*)&romData[m_RomData.getPCAddress("REINFORCE_TABLE")+ (2 * chapter_index)]);
            unsigned short chapter_army = *((unsigned short*)&romData[m_RomData.getPCAddress("ARMY_TABLE") + (2 * chapter_index)]);
            
            unsigned int currentoffset = LoROMToPC(0x880000+chapter_reinforcements);
            std::vector<Unit_Data> chapterUnits;
            
            while(romData[currentoffset] != 0xFF)
            {
                Unit_Data *temp = (Unit_Data*)&(romData[currentoffset]);
                if(characters.count(temp->name) != 0 )
                {
                    uint8_t temp_class = (temp->mountedClass == 0xFF || temp->mountedClass == 0x0) ? temp->unit_class : temp->mountedClass ;
                    uint8_t target_class = i <= 20 ? characters[temp->name].book1_class : characters[temp->name].book2_class;
                    bool class_matches = (temp_class == target_class);
                    
                    if(!class_matches)
                    {
                        if(temp->name == 0x11 && temp_class == 0x1B)
                        {
                            class_matches = true;
                        }
                        else if(class_list[temp_class].promoted == PROMOTED)
                        {
                            class_matches = ( target_class == ReplaceClass(0x0D, temp_class, temp->name));
                        }
                        else if(class_list[temp_class].promoted == PROMOTES)
                        {
                            class_matches = ( target_class == ReplaceClass(0x0C, temp_class, temp->name));
                        }
                    }
                    uint8_t luck_test = romData[m_RomData.getPCAddress("CHARACTER_TABLE") + (temp->unit * STAT_LENGTH) + 3 ] ;
                    //uint8_t target_level = i <= 20 ? characters[temp->name].book1_level : characters[temp->name].book2_level;
                    if(!class_matches)
                    {
                        
                    }
                    if(class_matches && luck_test != 0xFF)
                    {
                        chapterUnits.push_back(*temp);
                        if(stats.count(temp->unit) == 0)
                        {
                            stats[temp->unit] = temp->name;
                        }
                    }
                }
                           
                currentoffset += UNIT_LENGTH;
            }
            currentoffset = LoROMToPC(0x890000+chapter_army);
            
            while(romData[currentoffset] != 0xFF)
            {
                Unit_Data *temp = (Unit_Data*)&(romData[currentoffset]);
                
                if(characters.count(temp->name) != 0 )
                {
                    uint8_t temp_class = (temp->mountedClass == 0xFF || temp->mountedClass == 0x0) ? temp->unit_class : temp->mountedClass ;
                    uint8_t target_class = i <= 20 ? characters[temp->name].book1_class : characters[temp->name].book2_class;
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
                    uint8_t luck_test = romData[m_RomData.getPCAddress("CHARACTER_TABLE") + (temp->unit * STAT_LENGTH) + 3 ] ;
                    //uint8_t target_level = i <= 20 ? characters[temp->name].book1_level : characters[temp->name].book2_level;
                    if(class_matches && luck_test != 0xFF)
                    {
                        chapterUnits.push_back(*temp);
                        if(stats.count(temp->unit) == 0)
                        {
                            stats[temp->unit] = temp->name;
                        }
                    }
                }
                           
                currentoffset += UNIT_LENGTH;
            }
            if(!chapterUnits.empty())
            {
                if(i == 21)
                {
                    resultfile << "</table><br><h2><span id=\"book2\">Book 2 Unit Data</span></h2><table border=\"1\">" << std::endl;
                }
                resultfile << "<tr><th>";
                if( i <= 20)
                {
                    resultfile << "Chapter 1-" << std::dec << i;
                }
                else if( i <= 40 )
                {
                    resultfile << "Chapter 2-" << std::dec<< i-20;
                }
                else
                {
                    resultfile << "Chapter F-" << std::dec<< i-40;
                }
                resultfile << "</th></tr>"<< std::endl;
                
                resultfile << "<tr><th>Name</th><th>Unit</th><th>Class</th><th>Level</th>"
                            << "<th>Weapon 1</th><th>Weapon 2</th><th>Weapon 3</th><th>Weapon 4</th>"
                            << "<th>Item 1</th><th>Item 2</th></tr>" << std::endl;
                for(std::vector<Unit_Data>::iterator it = chapterUnits.begin(); it != chapterUnits.end(); ++it)
                {
                    
                    resultfile << "<tr><td>" << characters[it->name].name << "</td>";
                    resultfile << "<td>" << std::hex << (int)it->unit << "</td>";
                    if(it->mountedClass == 0xFF || it->mountedClass == 0x00)
                    {
                        resultfile << "<td>" << class_list[it->unit_class].name << "</td>";
                    }
                    else
                    {
                        resultfile << "<td>" << class_list[it->mountedClass].name << " (Dismounted)</td>";
                    }
                    resultfile << "<td>" << (int)it->level << "</td>";
                    uint8_t *inventory = &it->weapon1;
                    for(int i = 0; i < 6; i++)
                    {
                         resultfile << "<td>" << item_list[inventory[i]].name << "</td>";
                    }
                }
            }
            
            //SetUnits(0x880000+chapter_reinforcements, replacement, original, book);
            //SetUnits(0x890000+chapter_army, replacement, original, book);
        }
        resultfile << "</table><br>" << std::endl;
        
        resultfile << "<h2><span id=\"stats\">Character Stats</span></h2><table border=\"1\">" << std::endl;
        resultfile  << "<tr><th>Name</th><th>Type</th><th>Strength</th><th>Skill</th><th>Speed</th><th>Luck</th>"
                    << "<th>Defense</th><th>Resistance</th><th>HP</th><th>Weapn Level</th></tr>" << std::endl;
        for(std::map<uint8_t, uint8_t>::iterator it =  stats.begin(); it != stats.end(); ++it)
        {
            uint8_t unit = it->first;
            uint8_t name = it->second;
            resultfile  << "<tr><td>" << characters[name].name << "</td>"
                        << "<td>Bases</td>";
            unsigned int currentoffset = m_RomData.getPCAddress("CHARACTER_TABLE") + (unit * STAT_LENGTH);
            for(int c_idx = 0; c_idx < 8; c_idx ++)
            {
                resultfile  << "<td>" << std::dec << (int)(char)romData[currentoffset + c_idx] << "</td>\n";
            }
                        
            resultfile  << "</tr><tr><td>" << std::hex << (int)it->first << "</td><td>Growths</td>";
            for(int c_idx = 9; c_idx < 17; c_idx ++)
            {
                resultfile  << "<td>" << std::dec << (int)romData[currentoffset + c_idx] << "</td>\n";
            }
             resultfile  << "</tr>\n";
            
        }
        resultfile << "</table><br>" << std::endl;
        
        resultfile << "<h2><span id=\"classes\">Class Stats</span></h2><table border=\"1\">" << std::endl;
        resultfile  << "<tr><th>Name</th><th>Mounted</th><th>Promoted</th><th>Strength</th><th>Skill</th><th>Speed</th><th>Move</th>"
                    << "<th>Defense</th><th>Resistance</th><th>Experience</th><th>HP</th><th>Weapons</th></tr>" << std::endl;
        for(std::vector<MysteryClass>::iterator it = class_list.begin(); it != class_list.end(); ++it)
        {
            uint8_t *class_stats = (uint8_t*) &(it->class_bases);
            std::string isPromoted = "No";
            std::string isMounted = "No";
            if(it->promoted == PROMOTED || it->promoted == NO_PROMOTION)
            {
                isPromoted = "Yes";
            }
            if(it->mounted)
            {
                isMounted = "Yes";
            }
            resultfile  << "<tr><td>" << it->name << "</td><td>" << isMounted << "</td><td>" << isPromoted << "</td>";
            for(int c_idx = 0; c_idx < CLASS_LENGTH; c_idx ++)
            {
                if(c_idx != 7)
                {
                    resultfile  << "<td>" << std::dec << (int)class_stats[c_idx] << "</td>";
                }
            }
            resultfile  << "<td>" << std::hex << it->weaponType << "</td>";
            resultfile  << "</tr>\n";
        }
        resultfile << "</table><br>" << std::endl;
        
//         resultfile << "<h2><span id=\"statsdebug\">Character Stats</span></h2><table border=\"1\">" << std::endl;
//         resultfile  << "<tr><th>Name</th><th>Type</th><th>Strength</th><th>Skill</th><th>Speed</th><th>Luck</th>"
//                     << "<th>Defense</th><th>Resistance</th><th>HP</th><th>Weapn Level</th></tr>" << std::endl;
//         for(std::map<uint8_t, MysteryCharacter>::iterator it =  characters.begin(); it != characters.end(); ++it)
//         {
//             uint8_t name = it->first;
//             MysteryCharacter unit = it->second;
//             std::cout << unit.name << " 0x" << std::hex << (int)unit.book1_unit << " 0x" << (int)unit.book2_unit << std::endl;
//             resultfile  << "<tr><td rowspan=\"2\">" << unit.name << "</td>"
//                         << "<td>Bases</td>";
//             uint8_t *book_stats;
//             if(unit.book1_stats.lck != 0xFF)
//             {
//                book_stats = (uint8_t*) &(unit.book1_stats);
//             }
//             else
//             {
//                 book_stats = (uint8_t*) &(unit.book2_stats);
//             }
//             for(int c_idx = 0; c_idx < 8; c_idx ++)
//             {
//                 int stat = (char)book_stats[c_idx];
//                 resultfile  << "<td>" << std::dec << stat << "</td>\n";
//             }
//                         
//             resultfile  << "</tr><tr><td>Growths</td>";
//             for(int c_idx = 9; c_idx < 17; c_idx ++)
//             {
//                 int stat = book_stats[c_idx];
//                 resultfile  << "<td>" << std::dec << stat << "</td>\n";
//             }
//              resultfile  << "</tr>\n";
//             
//         }
//         resultfile << "</table><br>" << std::endl;
        
        resultfile << "</body></html>" << std::endl;
        std::cout << "Save Result.\n";
        resultfile.close();
    }
    if(strlen(backup) != 0)
    {
        std::cout << "Save Backup.\n";
    }
    //std::ifstream original(romFile.c_str(), std::ios::binary);
    //std::ofstream dest(output, std::ios::binary);
} 
