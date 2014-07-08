// StandardMonsterActions.h: interface for the StandardMonsterActions class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STANDARDMONSTERACTIONS_H__0827BB51_C8A4_43C0_81BA_74884B9970EF__INCLUDED_)
#define AFX_STANDARDMONSTERACTIONS_H__0827BB51_C8A4_43C0_81BA_74884B9970EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class monsterData;
class Item;
#include "SpellBase.h"

class StandardMonsterActions  
{
public:
	int		MoveMonster(monsterData* monster,int x,int y);
	int		AttackMonster(monsterData* monster,int x,int y);

	int		FireItem(monsterData* monster,int x,int y);
	int		CalculateDamage(monsterData* defender, int attackStr, int defenceStr);
	int		ThrowTarget(monsterData* attacker,int targetX, int targetY,eAction action);

	int		CastSpell(monsterData* caster, int spellID);
	int		CastTargetSpell(monsterData* caster, eSpellList spell, int TargetX, int TargetY);
	int		AddBrandDamage(monsterData* attacker,monsterData* defender, Item* attack_weapon);

    static void ShowTrajectory(int level, int sourceX, int sourceY, int targetX, int targetY, char symbol, int c1, int c2, int c3);

};

#endif // !defined(AFX_STANDARDMONSTERACTIONS_H__0827BB51_C8A4_43C0_81BA_74884B9970EF__INCLUDED_)
