// SpecialLevelGenerator.cpp: implementation of the SpecialLevelGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "SpecialLevelGenerator.h"
#include "CellularAutomata.h"
#include "Dungeonlevel.h"
#include "NumberGenerator.h"

using namespace Random;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

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
    case	slTrollCave:    makeCavern(); break;
    case	slPrison:	    makeSpecialDungeon(); break;
    case	slBoatHouse:    makeBoatHouse(); break;
    case	slShop:		    makeSpecialDungeon(); break;
    case	slVampire:      makeCrypt(); break;
    case	slTreasure:     makeSpecialDungeon(); break;
    case	slEncounter:    makeEncounterDungeon(); break;
    case	slBarracks:     makeBarracksDungeon(); break;
    case	slSpiderCave:   makeCavern(true); break;
    case	slUndead:       makeSpecialDungeon(); break;
        
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

int SpecialLevelGenerator::makeCrypt()
{
    makeUndeadDungeon();

    // remove '>' and '<'

    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap[w][h] == '<')
                dmap[w][h] = '(';
            else if (dmap[w][h] == '>')
                dmap[w][h] = '.';
        }
    }

    makeSpecialItemSpot();

    return 1;
}

void SpecialLevelGenerator::makeCavern(bool webby)
{
    CellularAutomata cavern;
    cavern.SetType(CA_LARGE_CAVERN);
    cavern.Create();

    for (int x = 0; x < DUNGEON_SIZE_W; x++)
    {
        for (int y = 0; y < DUNGEON_SIZE_H; y++)
        {
            dmap[x][y] = cavern.getCell(x, y) ? '#' : '.';
        }
    }

    int x, y;
    findTerrainType(x, y, '.');
    dmap[x][y] = ('(');
    makeSpecialItemSpot();

    if (webby)
    {
        for (int i = 0; i < 100; i++)
        {
            if (!findTerrainType(x, y, '.'))
                break;
            dmap[x][y] = 'W';
        }
    }
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
            for (int w = x; w > randCorridoor_W; w--)
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
    // put in room (or open space)
    COORDLIST specialSpotCoords;
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap[w][h] == '.' && dmap[w + 1][h] == '.' && dmap[w - 1][h] == '.' && dmap[w][h + 1] == '.' && dmap[w][h - 1] == '.'
                && dmap[w + 1][h + 1] == '.' && dmap[w - 1][h + 1] == '.' && dmap[w - 1][h - 1] == '.' && dmap[w + 1][h - 1] == '.')
            {
                Coord new_Coord; new_Coord.x = w; new_Coord.y = h;
                specialSpotCoords.push_back(new_Coord);
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