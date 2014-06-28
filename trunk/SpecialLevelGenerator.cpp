// SpecialLevelGenerator.cpp: implementation of the SpecialLevelGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "SpecialLevelGenerator.h"
#include "Dungeonlevel.h"
#include "NumberGenerator.h"

using namespace Random;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpecialLevelGenerator::SpecialLevelGenerator()
{
}

SpecialLevelGenerator::~SpecialLevelGenerator()
{

}

int SpecialLevelGenerator::Create(int _type)
{
    int w, h;

    //Fill map with solid earth

    for (h = 0; h < DUNGEON_SIZE_H; h++)
    for (w = 0; w < DUNGEON_SIZE_W; w++)
        dmap[w][h] = '#';

    type = lSpecial;

    switch (_type)
    {
    case	slTrollCave: makeSpecialDungeon(); break;
    case	slPrison:	 makeSpecialDungeon(); break;
    case	slBoatHouse: makeSpecialDungeon(); break;
    case	slShop:		 makeSpecialDungeon(); break;
    case	slVampire:   makeSpecialDungeon(); break;
    case	slTresure:   makeSpecialDungeon(); break;
    case	slEncounter: makeEncounterDungeon(); break;
    case	slBarracks:  makeBarracksDungeon(); break;
    default: makeSpecialDungeon();

    }

    SafetyReset();

    DumpDungeon();

    if (!FloodTest())
    {
        DumpDungeon();
        return Create(_type);
    }
    //	CreateMap();

    return lSpecial;;
}

int SpecialLevelGenerator::CreateMap()
{
    return 1;
}

int SpecialLevelGenerator::makeBarracksDungeon()
{
    //create dungeon
    //create rooms
    const int max_rooms = getInt(MAX_ROOMS, MIN_ROOMS);
    int rooms(0); 
    
    int dungeon_full = 0;
    SafetyReset();

    //makeCoridoor()
    int randCorridoor_W = getInt(35, 45);

    for (int h = 2; h < 35; h++)
    {
        dmap[randCorridoor_W][h] = '.';
        dmap[randCorridoor_W + 1][h] = '.';
    }

    dmap[randCorridoor_W][getInt(10, 5)] = '(';

    while (rooms < max_rooms + 1)
    {
        int x = getInt(MAX_X_POS, MIN_X_POS);
        int y = getInt(MAX_Y_POS, MIN_Y_POS);
        int w_size = getInt(MAX_X_SIZE, MIN_X_SIZE);
        int h_size = getInt(MAX_Y_SIZE, MIN_Y_SIZE);

        //check that it is a good spot to put the room 
        Safety("make Barrack Dungeon failed", 10000);
        if (!validRoomPos(x, y, w_size, h_size))
            continue;

        //create room
        buildRoom(x, y, w_size, h_size, 0);
        rooms++;

        if (x < randCorridoor_W)
        {
            for (int w = x; w<randCorridoor_W; w++)
            {
                dmap[w][y] = '.';
            }
        }
        else if (x > randCorridoor_W)
        {
            for (int w = x; w>randCorridoor_W; w--)
            {
                dmap[w][y] = '.';
            }
        }
    }

    //BuildDoors
    for (int h = 2; h < DUNGEON_SIZE_H - 2; h++)
    {
        if (dmap[randCorridoor_W - 1][h] == '.')
            dmap[randCorridoor_W - 1][h] = '+';
        if (dmap[randCorridoor_W + 2][h] == '.')
            dmap[randCorridoor_W + 2][h] = '+';
    }

    makeSpecialItemSpot();

    return 1;
}

int SpecialLevelGenerator::makeSpecialItemSpot()
{
    COORDLIST specialSpotCoords;
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap[w][h] == '.' && dmap[w + 1][h] == '.' && dmap[w - 1][h] == '.' && dmap[w][h + 1] == '.' && dmap[w][h - 1] == '.'
                && dmap[w + 1][h + 1] == '.' && dmap[w - 1][h + 1] == '.' && dmap[w - 1][h - 1] == '.' && dmap[w + 1][h - 1] == '.')
            {
                coord new_coord; new_coord.x = w; new_coord.y = h;
                specialSpotCoords.push_back(new_coord);
            }
        }
    }
    int i = 0;

    if (specialSpotCoords.size() > 0)
    {
        int getCoord = getInt(specialSpotCoords.size(), 0);
        for (COORDLIST::iterator it = specialSpotCoords.begin(); it != specialSpotCoords.end(); it++, i++)
        {
            if (getCoord == i)
            {
                int x = it->x;
                int y = it->y;
                dmap[it->x][it->y] = 'i';
                break;
            }
        }
        return 1;
    }
    return 0;
}