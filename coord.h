
#if !defined(COORD_H_123__INCLUDED_)
#define COORD_H_123__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <vector>
typedef struct 
{
	int x;
	int y;
}coord;

typedef std::vector<coord> COORDLIST;

class coordlist
{
public:
	coordlist(){};

COORDLIST coords;
};

#endif