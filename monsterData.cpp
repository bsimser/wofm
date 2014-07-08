
#include "WorldBuilder.h"
#include ".\monsterdata.h"
#include "monsterAI.h"

int terrain_attack;
using namespace Random;

monsterData::monsterData(void) :
experience(0), is_archer(0), kills(0), is_magic(0), miss_turn(0), fleeing(false), flee_count(0), luck_penalty(0), luck_counter(0)

{
    terrain_attack = 1;

}

monsterData::~monsterData(void)
{
}


/////////////////////////////////////////////////////////////////////////////////////////////



int monsterData::NextAction(Action *action)
{
    int success = 0;
    static int player_finished_last_action = 1;

    if (isPlayer() && player_finished_last_action) //test if player is influenced by effect
    {
        player_finished_last_action = 0;
        MonsterAI ai;
        if (ai.ProcessIntelligence(this))
        {
            player_finished_last_action = 1;
            return 1;//turn has taken place
        }
        player_finished_last_action = 1;
    }

    switch (action->Type())
    {
    case aWait:  break;
    case aMove:  success = World.getActionManager().monsterAction.MoveMonster(this, action->param1, action->param2); break;
    case aAttack: success = World.getActionManager().monsterAction.AttackMonster(this, action->param1, action->param2); break;
    case aPickup: success = World.getMonsterManager().monsterItems.PickupItem(this); break;
    case aDrop:  success = World.getMonsterManager().monsterItems.DropItem(this, action->param1); break;
    case aFire:  success = World.getActionManager().monsterAction.FireItem(this, action->param1, action->param2); break;
    case aCastSpell:  success = World.getActionManager().monsterAction.CastSpell(this, action->param1); break;

    }

    if (terrain_attack)
        TerrainAttack(pos.x, pos.y);

    if (success)
    {
        if (fleeing)
        {
            flee_count--;
            if (flee_count == 0)
            {
                fleeing = false;
                World.getTextManager().newLine("You are no longer fleeing. ");
                monster.color1 = 200;
                monster.color2 = 200;
                monster.color3 = 40;
            }
        }
        if (luck_penalty < 0)
        {
            luck_counter--;
            if (luck_counter % 1000 == 0)
                luck_penalty++;
        }
    }

    return success;
}

int monsterData::TerrainAttack(int x, int y)
{

    terrain_attack = 0;

    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x][y].terrain.type == deepWater && Name() != "crocodile")
    {
        //	if(pack weight > 100) //drown
        //	{
        //		if(stamina == 1)
        //			World.getTextManager().newLine("Glug Glug!. ");
        //		else
        ///		{
        //			World.getTextManager().newLine("You are drowning. ");
        //		}
        //		stamina--;
        //	}

        //Piranhas 
        int piranhas_attack = getInt(20, 0);//+World.GetCurrentLevel()/2;
        if (piranhas_attack > 18)
        {
            if (isPlayer())
                World.getTextManager().newLine("You are being eaten by paranhas!!. ");
            else if (isSeen())
            {
                World.getTextManager().newLine("The %s is attacked by something. ", monster.name.c_str());
            }

            monster.stamina--;
            if (monster.stamina <= 0)
                World.getDeathMessage().SetDeathMessage("was eaten alive by piranhas. ");
        }
        else if (piranhas_attack > 5)
        {
            if (isPlayer())
                World.getTextManager().newLine("There's something in the water!! ");//("Something nibbles at you armour. ");
            else if (isSeen() == 1 && state != asleep)
            {
                if (isHumanoid())
                    World.getTextManager().newLine("The %s curses at the water. ", monster.name.c_str());
                else
                    World.getTextManager().newLine("The %s splashes around frantically. ", monster.name.c_str());
            }
            else if (isSeen() == 2 && state != asleep)
            {
                if (isHumanoid())
                    World.getTextManager().newLine("You hear cursing. ", monster.name.c_str());
                else
                    World.getTextManager().newLine("You hear splashing. ", monster.name.c_str());
            }
        }
        int slow_swept = World.GetCurrentLevel() / 2;
        if (World.GetCurrentLevel() == 0)
            slow_swept = 3;

        do //swept
        {
            x = pos.x;
            y = pos.y;
            if ((getInt(8, 0) + slow_swept--) > 5) //swept away
            {
                int swept_away = 1;

                if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x][y - 1].terrain.type == deepWater)
                {
                    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x][y - 1].GetMonster() == NULL)
                        swept_away = NextAction(World.getActionManager().UpdateAction(&action, aMove, pos.x, pos.y - 1));

                    else swept_away = 2;
                }

                else if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x + 1][y - 1].terrain.type == deepWater)
                {
                    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x + 1][y - 1].GetMonster() == NULL)
                        swept_away = NextAction(World.getActionManager().UpdateAction(&action, aMove, pos.x + 1, pos.y - 1));
                    else swept_away = 2;
                }

                else if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x - 1][y - 1].terrain.type == deepWater)
                {
                    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x - 1][y - 1].GetMonster() == NULL)
                        swept_away = NextAction(World.getActionManager().UpdateAction(&action, aMove, pos.x - 1, pos.y - 1));
                    else swept_away = 2;
                }
                else if (World.getDungeonManager().level[World.GetCurrentLevel()].map[x][y - 1].terrain.type == bridge
                    &&World.getDungeonManager().level[World.GetCurrentLevel()].map[x][y - 2].terrain.type == deepWater)
                    swept_away = NextAction(World.getActionManager().UpdateAction(&action, aMove, pos.x, pos.y - 2));

                if (swept_away == 0)
                {
                    monster.stamina = 0;
                    if (isPlayer())
                    {
                        World.getTextManager().newLine("You are swept away never to be seen again. ");//("Something nibbles at you armour. ");
                        World.getDeathMessage().SetDeathMessage("was swept away never to be seen again. ");
                    }

                    else if (isSeen() == 1)
                    {
                        World.getTextManager().newLine("The %s screams as it swept away. ", monster.name.c_str());
                    }
                    else if (isSeen() == 2)
                        World.getTextManager().newLine("You hear a gurgled scream. ");
                    break;
                }
                if (swept_away == 2)
                {
                    if (isPlayer() || isSeen() == 1)
                        World.getTextManager().newLine("Bump!");
                }
                else
                {
                    if (isPlayer())
                        World.getTextManager().newLine("You are swept away!! ");
                    else if (isSeen() == 1)
                    {
                        //char buf[64];
                        //sprintf(buf,"The %s is swept away. ",monster.name.c_str());
                        //World.getTextManager().newLine(buf);
                    }
                }

            }
            else break;
        } while ((slow_swept + 1) > 0);

    }

    terrain_attack = 1;
    return 1;
}

