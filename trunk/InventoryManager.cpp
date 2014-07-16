// InventoryManager.cpp: implementation of the InventoryManager class.
//
//////////////////////////////////////////////////////////////////////

#include "InventoryManager.h"
#include "WorldBuilder.h"
#include <algorithm>

// --------------------------------------------------------------------------------------------------------------------------------

InventoryManager::InventoryManager():
    mState(eDisplay),
    mShowEquip(false),
    mShowInv(false),
    mShowAll(false)
{
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::InventoryCommand(bool *keys)
{
    if (keys[VC_X] || keys[VK_ESC]) //inventory look
    {
        keys[VK_ESC] = false;
        keys[VC_X] = false;

        if (mState == eEquip || mState == eDrop)
        {
            ShowInventory(-1);
            mState = eDisplay;
            return 1;
        }
        else if (mState == eDisplay)
        {
            return 0;
        }
    }

    else if (mState == eEquip)
    {
        if (Equip(keys))
            return 2;
    }

    else if (mState == eDrop)
    {
        if (Drop(keys))
            return 2;
    }

    else if (keys[VK_U]) //equip/unequip
    {
        EquipMessage();
        mState = eEquip;
        keys[VK_U] = false;
    }

    else if (keys[VK_D]) //drop
    {
        DropMessage();
        mState = eDrop;
        keys[VK_D] = false;
    }

    else if (keys[VK_E]) //toggle keys
    {
        mShowEquip = true;
        mShowAll = mShowInv = false;

        ShowInventory(1);
        keys[VK_E] = false;
    }
    else if (keys[VK_I]) //toggle keys
    {
        mShowInv = true;
        mShowAll = mShowEquip = false;
        ShowInventory(0);
        keys[VK_E] = false;
    }
    else if (keys[VK_A]) //toggle keys
    {
        mShowAll = !mShowAll;
        ShowInventory(-1);
        keys[VK_E] = false;
    }

    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::Drop(bool *keys)
{
    int find = 0;

    for (int i = 65; i < 91; i++) //a - w
    {
        if (keys[i] == true)
        {
            Unequip(i - 65);

            if (DropItem(i - 65))
            {
                ShowInventory(-1);
                mState = eDisplay;
            }
            keys[i] = false;
        }
    }
    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::DropItem(int item)
{
    if (World.getMonsterManager().Player()->NextAction(World.getActionManager().UpdateAction(&World.getMonsterManager().Player()->action, aDrop, item)))
        return 1;

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::DropMessage()
{
    World.getTextManager().SetDisplayLine(39, "Choose a item to drop, [x] cancel");

    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::ShowInventory(int type)
{
    World.getTextManager().ClearDisplayLines();

    if (type == 0)
    {
        mShowInv = true;
        mShowEquip = false;
    }
    else if (type == 1)
    {
        mShowEquip = true;
        mShowInv = false;
    }

    if (mShowEquip)
    {
        World.getTextManager().SetDisplayLine(0, "Equipment");
        World.getTextManager().SetDisplayLine(1, "=========");
        World.getTextManager().SetDisplayLine(39, "[u] Unequip, [d] Drop, [i] Inventory, [a] Show All - [x] to exit");
    }
    else
    {
        World.getTextManager().SetDisplayLine(0, "Inventory");
        World.getTextManager().SetDisplayLine(1, "=========");
        World.getTextManager().SetDisplayLine(39, "[u] Use\\Equip, [d] Drop, [e] Equipment, [a] Show All - [x] to exit");
    }

    ITEMLIST::iterator it;

    ITEMLIST *inventory = &World.getMonsterManager().Player()->inventory;

    int i = 3;

    int count = 0x61; //'a'

    if (mShowEquip)
    {
        /*	sprintf(buf,"Head:            - ");
            World.getTextManager().SetDisplayLine(3,buf);
            sprintf(buf,"Body:            - ");
            World.getTextManager().SetDisplayLine(4,buf);
            sprintf(buf,"Cloak:           - " );
            World.getTextManager().SetDisplayLine(5,buf);
            sprintf(buf,"Left Arm:        - ");
            World.getTextManager().SetDisplayLine(6,buf);
            sprintf(buf,"Right Arm:       - ");
            World.getTextManager().SetDisplayLine(7,buf);
            sprintf(buf,"Left Hand:       - ");
            World.getTextManager().SetDisplayLine(8,buf);
            sprintf(buf,"Right Hand:      - ");
            World.getTextManager().SetDisplayLine(9,buf);
            sprintf(buf,"Feet:            - ");
            World.getTextManager().SetDisplayLine(10,buf);
            sprintf(buf,"Missile Weapon:  - ");
            World.getTextManager().SetDisplayLine(11,buf);
            sprintf(buf,"Missiles:        - " );
            World.getTextManager().SetDisplayLine(12,buf);*/
    }

    //std::sort(inventory->begin(), inventory->end());
    inventory->sort();
    i = 3;

    for (it = inventory->begin(); it != inventory->end(); it++, i++)
    {
        if (mShowAll)
        {
            World.getTextManager().SetDisplayLine(i, "(%c) %s", count, it->GetName().c_str());
        }
        else if (mShowEquip)
        {
            if (it->equipped == 1)
                World.getTextManager().SetDisplayLine(i, "(%c) %s", count, it->GetName().c_str());
            else 
                i--;
        }
        else if (mShowInv)
        {
            if (it->equipped != 1)
                World.getTextManager().SetDisplayLine(i, "(%c) %s", count, it->GetName().c_str());
            else 
                i--;
        }
        count++;
    }
    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::EquipMessage()
{
    World.getTextManager().SetDisplayLine(39, "Choose a item to use\\unequip, [x] cancel");
    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::Equip(bool *keys)
{
    for (int i = 65; i < 91; i++) //a - w
    {
        if (keys[i] == true)
        {
            int ret = World.getMonsterManager().monsterItems.EquipItem(&(*World.getMonsterManager().Player()), i - 65);
            mState = eDisplay;
            keys[i] = false;
            ShowInventory(-1);
            return ret;
        }
    }
    return 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

int InventoryManager::Unequip(int item)
{
    ITEMLIST::iterator it;

    ITEMLIST *inventory = &World.getMonsterManager().Player()->inventory;
    MonsterData *player = World.getMonsterManager().Player();

    int i = 0;

    for (it = inventory->begin(); it != inventory->end(); it++, i++)
    {
        if (i == item) //found
        {
            if (it->equipped == 1) //unequip
            {
                it->equipped = 0;
                ShowInventory(-1);
            }
            return 1;
        }
    }
    return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------

void InventoryManager::setState(EInvState newState)
{
    mState = newState;
}
