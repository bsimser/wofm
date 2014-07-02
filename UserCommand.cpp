// UserCommand.cpp: implementation of the UserCommand class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "UserCommand.h"
#include "MonsterInfo.h"

#include "NumberGenerator.h"
#include "LevelChange.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

coord UserCommand::look_pos;
int UserCommand::run_dir;
monsterData * UserCommand::lastTarget = NULL;

coord UserCommand::autoTarget()
{
    monsterData * player = WorldBuilder::monsterManager.Player();
    DungeonLevel * level = WorldBuilder::dungeonManager.Level(player->level);

    int pX = player->pos.x;
    int pY = player->pos.y;

    if (lastTarget != NULL)
    {
        if (lastTarget->Stamina() <= 0 || lastTarget->isSeen() != 1)
            lastTarget = NULL;
    }

    if (lastTarget == NULL) // find closest monster
    {
        MONSTERLIST & monsters = WorldBuilder::monsterManager.monster_list;

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

int UserCommand::ThrowItem()
{
    UnLook();
    if (!WorldBuilder::monsterManager.monsterItems.GetEquipment(WorldBuilder::monsterManager.Player(), projectile))
    {
        WorldBuilder::textManager.newLine("Nothing to fire. ");
        return 0;
    }
    look_pos = autoTarget();
    
    bool keys[256] = { 0 };
    Look(keys, true);

    WorldBuilder::textManager.newLine("Use direction keys[dir], [space] to target, [x] to cancel. ");
    return 1;
}

int UserCommand::ThrowTarget(eAction action)
{
    WorldBuilder::textManager.clear();

    UnLook();
    // set auto target

    DungeonLevel & level = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];
    monsterData * monster = WorldBuilder::monsterManager.FindMonsterData(level.map[look_pos.x][look_pos.y].GetMonster());

    if (monster)
        lastTarget = monster;
    //else
    //    lastTarget = NULL;

     return WorldBuilder::actionManager.monsterAction.ThrowTarget(WorldBuilder::monsterManager.Player(), look_pos.x, look_pos.y, action);
}

int UserCommand::Flee()
{
    monsterData * player = WorldBuilder::monsterManager.Player();
    if (player->fleeing == true)
    {
        WorldBuilder::textManager.newLine("You are already fleeing. ");
        return 0;
    }

    WorldBuilder::textManager.newLine("Flee which direction[dir], [x] to cancel. ");
    return 1;
}

int UserCommand::CastSpellAtTarget()
{
    UnLook();
    return //WorldBuilder::actionManager.monsterAction.ThrowTarget(WorldBuilder::monsterManager.Player(),look_pos.x,look_pos.y);
        WorldBuilder::spellManager.CastCurrentSpell(WorldBuilder::monsterManager.Player(), look_pos.x, look_pos.y);
}

int UserCommand::ClimbStairs(int dir)
{
    int ret = 0;
    try{
        LevelChange change;
        ret = change.ChangeLevel(dir, NULL);

        int cur_level = WorldBuilder::GetCurrentLevel();

        if (cur_level == 1)
            WorldBuilder::deathMessage.done.entered = 1;
        if (cur_level == 11)
            WorldBuilder::deathMessage.done.barracks = 1;
        if (cur_level == 5)
            WorldBuilder::deathMessage.done.river = 1;
        if (cur_level == 6)
            WorldBuilder::deathMessage.done.undead = 1;
        if (cur_level == 8)
            WorldBuilder::deathMessage.done.maze = 1;
    }
    catch (const std::exception & ex)
    {
        MessageBox(NULL, ex.what(), "Climb Stairs", MB_ICONERROR);
    }
    return ret;
}

int UserCommand::UnLook()
{
    DungeonLevel *level = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];
    level->map[look_pos.x][look_pos.y].show_target = 0;
    return 1;
}

