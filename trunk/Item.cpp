// Item.cpp: implementation of the Item class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4786) 

#include "Item.h"
#include "ItemManager.h"
#include "numberGenerator.h"
#include <string>

using namespace Random;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Item::Item() :
skill_bonus(0),
defence_bonus(0),
absorb_bonus(0),
identified(0),
equipped(0),
stackable(0),
wearable(0),
type(no_type),
secondaryType(no_type),
symbol(0),
weight(0),
ref(-1)
{
    for (int i = 0; i < 3; i++)
        itemNumber[i] = 0;

    setColor(255, 255, 255);

    SetDice_h2h(1, 1);
    SetDice_thr(1, 1);

    strcpy(postfix, "");
    strcpy(prefix, "");
    strcpy(nothing, "");
    strcpy(id_name, "");
    strcpy(name, "");
}

Item::Item(const Item& rhs) : ResistanceBrands(rhs)
{
    strcpy(name, rhs.name);
    strcpy(id_name, rhs.id_name);

    type = rhs.type;
    secondaryType = rhs.secondaryType;
    symbol = rhs.symbol;
    weight = rhs.weight;

    setColor(rhs.color1, rhs.color2, rhs.color3);

    skill_bonus = rhs.skill_bonus;
    defence_bonus = rhs.defence_bonus;
    absorb_bonus = rhs.absorb_bonus;

#ifdef _DEBUG
    identified = 1;
#else
    identified = rhs.identified;
#endif
    identified = rhs.identified;

    equipped = rhs.equipped;

    ref = rhs.ref;
    wearable = rhs.wearable;
    stackable = rhs.stackable;

    for (int i = 0; i < 3; i++)
        itemNumber[i] = rhs.itemNumber[i];

    h2hSidesDice = rhs.h2hSidesDice;
    h2hNumDice   = rhs.h2hNumDice;

    thrSidesDice = rhs.thrSidesDice;
    thrNumDice   = rhs.thrNumDice;

    strcpy(postfix, rhs.postfix);
    strcpy(prefix, rhs.prefix);
    strcpy(nothing, rhs.nothing);
}

Item::~Item()
{
}

bool Item::operator< (const Item & rhs)
{
    if (rhs.type < this->type)
        return false;
    else if (rhs.type > this->type)
        return true;

    /*else if (rhs.absorb_bonus < this->absorb_bonus)
        return false;
    else if (rhs.absorb_bonus > this->absorb_bonus)
        return true;

    else if (rhs.defence_bonus < this->defence_bonus)
        return false;
    else if (rhs.defence_bonus > this->defence_bonus)
        return true;

    else if (rhs.skill_bonus < this->absorb_bonus)
        return false;
    else if (rhs.skill_bonus > this->skill_bonus)
        return true;*/

    return (rhs.ref < this->ref) ? false : true;
}

int Item::CreateItem(eItemType _type, int level, int secondary_type)
{
    switch (_type)
    {
    case corpse:        createCorpse();             wearable = 0; break;
    case weapon:        createWeapon(level, secondary_type);     wearable = 1; break;
    case armour:        createArmour(level, secondary_type);     wearable = 1; break;
    case key:            createKey(level);         wearable = 0; break;
    case lockedChest:    createLockedChest(level); wearable = 0; break;
    case openChest:        createOpenChest(level);     wearable = 0; break;
    case gold:            createGold(level);         wearable = 0; break;
    case shield:        createShield(level);     wearable = 1; break;
    case cards:            createCards(level);         wearable = 0; break;

    default:     char err[128]; sprintf(err, "Invalid item type passed into item creator: %d", _type); throw std::exception(err);
    }

    type = _type; //type defines behaviour

    return 0;
}

