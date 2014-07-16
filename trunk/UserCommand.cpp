// UserCommand.cpp: implementation of the UserCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "UserCommand.h"
#include "MonsterInfo.h"

#include "NumberGenerator.h"
#include "LevelChange.h"
#include <assert.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Coord UserCommand::look_pos;
int UserCommand::run_dir;
MonsterData * UserCommand::lastTarget = NULL;
Item * UserCommand::toThrow = NULL;

Coord UserCommand::autoTarget()
{
    MonsterData * player = World.getMonsterManager().Player();
    DungeonLevel * level = World.getDungeonManager().Level(player->level);

    int pX = player->pos.x;
    int pY = player->pos.y;

    if (lastTarget != NULL)
    {
        if (lastTarget->Stamina() <= 0 || lastTarget->isSeen() != 1)
            lastTarget = NULL;
    }

    if (lastTarget == NULL) // find closest monster
    {
        MONSTERLIST & monsters = World.getMonsterManager().monster_list;

        float range = 100;
        MONSTERLIST::iterator it = ++monsters.begin();// ignore player

        for (it; it != monsters.end(); it++)
        {
            if (it->level == player->level && it->isSeen() == 1)
            {
                float monsterRange = sqrt((float)(pX - it->pos.x)*(pX - it->pos.x) + (pY - it->pos.y)*(pY - it->pos.y));

                if (monsterRange < range)
                {
                    range = monsterRange;
                    lastTarget = &(*it);
                }
            }
        }
    }
    return lastTarget ? lastTarget->pos : player->pos;
}

int UserCommand::FireItem(bool itemCheck)
{
    UnLook();
    if (itemCheck && !World.getMonsterManager().monsterItems.GetEquipment(World.getMonsterManager().Player(), projectile))
    {
        World.getTextManager().newLine("Nothing to fire. ");
        return 0;
    }
    look_pos = autoTarget();

    bool keys[256] = { 0 };
    Look(keys, true);

    World.getTextManager().newLine("Use direction keys[dir], [space] to target, [x] to cancel. ");
    return 1;
}

int UserCommand::ThrowItem()
{
    World.getTextManager().clear();

    UnLook();
    // set auto target

    DungeonLevel & level = World.getDungeonManager().level[World.GetCurrentLevel()];
    MonsterData * monster = World.getMonsterManager().FindMonsterData(level.map[look_pos.x][look_pos.y].GetMonster());

    if (monster)
        lastTarget = monster;
    //else
    //    lastTarget = NULL;
    int ret = 0;
    if (toThrow)
        ret =  World.getActionManager().monsterAction.ThrowItem(World.getMonsterManager().Player(), look_pos.x, look_pos.y, toThrow->ref);
    toThrow = NULL;
    return ret;
}

int UserCommand::ThrowTarget(eAction action)
{
    World.getTextManager().clear();

    UnLook();
    // set auto target

    DungeonLevel & level = World.getDungeonManager().level[World.GetCurrentLevel()];
    MonsterData * monster = World.getMonsterManager().FindMonsterData(level.map[look_pos.x][look_pos.y].GetMonster());

    if (monster)
        lastTarget = monster;
    //else
    //    lastTarget = NULL;

    return World.getActionManager().monsterAction.ThrowTarget(World.getMonsterManager().Player(), look_pos.x, look_pos.y, action);
}

int UserCommand::Flee()
{
    MonsterData * player = World.getMonsterManager().Player();
    if (player->fleeing == true)
    {
        World.getTextManager().newLine("You are already fleeing. ");
        return 0;
    }

    World.getTextManager().newLine("Flee which direction[dir], [x] to cancel. ");
    return 1;
}

int UserCommand::CastSpellAtTarget()
{
    World.getTextManager().clear();

    UnLook();

    DungeonLevel & level = World.getDungeonManager().level[World.GetCurrentLevel()];
    MonsterData * monster = World.getMonsterManager().FindMonsterData(level.map[look_pos.x][look_pos.y].GetMonster());

    if (monster)
        lastTarget = monster;

    return //World.getActionManager().monsterAction.ThrowTarget(World.getMonsterManager().Player(),look_pos.x,look_pos.y);
        World.getSpellManager().CastCurrentSpell(World.getMonsterManager().Player(), look_pos.x, look_pos.y);
}

int UserCommand::ClimbStairs(int dir)
{
    int ret = 0;
    try{
        LevelChange change;
        ret = change.ChangeLevel(dir, NULL);

        int cur_level = World.GetCurrentLevel();

        if (cur_level == 1)
            World.getDeathMessage().done.entered = 1;
        if (cur_level == 11)
            World.getDeathMessage().done.barracks = 1;
        if (cur_level == 5)
            World.getDeathMessage().done.river = 1;
        if (cur_level == 6)
            World.getDeathMessage().done.undead = 1;
        if (cur_level == 8)
            World.getDeathMessage().done.maze = 1;
    }
    catch (const std::exception & ex)
    {
        MessageBox(NULL, ex.what(), "Climb Stairs", MB_ICONERROR);
    }
    return ret;
}

