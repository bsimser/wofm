
#include "WorldBuilder.h"
#include "CastSpell.h"

SpellManager WorldBuilder::spellManager;

SpellManager::SpellManager(void)
{
}

SpellManager::~SpellManager(void)
{
}

int SpellManager::Initialise(void)
{
    CreateSpell(spFlyingWeapon);
    CreateSpell(spSlowEnemies);
    CreateSpell(spRepelMissiles);
    CreateSpell(spTeleport);
    CreateSpell(spDragonFire);

    return 1;
}

void SpellManager::CreateSpell(eSpellList type)
{
    SpellBase s;// = new SpellBase;
    if (s.CreateSpell(type))
        all_spells.push_back(s);

}
int SpellManager::CallSpellRoutine(monsterData* caster, int spell) //call spell from list;
{
    //GetSpell from caster spell list

    if (caster->spellList.size() == 0)
    {
        //throw std::exception("Cant cast spell");
        return 0;
    }

    MONSTERSPELLLIST::iterator sp;
    int i = 0;
    for (sp = caster->spellList.begin(); sp != caster->spellList.end(); sp++, i++)
    {
        if (spell == i)
            return CastSpell(caster, (*sp)->GetSpell());
    }

    currentSpell = spNone;
    //throw std::exception("Cant cast spell");
    return 0;
}

int SpellManager::AddMonsterSpell(monsterData* caster, eSpellList spell)
{
    SPELLLIST::iterator sp;

    // check spell exists
    for (sp = all_spells.begin(); sp != all_spells.end(); sp++)
    {
        if (sp->GetSpell() == spell)
        {
            bool allreadyHasit = false;
            for (MONSTERSPELLLIST::iterator it = caster->spellList.begin(); it != caster->spellList.end(); it++)
            {
                if ((*it)->GetSpell() == spell)
                    allreadyHasit = true;
            }
            if (!allreadyHasit)
                caster->spellList.push_back(&(*sp));
        }
    }
    return 1;
}
SpellBase* SpellManager::GetMonsterSpell(monsterData*caster, int random_spell)
{
    MONSTERSPELLLIST::iterator sp;

    int i = 0;
    for (sp = caster->spellList.begin(); sp != caster->spellList.end(); sp++, i++)
    {
        if (i == random_spell)
            return (*sp);
    }
    return NULL;
}

int SpellManager::CastSpell(monsterData* caster, int spell)
{
    CastMagic s;

    switch (spell)
    {
    case spFlyingWeapon:
        if (!caster->isPlayer())
            SpellText(caster, "casts magic missile at you");
        currentSpell = spFlyingWeapon;
        return SELECT_TARGET;

    case spSlowEnemies:
        if (!caster->isPlayer())
            SpellText(caster, "points at you and curses");
        s.SlowEnemies(caster);  	currentSpell = spSlowEnemies;
        return NORMAL;

    case spRepelMissiles:
        if (!caster->isPlayer())
        {
            if (Random::getInt(2, 0) == 1)
                SpellText(caster, "glows blue");
            else
                SpellText(caster, "glows purple");
        }

        s.RepelMissiles(caster);    currentSpell = spRepelMissiles;
        return NORMAL;

    case spTeleport:  
        if (!caster->isPlayer())
            SpellText(caster, "blinks");
        else
            WorldBuilder::textManager.newLine("You blink. ");
        s.Teleport(caster); currentSpell = spTeleport;
        return NORMAL;

    case spDragonFire:  
        if (!caster->isPlayer())
            SpellText(caster, "breaths fire at you");
        else
            WorldBuilder::textManager.newLine("You breath fire. ");

        currentSpell = spDragonFire;
        return SELECT_TARGET;
    }

    currentSpell = spNone;
    return CANCELED; //never get here
}

void SpellManager::SpellText(monsterData* caster, const char * spell_name)
{
    if (caster->isPlayer())
        WorldBuilder::textManager.newLine("You cast %s. ", spell_name);
    else
        WorldBuilder::textManager.newLine("The %s %s. ", caster->Name().c_str(), spell_name);
}

int SpellManager::CastCurrentSpell(monsterData* caster, int targetX, int targetY) //target select spells
{
    //CastSpellAt(caster,x,y,currentSpell);

    CastMagic s;

    DungeonLevel* level = WorldBuilder::dungeonManager.CurrentLevel();
    level->HighLightPath(caster->pos.x, caster->pos.y, targetX, targetY);
    COORDLIST::iterator it;

    for (it = level->show_path.begin(); it != level->show_path.end(); it++)
    {
        if (!level->IsCellTransparent(it->x, it->y) || level->map[it->x][it->y].monsterExists())
        {

            int ret = 0;

            switch (currentSpell)
            {
            case spFlyingWeapon:	ret = s.FlyingWeapon(caster, it->x, it->y); break;
            case spSlowEnemies:		ret = s.SlowEnemies(caster); break;
            case spRepelMissiles:	ret = s.RepelMissiles(caster); break;
            case spTeleport:		ret = s.Teleport(caster); break;
            case spDragonFire:		ret = s.DragonBreath(caster, it->x, it->y); break;
            }

            WorldBuilder::dungeonManager.CurrentLevel()->ClearPath(); //uncomment to remove path from monsters
            return ret;
        }
    }
    WorldBuilder::dungeonManager.CurrentLevel()->ClearPath();
    return 0;
}

void SpellManager::PrintSpells()
{
    //printf items
#ifdef _DEBUG
    std::ofstream ofile;

    ofile.open("Debug Files\\spells.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open spells.txt");

    ofile << "Items" << std::endl;

    ofile << "Name" << std::endl << std::endl;

    SPELLLIST::iterator sp;

    for (sp = all_spells.begin(); sp != all_spells.end(); sp++)
    {
        ofile << sp->Name();
        ofile << std::endl;
    }
    ofile.close();

#endif
}