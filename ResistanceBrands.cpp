// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#include ".\resistancebrands.h"

ResistanceBrands::ResistanceBrands(void) :
brand(0), 
resistance(0)
{
}

ResistanceBrands::ResistanceBrands(const ResistanceBrands& rhs)
{
    BRANDMAP_CITERATOR it;

    for (it = rhs.resistanceMap.begin(); it != rhs.resistanceMap.end(); it++)
        resistanceMap[it->first] = it->second;

    for (it = rhs.brandMap.begin(); it != rhs.brandMap.end(); it++)
        brandMap[it->first] = it->second;

    brand = rhs.brand;
    resistance = rhs.resistance;
}

int ResistanceBrands::GetResistance(eBrandType type)
{
    BRANDMAP::iterator it = resistanceMap.find(type);
    if (it != resistanceMap.end())
        return it->second;

    return 0;
}

int ResistanceBrands::GetBrand(eBrandType type)
{
    BRANDMAP::iterator it = brandMap.find(type);
    if (it != brandMap.end())
        return it->second;

    return 0;
}

int ResistanceBrands::hasResistance()
{
    return resistanceMap.size();
}

int ResistanceBrands::hasBrand()
{
    return brand;
}

//will override existing resistance.
void ResistanceBrands::SetResistance(eBrandType type, int strength)
{
    resistanceMap[type] = strength;
    if (strength != 0)
        resistance = strength;
}
void ResistanceBrands::SetBrand(eBrandType type, int strength)
{
    brandMap[type] = strength;

    if (strength != 0)
        brand = strength;
}

////////////////////////////////

char * ResistanceBrands::GetBrandName(eBrandType brandType)
{
    switch (brandType)
    {
    case bFire:             return "burned";
    case bFrost:            return "chilled";
    case bLightning:        return "shocked";
    case bAcid:             return "partially dissolved";
    case bPoison:           return "poisoned";
    case bParalysis:        return "paralysed";
    case bConfusion:        return "confused";
#ifdef _DEBUG
    default:                return "tickled";
#else
    default:                return "";
#endif

    }
}

char * ResistanceBrands::GetResistanceName(eBrandType brandType)
{
    switch (brandType)
    {
    case bFire:             return "heat";
    case bFrost:            return "cold";
    case bLightning:        return "electricity";
    case bAcid:             return "acid";
    case bPoison:           return "poison";
    case bParalysis:        return "paralysis";
    case bConfusion:        return "confusion";
#ifdef _DEBUG
    default: return "tickling";
#else
    default: return "";
#endif
    }
}

