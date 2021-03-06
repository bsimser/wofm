// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#if !defined(AFX_LEVELCHANGE_H__D38E4BA9_9542_420F_BF94_A66EB0F1D61A__INCLUDED_)
#define AFX_LEVELCHANGE_H__D38E4BA9_9542_420F_BF94_A66EB0F1D61A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class MonsterData;

class LevelChange  
{
public:
	int LevelChange::ChangeLevel(int dir,MonsterData * monster);

	int ClimbSpecial(int dir);
	int ClimbNormal(int dir);
	int ClimbEncounter(int dir);

	void DebugLevel(int dir);

};

#endif // !defined(AFX_LEVELCHANGE_H__D38E4BA9_9542_420F_BF94_A66EB0F1D61A__INCLUDED_)