void Item::createShield(int level)
{
    symbol = '[';
    setColor(50, 50, 50);

    int size = getInt(4, 0);

    if (getInt(10, 0) == 0 && level > 5 || level > 24)
        size++;

    if (size == 4 && level > 9)
    {
        sprintf(name, "tower shield");
        absorb_bonus = 4;
    }

    else if (size == 3 && level > 9)
    {
        sprintf(name, "large shield");
        absorb_bonus = 3;
    }
    else if (size == 2 && level > 3)
    {
        sprintf(name, "medium shield");
        absorb_bonus = 2;
    }
    else
    {
        sprintf(name, "small shield");
        absorb_bonus = 1;
    }


}

void Item::createKey(int level)
{
    symbol = '}';

    if (level > 9)
    {
        itemNumber[0] = ItemManager::KeySpecial[level - 10];
        sprintf(name, "key labelled %d", itemNumber[0]);

    }

    else
    {
        itemNumber[0] = ItemManager::keyLabels[level];
        sprintf(name, "key labelled %d", itemNumber[0]);
    }

    identified = 1;
    weight = 1;

    setColor(255, 255, 0);
}


void Item::createOpenChest(int level)
{
    symbol = '&';

    sprintf(name, "chest (empty)");

    identified = 1;
    weight = 100;

    setColor(128, 128, 128);
}

void Item::createLockedChest(int level)
{
    symbol = '&';

    if (level == 19)
    {
        int key1 = getInt(10, 0);
        int key2;
        int key3;

        while ((key2 = getInt(10, 0)) == key1);

        do{
            key3 = getInt(10, 0);
        } while (key3 == key2 || key3 == key1);

        key1 = ItemManager::KeySpecial[key1];
        key2 = ItemManager::KeySpecial[key2];
        key3 = ItemManager::KeySpecial[key3];


        sprintf(name, "large chest (%d) (%d) (%d)", key1, key2, key3);
        weight = 1000;
        setColor(255, 255, 0);
        itemNumber[0] = key1;
        itemNumber[1] = key2;
        itemNumber[2] = key3;

    }

    else
    {
        if (level > 9)
            itemNumber[0] = ItemManager::KeySpecial[level - 10];
        else
            itemNumber[0] = ItemManager::KeySpecial[level];

        sprintf(name, "chest (%d)", itemNumber[0]);
        weight = 100;
        setColor(128, 128, 20);
    }

    identified = 1;

}

void Item::createCorpse()
{
    symbol = 'c';
    strcpy(name, "rotting corpse");
    identified = 0;
    weight = 15;
    setColor(128, 128, 128);
}

void Item::createGold(int level)
{
    symbol = '$';
    strcpy(name, "Zagor's gold");
    identified = 0;
    weight = 100;
    setColor(255, 255, 0);
    identified = 1;
}

void Item::createCards(int level)
{
    symbol = '+';
    strcpy(name, "pack of cards");
    identified = 0;
    weight = 100;
    setColor(128, 255, 0);
}

