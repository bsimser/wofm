// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#pragma warning(disable : 4786) 

#include "WorldBuilder.h"

#include "monsterAi.h"
#include "MonsterManager.h"

#include "NumberGenerator.h"
#include "EffectManager.h"

using namespace Random;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MonsterManager::MonsterManager():
m_monsterNum(0)
{
}

int MonsterManager::Initialise()
{
    monster_list.clear();
    return 1;
}

MonsterData* MonsterManager::CreateMonster(int major_type, int minor_type, int level, int x, int y)
{
    MonsterData new_monster;

    //select monsters
    switch (major_type)
    {
    case mGuards:{	SpecialGuardians guard; guard.Create(minor_type, level); new_monster.monster = guard; }break;
    case mUndead:{	UndeadMonster undead; undead.Create(minor_type, level); new_monster.monster = undead; }break;
    case mOrc:		new_monster.monster.Create(mOrc, level, 0); break;
    case mCrocodile:    new_monster.monster.Create(mCrocodile, level, 0); break;
    case mSpecial:  new_monster.monster.Create(mSpecial, level); break;
    case mWizard:	new_monster.monster.Create(mSpecial, level); break;
    case mPlayer:	new_monster.monster.Create(mPlayer, level); break;
    case mBridgeMaster:	new_monster.monster.Create(mBridgeMaster, level); break; //special case
    case mRandom:{	RandomMonster random; random.Create(level, minor_type); new_monster.monster = random; }break;

    default: new_monster.monster.Create(major_type, level); break;
    }
    new_monster.experience_level = level;

    //equip monsters
    if (new_monster.isHumanoid()) //equip humanoids
        //new_monster.EquipMonster(level);
    if (monster_list.size() == 0) //no monsters so must be player
        monsterItems.EquipPlayer(&new_monster);
    else
        monsterItems.EquipMonster(&new_monster, level);

    if (new_monster.isSpecial())
        new_monster.EquipKey(level);

    //set position
    new_monster.pos.x = x;
    new_monster.pos.y = y;
    new_monster.level = level;

    //set default action and state
    new_monster.action.CreateAction(aWait); //default action
    new_monster.SetState(normal);

    //monster ref
    new_monster.ref = m_monsterNum++;

    //set special states
    if (new_monster.monster.name == "orc sentry")
        new_monster.SetState(sentry);

    int asleep_test = getInt(100, 0);
    if (new_monster.GetState() == sentry) 
        asleep_test += 40; //40 %more chance that sentry will be asleep

    if (asleep_test > 90 + level && major_type != 0) //10% - level that creature will be asleep
        new_monster.SetState(asleep);

    //monster will not chase player unless seen
    new_monster.go_to.x = -1;
    new_monster.go_to.y = -1;

    new_monster.last_stamina = new_monster.monster.stamina;

    new_monster.TestArcher();

    // natural resistances
    if (new_monster.isUndead())
    {
    }

    //put monster in list //cannot use isPlayer until after this
    monster_list.push_back(new_monster);

    return &monster_list.back();
}

int MonsterManager::DestroyMonster(int ref)
{
    MONSTERLIST::iterator it;

    for (it = monster_list.begin(); it != monster_list.end(); it++)
    {
        if (it->ref == ref)
        {
            Coord * pos = it->getPosition();
            World.getDungeonManager().level(World.GetCurrentLevel()).getCell(pos->x, pos->y).RemoveMonsterRef();
            monster_list.erase(it);
            return 1;
        }
    }
    return 0;
}

int MonsterManager::DestroyMonster(MONSTERLIST::iterator it)
{
    //remove from map;
    Coord * pos = it->getPosition();
    World.getDungeonManager().level(World.GetCurrentLevel()).getCell(pos->x, pos->y).RemoveMonsterRef();

    monster_list.erase(it); //remove from list

    return 1;
}

MonsterData * /*MONSTERLIST::iterator */MonsterManager::Player()
{
    static MonsterData * player = &(*monster_list.begin());
    return player;
}

