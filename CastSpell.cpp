#include "WorldBuilder.h"
#include ".\CastSpell.h"

// -------------------------------------------------------------------------------------------------------------------------------

int	CastMagic::SlowEnemies(monsterData* caster)
{
    if (caster->isPlayer())
    {
        MONSTERLIST::iterator it;
        //get list of enemies on LOS.
        for (it = World.getMonsterManager().monster_list.begin(); it != World.getMonsterManager().monster_list.end(); it++)
        {
            if (it->isSeen() == 1 && !it->isPlayer())
            {
                int resistance = it->monster.GetResistance(bSlow);
                if (Random::getInt(10, 0) + resistance < 5)
                {
                    it->monster.AddEffect(slowed, 3);
                    World.getTextManager().newLine("The %s is slowed. ", it->monster.name.c_str());
                }
                else
                    World.getTextManager().newLine("The %s resists. ", it->monster.name.c_str());
            }
        }
    }
    else //cast on player
    {
        monsterData* player = World.getMonsterManager().Player();
        int resistance = player->monster.GetResistance(bSlow);
        if (Random::getInt(10, 0) + resistance < 5)
        {
            player->monster.AddEffect(slowed, 3);
            World.getTextManager().newLine("You are slowed. ");
        }
        else
            World.getTextManager().newLine("You resist. ");
    }
    return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------

int	CastMagic::RepelMissiles(monsterData* caster) //glow blue?
{
    if (caster->isPlayer())
    {
        if (caster->monster.GetEffect(repelMissiles)) //already done
            World.getTextManager().newLine("Your glow increases. ");
        else
            World.getTextManager().newLine("You start glowing. ");
    }
    else
    {
        if (caster->monster.GetEffect(repelMissiles)) //already done
            World.getTextManager().newLine("The %s glows brighter. ", caster->monster.name.c_str());
        else
            World.getTextManager().newLine("The %s starts glowing. ", caster->monster.name.c_str());
    }

    caster->monster.AddEffect(repelMissiles, 3);

    // hack to show traj on map
    WorldBuilder & world = World;
    world.getScene().DisplayMap(true);
    StandardMonsterActions::ShowTrajectory(caster->level, caster->pos.x, caster->pos.y, caster->pos.x, caster->pos.y, '#', Random::getInt(2, 0) ? 255 : 128, 0, 255);

    return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------

int	CastMagic::Teleport(monsterData* caster)
{
    DungeonLevel* level = World.getDungeonManager().CurrentLevel();

    int new_x;
    int new_y;

    int range = 20;
    int failed;

    do
    {
        new_x = Random::getInt(range, 1);
        new_y = Random::getInt(range, 1);

        new_x = caster->pos.x + new_x - range / 2;
        new_y = caster->pos.y + new_y - range / 2;

        failed = 0; //santiy check
        if (new_x < 0 || new_x >= DUNGEON_SIZE_W) failed = 1;
        if (new_y < 0 || new_y >= DUNGEON_SIZE_H - 1) failed = 1;

    } while (failed || level->map[new_x][new_y].terrain.type == stone || level->map[new_x][new_y].monsterExists());

    // hack to show traj on map
    WorldBuilder & world = World;
    world.getScene().DisplayMap(true);
    StandardMonsterActions::ShowTrajectory(caster->level, caster->pos.x, caster->pos.y, new_x, new_y, '@', Random::getInt(2, 0) ? 255 : 128, 0, 255);

    level->map[caster->pos.x][caster->pos.y].RemoveMonsterRef();

    caster->NextAction(World.getActionManager().UpdateAction(&caster->action, aMove, new_x, new_y));

    World.getMonsterManager().Player()->UpdateSightRange();
    return 0;
}

// -------------------------------------------------------------------------------------------------------------------------------

int CastMagic::FlyingWeapon(monsterData* caster, int x, int y)
{
    Monster * d = World.getDungeonManager().level[World.GetCurrentLevel()].map[x][y].GetMonster();
    monsterData* defender = World.getMonsterManager().FindMonsterData(d);

    WorldBuilder & world = World;
    StandardMonsterActions::ShowTrajectory(caster->level, caster->pos.x, caster->pos.y, x, y, '*', Random::getInt(2,0) ? 255:128, 0, 255);

    if (defender == NULL || d == NULL) 
        return 0;

    int attackStrength  = caster->AttackStrength();
    int defenceStrength = defender->DefendStrength();

    attackStrength  = attackStrength  + (Random::getInt(7, 1) + Random::getInt(7, 1));
    defenceStrength = defenceStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));

    if (attackStrength > defenceStrength) //caster hits defender
    {
        if (caster->isPlayer()) //player is caster
        {
            if (defender->AbsorbTest())
            {
                World.getTextManager().newLine("The %s's armour deflected the magic missile. ", defender->monster.name.c_str());
            }
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) //repel missiles
                {
                    if (Random::getInt(5, 0) <= effect)
                    {
                        World.getTextManager().newLine("The %s repels your magic missile. ", defender->monster.name.c_str());
                        return 1;
                    }
                }

                World.getTextManager().newLine("Your magic missile hits the %s. ", defender->monster.name.c_str());
                defender->monster.stamina -= CalculateDamage(attackStrength, defenceStrength);
                if (defender->monster.stamina <= 0)
                {
                    caster->experience += defender->monster.experience;
                    caster->XP();
                }
            }
        }
        else if (defender->isPlayer())
        {
            if (defender->AbsorbTest())
            {
                World.getTextManager().newLine("The %s's magic missile was deflected by your armour. ", caster->monster.name.c_str());
            }
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) //repel missiles
                {
                    if (Random::getInt(5, 0) <= effect)
                    {
                        World.getTextManager().newLine("The repel the %s's magic missile. ", caster->monster.name.c_str());
                        return 1;
                    }
                }
                World.getTextManager().newLine("The %s's magic missile hits you. ", caster->monster.name.c_str());
                defender->monster.stamina -= CalculateDamage(attackStrength, defenceStrength);
                World.getDeathMessage().SetDeathMessage("was killed by a %s's magic missile. ", caster->monster.name.c_str());

            }
        }
    }

    else if (attackStrength < defenceStrength) //defender blocks caster
    {
        if (caster->isPlayer()) //player is caster
        {
            World.getTextManager().newLine("Your magic missile misses the %s. ", defender->monster.name.c_str());
        }

        else if (defender->isPlayer()) //player is defender
        {
            World.getTextManager().newLine("You dodge the %s's magic missile. ", caster->monster.name.c_str());
        }
    }
    else //no damage done
    {
        if (caster->isPlayer())
        {
            World.getTextManager().newLine("The %s dodges your magic missile. ", defender->monster.name.c_str());
        }
        if (defender->isPlayer())
        {
            //	if(defender->slots.weapon !=NULL)
            if (World.getMonsterManager().monsterItems.GetEquipment(defender, weapon))
            {
                World.getTextManager().newLine("The %s's magic missile misses you. ", caster->monster.name.c_str());
            }
            else
            {
                World.getTextManager().newLine("You dodge the %s's magic missile. ", caster->monster.name.c_str());
            }
        }
    }

    return 1;
}

