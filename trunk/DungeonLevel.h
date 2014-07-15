// DungeonLevel.h: interface for the DungeonLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUNGEONLEVEL_H__764234F8_4887_42E3_812F_FD367433372A__INCLUDED_)
#define AFX_DUNGEONLEVEL_H__764234F8_4887_42E3_812F_FD367433372A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma warning(disable : 4786) 

#include "terrain.h"
#include "monster.h"
#include "Item.h"
#include "LOS/LineOfSight.h"

#include "Coord.h"

enum eLevelType
{
    lNormal = 0,
    lRiver,
    lEnd,
    lOutside,
    lSwamp,
    lMaze,
    lUndead,
    lSpecial,
    lCavern
};

enum eSpecialLevelType
{
    slTrollCave = 10,
    slBarracks,
    slPrison,
    slBoatHouse,
    slShop,
    slVampire,
    slTreasure,
    slSpiderCave,
    slUndead,
    slEncounter = 20,
    slRandom
};
enum ePath
{
    none,
    clear,
    blocked
};

struct Symbol
{
    Symbol(char symbol, unsigned char c1, unsigned char c2, unsigned char c3):
        mSymbol(symbol), mColour1(c1), mColour2(c2), mColour3(c3)
    {}
    char mSymbol;
    unsigned char mColour1;
    unsigned char mColour2;
    unsigned char mColour3;
};

class cell
{
public:
    cell() :
        extra(0), show_target(0), show_path(none), monster(NULL), item(NULL), mSymbol(NULL)
    {};

    ~cell(){};
    Terrain terrain;

    int extra;
    int show_target;
    ePath show_path;

    Monster * GetMonster()  { return monster; };

    void RemoveMonsterRef() { monster = NULL; };
    void RemoveItemRef()    { item = NULL; };

    void AssignMonster(Monster * new_monster) { monster = new_monster; };
    void AssignItem(Item * new_item)          { item = new_item; };

    Monster * getMonster()  { return monster; };
    Item * getItem()        { return item; };
    bool monsterExists()    { return (monster == NULL ? false : true); };
    bool itemExists()       { return (item == NULL ? false : true); };

    void setSymbol(Symbol & symbol)     
    {
        mSymbol = &symbol; 
    };
    void clearSymbol()                  { mSymbol = 0; };
    Symbol * getSymbol()                { return mSymbol; };

private:
    Monster *   monster;
    Item *      item;
    Symbol *    mSymbol;
};

typedef std::vector<Coord> COORDLIST1;

class DungeonLevel : public LineOfSight
{
public:
    DungeonLevel();
    virtual ~DungeonLevel();

    int Initialise(int special = 0);
    int Delete();

    int ClearMonsters();

    void LightDungeon(int x, int y, int range);
    int ClearPath();
    int LOS(int x, int y, int x2, int y2, int range);

    cell map[DUNGEON_SIZE_W][DUNGEON_SIZE_H];

    int MakeLevelMap();
    Coord * getStartPos();
    Coord * GetEndPosition();
    Coord * NewBridgePosition();
    Coord * NewItemPosition(bool space = false);
    int     getAdjacentFreeSpaces(int x, int y);
    Coord * NewSpecialItemPosition();
    Coord * FreeTerrainPosition(eTerrainType type);
    Coord * FreeTerrainPosition(const std::string & name);

    bool IsCellTransparent(int x, int y);
    bool HighLightPath(int x, int y, int w, int h);
    int  isNearWall(int x, int y);

    COORDLIST show_path;
    int _type;
    Coord tempPos;
    int initialised;

    enum EMapLight
    {
        eNormal,    // show lit tiles, and show found tiles
        eShowAll,   // show lit tiles and all tiles
        eLightAll,  // show all tiles as lit tiles.
        eNoFound,   // show lit tiles
    };

    EMapLight getMapLight();
    void setMapLight(EMapLight mapLighting);
private:

    EMapLight mapLighting;
};

#endif // !defined(AFX_DUNGEONLEVEL_H__764234F8_4887_42E3_812F_FD367433372A__INCLUDED_)
