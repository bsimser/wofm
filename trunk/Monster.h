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
    virtual int createWerewolf();
    virtual int createDigger();
    virtual void createCrocodile();

	int dead();
	int MaxStamina()    {return max_stamina; };
	void setColor(int c1, int c2, int c3);
	int newPos(int x,int y);
	int GetType()   {return type; };

    void setDescription(const std::string & about);
    const std::string getDescription() const;

	char symbol;
    unsigned char color1, color2, color3;

	signed char stamina;
	unsigned char skill;
    char luck;

    unsigned char sight_range;
    unsigned char asleep_sight_Range;

    std::string name;
	//char name[64];

	bool humanoid;
    bool special;
    bool undead;

	int experience;
    unsigned char max_stamina;

    signed char speed;
private:

    unsigned char mLevelVariation;
    unsigned char mLevel;
    unsigned char type;
    unsigned char description;
    static std::vector<std::string> descriptions;
};

#endif // !defined(AFX_MONSTER_H__88C4B4AD_2D3B_4EB8_ABA1_07FEFBF7C2F1__INCLUDED_)
