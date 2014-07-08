// DungeonLevel.cpp: implementation of the DungeonLevel class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4786) 

#include "DungeonLevel.h"
#include "NumberGenerator.h"

#include <math.h>

#include "dungeonGenerator.h"
#include "SpecialLevelGenerator.h"
#include "Worldbuilder.h"

bool highlight;

COORDLIST lit_cells;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

using namespace Random;

DungeonLevel::DungeonLevel() :
initialised(0),
mapLighting(eNormal)
{
    _type = lNormal;
}

DungeonLevel::~DungeonLevel()
{
    //	Delete();
} 

int DungeonLevel::Initialise(int level_type)
{
    DungeonGenerator *generator;
    if (level_type < 10)
    {
        generator = new DungeonGenerator();
    }
    else
    {
        generator = new SpecialLevelGenerator();
    }

    _type = generator->Create(level_type);

    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    {
        for (int w = 0; w < DUNGEON_SIZE_W; w++)
        {
            //terrain
            switch (generator->dmap[w][h])
            {
            case '#':map[w][h].terrain.Create(stone); break;
            case '.':map[w][h].terrain.Create(dfloor); break;
            case '<':map[w][h].terrain.Create(upStairs); break;
            case '>':map[w][h].terrain.Create(lockedStairs); break;
            case '+':map[w][h].terrain.Create(closedDoor); break;
            case '"':map[w][h].terrain.Create(openDoor); break;
            case '=':map[w][h].terrain.Create(deepWater); break;
            case 'w':map[w][h].terrain.Create(shallowWater); break;
            case 'b':map[w][h].terrain.Create(bridge); break;
            case 'g':map[w][h].terrain.Create(grass); break;
            case 'T':map[w][h].terrain.Create(tree); break;
            case 't':map[w][h].terrain.Create(deadTree); break;
            case 's':map[w][h].terrain.Create(sand); break;
            case 'M':map[w][h].terrain.Create(mountain); break;
            case 'u':map[w][h].terrain.Create(tomb); break;
            case '(':map[w][h].terrain.Create(specialUp); break;
            case ')':map[w][h].terrain.Create(specialLocked); break;
            case 'i':map[w][h].terrain.Create(specialItem); break;
            case 'r':map[w][h].terrain.Create(ruins); break;
            case 'f':map[w][h].terrain.Create(fountain); break;
            case 'v':map[w][h].terrain.Create(teleport); break;
            case 'S':map[w][h].terrain.Create(fountain); break;

            default:map[w][h].terrain.Create(stone); break;
            }

            //monsters
            map[w][h].RemoveMonsterRef();
            map[w][h].RemoveItemRef();
            //items
        }
    }
    //MakeLevelMap();
    initialised = 1;
    return initialised;
}

int DungeonLevel::MakeLevelMap()
{
    /*int y=0;

    for( int h =0;h<DUNGEON_SIZE_H;h+=MAP_SCALE,y++)
    {
    int x=0;
    for( int w=0;w<DUNGEON_SIZE_W;w+=MAP_SCALE,x++)
    {
    bigmap[x][y].terrain = map[w][h].terrain;
    bigmap[x][y].RemoveMonsterRef();
    bigmap[x][y].RemoveItemRef();
    }
    }*/
    return 1;
}

int DungeonLevel::Delete()
{
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    for (int w = 0; w < DUNGEON_SIZE_W; w++)
    {
        //if(map[w][h].monster) //monsters deleted elsewhere
        //	delete map[w][h].monster;
        if (map[w][h].getItem() != NULL)
            delete map[w][h].getItem();

        map[w][h].RemoveMonsterRef();
        map[w][h].RemoveItemRef();
    }
    return 1;
}

//This lights the dungeon around the player within a sight range
//This keeps a list of the Coords that are lit only changes these and any others within the players radius.
//This is much faster that testing the entire dungeon




/*	if(lit_cells.size()==0) //initial test, do this once - test entire dungeon
    {
    for( int h =0;h<DUNGEON_SIZE_H;h++)
    {
    for( int w=0;w<DUNGEON_SIZE_W;w++)
    {
    if(LOS(x,y,w,h,sight))
    {
    map[w][h].terrain.found = 1;
    map[w][h].terrain.light = 1;
    Coord *pos = new Coord;
    pos->x=w;
    pos->y=h;
    lit_cells.push_back(*pos);
    }
    else
    {
    map[w][h].terrain.light = 0;
    }
    }
    }
    return;

    }*/

