// ItemManager.cpp: implementation of the ItemManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ItemManager.h"

#include "numberGenerator.h"

#include <iostream>
#include <fstream>


using namespace Random;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

int ItemManager::keyLabels[10];
int ItemManager::KeySpecial[10];

ItemManager::ItemManager()
{

}

ItemManager::~ItemManager()
{
    //all_items.clear();
}


int ItemManager::Initialise()
{
    all_items.clear();
    try{

        makeKeysLabels();
    }
    catch (...)
    {
        throw std::exception("unknown exception in ItemManager::initialise");
    }
    return 1;
}

Item * ItemManager::CreateRandomItem(int level)
{
    //use level to get high level items - not use at this stage

    int type = getInt(7, 1);

    //item.CreateItem((eItemType) type,level);
    Item* item = CreateItem(level, type);

    if (item->type == projectile)
        item->itemNumber[1] = Random::getInt(10, 5);

    all_items.push_back(*item);

    return &all_items.back();

}

Item * ItemManager::CreateItem(int level, int type, int secondary_type)
{
    if (type == projectileWeapon)
    {
        ProjectileItem item;
        item.CreateItem((eItemType)type, level, secondary_type);
        item.ref = all_items.size();
        all_items.push_back(item);
#ifdef _DEBUG
        //item.identified=1;
#endif
        return &all_items.back();
    }
    else if (type == projectile)
    {
        ProjectileItem item;
        item.CreateItem((eItemType)type, level, secondary_type);
        item.ref = all_items.size();
        all_items.push_back(item);
#ifdef _DEBUG
        //item.identified=1;
#endif
        return &all_items.back();
    }
    else
    {
        Item item;
        item.ref = all_items.size();
        item.CreateItem((eItemType)type, level, secondary_type);
        all_items.push_back(item);
#ifdef _DEBUG
        //item.identified=1;
#endif

        return &all_items.back();
    }
}


int ItemManager::CalculateBrandDamage(Item* item, eBrandType brandType, int strength)
{
    int nDice = 0;
    int tDice = 0;
    int Damage = 0;

    switch (brandType)
    {
    case bFire:
    case bFrost:
    case bLightning:
    case bAcid:	  tDice = 6;	nDice = strength; break;

    case bPoison: tDice = 3;	nDice = strength; break;
    }

    for (int i = 0; i < nDice; i++)
        Damage += getInt(tDice + 1, 1);

    return Damage;
}


void ItemManager::makeKeysLabels()
{
    bool unique = true;
    int num;

    //make standard dungeon keys
    for (int i = 0; i < 10; i++)
    {
        do
        {
            num = getInt(20, 0);
            int test = Random::primeNumbers[num];

            for (int t = 0; t < 10; t++)
            {
                if (test == keyLabels[t])
                {
                    unique = false;
                    break;
                }
                unique = true;
            }

        } while (!unique);

        keyLabels[i] = primeNumbers[num];
    }


    //make special level keys
    for (int i = 0; i < 10; i++)
    {
        do
        {
            num = getInt(20, 0);
            int test = Random::primeNumbers[num];

            for (int t = 0; t < 10; t++)
            {
                if (test == keyLabels[t] || test == KeySpecial[t])
                {
                    unique = false;
                    break;
                }
                unique = true;
            }

        } while (!unique);

        KeySpecial[i] = primeNumbers[num];
    }

#ifdef _DEBUG
    std::ofstream ofile;

    ofile.open("Debug Files\\keys.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open keys.txt");


    ofile << "Keys  For debug purposes only" << std::endl;

    for (int t = 0; t < 10; t++)
    {
        ofile << "Standard Key " << keyLabels[t] << std::endl;
    }
    for (int t = 0; t < 10; t++)
    {
        ofile << "Special Key  " << KeySpecial[t] << std::endl;
    }

    ofile << std::endl;


    ofile.close();
#endif
}

void ItemManager::PrintItems()
{
    //printf items
#ifdef _DEBUG
    std::ofstream ofile;

    ofile.open("Debug Files\\items.txt");
    if (!ofile.is_open())
        throw std::exception("Could not open items.txt");

    ofile << "Items" << std::endl;

    ofile << "Name Level Bonus" << std::endl;

    ITEMLIST::iterator item;

    for (item = all_items.begin(); item != all_items.end(); item++)
    {
        bool id = item->identified;

        item->identified = 1;
        ofile << item->GetName();
        ofile << std::endl;
        item->identified = id;
    }
    ofile.close();

#endif

}