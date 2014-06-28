// MonsterInfo.cpp: implementation of the MonsterInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "MonsterInfo.h"
#include "EffectManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MonsterInfo::MonsterInfo()
{
}

MonsterInfo::~MonsterInfo()
{
}


void MonsterInfo::ShowMonsterInfo(monsterData* monster)
{
    //#ifdef _DEBUG
    //	ShowCompleteMonsterInfo(monster);
    //#else
    ShowNormalMonsterInfo(monster);
    //#endif

}
void MonsterInfo::ShowNormalMonsterInfo(monsterData* monster)
{
    WorldBuilder::textManager.ClearDisplayLines();

    WorldBuilder::textManager.SetDisplayLine(0, monster->monster.name);
    WorldBuilder::textManager.SetDisplayLine(1, "==============");

    WorldBuilder::textManager.SetDisplayLine(3, "%s", WorldBuilder::monsterManager.getDescription(monster->monster).c_str());

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
        WorldBuilder::textManager.SetDisplayLine(5, "You look %s.", health);
    else
        WorldBuilder::textManager.SetDisplayLine(5, "It looks %s.", health);

    //sprintf(buf,"%s",status);
    WorldBuilder::textManager.SetDisplayLine(6, "%s", status);

    int pSkill = WorldBuilder::monsterManager.Player()->Skill();
    int tSkill = monster->Skill();

    if (tSkill < 6) //0-5
        WorldBuilder::textManager.SetDisplayLine(7, "Experience: Low");

    else if (tSkill < 7) //6
        WorldBuilder::textManager.SetDisplayLine(7, "Experience: Average");
    else if (tSkill < 9) //6-7
        WorldBuilder::textManager.SetDisplayLine(7, "Experience: Medium");
    else if (tSkill < 11) //8-9
        WorldBuilder::textManager.SetDisplayLine(7, "Experience: High");
    else //>=10
        WorldBuilder::textManager.SetDisplayLine(7, "Experience: Very High");

    //inventory
    WorldBuilder::textManager.SetDisplayLine(9, "Equipment");
    ITEMLIST::iterator it;
    int i = 10;
    for (it = monster->inventory.begin(); it != monster->inventory.end(); it++, i++)
    {
        if (it->equipped)
        {
            WorldBuilder::textManager.SetDisplayLine(i, it->GetName());
        }
        else i--;
    }

    i++;
    if (monster->monster.effectList.size() > 0)
        WorldBuilder::textManager.SetDisplayLine(i++, "Active Effects"); i++;

    EffectManager effectManager;

    EFFECTLIST::iterator eff;

    for (eff = monster->monster.effectList.begin(); eff != monster->monster.effectList.end(); eff++, i++)
    {
        if (eff->strength > 0)
        {
            std::string eff_msg = effectManager.EffectName(eff->type);
            for (int e = 1; e < eff->strength; e++)
                eff_msg.append("!");
            WorldBuilder::textManager.SetDisplayLine(i, (char *)eff_msg.c_str());
        }
    }

    i++;
    if (monster->monster.hasResistance() > 0)
        WorldBuilder::textManager.SetDisplayLine(i++, "Resistances and vulnerabilities"); i++;

    BRANDMAP::iterator resist_it;
    for (resist_it = monster->monster.resistanceMap.begin(); resist_it != monster->monster.resistanceMap.end(); resist_it++, i++)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "  %s (%d)", ResistanceBrands::GetResistanceName(resist_it->first), resist_it->second);
    }

    /*EFFECTMAP_CITERATOR eff;
    for(eff=monster->monster.effectMap.begin();eff != monster->monster.effectMap.end();it++,i++)
    {
    if(eff->second > 0)
    WorldBuilder::textManager.SetDisplayLine(i,(char*)effectManager.EffectName(eff->first));

    }*/
    WorldBuilder::textManager.SetDisplayLine(39, "[x] to Return to looking.");

}


void MonsterInfo::ShowCompleteMonsterInfo(monsterData* monster)
{
    int i;
    for (i = 3; i < 30; i++)
        WorldBuilder::textManager.SetDisplayLine(i, "");

    char buf[128];

    WorldBuilder::textManager.SetDisplayLine(0, monster->monster.name);
    WorldBuilder::textManager.SetDisplayLine(1, "==========");

    WorldBuilder::textManager.SetDisplayLine(3, "");

    sprintf(buf, "Stamina: %d", monster->Stamina());
    WorldBuilder::textManager.SetDisplayLine(5, buf);

    sprintf(buf, "Skill: %d(+%d)", monster->AdjustedSkill(), monster->AdjustedSkill() - monster->Skill());
    WorldBuilder::textManager.SetDisplayLine(6, buf);


    switch (monster->GetState())
    {
    case asleep: sprintf(buf, "State: asleep"); break;
    case sentry: sprintf(buf, "State: sentry"); break;
    case normal: sprintf(buf, "State: normal"); break;
    case hostile: sprintf(buf, "State: hostile"); break;
    case dead: sprintf(buf, "State: dead"); break;
    }

    WorldBuilder::textManager.SetDisplayLine(7, buf);

    sprintf(buf, "Experience: %d", monster->experience);
    WorldBuilder::textManager.SetDisplayLine(8, buf);

    sprintf(buf, "Experience Level: %d", monster->experience_level);
    WorldBuilder::textManager.SetDisplayLine(9, buf);

    sprintf(buf, "Kills: %d", monster->Kills());
    WorldBuilder::textManager.SetDisplayLine(10, buf);

    sprintf(buf, "Dungeon Level: %d", monster->level);
    WorldBuilder::textManager.SetDisplayLine(11, buf);

    sprintf(buf, "Inventory Items %d", monster->inventory.size());
    WorldBuilder::textManager.SetDisplayLine(13, buf);

    ITEMLIST::iterator it;

    i = 15;
    for (it = monster->inventory.begin(); it != monster->inventory.end(); it++, i++)
    {

        //	if(it->equipped)
        {
            WorldBuilder::textManager.SetDisplayLine(i, it->GetName());
        }
    }
    i++;
    WorldBuilder::textManager.SetDisplayLine(i++, "Effects");

    EffectManager effectManager;

    EFFECTLIST::iterator eff;

    for (eff = monster->monster.effectList.begin(); eff != monster->monster.effectList.end(); eff++, i++)
    {
        if (eff->strength > 0)
            WorldBuilder::textManager.SetDisplayLine(i, (char*)effectManager.EffectName(eff->type));
    }

    /*EFFECTMAP_CITERATOR eff;
    for(eff=monster->monster.effectMap.begin();eff != monster->monster.effectMap.end();it++,i++)
    {
    if(eff->second > 0)
    WorldBuilder::textManager.SetDisplayLine(i,(char*)effectManager.EffectName(eff->first));

    }*/


    WorldBuilder::textManager.SetDisplayLine(39, "[x] to Exit");


}