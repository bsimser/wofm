// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#include "NumberGenerator.h"

#include <stdlib.h>
#include <fstream>
#include <time.h>	//seed generation
#include <assert.h>

namespace Random
{
    int getInt(int max, int min)
    {
        int random_integer;

        if (max < min) //swap
        {
            int temp;
            temp = max;
            max = min;
            min = temp;
        }

        int range = (max - min);

        random_integer = min + int(range*rand() / (RAND_MAX + 1.0));

        assert(random_integer <= max);
        assert(random_integer >= min);

        //fail safe
        if (random_integer >= max)
            random_integer = max - 1;
        if (random_integer < min)
            random_integer = min;

        return random_integer;
    }

    void reseed(int seed)
    {
        if (seed == -1)
            seed = (int)time(NULL);

        srand(seed);
#ifdef _DEBUG
        std::ofstream myfile;
        myfile.open("seed.txt");
        myfile << seed;
        myfile.close();
#endif
    }
}