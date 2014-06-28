// Item.h: interface for the Item class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ITEM_H__B4B8BC25_65E3_47B9_A8D8_1ACF01097435__INCLUDED_)
#define AFX_ITEM_H__B4B8BC25_65E3_47B9_A8D8_1ACF01097435__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include ".\resistancebrands.h"


enum eItemType
{
	no_type,
    weapon,
    armour,
	shield,
	projectileWeapon,
	projectile,
	corpse, //random item here and below
	lockedChest,
	openChest,
    cards,
    gold,
	key,
};

enum eWeaponType
{
	sword=1,
	axe,
	mace,
	staff,
	cleaver
};

enum eArmourType
{
	leather=1,
	chainmail,
	platemail,
	blackcloak,
	bloodyapron
};

class Item : public ResistanceBrands
{
public:
	Item();
    Item(const Item& rhs);
    bool operator< (const Item& rhs);
    
	virtual ~Item();

	virtual int CreateItem(eItemType type,int level, int secondary_type=no_type);

	void createCorpse();
	void createArmour(int level,int secondary_type);
	void createWeapon(int level,int secondary_type);
	void createKey(int level);
	void createLockedChest(int level);
	void createOpenChest(int level);
	void createGold(int level);
	void createShield(int level);
	void createCards(int level);

	void setColor(int c1, int c2, int c3);
	virtual char * GetName();

	void SetDice_h2h(int nDice, int sides);
	void SetDice_thr(int nDice, int sides);

	int GetAttack_h2h();
	int GetAttack_thr();
    float getAverage_h2h();
    float getAverage_thr();

	void AddBrand(bool special = false);
	void AddResistance(bool special = false);

	int Wearable(){return wearable;};
	int Stackable(){return stackable;};;

	char name[64];
	char id_name[64];
	char prefix[32];
	char postfix[32];

	char nothing[1];

	eItemType type;
	int secondaryType;

	int symbol;

	int weight;

	short color1;
    short color2;
    short color3;

	int skill_bonus;
	int defence_bonus;
	int absorb_bonus;

	int identified;

	int equipped;

	int ref;


	int itemNumber[3];




protected:
	int h2hSidesDice;		//I.e 6 = 6 sided dice
	int h2hNumDice;			//I.e 2 = 2 dice i.e Attack = (h2hNumDice)d(h2hSidesDice) +skill_bonus;

	int thrSidesDice;		//I.e 6 = 6 sided dice
	int thrNumDice;			//I.e 2 = 2 dice i.e Attack = (thrNumDice)d(thrSidesDice) +skill_bonus;
	
	int stackable;
	int wearable;

private:
	

};

#endif // !defined(AFX_ITEM_H__B4B8BC25_65E3_47B9_A8D8_1ACF01097435__INCLUDED_)
