// DungeonManager.cpp: implementation of the DungeonManager class.
//
//////////////////////////////////////////////////////////////////////

#include "DungeonManager.h"

#include "WorldBuilder.h"

#include "numberGenerator.h"
#include <sstream>

using namespace Random;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


int DungeonManager::Initialise()
{
    return 1;
}

int DungeonManager::BuildCompleteDungeon(int max_num_levels)
{
    int i = 0;
    try
    {
        for (i = 0; i < max_num_levels / 2; i++)
        {
            CreateDungeon(i, i);
            PopulateDungeon(i);
        }

    }
    catch (const std::exception & ex)
    {
        std::stringstream err;
        err << ex.what() << " on level: " << i;
        throw std::exception(err.str().c_str());

    }
    catch (...)
    {
        std::stringstream err;
        err << "Unknown exception while creating and populating standard dungeon" << " on level: " << i;
        throw std::exception(err.str().c_str());

    }

    CreateLockedDungeon();

    try{
        CreateSpecialDungeon();
        AddKeysToDungeon();
    }
    catch (const std::exception&)
    {
        throw;
    }
    catch (...)
    {
        std::stringstream err;
        err << "Unknown exception while creating and populating special dungeon: Creating";
        throw std::exception(err.str().c_str());
    }
    return 1;
}

int DungeonManager::CreateDungeon(int dlevel, int type)
{
    level[dlevel].Initialise(type);

    if (dlevel == 8) // minotaur level
        level[dlevel].setMapLight(DungeonLevel::eNoFound);

    return 1;
}

int DungeonManager::DeleteDungeon(int dlevel)
{
    //	level[dlevel].Delete();
    return 1;
}


coord * DungeonManager::NewMonsterPosition(int lev)
{
    try
    {
        coord *newPos = level[lev].FreeTerrainPosition(dfloor);
        if (newPos)
            return newPos;
        throw std::exception("Failed to find free floor position for monster");
    }
    catch (const std::exception & ex)
    {
        std::stringstream err;
        err << ex.what() << " on level: " << lev;
        throw std::exception(err.str().c_str());
    }
}

coord * DungeonManager::NewBridgePosition(int lev)
{
    try
    {
        return level[lev].FreeTerrainPosition(bridge);
    }
    catch (const std::exception & ex)
    {
        std::stringstream err;
        err << ex.what() << " on level: " << lev;
        throw std::exception(err.str().c_str());
    }
}


int DungeonManager::AddItems(int lev)
{
    int nItems = getInt(4 + lev, 2 + lev);

    for (int i = 0; i < nItems; i++)
    {
        coord* pos = level[lev].NewItemPosition();
        //level[lev].map[pos->x][pos->y].getItem() = WorldBuilder::itemManager.CreateRandomItem(lev);
        int item_test = getInt(10, 0);
        if (item_test == 0)
            level[lev].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(lev, lockedChest));
        else if (getInt(10, 0) < 2)
            level[lev].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(lev, openChest));

        else
            level[lev].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(lev, corpse));
    }

    /*	if(lev==1)
        {
        coord* pos = level[lev].NewItemPosition();
        level[lev].map[pos->x][pos->y].item = WorldBuilder::itemManager.CreateItem(lockedChest,lev);
        }*/

    return 1;
}

