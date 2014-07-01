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

	int RunEffect(monsterData *monster, eEffect effect, int strength);	//effect that effect has on monster - also test reduce
	int TestGetEffect(monsterData *monster, eBrandType brand, int strength); //does monster get effect
	void AddBrandEffect(monsterData *monster, eBrandType brand, int strength); //add effect from brand
	void AddOtherEffect(monsterData *monster, eEffect effect, int strength);	//add effect from other i.e potions etc

	eBrandType EquivalentResistance(eEffect effect);

	const char* EffectName(eEffect effect);

private:
	int TestEffect(monsterData *monster, eEffect effect, int strength);
	int RunPoison(monsterData *monster, int strength);
	int RunSlowed(monsterData *monster, int strength);
	void RunTeleport(monsterData *monster, int strength);

};
