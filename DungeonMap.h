// DungeonMap.h: interface for the DungeonMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DUNGEONMAP_H__710662A6_2E7C_4B3D_AE51_8FE8E9811210__INCLUDED_)
#define AFX_DUNGEONMAP_H__710662A6_2E7C_4B3D_AE51_8FE8E9811210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



class DungeonMap  
{
public:
	DungeonMap();
	virtual ~DungeonMap();

	int convert_map(int *old_map);

	struct converted_cell
	{
		char symbol;
		int c1,c2,c3; //colour
	};

	converted_cell map[70][33];
};

#endif // !defined(AFX_DUNGEONMAP_H__710662A6_2E7C_4B3D_AE51_8FE8E9811210__INCLUDED_)
