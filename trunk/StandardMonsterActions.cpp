// StandardMonsterActions.cpp: implementation of the StandardMonsterActions class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "StandardMonsterActions.h"
#include <math.h>
#include "NumberGenerator.h"
#include ".\effectmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


int StandardMonsterActions::ThrowTarget(monsterData* attacker, int targetX, int targetY, eAction action)
{
    int ret;
    COORDLIST::iterator it;
    DungeonLevel* level = WorldBuilder::dungeonManager.CurrentLevel();

    level->HighLightPath(attacker->pos.x, attacker->pos.y, targetX, targetY);

    for (it = level->show_path.begin(); it != level->show_path.end(); it++) //something in the way!!
    {
        if (!level->IsCellTransparent(it->x, it->y) || level->map[it->x][it->y].monsterExists())
        {
            int ret = attacker->NextAction(WorldBuilder::actionManager.UpdateAction(&attacker->action, action, it->x, it->y));
            WorldBuilder::dungeonManager.CurrentLevel()->ClearPath();
            return ret;
        }
    }

    //path clear - attack target
    ret = attacker->NextAction(WorldBuilder::actionManager.UpdateAction(&attacker->action, action, targetX, targetY));

    WorldBuilder::dungeonManager.CurrentLevel()->ClearPath();

    return ret;
}