void DungeonLevel::LightDungeon(int x, int y, int range) //los??
{

    if (World.GetCurrentLevel() == 0) //outside
        range = 20;

    /*//if(lit_cells.size()==0) //initial test, do this once - test entire dungeon
    {
    for( int h =0;h<DUNGEON_SIZE_H;h++)
    {
    for( int w=0;w<DUNGEON_SIZE_W;w++)
    {
    if(LOS(x,y,w,h,range))
    {
    map[w][h].terrain.found = 1;
    map[w][h].terrain.light = 1;
    //Coord *pos = new Coord;
    //	pos->x=w;
    //pos->y=h;
    //lit_cells.push_back(*pos);
    }
    else
    {
    map[w][h].terrain.light = 0;
    }
    }
    }
    return;

    }*/
    int get = _type;
    highlight = false;
    COORDLIST1::iterator it;

    //turn off light last light
    for (it = lit_cells.begin(); it != lit_cells.end(); it++)
    {
        int x1 = it->x;
        int y1 = it->y;
        map[x1][y1].terrain.light = 0; //shadows
        //	map[x1][y1].terrain.found = 0; //hide
    }
    lit_cells.clear();

    //do new light
    int min_h_range = y - range - 5;
    if (min_h_range < 0)
        min_h_range = 0;

    int min_w_range = x - range - 5;
    if (min_w_range < 0)
        min_w_range = 0;

    int max_h_range = y + range + 5;
    if (max_h_range > DUNGEON_SIZE_H)
        max_h_range = DUNGEON_SIZE_H;

    int max_w_range = x + range + 5;
    if (max_w_range > DUNGEON_SIZE_W)
        max_w_range = DUNGEON_SIZE_W;


    //check all squares inside radius
    for (int h = min_h_range; h < max_h_range; h++)
    {
        for (int w = min_w_range; w < max_w_range; w++)
        {
            int r = (int)sqrt((float)(x - w)*(x - w) + (h - y)*(h - y));
            if (r <= range)
            {
                if (IsCellVisible(x, y, w, h))
                {
                    map[w][h].terrain.found = 1;
                    map[w][h].terrain.light = 1;
                    Coord new_Coord;
                    new_Coord.x = w;
                    new_Coord.y = h;
                    lit_cells.push_back(new_Coord);
                }
            }
        }
    }
}

int DungeonLevel::ClearPath()
{
    COORDLIST::iterator it;

    for (it = show_path.begin(); it != show_path.end(); it++)
    {
        map[it->x][it->y].show_path = none;
    }
    show_path.clear();

    return 0;
}

bool isAdjacent(const Coord & c1, const Coord & c2)
{
    if (abs(c1.x - c2.x) <= 1 && abs(c1.y - c2.y) <= 1)
        return true;
    return false;
}

bool DungeonLevel::HighLightPath(int x, int y, int w, int h)
{
    highlight = true;

    COORDLIST::iterator it;

    ClearPath();

    bool mod = 0;

    if (y == h)
    {
        mod = true;
        h--;
    }

    // get path
    IsCellVisible(x, y, w, h);

    // clean path
    COORDLIST::iterator test_it1, test_it2;

    // add end
    if (mod)
        h++;

    Coord pos;
    pos.x = w;
    pos.y = h;
    show_path.push_back(pos);

    for (it = show_path.begin(); it != show_path.end(); it++)
    {
        // if it+2 is adjacent to it then delete it+1;
        test_it1 = (it + 1);
        if (test_it1 != show_path.end())
        {
            test_it2 = (it + 2);
            if (test_it2 != show_path.end())
            {
                if (isAdjacent(*it, *test_it2))
                {
                    //delete test_it1
                    map[test_it1->x][test_it1->y].show_path = none;
                    show_path.erase(test_it1);
                    if (it != show_path.begin()) // keep testing current cell until none are adjacent
                        it--;
                }
            }
        }
    }

    // set character.
    for (it = show_path.begin(); it != show_path.end() - 1; it++) // dont do the end one as a box character is needed.
    {
        if (map[it->x][it->y].terrain.type == stone || map[it->x][it->y].terrain.type == closedDoor || map[it->x][it->y].GetMonster())
            map[it->x][it->y].show_path = blocked;
        else
            map[it->x][it->y].show_path = clear;
    }



    highlight = false;

    return 0;
}

