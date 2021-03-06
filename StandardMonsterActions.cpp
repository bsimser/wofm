// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#include "WorldBuilder.h"
#include "StandardMonsterActions.h"
#include <math.h>
#include "NumberGenerator.h"
#include ".\effectmanager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int StandardMonsterActions::ThrowTarget(MonsterData* attacker, int targetX, int targetY, eAction action)
{
    int ret;
    COORDLIST::iterator it;
    DungeonLevel* level = World.getDungeonManager().CurrentLevel();

    int finalX = targetX;
    int finalY = targetY;

    level->HighLightPath(attacker->pos.x, attacker->pos.y, targetX, targetY);

    for (it = level->show_path.begin(); it != level->show_path.end(); it++) //something in the way!!
    {
        if (!level->IsCellTransparent(it->x, it->y) || level->getCell(it->x, it->y).monsterExists())
        {
            finalX = it->x;
            finalY = it->y;
            break;
        }
    }

    //path clear - attack target
    ret = attacker->NextAction(World.getActionManager().UpdateAction(&attacker->action, action, finalX, finalY));

    World.getDungeonManager().CurrentLevel()->ClearPath();
    return ret;
}

void StandardMonsterActions::ShowTrajectory(int level, int sourceX, int sourceY, int targetX, int targetY, char symbol, int c1, int c2, int c3)
{
    Symbol vSymbol(symbol, c1, c2, c3);

    // get and copy path
    DungeonLevel & dungeonLevel = World.getDungeonManager().level(level);
    dungeonLevel.HighLightPath(sourceX, sourceY, targetX, targetY);
    COORDLIST path = dungeonLevel.show_path;
    World.getDungeonManager().CurrentLevel()->ClearPath();

    COORDLIST::iterator it;
    for (it = path.begin(); it != path.end(); it++)
    {
        dungeonLevel.getCell(it->x, it->y).setSymbol(vSymbol);
        World.RenderScene();
        Sleep(32);
        dungeonLevel.getCell(it->x, it->y).clearSymbol();
    }
}