void Item::createWeapon(int level, int secondary_type)
{
    int varient;
    if (secondary_type != 0)
        varient = secondary_type;

    else
        varient = getInt(4, 1);

    if (level > 10)
        level /= 2 + 2;

    skill_bonus = getInt(91 + level, 0);

    if (skill_bonus > 97) skill_bonus = 4;        // 5%
    else if (skill_bonus > 93) skill_bonus = 3;    // 5%
    else if (skill_bonus > 85 && level != 0) skill_bonus = 2;    // 15%
    else if (skill_bonus > 50) skill_bonus = 1;    // 30%
    else skill_bonus = 0;                        // 50%

    switch (varient)
    {
    case sword: {

                    int type1 = getInt(2, 0);
                    switch (type1)
                    {
                    case 0: strcpy(name, "long sword");    symbol = '|';    weight = 12; setColor(90, 90, 90); SetDice_h2h(1, 8); break;
                    case 1: strcpy(name, "short sword");    symbol = '|';    weight = 8; setColor(90, 90, 90); SetDice_h2h(1, 6); break;
                    }

                    break;
    }
    case axe:{
                 int type2 = getInt(2, 0); if (level == 0) type2 = 0; //no battle 
                 switch (type2)
                 {
                 case 0: strcpy(name, "hand axe");    symbol = '/';    weight = 15; setColor(90, 90, 90); SetDice_h2h(2, 4); break;
                 case 1: strcpy(name, "battle axe");    symbol = '/';    weight = 15; setColor(140, 140, 140); SetDice_h2h(2, 5); break;
                 }
                 break;
    }
    case mace:  strcpy(name, "club");    symbol = '/';    weight = 15; setColor(150, 70, 0); SetDice_h2h(2, 3); break;
    case staff:  strcpy(name, "black staff");    symbol = '|';    weight = 15; setColor(70, 70, 70); SetDice_h2h(3, 3); break;
    case cleaver:  strcpy(name, "cleaver");    symbol = '/';    weight = 15; setColor(200, 0, 0); SetDice_h2h(3, 4); break;

    default: strcpy(name, "orcish sword");    symbol = '|';    weight = 10; setColor(130, 130, 130); SetDice_h2h(2, 10); break;
    }

    if (level < 2)
        return;

    if (getInt(100, 0) == 99 && skill_bonus > 2)
    {
        strcpy(postfix, " of Pain");
        setColor(230, 230, 230);
        skill_bonus = 5;
    }

    else
    {
        AddBrand();
    }

    //setColor(50,50,50);
}

void Item::createArmour(int level, int secondary_type)
{
    int varient;
    if (secondary_type > 0)
    {
        if (secondary_type == leather)
            varient = 0;
        else if (secondary_type == chainmail)
            varient = 84;
        else if (secondary_type == platemail)
            varient = 99;
        else if (secondary_type == blackcloak)
            varient = 200;
        else if (secondary_type == bloodyapron)
            varient = 201;

    }
    else varient = getInt(95, 0) + level;
    int quality = getInt(90, 0) + level * 2;

    //    if(absorb_bonus > 95) absorb_bonus = 3;        // 0%
    if (quality >= 80 && level != 0) absorb_bonus = 2;    // 20%
    else if (quality >= 50) absorb_bonus = 1;    // 30%
    else quality = 0;                        // 50%

    if (varient == 200)
    {

        strcpy(name, "black cloak");
        symbol = ']';

        absorb_bonus = 4;  //absorb 1-3
        weight = 15;

        AddResistance(true);
        setColor(64, 64, 64);

        return;
    }
    else if (varient == 201)
    {
        strcpy(name, "bloody apron");
        symbol = ']';
        setColor(233, 64, 64);

        absorb_bonus = 3;  //absorb 1-3
        weight = 15;

    }
    else if (varient < 70)
    {
        strcpy(name, "leather armour");

        if(absorb_bonus == 0)
            strcpy(prefix, "crude ");
        else if(absorb_bonus == 2)
            strcpy(prefix, "quality ");

        symbol = ']';
        setColor(128, 64, 0);

        absorb_bonus += 1;  //absorb 1-3
        weight = 15;
    }
    else if (varient < 90 || level == 0)
    {
        strcpy(name, "chainmail");

        if(absorb_bonus == 0)
            strcpy(prefix, "orcish ");
        else if(absorb_bonus == 2)
            strcpy(prefix, "dwarven ");

        symbol = ']'; setColor(100, 100, 100);
        absorb_bonus += 2; //absorb 2-4 
        weight = 25;
    }
    else
    {
        strcpy(name, "plate mail");

        if(absorb_bonus == 0)
            strcpy(prefix, "rusty ");
        else if(absorb_bonus == 2)
            strcpy(prefix, "full ");

        symbol = ']';
        setColor(200, 200, 200);
        absorb_bonus += 3; //absorb 3-5
        weight = 35;
    }

    if (getInt(100, 0) >= 98 && absorb_bonus > 2)
    {
        strcpy(postfix, " of Defence");
        setColor(255, 255, 255);
        absorb_bonus += 1;
    }
    else if (level > 2)
        AddResistance();

}

