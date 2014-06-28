// LevelChange.cpp: implementation of the LevelChange class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "LevelChange.h"
#include "UserCommand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

LevelChange::LevelChange()
{
}

LevelChange::~LevelChange()
{
}

int LevelChange::ClimbSpecial(int dir)
{

    monsterData* player = WorldBuilder::monsterManager.Player();
    coord * pos = player->getPosition();
    DungeonLevel *current_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

    if (dir == dUp && current_lev->map[pos->x][pos->y].terrain.type == specialUp)
    {
        WorldBuilder::UpSpecialLevel(); //up 10 levels

        DungeonLevel *new_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

        if (!new_lev)
            throw std::exception("invalid level move, up command");

        //remove reference to player on current level map
        current_lev->map[pos->x][pos->y].RemoveMonsterRef();

        //get start coords on new map
        coord* start_pos = new_lev->FreeTerrainPosition(specialOpen);

        //place player on new map
        new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);
        player->pos.x = start_pos->x;
        player->pos.y = start_pos->y;
        player->level = WorldBuilder::GetCurrentLevel();
        return 1;
    }
    else if (dir == dDown)
    {
        if (current_lev->map[pos->x][pos->y].terrain.type == specialLocked) //move down
        {
            UserCommand uc;
            uc.UseItem(NULL, dWait);

            if (current_lev->map[pos->x][pos->y].terrain.type == specialLocked) //move down
            {
                WorldBuilder::textManager.newLine("The gate is locked. ");
                return 0;
            }
            //The below code is an automatic door open code - left incase someone wants it
            /*//check if has key
            {

            ITEMLIST::iterator it;
            ITEMLIST *inventory = &WorldBuilder::monsterManager.Player()->inventory;
            int level = WorldBuilder::GetCurrentLevel();

            char buf[64];

            sprintf(buf,"key labelled %d", WorldBuilder::itemManager.KeySpecial[level]);

            int match = 0;

            for(it=inventory->begin();it != inventory->end();it++)
            {
            if(!strcmp(it->name,buf))
            match = 1;
            }

            if(match == 0)
            {
            WorldBuilder::textManager.newLine("The gate is locked.");
            return 0;
            }
            else
            {
            WorldBuilder::textManager.newLine("You unlock the gate and you decend into the darkness. ");
            current_lev->map[pos->x][pos->y].terrain.Create(specialOpen);
            }

            }
            WorldBuilder::DownSpecialLevel();

            int new_level = WorldBuilder::GetCurrentLevel();

            if(new_level>WorldBuilder::GetMaxLevels() -1)
            {
            WorldBuilder::textManager.newLine("Hmm, this goes nowhere. ");
            return 0;
            }

            DungeonLevel *new_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

            if(!new_lev)
            throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start coords on new map
            coord* start_pos = new_lev->getStartPos();

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            return 1;	*/
        }
        if (current_lev->map[pos->x][pos->y].terrain.type == specialOpen)
        {
            WorldBuilder::DownSpecialLevel();

            int new_level = WorldBuilder::GetCurrentLevel();

            DungeonLevel *new_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start coords on new map
            coord* start_pos = new_lev->FreeTerrainPosition(specialUp);//new_lev->getSpecialStartPos();
            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            player->level = WorldBuilder::GetCurrentLevel();
            return 1;
        }
    }
    return 0;
}

int LevelChange::ClimbNormal(int dir)
{
    if (dir == dUp)
    {
        monsterData* player = WorldBuilder::monsterManager.Player();

        coord * pos = player->getPosition();

        DungeonLevel *current_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

        if (current_lev->map[pos->x][pos->y].terrain.type == upStairs) //move up
        {
            WorldBuilder::UpLevel();

            if (WorldBuilder::GetCurrentLevel() == -1) //escape dungeon
            {
                WorldBuilder::textManager.newLine("A voice says: \"Dungeon exit under construction. Sorry for any inconvenience.\"");
                WorldBuilder::DownLevel();
                //WorldBuilder::monsterManager.PlayerEscape();
                return 2;
            }

            DungeonLevel *new_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, up command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start coords on new map
            coord* start_pos = new_lev->FreeTerrainPosition(openStairs);

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);
            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            return 1;
        }
    }
    if (dir == dDown)
    {
        monsterData* player = WorldBuilder::monsterManager.Player();

        coord * pos = player->getPosition();

        DungeonLevel *current_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

        if (current_lev->map[pos->x][pos->y].terrain.type == lockedStairs) //locked
        {
            UserCommand uc;
            uc.UseItem(NULL, dWait);

            if (current_lev->map[pos->x][pos->y].terrain.type == lockedStairs) //locked)
            {
                WorldBuilder::textManager.newLine("The gate is locked. ");
                return 0;
            }
        }
        if (current_lev->map[pos->x][pos->y].terrain.type == openStairs)//move down
        {
            WorldBuilder::DownLevel();

            int new_level = WorldBuilder::GetCurrentLevel();

            if (new_level == 10)
            {
                WorldBuilder::textManager.newLine("Hmm, this goes nowhere. ");
                WorldBuilder::UpLevel();
                return 0;
            }

            DungeonLevel *new_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start coords on new map
            coord* start_pos = new_lev->getStartPos();

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            player->level = WorldBuilder::GetCurrentLevel();
            return 1;
        }
    }
    return 0;
}

