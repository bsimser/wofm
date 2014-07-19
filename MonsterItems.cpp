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
#include "MonsterItems.h"
#include "UserCommand.h"
#include "numberGenerator.h"

using namespace Random;

Item * MonsterItems::GetEquipment(MonsterData *monster, eItemType equip_type)
{
    ITEMLIST::iterator it;
    ITEMLIST *inventory = &monster->inventory;

    for (it = inventory->begin(); it != inventory->end(); it++)
    {
        if (it->type == equip_type && it->equipped)
            return &(*it);

    }
    return NULL;
}

Item * MonsterItems::GetInventoryItem(MonsterData *monster, eItemType equip_type)
{
    ITEMLIST::iterator it;
    ITEMLIST *inventory = &monster->inventory;

    for (it = inventory->begin(); it != inventory->end(); it++)
    {
        if (it->type == equip_type)
            return &(*it);
    }
    return NULL;
}

int MonsterItems::EquipMonster(MonsterData *monster, int level)
{
    std::string & name = monster->monster.name;

    monster->inventory.clear();

    Item * item;

    if (name.find("Servant") != -1) //servants do not use armour and have maces or swords
    {
        if (Random::getInt(2, 0) == 0)
            item = World.getItemManager().CreateItem(level, weapon, sword);
        else
            item = World.getItemManager().CreateItem(level, weapon, mace);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;
    }
    else if (name.find("archer") != -1) //archers use projectile weapons and have leather armour
    {
        int p;
        int pw;

        int random_weapon = getInt(3, 0);

        switch (random_weapon)
        {
        case 0: p = bolt; pw = crossbow; monster->monster.name = "orc crossbower"; break;
        case 1: p = arrow; pw = bow;     monster->monster.name = "orc archer"; break;
        case 2: p = arrow; pw = longbow; monster->monster.name = "orc archer"; break;
        }

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, projectile, p));
        monster->inventory.back().itemNumber[1] = getInt(20, 10);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, projectileWeapon, pw));
        monster->inventory.back().equipped = 1;

        if (Random::getInt(2, 0) == 0)
            item = World.getItemManager().CreateItem(level, weapon, sword);
        else
            item = World.getItemManager().CreateItem(level, weapon, mace);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, armour, leather));
        monster->inventory.back().equipped = 1;
    }
    else if (name.find("warrior") != -1 || name.find("sentry") != -1)
    {
        item = World.getItemManager().CreateItem(level, weapon);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, armour));
        monster->inventory.back().equipped = 1;

        if (Random::getInt(3, 0) == 0)//add shield
        {
            monster->inventory.push_back(*World.getItemManager().CreateItem(level, shield, 0));
            monster->inventory.back().equipped = 1;
        }

    }
    else if (name.find("butcher") != -1)
    {
        item = World.getItemManager().CreateItem(level, weapon, cleaver);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, armour, bloodyapron));
        monster->inventory.back().equipped = 1;
    }

    else if (name.find("Zagor") != -1)
    {
        item = World.getItemManager().CreateItem(level, weapon, staff);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, armour, blackcloak));
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, cards, 0));
    }
    else //create random weapon and armour
    {
        item = World.getItemManager().CreateItem(level, weapon, sword);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*World.getItemManager().CreateItem(level, armour));
        monster->inventory.back().equipped = 1;
    }

    if (monster->monster.GetType() == mSpecial || monster->monster.GetType() == mGuards)
    {
        if (level > 1)
        {
            int itemType = Random::getInt(silverArrow + 1, provisions);
            if (itemType = silverArrow)
            {
                item = World.getItemManager().CreateItem(level, projectile, silver);
            }
            else
            {
                item = World.getItemManager().CreateItem(level, itemType, 0);
                if (itemType == stake)
                    item->itemNumber[1] = Random::getInt(3, 1);
            }

            monster->inventory.push_back(*item);
        }
        if (monster->Name() == "ogre")
        {
            monster->inventory.push_back(*World.getItemManager().CreateItem(level, carcass));

        }
    }

    return 1;
}