/////////////////////////////////

void Item::setColor(int c1, int c2, int c3)
{
    color1 = c1;
    color2 = c2;
    color3 = c3;
}

char *Item::GetName()
{
    if (identified)
    {
        switch (type)
        {
        case armour:            sprintf(id_name, "%s%s[%d]%s", prefix, name, absorb_bonus, postfix); break;
        case weapon:            //sprintf(id_name, "+%d %s%s [%d]%s", skill_bonus, prefix, name, h2hNumDice, postfix);
            sprintf(id_name, "+%d %s%s (%dd%d)%s", skill_bonus, prefix, name, h2hNumDice, h2hSidesDice, postfix);
            break;

        case projectile:        if (itemNumber[1] > 1)    sprintf(id_name, "%d +%d %s%s%s%s", itemNumber[1], skill_bonus, prefix, name, itemNumber[1] > 1 ? "s" : nothing, postfix);
                                else                sprintf(id_name, "+%d %s%s%s", skill_bonus, prefix, name, postfix); break;
        case projectileWeapon:  //sprintf(id_name, "+%d %s%s [%d]%s", skill_bonus, prefix, name, thrNumDice/*, thrSidesDice*/, postfix);    break;
                                sprintf(id_name, "+%d %s%s (%dd%d)%s", skill_bonus, prefix, name, thrNumDice, thrSidesDice, postfix);    break;
        case shield:            sprintf(id_name, "%s%s[%d]%s", prefix, name, absorb_bonus, postfix);    break;
        default:                sprintf(id_name, "%s", name); break;
        }
    }
    else
    {
        if (strlen(postfix) > 0)
        {
            sprintf(id_name, "glowing %s%s", name, itemNumber[1] > 1 ? "s" : nothing);
        }
        else
            sprintf(id_name, "unidentified %s%s", name, itemNumber[1] > 1 ? "s" : nothing);
    }

    if (equipped)
        strcat(id_name, " (equipped)");
    //sprintf(id_name,"%s (equipped)",id_name);


    return id_name;

}


void Item::SetDice_h2h(int nDice, int sides)
{
    h2hSidesDice = sides;
    h2hNumDice = nDice;
}

void Item::SetDice_thr(int nDice, int sides)
{
    thrSidesDice = sides;
    thrNumDice = nDice;

}

int Item::GetAttack_h2h()
{
    int attack = 0;
    for (int i = 0; i < h2hNumDice; i++)
        attack += getInt(h2hSidesDice + 1, 1);

    return attack + skill_bonus;
}

float Item::getAverage_h2h()
{
    return  ((float)h2hSidesDice * (float)h2hNumDice) / 2.0f + (float)skill_bonus;
}

float Item::getAverage_thr()
{
    return  ((float)thrSidesDice * (float)thrNumDice) / 2.0f + (float)skill_bonus;
}

int Item::GetAttack_thr()
{
    int attack = 0;
    for (int i = 0; i < thrNumDice; i++)
        attack += getInt(thrSidesDice + 1, 1);

    return attack + skill_bonus;
}

