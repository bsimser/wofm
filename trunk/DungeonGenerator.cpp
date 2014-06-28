// DungeonGenerator.cpp: implementation of the DungeonGenerator class.
//
//////////////////////////////////////////////////////////////////////

#include "DungeonGenerator.h"
#include "Dungeonlevel.h"
#include "NumberGenerator.h"
#include <math.h>
#include <vector>
#include <cassert>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


using namespace Random;

DungeonGenerator::DungeonGenerator()
{
    getInt(10, 3);
}

DungeonGenerator::~DungeonGenerator()
{

}

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

    int cX, cY;
    assert(findTerrainType(cX, cY, '>'));

    assert(type == lOutside || findTerrainType(cX, cY, '<'));

    return type;
}

int DungeonGenerator::CreateMap()
{
    int w, h;

    //Fill map with solid earth
    for (h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (w = 0; w < DUNGEON_SIZE_W; w++)
        {
            dmap[w][h] = '#';
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
            makeRiver(getInt(35, 25));

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
            throw std::exception("Failed to create exit for level");
        dmap[cX][cY] = '>';
    }

#ifdef _DEBUG
    //printf dungeon
    std::ofstream ofile;

    ofile.open("Debug Files\\last_dungeon.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open dungeon.txt");

    ofile << "Dungeon Level " << lRiver << " For debug purposes only" << std::endl;

    for (h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (w = 0; w < DUNGEON_SIZE_W; w++)
            ofile << dmap[w][h];
        ofile << std::endl;
    }

    ofile.close();

#endif
    return 1;

}
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
            ofile << dmap[w][h];
            ofile2 << flood_test[w][h];
        }
        ofile << std::endl;
        ofile2 << std::endl;
    }

    ofile.close();
#endif
    return 1;
}

int DungeonGenerator::makeEncounterDungeon()
{
    //while(!createUndeadRoom(getInt(20,35),15));

    int w_size = getInt(28, 28);
    int h_size = getInt(8, 8);

    //create room

    int ww = getInt(35, 20);

    buildRoom(ww, 15, w_size, h_size, 0);
    buildRoom(ww, 15, h_size, w_size / 2, 0);

    int created = 0;
    //make exit
    for (int w = 0; w < DUNGEON_SIZE_W; w++)
    {
        if (dmap[w][15] == '.')
        {
            dmap[w][15] = '>';
            created = 1;
            break;
        }
    }
    if (!created) throw std::exception("Cound not add exit to enconter dungeon");

    created = 0;

    //make entrance
    for (int w = DUNGEON_SIZE_W - 1; w > 0; w--)
    {
        if (dmap[w][15] == '.')
        {
            dmap[w][15] = '<';
            created = 1;
            break;
        }
    }
    if (!created)
        throw std::exception("Could not add entrance to encounter dungeon");

    return 1;
}
int DungeonGenerator::makeUndeadDungeon()
{
    //create dungeon
    const int max_rooms = getInt(7, 5);
    int rooms(0);

    //create rooms
    int dungeon_full = 0;
    SafetyReset();

    while (rooms < max_rooms + 1)
    {
        if (createUndeadRoom(rooms, max_rooms, getInt(MAX_X_POS, MIN_X_POS), getInt(MAX_Y_POS, MIN_Y_POS)))
            rooms++;

        if (dungeon_full++ == 1000) //safety feature - no room left so add stairs
        {
            break;
        }
    }

    // add river
    int make_river = getInt(10, 0);
    int complexity = getInt(3, 0);
    if (make_river > 7)
    {
        for (int i = 0; i < complexity; i++)
            makeRiver(getInt(10, 0) + 30);
    }

    //add tombs
    for (int i = 0; i < 50; i++)
    {
        int h = getInt(DUNGEON_SIZE_H - 3, 3);
        int w = getInt(DUNGEON_SIZE_W - 3, 3);
        makeTomb(w, h);
    };

    return 1;
}