int	StandardMonsterActions::FireItem(MonsterData* attacker, int x, int y)
{
    Item* projectilePile = World.getMonsterManager().getMonsterItems().GetEquipment(attacker, projectile);
    Item* projectileLauncher = World.getMonsterManager().getMonsterItems().GetEquipment(attacker, projectileWeapon);

    if (attacker->pos.x == x && attacker->pos.y == y)
    {
        if (attacker->isPlayer())
            World.getTextManager().newLine("Can not shoot yourself on purpose. ");
        return 0;
    }

    if (!projectilePile) //no projectiles
    {
        if (attacker->isPlayer())
            World.getTextManager().newLine("You have no projectiles equipped. ");
        attacker->is_archer = 0;
        return 0;
    }
    if (!projectileLauncher)  //no projectile weapon
    {
        if (projectilePile->itemNumber[0] != 0)
        {
            if (attacker->isPlayer())
                World.getTextManager().newLine("You do not have the correct weapon equipped. ");
            return 0;
        }
    }
    else if (projectileLauncher) //projectile weapon
    {
        if (projectilePile->itemNumber[0] != projectileLauncher->itemNumber[0])  //wrong projectiles
        {
            //if(attacker->isPlayer())
            World.getTextManager().newLine("You do not have the correct weapon equipped. ");
            return 0;
        }
    }

    Item *actualProjectile = World.getItemManager().DuplicateItem(projectilePile);
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

    // show trajectory
    Symbol symbol('*', 255, 0, 0);
    ShowTrajectory(attacker->level, attacker->pos.x, attacker->pos.y, x, y, actualProjectile->symbol, actualProjectile->color1, actualProjectile->color2, actualProjectile->color3);

    actualProjectile->equipped = 0;

    Monster *d = World.getDungeonManager().level(World.GetCurrentLevel()).getCell(x, y).GetMonster();
    MonsterData* defender = World.getMonsterManager().FindMonsterData(d);

    std::string throw_name = actualProjectile->BaseName();

    if (defender == NULL || d == NULL) //no target but terrain
    {
        if (World.getDungeonManager().level(World.GetCurrentLevel()).getCell(x, y).terrain.type == deepWater ||
            World.getDungeonManager().level(World.GetCurrentLevel()).getCell(x, y).terrain.type == shallowWater)
            World.getTextManager().newLine("Splash! "); //missile are lost
        else
        {
            World.getTextManager().newLine("Thud! ");
            if (Random::getInt(10, 0) < 5) //50% breakage
                World.getMonsterManager().getMonsterItems().AttemptDropItem(NULL, actualProjectile, x, y);
        }
        return 1;
    }

    //add to ground
    if (Random::getInt(10, 0) < 7) //20% breakage
    {
        World.getMonsterManager().getMonsterItems().AttemptDropItem(NULL, actualProjectile, x, y);
    }

    //calculate attack
    int dist = (int)sqrt((float)(attacker->pos.x - x)*(attacker->pos.x - x) + (attacker->pos.y - y) *(attacker->pos.y - y));

    int attackStrength;//= thrown_p->GetAttack_thr() + attacker->Skill()-dist/2; //+attack weapon I.e Bow

    if (projectileLauncher) //add projectile and launcher
        attackStrength = (int)((projectileLauncher->GetAttack_thr() + projectileLauncher->GetAttack_thr()) + actualProjectile->skill_bonus + projectileLauncher->skill_bonus + attacker->Skill()) - dist / 2;
    else //no launcher
        attackStrength = 3 + attacker->Skill() - dist / 2;

    int defenceStrength = defender->monster.skill + dist;

    attackStrength = attackStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));
    defenceStrength = defenceStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));

    if (dist == 1)
        attackStrength = attackStrength - 5;

    bool silver = ((defender->monster.GetType() == mUndead || defender->Name() == "wight" || defender->Name().find("vampire") != -1 || defender->Name().find("pirate") != -1) && actualProjectile->BaseName() == "silver arrow") ? true : false;
    if (silver)
        attackStrength += 5;

    //if(slots.weapon != NULL)
    Item* a = World.getMonsterManager().getMonsterItems().GetEquipment(defender, armour);
    if (a)
        defenceStrength += a->absorb_bonus;

    //World.getTextManager().newLine("A:%d D:%d Dam:%d ",attackStrength, defenceStrength, CalculateDamage(attackStrength,defenceStrength));

    if (attackStrength > defenceStrength) //attacker hits defender
    {
        if (!defender->isPlayer()) //player is attacker
        {

            if (!silver && defender->AbsorbTest())
            {
                if (defender->isSeen() == 1)
                    World.getTextManager().newLine("The %s's armour deflected the %s. ", defender->monster.name.c_str(), throw_name.c_str());
            }
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) // repel missiles
                {
                    if (Random::getInt(4, 0) <= effect)
                    {
                        if (defender->isSeen() == 1)
                            World.getTextManager().newLine("The %s is repelled. ", throw_name.c_str(), defender->monster.name.c_str());
                        return 1;
                    }
                }
                if (defender->isSeen() == 1)
                    World.getTextManager().newLine("The %s hits the %s. ", throw_name.c_str(), defender->monster.name.c_str());
                else
                    World.getTextManager().newLine("The hear a painful grunt. ");

                if (defender->Name() == "wight" && !silver)
                {
                    if (defender->isSeen() == 1)
                        World.getTextManager().newLine("It does no damage. ", throw_name.c_str(), defender->monster.name.c_str());
                }
                else
                    defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);

                defender->monster.stamina -= AddBrandDamage(attacker, defender, actualProjectile);

                if (silver)
                {
                    defender->monster.stamina = -8;
                    if (defender->isSeen() == 1)
                        World.getTextManager().newLine("It sinks in deep! ");
                }
                if (defender->monster.stamina <= 0)
                {
                    attacker->experience += defender->monster.experience;
                    attacker->XP();
                }
            }
        }
        else
        {
            if (defender->AbsorbTest())
                World.getTextManager().newLine("The %s's %s clipped your armour. ", attacker->monster.name.c_str(), throw_name.c_str());
            else
            {
                if (int effect = defender->monster.GetEffect(repelMissiles) > 0) //always repel missiles?
                {
                    World.getTextManager().newLine("You repel the %s. ", throw_name.c_str());
                    return 1;
                }
                World.getTextManager().newLine("The %s's %s hits you. ", attacker->monster.name.c_str(), throw_name.c_str());
                defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);
                defender->monster.stamina -= AddBrandDamage(attacker, defender, actualProjectile);

                if (defender->monster.stamina < 1) //dead
                {
                    World.getDeathMessage().SetDeathMessage("was killed by a %s's %s. ", attacker->monster.name.c_str(), throw_name.c_str());
                }
            }
        }
    }
    else if (attackStrength < defenceStrength) //defender blocks attacker
    {
        if (attacker->isPlayer()) //player is attacker
            World.getTextManager().newLine("You miss the %s. ", defender->monster.name.c_str());


        else if (defender->isPlayer()) //player is defender
            World.getTextManager().newLine("The %s's %s misses you. ", attacker->monster.name.c_str(), throw_name.c_str());

    }
    else //no damage done
    {
        if (attacker->isPlayer())
        {
            if (World.getMonsterManager().getMonsterItems().GetEquipment(defender, weapon) && defender->isHumanoid())
                World.getTextManager().newLine("The %s deflects your attack. ", defender->monster.name.c_str());
            else
                World.getTextManager().newLine("The %s dodges your attack. ", defender->monster.name.c_str());
        }
        if (defender->isPlayer())
        {
            if (World.getMonsterManager().getMonsterItems().GetEquipment(defender, weapon))
                World.getTextManager().newLine("You deflect the %s's %s. ", attacker->monster.name.c_str(), throw_name.c_str());
            else
                World.getTextManager().newLine("You dodge the %s's %s. ", attacker->monster.name.c_str(), throw_name.c_str());
        }
    }

    //World.getTextManager().newLine(text_output);
    return 1;
}

