#include "CellularAutomata.h"
#include "NumberGenerator.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// --------------------------------------------------------------------------------------------------------------------------------

CellularAutomata::CellularAutomata(void) :
current_rep(0),
use_2R(0),
generations(0)
{
    fillprob = 45;
    r1_cutoff = 5;
    r2_cutoff = 2;
    mSizeX = DUNGEON_SIZE_W;
    mSizeY = DUNGEON_SIZE_H;
    reps = 5;

    cutof_2R = reps - 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

CellularAutomata::CellularAutomata(int fillProbability, int r1_adjacent, int r2_adjacent, int numRepititions, int width, int height) :
fillprob(fillProbability),
r1_cutoff(r1_adjacent),
r2_cutoff(r2_adjacent),
reps(numRepititions),
mSizeX(width),
mSizeY(height),
current_rep(0),
use_2R(0),
generations(0)
{
    cutof_2R = reps; //no cutoff
}

// --------------------------------------------------------------------------------------------------------------------------------

int CellularAutomata::SetType(int type)
{
    switch (type)
    {
    case CA_LARGE_CAVERN:

        fillprob = 45;
        r1_cutoff = 5;
        r2_cutoff = 0;
        mSizeX = DUNGEON_SIZE_W;
        mSizeY = DUNGEON_SIZE_H;
        reps = 5;
        break;

    case CA_TWISTY:

        fillprob = 45;
        r1_cutoff = 5;
        r2_cutoff = 2;
        mSizeX = DUNGEON_SIZE_W;
        mSizeY = DUNGEON_SIZE_H;
        reps = 5;
        cutof_2R = reps;

        break;

    case CA_DEFAULT:
    default:
        fillprob = 45;
        r1_cutoff = 5;
        r2_cutoff = 2;
        mSizeX = DUNGEON_SIZE_W;
        mSizeY = DUNGEON_SIZE_H;
        reps = 5;
        cutof_2R = reps - 1;
        break;
    }

    return type;

}

// --------------------------------------------------------------------------------------------------------------------------------

int CellularAutomata::randpick(void)
{
    if (rand() % 100 < fillprob)
        return TILE_WALL;
    else
        return TILE_FLOOR;
}

// --------------------------------------------------------------------------------------------------------------------------------

void CellularAutomata::initmap(void)
{
    int y, x;

    //puts wall everywhere 
    for (x = 0; x < mSizeX; x++) 
    {
        for (y = 0; y < mSizeY; y++)
        {
            grid[x][y] = grid2[x][y] = TILE_WALL;
        }
    }

    //randomly fill map (grid 1)
    for (x = 1; x < mSizeX - 1; x++) 
    {
        for (y = 1; y < mSizeY - 1; y++)
            grid[x][y] = randpick();
    }
}

// --------------------------------------------------------------------------------------------------------------------------------

void CellularAutomata::generation(void)
{
    // rules
    int y, x, ii, jj;

    use_2R = (r2_cutoff == 0) ? false : true;

    for (x = 1; x < mSizeX - 1; x++)
    {
        for (y = 1; y < mSizeY - 1; y++)
        {
            int adjcount_r1 = 0,	//number of adjacent walls (range 1) for current square
                adjcount_r2 = 0;	//number of adjacent walls (range 2) for current square

            for (ii = -1; ii <= 1; ii++)	//check for adjacent walls at current square
            {
                for (jj = -1; jj <= 1; jj++)
                {
                    if (grid[x + ii][y + jj] != TILE_FLOOR)
                        adjcount_r1++;
                }
            }
            if (current_rep >= cutof_2R)
                use_2R = false;
            for (ii = x - 2; ii <= x + 2 && use_2R; ii++) //check adjacent +1 squares
            {
                for (jj = y - 2; jj <= y + 2; jj++)
                {
                    if (abs(ii - x) == 2 && abs(jj - y) == 2)	//ignore current square
                        continue;
                    if (ii < 0 || jj < 0 || ii >= mSizeY || jj >= mSizeX) //ignore out of bounds
                        continue;
                    if (grid[ii][jj] != TILE_FLOOR)	//get count
                        adjcount_r2++;
                }
            }
            if (x > 40)
                int t = 6;
            if (adjcount_r1 >= params.r1_cutoff || (adjcount_r2 <= params.r2_cutoff && use_2R))
                grid2[x][y] = TILE_WALL; //add to working grid
            else
                grid2[x][y] = TILE_FLOOR;
        }
    }

    //copy working grid in to base grid.
    for (x = 1; x < mSizeX - 1; x++)
    {
        for (y = 1; y < mSizeY - 1; y++)
        {
            grid[x][y] = grid2[x][y]; 
        }
    }
}

// --------------------------------------------------------------------------------------------------------------------------------

int GetNext(int x, int y)
{
    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

int CellularAutomata::Connect()
{
    //get random open area
    for (int x = 1; x < mSizeX - 1; x++)
    {
        for (int y = 1; y < mSizeY - 1; y++)
        {

        }
    }

    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

void CellularAutomata::FloodFill(int x, int y)
{
    if ((grid[x][y] == TILE_FLOOR) && (grid2[x][y] != TILE_FLOOR))
        grid2[x][y] = TILE_FLOOR;
    else
        return;
    FloodFill(x + 1, y);
    FloodFill(x - 1, y);
    FloodFill(x, y + 1);
    FloodFill(x, y - 1);

    //diagonal floodfill??
    FloodFill(x - 1, y - 1);
    FloodFill(x + 1, y - 1);
    FloodFill(x - 1, y + 1);
    FloodFill(x + 1, y + 1);
}

// --------------------------------------------------------------------------------------------------------------------------------

void CellularAutomata::printfunc(void)
{
    /*int ii = 0;

    printf("W[0](p) = rand[0,100) < %i\n", fillprob);

    //for(ii=0; ii<generations; ii++)
    //	{
    printf("Repeat %i: W'(p) = R[1](p) >= %i",
        params_set[ii].reps, params_set[ii].r1_cutoff);

    if (params_set[ii].r2_cutoff >= 0)
        printf(" || R[2](p) <= %i\n", params_set[ii].r2_cutoff);
    else
        putchar('\n');*/
    //	}
}

// --------------------------------------------------------------------------------------------------------------------------------

void CellularAutomata::printmap(void)
{
    int y, x;

    for (x = 0; x < mSizeX; x++)
    {
        for (y = 0; y < mSizeY; y++)
        {
            switch (grid[x][y]) {
            case TILE_WALL:  std::cout << '#'; break;
            case TILE_FLOOR: std::cout << '.'; break;
            }
        }
        std::cout << std::endl;
    }
}

// --------------------------------------------------------------------------------------------------------------------------------

int CellularAutomata::getCell(const unsigned int x, const unsigned int y) const
{
    if (x >= DUNGEON_SIZE_W || y >= DUNGEON_SIZE_H)
        throw std::exception("Invalid size for CellularAutomata - out of bounds.");

    return grid[x][y];
}

// --------------------------------------------------------------------------------------------------------------------------------

int CellularAutomata::Clean()
{
    return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------

void CellularAutomata::setSize(const unsigned int x, const unsigned int y)
{
    if (x >= DUNGEON_SIZE_W || y >= DUNGEON_SIZE_H)
        throw std::exception("Invalid size for CellularAutomata - out of bounds.");

    mSizeX = x;
    mSizeY = y;
}

// --------------------------------------------------------------------------------------------------------------------------------

//this now has flood fill and rejects unconnected parts (as well as small dungeons)
int CellularAutomata::Create()
{
    int ii = 0, jj;

    params.r1_cutoff = r1_cutoff;
    params.r2_cutoff = r2_cutoff;
    params.reps = reps;

    int good_map_percentage = 40;
    int good_map = 0;
    static int num_failed = 0;
    static int num_generated = 0;

    while (!good_map)
    {
        initmap();

        for (jj = 0; jj < params.reps; jj++)
        {
            current_rep = jj;
            generation();
        }
        // printfunc();
        // printmap();

        //clear grid 2
        for (int x = 0; x < mSizeX; x++) //put wall everywhere (grid 2)
        {
            for (int y = 0; y < mSizeY; y++)
                grid2[x][y] = TILE_WALL;
        }

        int found = 0;
        int x;
        int y;
        while (!found)
        {
            x = Random::getInt(mSizeX, 1);
            y = Random::getInt(mSizeY, 1);

            if (grid[x][y] == TILE_FLOOR)
                found = 1;
        }

        //The following code uses flood fill to delete disconnected parts
        //First flood fill grid2, then compare each cell with grid1, if both cells are the same then keep it.
        // This essentially is grid1-grid2. Leaving only the connected cells, disconnected cells are turned to walls.

        FloodFill(x, y);	//flood fill grid2 with connected parts

        for (int x = 0; x < mSizeX; x++)	//and flood fill with map
        {
            for (int y = 0; y < mSizeY; y++)
            {
                if (grid[x][y] == TILE_FLOOR && grid2[x][y] == TILE_FLOOR)
                {
                    grid[x][y] = TILE_FLOOR;
                }
                else
                    grid[x][y] = TILE_WALL;
            }
        }

        //Have a percentage test, if enough usable space. Default is 32% filled.
        int total = 0;
        int count = 0;
        for (int x = 0; x < mSizeX; x++)
        {
            for (int y = 0; y < mSizeY; y++)
            {
                if (grid[x][y] == TILE_FLOOR)
                    count++;
                total++;
            }
        }

        float per = (float)count / (float)total;

        //std::cout << "Count: "<<count<<"/"<<total<<"="<<per*100<<"%"<<std::endl;
        if (per * 100 > 32)
        {
            good_map = true;
        }
        else
        {
            num_failed++;
            Clean();
        }
        num_generated++;

        // printfunc();
        // printmap();
    }

#ifdef _DEBUG
    if (num_failed)
        std::cout << "CellularAutomata>> Failed Maps: " << num_failed << "/" << num_generated << "=" << ((float)num_failed / (float)num_generated) * 100 << "%%" << std::endl;
#endif
    //test completeness
    //AND grid 1 with grid 2 to get
    return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------
