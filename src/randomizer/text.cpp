#include "text.h"

#include <iostream>
#include <fstream>

 TextMap::TextMap(const char* fname)
 {
     for(int i = 0 ; i < 255; i++)
     {
         cmd_lengths[i] = -1;
     }
     std::ifstream ini_file(fname);
     int text_c;
     while(ini_file >> std::hex >> text_c)
     {
         ini_file.ignore(5,':');
         ini_file >> cmd_lengths[text_c];
     }
 }
 
 int TextMap::get_length(int index)
 {
     return cmd_lengths[index];
 }
 
//  int main()
//  {
//      TextMap t("text.ini");
//      std::cout << t.get_length(0x89) << std::endl;
//      std::cout << t.get_length(0x09) << std::endl;
//      return 0;
//  }
