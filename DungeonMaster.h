// DungeonMaster.h: interface for the DungeonMaster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUNGEONMASTER_H__3B0A13E8_CDC4_4D6F_9464_478538437580__INCLUDED_)
#define AFX_DUNGEONMASTER_H__3B0A13E8_CDC4_4D6F_9464_478538437580__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "DungeonManager.h" //manage and create dungeons
#include "MonsterManager.h" //manage and create monsters


class DungeonMaster  
{
public:
	DungeonMaster();
	virtual ~DungeonMaster();

	DungeonManager dungeonManager;
	MonsterManager monsterManager;

};

#endif // !defined(AFX_DUNGEONMASTER_H__3B0A13E8_CDC4_4D6F_9464_478538437580__INCLUDED_)
