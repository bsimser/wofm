// DungeonManager.h: interface for the DungeonManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUNGEONMANAGER_H__AE152525_8D5E_4921_81AF_15FF648CBCC7__INCLUDED_)
#define AFX_DUNGEONMANAGER_H__AE152525_8D5E_4921_81AF_15FF648CBCC7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DungeonLevel.h"

#define MAX_DUNGEON_LEVELS 20

class DungeonManager  
{
public:
	int Initialise();

	DungeonLevel level[21];		

	virtual int PopulateDungeon(int level);

	Coord * NewMonsterPosition(int lev);
	Coord * NewBridgePosition(int lev);

	virtual int AddItems(int lev);

	virtual int BuildCompleteDungeon(int max_num_levels);

	virtual int CreateDungeon(int dlevel,int type);
	virtual int DeleteDungeon(int dlevel);

	void CreateLockedDungeon();
	void CreateSpecialDungeon();

	void PrintDungeons();
	void PopulateSpecialDungeon(int level,int type);

	DungeonLevel* Level(int id){return &level[id];};

	DungeonLevel* CurrentLevel();

private:
	int AddKeysToDungeon();

};

#endif // !defined(AFX_DUNGEONMANAGER_H__AE152525_8D5E_4921_81AF_15FF648CBCC7__INCLUDED_)