int MonsterItems::EquipPlayer(MonsterData *player)
{
    int level = 0;

    player->inventory.clear();

    //weapon
    Item * item = World.getItemManager().CreateItem(level, weapon);
    item->identified = 1;
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;

    //armour
    item = World.getItemManager().CreateItem(level, armour);
    item->identified = 1;
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;

    item = World.getItemManager().CreateItem(level, provisions);
    item->itemNumber[1] = 4;
    player->inventory.push_back(*item);

    /*item = World.getItemManager().CreateItem(level, cheese);
    item->itemNumber[1] = 4;
    player->inventory.push_back(*item);

    item = World.getItemManager().CreateItem(level, DiMaggio);
    player->inventory.push_back(*item);

    item = World.getItemManager().CreateItem(level, gem);
    player->inventory.push_back(*item);*/

    //player->inventory.push_back(*World.getItemManager().CreateItem(level,cards,0)); //give magic to player

    //shield
    //player->inventory.push_back(*World.getItemManager().CreateItem(level,shield,0));
    //player->inventory.back().equipped=1;

    //for(int i=0;i<20;i++)
    {
        //	item = World.getItemManager().CreateItem(i,key);
        //	player->inventory.push_back(*item);
    }

    //PROJECTILE WEAPON
    int p;
    int pw;

    int random_weapon = getInt(3, 0);
    switch (random_weapon)
    {
    case 0: p = bolt;  pw = crossbow; break;
    case 1: p = arrow; pw = bow;     break;
    case 2: p = arrow; pw = longbow; break;
    }

    //projectile weapons
    item = World.getItemManager().CreateItem(level, projectileWeapon, pw);
    item->identified = 1;
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;

    //projectiles
    item = World.getItemManager().CreateItem(level, projectile, p);
    item->identified = 1;
    item->itemNumber[1] = getInt(12, 6);
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;


    return 1;
}

int MonsterItems::EquipItem(MonsterData *monster, int item)
{
    ITEMLIST::iterator it;

    ITEMLIST *inventory = &monster->inventory;

    int i = 0;

    int gogo = inventory->size();

    for (it = inventory->begin(); it != inventory->end(); it++, i++)
    {
        if (i == item) //found
        {
            if (!it->wearable())
                return 0; //can not be equipped

            if (it->equipped == 1) //unequip
            {
                it->equipped = 0;
            }
            else	//equip //unequip privious item
            {
                switch (it->type)
                {
                case projectile:
                {
                    Item *eq = GetEquipment(monster, projectile);
                    if (eq != NULL)
                        eq->equipped = 0;
                    break;
                }
                case projectileWeapon:
                {
                    Item *eq = GetEquipment(monster, projectileWeapon);
                    if (eq != NULL)
                        eq->equipped = 0;
                    break;
                }
                case provisions:
                {
                    return UseItem(monster, *it);
                }
                case cheese:
                {
                    UseItem(monster, *it);
                    return 0;
                }
                case DiMaggio:
                {
                    return UseItem(monster, *it);
                }
                case gem:
                {
                    return UseItem(monster, *it);
                }
                case armour:
                {
                    Item *eq = GetEquipment(monster, armour);
                    if (eq != NULL)
                        eq->equipped = 0;
                    break;
                }
                case shield: {
                    Item *eq = GetEquipment(monster, shield);
                    if (eq != NULL)
                        eq->equipped = 0;
                    break;
                }

                case weapon:{//if(monster->slots.weapon != NULL)
                    Item *eq = GetEquipment(monster, weapon);
                    if (eq != NULL)
                        eq->equipped = 0;
                    //		 {
                    //			monster->slots.weapon->equipped = 0;
                    //		 } */
                    //	monster->slots.weapon = &(*it);
                    break;
                }
                }

                it->equipped = 1;
            }
            return 1;
        }
    }
    return 0;
}

int MonsterItems::RemoveItem(MonsterData *monster, int ref)
{
    for (ITEMLIST::iterator it = monster->inventory.begin(); it != monster->inventory.end(); it++)
    {
        if (it->ref == ref)
        {
            monster->inventory.erase(it);
            return 1;
        }
    }
    return 0;
}

