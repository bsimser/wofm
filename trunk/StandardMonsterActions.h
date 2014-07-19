// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_STANDARDMONSTERACTIONS_H__0827BB51_C8A4_43C0_81BA_74884B9970EF__INCLUDED_)
#define AFX_STANDARDMONSTERACTIONS_H__0827BB51_C8A4_43C0_81BA_74884B9970EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MonsterData;
class Item;
#include "SpellBase.h"

class StandardMonsterActions  
{
public:
	int		MoveMonster(MonsterData* monster,int x,int y);
	int		AttackMonster(MonsterData* monster,int x,int y);

	int		FireItem(MonsterData* monster,int x,int y);
	int		CalculateDamage(MonsterData* defender, int attackStr, int defenceStr);
    int		ThrowTarget(MonsterData* attacker, int targetX, int targetY, eAction action);
    int		ThrowItem(MonsterData* attacker, const unsigned int targetX, const unsigned int targetY, const unsigned int inventoryItemRef);
    
	int		CastSpell(MonsterData* caster, int spellID);
	int		CastTargetSpell(MonsterData* caster, eSpellList spell, int TargetX, int TargetY);
	int		AddBrandDamage(MonsterData* attacker,MonsterData* defender, Item* attack_weapon);

    static void ShowTrajectory(int level, int sourceX, int sourceY, int targetX, int targetY, char symbol, int c1, int c2, int c3);

};

#endif // !defined(AFX_STANDARDMONSTERACTIONS_H__0827BB51_C8A4_43C0_81BA_74884B9970EF__INCLUDED_)
