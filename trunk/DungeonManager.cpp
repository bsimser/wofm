// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

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

    try
    {
        CreateSpecialDungeon();
        //AddKeysToDungeon(); move to before zagors chest is created
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
    level(dlevel).Initialise(type);

    if (dlevel == 8) // minotaur level
        level(dlevel).setMapLight(DungeonLevel::eNoFound);

    return 1;
}

int DungeonManager::DeleteDungeon(int dlevel)
{
    //	level(dlevel).Delete();
    return 1;
}


Coord * DungeonManager::NewMonsterPosition(int lev)
{
    try
    {
        Coord *newPos = level(lev).FreeTerrainPosition(dfloor);
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

Coord * DungeonManager::NewBridgePosition(int lev)
{
    try
    {
        return level(lev).FreeTerrainPosition(bridge);
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
        Coord* pos = level(lev).NewItemPosition();
        //level(lev).getCell(pos->x, pos->y).getItem() = World.getItemManager().CreateRandomItem(lev);
        int item_test = getInt(10, 0);
        if (item_test == 0)
        {
            Coord* pos = level(lev).NewItemPosition(true);
            level(lev).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(lev, lockedChest));
        }
        else if (getInt(10, 0) < 2)
        {
            Coord* pos = level(lev).NewItemPosition(true);
            level(lev).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(lev, openChest));
        }
        else
        {
            Coord* pos = level(lev).NewItemPosition();
            level(lev).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(lev, corpse));
        }
    }

    Coord * pos = level(lev).FreeTerrainPosition(random);

    if (pos)
    {
        switch (Random::getInt(5,0))
        {
        case 0: level(lev).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(lev+25, corpse)); break;
        case 1: level(lev).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(lev+25, weapon)); break;
        case 2: level(lev).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(lev+25, armour)); break;
        case 3: level(lev).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(lev+25, shield)); break;
        case 4:
        {
            Item * item = World.getItemManager().CreateItem(lev + 25, projectile);
            item->itemNumber[1] = 10;
            level(lev).getCell(pos->x, pos->y).AssignItem(item);
        }break;

        default: level(lev).getCell(pos->x, pos->y).terrain.Create(fountain); break;

        }
        if (Random::getInt(2, 0))
        {
            World.getMonsterManager().CreateMonster(mRandom, 0, 20, pos->x, pos->y)->SetState(sentry);
        }
        
    }

    /*	if(lev==1)
        {
        Coord* pos = level(lev).NewItemPosition();
        level(lev).getCell(pos->x, pos->y].item = World.getItemManager().CreateItem(lockedChest,lev);
        }*/

    return 1;
}

