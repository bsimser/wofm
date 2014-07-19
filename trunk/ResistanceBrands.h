// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#pragma once


#include <map>
#include <list>


/* magical brands and resistance

All brands/resistances have a rating from 0-3. 0 is none, 3 is best
Resistance 
	0 is 0% Full damahe
	1 is 30% 2/3 damage
	2 is 60% 1/3 damage
	3 is 100% (90%?)imunity

Adding resistances should not be linear, and immunity should not be achievable unless res = 3
I.e adding two resistances items of 1 each	= 30/1 + 30/4		  == 45% not 60%
I.e adding two resistances items of 2 each	= 60/1 + 60/4		  == 75% not 120%
I.e adding two resistances items 2 1		= 60/1 + 30/4		  == 67%
I.e adding two resistances items 2 2 1		= 60/1 + 60/4 + 30/8  == 82%
I.e adding two resistances items 2 1 1		= 60/1 + 30/4 + 30/8  == 71%
I.e adding two resistances items 2 2 2 2	= 60/1 + 60/4 + 60/8 + 60/12 == 87%

Brands
	0 none
	1 some		ie +1d6 extra fire damage
	2 some x 2	ie +2d6 extra fire damage
	3 some x 3	ie +3d6 extra fire damage

Fire
Frost
Lightning
Posion
Acid
*/

#define BRAND 0
#define RES   1

enum eBrandType
{
	bNone,			
	bFire			= 0x01,
	bFrost			= 0x02,
	bLightning		= 0x04,
	bPoison			= 0x08,
	bAcid			= 0x10,
    bDisplacment	= 0x20,	//teleport some creature or resistance to being teleported

	bSlow,			//effects
	bParalysis	,
	bConfusion	,
	bDisease,

};

enum eStrength
{
	sNone,	//0
	sOne,	//30%
	sTwo,	//60%
	sFull	//100%
};

typedef std::map<eBrandType,int> BRANDMAP;

typedef std::list<eBrandType> BRANDLIST;

typedef BRANDMAP::const_iterator BRANDMAP_CITERATOR;

class ResistanceBrands
{
public:
	ResistanceBrands(void);

	ResistanceBrands(const ResistanceBrands& rhs);

	int GetResistance(eBrandType);
	int GetBrand(eBrandType);

	void SetResistance(eBrandType,int strength);
	void SetBrand(eBrandType,int strength);

	int hasResistance();
	int hasBrand();

    char *GetBrandName(eBrandType type);
    static char *GetResistanceName(eBrandType type);

	BRANDMAP brandMap;
	BRANDMAP resistanceMap;

private:
	int		resistance;
	int		brand;
};