int UserCommand::UnLook()
{
    DungeonLevel *level = &World.getDungeonManager().level[World.GetCurrentLevel()];
    level->map[look_pos.x][look_pos.y].show_target = 0;
    return 1;
}

int UserCommand::Look(bool *keys, bool show_path)
{
    DungeonLevel *level = &World.getDungeonManager().level[World.GetCurrentLevel()];
    UnLook();

    if (keys[VC_X] || keys[VK_ESC])
    {
        keys[VC_X] = false;
        keys[VK_ESC] = false;
        return 0;
    }

    if (!show_path && keys[VK_M] && level->map[look_pos.x][look_pos.y].GetMonster())
    {
        World.SetState(sLookMore);
        keys[VK_M] = false;
    }
    else
    {
        int dir = GetDirection(keys);

        switch (dir)
        {
        case dNorth: look_pos.y--; break;
        case dEast:  look_pos.x++; break;
        case dSouth: look_pos.y++; break;
        case dWest:  look_pos.x--; break;

        case dNorthEast: look_pos.x++; look_pos.y--; break;
        case dSouthEast: look_pos.x++; look_pos.y++; break;
        case dSouthWest: look_pos.x--; look_pos.y++; break;
        case dNorthWest: look_pos.x--; look_pos.y--; break;

        default:
            if (show_path)
            {
                MonsterData * player = World.getMonsterManager().Player();
                int p_x = player->pos.x;
                int p_y = player->pos.y;
                DungeonLevel *lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

                lev->HighLightPath(p_x, p_y, look_pos.x, look_pos.y);
            }
            level->map[look_pos.x][look_pos.y].show_target = 1; return 1;
        }

        if (look_pos.x < 0) look_pos.x = 0;
        if (look_pos.x > DUNGEON_SIZE_W - 1) look_pos.x = DUNGEON_SIZE_W - 1;
        if (look_pos.y < 0) look_pos.y = 0;
        if (look_pos.y > DUNGEON_SIZE_H - 1) look_pos.y = DUNGEON_SIZE_H - 1;

        if (World.State() == sFire || World.State() == sThrow || World.State() == sTargetSpell) //throwing not looking
        {
            if (show_path)
            {
                MonsterData * player = World.getMonsterManager().Player();
                int p_x = player->pos.x;
                int p_y = player->pos.y;
                DungeonLevel *lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

                lev->HighLightPath(p_x, p_y, look_pos.x, look_pos.y);
                level->map[look_pos.x][look_pos.y].show_target = 1;

                //World.getTextManager().newLine("Use direction keys[dir], [space] to target, [x] to cancel. ");
            }
            return 1;
        }
        else
            level->map[look_pos.x][look_pos.y].show_target = 1;
    }

    if (!level->map[look_pos.x][look_pos.y].terrain.found) //dont show
    {
        World.getTextManager().newLine("You have not been here yet. ");
    }
    else if (level->map[look_pos.x][look_pos.y].GetMonster() && level->map[look_pos.x][look_pos.y].terrain.light) //display monsters
    {
        MonsterData * monster = World.getMonsterManager().FindMonsterData(level->map[look_pos.x][look_pos.y].GetMonster());

        if (World.State() == sLookMore)
        {
            MonsterInfo info;
            info.ShowMonsterInfo(monster);
            return 2;

        }
        float max_stamina = (float)level->map[look_pos.x][look_pos.y].GetMonster()->MaxStamina();
        float cur_stamina = (float)level->map[look_pos.x][look_pos.y].GetMonster()->stamina;

        int state = World.getMonsterManager().FindMonsterData(level->map[look_pos.x][look_pos.y].GetMonster())->GetState();

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
            strcpy(status, "You are ready for anything");
        else
        {
            switch (state)
            {
            case normal:	strcpy(status, "It does not care about you"); break;
            case sentry:	strcpy(status, "It appears sleepy"); break;
            case hostile:	strcpy(status, "It is hostile"); break;
            case waking:	strcpy(status, "It looked annoyed"); break;
            case asleep:	strcpy(status, "It is asleep"); break;
            }
        }

        if (monster->isPlayer())
            World.getTextManager().newLine("This is %s. %s. You are %s. [m]ore.", monster->Name().c_str(), status, health);
        else
            World.getTextManager().newLine("You see a %s. %s. It looks %s. [m]ore.", monster->Name().c_str(), status, health);
    }
    else if (level->map[look_pos.x][look_pos.y].getItem()) //display items
    {
        Item * item = level->map[look_pos.x][look_pos.y].getItem();
        if (level->map[look_pos.x][look_pos.y].terrain.light)
        {
            if (item->stackable() && item->itemNumber[1] > 1)
            {
                if (item->identified)
                    World.getTextManager().newLine("You see %s here.", item->GetName().c_str());
                else
                    World.getTextManager().newLine("You see some %s here.", item->GetName().c_str());
            }
            else if (level->map[look_pos.x][look_pos.y].getItem()->identified)
                World.getTextManager().newLine("You see a %s.", item->GetName().c_str());
            else
                World.getTextManager().newLine("You see an %s.", item->GetName().c_str());
        }
        else
            World.getTextManager().newLine("Last time you looks this was a %s.", item->BaseName().c_str());
    }
    else //display terrain
    {
        if (level->map[look_pos.x][look_pos.y].terrain.type == lockedStairs)
        {
            World.getTextManager().newLine("This is %s (%d).", level->map[look_pos.x][look_pos.y].terrain.name,
                World.getItemManager().keyLabels[World.GetCurrentLevel()]);
        }
        else if (level->map[look_pos.x][look_pos.y].terrain.type == specialLocked)
        {
            World.getTextManager().newLine("This is %s (%d).", level->map[look_pos.x][look_pos.y].terrain.name,
                World.getItemManager().KeySpecial[World.GetCurrentLevel()]);
        }
        else
            World.getTextManager().newLine("This is %s.", level->map[look_pos.x][look_pos.y].terrain.name);
    }

    return 1;
}

