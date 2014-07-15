#pragma once

#include <list>
#include <map>
#include "SpellBase.h"

class MonsterData;

typedef std::list <SpellBase> SPELLLIST;
typedef std::list <SpellBase*> MONSTERSPELLLIST;

//typedef std::map <eSpellList,SpellBase> SPELLMAP;

class SpellManager
{
public:
    int Initialise(void);

    void CreateSpell(eSpellList type);

    int AddMonsterSpell(MonsterData* caster, eSpellList spell);
    SpellBase* GetMonsterSpell(MonsterData*caster, int random_spell);


    int CallSpellRoutine(MonsterData *caster, int spell);
    int SpellManager::CastSpell(MonsterData* caster, int spell);
    int CastCurrentSpell(MonsterData* caster, int x, int y);

    char * GetSpellName(eSpellList id);

    void PrintSpells();

private:
    eSpellList currentSpell;
    int CalculateDamage(int attackStr, int defenceStr);

    void SpellText(MonsterData* caster, const char * spell_name);

    //	SPELLMAP spellMap; //contains every of spell
    SPELLLIST all_spells;


};
