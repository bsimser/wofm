// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#include "DungeonGenerator.h"
#include "Dungeonlevel.h"
#include "NumberGenerator.h"
#include <math.h>
#include <vector>
#include <cassert>

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::Create(int _type)
{
    OpenTrace(true);

    if (_type == 0)
        type = lOutside;

    else if (_type == 7)
        type = lUndead;

    else if (_type == 8)
        type = lMaze;

    else if (_type == 4)
        type = lRiver;

    else if (_type == 9)
        type = lEnd;

    else if (_type > 9)
        type = lSpecial;

    else
        type = lNormal;

    SafetyReset();

    CreateMap();
    fixDoors();

    DumpDungeon();

#ifdef _DEBUG
    //int cX, cY;
    //assert(findTerrainType(cX, cY, '>'));

    //assert(type == lOutside || findTerrainType(cX, cY, '<'));
#endif
    return type;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::CreateMap()
{
    int w, h;

    //Fill map with solid earth
    for (h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (w = 0; w < DUNGEON_SIZE_W; w++)
        {
            dmap.setCell(w, h, '#');
        }
    }

    if (type == lCavern)
    {
        makeCavernDungeon();
        //makeOutSideDungeon();
    }

    if (type == lNormal)
    {
        makeNormalDungeon();
        AddFountain();
        AddTeleport();

        if (!FloodTest())
        {
            DumpDungeon();
            return CreateMap();
        }
        if (Random::getInt(5,0) == 0)
            addSpecialRoom();
    }
    else if (type == lSpecial)
    {
        int ret = makeSpecialDungeon();
        AddTeleport();

        if (!FloodTest() || ret == 0)
        {
            DumpDungeon();
            return CreateMap();
        }
    }
    else if (type == lUndead)
    {
        makeUndeadDungeon();
        AddFountain();
        AddTeleport();

        if (!FloodTest())
        {
            DumpDungeon();
            return CreateMap();
        }
    }

    else if (type == lOutside)
    {
        makeOutSideDungeon();
    }

    else if (type == lMaze)
    {
        makeMazeDungeon();
        AddFountain();
        AddTeleport();
        AddTeleport();
        AddTeleport();

        if (!FloodTest())
        {
            DumpDungeon();
            return CreateMap();
        }

    }
    else if (type == lRiver)
    {
        makeRiverDungeon();

        for (int i = 0; i < 15; i++)
            makeRiver(Random::getInt(35, 25));

        makeBridge();
        if (!FloodTest())
        {
            DumpDungeon();
            return CreateMap();
        }
    }

    else if (type == lEnd)
    {
        makeNormalDungeon();
        AddFountain();
        AddTeleport();
        if (!FloodTest())
        {
            DumpDungeon();
            return CreateMap();
        }
    }

    int cX, cY;
    if (!findTerrainType(cX, cY, '>'))
    {
        if (!findTerrainType(cX, cY, '.'))
        {
            DumpDungeon();

            throw std::exception("Failed to create exit for level");
        }
        dmap.setCell(cX, cY, '>');
    }

#ifdef _DEBUG
    DumpDungeon();

   /* //printf dungeon
    std::ofstream ofile;

    ofile.open("Debug Files\\last_dungeon.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open dungeon.txt");

    ofile << "Dungeon Level " << lRiver << " For debug purposes only" << std::endl;

    for (h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (w = 0; w < DUNGEON_SIZE_W; w++)
            ofile << dmap.getCell(w, h);
        ofile << std::endl;
    }

    ofile.close();*/

#endif
    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::DumpDungeon()
{
#ifdef _DEBUG
    std::ofstream ofile;
    std::ofstream ofile2;

    ofile.open("Debug Files\\last_dungeon.txt");
    ofile2.open("Debug Files\\flood.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open dungeon.txt");

    ofile << "Dungeon Level type: " << type << ". For debug purposes only" << std::endl;

    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            ofile << dmap.getCell(w, h);
            ofile2 << flood_test.getCell(w, h);
        }
        ofile << std::endl;
        ofile2 << std::endl;
    }

    ofile.close();
#endif
    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::makeEncounterDungeon()
{
    //while(!createUndeadRoom(Random::getInt(20,35),15));

    int w_size = Random::getInt(28, 28);
    int h_size = Random::getInt(8, 8);

    //create room

    int ww = Random::getInt(35, 20);

    buildRoom(ww, 15, w_size, h_size, 0);
    buildRoom(ww, 15, h_size, w_size / 2, 0);

    int created = 0;
    //make exit
    for (int w = 0; w < DUNGEON_SIZE_W; w++)
    {
        if (dmap.getCell(w, 15) == '.')
        {
            dmap.setCell(w, 15, '>');
            created = 1;
            break;
        }
    }
    if (!created)
    {
        DumpDungeon();
        throw std::exception("Cound not add exit to encounter dungeon");
    }

    created = 0;

    //make entrance
    for (int w = DUNGEON_SIZE_W - 1; w > 0; w--)
    {
        if (dmap.getCell(w, 15) == '.')
        {
            dmap.setCell(w, 15, '<');
            created = 1;
            break;
        }
    }
    if (!created)
    {
        DumpDungeon();

        throw std::exception("Could not add entrance to encounter dungeon");
    }

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::makeUndeadDungeon()
{
    //create dungeon
    const int max_rooms = Random::getInt(7, 5);
    int rooms(0);

    //create rooms
    int dungeon_full = 0;
    SafetyReset();

    while (rooms < max_rooms + 1)
    {
        if (createUndeadRoom(rooms, max_rooms, Random::getInt(MAX_X_POS, MIN_X_POS), Random::getInt(MAX_Y_POS, MIN_Y_POS)))
            rooms++;

        if (dungeon_full++ == 1000) //safety feature - no room left so add stairs
        {
            break;
        }
    }

    // add river
    int make_river = Random::getInt(10, 0);
    int complexity = Random::getInt(3, 0);
    if (make_river > 7)
    {
        for (int i = 0; i < complexity; i++)
            makeRiver(Random::getInt(10, 0) + 30);
    }

    //add tombs
    for (int i = 0; i < 50; i++)
    {
        int h = Random::getInt(DUNGEON_SIZE_H - 3, 3);
        int w = Random::getInt(DUNGEON_SIZE_W - 3, 3);
        makeTomb(w, h);
    };

   return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

bool DungeonGenerator::findTerrainType(int &x, int &y, const char terrain)
{
    std::vector<Coord> terrainCoords;
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap.getCell(w, h) == terrain)
            {
                Coord newCoord;
                newCoord.x = w;
                newCoord.y = h;
                terrainCoords.push_back(newCoord);
            }
        }
    }
    if (terrainCoords.size() > 0)
    {
        int nRandom = Random::getInt(0, terrainCoords.size());

        x = terrainCoords[nRandom].x;
        y = terrainCoords[nRandom].y;
        return true;
    }

    return false;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::makeCavernDungeon()
{
    //centre cavern at 125,125
    int range = 75;
    int x = DUNGEON_SIZE_W / 8;
    int y = DUNGEON_SIZE_H / 8;

    //make entrance
    dmap.setCell(0, 0, 'g');
    dmap.setCell(1, 0, 'g');
    dmap.setCell(2, 0, 'g');
    dmap.setCell(3, 0, 'g');

    makeGrass(0, 0); makeGrass(0, 1); makeGrass(0, 2); makeGrass(0, 3); makeGrass(0, 4);
    makeGrass(1, 0); makeGrass(1, 1); makeGrass(1, 2); makeGrass(1, 3);
    makeGrass(1, 0); makeGrass(2, 0); makeGrass(3, 0); makeGrass(4, 0); makeGrass(5, 0);
    makeGrass(1, 1); makeGrass(2, 1); makeGrass(3, 1); makeGrass(4, 1);

    //make cavern
    int max_h = DUNGEON_SIZE_H / 4 + Random::getInt(20, 0);
    int max_w = DUNGEON_SIZE_W / 4 + Random::getInt(20, 0);

    for (int h = 0; h < max_h; h++)
    {
        for (int w = 0; w < max_w; w++)
        {
            int r = (int)sqrt((float)(x - w)*(x - w) + (h - y)*(h - y)) + Random::getInt(3, 0);;
            if (r < range) //square is outside radius
                makeGrass(w, h);
        }
    }

    max_h = DUNGEON_SIZE_H / 4 + Random::getInt(20, 0) + DUNGEON_SIZE_H / 4;
    max_w = DUNGEON_SIZE_W / 4 + Random::getInt(20, 0);
    x = DUNGEON_SIZE_W / 8;
    y = DUNGEON_SIZE_H / 8 + DUNGEON_SIZE_H / 4;;

    //make lake

    makeRiver(Random::getInt(40, 65), Random::getInt(8, 5));
    makeRiver(Random::getInt(35, 55));

    makeRiver(Random::getInt(200, 200), 30);

    for (int h = DUNGEON_SIZE_H / 4; h < max_h; h++)
    {
        for (int w = 0; w < max_w; w++)
        {
            int r = (int)sqrt((float)(x - w)*(x - w) + (h - y)*(h - y)) + Random::getInt(3, 0);;
            if (r < range / 2) //square is outside radius
            {
                //makeGrass(w,h);
                dmap.setCell(w, h,  'w');
                makeSand(w + 1, h + 1);
                makeSand(w + 1, h);
                makeSand(w, h + 1);
                makeSand(w - 1, h);
            }
        }
    }

    for (int i = 1; i < range / 4; i++)
    {
        makeGrass(i, i);
        makeGrass(i, i + 1);
        makeGrass(i + 1, i);
    }
    //make river

    //	makeRiver(Random::getInt(30,30),20);
    //  int h,w;
    /*	int top = Random::getInt(10,0)+30;

        //create grass
        for( h =0;h<DUNGEON_SIZE_H;h++)
        for( w=0;w<DUNGEON_SIZE_W;w++)
        dmap.setCell(w, h,  'g');

        */
    //plant trees
    for (int i = 0; i < 1000; i++)
    {
        int h = Random::getInt(DUNGEON_SIZE_H, 0);
        int w = Random::getInt(DUNGEON_SIZE_W, 0);
        makeTree(w, h);
    }

    //more rocks
    for (int i = 0; i < 500; i++)
    {
        int h = Random::getInt(DUNGEON_SIZE_H, 0);
        int w = Random::getInt(DUNGEON_SIZE_W, 0);
        if (w != 2 && h != 2) //player pos
            dmap.setCell(w, h, '#');
    }

    //create dungeon
    const int max_rooms = Random::getInt(50, 18);

    //create rooms
    for (int i = 0; i < max_rooms; i++)
    {
        int x = Random::getInt(100, 10);
        int y = Random::getInt(100, 10);

        if (x < 20 && y < 20)
        {
            i--;
            continue;
        }

        int w_size = Random::getInt(10, 4) + 2;
        int h_size = Random::getInt(10, 4) + 2;

        if ((w_size + x) >= DUNGEON_SIZE_W || (w_size - x) >= 0 || (h_size + y) >= DUNGEON_SIZE_H || (h_size - y) >= 0)
        {
            i--;
            continue;
        }

        int river = 0;
        for (int h = 0; h < h_size; h++)
        {
            for (int w = 0; w < w_size; w++)
            {
                if (dmap.getCell(x + w - w_size / 2, y + h - h_size / 2) == 's' || dmap.getCell(x + w - w_size / 2, y + h - h_size / 2) == '=')
                {
                    river = 1;
                }
            }
        }

        if (river)
        {
            i--;
            continue;
        }

        //make square room
        for (int h = 0; h < h_size; h++)
        {
            for (int w = 0; w < w_size; w++)
            {
                makeFloor(x + w - w_size / 2, y + h - h_size / 2);
                //dmap.getCell(x+w-w_size/2, y+h-h_size/2, '.'); 
                if (h == 0 || w == 0 || w == w_size - 1 || h == h_size - 1)
                    dmap.setCell(x + w - w_size / 2, y + h - h_size / 2, '#');
            }
        }

        //create doors
        if (Random::getInt(2, 0) && dmap.getCell(x - w_size / 2, y) == '#')
            dmap.setCell(x - w_size / 2, y + Random::getInt(2, 0), '+');		//left

        else
            dmap.setCell(x + Random::getInt(2, 0), y - h_size / 2, '+');	//top

        if (Random::getInt(2, 0) && dmap.getCell(x + w_size / 2, y) == '#')
            dmap.setCell(x + (w_size - 1) / 2, y + Random::getInt(2, 0), '+'); //right

        else
            dmap.setCell(x + Random::getInt(2, 0), y + (h_size - 1) / 2, '+');	//top
    }/*
    //make ruins

    createRuin(Random::getInt(20,10) ,Random::getInt(25,5),4,3,4,3);
    createRuin(Random::getInt(20,10) ,Random::getInt(25,5),5,4,5,4);


    // make mountain
    makeMountain();

    //make river*/
    //	makeRiver(Random::getInt(35,65));
    /*	makeRiver(Random::getInt(30,30));


        SafetyReset();

        do //make entrance
        {
        h = Random::getInt(DUNGEON_SIZE_H-2,10);
        w = Random::getInt(DUNGEON_SIZE_W-2,2);
        Safety("Could not add entrance to mountain");

        }while(!(dmap.getCell(w, h) == 'M' && (dmap.getCell(w-1, h) == 'g' || dmap.getCell(w-1, h) == 's')));

        dmap.setCell(w, h,  '>');

        */
    //make bridge
    //	for( i=w;i>0;i--)
    //	if(dmap.getCell(i, h) == '=')
    //		dmap.getCell(i, h, 'b');

    int h = Random::getInt(range, 20);

    for (int i = 0; i < range * 2; i++)
    if (dmap.getCell(i, h) == '=')
        dmap.setCell(i, h, 'b');

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::makeOutSideDungeon()
{
    int h, w;
    int top = Random::getInt(10, 0) + 30;

    //create grass
    for (h = 0; h < DUNGEON_SIZE_H; h++)
    for (w = 0; w < DUNGEON_SIZE_W; w++)
        dmap.setCell(w, h, 'g');

    //plant trees
    for (long i = 0; i < 30; i++)
    {
        h = Random::getInt(DUNGEON_SIZE_H, 0);
        w = Random::getInt(DUNGEON_SIZE_W / 2 + 15, 0);
        dmap.setCell(w, h, 'T');
    }

    //make ruins
    createRuin(Random::getInt(20, 10), Random::getInt(25, 5), 4, 3, 4, 3);
    createRuin(Random::getInt(20, 10), Random::getInt(25, 5), 5, 4, 5, 4);

    //plant rocks //can box in entrance
    for (int i = 0; i < 15; i++)
    {
        h = Random::getInt(DUNGEON_SIZE_H, 0);
        w = Random::getInt(DUNGEON_SIZE_W, DUNGEON_SIZE_W / 2);
        dmap.setCell(w, h, '#');
    }
    //more rocks
    for (int i = 0; i < 15; i++)
    {
        h = Random::getInt(DUNGEON_SIZE_H, 0);
        w = Random::getInt(DUNGEON_SIZE_W / 2 + 10, 0);
        if (w > 2 && h > 3) //player pos
            dmap.setCell(w, h, '#');
    }

    // make mountain
    makeMountain();

    //make river
    makeRiver(Random::getInt(43, 43));
    makeRiver(Random::getInt(37, 37));

    SafetyReset();

    do //make entrance
    {
        h = Random::getInt(DUNGEON_SIZE_H - 2, 10);
        w = Random::getInt(DUNGEON_SIZE_W - 2, 2);
        Safety("Could not add entrance to mountain");
    } while (!(dmap.getCell(w, h) == 'M' && (dmap.getCell(w - 1, h) == 'g' || dmap.getCell(w - 1, h) == 's')));

    dmap.setCell(w, h, '>');

    //make bridge
    for (int i = w; i > 0; i--)
    if (dmap.getCell(i, h) == '=')
        dmap.setCell(i, h, 'b');

    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::SafetyReset()
{
    safety = 0;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::Safety(char * message, int test)
{
    safety++;
    if (safety > test)
    {
        throw std::exception(message);
    }
    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeMountain()
{
    int start = 43;
    int top = 43;

    int h = 0, w;
    for (h = 0; h < DUNGEON_SIZE_H; h++)
    {
        int rand = Random::getInt(7, 1) - 3;
        top += rand;

        //make water
        for (w = top; w < DUNGEON_SIZE_W; w++)
            dmap.setCell(w, h, 'M');
    }
}

int DungeonGenerator::makeRiverDungeon()
{
    //create dungeon
    //create rooms
    const int max_rooms = Random::getInt(4, 3);
    int rooms(0);
    int dungeon_full = 0;
    int left = 1;

    while (rooms < max_rooms + 1)
    {
        if (left)
        {
            if (createRoom(rooms, max_rooms, 7, Random::getInt(MAX_Y_POS, MIN_Y_POS)))
            {
                rooms++;
                left = 0;
            }
        }
        else
        {
            if (createRoom(rooms, max_rooms, 60, Random::getInt(MAX_Y_POS, MIN_Y_POS)))
            {
                rooms++;
                left = 1;
            }
        }

        if (dungeon_full++ == 1000) //safety feature - no room left so add stairs
        {
            break;
        }
    }
    int cX, cY;
    if (!findTerrainType(cX, cY, '.'))
        throw std::exception("Failed to create exit for level");

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::makeMazeDungeon()
{
    //create dungeon
    const int max_rooms = 30;
    int rooms(0);

    //create rooms
    int dungeon_full = 0;

    while (rooms < max_rooms + 1)
    {
        if (createRoom(rooms, max_rooms, Random::getInt(MAX_X_POS, MIN_X_POS), Random::getInt(MAX_Y_POS, MIN_Y_POS), 5, 3, 5, 3))
            rooms++;

        if (dungeon_full++ == 1000) //safety feature - no room left so add stairs
        {
            break;
        }
    }

    // add river
    int make_river = Random::getInt(10, 0);
    //int complexity = Random::getInt(3,0);
    if (make_river == 9)
    {
        makeRiver(Random::getInt(10, 0) + 30);
    }

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::makeNormalDungeon()
{
    //create dungeon
    const int max_rooms = Random::getInt(MAX_ROOMS, MIN_ROOMS);
    assert(max_rooms >= MIN_ROOMS);
    int rooms(0);

    //create rooms
    SafetyReset();
    int dungeon_full = 0;

    while (rooms < max_rooms + 1)
    {
        if (createRoom(rooms, max_rooms, Random::getInt(MAX_X_POS, MIN_X_POS), Random::getInt(MAX_Y_POS, MIN_Y_POS)))
            rooms++;

        if (dungeon_full++ == 1000) //safety feature - no room left so add stairs
        {
            break;
        }
    }

    //add loop corridor.
    addLoops(5);

    // add river
    int make_river = Random::getInt(10, 0);
    int complexity = Random::getInt(3, 0);
    if (make_river > 7)
    {
        for (int i = 0; i < complexity; i++)
            makeRiver(Random::getInt(10, 0) + 30);
    }

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeBoatHouse()
{
    makeSpecialDungeon();
    
    DumpDungeon();
    
    addLoops(2);
    makeRiver(Random::getInt(10, 0) + 30, 0);
    DumpDungeon();

    makeBridge();
    DumpDungeon();
    makeBridge();
    DumpDungeon();
    makeBridge();
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::makeSpecialDungeon()
{
    //create dungeon
    const int max_rooms = Random::getInt(MAX_ROOMS, MIN_ROOMS);
    int rooms(0);
    assert(max_rooms >= MIN_ROOMS);

    //create rooms
    SafetyReset();

    while (rooms < max_rooms + 1)
    {
        if (createRoom(rooms, max_rooms, Random::getInt(MAX_X_POS, MIN_X_POS), Random::getInt(MAX_Y_POS, MIN_Y_POS)))
            rooms++;

        Safety("make Special Dungeon failed", 10000);
    }

    addLoops(2);

    // add river
    int make_river = Random::getInt(10, 0);
    int complexity = Random::getInt(3, 0);
    if (make_river > 7)
    {
        for (int i = 0; i < complexity; i++)
            makeRiver(Random::getInt(10, 0) + 30);
    }

    AddFountain();

    COORDLIST specialSpotCoords;
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap.getCell(w, h) == '.' && dmap.getCell(w + 1, h) == '.' && dmap.getCell(w - 1, h) == '.' && dmap.getCell(w, h + 1) == '.' && dmap.getCell(w, h - 1) == '.'
                && dmap.getCell(w + 1, h + 1) == '.' && dmap.getCell(w - 1, h + 1) == '.' && dmap.getCell(w - 1, h - 1) == '.' && dmap.getCell(w + 1, h - 1) == '.')
            {
                Coord new_Coord; new_Coord.x = w; new_Coord.y = h;
                specialSpotCoords.push_back(new_Coord);
            }
        }
    }

    int     size = specialSpotCoords.size();
    int     i = 0;
    bool    placed = false;
    assert(size > 0);

    if (size != 0)
    {
        int getCoord = Random::getInt(size, 0);
        for (COORDLIST::iterator it = specialSpotCoords.begin(); it != specialSpotCoords.end(); it++, i++)
        {
            if (getCoord == i)
            {
                int x = it->x;
                int y = it->y;
                dmap.setCell(it->x, it->y, 'i');
                placed = true;
                break;
            }
        }
    }
    else
    {
        return 0;
    }
    assert(placed);

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::createRuin(int x, int y, int max_w, int min_w, int max_h, int min_h)
{
    int w_size = Random::getInt(max_w, min_w);
    int h_size = Random::getInt(max_h, min_h);

    //check that it is a good spot to put the room 

    if (!validRoomPos(x, y, w_size, h_size))
        return 0;

    //create room

    for (int h = 0; h < h_size; h++)
    {
        for (int w = 0; w < w_size; w++)
        {
            dmap.setCell(x + w - w_size / 2, y + h - h_size / 2, '.');

            if (w == w_size - 1 && Random::getInt(2, 0))
                dmap.setCell(x + w - w_size / 2 + 1, y + h - h_size / 2, 'r');
            if (w == 0 && Random::getInt(2, 0))
                dmap.setCell(x + w - w_size / 2 - 1, y + h - h_size / 2, 'r');
            if (h == h_size - 1 && Random::getInt(2, 0))
                dmap.setCell(x + w - w_size / 2, y + h - h_size / 2 + 1, 'r');
            if (h == 0 && Random::getInt(2, 0))
                dmap.setCell(x + w - w_size / 2, y + h - h_size / 2 - 1, 'r');
        }
    }
    if (Random::getInt(2, 0))
        dmap.setCell(x + w_size / 2 + 1, y - h_size / 2 - 1, 'r');
    if (Random::getInt(2, 0))
        dmap.setCell(x + w_size / 2 + 1, y + h_size / 2 + 1, 'r');
    if (Random::getInt(2, 0))
        dmap.setCell(x - w_size / 2 - 1, y - h_size / 2 - 1, 'r');
    if (Random::getInt(2, 0))
        dmap.setCell(x - w_size / 2 - 1, y + h_size / 2 + 1, 'r');

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::createRoom(int roomCount, const int maxRooms, int x, int y, int max_w, int min_w, int max_h, int min_h)
{
    int w_size = Random::getInt(max_w, min_w);
    int h_size = Random::getInt(max_h, min_h);

    //check that it is a good spot to put the room 

    if (!validRoomPos(x, y, w_size, h_size))
        return 0;

    //create room
    buildRoom(x, y, w_size, h_size, 0);

    if (roomCount) //more than one room make a path
    {
        /*TraceMsg("Room Succeeded");
        char buf[32];
        sprintf(buf,"x%d y%d x%d y%d",x,y,last_room_x,last_room_y);
        TraceMsg(buf);*/
        createPath(x, y, last_room_x, last_room_y);
    }

    //label room - debug
    {
        //	char id = 'a');
        //	id +=rooms;
        //	dmap.getCell(x, y) = id;
    }

    //add exit/entry
    if (roomCount == 0) //start room
    {
        if (type != lSpecial)
            dmap.setCell(x, y, '<');
        else
            dmap.setCell(x, y, '(');
    }
    if (roomCount == maxRooms && type != lSpecial)
    {
        dmap.setCell(x, y, '>');
    }

    //set up last know room to link next room to
    last_room_x = x;
    last_room_y = y;
    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::buildRoom(int x, int y, int w_size, int h_size, int type)
{
    switch (type)
    {
    case 0: //makeSquareRoom();
        break;
    }

    //make square room
    for (int h = 0; h < h_size; h++)
    {
        for (int w = 0; w < w_size; w++)
        {
            makeFloor(x + w - w_size / 2, y + h - h_size / 2);
            //dmap.getCell(x+w-w_size/2, y+h-h_size/2, '.'); 
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::validRoomPos(int x, int y, int w_size, int h_size)
{
    //check that room is not next to limit
    if ((w_size + x) >= DUNGEON_SIZE_W || (w_size - x) >= 0 || (h_size + y) >= DUNGEON_SIZE_H || (h_size - y) >= 0)
    {
        //TraceMsg("Room Failed - dungeon limit");
        return 0;
    }

    //check that room is not inside another room or adjacent
    for (int h = 0; h < h_size + 2; h++)
    {
        for (int w = 0; w < w_size + 2; w++)
        {
            if (dmap.getCell(x + w - (w_size + 2) / 2, y + h - (h_size + 2) / 2) == '.' || dmap.getCell(x + w - (w_size + 2) / 2, y + h - (h_size + 2) / 2) == '+')
            {
                //TraceMsg("Room Failed - touching existing room");
                return 0;
            }
        }
    }
    return 1; //valid position
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::createDoor(int x, int y, int chance)
{
    int random = Random::getInt(100, 0);

    //door sanity check (not yet perfect)
    if (!((dmap.getCell(x + 1, y) != '#' &&  dmap.getCell(x - 1, y) != '#') || (dmap.getCell(x, y - 1) != '#' &&  dmap.getCell(x, y + 1) != '#')))
        return 0;

    if (((dmap.getCell(x + 1, y) != '#' &&  dmap.getCell(x - 1, y) != '#') && (dmap.getCell(x, y - 1) != '#' &&  dmap.getCell(x, y + 1) != '#')))
        return 0;

    if (random < chance) //make door
        dmap.setCell(x, y, '+');
    //else do nothing

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

//this would have to be the worst path finder routine ever. Hell it works! (most of the time)
int DungeonGenerator::createPath(int x1, int y1, int x2, int y2)
{
    int dir1;
    int dir2;
    //go from x1 to x2
    SafetyReset();

    //find direction

    if (x1 > x2)
        dir1 = dWest;
    else if (x1 == x2)
        dir1 = dNorth;
    else if (x1 < x2)
        dir1 = dEast;

    if (y1 > y2)
        dir2 = dSouth;
    else if (y1 == y2)
        dir2 = dNorth;
    else if (y1 < y2)
        dir2 = dNorth;

    int start_dir = Random::getInt(2, 1);

    if (start_dir == 1 && dir1 == dNorth) //??
        start_dir = 2;

    if (start_dir == 2 && dir2 == dNorth) //??

        start_dir = 1;

    if (start_dir == 1)
        TraceMsg("Start Dir W/E");
    if (start_dir == 2)
        TraceMsg("Start Dir N/S");

    if (start_dir == 1) //use dir 1 to start;
    {
        while (dmap.getCell(x1, y1) == '.' && dir1 != dNorth) //find wall of start room
        {
            if (dir1 == dWest) x1--;
            else if (dir1 == dEast) x1++;

            Safety("could not find wall of start room");
        }

        SafetyReset();
        //starting point found make path
        do //make path in x axis
        {
            if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                dmap.setCell(x1, y1, '.');

            if (dir1 == dWest) x1--;
            else if (dir1 == dEast) x1++;

            //if(dmap.getCell(x1, y1) == '.') //another path encountered
            if (isNearWall(x1, y1))
            {
                if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                    dmap.setCell(x1, y1, '.');
                TraceMsg("another path encountered going W/E");
                createDoor(x1, y1, 100);
                return 0;
            }

            if (dir1 == dEast && x1 > x2) break;
            if (dir1 == dWest && x1 < x2) break;

            Safety("could not make path in x axis");

        } while (x1 != x2);

        SafetyReset();

        while (y1 != y2) //make path in y axis
        {
            if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                dmap.setCell(x1, y1, '.');

            if (dir2 == dNorth) y1++;
            else if (dir2 == dSouth) y1--;

            //if(dmap.getCell(x1, y1) == '.')
            if (isNearWall(x1, y1)) //another path encountered
            {
                if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                    dmap.setCell(x1, y1, '.');
                TraceMsg("another path encountered going N/S");
                createDoor(x1, y1, 100);
                return 0;
            }
            Safety("could not make path in y axis");
        }

        SafetyReset();
        if (x1 != x2)
        {
            if (x1 > x2)
            {
                TraceMsg("Fixing");
                while (x1 != x2)
                {
                    if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                        dmap.setCell(x1, y1, '.');
                    if (dir1 == dEast) x1--;	//reverse direction
                    else if (dir1 == dWest) x1++;

                    if (dmap.getCell(x1, y1) == '.')//another path encountered
                    {
                        createDoor(x1, y1, 100);
                        if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                            dmap.setCell(x1, y1, '.');
                        return 0;
                    }
                    Safety("Fixing in y axis");
                }
            }
            else if (x2 > x1)
            {
                TraceMsg("Fixing");
                while (x1 != x2)
                {
                    if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                        dmap.setCell(x1, y1, '.');
                    if (dir1 == dEast) x1--;	//reverse direction
                    else if (dir1 == dWest) x1++;

                    if (dmap.getCell(x1, y1) == '.')//another path encountered
                    {
                        if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                            dmap.setCell(x1, y1, '.');
                        createDoor(x1, y1, 100);
                        return 0;
                    }
                    Safety("Fixing in x axis");
                }
            }
        }
    }

    else if (start_dir == 2) //use dir 1 to start;
    {
        SafetyReset();
        while (dmap.getCell(x1, y1) == '.') //find wall
        {
            if (dir2 == dNorth) y1++;
            else if (dir2 == dSouth) y1--;
            Safety("X1");
        }

        do
        {
            if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                dmap.setCell(x1, y1, '.');

            if (dir2 == dNorth) y1++;
            else if (dir2 == dSouth) y1--;

            //if(dmap.getCell(x1, y1) == '.') //another path encountered
            if (isNearWall(x1, y1))
            {
                if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                    dmap.setCell(x1, y1, '.');
                createDoor(x1, y1, 100);
                return 0;
            }

            if (dir2 == dNorth && y1 > y2) break;
            if (dir2 == dSouth && y1 < y2) break;

            Safety("X2");
        } while (y1 != y2);

        SafetyReset();

        while (x1 != x2)
        {
            if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                dmap.setCell(x1, y1, '.');
            if (dir1 == dWest) x1--;
            else if (dir1 == dEast) x1++;

            //if(dmap.getCell(x1, y1) == '.') //another path encountered
            if (isNearWall(x1, y1))
            {
                if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                    dmap.setCell(x1, y1, '.');
                createDoor(x1, y1, 100);
                return 0;
            }
            Safety("Y1");
        }

        //check if path complete; //fix path
        if (y1 != y2)
        {
            if (y1 > y2)
            {
                TraceMsg("Fixing");
                while (y1 != y2)
                {
                    if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                        dmap.setCell(x1, y1, '.');
                    if (dir2 == dNorth) y1--;	//reverse direction
                    else if (dir2 == dSouth) y1++;

                    if (dmap.getCell(x1, y1) == '.') //another path encountered
                    {
                        if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                            dmap.setCell(x1, y1, '.');
                        createDoor(x1, y1, 100);
                        return 0;
                    }
                    Safety("Y3");
                }
            }
            else if (y2 > y1)
            {
                TraceMsg("Fixing");
                while (y1 != y2)
                {
                    if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                        dmap.setCell(x1, y1, '.');
                    if (dir2 == dNorth) y1--;	//reverse direction
                    else if (dir2 == dSouth) y1++;

                    if (dmap.getCell(x1, y1) == '.') //another path encountered
                    {
                        if (dmap.getCell(x1, y1) != '<' && dmap.getCell(x1, y1) != '>' && dmap.getCell(x1, y1) != 'i' && dmap.getCell(x1, y1) != ')'  && dmap.getCell(x1, y1) != '(')
                            dmap.setCell(x1, y1, '.');
                        createDoor(x1, y1, 100);
                        return 0;
                    }
                    Safety("Fixing Y3");
                }
            }

        }
    }

    return 1;
}

int DungeonGenerator::findwall(int *x, int *y)
{
    bool found = false;
    SafetyReset();

    do
    {
        *x = Random::getInt(DUNGEON_SIZE_W, 0);
        *y = Random::getInt(DUNGEON_SIZE_H, 0);

        if (dmap.getCell(*x, *y) == '#')
        {
            if (dmap.getCell(*x + 1, *y) == '.') return dWest;
            if (dmap.getCell(*x, *y + 1) == '.') return dSouth;
            if (dmap.getCell(*x - 1, *y) == '.') return dEast;
            if (dmap.getCell(*x, *y - 1) == '.') return dNorth;
        }

        Safety("findwall");
    } while (!found);
    return 0; //never reach
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::createCorridor(int *x, int *y, int dir1)
{

    int dir = Random::getInt(2, 1);

    if (dir == 1)
    {
        for (int i = 0; i < 35; i++)
            dmap.setCell(35, i + 1, '.');
    }
    else
    {
        for (int i = 0; i < 85; i++)
            dmap.setCell(i + 2, 15, '.');
    }

    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::isNearWall(int x, int y)
{
    int found = 0; //found must be two or more to return true

    if (dmap.getCell(x + 1, y) != '#')
        found++;
    if (dmap.getCell(x - 1, y) != '#')
        found++;
    if (dmap.getCell(x, y + 1) != '#')
        found++;
    if (dmap.getCell(x, y - 1) != '#')
        found++;

    if (found > 1) 
        return 1;
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::OpenTrace(bool open)
{
    //	if(open)
    //	ofile.open("Debug Files\\dungeonTrace.txt");

    //	else
    //	//	ofile.close();

}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::TraceMsg(char * msg)
{
    //char id[1);
    //	_itoa(rooms,id,10);

    //	ofile << id[0] <<": " <<msg<< std::endl;

}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeRiver(int top, int size)
{
    //	int top = 
    int bottom = 35;//Random::getInt(60,0)+5;
    int path_side = Random::getInt(2, 0);

    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        int width = Random::getInt(5, 2) + size;
        int rand = Random::getInt(4, 1) - 2;
        top += rand;

        //	if(path_side==0)
        {
            //make sand left
            makeSand(top - (width / 2) - 1, h);
            makeSand(top - (width / 2) - 1, h + 1);
            makeSand(top - (width / 2), h + 1);
        }

        //	else
        {
            //make sand right
            if (width % 2 == 0)
            {
                makeSand(top + (width / 2), h);
                makeSand(top + (width / 2), h + 1);
            }
            else
            {
                makeSand(top + (width / 2) + 1, h);
                makeSand(top + (width / 2) + 1, h - 1);
            }
        }

        //make water
        for (int w = 0; w < width; w++)
        {
            makeWater(top - (width / 2) + w, h);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeWater(int x, int y)
{
    if (x >= DUNGEON_SIZE_W || y >= DUNGEON_SIZE_H || x < 0 || y < 0)
        return;

    if (dmap.getCell(x, y) == '>' || dmap.getCell(x, y) == '<' || dmap.getCell(x, y) == '(' || dmap.getCell(x, y) == 'i' || dmap.getCell(x, y) == ')')
        return;

    dmap.setCell(x, y, '=');

}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeSand(int x, int y)
{
    if (x >= DUNGEON_SIZE_W || y >= DUNGEON_SIZE_H || x < 0 || y < 0) return;

    if (dmap.getCell(x, y) == '>' || dmap.getCell(x, y) == '<' || dmap.getCell(x, y) == '=' || dmap.getCell(x, y) == 'w' || dmap.getCell(x, y) == '(' || dmap.getCell(x, y) == 'i' || dmap.getCell(x, y) == ')' )
        return;
    else
        dmap.setCell(x, y, 's');

}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeTomb(int x, int y)
{
    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0) return;

    if (dmap.getCell(x, y) == '.')
        dmap.setCell(x, y, 'u');
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeFloor(int x, int y)
{
    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0)return;

    dmap.setCell(x, y, '.');

}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeGrass(int x, int y)
{
    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0)return;

    dmap.setCell(x, y, 'g');
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeTree(int x, int y)
{
    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0)return;

    if (dmap.getCell(x, y) == '>' || dmap.getCell(x, y) == '<' || dmap.getCell(x, y) == '=' || dmap.getCell(x, y) == '#' || dmap.getCell(x, y) == 'w')
        return;

    dmap.setCell(x, y, 'T');
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::makeBridge()
{
    int x = 15, y;
    int inc = 1;

   // int dir = Random::getInt(2, 0);
   // if (dir == 1)
   //     inc = -1;

    for (y = 1; y < 15; y += inc)
    {
        if (dmap.getCell(x, y) == '.')
            break;
    }

    for (x = 1; x <= DUNGEON_SIZE_W-1; x++)
    {
        if (dmap.getCell(x, y) == '=')
        {
            if (dmap.getCell(x, y) != '>' && dmap.getCell(x , y) != '<' && dmap.getCell(x, y) != '(' && dmap.getCell(x, y) != 'i' && dmap.getCell(x , y) != ')')
                dmap.setCell(x, y, 'b');
            if (dmap.getCell(x - 1, y) != '>' && dmap.getCell(x - 1, y) != '<' && dmap.getCell(x - 1, y) != '(' && dmap.getCell(x - 1, y) != 'i' && dmap.getCell(x - 1, y) != ')')
                dmap.setCell(x - 1, y, 'b');
        }
    }
    //if (dmap.getCell(x + 1, y) != '>' && dmap.getCell(x + 1, y) != '<' && dmap.getCell(x + 1, y) != '(' && dmap.getCell(x + 1, y) != 'i' && dmap.getCell(x + 1, y) != ')')
    //    dmap.setCell(x + 1, y, 'b');
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::createUndeadRoom(int rooms, const int max_rooms, int x, int y, int max_w, int min_w, int max_h, int min_h)
{
    int w_size = Random::getInt(max_w, min_w);
    int h_size = Random::getInt(max_h, min_h);

    //check that it is a good spot to put the room 

    if (!validRoomPos(x, y, w_size, h_size))
        return 0;

    //create room

    buildRoom(x, y, w_size, h_size, 0);
    buildRoom(x, y - 1, h_size, w_size, 0);


    if (rooms) //more than one room make a path
    {
        createPath(x, y, last_room_x, last_room_y);
    }

    //add exit/entry
    if (rooms == 0) //start room
    {
        dmap.setCell(x, y, '<');
    }
    if (rooms == max_rooms)
    {
        dmap.setCell(x, y, '>');
    }

    // set up last know room to link next room to
    last_room_x = x;
    last_room_y = y;
    return 1;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::AddTeleport()
{
    //tele chance
    if (Random::getInt(2, 0) == 0) //33%
    {
        COORDLIST free_spaces;

        for (int x = 0; x < DUNGEON_SIZE_W; x++)
        {
            for (int y = 0; y < DUNGEON_SIZE_H; y++)
            {
                if (dmap.getCell(x, y) == '.')
                {
                    Coord new_Coord;
                    new_Coord.x = x;
                    new_Coord.y = y;
                    free_spaces.push_back(new_Coord);
                }
            }
        }
        if (free_spaces.size() > 0)
        {
            int choice = Random::getInt(free_spaces.size(), 0);
            int count = 0;
            for (COORDLIST::iterator it = free_spaces.begin(); it != free_spaces.end(); it++, count++)
            {
                if (count == choice)
                {
                    dmap.setCell(it->x, it->y, 'v');
                    return 1;
                }
            }
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::AddFountain()
{
    //fountain chance
    if (Random::getInt(3, 0) == 0) //33%
    {
        COORDLIST free_spaces;

        for (int x = 0; x < DUNGEON_SIZE_W; x++)
        {
            for (int y = 0; y < DUNGEON_SIZE_H; y++)
            {
                if (dmap.getCell(x, y) == '.')
                {
                    Coord new_Coord;
                    new_Coord.x = x;
                    new_Coord.y = y;
                    free_spaces.push_back(new_Coord);
                }
            }
        }
        if (free_spaces.size() > 0)
        {
            int choice = Random::getInt(free_spaces.size(), 0);
            int count = 0;
            for (COORDLIST::iterator it = free_spaces.begin(); it != free_spaces.end(); it++, count++)
            {
                if (count == choice)
                {
                    dmap.setCell(it->x, it->y, 'f');
                    return 1;
                }
            }
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------
GeneratorMap<char> DungeonGenerator::getMap()
{
    return dmap;
}

bool DungeonGenerator::FloodTest()
{
    COORDLIST free_spaces;

    int X = 0;
    int Y = 0;

    for (int x = 0; x < DUNGEON_SIZE_W; x++)
    {
        for (int y = 0; y < DUNGEON_SIZE_H; y++)
        {
            flood_test.setCell(x, y, '#');
            if (dmap.getCell(x, y) == '.')
            {
                Coord new_Coord;
                new_Coord.x = x;
                new_Coord.y = y;
                free_spaces.push_back(new_Coord);
            }
        }
    }
    if (free_spaces.size() > 0)
    {
        int choice = Random::getInt(free_spaces.size(), 0);
        X = free_spaces[choice].x;
        Y = free_spaces[choice].y;
    }

    if (X == 0 || Y == 0)
        return true;

    FloodFill(X, Y);

    int original_count = 0;
    int flood_count = 0;

    for (int x = 0; x < DUNGEON_SIZE_W; x++)
    {
        for (int y = 0; y < DUNGEON_SIZE_H; y++)
        {
            if ((dmap.getCell(x, y) != '#' && dmap.getCell(x, y) != 'r' && dmap.getCell(x, y) != 'M'))
            {
                original_count++;
            }
            if (flood_test.getCell(x, y) == '.')
            {
                flood_count++;
            }
        }
    }
    return (original_count == flood_count) ? true : false;
}

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::FloodFill(int x, int y)
{
    if (x < 0 || x >= DUNGEON_SIZE_W || y < 0 || y >= DUNGEON_SIZE_H)
    {
        return;
    }

    if ((dmap.getCell(x, y) != '#' && dmap.getCell(x, y) != 'r' && dmap.getCell(x, y) != 'M') && (flood_test.getCell(x, y) != '.'))
        flood_test.setCell(x, y, '.');
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

//---------------------------------------------------------------------------------------------------------------------------------

void DungeonGenerator::addLoops(int nLoops)
{
    int loops = nLoops; // Random::Random::getInt(nLoops + 1, 1);

    do
    {
        Coord c1, c2;

        do
        {
            int x = Random::getInt(DUNGEON_SIZE_W, 1);
            int y = Random::getInt(DUNGEON_SIZE_H, 1);

            if (dmap.getCell(x, y) == '.')
            {
                c1.x = x; c1.y = y;
                break;
            }

        } while (true);
        do
        {
            int x = Random::getInt(DUNGEON_SIZE_W, 1);
            int y = Random::getInt(DUNGEON_SIZE_H, 1);

            if (dmap.getCell(x, y) == '.')
            {
                c2.x = x; c2.y = y;
                break;
            }

        } while (true);

        createPath(c1.x, c1.y, c2.x, c2.y);
    } while (--loops);
}

//---------------------------------------------------------------------------------------------------------------------------------

int DungeonGenerator::fixDoors()
{
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap.getCell(w, h) == '+')
            {
                if (dmap.getCell(w + 1, h) == '#' && dmap.getCell(w - 1, h) == '#')
                    continue;
                else if (dmap.getCell(w, h+1) == '#' && dmap.getCell(w, h-1) == '#')
                    continue;

                dmap.setCell(w, h, '.');
            }
        }
    }
    return 0;
}

//---------------------------------------------------------------------------------------------------------------------------------

bool DungeonGenerator::addSpecialRoom()
{
    // find 5x5
    COORDLIST coords;

    for (int y = 5; y < DUNGEON_SIZE_H - 6; y++)
    {
        for (int x = 5; x < DUNGEON_SIZE_W - 6; x++)
        {
            bool good = true;
            if (dmap.getCell(x, y) == '.' && dmap.getCell(x + 1, y) == '.' && dmap.getCell(x - 1, y) == '.' && dmap.getCell(x, y + 1) == '.' && dmap.getCell(x, y - 1) == '.' &&
                dmap.getCell(x - 1, y - 1) == '.' && dmap.getCell(x - 1, y + 1) == '.' && dmap.getCell(x + 1, y - 1) == '.' && dmap.getCell(x + 1, y + 1) == '.' &&
                dmap.getCell(x - 2, y - 2) == '.' && dmap.getCell(x - 2, y - 1) == '.' && dmap.getCell(x - 2, y) == '.' && dmap.getCell(x - 2, y + 1) == '.' && dmap.getCell(x - 2, y + 2) == '.' &&
                dmap.getCell(x + 2, y - 2) == '.' && dmap.getCell(x + 2, y - 1) == '.' && dmap.getCell(x + 2, y) == '.' && dmap.getCell(x + 2, y + 1) == '.' && dmap.getCell(x + 2, y + 2) == '.' &&
                dmap.getCell(x - 1, y - 2) == '.' && dmap.getCell(x, y - 2) == '.' && dmap.getCell(x + 1, y - 2) == '.' &&
                dmap.getCell(x - 1, y + 2) == '.' && dmap.getCell(x, y + 2) == '.' && dmap.getCell(x + 1, y + 2) == '.')
            {
                coords.push_back(Coord(x, y));
            }
        }
    }

    if (coords.size() > 0)
    {
        unsigned int vChoice = Random::getInt(coords.size(), 0);
        int & X = coords[vChoice].x;
        int & Y = coords[vChoice].y;

        dmap.setCell(X, Y, 'S');

        int randomDoor = Random::getInt(4, 0);
        // door
        dmap.setCell(X + 1, Y, (randomDoor == 0) ? '+' : '#');
        dmap.setCell(X - 1, Y, (randomDoor == 1) ? '+' : '#');
        dmap.setCell(X, Y + 1, (randomDoor == 2) ? '+' : '#');
        dmap.setCell(X, Y - 1, (randomDoor == 3) ? '+' : '#');

        dmap.setCell(X + 1, Y - 1, '#');
        dmap.setCell(X + 1, Y + 1, '#');
        dmap.setCell(X - 1, Y - 1, '#');
        dmap.setCell(X - 1, Y + 1, '#');
    }

    return true;
}   

//---------------------------------------------------------------------------------------------------------------------------------
