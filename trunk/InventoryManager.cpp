// InventoryManager.cpp: implementation of the InventoryManager class.
//
//////////////////////////////////////////////////////////////////////

#include "InventoryManager.h"

#include "WorldBuilder.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


InventoryManager::InventoryManager()
{
    showEquip = false;
    showInv   = false;
    showAll   = false;
}

InventoryManager::~InventoryManager()
{
}

int InventoryManager::InventoryCommand(bool *keys)
{
    if (keys[VC_X] || keys[VK_ESC]) //inventory look
    {
        keys[VK_ESC] = false;
        keys[VC_X] = false;

        if (state == equip || state == drop)
        {
            ShowInventory(-1);
            state = display;
            return 1;
        }
        else if (state == display)
            return 0;
    }

    else if (state == equip)
    {
        if (Equip(keys))
            return 2;
    }

    else if (state == drop)
    {
        if (Drop(keys))
            return 2;
    }

    else if (keys[VK_U]) //equip/unequip
    {
        EquipMessage();
        state = equip;
        keys[VK_U] = false;
    }

    else if (keys[VK_D]) //drop
    {
        DropMessage();
        state = drop;
        keys[VK_D] = false;
    }

    else if (keys[VK_E]) //toggle keys
    {
        showEquip = true;
        showAll = showInv = false;

        ShowInventory(1);
        keys[VK_E] = false;
    }
    else if (keys[VK_I]) //toggle keys
    {
        showInv = true;
        showAll = showEquip = false;
        ShowInventory(0);
        keys[VK_E] = false;
    }
    else if (keys[VK_A]) //toggle keys
    {
        showAll = !showAll;
        ShowInventory(-1);
        keys[VK_E] = false;
    }

    return 1;
}

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
                state = display;
            }
            keys[i] = false;
        }
    }
    return 1;
}

int InventoryManager::DropItem(int item)
{
    if (WorldBuilder::monsterManager.Player()->NextAction(WorldBuilder::actionManager.UpdateAction(&WorldBuilder::monsterManager.Player()->action, aDrop, item)))
        return 1;

    return 0;
}


int InventoryManager::DropMessage()
{
    WorldBuilder::textManager.SetDisplayLine(39, "Choose a item to drop, [x] cancel");

    return 1;
}

int InventoryManager::ShowInventory(int type)
{
    WorldBuilder::textManager.ClearDisplayLines();

    if (type == 0)
    {
        showInv = true;
        showEquip = false;
    }
    else if (type == 1)
    {
        showEquip = true;
        showInv = false;
    }

    if (showEquip)
    {
        WorldBuilder::textManager.SetDisplayLine(0, "Equipment");
        WorldBuilder::textManager.SetDisplayLine(1, "=========");
        WorldBuilder::textManager.SetDisplayLine(39, "[u] Unequip, [d] Drop, [i] Inventory, [a] Show All - [x] to exit");
    }
    else
    {
        WorldBuilder::textManager.SetDisplayLine(0, "Inventory");
        WorldBuilder::textManager.SetDisplayLine(1, "=========");
        WorldBuilder::textManager.SetDisplayLine(39, "[u] Use\\Equip, [d] Drop, [e] Equipment, [a] Show All - [x] to exit");
    }

    ITEMLIST::iterator it;

    ITEMLIST *inventory = &WorldBuilder::monsterManager.Player()->inventory;

    int i = 3;

    int count = 0x61; //'a'

    if (showEquip)
    {
        /*	sprintf(buf,"Head:            - ");
            WorldBuilder::textManager.SetDisplayLine(3,buf);
            sprintf(buf,"Body:            - ");
            WorldBuilder::textManager.SetDisplayLine(4,buf);
            sprintf(buf,"Cloak:           - " );
            WorldBuilder::textManager.SetDisplayLine(5,buf);
            sprintf(buf,"Left Arm:        - ");
            WorldBuilder::textManager.SetDisplayLine(6,buf);
            sprintf(buf,"Right Arm:       - ");
            WorldBuilder::textManager.SetDisplayLine(7,buf);
            sprintf(buf,"Left Hand:       - ");
            WorldBuilder::textManager.SetDisplayLine(8,buf);
            sprintf(buf,"Right Hand:      - ");
            WorldBuilder::textManager.SetDisplayLine(9,buf);
            sprintf(buf,"Feet:            - ");
            WorldBuilder::textManager.SetDisplayLine(10,buf);
            sprintf(buf,"Missile Weapon:  - ");
            WorldBuilder::textManager.SetDisplayLine(11,buf);
            sprintf(buf,"Missiles:        - " );
            WorldBuilder::textManager.SetDisplayLine(12,buf);*/
    }

    inventory->sort();
    i = 3;
    for (it = inventory->begin(); it != inventory->end(); it++, i++)
    {
        if (showAll)
        {
            WorldBuilder::textManager.SetDisplayLine(i, "(%c) %s", count, it->GetName());
        }
        else if (showEquip)
        {
            if (it->equipped == 1)
                WorldBuilder::textManager.SetDisplayLine(i, "(%c) %s", count, it->GetName());
            else i--;
        }
        else if (showInv)
        {
            if (it->equipped != 1)
                WorldBuilder::textManager.SetDisplayLine(i, "(%c) %s", count, it->GetName());
            else i--;
        }
        count++;
    }
    return 1;
}

int InventoryManager::EquipMessage()
{
    WorldBuilder::textManager.SetDisplayLine(39, "Choose a item to equip\\unequip, [x] cancel");
    return 1;
}

int InventoryManager::Equip(bool *keys)
{
    for (int i = 65; i < 91; i++) //a - w
    {
        if (keys[i] == true)
        {
            WorldBuilder::monsterManager.monsterItems.EquipItem(&(*WorldBuilder::monsterManager.Player()), i - 65);
            state = display;
            keys[i] = false;
            ShowInventory(-1);
        }
    }
    return 1;
}

int InventoryManager::Unequip(int item)
{
    ITEMLIST::iterator it;

    ITEMLIST *inventory = &WorldBuilder::monsterManager.Player()->inventory;
    monsterData *player = WorldBuilder::monsterManager.Player();

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