int UserCommand::Look()
{
    World.getTextManager().newLine("Use direction keys to look[dir]. [x] to cancel. ");

    MonsterData * player = World.getMonsterManager().Player();

    Coord * pos = player->getPosition();

    look_pos.x = pos->x;
    look_pos.y = pos->y;

    return 0;
}

int UserCommand::GetDirection(bool *keys)
{

    if (keys[VK_UP] || keys[VK_NUMPAD8])
    {
        keys[VK_UP] = false;
        keys[VK_NUMPAD8] = false;
        return dNorth;
    }

    else if (keys[VK_NUMPAD9] || keys[VK_PRIOR])
    {
        keys[VK_NUMPAD9] = false;
        keys[VK_PRIOR] = false;
        return dNorthEast;
    }

    else if (keys[VK_RIGHT] || keys[VK_NUMPAD6])
    {
        keys[VK_RIGHT] = false;
        keys[VK_NUMPAD6] = false;
        return dEast;
    }

    else if (keys[VK_NUMPAD3] || keys[VK_NEXT])
    {
        keys[VK_NUMPAD3] = false;
        keys[VK_NEXT] = false;
        return dSouthEast;
    }

    else if (keys[VK_DOWN] || keys[VK_NUMPAD2])
    {
        keys[VK_DOWN] = false;
        keys[VK_NUMPAD2] = false;
        return dSouth;
    }

    else if (keys[VK_NUMPAD1] || keys[VK_END])
    {
        keys[VK_NUMPAD1] = false;
        keys[VK_END] = false;
        return dSouthWest;
    }

    else if (keys[VK_LEFT] || keys[VK_NUMPAD4])
    {
        keys[VK_NUMPAD4] = false;
        keys[VK_LEFT] = false;
        return dWest;
    }

    else if (keys[VK_NUMPAD7] || keys[VK_HOME])
    {
        keys[VK_HOME] = false;
        keys[VK_NUMPAD7] = false;
        return dNorthWest;
    }

    else if (keys[VK_NUMPAD5] || keys[VK_CLEAR] || keys[VK_FULLSTOP])
    {
        keys[VK_FULLSTOP] = false;
        keys[VK_CLEAR] = false;
        keys[VK_NUMPAD5] = false;
        return dWait;
    }

    else return dNone;
}


int UserCommand::FleeCommand(int  dir)
{
    MonsterData * player = World.getMonsterManager().Player();


    if (player->fleeing == true)
    {
        World.getTextManager().newLine("You are already fleeing. ");
        return 0;
    }

    Coord * pos = player->getPosition();
    Coord new_pos;
    new_pos.x = pos->x;
    new_pos.y = pos->y;


    switch (dir)
    {
    case dNorth: new_pos.y--; break;
    case dEast:  new_pos.x++; break;
    case dSouth: new_pos.y++; break;
    case dWest:  new_pos.x--; break;

    case dNorthEast: new_pos.x++; new_pos.y--; break;
    case dSouthEast: new_pos.x++; new_pos.y++; break;
    case dSouthWest: new_pos.x--; new_pos.y++; break;
    case dNorthWest: new_pos.x--; new_pos.y--; break;

    case dWait: return 0;

    }

    //can more there
    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.type != stone)
    {
        //check if occupied
        if (World.getDungeonManager().level[World.GetCurrentLevel()].map[new_pos.x][new_pos.y].getMonster())
        {
            World.getTextManager().newLine("You cant flee that way. ");

            return 0;
        }

        if (player->NextAction(World.getActionManager().UpdateAction(&player->action, aMove, new_pos.x, new_pos.y)))
        {
            //FleeTest
            if (!player->TestLuck(false))
            {
                World.getTextManager().newLine("You fail to flee. ");
                return 1;
            }

            player->fleeing = true;
            player->flee_count += 10;
            World.getTextManager().newLine("You flee. ");

            player->monster.color1 = 255;
            player->monster.color2 = 128;
            player->monster.color3 = 64;

            player->luck_penalty--;

            return 0; //return 0 to get a free movement turn.
        }
    }
    else
        World.getTextManager().newLine("You cant flee that way. ");

    return 0;
}