int	StandardMonsterActions::MoveMonster(MonsterData* monster, int new_x, int new_y)
{
    if (monster->monster.GetEffect(paralysis))
        return 0;

    if (World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).terrain.type == stone)
        return 0;

    if (World.GetCurrentLevel() == 0 && (new_y == 0 || new_x == 0 || new_x == DUNGEON_SIZE_W - 1 || new_y == DUNGEON_SIZE_H - 1)
        && monster->isPlayer()) //exit world
    {
        if (World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).terrain.type != deepWater)
        {
            World.getTextManager().newLine("Do you want to leave? (y)");
            World.SetState(sLeave);
            return 0;
        }
    }

    if (new_x >= DUNGEON_SIZE_W - 1 || new_y >= DUNGEON_SIZE_H - 1 || new_x <= 0 || new_y <= 0)
        return 0;

    //check if a monster is in new spot(attack or stop)
    if (World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).GetMonster() != NULL)
    {
        if (monster->fleeing)
        {
            World.getTextManager().newLine("You cannot attack while fleeing. ");
            return 0;
        }
        Action* new_action = World.getActionManager().UpdateAction(&monster->action, aAttack, new_x, new_y);
        monster->NextAction(new_action);
        return 2;
    }

    //open door
    if (World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).terrain.type == closedDoor)   //open door
    {
        if (monster->isHumanoid())
        {
            World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).terrain.Create(openDoor);     //makeopenDoor();
            if (monster->isPlayer())
                World.getTextManager().newLine("You open the door. ");
            else if (monster->isSeen() == 1)
            {
                World.getTextManager().newLine("The %s opens the door. ", monster->monster.name.c_str());
            }
            else if (monster->isSeen() == 2)
                World.getTextManager().newLine("A door squeaks. ");
        }
        else
        {
            if (monster->isPlayer())
                World.getTextManager().newLine("You bang at the door. ");
            else if (monster->isSeen() == 1)
                World.getTextManager().newLine("The %s %s at the door. ", monster->monster.name.c_str(), Random::getInt(2, 0) ? "scratches" : "bangs");
            else if (monster->isSeen() == 2)
                World.getTextManager().newLine("You hear banging. ");

            if (Random::getInt(10, 0) == 0)
            {
                World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).terrain.Create(openDoor);     //makeopenDoor();
            }
        }
        return 3;
    }

    if (strcmp(World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).terrain.name, "a teleport") == 0)
    {
        Coord * ner_Coord = World.getDungeonManager().level(World.GetCurrentLevel()).FreeTerrainPosition(dfloor);
        if (ner_Coord)
        {
            new_x = ner_Coord->x;
            new_y = ner_Coord->y;

            StandardMonsterActions::ShowTrajectory(monster->level, monster->pos.x, monster->pos.y, new_x, new_y, '#', Random::getInt(2, 0) ? 255 : 128, 0, 255);


            if (monster->isPlayer())
                World.getTextManager().newLine("You are surrounded by colourful swirls of magic. ");
            else if (monster->isSeen() == 1)
            {
                World.getTextManager().newLine("The %s is engulfed by colourful swirls of magic.", monster->monster.name.c_str());
            }
            else if (monster->isSeen() == 2)
                World.getTextManager().newLine("You hear a distant whumph! ");

            monster->monster.AddEffect(confused, 1);
        }
    }

    //remove old ref on successful move
    World.getDungeonManager().level(World.GetCurrentLevel()).getCell(monster->pos.x, monster->pos.y).RemoveMonsterRef();

    //update monster position
    monster->pos.x = new_x;
    monster->pos.y = new_y;

    World.getDungeonManager().level(World.GetCurrentLevel()).getCell(monster->pos.x, monster->pos.y).RemoveMonsterRef();

    World.getDungeonManager().level(World.GetCurrentLevel()).getCell(new_x, new_y).AssignMonster(&monster->monster);

    return 1;
}

