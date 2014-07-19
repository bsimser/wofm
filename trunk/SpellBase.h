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

#include <string>


enum eSpellList
{
	spNone,
	spFlyingWeapon,
	spSlowEnemies,
	spRepelMissiles,
	spTeleport,
	spDragonFire
};

enum eSpellTarget
{
	spTarget,		//requires target
	spAreaEffect,	//area effect - centred around caster
	//spSelf			//Self - maybe redundant as could use self target.
};

enum eSpellType //used for AI 
{
	spAttack,		//damage/hamper monster	i.e magic missile, slow monster
	spDefence,		//heal/help caster	i.e heal, repel missiles
	spEnhance,		//make caster more powerful	i.e blade hands, speed
	spPanic,		//cast in emergency i.e teleport
};

class SpellBase
{
public:
	SpellBase(void);
	~SpellBase(void);

	SpellBase(const SpellBase& rhs);

	int CreateSpell(eSpellList type);

	char * Name(){return name;};
	eSpellList GetSpell(){return spellname;};
	eSpellTarget GetTarget(){return target;};
	eSpellType GetType(){return type;};

	const std::string& Desription() {return spell_desription;};
	const int Cost(){return base_cost;};

private:
	void createFlyingWeapon();
	void createTeleport();
	void createRepelMissiles();
	void createSlowMonsters();
	void createDragonFire();

	eSpellTarget target;
	eSpellType type;
	eSpellList spellname;

	char name[32];
	int base_cost;
	std::string spell_desription;
};