int UserCommand::MoveCommand(int  dir)
{
    MonsterData * player = World.getMonsterManager().Player();

    Coord * pos = player->getPosition();
    Coord new_pos;
    new_pos.x = pos->x;
    new_pos.y = pos->y;

    switch (dir)
    {
    case dNorth: new_pos.y--; break;
    case dEast:  new_pos.x++; break;
    case dSouth: new_pos.y++; break;
    case dWest:  new_pos.x--; break;

    case dNorthEast: new_pos.x++; new_pos.y--; break;
    case dSouthEast: new_pos.x++; new_pos.y++; break;
    case dSouthWest: new_pos.x--; new_pos.y++; break;
    case dNorthWest: new_pos.x--; new_pos.y--; break;

    case dWait: break;

    }

    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.type != stone)
    {
        if (dir != dWait)
            player->NextAction(World.getActionManager().UpdateAction(&player->action, aMove, new_pos.x, new_pos.y));
        else
            player->NextAction(World.getActionManager().UpdateAction(&player->action, aWait));

        //move over item mesage 
        if (dir != dWait  && World.getDungeonManager().level[World.GetCurrentLevel()].map[player->getPosition()->x][player->getPosition()->y].getItem())
        {
            if (player->action.Type() == aMove)
            {
                Item * item = World.getDungeonManager().level[World.GetCurrentLevel()].map[player->getPosition()->x][player->getPosition()->y].getItem();
                assert(item);

                // auto projectile pickup
                bool pickup = false;
                if (item->type == projectile)
                {
                    Item* projectilePile = World.getMonsterManager().monsterItems.GetEquipment(player, projectile);
                    if (projectilePile && projectilePile->secondaryType == item->secondaryType &&
                        projectilePile->skill_bonus == item->skill_bonus  &&
                        projectilePile->BaseName() == item->BaseName() &&
                        projectilePile->getPrefix() == item->getPrefix() &&
                        projectilePile->getPostfix() == item->getPostfix() &&
                        item->identified)
                    {
                        player->NextAction(World.getActionManager().UpdateAction(&player->action, aPickup, new_pos.x, new_pos.y));
                        pickup = true;
                    }

                }
                /*  else if (item->type == key)
                  {
                  player->NextAction(World.getActionManager().UpdateAction(&player->action, aPickup, new_pos.x, new_pos.y));
                  pickup = true;
                  }*/

                if (!pickup)
                {
                    DungeonLevel* level = &World.getDungeonManager().level[World.GetCurrentLevel()];
                    if (level->map[player->getPosition()->x][player->getPosition()->y].getItem()->itemNumber[1] > 1)
                    {
                        if (item->identified)
                            World.getTextManager().newLine("You see %s here.", level->map[player->getPosition()->x][player->getPosition()->y].getItem()->GetName().c_str());
                        else
                            World.getTextManager().newLine("You see some %s.", level->map[player->getPosition()->x][player->getPosition()->y].getItem()->GetName().c_str());
                    }
                    else if (level->map[player->getPosition()->x][player->getPosition()->y].getItem()->identified)
                        World.getTextManager().newLine("You see a %s here. ", level->map[player->getPosition()->x][player->getPosition()->y].getItem()->GetName().c_str());
                    else
                        World.getTextManager().newLine("You see a %s here. ", level->map[player->getPosition()->x][player->getPosition()->y].getItem()->GetName().c_str());
                }
            }
        }
        return 1;
    }
    return 0; //no move
}