int MonsterManager::UpdateMonsters(DungeonLevel* dungeonLevel, ActionManager* actionManager)
{
    dungeonLevel = &World.getDungeonManager().level(World.GetCurrentLevel());

    //check status
    for (MONSTERLIST::iterator r_it = monster_list.begin(); r_it != monster_list.end();)
    {
        if (r_it->monster.stamina < 1) //death
        {
            if (r_it->isPlayer())
            {
                World.getTextManager().newLine("You die. ");
                World.getDeathMessage().ShowDeath(0);

                // death flash
                int c1 = r_it->monster.color1;
                int c2 = r_it->monster.color2;
                int c3 = r_it->monster.color3;
                for (int i = 0; i < 10; i++)
                {
                    r_it->monster.color1 = 255;
                    r_it->monster.color2 = 0;
                    r_it->monster.color3 = 0;
                    World.Render();
                    Sleep(50 );
                    r_it->monster.color1 = c1;
                    r_it->monster.color2 = c2;
                    r_it->monster.color3 = c3;
                    World.Render();
                    Sleep(50);
                }
                World.SetState(sDeath);
                return 0;
            }
            //	World.getTextManager().newLine("You die. Your spirit floats to another soul.");
            else if (r_it->isSeen())
            {
                World.getTextManager().newLine("The %s dies. ", r_it->monster.name.c_str());
            }

            if (r_it->Name() == "minotaur")
            {
                World.getTextManager().newLine("You see more clearly. ");
                dungeonLevel->setMapLight(DungeonLevel::eNormal);
            }

            World.getMonsterManager().monsterItems.DropRandomItems(&(*r_it));
            r_it->SetState(dead);
            r_it->monster.dead();
            if (r_it->monster.GetType() == mWizard)
            {
                World.getDeathMessage().done.zagor = 1;
                World.getTextManager().newLine("'And the mad wizard falls...hahaha'. ");
            }

            //remove from map;
            Coord * pos = r_it->getPosition();
            World.getDungeonManager().level(World.GetCurrentLevel()).getCell(pos->x, pos->y).RemoveMonsterRef();

            r_it = monster_list.erase(r_it); //remove from list
        }
        else
        {
            r_it++;
        }
    }

    //update monster/player with new positions
    for (MONSTERLIST::iterator it = monster_list.begin(); it != monster_list.end(); it++)
    {
        //RunEffects(&(*it));

        if (it->isPlayer()) //no AI for player
        {
            Coord * pos = it->getPosition();
            dungeonLevel->getCell(pos->x, pos->y).AssignMonster(&it->monster);
            //static int gogo =0; //effect tests
            //if(gogo==0)
            {
                //it->monster.AddEffect(poisoned,3);
                //it->monster.AddEffect(diseased,3);
                //it->monster.AddEffect(slowed,3);
                //it->monster.AddEffect(paralysis,3);
                //it->monster.AddEffect(teleportitus,3);
                //it->monster.AddEffect(confused,3);
                //	gogo =1;
            }
        }
        else
        {
            if (it->level == World.GetCurrentLevel()) //don't process AI for monsters on other levels
            {
                if (it->Name() == "troll")
                {
                    if (Random::getInt(3, 0) == 1)
                    {
                        if (it->monster.stamina < it->monster.MaxStamina())
                        {
                            it->monster.stamina++;
                            if (it->isSeen() == 1)
                                World.getTextManager().newLine("The troll regenerates! ");
                        }
                    }
                }
                MonsterAI ai;

                ai.ProcessIntelligence(&(*it));

                Coord * pos = it->getPosition();
                dungeonLevel->getCell(pos->x, pos->y).AssignMonster(&it->monster);

                it->last_stamina = it->monster.stamina;
            }
        }
    }

    if (World.getMonsterManager().monster_list.size() == 1 && World.getMonsterManager().monsterItems.GetEquipment(World.getMonsterManager().Player(), gold) != NULL)
    {
        World.getDeathMessage().SetDeathMessage("killed all that moved and acquired the treasure. WOW!");
        World.getDeathMessage().ShowDeath(1);
        Sleep(1000);
        World.SetState(sDeath);
    }

    return 1;
}

MonsterData * MonsterManager::FindMonsterData(Monster * monster)
{
    if (monster == NULL)
        return NULL;
    MONSTERLIST::iterator it;

    for (it = monster_list.begin(); it != monster_list.end(); it++)
    {
        if (monster == &it->monster)
            return &(*it);
    }

    return NULL;
}

int MonsterManager::CalculateBrandDamageOnMonster(MonsterData *defender, eBrandType brandType, int damage)
{
    int new_damage = damage;
    int monster_resistance = 0;

    if (defender->monster.hasResistance()) //get natural resistance
    {
        monster_resistance = defender->monster.GetResistance(bFire);
        //resList.push_back(monster_resistance);
    }

    int found_all = 0;

    ITEMLIST::iterator it;

    //do  //search through equipment
    //{
    int highest_res = 0;
    for (it = defender->inventory.begin(); it != defender->inventory.end(); it++)
    {
        int res = it->GetResistance(brandType);
        if (res > highest_res)
            highest_res = res;
    }


    //}while (!found_all);

    //Get resistance from equipment 

    float modifier = 1 - (highest_res)*0.3f;

    new_damage = (int)((damage* modifier) + .5f);

    return new_damage;
}

void MonsterManager::RunEffects(MonsterData * monster)
{
    /*EFFECTMAP_CITERATOR it;
    EffectManager effectManager;

    for(it = monster->monster.effectMap.begin();it !=monster->monster.effectMap.end();it++)
    {
    effectManager.RunEffect(monster,it->first, it->second);
    }*/
}

void MonsterManager::PrintMonsters()
{
    //printf creaturns

#ifdef _DEBUG
    std::ofstream ofile;

    ofile.open("Debug Files\\creatures.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open creatures.txt");

    ofile << "Creatures" << std::endl;

    ofile << "Name,Skill,Stamina,Humanoid" << std::endl;

    MONSTERLIST::iterator it;

    for (it = World.getMonsterManager().monster_list.begin(); it != World.getMonsterManager().monster_list.end(); it++)
    {
        ofile << it->monster.name << "," << (int)it->monster.skill << "," << (int)it->monster.stamina << "," << (it->isHumanoid()?"true":"false");
        ofile << std::endl;
    }
    ofile.close();
#endif
}

const std::string MonsterManager::getDescription(const Monster & monster) const
{
    return monster.getDescription();
}
