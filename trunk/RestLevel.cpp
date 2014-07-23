// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

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

void RestLevel::Rest(int level)
{
    World.getTextManager().ClearDisplayLines();

    World.getTextManager().SetDisplayLine(0, "Descending into the mountain");
    World.getTextManager().SetDisplayLine(1, "============================");
    World.getTextManager().SetDisplayLine(2, "");
    
    if (level == 1)
    {
        World.getTextManager().SetDisplayLine(3, "Peering into the gloom you see dark, slimy walls with pools of water on the stone floor.");
        World.getTextManager().SetDisplayLine(4, "The air is cold and dank. You light your lantern and step warily into the blackness.");
        World.getTextManager().SetDisplayLine(5, "Far off you hear the scurrying of tiny feet: rats, most likely.");
        World.getTextManager().SetDisplayLine(7, "You can choose to rest before you continue.");
        restComplete[0] = 1;
    }
    if (level == 4)
    {
        World.getTextManager().SetDisplayLine(3, "You cautiously creep down the steps, listening for the slightest sound of");
        World.getTextManager().SetDisplayLine(4, "trouble. The steps round a bend and hidden in a alcove you see a sign.");
        World.getTextManager().SetDisplayLine(5, "'Rest Ye Here Weary Traveller'. Below the sign is a comfortable looking chair.");
        World.getTextManager().SetDisplayLine(6, "You feel drowsy and wish to rest. Do you dare? You hear no signs of trouble,");
        World.getTextManager().SetDisplayLine(7, "but as always you feel as though you are being watched.");
        World.getTextManager().SetDisplayLine(9, "You can choose to rest before you continue.");
        restComplete[1] = 1;
    }
    if (level == 7)
    {
        World.getTextManager().SetDisplayLine(3, "As you descend further into the dungeon a foul stench consumes the air.");
        World.getTextManager().SetDisplayLine(5, "You can choose to rest before you continue.");
        restComplete[2] = 1;
    }
    if (level == 10)
    {
        World.getTextManager().SetDisplayLine(3, "You have killed the Warlock all his riches are yours for the taking.");
        World.getTextManager().SetDisplayLine(4, "Or so you hope");
        World.getTextManager().SetDisplayLine(6, "You can choose to rest before you continue.");
        restComplete[3] = 1;
    }

    World.getTextManager().SetDisplayLine(39, "[x] to continue, [r] to rest");

    return;
}

int RestLevel::Resting(int level)
{
    if (rest_count > 10)
        return 0;

    MonsterData* player = World.getMonsterManager().Player();

    if (player->monster.stamina == player->monster.MaxStamina() && player->luck_penalty >= 0)
        return 0;

    Rest(level);
    World.getTextManager().SetDisplayLine(9, "Resting.");

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
        case 0:World.getTextManager().SetDisplayLine(11,  "You hear shuffling sounds."); break;
        case 1:World.getTextManager().SetDisplayLine(11,  "Something or someone is watching you."); break;
        case 2:World.getTextManager().SetDisplayLine(11,  "Something emerges from the darkness. "); break;
        default:World.getTextManager().SetDisplayLine(11, "Something has disturbed your rest."); break;
        }

        encounterFlag = 1;
    }

    rest_count++;

    sleep.append("z");
    World.getTextManager().SetDisplayLine(9, (char *)sleep.c_str());

    if (player->monster.stamina != player->monster.MaxStamina())
    {
        player->monster.stamina++;
    }
    else if (rest_count == 10 && player->monster.stamina != player->monster.MaxStamina())
        player->monster.stamina++;

    if (player->luck_penalty < 0 && (rest_count % 2) == 0)
    {
        player->luck_penalty++;
    }

    player->flee_count = 0;

    World.getTextManager().SetDisplayLine(39, "Resting");

    return 1;
}

void RestLevel::Encounter()
{
    MonsterData * player = World.getMonsterManager().Player();
    Coord * pos = player->getPosition();
    DungeonLevel * current_lev = &World.getDungeonManager().level(World.GetCurrentLevel());

    old_level = World.GetCurrentLevel();
    //Set to 

    int new_level = World.DownEncounterLevel(); //level 20

    World.getDungeonManager().CreateDungeon(20, 20); //encounter dungeon
    World.getDungeonManager().PopulateDungeon(20);

    //get gate position make sure it is open
    Coord *pos1 = World.getDungeonManager().level(20).FreeTerrainPosition(lockedStairs);

    if (World.getDungeonManager().level(20).getCell(pos1->x, pos1->y).terrain.type == lockedStairs)
        World.getDungeonManager().level(20).getCell(pos1->x, pos1->y).terrain.Create(openStairs);

    DungeonLevel *new_lev = &World.getDungeonManager().level(World.GetCurrentLevel());

    if (!new_lev)
        throw std::exception("invalid level move, down command");

    //remove reference to player on current level map
    current_lev->getCell(pos->x, pos->y).RemoveMonsterRef();

    //get start Coords on new map
    Coord* start_pos = new_lev->getStartPos();

    //place player on new map
    new_lev->getCell(start_pos->x, start_pos->y).AssignMonster(&player->monster);

    player->pos.x = start_pos->x;
    player->pos.y = start_pos->y;
}