int UserCommand::Look(bool *keys, bool show_path)
{
    DungeonLevel *level = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];
    UnLook();

    if (keys[VC_X] || keys[VK_ESC])
    {
        keys[VC_X] = false;
        keys[VK_ESC] = false;
        return 0;
    }

    if (!show_path && keys[VK_M] && level->map[look_pos.x][look_pos.y].GetMonster())
    {
        WorldBuilder::SetState(sLookMore);
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
                monsterData * player = WorldBuilder::monsterManager.Player();
                int p_x = player->pos.x;
                int p_y = player->pos.y;
                DungeonLevel *lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

                lev->HighLightPath(p_x, p_y, look_pos.x, look_pos.y);
            }
            level->map[look_pos.x][look_pos.y].show_target = 1; return 1;
        }

        if (look_pos.x < 0) look_pos.x = 0;
        if (look_pos.x > DUNGEON_SIZE_W - 1) look_pos.x = DUNGEON_SIZE_W - 1;
        if (look_pos.y < 0) look_pos.y = 0;
        if (look_pos.y > DUNGEON_SIZE_H - 1) look_pos.y = DUNGEON_SIZE_H - 1;

        if (WorldBuilder::State() == sThrow || WorldBuilder::State() == sTargetSpell) //throwing not looking
        {
            if (show_path)
            {
                monsterData * player = WorldBuilder::monsterManager.Player();
                int p_x = player->pos.x;
                int p_y = player->pos.y;
                DungeonLevel *lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

                lev->HighLightPath(p_x, p_y, look_pos.x, look_pos.y);
                level->map[look_pos.x][look_pos.y].show_target = 1;

                //WorldBuilder::textManager.newLine("Use direction keys[dir], [space] to target, [x] to cancel. ");
            }
            return 1;
        }
        else
            level->map[look_pos.x][look_pos.y].show_target = 1;
    }

    if (!level->map[look_pos.x][look_pos.y].terrain.found ) //dont show
    {
        WorldBuilder::textManager.newLine("You have not been here yet. ");
    }
    else if (level->map[look_pos.x][look_pos.y].GetMonster() && level->map[look_pos.x][look_pos.y].terrain.light) //display monsters
    {
        monsterData * monster = WorldBuilder::monsterManager.FindMonsterData(level->map[look_pos.x][look_pos.y].GetMonster());

        if (WorldBuilder::State() == sLookMore)
        {
            MonsterInfo info;
            info.ShowMonsterInfo(monster);
            return 2;

        }
        float max_stamina = (float)level->map[look_pos.x][look_pos.y].GetMonster()->MaxStamina();
        float cur_stamina = (float)level->map[look_pos.x][look_pos.y].GetMonster()->stamina;

        int state = WorldBuilder::monsterManager.FindMonsterData(level->map[look_pos.x][look_pos.y].GetMonster())->GetState();

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
            case sentry:	strcpy(status, "It appears asleep"); break;
            case hostile:	strcpy(status, "It is hostile"); break;
            case waking:	strcpy(status, "It looked annoyed"); break;
            case asleep:	strcpy(status, "It is asleep"); break;
            }
        }

        if (monster->isPlayer())
            WorldBuilder::textManager.newLine("This is %s. %s. You are %s. [m]ore.", monster->Name().c_str(), status, health);
        else
            WorldBuilder::textManager.newLine("You see a %s. %s. It looks %s. [m]ore.", monster->Name().c_str(), status, health);
    }
    else if (level->map[look_pos.x][look_pos.y].getItem()) //display items
    {
        if (level->map[look_pos.x][look_pos.y].terrain.light)
        {
            if (level->map[look_pos.x][look_pos.y].getItem()->Stackable() && level->map[look_pos.x][look_pos.y].getItem()->itemNumber[1] > 1)
                WorldBuilder::textManager.newLine("You see some %s.", level->map[look_pos.x][look_pos.y].getItem()->GetName());
            else if (level->map[look_pos.x][look_pos.y].getItem()->identified)
                WorldBuilder::textManager.newLine("You see a %s.", level->map[look_pos.x][look_pos.y].getItem()->GetName());
            else
                WorldBuilder::textManager.newLine("You see an %s.", level->map[look_pos.x][look_pos.y].getItem()->GetName());
        }
        else
            WorldBuilder::textManager.newLine("Last time you looks this was a %s.", level->map[look_pos.x][look_pos.y].getItem()->name);
    }
    else //display terrain
    {
        if (level->map[look_pos.x][look_pos.y].terrain.type == lockedStairs)
        {
            WorldBuilder::textManager.newLine("This is %s (%d).", level->map[look_pos.x][look_pos.y].terrain.name,
                WorldBuilder::itemManager.keyLabels[WorldBuilder::GetCurrentLevel()]);
        }
        else if (level->map[look_pos.x][look_pos.y].terrain.type == specialLocked)
        {
            WorldBuilder::textManager.newLine("This is %s (%d).", level->map[look_pos.x][look_pos.y].terrain.name,
                WorldBuilder::itemManager.KeySpecial[WorldBuilder::GetCurrentLevel()]);
        }
        else
            WorldBuilder::textManager.newLine("This is %s.", level->map[look_pos.x][look_pos.y].terrain.name);
    }

    return 1;
}

