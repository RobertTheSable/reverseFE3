#include "mystery.h"
#include <algorithm>
#include <random> 
#include <chrono> 

using std::cout;
using std::endl;
void Mystery::SortCharacters(const sortmode& mode)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::vector<uint8_t> *orders[] = {&book1Order, &book2Order};
    sortmode book_mode = mode;
    bool swap_same = (book_mode & treat_same) != 0;
    for(int i = 0; i < 2; i++)
    {
//         cout << "Book " << i + 1 << ": " << endl;
        if( (book_mode & sort_reverse) != 0 || (book_mode & sort_random) != 0)
        {
            //cout << "Book " << i+1 << ": " << endl;
            std::vector<uint8_t> oldOrder(orders[i]->size());
            std::copy(orders[i]->begin(), orders[i]->end(), oldOrder.begin());
            bool sync_book = (book_mode & sort_sync) != 0;
            if( (book_mode & sort_random) !=0 )
            {
                std::shuffle(orders[i]->begin(), orders[i]->end(), std::default_random_engine(seed));
            }
            else if( (book_mode & sort_reverse) !=0 )
            {
                std::reverse(orders[i]->begin(), orders[i]->end());
            }
            std::vector<uint8_t>::iterator old_start = oldOrder.begin();
            std::vector<uint8_t>::iterator new_start = orders[i]->begin();
            for( ; new_start != orders[i]->end(); ++new_start)
            {
                //cout << characters[*new_start].name << " replaces " << characters[*old_start].name << endl;
                bool join_same = characters[*old_start].join_point[i] == characters[*new_start].join_point[i];
                if(!(join_same && !swap_same))
                {
                    ChangeCharacter(*old_start, *new_start, i+1, sync_book);
//                     cout << characters[*new_start].name << " replaces " << characters[*old_start].name << endl;
                }
//                 else
//                     cout << characters[*new_start].name << " doesn't change." << endl;
                ++old_start;
            }
        }
        book_mode = book_mode >> 3;
        //cout << endl;
    }

}

void Mystery::ChangeCharacter(uint8_t original, uint8_t replacement, int book, bool sync)
{
    //static int lords_found = 0;
    //unsigned int currentoffset = 0;
    //int target_level = 1;
    if( book == 1 )
    {
        characters[original].book1_replacement = replacement;
//         if(original == lord_name1 && lords_found == 0)
//         {
//             lord_name1 = replacement;
//             lords_found ++;
//         }
        if(sync)
        {
            characters[original].book2_replacement = replacement;
        }
    }
    else if( book == 2)
    {
        characters[original].book2_replacement = replacement;
//         if(original == lord_name2 && lords_found <= 1)
//         {
//             lord_name2 = replacement;
//             lords_found ++;
//         }
        if(sync)
        {
            characters[original].book1_replacement = replacement;
        }
    }
    //Relevel(original, replacement, 10, book);
}
