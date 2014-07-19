// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_MONSTERITEMS_H__FB40F0AB_EA77_47C4_BBA8_3CAECF1DB255__INCLUDED_)
#define AFX_MONSTERITEMS_H__FB40F0AB_EA77_47C4_BBA8_3CAECF1DB255__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "monsterManager.h"
#include "item.h"

class MonsterData;
class Item;

class MonsterItems  
{
public:

	int EquipMonster(MonsterData *monster,int level);
	int EquipPlayer(MonsterData *player);

	int EquipItem(MonsterData *monster,int item);
    int RemoveItem(MonsterData *monster, int item);
    int UseItem(MonsterData *monster, Item & item);
	int DropItem(MonsterData *monster,int item, bool erace_from_inv = true);
	int PickupItem(MonsterData *monster);
	int DropRandomItems(MonsterData *monster);

	int AttemptDropItem(MonsterData *monster,Item *item,int x, int y);

	Item * GetEquipment(MonsterData *monster,eItemType equip_type);
	Item * GetInventoryItem(MonsterData *monster,eItemType equip_type);

    bool isBetter(MonsterData &monster, Item &item);
private:

	int DropItem(MonsterData *monster,Item *item,int x, int y);
	int DropStackableItem(MonsterData *monster,Item *item,int x, int y);

	int moveFromInvToEquipList(MonsterData *monster);
	int moveFromEquipToInvList(MonsterData *monster);
	int moveFromInvToAllList(MonsterData *monster);
	int moveFromAllToInvList(MonsterData *monster);
};

#endif // !defined(AFX_MONSTERITEMS_H__FB40F0AB_EA77_47C4_BBA8_3CAECF1DB255__INCLUDED_)
