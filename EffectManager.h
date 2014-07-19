// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#pragma once

#include "effects.h"
#include "monstermanager.h"

#define NO_CHANGE 0 
#define REDUCED 1
#define REMOVED 2

class EffectManager
{
public:
	EffectManager(void);
	~EffectManager(void);

	int RunEffect(MonsterData *monster, eEffect effect, int strength);	//effect that effect has on monster - also test reduce
	int TestGetEffect(MonsterData *monster, eBrandType brand, int strength); //does monster get effect
	void AddBrandEffect(MonsterData *monster, eBrandType brand, int strength); //add effect from brand
	void AddOtherEffect(MonsterData *monster, eEffect effect, int strength);	//add effect from other i.e potions etc

	eBrandType EquivalentResistance(eEffect effect);

	const std::string EffectName(eEffect effect);

private:
	int TestEffect(MonsterData *monster, eEffect effect, int strength);
	int RunPoison(MonsterData *monster, int strength);
	int RunSlowed(MonsterData *monster, int strength);
	void RunTeleport(MonsterData *monster, int strength);
};
