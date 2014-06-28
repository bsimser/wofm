// RestLevel.cpp: implementation of the RestLevel class.
//
//////////////////////////////////////////////////////////////////////

#include "worldbuilder.h"
#include "numberGenerator.h"
#include "RestLevel.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RestLevel::RestLevel() :
rest_count(0),
encounterFlag(0)
{

    for (int i = 0; i < 4; i++)
        restComplete[i] = 0;
}

RestLevel::~RestLevel()
{

}

void RestLevel::Rest(int level)
{
    WorldBuilder::textManager.ClearDisplayLines();

    WorldBuilder::textManager.SetDisplayLine(0, "Descending into the mountain");
    WorldBuilder::textManager.SetDisplayLine(1, "============================");
    WorldBuilder::textManager.SetDisplayLine(2, "");
    if (level == 1)
    {
        WorldBuilder::textManager.SetDisplayLine(3, "You enter the mountain.");
        WorldBuilder::textManager.SetDisplayLine(5, "You can choose to rest before you continue.");
        restComplete[0] = 1;
    }
    if (level == 4)
    {
        WorldBuilder::textManager.SetDisplayLine(3, "You cautiously creep down the steps, listening for the slightest sound of");
        WorldBuilder::textManager.SetDisplayLine(4, "trouble. The steps round a bend and hidden in a alcove you see a sign.");
        WorldBuilder::textManager.SetDisplayLine(5, "'Rest here weary traveller'. Below the sign is a comfortable looking chair.");
        WorldBuilder::textManager.SetDisplayLine(6, "You feel drowsy and wish to rest. Do you dare? You hear no signs of trouble,");
        WorldBuilder::textManager.SetDisplayLine(7, "but as always you feel as though you are being watched.");
        WorldBuilder::textManager.SetDisplayLine(9, "You can choose to rest before you continue.");
        restComplete[1] = 1;
    }
    if (level == 7)
    {
        WorldBuilder::textManager.SetDisplayLine(3, "As you descend further into the dungeon a foul stench consumes the air.");
        WorldBuilder::textManager.SetDisplayLine(5, "You can choose to rest before you continue.");
        restComplete[2] = 1;
    }
    if (level == 10)
    {
        WorldBuilder::textManager.SetDisplayLine(3, "You have killed the Warlock all his riches are yours for the taking.");
        WorldBuilder::textManager.SetDisplayLine(4, "Or so you hope");
        WorldBuilder::textManager.SetDisplayLine(6, "You can choose to rest before you continue.");
        restComplete[3] = 1;
    }

    WorldBuilder::textManager.SetDisplayLine(39, "[x] to continue, [r] to rest");

    return;
}

int RestLevel::Resting(int level)
{
    if (rest_count > 10)
        return 0;

    Rest(level);
    WorldBuilder::textManager.SetDisplayLine(9, "Resting.");

    Sleep(1000);

    if (encounterFlag)
    {
        Encounter();
        return 0;
    }

    int encounter = Random::getInt(25, 0); //5% for 10 turns
    if (encounter == 25 && level != 1) //no encounter on level 1 
    {
        int message = Random::getInt(5, 0);
        switch (message)
        { 
        case 0:WorldBuilder::textManager.SetDisplayLine(11,  "You hear shuffling sounds."); break;
        case 1:WorldBuilder::textManager.SetDisplayLine(11,  "Something or someone is watching you."); break;
        case 2:WorldBuilder::textManager.SetDisplayLine(11,  "Something emerges from the darkness. "); break;
        default:WorldBuilder::textManager.SetDisplayLine(11, "Something has disturbed your rest."); break;
        }

        encounterFlag = 1;
    }

    rest_count++;

    sleep.append("z");
    WorldBuilder::textManager.SetDisplayLine(9, (char *)sleep.c_str());

    monsterData* player = WorldBuilder::monsterManager.Player();

    if (player->monster.stamina != player->monster.MaxStamina())// && rest_count%3 ==0)
    {
        player->monster.stamina++;

    }
    else if (rest_count == 10 && player->monster.stamina != player->monster.MaxStamina())
        player->monster.stamina++;

    player->flee_count = 0;
    player->luck_counter -= 150;

    WorldBuilder::textManager.SetDisplayLine(39, "Resting");

    return 1;
}

void RestLevel::Encounter()
{
    monsterData * player = WorldBuilder::monsterManager.Player();
    coord * pos = player->getPosition();
    DungeonLevel * current_lev = &WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()];

    old_level = WorldBuilder::GetCurrentLevel();
    //Set to 

    int new_level = WorldBuilder::DownEncounterLevel(); //level 20

    WorldBuilder::dungeonManager.CreateDungeon(20, 20); //encounter dungeon
    WorldBuilder::dungeonManager.PopulateDungeon(20);

    //get gate position make sure it is open
    coord *pos1 = WorldBuilder::dungeonManager.level[20].FreeTerrainPosition(lockedStairs);

    if (WorldBuilder::dungeonManager.level[20].map[pos1->x][pos1->y].terrain.type == lockedStairs)
        WorldBuilder::dungeonManager.level[20].map[pos1->x][pos1->y].terrain.Create(openStairs);

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
}
