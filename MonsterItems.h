// MonsterItems.h: interface for the MonsterItems class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTERITEMS_H__FB40F0AB_EA77_47C4_BBA8_3CAECF1DB255__INCLUDED_)
#define AFX_MONSTERITEMS_H__FB40F0AB_EA77_47C4_BBA8_3CAECF1DB255__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "monsterManager.h"
#include "item.h"

class monsterData;
class Item;

class MonsterItems  
{
public:
	MonsterItems();
	virtual ~MonsterItems();

	int EquipMonster(monsterData *monster,int level);
	int EquipPlayer(monsterData *player);

	int EquipItem(monsterData *monster,int item);
	int RemoveItem(monsterData *monster);
	int UseItem(monsterData *monster);
	int DropItem(monsterData *monster,int item, bool erace_from_inv = true);
	int PickupItem(monsterData *monster);
	int DropRandomItems(monsterData *monster);

	int AttemptDropItem(monsterData *monster,Item *item,int x, int y);

	Item * GetEquipment(monsterData *monster,eItemType equip_type);
	Item * GetInventoryItem(monsterData *monster,eItemType equip_type);

private:

	int DropItem(monsterData *monster,Item *item,int x, int y);
	int DropStackableItem(monsterData *monster,Item *item,int x, int y);

	int moveFromInvToEquipList(monsterData *monster);
	int moveFromEquipToInvList(monsterData *monster);
	int moveFromInvToAllList(monsterData *monster);
	int moveFromAllToInvList(monsterData *monster);
};

#endif // !defined(AFX_MONSTERITEMS_H__FB40F0AB_EA77_47C4_BBA8_3CAECF1DB255__INCLUDED_)
