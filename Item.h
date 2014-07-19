// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_ITEM_H__B4B8BC25_65E3_47B9_A8D8_1ACF01097435__INCLUDED_)
#define AFX_ITEM_H__B4B8BC25_65E3_47B9_A8D8_1ACF01097435__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include ".\resistancebrands.h"
#include <string>

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
    gem,
    provisions,
    stake,
    DiMaggio,
    cheese,
    silverArrow,
    carcass,
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
    
	virtual int CreateItem(eItemType type,int level, int secondary_type=no_type);

	void createCorpse();
	void createArmour(int level, int secondary_type);
	void createWeapon(int level, int secondary_type);
	void createKey(int level);
	void createLockedChest(int level);
	void createOpenChest(int level);
	void createGold(int level);
	void createShield(int level);
    void createCards(int level);
    void createProvision();
    void createGem();
    void createStake();
    void createCheese();
    void createCarcass();
    void createDiMaggio();

	void setColor(int c1, int c2, int c3);
    std::string GetName();
    std::string BaseName();

    std::string getPostfix();
    std::string getPrefix();

	void SetDice_h2h(int nDice, int sides);
	void SetDice_thr(int nDice, int sides);

	int  GetAttack_h2h();
	int  GetAttack_thr();
    int getAverage_h2h();
    int getAverage_thr();

	void AddBrand(bool special = false);
	void AddResistance(bool special = false);

    Item& wearable(bool);
    bool wearable();
    bool stackable();
    Item& stackable(bool);

	eItemType type;
	int secondaryType;

	char symbol;

	unsigned int weight;

	unsigned char color1;
    unsigned char color2;
    unsigned char color3;

	signed char skill_bonus;
    signed char defence_bonus;
    signed char absorb_bonus;

	bool identified;
    bool equipped;
    unsigned int ref;
    int itemNumber[3];

protected:
    std::string name;
    std::string prefix;
    std::string postfix;

	int h2hSidesDice;		//I.e 6 = 6 sided dice
	int h2hNumDice;			//I.e 2 = 2 dice i.e Attack = (h2hNumDice)d(h2hSidesDice) +skill_bonus;

	int thrSidesDice;		//I.e 6 = 6 sided dice
	int thrNumDice;			//I.e 2 = 2 dice i.e Attack = (thrNumDice)d(thrSidesDice) +skill_bonus;
	
	bool mStackable;
	bool mWearable;
};

#endif // !defined(AFX_ITEM_H__B4B8BC25_65E3_47B9_A8D8_1ACF01097435__INCLUDED_)