int UserCommand::Close(bool *keys)
{
    if (keys[VC_X] || keys[VK_ESC])
    {
        keys[VC_X] = false;;
        keys[VK_ESC] = false;
        return 0;
    }


    int dir = GetDirection(keys);

    if (dir == dNone) //invalid key press
    {
        World.getTextManager().newLine("Invalid direction! Close What?[dir] - [x] to cancel. ");
        return -1;
    }

    MonsterData * player = World.getMonsterManager().Player();

    Coord * pos = player->getPosition();
    Coord new_pos;
    new_pos.x = pos->x;
    new_pos.y = pos->y;

    switch (dir)
    {
    case dNorth: new_pos.y--; break;
    case dEast:  new_pos.x++; break;
    case dSouth: new_pos.y++; break;
    case dWest:  new_pos.x--; break;

    case dNorthEast: new_pos.x++; new_pos.y--; break;
    case dSouthEast: new_pos.x++; new_pos.y++; break;
    case dSouthWest: new_pos.x--; new_pos.y++; break;
    case dNorthWest: new_pos.x--; new_pos.y--; break;

    case dWait: return 1;

    }

    if (World.getDungeonManager().level[World.GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.type == openDoor) //open door
    {

        if (World.getDungeonManager().level[World.GetCurrentLevel()].map[new_pos.x][new_pos.y].getItem() ||
            World.getDungeonManager().level[World.GetCurrentLevel()].map[new_pos.x][new_pos.y].GetMonster())
        {
            World.getTextManager().newLine("The door is blocked. ");
            return 0;
        }

        World.getDungeonManager().level[World.GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.Create(closedDoor);//makeclosedDoor();
        World.getTextManager().newLine("You close the door. ");
        return 1;
    }


    World.getTextManager().newLine("There is nothing there to close. ");
    return 0;
}

int UserCommand::InitRun()
{

    World.getTextManager().newLine("Run which direction[dir]?. [x] to cancel. ");

    run_dir = -1;
    return 1;
}

int UserCommand::RunDirection(int dir)
{
    if (dir == dNone || dir == dWait)
        return 0;
    run_dir = dir;

    return 1;
}

int UserCommand::Run()
{

    MonsterData *player = World.getMonsterManager().Player();

    Coord * pos = player->getPosition();
    Coord new_pos;
    new_pos.x = pos->x;
    new_pos.y = pos->y;

    switch (run_dir)
    {
    case dNorth: new_pos.y--; break;
    case dEast:  new_pos.x++; break;
    case dSouth: new_pos.y++; break;
    case dWest:  new_pos.x--; break;

    case dNorthEast: new_pos.x++; new_pos.y--; break;
    case dSouthEast: new_pos.x++; new_pos.y++; break;
    case dSouthWest: new_pos.x--; new_pos.y++; break;
    case dNorthWest: new_pos.x--; new_pos.y--; break;

    case dWait: return 2;

    }


    DungeonLevel *level = &World.getDungeonManager().level[World.GetCurrentLevel()];

    if (run_dir == dNone)
        return 2;

    for (MONSTERLIST::iterator it = World.getMonsterManager().monster_list.begin(); it != World.getMonsterManager().monster_list.end(); it++)
    { //is this too slow? seems OK to me
        if (it->level == World.GetCurrentLevel() && !it->isPlayer())
            if (it->isSeen() == 1)
                return 0;
    }


    if (level->map[new_pos.x][new_pos.y].terrain.type != deepWater)
    {
        int halt;// = HaltRunFeature(pos->x, pos->y);
        halt = HaltRunFeature(pos->x + 1, pos->y);
        halt += HaltRunFeature(pos->x - 1, pos->y);
        halt += HaltRunFeature(pos->x, pos->y + 1);
        halt += HaltRunFeature(pos->x, pos->y - 1);
        halt = HaltRunFeature(pos->x + 2, pos->y);
        halt += HaltRunFeature(pos->x - 2, pos->y);
        halt += HaltRunFeature(pos->x, pos->y + 2);
        halt += HaltRunFeature(pos->x, pos->y - 2);

        halt = HaltRunFeature(new_pos.x + 1, new_pos.y, pos->x, pos->y);
        halt += HaltRunFeature(new_pos.x - 1, new_pos.y, pos->x, pos->y);
        halt += HaltRunFeature(new_pos.x, new_pos.y + 1, pos->x, pos->y);
        halt += HaltRunFeature(new_pos.x, new_pos.y - 1, pos->x, pos->y);

        halt += HaltRun(new_pos.x + 1, new_pos.y + 1, pos->x, pos->y);
        halt += HaltRun(new_pos.x + 1, new_pos.y - 1, pos->x, pos->y);
        halt += HaltRun(new_pos.x - 1, new_pos.y + 1, pos->x, pos->y);
        halt += HaltRun(new_pos.x - 1, new_pos.y - 1, pos->x, pos->y);
        if (!halt)
            return MoveCommand(run_dir);
    }

    return 0; //stop running
}

int UserCommand::HaltRun(int x, int y, int oldx, int oldy)
{
    if (x == oldx && y == oldy)
        return 0;

    DungeonLevel *level = &World.getDungeonManager().level[World.GetCurrentLevel()];

    if (level->map[x][y].GetMonster() == NULL)

        return 0;
    return 1;

}
int UserCommand::HaltRunFeature(int x, int y, int oldx, int oldy)
{
    if (x == oldx && y == oldy)
        return 0;

    DungeonLevel *level = &World.getDungeonManager().level[World.GetCurrentLevel()];

    if (level->map[x][y].GetMonster() == NULL
        && level->map[x][y].terrain.type != closedDoor
        && level->map[x][y].terrain.type != bridge
        //&& level->map[x][y].terrain.type != openDoor
        && level->map[x][y].getItem() == NULL)
        return 0;
    return 1;

}

int UserCommand::DisplayHelpAbout()
{
    World.getTextManager().ClearDisplayLines();

    World.getTextManager().SetDisplayLine(0, "The Warlock of Firetop Mountain Roguelike (c) 2014 Corremn");
    World.getTextManager().SetDisplayLine(1, "==========================================================");
    World.getTextManager().SetDisplayLine(3, "http://sites.google.com/site/corremn/");
    World.getTextManager().SetDisplayLine(4, "mailto:corremn@gmail.com");

    World.getTextManager().SetDisplayLine(5, "This game was originally created for the 2007 seven-day roguelike competition. ");
    World.getTextManager().SetDisplayLine(5, "See changelog.txt for recent developments. ");

    World.getTextManager().SetDisplayLine(9, "You are seeking the Warlock Zagor's Treasure. In your way are the inhabitants");
    World.getTextManager().SetDisplayLine(10, "of the mountain, underground rivers, locked gates and of course the warlock himself.");
    World.getTextManager().SetDisplayLine(11, "Of course you will find plenty of items within the mountain to help you in your quest.");

    World.getTextManager().SetDisplayLine(14, "HINTS");
    World.getTextManager().SetDisplayLine(15, "Skill represents your skill with the current weapon, Stamina is your hit points.");
    World.getTextManager().SetDisplayLine(16, "Luck helps you escape damage and find better items, especially from chests. Higher the better.");
    World.getTextManager().SetDisplayLine(17, "Armour quality is based on the AC value [x]. The higher the better. ");
    World.getTextManager().SetDisplayLine(18, "Most damage taken is 1-2 points. Shields can reduce damage from hits. ");
    World.getTextManager().SetDisplayLine(19, "You can attempt to flee combat at the cost of a luck point.");

    World.getTextManager().SetDisplayLine(20, "Creatures are weaker in water.");
    World.getTextManager().SetDisplayLine(21, "Locked gates and chests can be opened with the correct key.");
    World.getTextManager().SetDisplayLine(22, "To unlock something use the [u]se command or enter it.");
    World.getTextManager().SetDisplayLine(23, "The same key may have more than one use, so dont throw them away.");
    World.getTextManager().SetDisplayLine(24, "To access Zagor's treasure you will need multiple keys.");
    World.getTextManager().SetDisplayLine(25, "Archers are more accurate over short distances.");
    //World.getTextManager().SetDisplayLine(25, "Resting between levels can be dangerous.");
    World.getTextManager().SetDisplayLine(26, "Items will change to blue if it is better than your current one.");
    World.getTextManager().SetDisplayLine(29, "Fountains need to be [u]sed.");

    World.getTextManager().SetDisplayLine(30, "Contact the author to support the continuing development of this game.");

    World.getTextManager().SetDisplayLine(39, "[k] key bindings, [x] to exit");

    return 1;
}

int UserCommand::DisplayHelp()
{

    World.getTextManager().ClearDisplayLines();

    World.getTextManager().SetDisplayLine(0, "Help");
    World.getTextManager().SetDisplayLine(1, "====");
    World.getTextManager().SetDisplayLine(2, "");

    World.getTextManager().SetDisplayLine(3, "Commands");

    World.getTextManager().SetDisplayLine(5, " move               - [arrow keys] or [numpad]");
    World.getTextManager().SetDisplayLine(6, " continuous move    - [.] + [dir]");
    World.getTextManager().SetDisplayLine(7, " wait               - [numpad 5]");
    World.getTextManager().SetDisplayLine(8, " fire ranged weapon - [numpad f]");
    World.getTextManager().SetDisplayLine(9, " flee               - [F]");
    World.getTextManager().SetDisplayLine(10, " pickup             - [g] or [,]");
    World.getTextManager().SetDisplayLine(12, " close              - [c]");
    World.getTextManager().SetDisplayLine(13, " unlock/use         - [u] + [dir]");
    World.getTextManager().SetDisplayLine(14, " climb up           - [<]");
    World.getTextManager().SetDisplayLine(15, " climb down         - [>]");
    World.getTextManager().SetDisplayLine(16, " use magic          - [z]");

    World.getTextManager().SetDisplayLine(11, " look around        - [l] + [dir]");
    World.getTextManager().SetDisplayLine(18, " view inventory     - [i]");
    World.getTextManager().SetDisplayLine(19, " view equipment     - [e]");
    World.getTextManager().SetDisplayLine(20, " message history    - [ctrl-p]");

    World.getTextManager().SetDisplayLine(21, " toggle Full Screen - [F1]");

#ifdef _DEBUG
    World.getTextManager().SetDisplayLine(23, "Debug commands");
    World.getTextManager().SetDisplayLine(24, " God view            - [s]");
    World.getTextManager().SetDisplayLine(25, " suicide             - [t]");
    World.getTextManager().SetDisplayLine(26, " Dungeon Level Down  - [1]     Dungeon Level Up  - [2]");
    World.getTextManager().SetDisplayLine(27, " Max Health          - [3]");
#endif

    World.getTextManager().SetDisplayLine(39, "[a] about game, [x] to exit");
    return 1;
}


int UserCommand::HelpCommand(bool *keys)
{
    if (keys[VC_X] || keys[VK_ESC]) //exit help
    {
        keys[VC_X] = false;
        keys[VK_ESC] = false;
        return 0;

    }
    else if (keys[VK_A])
    {
        keys[VK_A] = false;
        DisplayHelpAbout();
        return 1;

    }
    else if (keys[VK_K])
    {
        keys[VK_K] = false;
        DisplayHelp();
        return 1;

    }

    return 1;
}

int UserCommand::UseItem()
{
    World.getTextManager().newLine("What direction[dir]?");
    return 0;
}
int UserCommand::UseItem(bool *keys)
{
    if (keys[VC_X] || keys[VK_ESC]) //exit help
    {
        keys[VC_X] = false;
        keys[VK_ESC] = false;
        return 0;

    }
    return UseItem(NULL, GetDirection(keys));
}

int  UserCommand::UseItem(Item*item, int dir)
{
    DungeonLevel *dlevel = &World.getDungeonManager().level[World.GetCurrentLevel()];
    MonsterData * player = World.getMonsterManager().Player();

    Coord * pos = player->getPosition();
    Coord new_pos;
    new_pos.x = pos->x;
    new_pos.y = pos->y;

    switch (dir)
    {
    case dNorth: new_pos.y--; break;
    case dEast:  new_pos.x++; break;
    case dSouth: new_pos.y++; break;
    case dWest:  new_pos.x--; break;

    case dNorthEast: new_pos.x++; new_pos.y--; break;
    case dSouthEast: new_pos.x++; new_pos.y++; break;
    case dSouthWest: new_pos.x--; new_pos.y++; break;
    case dNorthWest: new_pos.x--; new_pos.y--; break;

    case dWait: break;
    default: World.getTextManager().newLine("Invalid direction. "); return 0;

    }

    //	if(item->type==key) //unlock
    {
        if (dlevel->map[new_pos.x][new_pos.y].terrain.type == lockedStairs)
        {
            ITEMLIST::iterator it;
            ITEMLIST *inventory = &World.getMonsterManager().Player()->inventory;
            int level = World.GetCurrentLevel();

            char buf[64];
            sprintf(buf, "key labelled %d", World.getItemManager().keyLabels[level]);

            int match = 0;

            for (it = inventory->begin(); it != inventory->end(); it++)
            {
                if (it->BaseName() == buf)
                    match = 1;
            }
            if (match == 0)
            {
                World.getTextManager().newLine("You do not have the correct key. ");
                return 1;
            }
            else
            {
                World.getTextManager().newLine("You unlock the gate. ");
                dlevel->map[new_pos.x][new_pos.y].terrain.Create(openStairs);
                return 1;
            }
        }
        else if (dlevel->map[new_pos.x][new_pos.y].terrain.type == specialLocked)
        {
            ITEMLIST::iterator it;
            ITEMLIST *inventory = &World.getMonsterManager().Player()->inventory;
            int level = World.GetCurrentLevel();

            char buf[64];
            sprintf(buf, "key labelled %d", World.getItemManager().KeySpecial[level]);

            int match = 0;

            for (it = inventory->begin(); it != inventory->end(); it++)
            {
                if (it->BaseName() == buf)
                    match = 1;
            }
            if (match == 0)
            {
                World.getTextManager().newLine("You do not have the correct key. ");
                return 0;
            }
            else
            {
                World.getTextManager().newLine("You unlock the gate. You hear mad laughing. ");
                dlevel->map[new_pos.x][new_pos.y].terrain.Create(specialOpen);
                return 1;
            }
        }
        if (strcmp(dlevel->map[new_pos.x][new_pos.y].terrain.name, "a teleport") == 0)
        {
            World.getTextManager().newLine("Walk into it! ");
            return 0;
        }
        if (strcmp(dlevel->map[new_pos.x][new_pos.y].terrain.name, "a fountain") == 0)
        {
            player->monster.stamina = player->monster.MaxStamina();

            World.getTextManager().newLine("You drink from the fountain. You feel healthier. ");
            dlevel->map[new_pos.x][new_pos.y].terrain.Create(dryFountain);

            return 1;
        }
        if (dlevel->map[new_pos.x][new_pos.y].getItem()) //item exists
        {
            if (dlevel->map[new_pos.x][new_pos.y].getItem()->type == lockedChest)
            {
                ITEMLIST::iterator it;
                ITEMLIST *inventory = &World.getMonsterManager().Player()->inventory;

                int match = 0;

                if (World.GetCurrentLevel() != 19)
                {
                    for (it = inventory->begin(); it != inventory->end(); it++)
                    {
                        if (it->itemNumber[0] == dlevel->map[new_pos.x][new_pos.y].getItem()->itemNumber[0]) //chest (%d)
                        {
                            match = 1;
                            break;
                        }
                    }
                    if (match)
                    {
                        dlevel->map[new_pos.x][new_pos.y].getItem()->CreateItem(openChest, World.GetCurrentLevel());
                        World.getTextManager().newLine("You open the chest. ");

                        if (World.GetCurrentLevel() > 9) //create special item
                        {
                            int item_boost = 0;
                            if (player->TestLuck())
                            {
                                item_boost += 50;
                            }

                            int random_item = Random::getInt(5, 0);
                            Item *new_it;
                            switch (random_item)
                            {
                            case 0: new_it = World.getItemManager().CreateItem(20 + item_boost, armour, platemail);
                                World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            case 1: new_it = World.getItemManager().CreateItem(20 + item_boost, weapon, axe);
                                World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            case 2:;
                            case 3: new_it = World.getItemManager().CreateItem(20 + item_boost, shield);
                                World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            default:  new_it = World.getItemManager().CreateRandomItem(World.GetCurrentLevel() + 20 + item_boost);
                                World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y);
                                new_it = World.getItemManager().CreateRandomItem(World.GetCurrentLevel() + 20 + item_boost);
                                World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                                new_it = World.getItemManager().CreateRandomItem(World.GetCurrentLevel() + 20 + item_boost);
                                World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            }
                        }
                        else
                        {
                            int random_items = Random::getInt(5, 2);

                            for (int i = 0; i < random_items; i++)
                            {
                                int item_boost = 0; //lucky items
                                if (player->TestLuck())
                                {
                                    item_boost += 15;

                                    if (player->TestLuck())
                                    {
                                        item_boost += 15;
                                    }
                                }

                                Item *new_it = World.getItemManager().CreateRandomItem(World.GetCurrentLevel() + item_boost);
                                World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y);
                            }
                        }
                        return 1;
                    }
                    else
                    {
                        World.getTextManager().newLine("You do not have the correct key. ");
                        return 1;
                    }
                }
                else //Zagor Chest
                {
                    for (it = inventory->begin(); it != inventory->end(); it++)
                    {
                        if (it->itemNumber[0] == dlevel->map[new_pos.x][new_pos.y].getItem()->itemNumber[0] ||
                            it->itemNumber[0] == dlevel->map[new_pos.x][new_pos.y].getItem()->itemNumber[1] ||
                            it->itemNumber[0] == dlevel->map[new_pos.x][new_pos.y].getItem()->itemNumber[2])
                            match++;
                    }
                    if (match == 3)
                    {
                        dlevel->map[new_pos.x][new_pos.y].getItem()->CreateItem(openChest, World.GetCurrentLevel());
                        World.getTextManager().newLine("Click, Click, Click! You open the chest. <insert trumpet sounds here>");
                        Item *new_it = World.getItemManager().CreateItem(World.GetCurrentLevel(), gold);
                        World.getMonsterManager().monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y);

                        return 1;
                    }
                    else if (match == 2)
                    {
                        World.getTextManager().newLine("Click, Click, Damn!! You do not have all the keys. ");
                        return 1;
                    }
                    else if (match == 1)
                    {
                        World.getTextManager().newLine("Click, Umm, Hmmm!! You do not have all the keys. ");
                        return 1;
                    }
                    else
                    {
                        World.getTextManager().newLine("You do not have any of the correct keys. ");
                        return 1;
                    }

                }

            }
            else if (dlevel->map[new_pos.x][new_pos.y].getItem()->type == openChest)
            {
                World.getTextManager().newLine("All ready open. ");
                return 0;
            }
            else if (dlevel->map[new_pos.x][new_pos.y].getItem()->type == corpse)
            {
                if (Random::getInt(2, 0))
                    World.getTextManager().newLine("It says 'Hello my name's Frank, whats yours?'. Come on, its a corpse!");
                else
                    World.getTextManager().newLine("It appears to be smiling at you! ");
                return 0;
            }
            else //nothing to use
            {
                World.getTextManager().newLine("Cant use that. ");
                return 0;
            }
        }
    }
    World.getTextManager().newLine("Nothing to use. ");
    return 0;

}

//some debug commands
int UserCommand::Debug(bool *keys)
{
    try{
        LevelChange change;
        if (keys[VK_1])
            change.DebugLevel(1);
        else if (keys[VK_2])
            change.DebugLevel(2);

    }
    catch (const std::exception & ex)
    {
        MessageBox(NULL, ex.what(), "Debug", MB_ICONERROR);
    }

    return 1;
}

void UserCommand::Throw(Item * throwItem)
{
    //UnLook();
    look_pos.x = World.getMonsterManager().Player()->pos.x;
    look_pos.y = World.getMonsterManager().Player()->pos.y;
    World.getTextManager().newLine("Use direction keys[dir], [space] to target, [x] to cancel. ");

    toThrow = throwItem;
}