int MonsterItems::UseItem(MonsterData *monster, Item & item)
{
    if (item.type == provisions && item.itemNumber[1])
    {
        if (item.itemNumber[1] > 0)
        {
            item.itemNumber[1]--;

            if (monster->monster.stamina >= monster->monster.MaxStamina())
            {
                World.getTextManager().newLine("You are not hungry. ");
                return 0;
            }
            World.getTextManager().newLine("You eat your provision. You feel a little better. ");

            monster->monster.stamina += 4;
            if (monster->monster.stamina > monster->monster.MaxStamina())
                monster->monster.stamina = monster->monster.MaxStamina();

            if (item.itemNumber[1] == 0)
            {
                //delete from inventory
                if (item.itemNumber[1] == 0)
                {
                    RemoveItem(monster, item.ref);
                    World.getTextManager().newLine("You have no more provisions. ");
                }
            }
            return 1;
        }
    }
    if (item.type == cheese)
    {
        World.SetState(sThrow);
        UserCommand::Throw(&item);
        return 1;
    }
    if (item.type == DiMaggio)
    {
        // DRAGON
        for (MONSTERLIST::iterator it = World.getMonsterManager().monster_list.begin(); it != World.getMonsterManager().monster_list.end(); ++it)
        {
            if (it->monster.name == "dragon")
            {
                if (it->isSeen() == 1 && it->spellList.size() > 0)
                {
                    it->spellList.clear();
                    World.getTextManager().newLine("The dragon shakes it's head violently and smoke comes out it's ears! ");
                    it->monster.stamina -= 5;
                    it->monster.color1 = 200;
                    it->monster.color2 = 200;
                    it->monster.color3 = 200;
                    it->monster.resistanceMap.clear();
                }
                else
                    World.getTextManager().newLine("Nothing happens. ");
            }
        }
        return 3; // return to main screen
    }
    else if (item.type == gem)
    {
        bool done = false;
        for (MONSTERLIST::iterator it = World.getMonsterManager().monster_list.begin(); it != World.getMonsterManager().monster_list.end(); ++it)
        {
            if (it->isSeen() == 1 && !it->isPlayer() )
            {
                it->spellList.clear();
                World.getTextManager().newLine("The %s is weakened! ", it->Name().c_str());
                it->monster.skill -= 4;
                it->monster.stamina = (it->monster.stamina * 70) / 100;
                done = true;
            }
        }
        // remove gem.
        if (done)
        {
            RemoveItem(monster, item.ref);
        }
        else
            World.getTextManager().newLine("Nothing happens. ");

        return 3; // return to main screen
    }
    return 0;
}

int MonsterItems::DropItem(MonsterData *monster, int item, bool erace_from_inv)
{
    ITEMLIST::iterator it;
    ITEMLIST::iterator it_all;

    ITEMLIST *all = &World.getItemManager().all_items;

    Coord *pos = &monster->pos;

    int i = 0;


    int test = monster->inventory.size();

    for (it = monster->inventory.begin(); it != monster->inventory.end(); it++, i++)
    {
        if (i == item) //found
        {
            //Find item in all items and place it in square
            for (it_all = all->begin(); it_all != all->end(); it_all++)
            {
                if (it->ref == it_all->ref)
                {
                    //if(it->equipped == 1)
                    //Unequip();
                    it->equipped = 0;
                    it_all->itemNumber[1] = it->itemNumber[1];

                    if (AttemptDropItem(monster, &(*it_all), pos->x, pos->y))
                    {
                        if (erace_from_inv)
                            monster->inventory.erase(it);
                    }
                    it_all->equipped = 0;

                    break;
                }
            }
            return 1;
        }
    }
    return 0;
}