int DungeonManager::PopulateDungeon(int lev)
{
    int sub_type = 0;
    //add player

    if (!level[lev].initialised)
        throw("Attempting to populate uninitialised level:");

    if (lev == 0) //first level add player
    {
        coord *start = level[lev].getStartPos(); //get starting point (up stairs)
        monsterData* player = WorldBuilder::monsterManager.CreateMonster(mPlayer, sub_type, 0, start->x, start->y);

        WorldBuilder::spellManager.AddMonsterSpell(player, spFlyingWeapon);
        WorldBuilder::spellManager.AddMonsterSpell(player, spRepelMissiles);
        WorldBuilder::spellManager.AddMonsterSpell(player, spSlowEnemies);
        WorldBuilder::spellManager.AddMonsterSpell(player, spTeleport);
        //WorldBuilder::spellManager.AddMonsterSpell(player,spDragonFire);
    }

    //add monsters
#ifndef NO_MONSTERS

    if (lev == 7) //undead level
    {
        int max_monsters = Random::getInt(10, 5);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mUndead;//Random::getInt(1,1);

            coord *pos = NewMonsterPosition(lev); //??
            monsterData *mob = WorldBuilder::monsterManager.CreateMonster(type, Random::getInt(4, 0), lev, pos->x, pos->y);

            	std::string name = mob->Name();
            //	if(name.find("Ghoul") !=-1 )
            //		mob->monster.SetBrand(bFire,3);
        }

        coord *pos = NewMonsterPosition(lev); //??
        WorldBuilder::monsterManager.CreateMonster(mSpecial, sub_type, lev, pos->x, pos->y);
    }

    else if (lev == 0) //start level
    {
        coord *pos = NewBridgePosition(lev); //??
        if (!pos)
            pos = NewMonsterPosition(lev);

        monsterData *gog = WorldBuilder::monsterManager.CreateMonster(mOrc, sub_type, lev, pos->x, pos->y);

        pos = NewBridgePosition(lev); //??
        if (!pos)
            pos = NewMonsterPosition(lev);
        
        gog = WorldBuilder::monsterManager.CreateMonster(mOrc, sub_type, lev, pos->x, pos->y);

        //pos = level[lev].FreeTerrainPosition(openStairs); //??
        pos = level[lev].GetEndPosition();

        WorldBuilder::monsterManager.CreateMonster(mSpecial, sub_type, lev, pos->x, pos->y);
    }

    else if (lev == 20) //encounter level
    {

        int player_level = WorldBuilder::restLevel.GetOldLevel();

        int max_monsters = Random::getInt(3 + player_level, 1);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mOrc;//Random::getInt(1,1);

            coord *pos = NewMonsterPosition(lev); //??
            WorldBuilder::monsterManager.CreateMonster(type, sub_type, lev, pos->x, pos->y);
        }
    }
    else	//standard orc level
    {
        int max_monsters = Random::getInt(10, 5);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mOrc;//Random::getInt(1,1);

            coord *pos = NewMonsterPosition(lev); //??
            WorldBuilder::monsterManager.CreateMonster(type, sub_type, lev, pos->x, pos->y);
        }
        coord *pos = NewMonsterPosition(lev); //??
        if (lev != 9)
            monsterData * special = WorldBuilder::monsterManager.CreateMonster(mSpecial, sub_type, lev, pos->x, pos->y);

    }
    if (lev == 4) //river level
    {
        coord *pos = NewBridgePosition(lev); //??
        if (pos == NULL)
            throw std::exception("no bridge master, Game halted");
        WorldBuilder::monsterManager.CreateMonster(mBridgeMaster, sub_type, lev, pos->x, pos->y);

    }

    if (lev == 9) //end level
    {
        coord *pos = NewMonsterPosition(lev); //??
        monsterData* wiz = WorldBuilder::monsterManager.CreateMonster(mWizard, sub_type, lev, pos->x, pos->y);
        WorldBuilder::spellManager.AddMonsterSpell(wiz, spRepelMissiles);
        WorldBuilder::spellManager.AddMonsterSpell(wiz, spTeleport);
        WorldBuilder::spellManager.AddMonsterSpell(wiz, spFlyingWeapon);
        WorldBuilder::spellManager.AddMonsterSpell(wiz, spSlowEnemies);

        wiz->is_magic = 1;
    }

#endif

#ifndef NO_ITEMS
    //add items
    if (lev != 0 && lev <= 10)
        AddItems(lev);

    if (lev == 0)
    {
        //	coord* pos = level[lev].NewItemPosition();
        //level[lev].map[3][3].AssignItem( WorldBuilder::itemManager.CreateItem(lev,lockedChest));

    }

#endif

    return 1;
}

