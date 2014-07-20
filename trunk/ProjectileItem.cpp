// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#include ".\projectileitem.h"
#include "numberGenerator.h"
#include <string>

using namespace Random;

ProjectileItem::ProjectileItem(void) : Item()
{
    wearable(true);
}

int ProjectileItem::CreateItem(eItemType _type, int level, int secondary_type)
{
    type = _type;
    secondaryType = secondary_type;
    wearable(true);

    switch (type)
    {
    case projectile:            return createProjectile(level, secondaryType);          break;
    case projectileWeapon:      return createProjectileWeapon(level, secondaryType);    break;
    default:                    throw std::exception("incorrect type given to prjectile item");

    }
    return 0;
}

int ProjectileItem::createProjectile(int level, int secondary_type)
{
    secondaryType = secondary_type;
    if (secondaryType == 0) // choose random
        secondaryType = getInt(bolt + 1, arrow);

    switch (secondaryType)
    {
    case arrow: symbol = '\\';
        name = "arrow";
        weight = 1;
        setColor(128, 64, 0);
        //SetDice_thr(1, 6);
        itemNumber[0] = bowArrow;
        break;

    case bolt: symbol = '\\';
        name = "bolt";
        weight = 2;
        setColor(100, 100, 100);
        //SetDice_thr(1, 6);
        itemNumber[0] = crossbowBolts;
        break;

    case silver:  symbol = '\\';
        name = ("silver arrow");
        weight = 1;
        setColor(99, 177, 177);
        //SetDice_thr(1, 6);
        itemNumber[0] = bowArrow;
        break;
    }

    skill_bonus = getInt(100, level);
    if (skill_bonus > 90 && level != 0) skill_bonus = 2;
    else if (skill_bonus > 70) skill_bonus = 1;
    else skill_bonus = 0;

    stackable(true);
    itemNumber[1] = 1;

    AddBrand(0);

    return 0;
}

int ProjectileItem::createProjectileWeapon(int level, int secondary_type)
{
    if (secondary_type == 0) //choose random
        secondary_type = getInt(4, 1);

    switch (secondary_type)
    {
    case bow: 	symbol = '{';
        name = ("short bow");
        identified = 0;
        weight = 100;
        setColor(200, 100, 0);
        SetDice_thr(4, 6);
        itemNumber[0] = bowArrow;
        break;

    case longbow: 	symbol = '{';
        name = ("long bow");
        identified = 0;
        weight = 100;
        setColor(128, 64, 0);
        itemNumber[0] = bowArrow;
        SetDice_thr(5, 8);
        break;

    case crossbow: 	symbol = '}';
        name = ("crossbow");
        identified = 0;
        weight = 100;
        setColor(100, 100, 100);
        itemNumber[0] = crossbowBolts;
        SetDice_thr(6, 5);
        break;
    }

    skill_bonus = getInt(100, 0);
    if (skill_bonus > 90 && level != 0)
        skill_bonus = 2;
    else if (skill_bonus > 70)
        skill_bonus = 1;
    else skill_bonus = 0;

    return 0;
}