int DungeonLevel::LOS(int x, int y, int x2, int y2, int range)
{
    int r = (int)sqrt((float)(x - x2)*(x - x2) + (y2 - y)*(y2 - y));

    if (r > range) //square is outside radius
        return 0;
    if (x2 == x && y2 == y)
        return 1;

    if (abs(x2 - x) > abs(y2 - y))  //further away in x direction
    {
        if (map[x][y].terrain.type == stone || map[x][y].terrain.type == closedDoor)
            return 0;

        if (x2 - x < 0)
        {
            return LOS(x - 1, y, x2, y2, range);
        }
        else
        {
            return LOS(x + 1, y, x2, y2, range);
        }
    }
    else if (abs(x2 - x) < abs(y2 - y)) //further away in y direction
    {
        if (map[x][y].terrain.type == stone || map[x][y].terrain.type == closedDoor)
            return 0;

        if (y2 - y < 0)
        {
            return LOS(x, y - 1, x2, y2, range);
        }
        else
        {
            return LOS(x, y + 1, x2, y2, range);
        }
    }

    else						//equal
    {
        if (map[x][y].terrain.type == stone || map[x][y].terrain.type == closedDoor)
            return 0;

        if (x2 - x < 0 && y2 - y < 0)
            return LOS(x - 1, y - 1, x2, y2, range);

        else if (x2 - x > 0 && y2 - y > 0)
            return LOS(x + 1, y + 1, x2, y2, range);

        else if (x2 - x > 0 && y2 - y < 0)
            return LOS(x + 1, y - 1, x2, y2, range);

        else if (x2 - x < 0 && y2 - y > 0)
            return LOS(x - 1, y + 1, x2, y2, range);


    }
    return 1;
}

int DungeonLevel::ClearMonsters()
{
    for (int h = 0; h < DUNGEON_SIZE_H; h++)
    for (int w = 0; w < DUNGEON_SIZE_W; w++)
        map[w][h].RemoveMonsterRef();

    return 0;

}


Coord * DungeonLevel::getStartPos()
{

    if (_type == lOutside)
    {
        tempPos.x = tempPos.y = 1;
        return &tempPos;
    }

    for (int y = 0; y < DUNGEON_SIZE_H; y++)
    {
        for (int x = 0; x < DUNGEON_SIZE_W; x++)
        {
            if (map[x][y].terrain.type == upStairs)
            {
                tempPos.x = x;
                tempPos.y = y;
                return &tempPos;
            }
        }
    }
    return NULL;
}
Coord * DungeonLevel::GetEndPosition()
{
    for (int y = 0; y < DUNGEON_SIZE_H; y++)
    {
        for (int x = 0; x < DUNGEON_SIZE_W; x++)
        {
            if (map[x][y].terrain.type == lockedStairs || map[x][y].terrain.type == openStairs)
            {
                tempPos.x = x;
                tempPos.y = y;
                return &tempPos;
            }
        }
    }
    return NULL;
}

Coord * DungeonLevel::FreeTerrainPosition(const std::string & name)
{
    COORDLIST posList;
    COORDLIST::iterator it;
    for (int h = 1; h < DUNGEON_SIZE_H - 1; h++)
    {
        for (int w = 1; w < DUNGEON_SIZE_W - 1; w++)
        {
            if (name == map[w][h].terrain.name)
            {
                Coord pos;
                pos.x = w;
                pos.y = h;
                posList.push_back(pos);
            }
        }
    }

    int pos = getInt(posList.size(), 0);

    int i = 0;
    for (it = posList.begin(); it != posList.end(); it++, i++)
    {
        if (i == pos)
        {
            tempPos.x = it->x;
            tempPos.y = it->y;
            return &tempPos;
        }
    }
    return NULL;
}

Coord * DungeonLevel::FreeTerrainPosition(eTerrainType type)
{
    COORDLIST posList;
    COORDLIST::iterator it;
    for (int h = 1; h < DUNGEON_SIZE_H - 1; h++)
    {
        for (int w = 1; w < DUNGEON_SIZE_W - 1; w++)
        {
            if (map[w][h].terrain.type == type)
            {
                Coord pos;
                pos.x = w;
                pos.y = h;
                posList.push_back(pos);
            }
        }
    }

    int pos = getInt(posList.size(), 0);

    int i = 0;
    for (it = posList.begin(); it != posList.end(); it++, i++)
    {
        if (i == pos)
        {
            tempPos.x = it->x;
            tempPos.y = it->y;
            return &tempPos;
        }
    }
    return NULL;
}