void DungeonManager::CreateSpecialDungeon()
{
    int special_type1 = 0;
    int special_type2 = 0;
    int special_type3 = 0;

    for (int i = 0; i < 10; i++)
    {
        try
        {
            int test = 0;
            if (i == 0)
            {
                CreateDungeon(i + 10, slBarracks);
                PopulateSpecialDungeon(i + 10, slRandom);

                int h, w;
                do //make special entrance
                {
                    h = getInt(DUNGEON_SIZE_H - 2, 10);
                    w = getInt(DUNGEON_SIZE_W - 2, 2);
                    test++;
                    if (test > 10000)
                        break;

                } while (strcmp(level[i].map[w][h].terrain.name, "dungeon floor"));
                level[i].map[w][h].terrain.Create(specialOpen);

                coord* pos = level[i + 10].NewSpecialItemPosition();
                level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

               // pos = level[i + 10].NewSpecialItemPosition(); //??
                WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
            }
            else if (i == 1 || i == 2 || i == 3)
            {
                if (getInt(1, 1) == 1 && i == 1) //%30
                {
                    CreateDungeon(i + 10, slBarracks);
                    PopulateSpecialDungeon(i + 10, slBarracks);
                    coord *pos = NewMonsterPosition(i);
                    level[i].map[pos->x][pos->y].terrain.Create(specialOpen);
                    special_type1++;

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    //pos = level[i + 10].NewSpecialItemPosition(); //??
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);

                }
                else if (i == 2 && special_type1 == 0)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    coord *pos = NewMonsterPosition(i);
                    level[i].map[pos->x][pos->y].terrain.Create(specialOpen);
                    special_type1++;

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    pos = level[i + 10].NewSpecialItemPosition(); //??
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                else if (i == 2 && special_type1 == 1 && getInt(4, 1) == 2) //%30
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    coord *pos = NewMonsterPosition(i);
                    level[i].map[pos->x][pos->y].terrain.Create(specialOpen);
                    special_type1++;

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    pos = level[i + 10].NewSpecialItemPosition(); //??
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                else if (i == 3 && special_type1 == 1)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    coord *pos = NewMonsterPosition(i); //create entry on normal dungeon
                    level[i].map[pos->x][pos->y].terrain.Create(specialOpen);
                    special_type1++;

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    //pos = level[i+10].NewSpecialItemPosition(); // put guard of chest
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }

            }
            else if (i == 4) // boat house - skeletons
            {
                coord *pos;
                CreateDungeon(i + 10, i + 10);
                PopulateSpecialDungeon(i + 10, slBoatHouse);

                do
                {
                    pos = NewMonsterPosition(i);
                    test++;
                    if (test > 10000)
                        break;
                } while (strcmp(level[i].map[pos->x][pos->y].terrain.name, "sand"));

                level[i].map[pos->x][pos->y].terrain.Create(specialLocked);

                pos = level[i + 10].NewSpecialItemPosition();
                level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                //pos = level[i + 10].NewSpecialItemPosition(); //??
                WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
            }
            else if (i == 5 || i == 6)
            {
                if (i == 5 && getInt(2, 0) == 1)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    //choose one entrances
                    coord *pos = NewMonsterPosition(i);
                    level[i].map[pos->x][pos->y].terrain.Create(specialOpen);
                    special_type2++;

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    pos = level[i + 10].NewSpecialItemPosition(); //??
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                else if (i == 6 && special_type2 == 0)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    //choose one entrances
                    coord *pos = NewMonsterPosition(i + 10);
                    level[i].map[pos->x][pos->y].terrain.Create(specialOpen);

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    pos = level[i + 10].NewSpecialItemPosition(); //??
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
            }
            else if (i == 7 || i == 8)
            {
                if (i == 7 && getInt(2, 0) == 1)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    //choose special undead level
                    coord *pos = NewMonsterPosition(i);
                    level[i].map[pos->x][pos->y].terrain.Create(specialLocked);
                    special_type3++;

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    pos = level[i + 10].NewSpecialItemPosition(); //??
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                else if (i == 8 && special_type3 == 0)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    //choose one entrances
                    coord *pos = NewMonsterPosition(i);
                    level[i].map[pos->x][pos->y].terrain.Create(specialOpen);

                    pos = level[i + 10].NewSpecialItemPosition();
                    level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                    pos = level[i + 10].NewSpecialItemPosition(); //??
                    WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
            }
            else if (i == 9)
            {
                CreateDungeon(i + 10, i + 10);
                //	PopulateSpecialDungeon(i+10,slRandom);

                //create treasure level
                coord *pos = NewMonsterPosition(i);
                level[i].map[pos->x][pos->y].terrain.Create(specialLocked);

                pos = level[i + 10].NewSpecialItemPosition();
                level[i + 10].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(i + 10, lockedChest));

                pos = level[i + 10].NewSpecialItemPosition(); //??
                monsterData *dragon = WorldBuilder::monsterManager.CreateMonster(mGuards, i, i + 10, pos->x, pos->y);

                WorldBuilder::spellManager.AddMonsterSpell(dragon, spDragonFire);

                dragon->is_magic = 1;
            }
        }
        catch (const std::exception & ex)
        {
            std::stringstream err;
            err << ex.what() << " on level: " << 10 + i;
            throw std::exception(err.str().c_str());
        }
        catch (...)
        {
            char err[1204];
            sprintf(err, "Exception in CreateSpecialDungeon creating dungeon: %d", i + 10);
            throw std::exception(err);
        }
    }
}