int DungeonManager::PopulateDungeon(int lev)
{
    int sub_type = 0;
    //add player

    if (!level(lev).initialised)
        throw("Attempting to populate uninitialised level:");

    if (lev == 0) //first level add player
    {
        Coord *start = level(lev).getStartPos(); //get starting point (up stairs)
        MonsterData* player = World.getMonsterManager().CreateMonster(mPlayer, sub_type, 0, start->x, start->y);

        player->SetState(normal);

        if (fopen("Zagor", "r"))
        {
            World.getSpellManager().AddMonsterSpell(player, spFlyingWeapon);
            World.getSpellManager().AddMonsterSpell(player, spFlyingWeapon);
            World.getSpellManager().AddMonsterSpell(player, spRepelMissiles);
            World.getSpellManager().AddMonsterSpell(player, spSlowEnemies);
            World.getSpellManager().AddMonsterSpell(player, spTeleport);
            World.getSpellManager().AddMonsterSpell(player, spTeleport);
        }
        else
            World.getSpellManager().AddMonsterSpell(player, spFlyingWeapon);
        
        //World.getSpellManager().AddMonsterSpell(player, spDragonFire);
    }

    //add monsters
#ifndef NO_MONSTERS

    if (lev == 7) //undead level
    {
        int max_monsters = Random::getInt(10, 5);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mUndead;//Random::getInt(1,1);

            Coord *pos = NewMonsterPosition(lev); //??
            MonsterData *mob = World.getMonsterManager().CreateMonster(type, Random::getInt(4, 0), lev, pos->x, pos->y);

            	std::string name = mob->Name();
            //	if(name.find("Ghoul") !=-1 )
            //		mob->monster.SetBrand(bFire,3);
        }

        Coord *pos = NewMonsterPosition(lev); //??
        World.getMonsterManager().CreateMonster(mSpecial, sub_type, lev, pos->x, pos->y);
    }

    else if (lev == 0) //start level
    {
        Coord *pos = NewBridgePosition(lev); //??
        if (!pos)
            pos = NewMonsterPosition(lev);

        MonsterData *gog = World.getMonsterManager().CreateMonster(mOrc, sub_type, lev, pos->x, pos->y);

        pos = NewBridgePosition(lev); //??
        if (!pos)
            pos = NewMonsterPosition(lev);
        
        gog = World.getMonsterManager().CreateMonster(mOrc, sub_type, lev, pos->x, pos->y);

        //pos = level(lev).FreeTerrainPosition(openStairs); //??
        pos = level(lev).GetEndPosition();

        World.getMonsterManager().CreateMonster(mSpecial, sub_type, lev, pos->x, pos->y);

        // rat nest
       /* pos = NewMonsterPosition(lev);
        for (int i = 0; i < 6; i++)
        {
            World.getMonsterManager().CreateMonster(mRandom, RandomMonster::rat, lev, pos->x, pos->y);
        }*/
    }

    else if (lev == 20) //encounter level
    {
        int player_level = World.getRestLevel().GetOldLevel();

        int max_monsters = Random::getInt(3 + player_level, 1);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mOrc;//Random::getInt(1,1);

            Coord *pos = NewMonsterPosition(lev); //??
            World.getMonsterManager().CreateMonster(type, sub_type, lev, pos->x, pos->y);
        }
    }
    else	//standard orc level
    {
        int max_monsters = Random::getInt(10, 5);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mOrc;//Random::getInt(1,1);

            Coord *pos = NewMonsterPosition(lev); //??
            World.getMonsterManager().CreateMonster(type, sub_type, lev, pos->x, pos->y);

        }
        // create digger
        if (lev != 8 && lev > 1 && Random::getInt(10, 0) == 0)
        {
            static int count = 0;
            count++;
            if (count < 3)
            {
                Coord *pos = NewMonsterPosition(lev);
                World.getMonsterManager().CreateMonster(mDigger, sub_type, lev, pos->x, pos->y);
            }
        }
        if ((lev != 8 && lev > 1 && Random::getInt(10, 0) == 0) || lev == 6)
        {
            Coord *pos = NewMonsterPosition(lev);
            for (int i = 0; i < 6; i++)
            {
                World.getMonsterManager().CreateMonster(mRandom, RandomMonster::rat, lev, pos->x, pos->y);
            }
        }

        // crocodiles!!
        if (lev > 1)
        {
            int maxNumber = lev/2; 

            maxNumber = Random::getInt(maxNumber, 0);
            if (maxNumber >= 5) 
                maxNumber = 4;
            for (int x = 0; x < maxNumber; x++)
            {
                Coord *pos = level(lev).FreeTerrainPosition(deepWater);
                if (pos)
                {
                    World.getMonsterManager().CreateMonster(mCrocodile, 0, lev, pos->x, pos->y);
                }
            }
        }

        // create level guardian
        Coord *pos = NewMonsterPosition(lev); //??
        if (lev != 9)
        {
            MonsterData * special = World.getMonsterManager().CreateMonster(mSpecial, sub_type, lev, pos->x, pos->y);
            if (lev == 6)
            {
                World.getMonsterManager().monsterItems.EquipMonster(special, lev);
            }
        }
    }
    if (lev == 4) //river level
    {
        Coord *pos = NewBridgePosition(lev); //??
        if (pos == NULL)
            throw std::exception("no bridge master, Game halted");
        World.getMonsterManager().CreateMonster(mBridgeMaster, sub_type, lev, pos->x, pos->y);

        pos = level(lev).FreeTerrainPosition(deepWater);
        if (pos)
        {
            World.getMonsterManager().CreateMonster(mCrocodile, 0, lev, pos->x, pos->y);
        }
    }

    if (lev == 9) //end level
    {
        Coord *pos = NewMonsterPosition(lev); //??
        MonsterData* wiz = World.getMonsterManager().CreateMonster(mWizard, sub_type, lev, pos->x, pos->y);
        World.getSpellManager().AddMonsterSpell(wiz, spRepelMissiles);
        World.getSpellManager().AddMonsterSpell(wiz, spTeleport);
        World.getSpellManager().AddMonsterSpell(wiz, spFlyingWeapon);
        World.getSpellManager().AddMonsterSpell(wiz, spSlowEnemies);

        wiz->is_magic = 1;
    }

