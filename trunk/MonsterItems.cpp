// MonsterItems.cpp: implementation of the MonsterItems class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "MonsterItems.h"
#include "numberGenerator.h"

using namespace Random;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MonsterItems::MonsterItems()
{

}

MonsterItems::~MonsterItems()
{

}
///////////////////////////////////////////


Item * MonsterItems::GetEquipment(monsterData *monster, eItemType equip_type)
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

Item * MonsterItems::GetInventoryItem(monsterData *monster, eItemType equip_type)
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

int MonsterItems::EquipMonster(monsterData *monster, int level)
{
    std::string name = monster->monster.name;


    monster->inventory.clear();

    Item * item;

    if (name.find("Servant") != -1) //servants do not use armour and have maces or swords
    {
        if (Random::getInt(2, 0) == 0)
            item = WorldBuilder::itemManager.CreateItem(level, weapon, sword);
        else
            item = WorldBuilder::itemManager.CreateItem(level, weapon, mace);
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
        case 0: p = bolt; pw = crossbow; strcpy(monster->monster.name, "orc bolter"); break;
        case 1: p = arrow; pw = bow;     strcpy(monster->monster.name, "orc archer"); break;
        case 2: p = arrow; pw = longbow; strcpy(monster->monster.name, "orc archer"); break;
        }

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, projectile, p));
        monster->inventory.back().itemNumber[1] = getInt(20, 10);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, projectileWeapon, pw));
        monster->inventory.back().equipped = 1;

        if (Random::getInt(2, 0) == 0)
            item = WorldBuilder::itemManager.CreateItem(level, weapon, sword);
        else
            item = WorldBuilder::itemManager.CreateItem(level, weapon, mace);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, armour, leather));
        monster->inventory.back().equipped = 1;
    }
    else if (name.find("warrior") != -1 || name.find("sentry") != -1)
    {
        item = WorldBuilder::itemManager.CreateItem(level, weapon);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, armour));
        monster->inventory.back().equipped = 1;

        if (Random::getInt(3, 0) == 0)//add shield
        {
            monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, shield, 0));
            monster->inventory.back().equipped = 1;
        }

    }
    else if (name.find("butcher") != -1)
    {
        item = WorldBuilder::itemManager.CreateItem(level, weapon, cleaver);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, armour, bloodyapron));
        monster->inventory.back().equipped = 1;

    }

    else if (name.find("Zagor") != -1)
    {
        item = WorldBuilder::itemManager.CreateItem(level, weapon, staff);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, armour, blackcloak));
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, cards, 0));
    }
    else //create random weapon and armour
    {
        item = WorldBuilder::itemManager.CreateItem(level, weapon, sword);
        monster->inventory.push_back(*item);
        monster->inventory.back().equipped = 1;

        monster->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level, armour));
        monster->inventory.back().equipped = 1;
    }
    return 1;
}

