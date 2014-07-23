// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


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
	//Monster(const Monster& rhs);
	virtual ~Monster();

	virtual int Create(int type,int level, int level_variation=0);

	int     createPlayer();
	int     createOrc(int level);
	int     createWarlock();
	int     createSpecial(int level);
    int     createBridgeMaster(bool wereRat = false);
    int     createWerewolf();
    int     createDigger();
    void    createCrocodile();

	int     dead();
	void    setColor(int c1, int c2, int c3);
	int     newPos(int x,int y);

	int     GetType()               {return type; };
    int     MaxStamina()            { return max_stamina; };

    void    setDescription(const std::string & about);
    const std::string getDescription() const;

    void    setColour(unsigned char color1, unsigned char color2, unsigned char color3);

	char            symbol;
    unsigned char   color1, color2, color3;
	signed char     stamina;
	unsigned char   skill;
    char            luck;
    unsigned char   sight_range;
    unsigned char   asleep_sight_Range;
    std::string     name;
	bool            humanoid;
    bool            special;
    bool            undead;
	int             experience;
    unsigned char   max_stamina;
    signed char     speed;

protected:
    unsigned char   mLevelVariation;
    unsigned char   mLevel;
    unsigned char   type;
    unsigned char   description;
    static std::vector<std::string> descriptions;
};

#endif // !defined(AFX_MONSTER_H__88C4B4AD_2D3B_4EB8_ABA1_07FEFBF7C2F1__INCLUDED_)