int monsterData::isPlayer()
{
    if (this == &(*World.getMonsterManager().Player()))
        return 1;
    else return 0;
}

int monsterData::isSeen()
{
    monsterData *player = World.getMonsterManager().Player();
    int hear_range = 20;

    if (level != World.GetCurrentLevel())
        return 0;

    int r = (int)sqrt((float)(pos.x - player->pos.x)*(pos.x - player->pos.x) + (player->pos.y - pos.y)*(player->pos.y - pos.y));/// +getInt(3, 0);

    int sightRange = player->monster.sight_range;
    if (level == 0)
        sightRange = 19;
    if (r <= sightRange)
    {
        //if(World.getDungeonManager().CurrentLevel()->IsCellVisible(pos.x,pos.y,player->pos.x,player->pos.y))
        //use point from player
        if (World.getDungeonManager().CurrentLevel()->IsCellVisible(player->pos.x, player->pos.y, pos.x, pos.y))
            return 1;
        else return 2;
    }

    else if (r <= hear_range)
        return 2;

    else return 0;
}
void monsterData::UpdateSightRange()
{
    if (World.GetCurrentLevel() >= 0)
        World.getDungeonManager().level[World.GetCurrentLevel()].LightDungeon(pos.x, pos.y, monster.sight_range);
    else
    {
        int x = 0;
    }
}

void monsterData::XP()
{
    if (experience < 50)
    {
    }

    else if (experience < 100)
    {
        if (experience_level != 1)
        {
            experience_level = 1;
            if (isPlayer())
                World.getTextManager().newLine("You gain a level. ");
            monster.stamina++;
            monster.max_stamina++;
            World.getSpellManager().AddMonsterSpell(this, spRepelMissiles);


        }
    }

    else if (experience < 200)
    {
        if (experience_level != 2)
        {
            experience_level = 2;
            if (isPlayer())
                World.getTextManager().newLine("You gain a level. ");
            monster.stamina++;
            monster.max_stamina++;
            World.getSpellManager().AddMonsterSpell(this, spSlowEnemies);

        }
    }


    else if (experience < 400)
    {
        if (experience_level != 3)
        {
            experience_level = 3;
            if (isPlayer())
                World.getTextManager().newLine("You gain a level. You feel more skillful. ");
            monster.skill++;
            monster.stamina++;
            monster.max_stamina++;
            World.getSpellManager().AddMonsterSpell(this, spTeleport);

        }
    }

    else if (experience < 800)
    {
        if (experience_level != 4)
        {
            experience_level = 4;
            if (isPlayer())
                World.getTextManager().newLine("You gain a level. ");
            monster.stamina++;
            monster.max_stamina++;
        }
    }

    else if (experience < 1600)
    {
        if (experience_level != 5)
        {
            experience_level = 5;
            if (isPlayer())
                World.getTextManager().newLine("You gain a level. ");

            monster.stamina++;
            monster.max_stamina++;
        }
    }

    else
    {
        if (experience_level != 6)
        {
            experience_level = 6;
            if (isPlayer())
                World.getTextManager().newLine("You gain a level. Wow you're buff!! ");
        }
    }
    kills++;

}

