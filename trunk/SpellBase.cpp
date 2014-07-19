// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#include ".\spellbase.h"

SpellBase::SpellBase(void)
{
    spell_desription = " ";
    base_cost = 0;
}

SpellBase::~SpellBase(void)
{
}

SpellBase::SpellBase(const SpellBase& rhs)
{
    //target = rhs. target;
    //type = rhs.type;
    //spellname = rhs.spellname;

    //strcpy(name,rhs.name);

    CreateSpell(rhs.spellname);
}


int SpellBase::CreateSpell(eSpellList spell)
{
    spellname = spell;

    switch (spell)
    {
    case spFlyingWeapon:	createFlyingWeapon(); break;
    case spTeleport:		createTeleport(); break;
    case spRepelMissiles:   createRepelMissiles(); break;
    case spSlowEnemies:	    createSlowMonsters(); break;
    case spDragonFire:	    createDragonFire(); break;


        //	case FlyingWeapon: createFlyingWeapon(); break;
        //	case FlyingWeapon: createFlyingWeapon(); break;
        //	case FlyingWeapon: createFlyingWeapon(); break;
        //	case FlyingWeapon: createFlyingWeapon(); break;
        //	case FlyingWeapon: createFlyingWeapon(); break;
    default: return 0;
    }
    return 1;

}

void SpellBase::createFlyingWeapon()
{
    strcpy(name, "Magic Missile ");
    spell_desription = "Unleashes raw energy and forms it into a missile";

    target = spTarget;
    type = spAttack;

    base_cost = 2;
}
void SpellBase::createTeleport()
{
    strcpy(name, "Blink         ");
    spell_desription = "Teleports to random destination";

    target = spTarget;
    type = spPanic;

    base_cost = 5;
}
void SpellBase::createRepelMissiles()
{
    strcpy(name, "Repel Missiles");
    spell_desription = "Repel Missiles";

    target = spTarget;
    type = spEnhance;

    base_cost = 4;
}

void SpellBase::createSlowMonsters()
{
    strcpy(name, "Slow Creatures");
    spell_desription = "Slows enemies in sight range";

    target = spAreaEffect;
    type = spAttack;

    base_cost = 5;
}

void SpellBase::createDragonFire()
{
    strcpy(name, "Dragon Fire");
    spell_desription = "Dragon Fire";

    target = spTarget;
    type = spAttack;

    base_cost = 5;
}