int	StandardMonsterActions::AttackMonster(MonsterData* attacker, int x, int y)
{
    MonsterData* defender = World.getMonsterManager().FindMonsterData(World.getDungeonManager().level(World.GetCurrentLevel()).getCell(x, y).GetMonster());

    if (defender == NULL)
        return 0;

    int attackStrength = attacker->AttackStrength();
    int defenceStrength = defender->DefendStrength();

    attackStrength = attackStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));
    defenceStrength = defenceStrength + (Random::getInt(7, 1) + Random::getInt(7, 1));

    // World.getTextManager().newLine("A:%d D:%d Dam:%d ", attackStrength, defenceStrength, CalculateDamage(defender, attackStrength, defenceStrength));

    // attack hits
    if (attackStrength > defenceStrength) //attacker hits defender
    {
        if (attacker->isPlayer()) //player is attacker
        {
            if (defender->AbsorbTest())
            {
                World.getTextManager().newLine("The %s's armour deflected the blow. ", defender->monster.name.c_str());
            }
            else
            {
                World.getTextManager().newLine("You hit the %s. ", defender->monster.name.c_str());
                if (defender->Name() == "wight")
                    World.getTextManager().newLine("It resists damage. ");
                else
                    defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);
                defender->monster.stamina -= AddBrandDamage(attacker, defender, World.getMonsterManager().getMonsterItems().GetEquipment(attacker, weapon));

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
                        World.getTextManager().newLine("The Ferryman transforms! ");
                    }
                    if (defender->Name() == "Boathouse Keeper" && Random::getInt(5, 1) == 3)
                    {
                        defender->monster.createWerewolf();
                        World.getTextManager().newLine("The Boathouse Keeper transforms! ");
                    }
                }
            }
        }
        else if (defender->isPlayer())
        {
            if (defender->AbsorbTest())
            {
                World.getTextManager().newLine("The %s's attack was deflected by your armour. ", attacker->monster.name.c_str());
            }
            else
            {
                World.getTextManager().newLine("The %s hits you. ", attacker->monster.name.c_str());
                defender->monster.stamina -= CalculateDamage(defender, attackStrength, defenceStrength);
                defender->monster.stamina -= AddBrandDamage(attacker, defender, World.getMonsterManager().getMonsterItems().GetEquipment(attacker, weapon));
                World.getDeathMessage().SetDeathMessage("was killed by a %s ", attacker->monster.name.c_str());
            }
        }
    }
    // attack misses
    else if (attackStrength < defenceStrength) //defender blocks attacker
    {
        if (attacker->isPlayer()) //player is attacker
        {
            World.getTextManager().newLine("You miss the %s. ", defender->monster.name.c_str());
        }

        else if (defender->isPlayer()) //player is defender
        {
            World.getTextManager().newLine("You block the %s's attack. ", attacker->monster.name.c_str());
        }
    }
    else //no damage done
    {
        if (attacker->isPlayer())
        {
            if (World.getMonsterManager().getMonsterItems().GetEquipment(defender, weapon) && defender->isHumanoid())
            {
                World.getTextManager().newLine("The %s parries your attack. ", defender->monster.name.c_str());
            }
            else
            {
                World.getTextManager().newLine("The %s dodges your attack. ", defender->monster.name.c_str());
            }
        }
        if (defender->isPlayer())
        {
            if (World.getMonsterManager().getMonsterItems().GetEquipment(defender, weapon))
            {
                World.getTextManager().newLine("You parry the %s's attack. ", attacker->monster.name.c_str());
            }
            else
            {
                World.getTextManager().newLine("You dodge the %s's attack. ", attacker->monster.name.c_str());
            }
        }
    }

    //sprintf(text_output, "%s as %d. %s ds %d. ",monster.name.c_str(),attackStrength,defender->name,defenceStrength);

    return 1;
}


