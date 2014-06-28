// Terrain.cpp: implementation of the Terrain class.
//
//////////////////////////////////////////////////////////////////////

#include "Terrain.h"
#include <string>
#include "NumberGenerator.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Terrain::Terrain():
light(0),
found(0)
{
    makeStone();
}

Terrain::~Terrain()
{

}

int Terrain::Create(int type)
{

    switch (type)
    {
    case stone:			makeStone();		break;
    case rock:			makeRock();			break;
    case dfloor:		makeFloor();		break;
    case lockedStairs:	makeLockedStairs();	break;
    case openStairs:	makeOpenStairs();	break;
    case upStairs:		makeUpStairs();		break;
    case closedDoor:	makeclosedDoor();	break;
    case openDoor:		makeopenDoor();		break;
    case shallowWater:	makeShallowWater(); break;
    case bridge:		makeBridge();		break;
    case deepWater:		makeDeepWater();	break;
    case grass:			makeGrass();		break;
    case tree:			makeTree();			break;
    case deadTree:		makeDeadTree();		break;
    case sand:			makeSand();			break;
    case mountain:		makeMountain();		break;
    case tomb:			makeTomb();			break;
    case specialUp:		makeSpecialUp();	break;
    case specialLocked:	makeSpecialLocked();	break;
    case specialOpen:	makeSpecialOpen();	break;
    case specialItem:	makeSpecialItem();  break;
    case ruins:			makeRuins();        break;
    case fountain:      makeFountain();     break;
    case dryFountain:   makeDryFountain();  break;
    case teleport:      makeTeleport();     break;

    default:  	char err[128];
        sprintf(err, "Invalid terrain type passed into Terrain constructor: %d", type);
        throw std::exception(err);
    }

    return 1;
}

void Terrain::makeStone()
{
    type = stone;

    color1 = 102;
    color2 = 102;
    color3 = 102;

    symbol = '#';

    strcpy(name, "stone");
}

void Terrain::makeFountain()
{
    type = dfloor;

    color1 = 0;
    color2 = 128;
    color3 = 255;

    symbol = '^';

    strcpy(name, "a fountain");

}
void Terrain::makeDryFountain()
{
    type = dfloor;

    color1 = 128;
    color2 = 128;
    color3 = 128;

    symbol = '^';

    strcpy(name, "a dry fountain");

}
void Terrain::makeTeleport()
{
    type = dfloor;

    color1 = 128;
    color2 = 0;
    color3 = 255;

    symbol = '^';
    strcpy(name, "a teleport");
}

void Terrain::makeRuins()
{
    type = stone;

    color1 = 102;
    color2 = 102;
    color3 = 102;

    symbol = '#';

    strcpy(name, "ruins");
}

void Terrain::makeMountain()
{
    type = stone;

    color1 = 102;
    color2 = 102;
    color3 = 102;

    symbol = '#';

    strcpy(name, "Firetop Mountain");
}

void Terrain::makeRock()
{
    type = stone;

    color1 = 134;
    color2 = 85;
    color3 = 70;

    symbol = '#';

    strcpy(name, "rock");
}

void Terrain::makeFloor()
{
    type = dfloor;
    color1 = 102;
    color2 = 102;
    color3 = 102;

    symbol = '.';

    strcpy(name, "dungeon floor");
}

void Terrain::makeSand()
{
    type = dfloor;
    color1 = 180;
    color2 = 180;
    color3 = 180;

    symbol = '.';

    strcpy(name, "sand");
}

void Terrain::makeUpStairs()
{
    type = upStairs;
    color1 = 102;
    color2 = 102;
    color3 = 102;

    symbol = '<';

    strcpy(name, "steps");
}

void Terrain::makeLockedStairs()
{
    type = lockedStairs;
    color1 = 174;
    color2 = 174;
    color3 = 132;

    symbol = '>';
    strcpy(name, "a locked gate");
}

void Terrain::makeOpenStairs()
{
    type = openStairs;
    color1 = 155;
    color2 = 155;
    color3 = 155;

    symbol = '>';
    strcpy(name, "an open gate");
}

void Terrain::makeSpecialUp()
{
    type = specialUp;
    color1 = 255;
    color2 = 102;
    color3 = 102;

    symbol = '<';
    strcpy(name, "an open iron gate");
}

void Terrain::makeSpecialOpen()
{
    type = specialOpen;
    color1 = 255;
    color2 = 102;
    color3 = 102;

    symbol = '>';
    strcpy(name, "an open iron gate");
}

void Terrain::makeSpecialLocked()
{
    type = specialLocked;
    color1 = 255;
    color2 = 102;
    color3 = 102;

    symbol = '>';
    strcpy(name, "a locked iron gate");
}

void Terrain::makeclosedDoor()
{
    type = closedDoor;
    color1 = 128;
    color2 = 64;
    color3 = 0;

    symbol = '+';

    strcpy(name, "a closed door");
}

void Terrain::makeopenDoor()
{
    type = openDoor;
    color1 = 128;
    color2 = 64;
    color3 = 0;

    symbol = '/';

    strcpy(name, "an open door");
}

void Terrain::makeDeepWater()
{
    int colour = Random::getInt(4, 0);

    type = deepWater;

    switch (colour)
    {
    case 0: setColor(0, 64, 128); break;
    case 1: setColor(0, 64, 128); break;
    case 2: setColor(0, 64, 128); break;
    case 3: setColor(0, 12, 128); break;
    }

    symbol = '=';

    strcpy(name, "fast flowing water");
}

void Terrain::makeShallowWater()
{
    type = shallowWater;
    int colour = Random::getInt(4, 0);
    switch (colour)
    {
    case 0: setColor(0, 64, 128); break;
    case 1: setColor(0, 64, 128); break;
    case 2: setColor(0, 12, 128); break;
    case 3: setColor(0, 12, 128); break;
    }

    symbol = '=';

    strcpy(name, "water");
}

void Terrain::makeBridge()
{
    type = bridge;
    color1 = 134;
    color2 = 85;
    color3 = 70;

    symbol = '=';

    strcpy(name, "a bridge");
}

void Terrain::makeDeadTree()
{
    type = dfloor;
    color1 = 85;
    color2 = 85;
    color3 = 85;

    symbol = 'T';

    strcpy(name, "a dead tree");

}

void Terrain::makeTree()
{
    type = dfloor;
    color1 = 20;
    color2 = 128;
    color3 = 20;

    symbol = '&';

    strcpy(name, "a tree");
}

void Terrain::makeGrass()
{
    type = dfloor;

    int colour = Random::getInt(2, 0);

    switch (colour)
    {
    case 0: setColor(20, 200, 20); break;
    default: setColor(40, 127, 40); break;
    }

    symbol = '.';

    strcpy(name, "grass");
}

void Terrain::makeTomb()
{
    type = dfloor;

    int colour = Random::getInt(2, 0);

    switch (colour)
    {
    case 0: setColor(100, 100, 100); break;

    }

    symbol = ',';

    strcpy(name, "a tomb");

}

void Terrain::makeSpecialItem()
{
    type = specialItem;
    color1 = 102;
    color2 = 102;
    color3 = 102;

    symbol = '@';

    strcpy(name, "dungeon floor");

}


void Terrain::setColor(int c1, int c2, int c3)
{
    color1 = c1;
    color2 = c2;
    color3 = c3;
}