void DungeonManager::CreateLockedDungeon()
{
    for (int i = 0; i < 10; i++)
    {
        //get gate position
        coord *pos = level[i].GetEndPosition();

        if (pos == NULL)
        {
            std::stringstream err;
            err << "CreateLockedDungeon: Failed to find end position on level: " << i;
            throw std::exception(err.str().c_str());
        }

        //create locks on these normal dungeon entrances
        if (i == 0 || //dungeon entrance
            i == 3 || //river entrance
            i == 6 || //undead entrance
            i == 8    //zagor entrance
            )
        {
            if (level[i].map[pos->x][pos->y].terrain.type == openStairs)
                level[i].map[pos->x][pos->y].terrain.Create(lockedStairs);
        }
        else
        {
            if (level[i].map[pos->x][pos->y].terrain.type == lockedStairs)
                level[i].map[pos->x][pos->y].terrain.Create(openStairs);
        }
    }
}

void DungeonManager::PrintDungeons()
{
    //printf dungeon

#ifdef _DEBUG
    std::ofstream ofile;

    ofile.open("Debug Files\\dungeon.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open dungeon.txt");

    ofile << std::endl << "Dungeon Levels - For debug purposes only" << std::endl << std::endl;

    for (int i = 0; i < MAX_DUNGEON_LEVELS; i++)
    {
        ofile << std::endl << "Dungeon Level: " << i << " For debug purposes only" << std::endl;

        for (int h = 0; h < DUNGEON_SIZE_H; h++)
        {
            for (int w = 0; w < DUNGEON_SIZE_W; w++)
            {
                ofile << (char)level[i].map[w][h].terrain.symbol;
            }
            ofile << std::endl;
        }
    }
    ofile.close();
#endif
}

void DungeonManager::PopulateSpecialDungeon(int lev, int type)
{
    int sub_type = 0;

    if (type == slBarracks) //undead level
    {
        int max_monsters = Random::getInt(15, 7);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mOrc;//Random::getInt(1,1);

            coord *pos = NewMonsterPosition(lev); //??
            WorldBuilder::monsterManager.CreateMonster(type, Random::getInt(4, 0), lev, pos->x, pos->y);
        }
    }

    else if (type == slBoatHouse) //skeleton level
    {
        int max_monsters = Random::getInt(15, 12);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mUndead;//Random::getInt(1,1);

            coord *pos = NewMonsterPosition(lev); //??
            WorldBuilder::monsterManager.CreateMonster(type, skeleton, lev, pos->x, pos->y);
        }
    }
     
    else	//random level
    {
        int max_monsters = Random::getInt(10, 6);

        for (int i = 0; i < max_monsters; i++)
        {
            coord *pos = NewMonsterPosition(lev); //??
            WorldBuilder::monsterManager.CreateMonster(mRandom, sub_type, lev, pos->x, pos->y);
        }

        //	coord *pos = NewMonsterPosition(lev); //??
        //	WorldBuilder::monsterManager.CreateMonster(mSpecial,sub_type,lev,pos->x,pos->y); 
    }

    /*	if(lev==0)
        {
        coord* pos = level[lev].NewItemPosition();
        level[lev].map[3][3].item = WorldBuilder::itemManager.CreateItem(lev,lockedChest);
        }*/
}

int DungeonManager::AddKeysToDungeon()
{
    //////add keys//////
    for (int ind = 10; ind < 20; ind++)
    {
        int rand_level = Random::getInt(ind - 1, 1); //key can be anywhere below current level. 
        //I.e key for level 15 (special level 5) can be anywhere below level 150
        if (!level[rand_level].initialised)
        {
            ind--;
            continue;
        }
        try
        {
            coord* pos = level[rand_level].NewItemPosition();
            if (pos == NULL)
                throw std::exception("Could not create keys");

            ITEMLIST::iterator it;
            int duplicated = 0;

            for (it = WorldBuilder::itemManager.all_items.begin(); it != WorldBuilder::itemManager.all_items.end(); it++)
            {
                if (it->type == key)
                {
                    int x = it->itemNumber[0];
                    int y = WorldBuilder::itemManager.KeySpecial[ind - 10];
                    if (it->itemNumber[0] == WorldBuilder::itemManager.KeySpecial[ind - 10])
                    {
                        duplicated = 1;
                        break;
                    }
                }
            }

            if (!duplicated)
                level[rand_level].map[pos->x][pos->y].AssignItem(WorldBuilder::itemManager.CreateItem(ind, key));
        }
        catch (const std::exception)
        {
            //no dungeon available
            ind--;
        }
    }
    return 1;
}


DungeonLevel* DungeonManager::CurrentLevel()
{
    return &level[WorldBuilder::GetCurrentLevel()];
}