int	StandardMonsterActions::CalculateDamage(MonsterData* defender, int attackStr, int defenceStr)
{
    int diff = attackStr - defenceStr;

    if (diff <= 0)
        return 0;

    Item* has_shield = World.getMonsterManager().getMonsterItems().GetEquipment(defender, shield);

    if (has_shield)
    {
        if (Random::getInt(6, 0) < has_shield->absorb_bonus) //block some damage
        {
            if (defender->isPlayer())
                World.getTextManager().newLine("Your shield partially blocks the attack. ");
            else
                World.getTextManager().newLine("The %s's shield partially blocks your attack. ", defender->Name().c_str());

            return 1;
        }
        // check luck 66% of time
        if (Random::getInt(3, 0))
        {
            if (defender->isPlayer() && defender->Stamina() < 4 && defender->TestLuck(true))
            {
                World.getTextManager().newLine("Your shield *luckily* partially blocks the attack. ");
                return 1;
            }
        }
    }
    return 2;
}

int StandardMonsterActions::AddBrandDamage(MonsterData* attacker, MonsterData* defender, Item* attack_weapon)
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
                int damage = World.getItemManager().CalculateBrandDamage(attack_weapon, it->first, it->second);
                extra_damage += World.getMonsterManager().CalculateBrandDamageOnMonster(defender, it->first, damage);
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
                        World.getTextManager().newLine("You get %s. ", attack_weapon->GetBrandName(it->first));
                    else
                        World.getTextManager().newLine("The %s gets %s. ", defender->monster.name.c_str(), attack_weapon->GetBrandName(it->first));
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
            int damage = World.getItemManager().CalculateBrandDamage(NULL, it->first, it->second);
            extra_damage += World.getMonsterManager().CalculateBrandDamageOnMonster(defender, it->first, damage);
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
                    World.getTextManager().newLine("You get %s. ", attacker->monster.GetBrandName(it->first));
                else
                    World.getTextManager().newLine("The %s gets %s. ", defender->monster.name.c_str(), attacker->monster.GetBrandName(it->first));
            }
        }
    }

    //World.getTextManager().newLine("Ext:%d ",extra_damage);
    if (extra_damage > 0)
        return 1;
    else return 0;

}

