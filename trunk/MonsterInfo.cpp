/// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#include "WorldBuilder.h"
#include "MonsterInfo.h"
#include "EffectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

void MonsterInfo::ShowMonsterInfo(MonsterData* monster)
{
    //#ifdef _DEBUG
    //	ShowCompleteMonsterInfo(monster);
    //#else
    ShowNormalMonsterInfo(monster);
    //#endif

}
void MonsterInfo::ShowNormalMonsterInfo(MonsterData* monster)
{
    World.getTextManager().ClearDisplayLines();

    World.getTextManager().SetDisplayLine(0, "%s", monster->monster.name.c_str());
    World.getTextManager().SetDisplayLine(1, "==============");

    World.getTextManager().SetDisplayLine(3, "%s", World.getMonsterManager().getDescription(monster->monster).c_str());

    float max_stamina = (float)monster->monster.MaxStamina();
    float cur_stamina = (float)monster->monster.stamina;

    int state = monster->GetState();

    float wounded = (max_stamina - cur_stamina) / max_stamina * 100;
    char health[32];
    char status[32];

    if (wounded < 20)
        strcpy(health, "healthy");
    else if (wounded < 50)
        strcpy(health, "wounded");
    else if (wounded < 80)
        strcpy(health, "very wounded");
    else
        strcpy(health, "almost dead");

    if (monster->isPlayer())
        strcpy(status, "You are ready for anything.");
    else
    {
        switch (state)
        {
        case normal:	strcpy(status, "It has not seen you."); break;
        case sentry:	strcpy(status, "It appears asleep."); break;
        case hostile:	strcpy(status, "It is hostile."); break;
        case waking:	strcpy(status, "It looked annoyed."); break;
        case asleep:	strcpy(status, "It is asleep."); break;
        }
    }
    if (monster->isPlayer())
        World.getTextManager().SetDisplayLine(5, "You look %s.", health);
    else
        World.getTextManager().SetDisplayLine(5, "It looks %s.", health);

    //sprintf(buf,"%s",status);
    World.getTextManager().SetDisplayLine(6, "%s", status);

    int pSkill = World.getMonsterManager().Player()->Skill();
    int tSkill = monster->Skill();

    if (tSkill < 6) //0-5
        World.getTextManager().SetDisplayLine(7, "Experience: Low");

    else if (tSkill < 7) //6
        World.getTextManager().SetDisplayLine(7, "Experience: Average");
    else if (tSkill < 9) //6-7
        World.getTextManager().SetDisplayLine(7, "Experience: Medium");
    else if (tSkill < 11) //8-9
        World.getTextManager().SetDisplayLine(7, "Experience: High");
    else //>=10
        World.getTextManager().SetDisplayLine(7, "Experience: Very High");

    //inventory
    World.getTextManager().SetDisplayLine(9, "Equipment");
    ITEMLIST::iterator it;
    int i = 10;
    for (it = monster->inventory.begin(); it != monster->inventory.end(); it++, i++)
    {
#ifdef _DEBUG
        World.getTextManager().SetDisplayLine(i, "%s", it->GetName().c_str());
#else
        if (it->equipped)
        {
            World.getTextManager().SetDisplayLine(i, "%s", it->GetName().c_str());
        }
        else
            i--;
#endif
    }

    i++;
    if (monster->monster.effectList.size() > 0)
        World.getTextManager().SetDisplayLine(i++, "Active Effects"); i++;

    EffectManager effectManager;

    EFFECTLIST::iterator eff;

    for (eff = monster->monster.effectList.begin(); eff != monster->monster.effectList.end(); eff++, i++)
    {
        if (eff->strength > 0)
        {
            std::string eff_msg = effectManager.EffectName(eff->type);
            for (int e = 1; e < eff->strength; e++)
                eff_msg.append("!");
            World.getTextManager().SetDisplayLine(i, (char *)eff_msg.c_str());
        }
    }

    i++;
    if (monster->monster.hasResistance() > 0)
        World.getTextManager().SetDisplayLine(i++, "Resistances and vulnerabilities"); i++;

    BRANDMAP::iterator resist_it;
    for (resist_it = monster->monster.resistanceMap.begin(); resist_it != monster->monster.resistanceMap.end(); resist_it++, i++)
    {
        World.getTextManager().SetDisplayLine(i, "  %s (%d)", ResistanceBrands::GetResistanceName(resist_it->first), resist_it->second);
    }
    World.getTextManager().SetDisplayLine(39, "[x] to Return to looking.");
}

void MonsterInfo::ShowCompleteMonsterInfo(MonsterData* monster)
{
    int i;
    for (i = 3; i < 30; i++)
        World.getTextManager().SetDisplayLine(i, "");

    char buf[128];

    World.getTextManager().SetDisplayLine(0, "%s", monster->monster.name.c_str());
    World.getTextManager().SetDisplayLine(1, "==========");

    World.getTextManager().SetDisplayLine(3, "");

    sprintf(buf, "Stamina: %d", monster->Stamina());
    World.getTextManager().SetDisplayLine(5, buf);

    sprintf(buf, "Skill: %d(+%d)", monster->AdjustedSkill(), monster->AdjustedSkill() - monster->Skill());
    World.getTextManager().SetDisplayLine(6, buf);

    switch (monster->GetState())
    {
    case asleep: sprintf(buf,  "State: asleep"); break;
    case sentry: sprintf(buf,  "State: sentry"); break;
    case normal: sprintf(buf,  "State: normal"); break;
    case hostile: sprintf(buf, "State: hostile"); break;
    case dead: sprintf(buf,    "State: dead"); break;
    }

    World.getTextManager().SetDisplayLine(7, buf);

    sprintf(buf, "Experience: %d", monster->experience);
    World.getTextManager().SetDisplayLine(8, buf);

    sprintf(buf, "Experience Level: %d", monster->experience_level);
    World.getTextManager().SetDisplayLine(9, buf);

    sprintf(buf, "Kills: %d", monster->Kills());
    World.getTextManager().SetDisplayLine(10, buf);

    sprintf(buf, "Dungeon Level: %d", monster->level);
    World.getTextManager().SetDisplayLine(11, buf);

    sprintf(buf, "Inventory Items %d", monster->inventory.size());
    World.getTextManager().SetDisplayLine(13, buf);

    ITEMLIST::iterator it;

    i = 15;
    for (it = monster->inventory.begin(); it != monster->inventory.end(); it++, i++)
    {

        //	if(it->equipped)
        {
            World.getTextManager().SetDisplayLine(i, "%s", it->GetName().c_str());
        }
    }
    i++;
    World.getTextManager().SetDisplayLine(i++, "Effects");

    EffectManager effectManager;

    EFFECTLIST::iterator eff;

    for (eff = monster->monster.effectList.begin(); eff != monster->monster.effectList.end(); eff++, i++)
    {
        if (eff->strength > 0)
            World.getTextManager().SetDisplayLine(i, (char*)effectManager.EffectName(eff->type).c_str());
    }

    /*EFFECTMAP_CITERATOR eff;
    for(eff=monster->monster.effectMap.begin();eff != monster->monster.effectMap.end();it++,i++)
    {
    if(eff->second > 0)
    World.getTextManager().SetDisplayLine(i,(char*)effectManager.EffectName(eff->first).c_str());

    }*/


    World.getTextManager().SetDisplayLine(39, "[x] to Exit");


}