/*Coord * DungeonLevel::NewBridgePosition()
{
COORDLIST posList;
COORDLIST::iterator it;
for(int h=1;h<DUNGEON_SIZE_H-1;h++)
{
for(int w=1;w<DUNGEON_SIZE_W-1;w++)
{
if(map[w][h].item == NULL && map[w][h].GetMonster() == NULL && map[w][h].terrain.type == bridge )
{
Coord pos;pos.x = w; pos.y=h;
posList.push_back(pos);
}
}
}

if(posList.size()==0)
throw(std::exception("Could not find new bridge position"));

int pos = getInt(posList.size(),0);

int i=0;
for(it = posList.begin();it!=posList.end();it++,i++)
{
if(i==pos)
{
tempPos.x = it->x;
tempPos.y = it->y;
}
}


return &tempPos;

}*/

int DungeonLevel::getAdjacentFreeSpaces(int x, int y)
{
    int spaces = 0;

    if (map[x+1][y].terrain.type == dfloor)
        spaces++;
    if (map[x-1][y].terrain.type == dfloor)
        spaces++;
    if (map[x][y+1].terrain.type == dfloor)
        spaces++;
    if (map[x][y-1].terrain.type == dfloor)
        spaces++;
    if (map[x-1][y-1].terrain.type == dfloor)
        spaces++;
    if (map[x-1][y+1].terrain.type == dfloor)
        spaces++;
    if (map[x+1][y-1].terrain.type == dfloor)
        spaces++;
    if (map[x+1][y+1].terrain.type == dfloor)
        spaces++;

    return spaces;
}

Coord * DungeonLevel::NewItemPosition(bool space)
{
    //
    COORDLIST posList;
    COORDLIST::iterator it;

    for (int h = 1; h < DUNGEON_SIZE_H - 1; h++)
    {
        for (int w = 1; w < DUNGEON_SIZE_W - 1; w++)
        {
            if (map[w][h].getItem() == NULL && map[w][h].GetMonster() == NULL && map[w][h].terrain.type == dfloor)
            {
                if (space)
                {
                    if (getAdjacentFreeSpaces(w, h) < 4)
                    {
                        continue;
                    }
                }
                Coord pos; pos.x = w; pos.y = h;
                posList.push_back(pos);
            }
        }
    }

    if (posList.size() == 0)
        throw(std::exception("Could not find new item position"));

    int pos = getInt(posList.size(), 0);

    int i = 0;
    for (it = posList.begin(); it != posList.end(); it++, i++)
    {
        if (i == pos)
        {
            tempPos.x = it->x;
            tempPos.y = it->y;
        }
    }
    return &tempPos;
}

Coord * DungeonLevel::NewSpecialItemPosition()
{
    for (int h = 1; h < DUNGEON_SIZE_H - 1; h++)
    {
        for (int w = 1; w < DUNGEON_SIZE_W - 1; w++)
        {
            if (map[w][h].terrain.type == specialItem)
            {
                tempPos.x = w; tempPos.y = h;
                return &tempPos;

            }
        }
    }

    throw std::exception("Failed to find Special Item Position");
}

int DungeonLevel::isNearWall(int x, int y)
{
    if (map[x + 1][y].terrain.type == stone &&
        map[x][y + 1].terrain.type == stone &&
        map[x - 1][y].terrain.type == stone &&
        map[x][y - 1].terrain.type == stone)
        return 0;

    return 1;
}

bool DungeonLevel::IsCellTransparent(int x, int y)
{
    bool is = map[x][y].terrain.type == stone || map[x][y].terrain.type == closedDoor ? 0 : 1;

    if (highlight)
    {
        //	if(is)
        {
            Coord pos;
            pos.x = x;
            pos.y = y;
            show_path.push_back(pos);
        }
        //if(is)
        {
        //	map[x][y].show_path = clear;

    }
        //else
        //map[x][y].show_path = blocked;
    }
    return is;
}

DungeonLevel::EMapLight DungeonLevel::getMapLight()
{
    return mapLighting;
}

void DungeonLevel::setMapLight(EMapLight lighting)
{
    mapLighting = lighting;
}