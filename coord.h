// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#if !defined(COORD_H_123__INCLUDED_)
#define COORD_H_123__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>

class Coord
{
public:
    Coord() : x(0), y(0){};
    Coord(int _x, int _y) : x(_x), y(_y){};
	int x;
	int y;
};

typedef std::vector<Coord> COORDLIST;

#endif