#include ".\projectileitem.h"
#include "numberGenerator.h"
#include <string>

using namespace Random;

ProjectileItem::ProjectileItem(void) : Item()
{
    wearable(true);
}

ProjectileItem::~ProjectileItem(void)
{
}

int ProjectileItem::CreateItem(eItemType _type, int level, int secondary_type)
{
    type = _type;
    secondaryType = secondary_type;
    wearable(true);

    switch (type)
    {
    case projectile: return createProjectile(level, secondaryType); break;
    case projectileWeapon: return createProjectileWeapon(level, secondaryType); break;
    default: throw std::exception("incorrect type given to prjectile item");

    }
    return 0;
}

int ProjectileItem::createProjectile(int level, int secondary_type)
{
    if (secondary_type == 0) //choose random
        secondary_type = getInt(6, 4);

    switch (secondary_type)
    {
    case arrow: symbol = '\\';
        strcpy(name, "arrow");
        weight = 1;
        setColor(128, 64, 0);
        //SetDice_thr(1, 6);
        itemNumber[0] = bowArrow;
        break;

    case bolt: symbol = '\\';
        strcpy(name, "bolt");
        weight = 2;
        setColor(100, 100, 100);
        //SetDice_thr(1, 6);
        itemNumber[0] = crossbowBolts;
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
        strcpy(name, "short bow");
        identified = 0;
        weight = 100;
        setColor(128, 64, 0);
        SetDice_thr(5, 6);
        itemNumber[0] = bowArrow;
        break;

    case longbow: 	symbol = '{';
        strcpy(name, "long bow");
        identified = 0;
        weight = 100;
        setColor(148, 64, 0);
        itemNumber[0] = bowArrow;
        SetDice_thr(6, 8);
        break;

    case crossbow: 	symbol = '{';
        strcpy(name, "crossbow");
        identified = 0;
        weight = 100;
        setColor(100, 100, 100);
        itemNumber[0] = crossbowBolts;
        SetDice_thr(8, 5);
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

char *ProjectileItem::GetName()
{
  /*  if (identified)
    {
        int bonus = 0;
        switch (type)
        {
        case projectile: 	bonus = skill_bonus;
            //sprintf(id_name, "%d $%d %s (%dd%d)", itemNumber[1], bonus, name, thrNumDice, thrSidesDice);
            sprintf(id_name, "%d $%d %s", itemNumber[1], bonus, name);
            break;
        case projectileWeapon:
            bonus = skill_bonus;
            sprintf(id_name, " $%d %s (%dd%d)", bonus, name, thrNumDice, thrSidesDice);
            break;

        default: bonus = 0; sprintf(id_name, "%s", name); break;
        }
    }
    else
    {
        sprintf(id_name, "unidentified %s", name);
    }

    if (equipped)
        strcat(id_name, " (equipped)");
    //sprintf(id_name,"%s (equipped)",id_name);


    return id_name;*/
    return id_name;

}