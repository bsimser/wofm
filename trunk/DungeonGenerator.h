// DungeonGenerator.h: interface for the DungeonGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUNGEONGENERATOR_H__D49B02F6_8AA1_4C67_8264_6171D1953767__INCLUDED_)
#define AFX_DUNGEONGENERATOR_H__D49B02F6_8AA1_4C67_8264_6171D1953767__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <iostream>
#include <fstream>

//#include "DungeonLevel.h"


typedef enum eDir
{
	dNone = -2,
	dWait = -1 ,
	dNorth = 1,
	dSouth = 2,
	dEast  = 3,
	dNorthEast = 4,
	dSouthEast = 5,
	dWest	   = 6,
	dNorthWest = 7,
	dSouthWest = 8,

	dUp,
	dDown
}direction;

#define DUNGEON_SIZE_W 80
#define DUNGEON_SIZE_H 39
#define MAP_SCALE	   4


#define MAX_ROOMS 18 //15
#define MIN_ROOMS 4	//8

#define  MAX_X_POS 85
#define MIN_X_POS 3

#define MAX_Y_POS 35
#define MIN_Y_POS 3

#define MAX_X_SIZE 20 //20
#define MIN_X_SIZE 3  //3

#define MAX_Y_SIZE 10 //10
#define MIN_Y_SIZE 3  //3


class DungeonGenerator  
{
public:
	DungeonGenerator();
	virtual ~DungeonGenerator();

	int CreateMap();
	virtual int Create(int type);

	char dmap[DUNGEON_SIZE_W ][DUNGEON_SIZE_H];
	char flood_test[DUNGEON_SIZE_W ][DUNGEON_SIZE_H];

//	DungeonLevel level;
	int ConvertMapToDungeon();
	
	bool FloodTest();

//	coord * DungeonGenerator::getStartPos();

protected:

	int DumpDungeon();

	virtual	int makeNormalDungeon();
	virtual	int makeRiverDungeon();
	virtual	int makeMazeDungeon();
	virtual	int makeOutSideDungeon();
	virtual int makeUndeadDungeon();
	virtual	int makeSpecialDungeon();
	virtual int makeEncounterDungeon();
	int makeCavernDungeon();

	void FloodFill(int x,int y);
	void addLoops(int loops);

	int findwall(int *x, int *y);

	int createCorridor(int *x, int *y,int dir);

    int createRoom(int roomCount, const int maxRooms, int x, int y, int max_w = MAX_X_SIZE, int min_w = MIN_X_SIZE, int max_h = MAX_Y_SIZE, int min_h = MIN_Y_SIZE);
    int createUndeadRoom(int roomCount, const int maxRooms, int x, int y, int max_w = MAX_X_SIZE, int min_w = MIN_X_SIZE, int max_h = MAX_Y_SIZE, int min_h = MIN_Y_SIZE);
	int createRuin(int x,int y,int max_w = MAX_X_SIZE ,int min_w = MIN_X_SIZE,int max_h = MAX_Y_SIZE,int min_h = MIN_Y_SIZE);
	int createPath(int x1,int y1,int x2,int y2);
	int createDoor(int x,int y,int chance = 50);

	int last_room_x;
	int last_room_y;

	std::ofstream ofile;

	void OpenTrace(bool open);

	void buildRoom(int x,int y,int w_size,int h_size,int type);

	void TraceMsg(char * msg);

    int isNearWall(int x, int y);
    int fixDoors();

	int validRoomPos(int x,int y,int w_size,int h_size);

	void makeRiver(int top,int size =0);
	void makeWater(int x,int y);
	void makeSand(int x,int y);
	void makeTomb(int x,int y);
	void makeFloor(int x,int y);
	void makeBridge();
	void makeMountain();
	void makeGrass(int x,int y);
	void makeTree(int x,int y);

    int AddFountain();
    int AddTeleport();

	int type;

    void SafetyReset();

    int Safety(char*message,int test=5000);
    int safety;

    bool findTerrainType(int &x, int &y, const char terrain);


};

#endif // !defined(AFX_DUNGEONGENERATOR_H__D49B02F6_8AA1_4C67_8264_6171D1953767__INCLUDED_)
