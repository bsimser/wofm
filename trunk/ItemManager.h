// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_ITEMMANAGER_H__B0BE6CF1_CF3C_46EC_8082_9616CAF8BCC4__INCLUDED_)
#define AFX_ITEMMANAGER_H__B0BE6CF1_CF3C_46EC_8082_9616CAF8BCC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"
#include <deque>

//item types
#include ".\projectileitem.h"

// IMPORTANT - do not use a container that reorganises its internal memory, because I lazily keep pointers to the internal structures.
typedef std::deque<Item> ITEMLIST;

class ItemManager
{
public:
    int Initialise();

    Item * CreateRandomItem(int level);
    Item * CreateItem(int level, int type, int secondary_type = 0);
    Item * DuplicateItem(Item * item);
    void makeKeysLabels();

    ITEMLIST all_items;

    void PrintItems();

    int CalculateBrandDamage(Item* item, eBrandType brandType, int strength);

    int keyLabels[10];
    int KeySpecial[10];
};

#endif // !defined(AFX_ITEMMANAGER_H__B0BE6CF1_CF3C_46EC_8082_9616CAF8BCC4__INCLUDED_)