int	StandardMonsterActions::FireItem(monsterData* attacker, int x, int y)
{
    Item* projectilePile = WorldBuilder::monsterManager.monsterItems.GetEquipment(attacker, projectile);
    Item* projectileLauncher = WorldBuilder::monsterManager.monsterItems.GetEquipment(attacker, projectileWeapon);

    if (attacker->pos.x == x && attacker->pos.y == y)
    {
        if (attacker->isPlayer())
            WorldBuilder::textManager.newLine("Can not shoot yourself on purpose. ");
        return 0;
    }

    if (!projectilePile) //no projectiles
    {
        if (attacker->isPlayer())
            WorldBuilder::textManager.newLine("You have no projectiles equipped. ");
        attacker->is_archer = 0;
        return 0;
    }
    if (!projectileLauncher)  //no projectile weapon
    {
        if (projectilePile->itemNumber[0] != 0)
        {
            if (attacker->isPlayer())
                WorldBuilder::textManager.newLine("You do not have the correct weapon equipped. ");
            return 0;
        }
    }
    else if (projectileLauncher) //projectile weapon
    {
        if (projectilePile->itemNumber[0] != projectileLauncher->itemNumber[0])  //wrong projectiles
        {
            //if(attacker->isPlayer())
            WorldBuilder::textManager.newLine("You do not have the correct weapon equipped. ");
            return 0;
        }
    }

    Item *actualProjectile = new Item(*projectilePile);
    actualProjectile->itemNumber[1] = 1; //make only one

    //remove projectile from inventory and add to ground
    projectilePile->itemNumber[1]--;
    if (projectilePile->itemNumber[1] == 0) //delete from inventory
    {
        ITEMLIST::iterator it;
        ITEMLIST *inventory = &attacker->inventory;

        for (it = inventory->begin(); it != inventory->end(); it++)
        {
            if (it->type == projectile && it->equipped)
            {
                inventory->erase(it);
                break;
            }
        }
    }
    actualProjectile->equipped = 0;

    Monster *d = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].GetMonster();
    monsterData* defender = WorldBuilder::monsterManager.FindMonsterData(d);

    std::string throw_name = actualProjectile->name;

    if (defender == NULL || d == NULL) //no target but terrain
    {
        if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].terrain.type == deepWater ||
            WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].terrain.type == shallowWater)
            WorldBuilder::textManager.newLine("Splash! "); //missile are lost
        else
        {
            WorldBuilder::textManager.newLine("Thud! ");
            if (Random::getInt(10, 0) < 5) //50% breakage
                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, actualProjectile, x, y);
        }
        return 1;
    }

    //add to ground
    if (Random::getInt(10, 0) < 7) //20% breakage
        WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, actualProjectile, x, y);

    //calculate attack
    int dist = (int)sqrt((float)(attacker->pos.x - x)*(attacker->pos.x - x) + (attacker->pos.y - y) *(attacker->pos.y - y));

    int attackStrength;//= thrown_p->GetAttack_thr() + attacker->Skill()-dist/2; //+attack weapon I.e Bow

    if (projectileLauncher) //add projectile and launcher
        attackStrength = (int)((projectileLauncher->GetAttack_thr() + projectileLauncher->GetAttack_thr()) + actualProjectile->skill_bonus + projectileLauncher->skill_bonus + attacker->Skill()) - dist / 2;
    else //no launcher
        attackStrength = 3 + attacker->Skill() - dist / 2;

    int defenceStrength = defender->monster.skill + dist;

    attackStrength  = attackStrength  + (Random::getInt(7, 1) + Random::getInt(7, 1));
    defenceStrength = defenceStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));

    if (dist == 1)
        attackStrength = attackStrength / 2;

    //if(slots.weapon != NULL)
    Item* a = WorldBuilder::monsterManager.monsterItems.GetEquipment(defender, armour);
    if (a)
        defenceStrength += a->absorb_bonus;

    //WorldBuilder::textManager.newLine("A:%d D:%d Dam:%d ",attackStrength,defenceStrength, CalculateDamage(attackStrength,defenceStrength));

    if (attackStrength > defenceStrength) //attacker hits defender
    {
        if (attacker->isPlayer()) //player is attacker
        {
            if (defender->AbsorbTest())
            {
                WorldBuilder::textManager.newLine("The %s's armour deflected the %s. ", defender->monster.name.c_str(), throw_name.c_str());
            }
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) //repel missiles
                {
                    if (Random::getInt(4, 0) <= effect)
                    {
                        WorldBuilder::textManager.newLine("The %s is repelled. ", throw_name.c_str(), defender->monster.name.c_str());
                        return 1;
                    }
                }
                WorldBuilder::textManager.newLine("The %s hits the %s. ", throw_name.c_str(), defender->monster.name.c_str());
                defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);
                defender->monster.stamina -= AddBrandDamage(attacker, defender, actualProjectile);

                if (defender->monster.stamina <= 0)
                {
                    attacker->experience += defender->monster.experience;
                    attacker->XP();
                }
            }
        }
        else if (defender->isPlayer())
        {
            if (defender->AbsorbTest())
                WorldBuilder::textManager.newLine("The %s's %s clipped your armour. ", attacker->monster.name.c_str(), throw_name.c_str());
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) //always repel missiles?
                {
                    WorldBuilder::textManager.newLine("You repel the %s. ", throw_name.c_str());
                    return 1;
                }
                WorldBuilder::textManager.newLine("The %s's %s hits you. ", attacker->monster.name.c_str(), throw_name.c_str());
                defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);
                defender->monster.stamina -= AddBrandDamage(attacker, defender, actualProjectile);

                if (defender->monster.stamina < 1) //dead
                {
                    WorldBuilder::deathMessage.SetDeathMessage("was killed by a %s's %s. ", attacker->monster.name.c_str(), throw_name.c_str());
                }
            }
        }
    }
    else if (attackStrength < defenceStrength) //defender blocks attacker
    {
        if (attacker->isPlayer()) //player is attacker
            WorldBuilder::textManager.newLine("You miss the %s. ", defender->monster.name.c_str());


        else if (defender->isPlayer()) //player is defender
            WorldBuilder::textManager.newLine("The %s's %s misses you. ", attacker->monster.name.c_str(), throw_name.c_str());

    }
    else //no damage done
    {
        if (attacker->isPlayer())
        {
            if (WorldBuilder::monsterManager.monsterItems.GetEquipment(defender, weapon) && defender->isHumanoid())
                WorldBuilder::textManager.newLine("The %s deflects your attack. ", defender->monster.name.c_str());
            else
                WorldBuilder::textManager.newLine("The %s dodges your attack. ", defender->monster.name.c_str());
        }
        if (defender->isPlayer())
        {
            if (WorldBuilder::monsterManager.monsterItems.GetEquipment(defender, weapon))
                WorldBuilder::textManager.newLine("You deflect the %s's %s. ", attacker->monster.name.c_str(), throw_name.c_str());
            else
                WorldBuilder::textManager.newLine("You dodge the %s's %s. ", attacker->monster.name.c_str(), throw_name.c_str());
        }
    }

    //WorldBuilder::textManager.newLine(text_output);
    return 1;
}