int UserCommand::Look()
{
    WorldBuilder::textManager.newLine("Use direction keys to look[dir]. [x] to cancel. ");

    monsterData * player = WorldBuilder::monsterManager.Player();

    coord * pos = player->getPosition();

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
    monsterData * player = WorldBuilder::monsterManager.Player();


    if (player->fleeing == true)
    {
        WorldBuilder::textManager.newLine("You are already fleeing. ");
        return 0;
    }

    coord * pos = player->getPosition();
    coord new_pos;
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
    if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.type != stone)
    {
        //check if occupied
        if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_pos.x][new_pos.y].getMonster())
        {
            WorldBuilder::textManager.newLine("You cant flee that way. ");

            return 0;
        }

        if (player->NextAction(WorldBuilder::actionManager.UpdateAction(&player->action, aMove, new_pos.x, new_pos.y)))
        {
            //FleeTest
            if (Random::getInt(7, 1) + Random::getInt(7, 1) > player->Luck())
            {
                WorldBuilder::textManager.newLine("You fail to flee. ");
                return 1;
            }

            player->fleeing = true;
            player->flee_count += 10;
            WorldBuilder::textManager.newLine("You flee. ");

            player->monster.color1 = 255;
            player->monster.color2 = 128;
            player->monster.color3 = 64;

            player->luck_counter += 1000;//Random::getInt(1000,800);
            if (player->Luck() > 5) //5 is luck min
                player->luck_penalty--;

            return 0; //return 0 to get a free movement turn.
        }
    }
    else
        WorldBuilder::textManager.newLine("You cant flee that way. ");

    return 0;
}

int UserCommand::MoveCommand(int  dir)
{
    monsterData * player = WorldBuilder::monsterManager.Player();

    coord * pos = player->getPosition();
    coord new_pos;
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

    if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.type != stone)
    {

        if (dir != dWait)
            player->NextAction(WorldBuilder::actionManager.UpdateAction(&player->action, aMove, new_pos.x, new_pos.y));
        else
            player->NextAction(WorldBuilder::actionManager.UpdateAction(&player->action, aWait));

        //move over item mesage 
        if (dir != dWait  && WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[player->getPosition()->x][player->getPosition()->y].getItem())
        {
            if (player->action.Type() == aMove)
            {
                DungeonLevel* level = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];
                if (level->map[player->getPosition()->x][player->getPosition()->y].getItem()->itemNumber[1] > 1)
                    WorldBuilder::textManager.newLine("You see some %s.", level->map[player->getPosition()->x][player->getPosition()->y].getItem()->GetName());
                else if (level->map[player->getPosition()->x][player->getPosition()->y].getItem()->identified)
                    WorldBuilder::textManager.newLine("You see an %s here. ", level->map[player->getPosition()->x][player->getPosition()->y].getItem()->GetName());
                else
                    WorldBuilder::textManager.newLine("You see an %s here. ", level->map[player->getPosition()->x][player->getPosition()->y].getItem()->GetName());
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
        WorldBuilder::textManager.newLine("Invalid direction! Close What?[dir] - [x] to cancel. ");
        return -1;
    }

    monsterData * player = WorldBuilder::monsterManager.Player();

    coord * pos = player->getPosition();
    coord new_pos;
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

    if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.type == openDoor) //open door
    {

        if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_pos.x][new_pos.y].getItem() ||
            WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_pos.x][new_pos.y].GetMonster())
        {
            WorldBuilder::textManager.newLine("The door is blocked. ");
            return 0;
        }

        WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[new_pos.x][new_pos.y].terrain.Create(closedDoor);//makeclosedDoor();
        WorldBuilder::textManager.newLine("You close the door. ");
        return 1;
    }


    WorldBuilder::textManager.newLine("There is nothing there to close. ");
    return 0;
}