int MonsterItems::PickupItem(MonsterData *monster)
{

    if (!World.getDungeonManager().level[monster->level].map[monster->pos.x][monster->pos.y].getItem()) //check if item exists
        return 0;


    if (monster->inventory.size() > 25)
    {
        if (monster->isPlayer())
            World.getTextManager().newLine("You have no room in you pack. ");
        return 0;
    }


    if (monster->isPlayer()) //id item for player
    {
        Item * item_to_copy = World.getDungeonManager().level[monster->level].map[monster->pos.x][monster->pos.y].getItem();
        if (item_to_copy->type == lockedChest || item_to_copy->type == openChest)
        {
            World.getTextManager().newLine("You cannot lift it. ");
            return 0;
        }
        item_to_copy->identified = 1;
    }
    //get copy of item

    Item item(*World.getDungeonManager().level[monster->level].map[monster->pos.x][monster->pos.y].getItem());



    //delete reference from map but item is still in all items list
    World.getDungeonManager().level[monster->level].map[monster->pos.x][monster->pos.y].RemoveItemRef();

    if (monster->isPlayer())
        World.getTextManager().newLine("You pick up the %s%s", item.GetName().c_str(), item.itemNumber[1] > 1 ? ". " : ". ");
    else
        World.getTextManager().newLine("The %s picks up the %s%s", monster->monster.name.c_str(), item.BaseName().c_str(), item.itemNumber[1] > 1 ? "s. " : ". ");

    //give item to monster

    //check if stackable and add to existing stack
    if (item.stackable())
    {
        ITEMLIST::iterator it;
        bool stacked = false;
        for (it = monster->inventory.begin(); it != monster->inventory.end(); it++)
        {
            //check if same type, secondary ype and skill bonus
            if (item.type == it->type && item.secondaryType == it->secondaryType && item.skill_bonus == it->skill_bonus &&
                item.getPrefix() == it->getPrefix() && item.getPostfix() == it->getPostfix())
            {
                it->itemNumber[1] += item.itemNumber[1];
                stacked = true;
                break;
            }
        }
        if (!stacked)
            monster->inventory.push_back(item);

    }
    else
        monster->inventory.push_back(item);

    return 1;
}

//drop all keys
int MonsterItems::DropRandomItems(MonsterData *monster)
{
    if (monster->inventory.size() < 0) //no items
        return 0;

    //drop all keys and cards

    ITEMLIST::iterator it;

    int test = monster->inventory.size();
    int i = 0;

    //drop all keys and cards
    for (it = monster->inventory.begin(); it != monster->inventory.end();)
    {
        if (it->type == key)
        {
            World.getMonsterManager().monsterItems.DropItem(monster, i, false);
            it = monster->inventory.erase(it);
        }
        else if (it->type == cards)
        {
            World.getMonsterManager().monsterItems.DropItem(monster, i, false);
            it = monster->inventory.erase(it);
        }
        else
        {
            it++;
            i++;
        }
        /*else if(rit->type == gold)
        {
        World.getMonsterManager().monsterItems.DropItem(monster,i);
        }*/
    }

    int rand_drop = getInt(monster->inventory.size() + 1, 0);

    if (rand_drop == monster->inventory.size()) //no drop (0=0)
        return 0;

    World.getMonsterManager().monsterItems.DropItem(monster, rand_drop);

    if (monster->inventory.size() > 0) //more items to drop
        DropRandomItems(monster);

    return 1;

}

///////////private///////////////////
int MonsterItems::DropStackableItem(MonsterData *monster, Item *item, int x, int y)
{
    int dungeonLevel = monster ? monster->level : World.GetCurrentLevel();

    Item* floor_item = World.getDungeonManager().level[dungeonLevel].map[x][y].getItem();
    if (World.getDungeonManager().level[dungeonLevel].map[x][y].terrain.type != dfloor
        &&World.getDungeonManager().level[dungeonLevel].map[x][y].terrain.type != bridge)
    {
        return 0;
    }

    else if (floor_item == NULL) //no item
    {
        World.getDungeonManager().level[dungeonLevel].map[x][y].AssignItem(item);
        return 1;
    }
    else //item - check if same
    {
        if (item->type == floor_item->type && item->secondaryType == floor_item->secondaryType && item->skill_bonus == floor_item->skill_bonus)
        {
            floor_item->itemNumber[1]++;
            return 1;
        }
    }
    return 0;
}