int MonsterItems::EquipPlayer(monsterData *player)
{
    int level = 0;

    player->inventory.clear();

    //weapon
    Item * item = WorldBuilder::itemManager.CreateItem(level, weapon);
    item->identified = 1;
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;

    //armour
    item = WorldBuilder::itemManager.CreateItem(level, armour);
    item->identified = 1;
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;

    //player->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level,cards,0)); //give magic to player

    //shield
    //player->inventory.push_back(*WorldBuilder::itemManager.CreateItem(level,shield,0));
    //player->inventory.back().equipped=1;

    //for(int i=0;i<20;i++)
    {
        //	item = WorldBuilder::itemManager.CreateItem(i,key);
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
    item = WorldBuilder::itemManager.CreateItem(level, projectileWeapon, pw);
    item->identified = 1;
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;

    //projectiles
    item = WorldBuilder::itemManager.CreateItem(level, projectile, p);
    item->identified = 1;
    item->itemNumber[1] = getInt(12, 6);
    player->inventory.push_back(*item);
    player->inventory.back().equipped = 1;


    return 1;
}

int MonsterItems::EquipItem(monsterData *monster, int item)
{
    ITEMLIST::iterator it;

    ITEMLIST *inventory = &monster->inventory;

    int i = 0;

    int gogo = inventory->size();

    for (it = inventory->begin(); it != inventory->end(); it++, i++)
    {
        if (i == item) //found
        {
            if (!it->Wearable())
                return 0; //can not be equipped

            if (it->equipped == 1) //unequip
            {
                it->equipped = 0;

            }
            else	//equip //unequip privious item
            {
                switch (it->type)
                {
                case projectile: {
                                     Item *eq = GetEquipment(monster, projectile);
                                     if (eq != NULL)
                                         eq->equipped = 0;
                                     break;
                }
                case projectileWeapon: {
                                           Item *eq = GetEquipment(monster, projectileWeapon);
                                           if (eq != NULL)
                                               eq->equipped = 0;
                                           break;
                }


                case armour: {
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

int MonsterItems::RemoveItem(monsterData *monster)
{
    return 1;
}

int MonsterItems::UseItem(monsterData *monster)
{
    return 1;
}

int MonsterItems::DropItem(monsterData *monster, int item, bool erace_from_inv)
{
    ITEMLIST::iterator it;
    ITEMLIST::iterator it_all;

    ITEMLIST *all = &WorldBuilder::itemManager.all_items;

    coord *pos = &monster->pos;

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


int MonsterItems::PickupItem(monsterData *monster)
{

    if (!WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[monster->pos.x][monster->pos.y].getItem()) //check if item exists
        return 0;


    if (monster->inventory.size() > 25)
    {
        if (monster->isPlayer())
            WorldBuilder::textManager.newLine("You have no room in you pack. ");
        return 0;
    }


    if (monster->isPlayer()) //id item for player
    {
        Item * item_to_copy = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[monster->pos.x][monster->pos.y].getItem();
        if (item_to_copy->type == lockedChest || item_to_copy->type == openChest)
        {
            WorldBuilder::textManager.newLine("You cannot lift it. ");
            return 0;
        }
        item_to_copy->identified = 1;
    }
    //get copy of item

    Item item(*WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[monster->pos.x][monster->pos.y].getItem());



    //delete reference from map but item is still in all items list
    WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[monster->pos.x][monster->pos.y].RemoveItemRef();

    if (monster->isPlayer())
        WorldBuilder::textManager.newLine("You pick up the %s%s", item.name, item.itemNumber[1] > 1 ? "s. " : ". ");
    else
        WorldBuilder::textManager.newLine("The %s picks up the %s%s", monster->monster.name, item.name, item.itemNumber[1] > 1 ? "s. " : ". ");

    //give item to monster

    //check if stackable and add to existing stack
    if (item.Stackable())
    {
        ITEMLIST::iterator it;
        bool stacked = false;
        for (it = monster->inventory.begin(); it != monster->inventory.end(); it++)
        {
            //check if same type, secondary ype and skill bonus
            if (item.type == it->type && item.secondaryType == it->secondaryType && item.skill_bonus == it->skill_bonus &&
                !strcmp(item.prefix, it->prefix) && !strcmp(item.postfix, it->postfix))
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
int MonsterItems::DropRandomItems(monsterData *monster)
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
            WorldBuilder::monsterManager.monsterItems.DropItem(monster, i, false);
            it = monster->inventory.erase(it);
        }
        else if (it->type == cards)
        {
            WorldBuilder::monsterManager.monsterItems.DropItem(monster, i, false);
            it = monster->inventory.erase(it);
        }
        else
        {
            it++;
            i++;
        }
        /*else if(rit->type == gold)
        {
        WorldBuilder::monsterManager.monsterItems.DropItem(monster,i);
        }*/
    }

    int rand_drop = getInt(monster->inventory.size() + 1, 0);

    if (rand_drop == monster->inventory.size()) //no drop (0=0)
        return 0;

    WorldBuilder::monsterManager.monsterItems.DropItem(monster, rand_drop);

    if (monster->inventory.size() > 0) //more items to drop
        DropRandomItems(monster);

    return 1;

}

///////////private///////////////////
int MonsterItems::DropStackableItem(monsterData *monster, Item *item, int x, int y)
{
    Item* floor_item = WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].getItem();
    if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].terrain.type != dfloor
        &&WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].terrain.type != bridge)
    {
        return 0;
    }

    else if (floor_item == NULL) //no item
    {
        WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].AssignItem(item);
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

int MonsterItems::DropItem(monsterData *monster, Item *item, int x, int y)
{
    if (x <= 0 || y <= 0 || x >= (DUNGEON_SIZE_W - 1) || y >= (DUNGEON_SIZE_W - 1))
        return 0;

    else if (item->Stackable())
    {
        return DropStackableItem(monster, item, x, y);
    }

    else if (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].getItem() ||
        (WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].terrain.type != dfloor
        &&WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].terrain.type != bridge))
    {
        return 0;
    }

    else
        WorldBuilder::dungeonManager.level[WorldBuilder::GetCurrentLevel()].map[x][y].AssignItem(item);
    return 1;
}

//Spiral drop
int MonsterItems::AttemptDropItem(monsterData *monster, Item *item, int x, int y)
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
                WorldBuilder::textManager.newLine("No room. ");
            //	throw std::exception("No room to drop any more items");
            return 0; //failure - dungeon is full
        }
    }

    return 1; //success
}

int MonsterItems::moveFromInvToEquipList(monsterData *monster)
{
    return 1;
}

int MonsterItems::moveFromEquipToInvList(monsterData *monster)
{
    return 1;
}

int MonsterItems::moveFromInvToAllList(monsterData *monster)
{
    return 1;
}

int MonsterItems::moveFromAllToInvList(monsterData *monster)
{
    return 1;
}