int LevelChange::ClimbEncounter(int dir)
{

    if (dir == dUp) //cannot escape
    {
        WorldBuilder::textManager.newLine("An unseen force stops you from escaping back up. You hear distant laughing. ");
        return 1;

    }
    if (dir == dDown)
    {
        monsterData* player = WorldBuilder::monsterManager.Player();

        coord * pos = player->getPosition();

        DungeonLevel *current_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

        if (current_lev->map[pos->x][pos->y].terrain.type == lockedStairs) //locked
        {
            WorldBuilder::textManager.newLine("The gate is locked. ");
            return 0;
        }
        else if (current_lev->map[pos->x][pos->y].terrain.type == openStairs)//move down
        {
            WorldBuilder::SetDungeonLevel(WorldBuilder::restLevel.GetOldLevel());

            int new_level = WorldBuilder::GetCurrentLevel();

            DungeonLevel *new_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start coords on new map
            coord* start_pos = new_lev->getStartPos();

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            player->level = WorldBuilder::GetCurrentLevel();
            return 1;
        }
    }
    return 0;
}

int LevelChange::ChangeLevel(int dir, monsterData * monster)
{
    DungeonLevel *current_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];
    monsterData* player = WorldBuilder::monsterManager.Player();
    coord * pos = player->getPosition();

    if (WorldBuilder::GetCurrentLevel() == 20) //encounter level
    {
        if (current_lev->map[pos->x][pos->y].terrain.type == upStairs ||
            current_lev->map[pos->x][pos->y].terrain.type == lockedStairs ||
            current_lev->map[pos->x][pos->y].terrain.type == openStairs)
            return ClimbEncounter(dir);
        else
            return 0;
    }

    //special level
    if (current_lev->map[pos->x][pos->y].terrain.type == specialUp
        || current_lev->map[pos->x][pos->y].terrain.type == specialLocked
        || current_lev->map[pos->x][pos->y].terrain.type == specialOpen)
        return ClimbSpecial(dir);

    //normal
    else if (current_lev->map[pos->x][pos->y].terrain.type == upStairs
        || current_lev->map[pos->x][pos->y].terrain.type == lockedStairs
        || current_lev->map[pos->x][pos->y].terrain.type == openStairs)
    {
        if (ClimbNormal(dir))
        {
            int level = WorldBuilder::GetCurrentLevel();
            if ((level == 1 && !WorldBuilder::restLevel.restComplete[0])
                || (level == 4 && !WorldBuilder::restLevel.restComplete[1])
                || (level == 7 && !WorldBuilder::restLevel.restComplete[2])
                || (level == 10 && !WorldBuilder::restLevel.restComplete[3]))
            {
                WorldBuilder::restLevel.Rest(level);
                WorldBuilder::SetState(sRest);
            }
            return 1;
        }
        else
            return 0;

    }
    else return 0;
}

void LevelChange::DebugLevel(int dir)
{

    DungeonLevel *current_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];
    monsterData* player = WorldBuilder::monsterManager.Player();
    coord * pos = player->getPosition();

    if (dir == 1)
        WorldBuilder::DownLevel();
    else if (dir == 2)
        WorldBuilder::UpLevel();
    else return;


    int new_level = WorldBuilder::GetCurrentLevel();


    if (new_level == 20)
    {
        WorldBuilder::textManager.newLine("Hmm, this goes nowhere. ");
        WorldBuilder::UpLevel();
        return;
    }

    DungeonLevel *new_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

    if (!new_lev)
        throw std::exception("invalid level move, down command");

    //remove reference to player on current level map
    current_lev->map[pos->x][pos->y].RemoveMonsterRef();

    //get start coords on new map
    coord* start_pos;

    if (new_level < 10)
    {
        if (dir == 1)
            start_pos = new_lev->getStartPos();
        else
        {
            start_pos = new_lev->FreeTerrainPosition(openStairs);
            if (start_pos == NULL)
                start_pos = new_lev->FreeTerrainPosition(lockedStairs);
        }

    }
    else if (new_level < 20)
    {
        if (dir == 1)
            start_pos = new_lev->FreeTerrainPosition(specialUp);
        else
            start_pos = new_lev->FreeTerrainPosition(specialUp);

        if (start_pos == NULL)
        {
            return DebugLevel(dir);
        }

    }
    else return;
    //place player on new map
    new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

    player->pos.x = start_pos->x;
    player->pos.y = start_pos->y;
}