int	StandardMonsterActions::MoveMonster(monsterData* monster, int new_x, int new_y)
{
    if (monster->monster.GetEffect(paralysis))
        return 0;

    if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_x][new_y].terrain.type == stone)
        return 0;

    if (WorldBuilder::GetCurrentLevel() == 0 && (new_y == 0 || new_x == 0 || new_x == DUNGEON_SIZE_W - 1 || new_y == DUNGEON_SIZE_H - 1)
        && monster->isPlayer()) //exit world
    {
        if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_x][new_y].terrain.type != deepWater)
        {
            WorldBuilder::textManager.newLine("Do you want to leave? (y)");
            WorldBuilder::SetState(sLeave);
            return 0;
        }
    }

    if (new_x >= DUNGEON_SIZE_W - 1 || new_y >= DUNGEON_SIZE_H - 1 || new_x <= 0 || new_y <= 0)
        return 0;

    //check if a monster is in new spot(attack or stop)
    if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_x][new_y].GetMonster() != NULL)
    {
        if (monster->fleeing)
        {
            WorldBuilder::textManager.newLine("You cannot attack while fleeing. ");
            return 0;
        }
        Action* new_action = WorldBuilder::actionManager.UpdateAction(&monster->action, aAttack, new_x, new_y);
        monster->NextAction(new_action);
        return 2;
    }

    //open door
    if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_x][new_y].terrain.type == closedDoor)   //open door
    {
        WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_x][new_y].terrain.Create(openDoor);     //makeopenDoor();
        if (monster->isPlayer())
            WorldBuilder::textManager.newLine("You open the door. ");
        else if (monster->isSeen() == 1)
        {
            WorldBuilder::textManager.newLine("The %s opens the door. ", monster->monster.name.c_str());
        }
        else if (monster->isSeen() == 2)
            WorldBuilder::textManager.newLine("A door squeaks. ");
        return 3;
    }

    if (strcmp(WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_x][new_y].terrain.name, "a teleport") == 0)
    {
        coord * ner_coord = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].FreeTerrainPosition(dfloor);
        if (ner_coord)
        {
            new_x = ner_coord->x;
            new_y = ner_coord->y;
            if (monster->isPlayer())
                WorldBuilder::textManager.newLine("You are surrounded by colourful swirls of magic. ");
            else if (monster->isSeen() == 1)
            {
                WorldBuilder::textManager.newLine("The %s is engulfed by colourful swirls of magic.", monster->monster.name.c_str());
            }
            else if (monster->isSeen() == 2)
                WorldBuilder::textManager.newLine("You hear a distant whumph! ");

            monster->monster.AddEffect(confused, 1);
        }
    }

    //remove old ref on successful move
    WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[monster->pos.x][monster->pos.y].RemoveMonsterRef();

    //update monster position
    monster->pos.x = new_x;
    monster->pos.y = new_y;

    WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[monster->pos.x][monster->pos.y].RemoveMonsterRef();

    WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_x][new_y].AssignMonster(&monster->monster);

    return 1;
}