#endif

#ifndef NO_ITEMS
    //add items
    if (lev != 0 && lev <= 10)
        AddItems(lev);

    if (lev == 0)
    {
        //	Coord* pos = level(lev).NewItemPosition();
        //level(lev).getCell(3,3).AssignItem( World.getItemManager().CreateItem(lev,lockedChest));
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
                // no special dungeon on level 0
                /*CreateDungeon(i + 10, slBarracks);
                PopulateSpecialDungeon(i + 10, slRandom);

                // make entrance
                Coord * pos = level(i].FreeTerrainPosition("dungeon floor");
                if (!pos)
                {
                    pos->x = 5;
                    pos->y = 5;
                }
                level(i).getCell(pos->x, pos->y).terrain.Create(specialOpen);

                pos = level(i + 10).NewSpecialItemPosition();
                level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

               // pos = level(i + 10).NewSpecialItemPosition(); //??
                World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);*/
            }
            else if (i == 1 || i == 2 || i == 3)
            {
                // barracks
                if (/*getInt(1, 1) == 1 &&*/ i == 1) // %100 
                {
                    CreateDungeon(i + 10, slBarracks);
                    PopulateSpecialDungeon(i + 10, slBarracks);
                    Coord *pos = NewMonsterPosition(i);
                    level(i).getCell(pos->x, pos->y).terrain.Create(specialOpen);

                    pos = level(i + 10).NewSpecialItemPosition();
                    level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                    World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                // troll cave
                else if (i == 2 && special_type1 == 0 && Random::getInt(2, 0)) // 50%
                {
                    CreateDungeon(i + 10, slTrollCave);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    Coord *pos = NewMonsterPosition(i);
                    level(i).getCell(pos->x, pos->y).terrain.Create(specialOpen);
                    special_type1++;

                    pos = level(i + 10).NewSpecialItemPosition();
                    level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                    pos = level(i + 10).NewSpecialItemPosition(); //??
                    World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                // ogre cave
                else if (i == 3 && special_type1 == 0) // 50 %
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    Coord *pos = NewMonsterPosition(i); //create entry on normal dungeon
                    level(i).getCell(pos->x, pos->y).terrain.Create(specialOpen);
                    special_type1++;

                    pos = level(i + 10).NewSpecialItemPosition();
                    level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                    //pos = level(i+10].NewSpecialItemPosition(); // put guard of chest
                    World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
            }
            // boat house - skeletons
            else if (i == 4) 
            {
                Coord *pos;
                CreateDungeon(i + 10, slBoatHouse);
                PopulateSpecialDungeon(i + 10, slBoatHouse);

                do
                {
                    pos = NewMonsterPosition(i);
                    test++;
                    if (test > 10000)
                        break;
                } while (std::string(level(i).getCell(pos->x, pos->y).terrain.name) != "sand");

                level(i).getCell(pos->x, pos->y).terrain.Create(specialLocked);

                pos = level(i + 10).NewSpecialItemPosition();
                level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                //pos = level(i + 10).NewSpecialItemPosition(); //??
                World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
            }
            // spider cave
            else if (i == 5 || i == 6)
            {
                if (i == 5 && getInt(2, 0) == 1)
                {
                    CreateDungeon(i + 10, slSpiderCave);
                    PopulateSpecialDungeon(i + 10, slSpiderCave);
                    //choose one entrances
                    Coord *pos = NewMonsterPosition(i);
                    level(i).getCell(pos->x, pos->y).terrain.Create(specialOpen);
                    special_type2++;

                    pos = level(i + 10).NewSpecialItemPosition();
                    level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                    pos = level(i + 10).NewSpecialItemPosition(); //??
                    World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                // sandworm
                else if (i == 6 && special_type2 == 0)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    //choose one entrances
                    Coord *pos = NewMonsterPosition(i + 10);
                    level(i).getCell(pos->x, pos->y).terrain.Create(specialOpen);

                    pos = level(i + 10).NewSpecialItemPosition();
                    level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                    pos = level(i + 10).NewSpecialItemPosition(); //??
                    World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
            }
            else if (i == 7 || i == 8)
            {
                // undead
                if (i == 7 && getInt(2, 0) == 1)
                {
                    CreateDungeon(i + 10, slUndead);
                    PopulateSpecialDungeon(i + 10, slUndead);
                    //choose special undead level
                    Coord *pos = NewMonsterPosition(i);
                    level(i).getCell(pos->x, pos->y).terrain.Create(specialLocked);
                    special_type3++;

                    pos = level(i + 10).NewSpecialItemPosition();
                    level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                    pos = level(i + 10).NewSpecialItemPosition(); //??
                    World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
                // minotaur
                else if (i == 8 && special_type3 == 0)
                {
                    CreateDungeon(i + 10, i + 10);
                    PopulateSpecialDungeon(i + 10, slRandom);
                    //choose one entrances
                    Coord *pos = NewMonsterPosition(i);
                    level(i).getCell(pos->x, pos->y).terrain.Create(specialOpen);

                    pos = level(i + 10).NewSpecialItemPosition();
                    level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                    pos = level(i + 10).NewSpecialItemPosition(); //??
                    World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);
                }
            }
            //dragon
            else if (i == 9)
            {
                CreateDungeon(i + 10, i + 10);
                //	PopulateSpecialDungeon(i+10,slRandom);

                //create treasure level
                Coord *pos = NewMonsterPosition(i);
                level(i).getCell(pos->x, pos->y).terrain.Create(specialLocked);

                AddKeysToDungeon();

                // zagor locked chest
                pos = level(i + 10).NewSpecialItemPosition();
                level(i + 10).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(i + 10, lockedChest));

                pos = level(i + 10).NewSpecialItemPosition(); //??
                MonsterData *dragon = World.getMonsterManager().CreateMonster(mGuards, i, i + 10, pos->x, pos->y);

                World.getSpellManager().AddMonsterSpell(dragon, spDragonFire);

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
        Coord *pos = level(i).GetEndPosition();

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
            if (level(i).getCell(pos->x, pos->y).terrain.type == openStairs)
                level(i).getCell(pos->x, pos->y).terrain.Create(lockedStairs);
        }
        else
        {
            if (level(i).getCell(pos->x, pos->y).terrain.type == lockedStairs)
                level(i).getCell(pos->x, pos->y).terrain.Create(openStairs);
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
                ofile << (char)level(i).getCell(w, h).terrain.symbol;
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

            Coord *pos = NewMonsterPosition(lev); //??
            World.getMonsterManager().CreateMonster(type, Random::getInt(4, 0), lev, pos->x, pos->y);
        }
    }

    else if (type == slBoatHouse) //skeleton level
    {
        int max_monsters = Random::getInt(15, 12);

        for (int i = 0; i < max_monsters; i++)
        {
            int type = mUndead;//Random::getInt(1,1);

            Coord *pos = NewMonsterPosition(lev); //??
            World.getMonsterManager().CreateMonster(type, skeleton, lev, pos->x, pos->y);
        }
    }
    else if (type == slSpiderCave) //skeleton level
    {
        int max_monsters = Random::getInt(15, 12);

        for (int i = 0; i < max_monsters; i++)
        {
            Coord *pos = NewMonsterPosition(lev); //??
            World.getMonsterManager().CreateMonster(mRandom, RandomMonster::spider, lev, pos->x, pos->y);
        }
    }
    else if (type == slUndead) //skeleton level
    {
        int max_monsters = Random::getInt(15, 12);

        for (int i = 0; i < max_monsters; i++)
        {
            Coord *pos = NewMonsterPosition(lev); //??
            World.getMonsterManager().CreateMonster(mUndead, Random::getInt(vampire+1, skeleton), lev, pos->x, pos->y);
        }
    }
    else	//random level
    {
        int max_monsters = Random::getInt(10, 6);

        for (int i = 0; i < max_monsters; i++)
        {
            Coord *pos = NewMonsterPosition(lev); //??
            World.getMonsterManager().CreateMonster(mRandom, sub_type, lev, pos->x, pos->y);
        }

        //	Coord *pos = NewMonsterPosition(lev); //??
        //	World.getMonsterManager().CreateMonster(mSpecial,sub_type,lev,pos->x,pos->y); 
    }

    int maxCrocs = Random::getInt(5, 0);
    for (int x = 0; x < maxCrocs; x++)
    {
        Coord *pos = level(lev).FreeTerrainPosition(deepWater);

        if (pos)
        {
            World.getMonsterManager().CreateMonster(mCrocodile, 0, lev, pos->x, pos->y);
        }
    }

    /*	if(lev==0)
        {
        Coord* pos = level(lev).NewItemPosition();
        level(lev).getCell(3, 3).item = World.getItemManager().CreateItem(lev,lockedChest);
        }*/
}

