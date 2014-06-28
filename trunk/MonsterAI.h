// MonsterAI.h: interface for the MonsterAI class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTERAI_H__63120427_CA91_4CCC_9525_1E2F1D16AE39__INCLUDED_)
#define AFX_MONSTERAI_H__63120427_CA91_4CCC_9525_1E2F1D16AE39__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MonsterManager.h"

enum eActionList
{
	move,
	pickup,
	equip,
	attack,
};

class MonsterAI 
{
public:
	MonsterAI();
	virtual ~MonsterAI();

	int ProcessIntelligence(monsterData* monster);

private:
	
	eMonsterState UpdateMonsterState(monsterData* monster);
	int Detect(monsterData* monster);
	int	DetermineAction(monsterData* monster);
	int RandomMove(monsterData* monster);
	int ChasePlayer(monsterData* monster);
	int AttackPlayer(monsterData* monster);
	int DistanceAttackMonster(monsterData* monster);
	void DoNothing(monsterData* monster);
	int DistanceAttackPlayer(monsterData* monster);
	int MagicAttackPlayer(monsterData* monster);
	
	int ProcessEffects(monsterData* monster);
	int	EffectAction(monsterData* monster,eEffect effect,int strength); //run effects I.e confused

	
	int DetectPlayer(int m_x,int m_y,int *p_x,int *p_y);
	int MoveCloserToPlayer(int m_x,int m_y,int p_x,int p_y);
	int LOS(int x,int y,int x2,int y2, int range);
	int CheckValidMove(int m_x,int m_y,int dir);

	int detect;
	int detect_timeout;
};

#endif // !defined(AFX_MONSTERAI_H__63120427_CA91_4CCC_9525_1E2F1D16AE39__INCLUDED_)