int	StandardMonsterActions::AttackMonster(monsterData* attacker, int x, int y)
{
    Monster *d = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].GetMonster();
    monsterData* defender = WorldBuilder::monsterManager.FindMonsterData(d);

    if (defender == NULL || d == NULL) return 0;

    int attackStrength = attacker->AttackStrength();
    int defenceStrength = defender->DefendStrength();

    attackStrength  = attackStrength  + (Random::getInt(7, 1) + Random::getInt(7, 1));
    defenceStrength = defenceStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));

    //WorldBuilder::textManager.newLine("A:%d D:%d Dam:%d ",attackStrength,defenceStrength, CalculateDamage(attackStrength,defenceStrength));

    // attack hits
    if (attackStrength > defenceStrength) //attacker hits defender
    {
        if (attacker->isPlayer()) //player is attacker
        {
            if (defender->AbsorbTest())
            {
                WorldBuilder::textManager.newLine("The %s's armour deflected the blow. ", defender->monster.name.c_str());
            }
            else
            {
                WorldBuilder::textManager.newLine("You hit the %s. ", defender->monster.name.c_str());
                defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);
                defender->monster.stamina -= AddBrandDamage(attacker, defender, WorldBuilder::monsterManager.monsterItems.GetEquipment(attacker, weapon));

                if (defender->monster.stamina <= 0)
                {
                    attacker->experience += defender->monster.experience;
                    attacker->XP();
                }
                else // special events on attack
                {
                    if (defender->Name() == "Ferryman" && Random::getInt(5, 1) == 3)
                    {
                        defender->monster.createBridgeMaster(true);
                        WorldBuilder::textManager.newLine("The Ferryman transforms! ");
                    }
                    if (defender->Name() == "Boathouse Keeper" && Random::getInt(5, 1) == 3)
                    {
                        defender->monster.createWerewolf();
                        WorldBuilder::textManager.newLine("The Boathouse Keeper transforms! ");
                    }
                }
            }
        }
        else if (defender->isPlayer())
        {
            if (defender->AbsorbTest())
            {
                WorldBuilder::textManager.newLine("The %s's attack was deflected by your armour. ", attacker->monster.name.c_str());
            }
            else
            {
                WorldBuilder::textManager.newLine("The %s hits you. ", attacker->monster.name.c_str());
                defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);
                defender->monster.stamina -= AddBrandDamage(attacker, defender, WorldBuilder::monsterManager.monsterItems.GetEquipment(attacker, weapon));
                WorldBuilder::deathMessage.SetDeathMessage("was killed by a %s ", attacker->monster.name.c_str());
            }
        }
    }
    // attack misses
    else if (attackStrength < defenceStrength) //defender blocks attacker
    {
        if (attacker->isPlayer()) //player is attacker
        {
            WorldBuilder::textManager.newLine("You miss the %s. ", defender->monster.name.c_str());
        }

        else if (defender->isPlayer()) //player is defender
        {
            WorldBuilder::textManager.newLine("You block the %s's attack. ", attacker->monster.name.c_str());
        }
    }
    else //no damage done
    {
        if (attacker->isPlayer())
        {
            if (WorldBuilder::monsterManager.monsterItems.GetEquipment(defender, weapon) && defender->isHumanoid())
            {
                WorldBuilder::textManager.newLine("The %s parries your attack. ", defender->monster.name.c_str());
            }
            else
            {
                WorldBuilder::textManager.newLine("The %s dodges your attack. ", defender->monster.name.c_str());
            }
        }
        if (defender->isPlayer())
        {
            if (WorldBuilder::monsterManager.monsterItems.GetEquipment(defender, weapon))
            {
                WorldBuilder::textManager.newLine("You parry the %s's attack. ", attacker->monster.name.c_str());
            }
            else
            {
                WorldBuilder::textManager.newLine("You dodge the %s's attack. ", attacker->monster.name.c_str());
            }
        }
    }

    //sprintf(text_output, "%s as %d. %s ds %d. ",monster.name.c_str(),attackStrength,defender->name,defenceStrength);

    return 1;
}


