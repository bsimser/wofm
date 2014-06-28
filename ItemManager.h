// ItemManager.h: interface for the ItemManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEMMANAGER_H__B0BE6CF1_CF3C_46EC_8082_9616CAF8BCC4__INCLUDED_)
#define AFX_ITEMMANAGER_H__B0BE6CF1_CF3C_46EC_8082_9616CAF8BCC4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Item.h"
#include <list>

//item types
#include ".\projectileitem.h"

typedef std::list<Item> ITEMLIST;

class ItemManager  
{
public:
	ItemManager();
	virtual ~ItemManager();

	int Initialise();

	Item * CreateRandomItem(int level);
	Item * CreateItem(int level, int type,int secondary_type=0);
	void makeKeysLabels();

	ITEMLIST all_items;

	void PrintItems();

	static int keyLabels[10];
	static int KeySpecial[10];

	int CalculateBrandDamage(Item* item,eBrandType brandType,int strength);

};

#endif // !defined(AFX_ITEMMANAGER_H__B0BE6CF1_CF3C_46EC_8082_9616CAF8BCC4__INCLUDED_)