bool DungeonGenerator::findTerrainType(int &x, int &y, const char terrain)
{
    std::vector<coord> terrainCoords;
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap[w][h] == terrain)
            {
                coord newCoord;
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

int DungeonGenerator::makeCavernDungeon()
{
    //centre cavern at 125,125
    int range = 75;
    int x = DUNGEON_SIZE_W / 8;
    int y = DUNGEON_SIZE_H / 8;

    //make entrance
    dmap[0][0] = 'g';
    dmap[1][0] = 'g';
    dmap[2][0] = 'g';
    dmap[3][0] = 'g';

    makeGrass(0, 0); makeGrass(0, 1); makeGrass(0, 2); makeGrass(0, 3); makeGrass(0, 4);
    makeGrass(1, 0); makeGrass(1, 1); makeGrass(1, 2); makeGrass(1, 3);
    makeGrass(1, 0); makeGrass(2, 0); makeGrass(3, 0); makeGrass(4, 0); makeGrass(5, 0);
    makeGrass(1, 1); makeGrass(2, 1); makeGrass(3, 1); makeGrass(4, 1);

    //make cavern
    int max_h = DUNGEON_SIZE_H / 4 + getInt(20, 0);
    int max_w = DUNGEON_SIZE_W / 4 + getInt(20, 0);

    for (int h = 0; h < max_h; h++)
    {
        for (int w = 0; w < max_w; w++)
        {
            int r = (int)sqrt((float)(x - w)*(x - w) + (h - y)*(h - y)) + getInt(3, 0);;
            if (r < range) //square is outside radius
                makeGrass(w, h);
        }
    }

    max_h = DUNGEON_SIZE_H / 4 + getInt(20, 0) + DUNGEON_SIZE_H / 4;
    max_w = DUNGEON_SIZE_W / 4 + getInt(20, 0);
    x = DUNGEON_SIZE_W / 8;
    y = DUNGEON_SIZE_H / 8 + DUNGEON_SIZE_H / 4;;

    //make lake

    makeRiver(getInt(40, 65), getInt(8, 5));
    makeRiver(getInt(35, 55));

    makeRiver(getInt(200, 200), 30);

    for (int h = DUNGEON_SIZE_H / 4; h < max_h; h++)
    {
        for (int w = 0; w < max_w; w++)
        {
            int r = (int)sqrt((float)(x - w)*(x - w) + (h - y)*(h - y)) + getInt(3, 0);;
            if (r < range / 2) //square is outside radius
            {
                //makeGrass(w,h);
                dmap[w][h] = 'w';
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

    //	makeRiver(getInt(30,30),20);
    //  int h,w;
    /*	int top = getInt(10,0)+30;

        //create grass
        for( h =0;h<DUNGEON_SIZE_H;h++)
        for( w=0;w<DUNGEON_SIZE_W;w++)
        dmap[w][h] = 'g';

        */
    //plant trees
    for (int i = 0; i < 1000; i++)
    {
        int h = getInt(DUNGEON_SIZE_H, 0);
        int w = getInt(DUNGEON_SIZE_W, 0);
        makeTree(w, h);
    }

    //more rocks
    for (int i = 0; i < 500; i++)
    {
        int h = getInt(DUNGEON_SIZE_H, 0);
        int w = getInt(DUNGEON_SIZE_W, 0);
        if (w != 2 && h != 2) //player pos
            dmap[w][h] = '#';
    }

    //create dungeon
    const int max_rooms = getInt(50, 18);

    //create rooms
    for (int i = 0; i < max_rooms; i++)
    {
        int x = getInt(100, 10);
        int y = getInt(100, 10);

        if (x < 20 && y < 20)
        {
            i--;
            continue;
        }

        int w_size = getInt(10, 4) + 2;
        int h_size = getInt(10, 4) + 2;

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
                if (dmap[x + w - w_size / 2][y + h - h_size / 2] == 's' || dmap[x + w - w_size / 2][y + h - h_size / 2] == '=')
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
                //dmap[x+w-w_size/2][y+h-h_size/2] = '.'; 
                if (h == 0 || w == 0 || w == w_size - 1 || h == h_size - 1)
                    dmap[x + w - w_size / 2][y + h - h_size / 2] = '#';
            }
        }

        //create doors
        if (getInt(2, 0) && dmap[x - w_size / 2][y] == '#')
            dmap[x - w_size / 2][y + getInt(2, 0)] = '+';		//left

        else
            dmap[x + getInt(2, 0)][y - h_size / 2] = '+';	//top

        if (getInt(2, 0) && dmap[x + w_size / 2][y] == '#')
            dmap[x + (w_size - 1) / 2][y + getInt(2, 0)] = '+'; //right

        else
            dmap[x + getInt(2, 0)][y + (h_size - 1) / 2] = '+';	//top
    }/*
    //make ruins

    createRuin(getInt(20,10) ,getInt(25,5),4,3,4,3);
    createRuin(getInt(20,10) ,getInt(25,5),5,4,5,4);


    // make mountain
    makeMountain();

    //make river*/
    //	makeRiver(getInt(35,65));
    /*	makeRiver(getInt(30,30));


        SafetyReset();

        do //make entrance
        {
        h = getInt(DUNGEON_SIZE_H-2,10);
        w = getInt(DUNGEON_SIZE_W-2,2);
        Safety("Could not add entrance to mountain");

        }while(!(dmap[w][h] == 'M' && (dmap[w-1][h] == 'g' || dmap[w-1][h] == 's')));

        dmap[w][h] = '>';

        */
    //make bridge
    //	for( i=w;i>0;i--)
    //	if(dmap[i][h] == '=')
    //		dmap[i][h] = 'b';

    int h = getInt(range, 20);

    for (int i = 0; i < range * 2; i++)
    if (dmap[i][h] == '=')
        dmap[i][h] = 'b';

    return 0;
}

int DungeonGenerator::makeOutSideDungeon()
{
    int h, w;
    int top = getInt(10, 0) + 30;

    //create grass
    for (h = 0; h < DUNGEON_SIZE_H; h++)
    for (w = 0; w < DUNGEON_SIZE_W; w++)
        dmap[w][h] = 'g';

    //plant trees
    for (long i = 0; i < 30; i++)
    {
        h = getInt(DUNGEON_SIZE_H, 0);
        w = getInt(DUNGEON_SIZE_W / 2 + 15, 0);
        dmap[w][h] = 'T';
    }

    //make ruins
    createRuin(getInt(20, 10), getInt(25, 5), 4, 3, 4, 3);
    createRuin(getInt(20, 10), getInt(25, 5), 5, 4, 5, 4);

    //plant rocks //can box in entrance
    for (int i = 0; i < 15; i++)
    {
        h = getInt(DUNGEON_SIZE_H, 0);
        w = getInt(DUNGEON_SIZE_W, DUNGEON_SIZE_W / 2);
        dmap[w][h] = '#';
    }
    //more rocks
    for (int i = 0; i < 15; i++)
    {
        h = getInt(DUNGEON_SIZE_H, 0);
        w = getInt(DUNGEON_SIZE_W / 2 + 10, 0);
        if (w > 2 && h > 3) //player pos
            dmap[w][h] = '#';
    }

    // make mountain
    makeMountain();

    //make river
    makeRiver(getInt(43, 43));
    makeRiver(getInt(37, 37));

    SafetyReset();

    do //make entrance
    {
        h = getInt(DUNGEON_SIZE_H - 2, 10);
        w = getInt(DUNGEON_SIZE_W - 2, 2);
        Safety("Could not add entrance to mountain");
    } while (!(dmap[w][h] == 'M' && (dmap[w - 1][h] == 'g' || dmap[w - 1][h] == 's')));

    dmap[w][h] = '>';

    //make bridge
    for (int i = w; i > 0; i--)
    if (dmap[i][h] == '=')
        dmap[i][h] = 'b';

    return 0;
}

void DungeonGenerator::SafetyReset()
{
    safety = 0;
}

int DungeonGenerator::Safety(char * message, int test)
{
    safety++;
    if (safety > test)
    {
        throw std::exception(message);
    }
    return 1;
}

void DungeonGenerator::makeMountain()
{
    int start = 43;
    int top = 43;

    int h = 0, w;
    for (h = 0; h < DUNGEON_SIZE_H; h++)
    {
        int rand = getInt(7, 1) - 3;
        top += rand;

        //make water
        for (w = top; w < DUNGEON_SIZE_W; w++)
            dmap[w][h] = 'M';
    }
}

int DungeonGenerator::makeRiverDungeon()
{
    //create dungeon
    //create rooms
    const int max_rooms = getInt(4, 3);
    int rooms(0);
    int dungeon_full = 0;
    int left = 1;

    while (rooms < max_rooms + 1)
    {
        if (left)
        {
            if (createRoom(rooms, max_rooms, 7, getInt(MAX_Y_POS, MIN_Y_POS)))
            {
                rooms++;
                left = 0;
            }
        }
        else
        {
            if (createRoom(rooms, max_rooms, 60, getInt(MAX_Y_POS, MIN_Y_POS)))
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
    //dmap[cX][cY] = '>';
    return 1;
}

int DungeonGenerator::makeMazeDungeon()
{
    //create dungeon
    const int max_rooms = 30;
    int rooms(0);

    //create rooms
    int dungeon_full = 0;

    while (rooms < max_rooms + 1)
    {
        if (createRoom(rooms, max_rooms, getInt(MAX_X_POS, MIN_X_POS), getInt(MAX_Y_POS, MIN_Y_POS), 5, 3, 5, 3))
            rooms++;

        if (dungeon_full++ == 1000) //safety feature - no room left so add stairs
        {
            break;
        }
    }

    // add river
    int make_river = getInt(10, 0);
    //int complexity = getInt(3,0);
    if (make_river == 9)
    {
        makeRiver(getInt(10, 0) + 30);
    }

    return 1;
}
int DungeonGenerator::makeNormalDungeon()
{
    //create dungeon
    const int max_rooms = getInt(MAX_ROOMS, MIN_ROOMS);
    assert(max_rooms >= MIN_ROOMS);
    int rooms(0);

    //create rooms
    SafetyReset();
    int dungeon_full = 0;

    while (rooms < max_rooms + 1)
    {
        if (createRoom(rooms, max_rooms, getInt(MAX_X_POS, MIN_X_POS), getInt(MAX_Y_POS, MIN_Y_POS)))
            rooms++;

        if (dungeon_full++ == 1000) //safety feature - no room left so add stairs
        {
            break;
        }
    }

    //add loop corridor.
    addLoops(5);

    // add river
    int make_river = getInt(10, 0);
    int complexity = getInt(3, 0);
    if (make_river > 7)
    {
        for (int i = 0; i < complexity; i++)
            makeRiver(getInt(10, 0) + 30);
    }

    return 1;
}

int DungeonGenerator::makeSpecialDungeon()
{
    //create dungeon
    const int max_rooms = getInt(MAX_ROOMS, MIN_ROOMS);
    int rooms(0);
    assert(max_rooms >= MIN_ROOMS);

    //create rooms
    SafetyReset();

    while (rooms < max_rooms + 1)
    {
        if (createRoom(rooms, max_rooms, getInt(MAX_X_POS, MIN_X_POS), getInt(MAX_Y_POS, MIN_Y_POS)))
            rooms++;

        Safety("make Special Dungeon failed", 10000);
    }

    addLoops(2);

    // add river
    int make_river = getInt(10, 0);
    int complexity = getInt(3, 0);
    if (make_river > 7)
    {
        for (int i = 0; i < complexity; i++)
            makeRiver(getInt(10, 0) + 30);
    }

    AddFountain();

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

    int     size = specialSpotCoords.size();
    int     i = 0;
    bool    placed = false;
    assert(size > 0);

    if (size != 0)
    {
        int getCoord = getInt(size, 0);
        for (COORDLIST::iterator it = specialSpotCoords.begin(); it != specialSpotCoords.end(); it++, i++)
        {
            if (getCoord == i)
            {
                int x = it->x;
                int y = it->y;
                dmap[it->x][it->y] = 'i';
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

int DungeonGenerator::createRuin(int x, int y, int max_w, int min_w, int max_h, int min_h)
{
    int w_size = getInt(max_w, min_w);
    int h_size = getInt(max_h, min_h);

    //check that it is a good spot to put the room 

    if (!validRoomPos(x, y, w_size, h_size))
        return 0;

    //create room

    for (int h = 0; h < h_size; h++)
    {
        for (int w = 0; w < w_size; w++)
        {
            dmap[x + w - w_size / 2][y + h - h_size / 2] = '.';

            if (w == w_size - 1 && getInt(2, 0))
                dmap[x + w - w_size / 2 + 1][y + h - h_size / 2] = 'r';
            if (w == 0 && getInt(2, 0))
                dmap[x + w - w_size / 2 - 1][y + h - h_size / 2] = 'r';
            if (h == h_size - 1 && getInt(2, 0))
                dmap[x + w - w_size / 2][y + h - h_size / 2 + 1] = 'r';
            if (h == 0 && getInt(2, 0))
                dmap[x + w - w_size / 2][y + h - h_size / 2 - 1] = 'r';
        }
    }
    if (getInt(2, 0))
        dmap[x + w_size / 2 + 1][y - h_size / 2 - 1] = 'r';
    if (getInt(2, 0))
        dmap[x + w_size / 2 + 1][y + h_size / 2 + 1] = 'r';
    if (getInt(2, 0))
        dmap[x - w_size / 2 - 1][y - h_size / 2 - 1] = 'r';
    if (getInt(2, 0))
        dmap[x - w_size / 2 - 1][y + h_size / 2 + 1] = 'r';


    return 1;
}

int DungeonGenerator::createRoom(int roomCount, const int maxRooms, int x, int y, int max_w, int min_w, int max_h, int min_h)
{
    int w_size = getInt(max_w, min_w);
    int h_size = getInt(max_h, min_h);

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
        //	char id = 'a';
        //	id +=rooms;
        //	dmap[x][y] = id;
    }

    //add exit/entry
    if (roomCount == 0) //start room
    {
        if (type != lSpecial)
            dmap[x][y] = '<';
        else
            dmap[x][y] = '(';
    }
    if (roomCount == maxRooms && type != lSpecial)
    {
        dmap[x][y] = '>';
    }

    //set up last know room to link next room to
    last_room_x = x;
    last_room_y = y;
    return 1;
}

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
            //dmap[x+w-w_size/2][y+h-h_size/2] = '.'; 
        }
    }
}

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
            if (dmap[x + w - (w_size + 2) / 2][y + h - (h_size + 2) / 2] == '.' || dmap[x + w - (w_size + 2) / 2][y + h - (h_size + 2) / 2] == '+')
            {
                //TraceMsg("Room Failed - touching existing room");
                return 0;
            }
        }
    }
    return 1; //valid position
}

int DungeonGenerator::createDoor(int x, int y, int chance)
{
    int random = getInt(100, 0);

    //door sanity check (not yet perfect)
    if (!((dmap[x + 1][y] != '#' &&  dmap[x - 1][y] != '#') || (dmap[x][y - 1] != '#' &&  dmap[x][y + 1] != '#')))
        return 0;

    if (((dmap[x + 1][y] != '#' &&  dmap[x - 1][y] != '#') && (dmap[x][y - 1] != '#' &&  dmap[x][y + 1] != '#')))
        return 0;

    if (random < chance) //make door
        dmap[x][y] = '+';
    //else do nothing

    return 1;
}

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

    int start_dir = getInt(2, 1);

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
        while (dmap[x1][y1] == '.' && dir1 != dNorth) //find wall of start room
        {
            if (dir1 == dWest) x1--;
            else if (dir1 == dEast) x1++;

            Safety("could not find wall of start room");
        }

        SafetyReset();
        //starting point found make path
        do //make path in x axis
        {
            if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                dmap[x1][y1] = '.';

            if (dir1 == dWest) x1--;
            else if (dir1 == dEast) x1++;

            //if(dmap[x1][y1] == '.') //another path encountered
            if (isNearWall(x1, y1))
            {
                if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                    dmap[x1][y1] = '.';
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
            if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                dmap[x1][y1] = '.';

            if (dir2 == dNorth) y1++;
            else if (dir2 == dSouth) y1--;

            //if(dmap[x1][y1] == '.')
            if (isNearWall(x1, y1)) //another path encountered
            {
                if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                    dmap[x1][y1] = '.';
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
                    if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                        dmap[x1][y1] = '.';
                    if (dir1 == dEast) x1--;	//reverse direction
                    else if (dir1 == dWest) x1++;

                    if (dmap[x1][y1] == '.')//another path encountered
                    {
                        createDoor(x1, y1, 100);
                        if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                            dmap[x1][y1] = '.';
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
                    if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                        dmap[x1][y1] = '.';
                    if (dir1 == dEast) x1--;	//reverse direction
                    else if (dir1 == dWest) x1++;

                    if (dmap[x1][y1] == '.')//another path encountered
                    {
                        if (dmap[x1][y1] != '<' && dmap[x1][y1] != '>')
                            dmap[x1][y1] = '.';
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
        while (dmap[x1][y1] == '.') //find wall
        {

            if (dir2 == dNorth) y1++;
            else if (dir2 == dSouth) y1--;
            Safety("X1");
        }

        do
        {
            dmap[x1][y1] = '.';

            if (dir2 == dNorth) y1++;
            else if (dir2 == dSouth) y1--;

            //if(dmap[x1][y1] == '.') //another path encountered
            if (isNearWall(x1, y1))
            {
                dmap[x1][y1] = '.';
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
            dmap[x1][y1] = '.';
            if (dir1 == dWest) x1--;
            else if (dir1 == dEast) x1++;

            //if(dmap[x1][y1] == '.') //another path encountered
            if (isNearWall(x1, y1))
            {
                dmap[x1][y1] = '.';
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
                    dmap[x1][y1] = '.';
                    if (dir2 == dNorth) y1--;	//reverse direction
                    else if (dir2 == dSouth) y1++;

                    if (dmap[x1][y1] == '.')//another path encountered
                    {
                        dmap[x1][y1] = '.';
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
                    dmap[x1][y1] = '.';
                    if (dir2 == dNorth) y1--;	//reverse direction
                    else if (dir2 == dSouth) y1++;

                    if (dmap[x1][y1] == '.')//another path encountered
                    {
                        dmap[x1][y1] = '.';
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
        *x = getInt(DUNGEON_SIZE_W, 0);
        *y = getInt(DUNGEON_SIZE_H, 0);

        if (dmap[*x][*y] == '#')
        {
            if (dmap[*x + 1][*y] == '.') return dWest;
            if (dmap[*x][*y + 1] == '.') return dSouth;
            if (dmap[*x - 1][*y] == '.') return dEast;
            if (dmap[*x][*y - 1] == '.') return dNorth;
        }

        Safety("findwall");
    } while (!found);
    return 0; //never reach
}

int DungeonGenerator::createCorridor(int *x, int *y, int dir1)
{

    int dir = getInt(2, 1);

    if (dir == 1)
    {
        for (int i = 0; i < 35; i++)
            dmap[35][i + 1] = '.';
    }
    else
    {
        for (int i = 0; i < 85; i++)
            dmap[i + 2][15] = '.';
    }


    /*
        dmap[w][h] = '.';

        int distance = getInt(15,3);

        for(int i=0;i<distance;i++)
        {
        switch(dir)
        {
        case dNorth: h++; break;
        case dEast:  w++; break;
        case dSouth: h--; break;
        case dWest:  w--; break;
        }
        if(h==0 || h==DUNGEON_SIZE_H || w==0 || w==DUNGEON_SIZE_W )
        return 0;

        dmap[w][h] = '.';
        *x = w; *y = h;
        }*/
    return 1;

}

int DungeonGenerator::isNearWall(int x, int y)
{
    int found = 0; //found must be two or more to return true

    if (dmap[x + 1][y] != '#')
        found++;
    if (dmap[x - 1][y] != '#')
        found++;
    if (dmap[x][y + 1] != '#')
        found++;
    if (dmap[x][y - 1] != '#')
        found++;

    if (found > 1) return 1;
    return 0;

}

void DungeonGenerator::OpenTrace(bool open)
{
    //	if(open)
    //	ofile.open("Debug Files\\dungeonTrace.txt");

    //	else
    //	//	ofile.close();

}

void DungeonGenerator::TraceMsg(char * msg)
{
    //char id[1];
    //	_itoa(rooms,id,10);

    //	ofile << id[0] <<": " <<msg<< std::endl;

}


void DungeonGenerator::makeRiver(int top, int size)
{

    //	int top = 
    int bottom = 35;//getInt(60,0)+5;
    int path_side = getInt(2, 0);

    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        int width = getInt(5, 2) + size;
        int rand = getInt(4, 1) - 2;
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
            makeWater(top - (width / 2) + w, h);


    }

}

void DungeonGenerator::makeWater(int x, int y)
{
    if (x >= DUNGEON_SIZE_W || y >= DUNGEON_SIZE_H || x < 0 || y < 0)
        return;

    if (dmap[x][y] != '>' && dmap[x][y] != '<')
        dmap[x][y] = '=';

}

void DungeonGenerator::makeSand(int x, int y)
{
    if (x >= DUNGEON_SIZE_W || y >= DUNGEON_SIZE_H || x < 0 || y < 0) return;

    if (dmap[x][y] == '>' || dmap[x][y] == '<' || dmap[x][y] == '=' || dmap[x][y] == 'w')
        return;
    else
        dmap[x][y] = 's';

}

void DungeonGenerator::makeTomb(int x, int y)
{
    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0) return;

    if (dmap[x][y] == '>' || dmap[x][y] == '<' || dmap[x][y] == '=' || dmap[x][y] == '#')
        return;

    dmap[x][y] = 'u';

}
void DungeonGenerator::makeFloor(int x, int y)
{
    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0)return;

    dmap[x][y] = '.';

}


void DungeonGenerator::makeGrass(int x, int y)
{

    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0)return;

    dmap[x][y] = 'g';

}

void DungeonGenerator::makeTree(int x, int y)
{

    if (x >= DUNGEON_SIZE_W - 1 || y >= DUNGEON_SIZE_H - 1 || x <= 0 || y <= 0)return;

    if (dmap[x][y] == '>' || dmap[x][y] == '<' || dmap[x][y] == '=' || dmap[x][y] == '#' || dmap[x][y] == 'w')
        return;


    dmap[x][y] = 'T';

}


void DungeonGenerator::makeBridge()
{
    int x = 15, y;
    int inc = 1;

    int dir = getInt(2, 0);
    if (dir == 1)
        inc = -1;

    for (y = 0; y < 15; y += inc)
    {
        if (dmap[x][y] == '.')
            break;
    }

    for (x = 0; x < DUNGEON_SIZE_W; x++)
    {
        if (dmap[x][y] == '=')
        {
            dmap[x][y] = 'b';
            dmap[x - 1][y] = 'b';
        }
    }
    dmap[x + 1][y] = 'b';
}


int DungeonGenerator::createUndeadRoom(int rooms, const int max_rooms, int x, int y, int max_w, int min_w, int max_h, int min_h)
{
    int w_size = getInt(max_w, min_w);
    int h_size = getInt(max_h, min_h);

    //check that it is a good spot to put the room 

    if (!validRoomPos(x, y, w_size, h_size))
        return 0;

    //create room

    buildRoom(x, y, w_size, h_size, 0);
    buildRoom(x, y - 1, h_size, w_size, 0);


    if (rooms) //more than one room make a path
    {
        /*TraceMsg("Room Suceeded");
        char buf[32];
        sprintf(buf,"x%d y%d x%d y%d",x,y,last_room_x,last_room_y);
        TraceMsg(buf);*/
        createPath(x, y, last_room_x, last_room_y);
    }

    //add exit/entry
    if (rooms == 0) //start room
    {
        dmap[x][y] = '<';
    }
    if (rooms == max_rooms)
    {
        dmap[x][y] = '>';
    }



    //set up last know room to link next room to
    last_room_x = x;
    last_room_y = y;
    return 1;
}


int DungeonGenerator::AddTeleport()
{
    //tele chance
    if (getInt(2, 0) == 0) //33%
    {
        COORDLIST free_spaces;

        for (int x = 0; x < DUNGEON_SIZE_W; x++)
        {
            for (int y = 0; y < DUNGEON_SIZE_H; y++)
            {
                if (dmap[x][y] == '.')
                {
                    coord new_coord;
                    new_coord.x = x;
                    new_coord.y = y;
                    free_spaces.push_back(new_coord);
                }
            }
        }
        if (free_spaces.size() > 0)
        {
            int choice = getInt(free_spaces.size(), 0);
            int count = 0;
            for (COORDLIST::iterator it = free_spaces.begin(); it != free_spaces.end(); it++, count++)
            {
                if (count == choice)
                {
                    dmap[it->x][it->y] = 'v';
                    return 1;
                }
            }
        }
    }
    return 0;
}

int DungeonGenerator::AddFountain()
{
    //fountain chance
    if (getInt(3, 0) == 0) //33%
    {
        COORDLIST free_spaces;

        for (int x = 0; x < DUNGEON_SIZE_W; x++)
        {
            for (int y = 0; y < DUNGEON_SIZE_H; y++)
            {
                if (dmap[x][y] == '.')
                {
                    coord new_coord;
                    new_coord.x = x;
                    new_coord.y = y;
                    free_spaces.push_back(new_coord);
                }
            }
        }
        if (free_spaces.size() > 0)
        {
            int choice = getInt(free_spaces.size(), 0);
            int count = 0;
            for (COORDLIST::iterator it = free_spaces.begin(); it != free_spaces.end(); it++, count++)
            {
                if (count == choice)
                {
                    dmap[it->x][it->y] = 'f';
                    return 1;
                }
            }
        }
    }
    return 0;
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
            flood_test[x][y] = '#';
            if (dmap[x][y] == '.')
            {
                coord new_coord;
                new_coord.x = x;
                new_coord.y = y;
                free_spaces.push_back(new_coord);
            }
        }
    }
    if (free_spaces.size() > 0)
    {
        int choice = getInt(free_spaces.size(), 0);
        int count = 0;
        for (COORDLIST::iterator it = free_spaces.begin(); it != free_spaces.end(); it++, count++)
        {
            if (count == choice)
            {
                X = it->x;
                Y = it->y;
            }
        }
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
            if ((dmap[x][y] != '#' && dmap[x][y] != 'r' && dmap[x][y] != 'M'))
            {
                original_count++;
            }
            if (flood_test[x][y] == '.')
            {
                flood_count++;
            }
        }
    }
    return (original_count == flood_count) ? true : false;
}


void DungeonGenerator::FloodFill(int x, int y)
{
    if ((dmap[x][y] != '#' && dmap[x][y] != 'r' && dmap[x][y] != 'M') && (flood_test[x][y] != '.'))
        flood_test[x][y] = '.';
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

void DungeonGenerator::addLoops(int nLoops)
{
    int loops = nLoops;// Random::getInt(nLoops + 1, 1);

    do
    {
        coord c1, c2;

        do
        {
            int x = getInt(DUNGEON_SIZE_W, 1);
            int y = getInt(DUNGEON_SIZE_H, 1);

            if (dmap[x][y] == '.')
            {
                c1.x = x; c1.y = y;
                break;
            }

        } while (true);
        do
        {
            int x = getInt(DUNGEON_SIZE_W, 1);
            int y = getInt(DUNGEON_SIZE_H, 1);

            if (dmap[x][y] == '.')
            {
                c2.x = x; c2.y = y;
                break;
            }

        } while (true);

        createPath(c1.x, c1.y, c2.x, c2.y);
    } while (--loops);
}


int DungeonGenerator::fixDoors()
{
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            if (dmap[w][h] == '+')
            {
                if (dmap[w + 1][h] == '#' && dmap[w - 1][h] == '#')
                    continue;
                else if (dmap[w][h+1] == '#' && dmap[w][h-1] == '#')
                    continue;

                dmap[w][h] = '.';
            }
        }
    }
    return 0;
}