int MonsterItems::DropItem(MonsterData *monster, Item *item, int x, int y)
{
    int dungeonLevel = monster ? monster->level : World.GetCurrentLevel();

    if (std::string("a fountain") == World.getDungeonManager().level[dungeonLevel].map[x][y].terrain.name)
        int test = 0;

    if (x <= 0 || y <= 0 || x >= (DUNGEON_SIZE_W - 1) || y >= (DUNGEON_SIZE_W - 1))
        return 0;

    else if (item->stackable())
    {
        return DropStackableItem(monster, item, x, y);
    }
    // dont drop if there is an item there or the ground is not floor or bridge
    else if (World.getDungeonManager().level[dungeonLevel].map[x][y].getItem() ||
        (World.getDungeonManager().level[dungeonLevel].map[x][y].terrain.type != dfloor &&
        World.getDungeonManager().level[dungeonLevel].map[x][y].terrain.type != bridge))
    {
        return 0;
    }
    else if (std::string("a fountain") == World.getDungeonManager().level[dungeonLevel].map[x][y].terrain.name)
        return 0;
    else if (std::string("a teleport") == World.getDungeonManager().level[dungeonLevel].map[x][y].terrain.name)
        return 0;
    else
        World.getDungeonManager().level[dungeonLevel].map[x][y].AssignItem(item);
    return 1;
}

//Spiral drop
int MonsterItems::AttemptDropItem(MonsterData *monster, Item *item, int x, int y)
{
    int xx = 0;
    int yy = 0;

    int lr_max = 0;
    int up_max = 0;

    int up = 1;
    int left = 1;

    while (DropItem(monster, item, x + xx, y + yy) != 1)
    {
        if (yy == up_max && xx == lr_max) //down
        {
            up = 0; left = 1; up_max++; lr_max++; yy--;
        }

        else if (yy == -up_max && xx != -lr_max && left == 1 && up == 0)  //left
        {
            left = 1; xx--;
        }

        else if (yy == -up_max && xx == -lr_max && left == 1 && up == 0) //up (bottom left)
        {
            up = 1; yy++;
        }

        else if (yy != up_max && xx == -lr_max && up == 1 && left == 1) //up (left)
        {
            up = 1; yy++;
        }

        else if (yy == up_max && xx == -lr_max && up == 1 && left == 1) //right (top)
        {
            xx++; left = 0;
        }

        else if (yy == up_max && xx != lr_max && up == 1 && left == 0)
        {
            xx++;
        }

        else if (yy != up_max && xx != lr_max && up == 0 && left == 1)
        {
            yy--;
        }
        else
        {
            if (monster->isPlayer())
                World.getTextManager().newLine("No room. ");
            //	throw std::exception("No room to drop any more items");
            return 0; //failure - dungeon is full
        }
    }

    return 1; //success
}

int MonsterItems::moveFromInvToEquipList(MonsterData *monster)
{
    return 1;
}

int MonsterItems::moveFromEquipToInvList(MonsterData *monster)
{
    return 1;
}

int MonsterItems::moveFromInvToAllList(MonsterData *monster)
{
    return 1;
}

int MonsterItems::moveFromAllToInvList(MonsterData *monster)
{
    return 1;
}

bool MonsterItems::isBetter(MonsterData &monster, Item &item)
{
    if (item.type == weapon)
    {
        Item * w = World.getMonsterManager().monsterItems.GetEquipment(&monster, weapon);

        if (!w || item.getAverage_h2h() > w->getAverage_h2h())
        {
            return true;
        }
    }
    else if (item.type == armour)
    {
        Item * w = World.getMonsterManager().monsterItems.GetEquipment(&monster, armour);

        if (!w || item.absorb_bonus > w->absorb_bonus)
        {
            return true;
        }
    }
    else if (item.type == shield)
    {
        Item * w = World.getMonsterManager().monsterItems.GetEquipment(&monster, shield);

        if (!w || item.absorb_bonus > w->absorb_bonus)
        {
            return true;
        }
    }
    else if (item.type == projectileWeapon)
    {
        Item * w = World.getMonsterManager().monsterItems.GetEquipment(&monster, projectileWeapon);

        if (!w || item.getAverage_thr() > w->getAverage_thr())
        {
            return true;
        }
    }
    return false;
}