DungeonLevel & DungeonManager::level(int level)
{
    if (level < 0 || level > 20)
        std::exception("Level out of bounds");
    return dungeonLevels[level];
}

int DungeonManager::AddKeysToDungeon()
{
    //////add keys//////
    for (int ind = 10; ind < 20; ind++)
    {
        int rand_level = Random::getInt(ind - 1, 1); // key can be anywhere below current level. 
        //I.e key for level 15 (special level 5) can be anywhere below level 150
        if (!level(rand_level).initialised)
        {
            ind--;
            continue;
        }
        try
        {
            Coord* pos = level(rand_level).NewItemPosition(true); // put keys in rooms
            if (pos == NULL)
                throw std::exception("Could not create keys");

            ITEMLIST::iterator it;
            int duplicated = 0;

            for (it = World.getItemManager().all_items.begin(); it != World.getItemManager().all_items.end(); it++)
            {
                if (it->type == key)
                {
                    int x = it->itemNumber[0];
                    int y = World.getItemManager().KeySpecial[ind - 10];
                    if (it->itemNumber[0] == World.getItemManager().KeySpecial[ind - 10])
                    {
                        duplicated = 1;
                        break;
                    }
                }
            }

            if (!duplicated)
                level(rand_level).getCell(pos->x, pos->y).AssignItem(World.getItemManager().CreateItem(ind, key));
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
    return &level(World.GetCurrentLevel());
}

std::string DungeonManager::getLevelName(int level)
{
    if (level == 0)
    {
        return "outside";
    }
    else if (level == 1) return "1";
    else if (level == 2) return "2";
    else if (level == 3) return "3";
    else if (level == 4) return "4";
    else if (level == 5) return "5";
    else if (level == 6) return "6";
    else if (level == 7) return "7";
    else if (level == 8) return "maze";
    else if (level == 9) return "9";
    else if (level == 11) return "barracks";
    else if (level == 12) return "cave";
    else if (level == 13) return "cave";
    else if (level == 14) return "boathouse";
    else if (level == 15) return "dusty cave";
    else if (level == 16) return "cave";
    else if (level == 17) return "musty rooms";
    else if (level == 18) return "deep rooms";
    else if (level == 19) return "treasure rooms";
    return "unknown";
}