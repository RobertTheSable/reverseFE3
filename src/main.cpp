#include <iostream>
#include <fstream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <functional>
#include "randomizer/mystery.h"
#include "addresser/addresser.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc >= 2)
    {
        int character_settings = Mystery::sort_reverse|Mystery::sort_reverse_2;
        RomMap a(argv[1]);

        Mystery myGame(a);

        myGame.SortCharacters(character_settings);
        myGame.SetStats(true);
        //myGame.SetClasses(RANDOM_SPECIAL|NON_PLAYER_CLASSES|KEEP_LIMIT);
        myGame.Finalize();
        myGame.SaveOutput("FE3_RR.smc", "", "results.html");
    }
    else
    {
        cout << "Pleace specify game file." << endl;
    }
    return 0;
}


