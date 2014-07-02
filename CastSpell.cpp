#include "WorldBuilder.h"
#include ".\CastSpell.h"

CastMagic::CastMagic(void)
{
}

CastMagic::~CastMagic(void)
{
}


int	CastMagic::SlowEnemies(monsterData* caster)
{
    if (caster->isPlayer())
    {
        MONSTERLIST::iterator it;
        //get list of enemies on LOS.
        for (it = WorldBuilder::monsterManager.monster_list.begin(); it != WorldBuilder::monsterManager.monster_list.end(); it++)
        {
            if (it->isSeen() == 1 && !it->isPlayer())
            {
                int resistance = it->monster.GetResistance(bSlow);
                if (Random::getInt(10, 0) + resistance < 5)
                {
                    it->monster.AddEffect(slowed, 3);
                    WorldBuilder::textManager.newLine("The %s is slowed. ", it->monster.name.c_str());
                }
                else
                    WorldBuilder::textManager.newLine("The %s resists. ", it->monster.name.c_str());

            }
        }
    }
    else //cast on player
    {
        monsterData* player = WorldBuilder::monsterManager.Player();
        int resistance = player->monster.GetResistance(bSlow);
        if (Random::getInt(10, 0) + resistance < 5)
        {
            player->monster.AddEffect(slowed, 3);
            WorldBuilder::textManager.newLine("You are slowed. ");
        }
        else
            WorldBuilder::textManager.newLine("You resist. ");
    }
    return 0;
}
int	CastMagic::RepelMissiles(monsterData* caster) //glow blue?
{

    if (caster->isPlayer())
    {
        if (caster->monster.GetEffect(repelMissiles)) //already done
            WorldBuilder::textManager.newLine("Your glow increases. ");
        else
            WorldBuilder::textManager.newLine("You start glowing. ");
    }
    else
    {
        if (caster->monster.GetEffect(repelMissiles)) //already done
            WorldBuilder::textManager.newLine("The %s glows brighter. ", caster->monster.name.c_str());
        else
            WorldBuilder::textManager.newLine("The %s starts glowing. ", caster->monster.name.c_str());
    }

    caster->monster.AddEffect(repelMissiles, 3);

    return 0;
}


int	CastMagic::Teleport(monsterData* caster)
{
    DungeonLevel* level = WorldBuilder::dungeonManager.CurrentLevel();

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

    caster->NextAction(WorldBuilder::actionManager.UpdateAction(&caster->action, aMove, new_x, new_y));

    WorldBuilder::monsterManager.Player()->UpdateSightRange();
    return 0;
}
int	CastMagic::FlyingWeapon(monsterData* caster, int x, int y)
{

    Monster *d = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].GetMonster();
    monsterData* defender = WorldBuilder::monsterManager.FindMonsterData(d);

    if (defender == NULL || d == NULL) return 0;

    int attackStrength = caster->AttackStrength();
    int defenceStrength = defender->DefendStrength();

    attackStrength = attackStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));
    defenceStrength = defenceStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));


    if (attackStrength > defenceStrength) //caster hits defender
    {
        if (caster->isPlayer()) //player is caster
        {
            if (defender->AbsorbTest())
            {
                WorldBuilder::textManager.newLine("The %s's armour deflected the magic missile. ", defender->monster.name.c_str());
            }
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) //repel missiles
                {
                    if (Random::getInt(5, 0) <= effect)
                    {
                        WorldBuilder::textManager.newLine("The %s repels your magic missile. ", defender->monster.name.c_str());
                        return 1;
                    }
                }

                WorldBuilder::textManager.newLine("Your magic missile hits the %s. ", defender->monster.name.c_str());
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
                WorldBuilder::textManager.newLine("The %s's magic missile was deflected by your armour. ", caster->monster.name.c_str());
            }
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) //repel missiles
                {
                    if (Random::getInt(5, 0) <= effect)
                    {
                        WorldBuilder::textManager.newLine("The repel the %s's magic missile. ", caster->monster.name.c_str());
                        return 1;
                    }
                }
                WorldBuilder::textManager.newLine("The %s's magic missile hits you. ", caster->monster.name.c_str());
                defender->monster.stamina -= CalculateDamage(attackStrength, defenceStrength);
                WorldBuilder::deathMessage.SetDeathMessage("was killed by a %s's magic missile. ", caster->monster.name.c_str());

            }
        }
    }

    else if (attackStrength < defenceStrength) //defender blocks caster
    {
        if (caster->isPlayer()) //player is caster
        {
            WorldBuilder::textManager.newLine("Your magic missile misses the %s. ", defender->monster.name.c_str());
        }

        else if (defender->isPlayer()) //player is defender
        {
            WorldBuilder::textManager.newLine("You dodge the %s's magic missile. ", caster->monster.name.c_str());
        }
    }
    else //no damage done
    {
        if (caster->isPlayer())
        {
            WorldBuilder::textManager.newLine("The %s dodges your magic missile. ", defender->monster.name.c_str());
        }
        if (defender->isPlayer())
        {
            //	if(defender->slots.weapon !=NULL)
            if (WorldBuilder::monsterManager.monsterItems.GetEquipment(defender, weapon))
            {
                WorldBuilder::textManager.newLine("The %s's magic missile misses you. ", caster->monster.name.c_str());
            }
            else
            {
                WorldBuilder::textManager.newLine("You dodge the %s's magic missile. ", caster->monster.name.c_str());
            }
        }
    }

    return 1;
}

