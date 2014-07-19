// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_UNDEADMONSTER_H__071C5F21_C9DB_4B4D_B14A_F8EF2B910355__INCLUDED_)
#define AFX_UNDEADMONSTER_H__071C5F21_C9DB_4B4D_B14A_F8EF2B910355__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Monster.h"

enum eUndead
{
	skeleton,
	zombie,
	ghoul,
	vampire
};

class UndeadMonster : public Monster  
{
public:
	UndeadMonster();
	virtual ~UndeadMonster();

	int Create(int type,int level);

private:
	int createSkeleton();
	int createZombie();
	int createGhoul();
	int createVampire();

};

#endif // !defined(AFX_UNDEADMONSTER_H__071C5F21_C9DB_4B4D_B14A_F8EF2B910355__INCLUDED_)
