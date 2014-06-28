// SpecialLevelGenerator.h: interface for the SpecialLevelGenerator class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SPECIALLEVELGENERATOR_H__50E5F382_0B44_41F6_8C67_1A1A2BAA53F0__INCLUDED_)
#define AFX_SPECIALLEVELGENERATOR_H__50E5F382_0B44_41F6_8C67_1A1A2BAA53F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DungeonGenerator.h"

class SpecialLevelGenerator : public DungeonGenerator  
{
public:
	SpecialLevelGenerator();
	virtual ~SpecialLevelGenerator();

	int Create( int _type);

	int CreateMap();

	int makeBarracksDungeon();
	int makeSpecialItemSpot();
};

#endif // !defined(AFX_SPECIALLEVELGENERATOR_H__50E5F382_0B44_41F6_8C67_1A1A2BAA53F0__INCLUDED_)
