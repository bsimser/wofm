#pragma once

#include <list>
#include <map>
#include "SpellBase.h"


class monsterData;

typedef std::list <SpellBase> SPELLLIST;
typedef std::list <SpellBase*> MONSTERSPELLLIST;

//typedef std::map <eSpellList,SpellBase> SPELLMAP;

class SpellManager
{
public:
    SpellManager(void);
    ~SpellManager(void);

    int Initialise(void);

    void CreateSpell(eSpellList type);

    int AddMonsterSpell(monsterData* caster, eSpellList spell);
    SpellBase* GetMonsterSpell(monsterData*caster, int random_spell);


    int CallSpellRoutine(monsterData *caster, int spell);
    int SpellManager::CastSpell(monsterData* caster, int spell);
    int CastCurrentSpell(monsterData* caster, int x, int y);

    char * GetSpellName(eSpellList id);

    void PrintSpells();

private:
    eSpellList currentSpell;
    int CalculateDamage(int attackStr, int defenceStr);

    void SpellText(monsterData* caster, const char * spell_name);

    //	SPELLMAP spellMap; //contains every of spell
    SPELLLIST all_spells;


};
