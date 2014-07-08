// RandomMonster.h: interface for the RandomMonster class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RANDOMMONSTER_H__CAE4E1A7_F2FB_4102_BB1F_424787B1D66F__INCLUDED_)
#define AFX_RANDOMMONSTER_H__CAE4E1A7_F2FB_4102_BB1F_424787B1D66F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Monster.h"

class RandomMonster : public Monster  
{
public:
	RandomMonster();

	int Create(int level);

private:
	int createGoblin(int level);
	int createTroll(int level);
	int createDwarf(int level);
	int createSpider(int level);

};

#endif // !defined(AFX_RANDOMMONSTER_H__CAE4E1A7_F2FB_4102_BB1F_424787B1D66F__INCLUDED_)