int monsterData::Items()
{
    int n = 0;
    ITEMLIST::iterator it;

    for (it = inventory.begin(); it != inventory.end(); it++)
    {
        n++;
    }

    return n;
}

int monsterData::Skill()
{
    return monster.skill;
}

std::string monsterData::Name()
{
    return monster.name;
}


int monsterData::AdjustedSkill()
{
    int skill = monster.skill;

    //	if(slots.weapon != NULL)

    Item* w = World.getMonsterManager().monsterItems.GetEquipment(this, weapon);
    if (w)
        skill += w->skill_bonus;

    return skill;
}

int monsterData::Stamina()
{
    return monster.stamina;
}

int monsterData::AttackStrength()
{
    int attack = monster.skill;

    //if(slots.weapon != NULL)
    Item* w = World.getMonsterManager().monsterItems.GetEquipment(this, weapon);
    if (w)
        attack += w->GetAttack_h2h(); //get weapon damage

    //half attack with no weapon for humanoids
    else if (isHumanoid() /*&&no unarmed combat skill*/) //no weapon
        attack = (int)(attack + .5) / 2;
    else attack += Random::getInt(6 + (level > 9) ? level / 2 : level, 1 + level / 3); //animal

    //attack is reduced in deep water
    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[pos.x][pos.y].terrain.type == deepWater &&
        Name() != "crocodile")
    {
        attack -= 5;
    }

    return attack;
}

int monsterData::DefendStrength()
{
    //defend strength with weapon not armour
    if (state == asleep)
        return 3;

    int defend = monster.skill;

    //if(slots.weapon != NULL)
    Item* w = World.getMonsterManager().monsterItems.GetEquipment(this, weapon);
    if (w)
        defend += w->GetAttack_h2h();

    //defend += slots.weapon->skill_bonus;

    else if (isHumanoid()) //no weapon
        defend = (int)(defend + .5) / 2;

    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[pos.x][pos.y].terrain.type == deepWater)
        defend -= 4;

    return defend;
}

bool monsterData::AbsorbTest()
{
    bool absorb = false;

    //if(slots.armour == NULL)
    if (!World.getMonsterManager().monsterItems.GetEquipment(this, armour))
        absorb = false;
    else
    {
        Item * a = World.getMonsterManager().monsterItems.GetEquipment(this, armour);
        int test = getInt(10, 0); //1-9
        if (a->absorb_bonus > test)
        {
            absorb = true;
        }
        else
        {
            // check luck 33% of time
            if (isPlayer() && Random::getInt(3, 0) == 1)
            {
                if (Stamina() < 4 && (Random::getInt(7, 1) + Random::getInt(7, 1)) < Luck())
                {
                    World.getTextManager().newLine("You feel lucky! ");
                    absorb = true;
                }
            }
        }
    }
    return absorb;
}

int monsterData::EquipKey(int level)
{
    //Item *item = 	World.getItemManager().CreateItem(level,key);
    //	inventory.push_back(*item);

    if (level == 0 || level == 3 || level == 6 || level == 8)
        inventory.push_back(*World.getItemManager().CreateItem(level, key));  //normal keys

    if (level == 4 || level == 9)
        inventory.push_back(*World.getItemManager().CreateItem(level + 10, key)); //special keys

    return 1;
}



void monsterData::Heal()
{
    int heal_test = getInt(501, 0);

    float s = (float)Stamina();
    float m = (float)monster.MaxStamina();

    float health = m / s;
    int chance = 0;

    if (health < 0.1) //health < 10%
        chance = 10;
    /*else if(health< 0.3) //health < 30%
        chance = 7;
        else if(health< 0.6) //health < 60%
        chance = 5;*/


    if (heal_test >(500 - chance))
        monster.stamina++;

    if (monster.stamina > monster.MaxStamina())
        monster.stamina = monster.MaxStamina();
}

int monsterData::TestArcher()
{
    Item* p = World.getMonsterManager().monsterItems.GetEquipment(this, projectile);
    Item* pw = World.getMonsterManager().monsterItems.GetEquipment(this, projectileWeapon);

    if (!p) //no projectiles
    {
        is_archer = 0;
        return 0;
    }
    if (!pw)  //no projectile weapon
    {
        if (p->itemNumber[0] != 0)
        {
            is_archer = 0;
            return 0;
        }
    }
    else if (pw) //projectile weapon
    {
        if (p->itemNumber[0] != pw->itemNumber[0])  //wrong projectiles
        {
            is_archer = 0;
            return 0;
        }
    }
    is_archer = 1;
    return 1;
}