int	StandardMonsterActions::CalculateDamage(monsterData* defender, int attackStr, int defenceStr)
{
    int diff = attackStr - defenceStr;

    if (diff <= 0)
        return 0;

    Item* has_shield = WorldBuilder::monsterManager.monsterItems.GetEquipment(defender, shield);

    if (has_shield)
    {
        if (Random::getInt(6, 0) < has_shield->absorb_bonus) //block some damage
        {
            if (defender->isPlayer())
                WorldBuilder::textManager.newLine("Your shield partially blocks the attack. ");
            else
                WorldBuilder::textManager.newLine("The %s's shield partially blocks your attack. ", defender->Name().c_str());

            return 1;
        }
        // check luck 50% of time
        if (Random::getInt(2, 0) == 1)
        {
            if (defender->isPlayer() && defender->Stamina() < 4 && (Random::getInt(7, 1) + Random::getInt(7, 1)) < defender->Luck())
            {
                WorldBuilder::textManager.newLine("Your shield *luckily* partially blocks the attack. ");
                return 1;
            }
        }
    }
    return 2;
}

int StandardMonsterActions::AddBrandDamage(monsterData* attacker, monsterData* defender, Item* attack_weapon)
{
    int extra_damage = 0;
    int taken_effect = 0;

    if (attack_weapon) //weapon attack
    {
        if (attack_weapon->hasBrand())
        {
            BRANDMAP_CITERATOR it;
            BRANDLIST brandList;

            for (it = attack_weapon->brandMap.begin(); it != attack_weapon->brandMap.end(); it++)
            {
                int damage = WorldBuilder::itemManager.CalculateBrandDamage(attack_weapon, it->first, it->second);
                extra_damage += WorldBuilder::monsterManager.CalculateBrandDamageOnMonster(defender, it->first, damage);
                extra_damage = int(extra_damage / 4 + .5f);

                EffectManager effectManager; //add effect
                if (effectManager.TestGetEffect(defender, it->first, it->second))
                {
                    effectManager.AddBrandEffect(defender, it->first, it->second);
                    taken_effect = 1;
                }
                if (extra_damage || taken_effect)
                {
                    if (defender->isPlayer())
                        WorldBuilder::textManager.newLine("You get %s. ", attack_weapon->GetBrandName(it->first));
                    else
                        WorldBuilder::textManager.newLine("The %s gets %s. ", defender->monster.name.c_str(), attack_weapon->GetBrandName(it->first));
                }
            }
        }
    }
    taken_effect = 0;

    if (attacker->monster.hasBrand()) //monster has damage type. I.e poison hands
    {
        BRANDMAP_CITERATOR it;
        BRANDLIST brandList;

        for (it = attacker->monster.brandMap.begin(); it != attacker->monster.brandMap.end(); it++)
        {
            int damage = WorldBuilder::itemManager.CalculateBrandDamage(NULL, it->first, it->second);
            extra_damage += WorldBuilder::monsterManager.CalculateBrandDamageOnMonster(defender, it->first, damage);
            extra_damage = int(extra_damage / 4 + .5f);

            EffectManager effectManager; //add effect
            if (effectManager.TestGetEffect(defender, it->first, it->second))
            {
                effectManager.AddBrandEffect(defender, it->first, it->second);
                taken_effect = 1;
            }

            if (extra_damage || taken_effect)
            {
                if (defender->isPlayer())
                    WorldBuilder::textManager.newLine("You get %s. ", attacker->monster.GetBrandName(it->first));
                else
                    WorldBuilder::textManager.newLine("The %s gets %s. ", defender->monster.name.c_str(), attacker->monster.GetBrandName(it->first));
            }
        }
    }

    //WorldBuilder::textManager.newLine("Ext:%d ",extra_damage);
    if (extra_damage > 0)
        return 1;
    else return 0;

}

int	StandardMonsterActions::CastSpell(monsterData*caster, int spellID)
{
    return WorldBuilder::spellManager.CallSpellRoutine(caster, spellID);
}

int	StandardMonsterActions::CastTargetSpell(monsterData* caster, eSpellList spell, int TargetX, int TargetY)
{
    return WorldBuilder::spellManager.CastCurrentSpell(caster, TargetX, TargetY);
    //return caster->NextAction(WorldBuilder::actionManager.UpdateAction(&caster->action,aCastSpell,TargetX,TargetY));
}