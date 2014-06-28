// InventoryManager.h: interface for the InventoryManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INVENTORYMANAGER_H__65C8D882_9323_46EC_8A13_BCC5962EC377__INCLUDED_)
#define AFX_INVENTORYMANAGER_H__65C8D882_9323_46EC_8A13_BCC5962EC377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class InventoryManager  
{
public:
	InventoryManager();
	virtual ~InventoryManager();

	int ShowInventory(int type);
	int EquipMessage();
	int Equip(bool *keys);
	int InventoryCommand(bool *keys);
	int EquipItem(int item);
	int DropMessage();
	int Drop(bool *keys);
	int DropItem(int item);
	int Unequip(int item);

private:

	enum invState{
		display,
		equip,
		drop,
		equipment
	};
	invState state;


	bool showEquip;
	bool showInv;
	bool showAll;
};

#endif // !defined(AFX_INVENTORYMANAGER_H__65C8D882_9323_46EC_8A13_BCC5962EC377__INCLUDED_)