// -------------------------------------------------------------------------------------------------------------------------------

int	CastMagic::DragonBreath(monsterData* caster, int x, int y)
{
    Monster *d = World.getDungeonManager().level[World.GetCurrentLevel()].map[x][y].GetMonster();
    monsterData* defender = World.getMonsterManager().FindMonsterData(d);

    if (defender == NULL || d == NULL) 
        return 0;

    int attackStrength = caster->AttackStrength();
    int defenceStrength = defender->DefendStrength();

    StandardMonsterActions::ShowTrajectory(caster->level, caster->pos.x, caster->pos.y, x, y, '#', (Random::getInt(2,0) == 0 ) ? 255:128, 0, 0);

    if (1)
    {
        int damage = 2;

        damage = World.getMonsterManager().CalculateBrandDamageOnMonster(defender, bFire, damage);

        defender->monster.stamina -= damage;

        if (damage == 1)
        {
            if (defender->isPlayer())
                World.getTextManager().newLine("You partially resist. ");
            else
                World.getTextManager().newLine("The %s partially resists. ", defender->monster.name.c_str());
        }
        else if (damage == 0)
        {
            if (defender->isPlayer())
                World.getTextManager().newLine("You resist. ");
            else
                World.getTextManager().newLine("The %s resists. ", defender->monster.name.c_str());

        }
        else
        {
            if (defender->isPlayer())
                World.getTextManager().newLine("You burn. ");
            else
                World.getTextManager().newLine("The %s burns. ", defender->monster.name.c_str());
        }
    }
    else //no damage done
    {
        if (caster->isPlayer())
        {
            World.getTextManager().newLine("The %s dodges. ", defender->monster.name.c_str());
        }
        if (defender->isPlayer())
        {
            World.getTextManager().newLine("You dodge. ", caster->monster.name.c_str());
        }
    }
    return 1;
}

// -------------------------------------------------------------------------------------------------------------------------------

int CastMagic::CalculateDamage(int attackStr, int defenceStr)
{
    int diff = attackStr - defenceStr;

    if (diff <= 0)
        return 0;

    int damage = 2;
    
    int crital_hit = Random::getInt(200, 0);

    if (crital_hit > 195)
    {
        World.getTextManager().newLine("It was a exellent hit! ");
        damage = +2;
    }
    else if (crital_hit > 190)
    {
        World.getTextManager().newLine("It was a good hit! ");;
        damage++;
    }

    return damage;
}

// -------------------------------------------------------------------------------------------------------------------------------