int	CastMagic::DragonBreath(monsterData* caster, int x, int y)
{
    Monster *d = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].GetMonster();
    monsterData* defender = WorldBuilder::monsterManager.FindMonsterData(d);

    if (defender == NULL || d == NULL) return 0;

    int attackStrength = caster->AttackStrength();
    int defenceStrength = defender->DefendStrength();

    if (1)
    {
        int damage = 2;

        damage = WorldBuilder::monsterManager.CalculateBrandDamageOnMonster(defender, bFire, damage);

        defender->monster.stamina -= damage;

        if (damage == 1)
        {
            if (defender->isPlayer())
                WorldBuilder::textManager.newLine("You partially resist. ");
            else
                WorldBuilder::textManager.newLine("The %s partially resists. ", defender->monster.name.c_str());
        }
        else if (damage == 0)
        {
            if (defender->isPlayer())
                WorldBuilder::textManager.newLine("You resist. ");
            else
                WorldBuilder::textManager.newLine("The %s resists. ", defender->monster.name.c_str());

        }
        else
        {
            if (defender->isPlayer())
                WorldBuilder::textManager.newLine("You burn. ");
            else
                WorldBuilder::textManager.newLine("The %s burns. ", defender->monster.name.c_str());
        }
    }


    else //no damage done
    {
        if (caster->isPlayer())
        {
            WorldBuilder::textManager.newLine("The %s dodges. ", defender->monster.name.c_str());
        }
        if (defender->isPlayer())
        {
            WorldBuilder::textManager.newLine("You dodge. ", caster->monster.name.c_str());
        }
    }
    return 1;
}

int CastMagic::CalculateDamage(int attackStr, int defenceStr)
{
    int diff = attackStr - defenceStr;

    if (diff <= 0)
        return 0;

    int ret = diff / 4 + 1;
    int crital_hit = Random::getInt(200, 0);

    if (crital_hit == 199)
    {
        WorldBuilder::textManager.newLine("It was a exellent hit! ");
        ret *= 4;
    }
    else if (crital_hit > 195)
    {
        WorldBuilder::textManager.newLine("It was a exellent hit! ");
        ret *= 3;
    }
    else if (crital_hit > 190)
    {
        WorldBuilder::textManager.newLine("It was a good hit! ");;
        ret *= 2;
    }

    return ret;
}