int UserCommand::InitRun()
{

    WorldBuilder::textManager.newLine("Run which direction[dir]?. [x] to cancel. ");

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

    monsterData *player = WorldBuilder::monsterManager.Player();

    coord * pos = player->getPosition();
    coord new_pos;
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


    DungeonLevel *level = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

    if (run_dir == dNone)
        return 2;

    for (MONSTERLIST::iterator it = WorldBuilder::monsterManager.monster_list.begin(); it != WorldBuilder::monsterManager.monster_list.end(); it++)
    { //is this too slow? seems OK to me
        if (it->level == WorldBuilder::GetCurrentLevel() && !it->isPlayer())
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

    DungeonLevel *level = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

    if (level->map[x][y].GetMonster() == NULL)

        return 0;
    return 1;

}
int UserCommand::HaltRunFeature(int x, int y, int oldx, int oldy)
{
    if (x == oldx && y == oldy)
        return 0;

    DungeonLevel *level = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

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
    WorldBuilder::textManager.ClearDisplayLines();

    WorldBuilder::textManager.SetDisplayLine(0, "The Warlock of Firetop Mountain Roguelike (c) 2014 Corremn");
    WorldBuilder::textManager.SetDisplayLine(1, "==========================================================");
    WorldBuilder::textManager.SetDisplayLine(3, "http://sites.google.com/site/corremn/");
    WorldBuilder::textManager.SetDisplayLine(4, "mailto:corremn@gmail.com");

    WorldBuilder::textManager.SetDisplayLine(5, "This game was originally created for the 2007 seven-day roguelike competition. ");
    WorldBuilder::textManager.SetDisplayLine(5, "See changelog.txt for recent developments. ");

    WorldBuilder::textManager.SetDisplayLine(9,  "You are seeking the Warlock Zagor's Treasure. In your way are the inhabitants");
    WorldBuilder::textManager.SetDisplayLine(10, "of the mountain, underground rivers, locked gates and of course the warlock himself.");
    WorldBuilder::textManager.SetDisplayLine(11, "Of course you will find plenty of items within the mountain to help you in your quest.");

    WorldBuilder::textManager.SetDisplayLine(14, "HINTS");
    WorldBuilder::textManager.SetDisplayLine(15, "Skill represents your skill with the current weapon, Stamina is your hit points.");
    WorldBuilder::textManager.SetDisplayLine(16, "Luck helps you escape damage and find better items, especially from chests. Higher the better.");
    WorldBuilder::textManager.SetDisplayLine(17, "Armour quality is based on the AC value [x]. The higher the better. ");
    WorldBuilder::textManager.SetDisplayLine(18, "Most damage taken is 1-2 points. Shields can reduce damage from hits. ");
    WorldBuilder::textManager.SetDisplayLine(19, "You can attempt to flee combat at the cost of a luck point.");

    WorldBuilder::textManager.SetDisplayLine(20, "Creatures are weaker in water.");
    WorldBuilder::textManager.SetDisplayLine(21, "Locked gates and chests can be opened with the correct key.");
    WorldBuilder::textManager.SetDisplayLine(22, "To unlock something use the [u]se command or enter it.");
    WorldBuilder::textManager.SetDisplayLine(23, "The same key may have more than one use, so dont throw them away.");
    WorldBuilder::textManager.SetDisplayLine(24, "To access Zagor's treasure you will need multiple keys.");
    WorldBuilder::textManager.SetDisplayLine(25, "Archers are more accurate over short distances.");
    //WorldBuilder::textManager.SetDisplayLine(25, "Resting between levels can be dangerous.");
    WorldBuilder::textManager.SetDisplayLine(26, "Items will change to blue if it is better than your current one.");
    WorldBuilder::textManager.SetDisplayLine(29, "Fountains need to be [u]sed.");

    WorldBuilder::textManager.SetDisplayLine(30, "Contact the author to support the continuing development of this game.");

    WorldBuilder::textManager.SetDisplayLine(39, "[k] key bindings, [x] to exit");

    return 1;
}

int UserCommand::DisplayHelp()
{

    WorldBuilder::textManager.ClearDisplayLines();

    WorldBuilder::textManager.SetDisplayLine(0, "Help");
    WorldBuilder::textManager.SetDisplayLine(1, "====");
    WorldBuilder::textManager.SetDisplayLine(2, "");

    WorldBuilder::textManager.SetDisplayLine(3, "Commands");

    WorldBuilder::textManager.SetDisplayLine(5, " move               - [arrow keys] or [numpad]");
    WorldBuilder::textManager.SetDisplayLine(6, " continuous move    - [.] + [dir]");
    WorldBuilder::textManager.SetDisplayLine(7, " wait               - [numpad 5]");
    WorldBuilder::textManager.SetDisplayLine(8, " fire ranged weapon - [numpad f]");
    WorldBuilder::textManager.SetDisplayLine(9, " flee               - [F]");
    WorldBuilder::textManager.SetDisplayLine(10, " pickup             - [g] or [,]");
    WorldBuilder::textManager.SetDisplayLine(11, " look               - [l] + [dir]");
    WorldBuilder::textManager.SetDisplayLine(12, " close              - [c]");
    WorldBuilder::textManager.SetDisplayLine(13, " unlock             - [u] + [dir]");
    WorldBuilder::textManager.SetDisplayLine(14, " use magic          - [z]");
    WorldBuilder::textManager.SetDisplayLine(15, " climb up           - [<]");
    WorldBuilder::textManager.SetDisplayLine(16, " climb down         - [>]");

    WorldBuilder::textManager.SetDisplayLine(18, " message history    - [ctrl-p]");
    WorldBuilder::textManager.SetDisplayLine(19, " view inventory     - [i]");
    WorldBuilder::textManager.SetDisplayLine(20, " view quipment      - [e]");

    WorldBuilder::textManager.SetDisplayLine(21, " toggle Full Screen - [F1]");

#ifdef _DEBUG
    WorldBuilder::textManager.SetDisplayLine(23, "Debug commands");
    WorldBuilder::textManager.SetDisplayLine(24, " God view            - [s]");
    WorldBuilder::textManager.SetDisplayLine(25, " suicide             - [t]");
    WorldBuilder::textManager.SetDisplayLine(26, " Dungeon Level Down  - [1]     Dungeon Level Up  - [2]");
    WorldBuilder::textManager.SetDisplayLine(27, " Max Health          - [3]");
#endif

    WorldBuilder::textManager.SetDisplayLine(39, "[a] about game, [x] to exit");
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
    WorldBuilder::textManager.newLine("What direction[dir]?");
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
    DungeonLevel *dlevel = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];
    monsterData * player = WorldBuilder::monsterManager.Player();

    coord * pos = player->getPosition();
    coord new_pos;
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
    default: WorldBuilder::textManager.newLine("Invalid direction. "); return 0;

    }

    //	if(item->type==key) //unlock
    {
        if (dlevel->map[new_pos.x][new_pos.y].terrain.type == lockedStairs)
        {
            ITEMLIST::iterator it;
            ITEMLIST *inventory = &WorldBuilder::monsterManager.Player()->inventory;
            int level = WorldBuilder::GetCurrentLevel();

            char buf[64];
            sprintf(buf, "key labelled %d", WorldBuilder::itemManager.keyLabels[level]);

            int match = 0;

            for (it = inventory->begin(); it != inventory->end(); it++)
            {
                if (!strcmp(it->name, buf))
                    match = 1;
            }
            if (match == 0)
            {
                WorldBuilder::textManager.newLine("You do not have the correct key. ");
                return 1;
            }
            else
            {
                WorldBuilder::textManager.newLine("You unlock the gate. ");
                dlevel->map[new_pos.x][new_pos.y].terrain.Create(openStairs);
                return 1;
            }
        }
        else if (dlevel->map[new_pos.x][new_pos.y].terrain.type == specialLocked)
        {
            ITEMLIST::iterator it;
            ITEMLIST *inventory = &WorldBuilder::monsterManager.Player()->inventory;
            int level = WorldBuilder::GetCurrentLevel();

            char buf[64];
            sprintf(buf, "key labelled %d", WorldBuilder::itemManager.KeySpecial[level]);

            int match = 0;

            for (it = inventory->begin(); it != inventory->end(); it++)
            {
                if (!strcmp(it->name, buf))
                    match = 1;
            }
            if (match == 0)
            {
                WorldBuilder::textManager.newLine("You do not have the correct key. ");
                return 0;
            }
            else
            {
                WorldBuilder::textManager.newLine("You unlock the gate. You hear mad laughing. ");
                dlevel->map[new_pos.x][new_pos.y].terrain.Create(specialOpen);
                return 1;
            }
        }
        if (strcmp(dlevel->map[new_pos.x][new_pos.y].terrain.name, "a teleport") == 0)
        {
            WorldBuilder::textManager.newLine("Walk into it! ");
            return 0;
        }
        if (strcmp(dlevel->map[new_pos.x][new_pos.y].terrain.name, "a fountain") == 0)
        {
            player->monster.stamina = player->monster.MaxStamina();

            WorldBuilder::textManager.newLine("You drink from the fountain. You feel healthier. ");
            dlevel->map[new_pos.x][new_pos.y].terrain.Create(dryFountain);

            return 1;
        }
        if (dlevel->map[new_pos.x][new_pos.y].getItem()) //item exists
        {
            if (dlevel->map[new_pos.x][new_pos.y].getItem()->type == lockedChest)
            {
                ITEMLIST::iterator it;
                ITEMLIST *inventory = &WorldBuilder::monsterManager.Player()->inventory;

                int match = 0;

                if (WorldBuilder::GetCurrentLevel() != 19)
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
                        dlevel->map[new_pos.x][new_pos.y].getItem()->CreateItem(openChest, WorldBuilder::GetCurrentLevel());
                        WorldBuilder::textManager.newLine("You open the chest. ");

                        if (WorldBuilder::GetCurrentLevel() > 9) //create special item
                        {
                            int item_boost = 0;
                            if ((Random::getInt(7, 1) + Random::getInt(7, 1)) < player->Luck())
                            {
                                item_boost += 50;
                            }

                            int random_item = Random::getInt(5, 0);
                            Item *new_it;
                            switch (random_item)
                            {
                            case 0: new_it = WorldBuilder::itemManager.CreateItem(20 + item_boost, armour, platemail);
                                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            case 1: new_it = WorldBuilder::itemManager.CreateItem(20 + item_boost, weapon, axe);
                                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            case 2:;
                            case 3: new_it = WorldBuilder::itemManager.CreateItem(20 + item_boost, shield);
                                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            default:  new_it = WorldBuilder::itemManager.CreateRandomItem(WorldBuilder::GetCurrentLevel() + 20 + item_boost);
                                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y);
                                new_it = WorldBuilder::itemManager.CreateRandomItem(WorldBuilder::GetCurrentLevel() + 20 + item_boost);
                                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                                new_it = WorldBuilder::itemManager.CreateRandomItem(WorldBuilder::GetCurrentLevel() + 20 + item_boost);
                                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y); break;
                            }
                        }
                        else
                        {
                            int random_items = Random::getInt(5, 2);

                            for (int i = 0; i < random_items; i++)
                            {
                                int item_boost = 0; //lucky items
                                if ((Random::getInt(7, 1) + Random::getInt(7, 1)) < player->Luck())
                                {
                                    item_boost += 15;

                                    if ((Random::getInt(7, 1) + Random::getInt(7, 1)) < player->Luck())
                                    {
                                        item_boost += 15;
                                    }
                                }

                                Item *new_it = WorldBuilder::itemManager.CreateRandomItem(WorldBuilder::GetCurrentLevel() + item_boost);
                                WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y);
                            }
                        }
                        return 1;
                    }
                    else
                    {
                        WorldBuilder::textManager.newLine("You do not have the correct key. ");
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
                        dlevel->map[new_pos.x][new_pos.y].getItem()->CreateItem(openChest, WorldBuilder::GetCurrentLevel());
                        WorldBuilder::textManager.newLine("Click, Click, Click! You open the chest. <insert trumpet sounds here>");
                        Item *new_it = WorldBuilder::itemManager.CreateItem(WorldBuilder::GetCurrentLevel(), gold);
                        WorldBuilder::monsterManager.monsterItems.AttemptDropItem(NULL, new_it, new_pos.x, new_pos.y);

                        return 1;
                    }
                    else if (match == 2)
                    {
                        WorldBuilder::textManager.newLine("Click, Click, Damn!! You do not have all the keys. ");
                        return 1;
                    }
                    else if (match == 1)
                    {
                        WorldBuilder::textManager.newLine("Click, Umm, Hmmm!! You do not have all the keys. ");
                        return 1;
                    }
                    else
                    {
                        WorldBuilder::textManager.newLine("You do not have any of the correct keys. ");
                        return 1;
                    }

                }

            }
            else if (dlevel->map[new_pos.x][new_pos.y].getItem()->type == openChest)
            {
                WorldBuilder::textManager.newLine("All ready open. ");
                return 0;
            }
            else if (dlevel->map[new_pos.x][new_pos.y].getItem()->type == corpse)
            {
                if (Random::getInt(2, 0))
                    WorldBuilder::textManager.newLine("It says 'Hello my name's Frank, whats yours?'. Come on, its a corpse!");
                else
                    WorldBuilder::textManager.newLine("It appears to be smiling at you! ");
                return 0;
            }
            else //nothing to use
            {
                WorldBuilder::textManager.newLine("Cant use that. ");
                return 0;
            }
        }
    }
    WorldBuilder::textManager.newLine("Nothing to use. ");
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