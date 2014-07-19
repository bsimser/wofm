// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_MONSTERINFO_H__986C9E36_E764_4202_9703_0B2CBBA576BF__INCLUDED_)
#define AFX_MONSTERINFO_H__986C9E36_E764_4202_9703_0B2CBBA576BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MonsterInfo  
{
public:
	void	ShowMonsterInfo(MonsterData* monster);
private:
	void	ShowCompleteMonsterInfo(MonsterData* monster);
	void	ShowNormalMonsterInfo(MonsterData* monster);
};

#endif // !defined(AFX_MONSTERINFO_H__986C9E36_E764_4202_9703_0B2CBBA576BF__INCLUDED_)