void Item::AddBrand(bool special)
{
    int brandType = getInt(103, 0);

    if (special)
    {
        brandType = getInt(103, 92);
    }


    if (brandType < 91)
        return;
    else if (brandType < 93)
    {
        int poison = getInt(4, 1);
        SetBrand(bPoison, poison);
        if (poison == 3)
            strcpy(postfix, " of the Snake");
        else if (poison == 2)
            strcpy(postfix, " of Venom");
        else if (poison == 1)
            strcpy(postfix, " of Weak Posion");
        
        setColor(0, 255, 0);
    }

    else if (brandType < 95)
    {
        int fire = getInt(4, 1);
        SetBrand(bFire, fire);
        if (fire == 3)
            strcpy(postfix, " of the Sun");
        else if (fire == 2)
            strcpy(postfix, " of Flame");
        else if (fire == 1)
            strcpy(postfix, " of Candle Light");
        setColor(128, 0, 0);

    }
    else if (brandType < 97)
    {
        int cold = getInt(4, 1);
        SetBrand(bFrost, cold);
        if (cold == 3)
            strcpy(postfix, " of the Ice Crystal");
        else if (cold == 2)
            strcpy(postfix, " of Frost");
        else if (cold == 1)
            strcpy(postfix, " of Chills");
        setColor(0, 0, 128);
    }
    else if (brandType < 99)
    {
        int lightning = getInt(4, 1);
        SetBrand(bLightning, lightning);
        if (lightning == 3)
            strcpy(postfix, " of the Thunder God");
        else if (lightning == 2)
            strcpy(postfix, " of Lightning");
        else if (lightning == 1)
            strcpy(postfix, " of Static");
        setColor(128, 128, 0);
    }
    else if (brandType < 101)
    {
        int acid = getInt(4, 1);
        SetBrand(bAcid, acid);
        if (acid == 3)
            strcpy(postfix, " of Zayt al-Zaj"); //acid
        else if (acid == 2)
            strcpy(postfix, " of Corrosion");    //corrosion
        else if (acid == 1)
            strcpy(postfix, " of Weak Acid"); //oil of vitriol or  Zayt al-Zaj
        setColor(128, 0, 128);
    }
}

void Item::AddResistance(bool special)
{
    if (strlen(postfix) > 0)
        return;

    special = true;
    int brandType = getInt(103, 0);

    if (special)
    {
        brandType = getInt(103, 92);
    }

    if (brandType < 92) //80
        return;

    else if (brandType < 94)
    {
        int poison = 3;//getInt(4,1);
        SetResistance(bFire, poison);
        if (poison == 3)
            strcpy(postfix, " of Strong Poison Resistance");
        else if (poison == 2)
            strcpy(postfix, " of Medium Poison Resistance");
        else if (poison == 1)
            strcpy(postfix, " of Weak Poison Resistance");
        setColor(0, 255, 0);
    }

    else if (brandType < 96)
    {
        int fire = 3;//getInt(4,1);
        SetResistance(bFire, fire);
        if (fire == 3)
            strcpy(postfix, " of Fire Resistance");
        else if (fire == 2)
            strcpy(postfix, " of Flame Retardant");
        else if (fire == 1)
            strcpy(postfix, " of the Anti-Candle");
        setColor(128, 0, 0);

    }
    else if (brandType < 98)
    {
        int cold = 3;//getInt(4,1);
        SetResistance(bFrost, cold);
        if (cold == 3)
            strcpy(postfix, " of Ice Shield");
        else if (cold == 2)
            strcpy(postfix, " of Frostproof");
        else if (cold == 1)
            strcpy(postfix, " of Frost Resistance");

        setColor(0, 0, 128);
    }
    else if (brandType < 100)
    {
        int lightning = getInt(4, 1);
        SetResistance(bLightning, lightning);
        if (lightning == 3)
            strcpy(postfix, " of Lightning Resistance");
        else if (lightning == 2)
            strcpy(postfix, " of Sparks Resistance");
        else if (lightning == 1)
            strcpy(postfix, " of Grounding");

        setColor(128, 128, 0);
    }
    else if (brandType < 102)
    {
        int acid = getInt(4, 1);
        SetResistance(bAcid, acid);
        if (acid == 3)
            strcat(postfix, " of Acid Resistance"); //acid
        else if (acid == 2)
            strcat(postfix, " of Corrosion Resistance");    //corrosion
        else if (acid == 1)
            strcat(postfix, " of Low Acid Resistance"); //oil of vitriol or  Zayt al-Zaj

        setColor(128, 0, 128);
    }
}