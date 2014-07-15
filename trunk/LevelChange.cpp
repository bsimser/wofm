// LevelChange.cpp: implementation of the LevelChange class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "LevelChange.h"
#include "UserCommand.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int LevelChange::ClimbSpecial(int dir)
{

    MonsterData* player = World.getMonsterManager().Player();
    Coord * pos = player->getPosition();
    DungeonLevel *current_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

    if (dir == dUp && current_lev->map[pos->x][pos->y].terrain.type == specialUp)
    {
        World.UpSpecialLevel(); //up 10 levels

        DungeonLevel *new_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

        if (!new_lev)
            throw std::exception("invalid level move, up command");

        //remove reference to player on current level map
        current_lev->map[pos->x][pos->y].RemoveMonsterRef();

        //get start Coords on new map
        Coord* start_pos = new_lev->FreeTerrainPosition(specialOpen);

        //place player on new map
        new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);
        player->pos.x = start_pos->x;
        player->pos.y = start_pos->y;
        player->level = World.GetCurrentLevel();
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
                World.getTextManager().newLine("The gate is locked. ");
                return 0;
            }
            //The below code is an automatic door open code - left incase someone wants it
            /*//check if has key
            {

            ITEMLIST::iterator it;
            ITEMLIST *inventory = &World.getMonsterManager().Player()->inventory;
            int level = World.GetCurrentLevel();

            char buf[64];

            sprintf(buf,"key labelled %d", World.getItemManager().KeySpecial[level]);

            int match = 0;

            for(it=inventory->begin();it != inventory->end();it++)
            {
            if(!strcmp(it->name,buf))
            match = 1;
            }

            if(match == 0)
            {
            World.getTextManager().newLine("The gate is locked.");
            return 0;
            }
            else
            {
            World.getTextManager().newLine("You unlock the gate and you decend into the darkness. ");
            current_lev->map[pos->x][pos->y].terrain.Create(specialOpen);
            }

            }
            World.DownSpecialLevel();

            int new_level = World.GetCurrentLevel();

            if(new_level>World.GetMaxLevels() -1)
            {
            World.getTextManager().newLine("Hmm, this goes nowhere. ");
            return 0;
            }

            DungeonLevel *new_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

            if(!new_lev)
            throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start Coords on new map
            Coord* start_pos = new_lev->getStartPos();

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            return 1;	*/
        }
        if (current_lev->map[pos->x][pos->y].terrain.type == specialOpen)
        {
            World.DownSpecialLevel();

            int new_level = World.GetCurrentLevel();

            DungeonLevel *new_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start Coords on new map
            Coord* start_pos = new_lev->FreeTerrainPosition(specialUp);//new_lev->getSpecialStartPos();
            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            player->level = World.GetCurrentLevel();
            return 1;
        }
    }
    return 0;
}

int LevelChange::ClimbNormal(int dir)
{
    if (dir == dUp)
    {
        MonsterData* player = World.getMonsterManager().Player();

        Coord * pos = player->getPosition();

        DungeonLevel *current_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

        if (current_lev->map[pos->x][pos->y].terrain.type == upStairs) //move up
        {
            World.UpLevel();

            if (World.GetCurrentLevel() == -1) //escape dungeon
            {
                World.getTextManager().newLine("A voice says: \"Dungeon exit under construction. Sorry for any inconvenience.\"");
                World.DownLevel();
                //World.getMonsterManager().PlayerEscape();
                return 2;
            }

            DungeonLevel *new_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, up command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start Coords on new map
            Coord* start_pos = new_lev->FreeTerrainPosition(openStairs);

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);
            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            player->level = World.GetCurrentLevel();

            return 1;
        }
    }
    if (dir == dDown)
    {
        MonsterData* player = World.getMonsterManager().Player();

        Coord * pos = player->getPosition();

        DungeonLevel *current_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

        if (current_lev->map[pos->x][pos->y].terrain.type == lockedStairs) //locked
        {
            UserCommand uc;
            uc.UseItem(NULL, dWait);

            if (current_lev->map[pos->x][pos->y].terrain.type == lockedStairs) //locked)
            {
                World.getTextManager().newLine("The gate is locked. ");
                return 0;
            }
        }
        if (current_lev->map[pos->x][pos->y].terrain.type == openStairs)//move down
        {
            World.DownLevel();

            int new_level = World.GetCurrentLevel();

            if (new_level == 10)
            {
                World.getTextManager().newLine("Hmm, this goes nowhere. ");
                World.UpLevel();
                return 0;
            }

            DungeonLevel *new_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start Coords on new map
            Coord* start_pos = new_lev->getStartPos();

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            player->level = World.GetCurrentLevel();
            return 1;
        }
    }
    return 0;
}

