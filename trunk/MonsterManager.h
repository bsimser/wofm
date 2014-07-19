// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_MONSTERMANAGER_H__31959C7C_29D8_4DF2_9B54_BA49F3533281__INCLUDED_)
#define AFX_MONSTERMANAGER_H__31959C7C_29D8_4DF2_9B54_BA49F3533281__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MonsterData.h"

#include "MonsterItems.h"
#include "DungeonManager.h" //manage and create dungeons

#include "ActionManager.h"	//manage and create actions
#include "ItemManager.h"

//non standard monsters
#include "UndeadMonster.h"
#include "SpecialGuardians.h"
#include "RandomMonster.h"

#include <vector>

enum eMonsterMajorType
{
	mNormal, //will not exist
	mUndead,
	mOrc,
	mSpecial,
	mWizard,
	mPlayer,
	mBridgeMaster,
	mGuards,
    mRandom,
    mCrocodile,
    mDigger,
};

/*struct equipmmentSlots
{
	Item * armour;
	Item * weapon;
};*/

typedef std::vector<MonsterData> MONSTERLIST;

class MonsterManager  
{
public:
	MonsterManager();

	int Initialise();
	MonsterData*  CreateMonster(int major_type, int minor_type,  int level, int x, int y);
	int DestroyMonster(int ref);
	int DestroyMonster(MONSTERLIST::iterator it);
	MonsterData * FindMonsterData(Monster * monster);
	MonsterData* Player();

	int UpdateMonsters(DungeonLevel* dungeonLevel,ActionManager* actionManager);

	int GetNumMonsters(){return m_monsterNum;};

	MONSTERLIST monster_list;

	MonsterItems monsterItems;

	void PrintMonsters();

	int CalculateBrandDamageOnMonster(MonsterData *defender, eBrandType brandType, int damage);
	void RunEffects(MonsterData * monster);

    const std::string getDescription(const Monster & monster) const;

private:
	
	int m_monsterNum;

};

#endif // !defined(AFX_MONSTERMANAGER_H__31959C7C_29D8_4DF2_9B54_BA49F3533281__INCLUDED_)
