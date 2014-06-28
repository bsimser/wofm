// Monster.h: interface for the Monster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MONSTER_H__88C4B4AD_2D3B_4EB8_ABA1_07FEFBF7C2F1__INCLUDED_)
#define AFX_MONSTER_H__88C4B4AD_2D3B_4EB8_ABA1_07FEFBF7C2F1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "action.h"
#include "DungeonGenerator.h"

#include ".\resistancebrands.h"
#include ".\effects.h"
#include <vector>


class Monster  : public ResistanceBrands, public Effects
{
public:
	Monster();
	Monster(const Monster& rhs);
	virtual ~Monster();

	virtual int Create(int type,int level, int level_variation=0);

	virtual int createPlayer();
	virtual int createOrc(int level);
	virtual int createWarlock();
	virtual int createSpecial(int level);
	virtual int createBridgeMaster(bool wereRat = false);

	int dead();
	int MaxStamina()    {return max_stamina; };
	void setColor(int c1, int c2, int c3);
	int newPos(int x,int y);
	int GetType()   {return type; };

    void setDescription(const std::string & about);
    const std::string getDescription() const;

	int symbol;
	int color1,color2,color3;

	int stamina;
	int skill;
	int luck;

	int sight_range;
	int asleep_sight_Range;

	char name[64];

	bool humanoid;
    bool special;
    bool undead;

	int experience;
	int max_stamina;

	int speed;
private:

	int mLevelVariation;
	int mLevel;
	int type;
    int description;
    static std::vector<std::string> descriptions;
};

#endif // !defined(AFX_MONSTER_H__88C4B4AD_2D3B_4EB8_ABA1_07FEFBF7C2F1__INCLUDED_)
