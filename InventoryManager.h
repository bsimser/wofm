// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#if !defined(AFX_INVENTORYMANAGER_H__65C8D882_9323_46EC_8A13_BCC5962EC377__INCLUDED_)
#define AFX_INVENTORYMANAGER_H__65C8D882_9323_46EC_8A13_BCC5962EC377__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class InventoryManager  
{
public:
    enum EInvState
    {
        eDisplay,
        eEquip,
        eDrop,
        eShowEquipment
    };

	InventoryManager();

	int ShowInventory(int type);
	int EquipMessage();
	int Equip(bool *keys);
	int InventoryCommand(bool *keys);
	int EquipItem(int item);
	int DropMessage();
	int Drop(bool *keys);
	int DropItem(int item);
	int Unequip(int item);
    void setState(EInvState state);

private:
    EInvState   mState;
	bool        mShowEquip;
	bool        mShowInv;
	bool        mShowAll;
};

#endif // !defined(AFX_INVENTORYMANAGER_H__65C8D882_9323_46EC_8A13_BCC5962EC377__INCLUDED_)