int LevelChange::ClimbEncounter(int dir)
{

    if (dir == dUp) //cannot escape
    {
        World.getTextManager().newLine("An unseen force stops you from escaping back up. You hear distant laughing. ");
        return 1;

    }
    if (dir == dDown)
    {
        MonsterData* player = World.getMonsterManager().Player();

        Coord * pos = player->getPosition();

        DungeonLevel *current_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

        if (current_lev->map[pos->x][pos->y].terrain.type == lockedStairs) //locked
        {
            World.getTextManager().newLine("The gate is locked. ");
            return 0;
        }
        else if (current_lev->map[pos->x][pos->y].terrain.type == openStairs)//move down
        {
            World.SetDungeonLevel(World.getRestLevel().GetOldLevel());

            int new_level = World.GetCurrentLevel();

            DungeonLevel *new_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

            if (!new_lev)
                throw std::exception("invalid level move, down command");

            //remove reference to player on current level map
            current_lev->map[pos->x][pos->y].RemoveMonsterRef();

            //get start Coords on new map
            Coord* start_pos = new_lev->getStartPos();

            //place player on new map
            new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

            player->pos.x = start_pos->x;
            player->pos.y = start_pos->y;
            player->level = World.GetCurrentLevel();
            return 1;
        }
    }
    return 0;
}

int LevelChange::ChangeLevel(int dir, MonsterData * monster)
{
    DungeonLevel *current_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];
    MonsterData* player = World.getMonsterManager().Player();
    Coord * pos = player->getPosition();

    if (World.GetCurrentLevel() == 20) //encounter level
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
            int level = World.GetCurrentLevel();
            if ((level == 1 && !World.getRestLevel().restComplete[0])
                || (level == 4 && !World.getRestLevel().restComplete[1])
                || (level == 7 && !World.getRestLevel().restComplete[2])
                || (level == 10 && !World.getRestLevel().restComplete[3]))
            {
                World.getRestLevel().Rest(level);
                World.SetState(sRest);
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
    DungeonLevel *current_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];
    MonsterData* player = World.getMonsterManager().Player();
    Coord * pos = player->getPosition();

    if (dir == 1)
        World.DownLevel();
    else if (dir == 2)
        World.UpLevel();
    else return;

    int new_level = World.GetCurrentLevel();

    if (new_level == 20)
    {
        World.getTextManager().newLine("Hmm, this goes nowhere. ");
        World.UpLevel();
        return;
    }
    else if (new_level < 0)
    {
        World.getTextManager().newLine("Hmm, can't do that. ");
        World.DownLevel();
        return;
    }
    DungeonLevel *new_lev = &World.getDungeonManager().level[World.GetCurrentLevel()];

    if (!new_lev)
        throw std::exception("invalid level move, down command");

    //remove reference to player on current level map
    current_lev->map[pos->x][pos->y].RemoveMonsterRef();

    //get start Coords on new map
    Coord* start_pos;

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
    else 
        return;
    //place player on new map
    new_lev->map[start_pos->x][start_pos->y].AssignMonster(&player->monster);

    player->pos.x = start_pos->x;
    player->pos.y = start_pos->y;
    player->level = World.GetCurrentLevel();
}