int	StandardMonsterActions::CastSpell(MonsterData*caster, int spellID)
{
    return World.getSpellManager().CallSpellRoutine(caster, spellID);
}

int	StandardMonsterActions::CastTargetSpell(MonsterData* caster, eSpellList spell, int TargetX, int TargetY)
{
    return World.getSpellManager().CastCurrentSpell(caster, TargetX, TargetY);
    //return caster->NextAction(World.getActionManager().UpdateAction(&caster->action,aCastSpell,TargetX,TargetY));
}

int	StandardMonsterActions::ThrowItem(MonsterData* attacker, const unsigned int targetX, const unsigned int targetY, const unsigned int inventoryItemRef)
{
    // check if has item
    ITEMLIST::iterator it;
    Item * throwItem = NULL;
    for (it = attacker->inventory.begin(); it != attacker->inventory.end(); it++)
    {
        if (it->ref == inventoryItemRef)
        {
            throwItem = &(*it);
            break;
        }
    }
    if (!throwItem)
    {
        World.getTextManager().newLine("You dont have that item. ");
        return 0;
    }

    //remove projectile from inventory and add to ground
    Item* newItem = World.getItemManager().DuplicateItem(throwItem);
    newItem->itemNumber[1] = 1;

    throwItem->itemNumber[1]--;
    if (throwItem->itemNumber[1] <= 0) //delete from inventory
    {
        attacker->inventory.erase(it);
    }



    // GET TRAJ
    DungeonLevel* level = World.getDungeonManager().CurrentLevel();

    int finalX = targetX;
    int finalY = targetY;

    level->HighLightPath(attacker->pos.x, attacker->pos.y, targetX, targetY);

    for (COORDLIST::iterator it = level->show_path.begin(); it != level->show_path.end(); it++) //something in the way!!
    {
        if (!level->IsCellTransparent(it->x, it->y) || level->getCell(it->x, it->y).monsterExists())
        {
            finalX = it->x;
            finalY = it->y;
            break;
        }
    }

    //GOT TRAJ

    ShowTrajectory(attacker->level, attacker->pos.x, attacker->pos.y, finalX, finalY, throwItem->symbol, throwItem->color1, throwItem->color2, throwItem->color3);

    Monster *d = World.getDungeonManager().level(World.GetCurrentLevel()).getCell(finalX, finalY).GetMonster();
    MonsterData* defender = World.getMonsterManager().FindMonsterData(d);

    if (attacker->isPlayer())
    {
        if (defender && defender != attacker)
        {
            World.getTextManager().newLine("The %s bounces off the %s. ", newItem->BaseName().c_str(), defender->Name().c_str());
            if (defender->Name() == "Zagor")
            {
                World.getTextManager().newLine("Zagor laughs at you! ");
                World.getDeathMessage().done.cheese = 1;
            }
        }
        else
            World.getTextManager().newLine("The %s hits the ground. ", newItem->BaseName().c_str());
    }
    else if (defender && defender->isPlayer())
    {
        if (!defender->TestLuck())
        {
            World.getTextManager().newLine("You are hit by a flying %s. ", newItem->BaseName().c_str());
            defender->monster.stamina -= 2;
        }
        else
        {
            World.getTextManager().newLine("You *luckily* avoid the flying %s. ", newItem->BaseName().c_str());
        }

    }
    World.getMonsterManager().getMonsterItems().AttemptDropItem(NULL, newItem, finalX, finalY);

    if (defender && defender->monster.stamina < 1) //dead
    {
        World.getDeathMessage().SetDeathMessage("was killed by a flying %s. ", newItem->BaseName().c_str());
    }

    return 1;
}
