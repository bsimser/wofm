// Terrain.h: interface for the Terrain class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TERRAIN_H__94B87907_AC32_4D5A_9EAB_F7F1EF5D3BC5__INCLUDED_)
#define AFX_TERRAIN_H__94B87907_AC32_4D5A_9EAB_F7F1EF5D3BC5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum eTerrainType
{
	stone = 0,
	rock,
	dfloor,
	upStairs,
	lockedStairs,
	openStairs,
	closedDoor,
	openDoor,
	shallowWater,
	deepWater,
	bridge,
	grass,
	tree,
	deadTree,
	sand,
	mountain,
	tomb,
	specialUp,
	specialLocked,
	specialOpen,
	specialItem,
	ruins,
	fountain,
	dryFountain,
    teleport,
    random,
    spiderWeb,
	/*stone = 0,
	rock,
	dfloor,
	upStairs,
	downStairs,
	closedDoor,
	openDoor,
	shallowWater,
	deepWater,
	bridge,
	grass,
	tree,
	deadTree,
	sand,
	mountain,
	tomb,
	specialUp,
	specialLocked,
	specialOpen,
	specialItem*/

};


class Terrain  
{
public:
	Terrain();
	int Create(int type);

	int type;

	int color1,color2,color3;

	int symbol;

	int light;
	int found;

	char name[22];
	

private:
	void makeclosedDoor();
	void makeopenDoor();
	void makeStone();
	void makeRock();
	void makeFloor();
	void makeUpStairs();
	void makeOpenStairs();
	void makeLockedStairs();
	void makeDeepWater();
	void makeShallowWater();
	void makeBridge();
	void makeGrass();
	void makeTree();	
	void makeDeadTree();
	void makeSand();
	void makeMountain();
	void makeTomb();
	void makeSpecialUp();
	void makeSpecialOpen();	
	void makeSpecialLocked();
	void makeSpecialItem();
	void makeRuins();	
	void makeFountain();	
	void makeDryFountain();	
    void makeTeleport();
    void makeRandom();
    void makeSpiderWeb();

	void setColor(int c1, int c2, int c3);
};

#endif // !defined(AFX_TERRAIN_H__94B87907_AC32_4D5A_9EAB_F7F1EF5D3BC5__INCLUDED_)
