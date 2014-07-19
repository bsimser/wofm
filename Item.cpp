// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#pragma warning(disable : 4786) 

#include "Item.h"
#include "ItemManager.h"
#include "numberGenerator.h"

#include "worldbuilder.h" // get all items from manager

#include <string>

using namespace Random;

// --------------------------------------------------------------------------------------------------------------------------------

Item::Item() :
    skill_bonus(0),
    defence_bonus(0),
    absorb_bonus(0),
    identified(false),
    equipped(false),
    mStackable(false),
    mWearable(false),
    type(no_type),
    secondaryType(no_type),
    symbol(0),
    weight(0),
    ref(-1),
    name(""),
    prefix(""),
    postfix("")
{
    for (int i = 0; i < 3; i++)
    {
        itemNumber[i] = 0;
    }

    setColor(255, 255, 255);

    SetDice_h2h(1, 1);
    SetDice_thr(1, 1);
}

// --------------------------------------------------------------------------------------------------------------------------------

Item::Item(const Item & rhs) : ResistanceBrands(rhs)
{
    name = rhs.name;

    type = rhs.type;
    secondaryType = rhs.secondaryType;
    symbol = rhs.symbol;
    weight = rhs.weight;

    setColor(rhs.color1, rhs.color2, rhs.color3);

    skill_bonus   = rhs.skill_bonus;
    defence_bonus = rhs.defence_bonus;
    absorb_bonus  = rhs.absorb_bonus;

#ifdef _DEBUG
    identified = 1;
#else
    identified = rhs.identified;
#endif
    identified = rhs.identified;

    equipped = rhs.equipped;

    ref = rhs.ref;
    mWearable = rhs.mWearable;
    mStackable = rhs.mStackable;

    for (int i = 0; i < 3; i++)
        itemNumber[i] = rhs.itemNumber[i];

    h2hSidesDice = rhs.h2hSidesDice;
    h2hNumDice   = rhs.h2hNumDice;

    thrSidesDice = rhs.thrSidesDice;
    thrNumDice   = rhs.thrNumDice;

    postfix = rhs.postfix;
    prefix =  rhs.prefix;
}

// --------------------------------------------------------------------------------------------------------------------------------

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

// --------------------------------------------------------------------------------------------------------------------------------

int Item::CreateItem(eItemType _type, int level, int secondary_type)
{
    switch (_type)
    {
    case corpse:        createCorpse();                         break;
    case weapon:        createWeapon(level, secondary_type);    break;
    case armour:        createArmour(level, secondary_type);    break;
    case key:           createKey(level);                       break;
    case lockedChest:   createLockedChest(level);               break;
    case openChest:     createOpenChest(level);                 break;
    case gold:          createGold(level);                      break;
    case shield:        createShield(level);                    break;
    case cards:         createCards(level);                     break;
    case provisions:    createProvision();                      break;
    case gem:           createGem();                            break;
    case stake:         createStake();                          break;
    case cheese:        createCheese();                         break;
    case carcass:       createCarcass();                        break;
    case DiMaggio:      createDiMaggio();                        break;

    default:     
        char err[128]; 
        sprintf(err, "Invalid item type passed into item creator: %d", _type); 
        throw std::exception(err);
    }

    type = _type; //type defines behaviour

    return 0;
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createProvision()
{
    symbol = ';';
    setColor(185, 122, 87);
    name = ("provision");
    itemNumber[1] = 1; // set stack to one
    identified = true;
    mStackable = true;

    mWearable = true; // hack to be usable in inventory
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createGem()
{
    symbol = '*';
    setColor(255, 50, 50);
    name = ("large red gem");
    mWearable = true; // hack to be usable in inventory
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createDiMaggio()
{
    symbol = '+';
    setColor(255, 50, 50);
    name = ("book");
    postfix = " of DiMaggio";
    mWearable = true; // hack to be usable in inventory
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createCarcass()
{
    symbol = 'c';
    name = ("pig carcass");
    identified = 1;
    weight = 15;
    setColor(255, 151, 185);
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createStake()
{
    symbol = '|';
    setColor(185, 122, 87);
    identified = true;
    mStackable = true;
    itemNumber[1] = 1; // set stack to one
    name = ("wooden stake");
    mWearable = true; // hack to be usable in inventory
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createCheese()
{
    mStackable = true;
    identified = true;
    symbol = ';';
    setColor(140, 230, 10);
    name = ("smelly cheese");
    mWearable = true; // hack to be usable in inventory
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createShield(int level)
{
    symbol = '[';
    setColor(50, 50, 50);
    mWearable = 1;
    int size = getInt(4, 0);

    if (getInt(10, 0) == 0 && level > 5 || level > 24)
        size++;

    if (size == 4 && level > 9)
    {
        name = ("tower shield");
        absorb_bonus = 4;
    }

    else if (size == 3 && level > 9)
    {
        name = ("large shield");
        absorb_bonus = 3;
    }
    else if (size == 2 && level > 3)
    {
        name = ("medium shield");
        absorb_bonus = 2;
    }
    else
    {
        name = ("small shield");
        absorb_bonus = 1;
    }
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createKey(int level)
{
    symbol = '}';
    char keyName[32];

    if (level > 9)
    {
        itemNumber[0] = ItemManager::KeySpecial[level - 10];
        sprintf(keyName, "key labelled %d", itemNumber[0]);
    }

    else
    {
        itemNumber[0] = ItemManager::keyLabels[level];
        sprintf(keyName, "key labelled %d", itemNumber[0]);
    }
    name = keyName;

    identified = 1;
    weight = 1;

    setColor(255, 255, 0);
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createOpenChest(int level)
{
    symbol = '&';
    name= ("chest (empty)");

    identified = 1;
    weight = 100;

    setColor(128, 128, 128);
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createLockedChest(int level)
{
    char chestName[32];

    symbol = '&';

    if (level == 19)
    {
        std::vector<int> keysNumbers;
        for (ITEMLIST::iterator it = World.getItemManager().all_items.begin(); it != World.getItemManager().all_items.end(); ++it)
        {
            if (it->type == key)
            {
                keysNumbers.push_back(it->itemNumber[0]); // key number
            }
        }
        if (keysNumbers.size() < 3)
            throw std::exception("Not enough keys generated for Zagors chest. ");
        int keys[3];
        for (int i = 0; i < 3; i++)
        {
            int choice = Random::getInt(keysNumbers.size(), 0);
            keys[i] = keysNumbers[choice];
            keysNumbers.erase(keysNumbers.begin() + choice);
        }


        /*int key1 = getInt(10, 0);
        int key2;
        int key3;

        while ((key2 = getInt(10, 0)) == key1);

        do{
            key3 = getInt(10, 0);
        } while (key3 == key2 || key3 == key1);

        key1 = ItemManager::KeySpecial[key1];
        key2 = ItemManager::KeySpecial[key2];
        key3 = ItemManager::KeySpecial[key3];*/

        sprintf(chestName, "large chest (%d) (%d) (%d)", keys[0], keys[1], keys[2]);
        weight = 1000;
        setColor(255, 255, 0);
        itemNumber[0] = keys[0];
        itemNumber[1] = keys[1];
        itemNumber[2] = keys[2];
    }

    else
    {
        if (level > 9)
            itemNumber[0] = ItemManager::KeySpecial[level - 10];
        else
            itemNumber[0] = ItemManager::KeySpecial[level];

        sprintf(chestName, "chest (%d)", itemNumber[0]);
        weight = 100;
        setColor(128, 128, 20);
    }

    name = chestName;
    identified = 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createCorpse()
{
    symbol = 'c';
    name = ("rotting corpse");
    identified = 0;
    weight = 15;
    setColor(128, 128, 128);
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createGold(int level)
{
    symbol = '$';
    name = ("Zagor's gold");
    identified = 0;
    weight = 100;
    setColor(255, 255, 0);
    identified = 1;
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createCards(int level)
{
    symbol = '+';
    name = ("pack of cards");
    identified = 1;
    weight = 100;
    setColor(128, 255, 0);
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createWeapon(int level, int secondary_type)
{
    mWearable = 1;
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
    case sword: 
    {

        int type1 = getInt(2, 0);
        if (level == 0)
            type1 = 0; //no long 
        switch (type1)
        {
        case 1: name = ("long sword");    symbol = '|';    weight = 12; setColor(90, 90, 90); SetDice_h2h(3, 8); break;
        default: name = ("short sword");   symbol = '|';    weight = 8; setColor(90, 90, 90); SetDice_h2h(2, 6); break;
        }

        break;
    }
    case axe:
    {
        int type2 = getInt(3, 0); 
        if (level == 0) 
            type2 = 0; //no battle 
        switch (type2)
        {
        case 1: name = ("battle axe");  symbol = '/';    weight = 15; setColor(140, 140, 140); SetDice_h2h(5, 5); break;
        default: name = ("hand axe");   symbol = '/';    weight = 15; setColor(90, 90, 90);    SetDice_h2h(3, 4); break;
        }
        break;
    }
    case mace:
    {
        int type = getInt(4, 0);
        if (level < 2)
            type = 0;
        switch (type)
        {
        case 3:  name = ("mace");  symbol = '/';    weight = 15; setColor(140, 140, 140); SetDice_h2h(4, 5); break;
        default: name = ("club"); symbol = '/';    weight = 15; setColor(150, 70, 0);    SetDice_h2h(2, 3);    break;
        }
        break;
    }
    case staff:    name = ("black staff");  symbol = '|';    weight = 15; setColor(70, 70, 70);  SetDice_h2h(4, 3); break;
    case cleaver: name = ("cleaver");      symbol = '/';    weight = 15; setColor(200, 0, 0);   SetDice_h2h(5, 4); break;

    default: name = ("orcish sword");    symbol = '|';    weight = 10; setColor(130, 130, 130); SetDice_h2h(6, 10); break;
    }

    if (level < 2)
        return;

    if (getInt(100, 0) == 99 && skill_bonus > 2)
    {
        postfix = " of Pain";
        setColor(230, 230, 230);
        skill_bonus = 5;
    }
    else
    {
        AddBrand();
    }

    //setColor(50,50,50);
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::createArmour(int level, int secondary_type)
{
    mWearable = 1;

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

        name="black cloak";
        symbol = ']';

        absorb_bonus = 4;  //absorb 1-3
        weight = 15;

        AddResistance(true);
        setColor(64, 64, 64);

        return;
    }
    else if (varient == 201)
    {
        name = "bloody apron";
        symbol = ']';
        setColor(233, 64, 64);

        absorb_bonus = 3;  //absorb 1-3
        weight = 15;

    }
    else if (varient < 70)
    {
        name = "leather armour";

        if(absorb_bonus == 0)
            prefix = "crude ";
        else if(absorb_bonus == 2)
            prefix = "quality ";

        symbol = ']';
        setColor(128, 64, 0);

        absorb_bonus += 1;  //absorb 1-3
        weight = 15;
    }
    else if (varient < 90 || level == 0)
    {
        name = "chainmail";

        if(absorb_bonus == 0)
            prefix = "orcish ";
        else if(absorb_bonus == 2)
            prefix = "dwarven ";

        symbol = ']'; setColor(100, 100, 100);
        absorb_bonus += 2; //absorb 2-4 
        weight = 25;
    }
    else
    {
        name = "plate mail";

        if(absorb_bonus == 0)
            prefix =  "rusty ";
        else if(absorb_bonus == 2)
            prefix = "full ";

        symbol = ']';
        setColor(200, 200, 200);
        absorb_bonus += 3; //absorb 3-5
        weight = 35;
    }

    if (getInt(100, 0) >= 98 && absorb_bonus > 2)
    {
        postfix = " of Defence";
        setColor(255, 255, 255);
        absorb_bonus += 1;
    }
    else if (level > 2)
        AddResistance();

}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::setColor(int c1, int c2, int c3)
{
    color1 = c1;
    color2 = c2;
    color3 = c3;
}

// --------------------------------------------------------------------------------------------------------------------------------

std::string Item::BaseName()
{
    return name;
}

// --------------------------------------------------------------------------------------------------------------------------------

std::string Item::getPostfix()
{
    return postfix;
}

// --------------------------------------------------------------------------------------------------------------------------------

std::string Item::getPrefix()
{
    return prefix;
}

// --------------------------------------------------------------------------------------------------------------------------------

std::string Item::GetName()
{
    char tempName[64];

    if (identified)
    {
        switch (type)
        {
        case armour:
            sprintf(tempName, "%s%s [Def:%d]%s", prefix.c_str(), name.c_str(), absorb_bonus, postfix.c_str()); break;

        case weapon:
            sprintf(tempName, "+%d %s%s [Att:%d]%s", skill_bonus, prefix.c_str(), name.c_str(), h2hNumDice, postfix.c_str());
            break;

        case projectile:
            if (itemNumber[1] > 1)
                sprintf(tempName, "%d +%d %s%s%s%s", itemNumber[1], skill_bonus, prefix.c_str(), name.c_str(), itemNumber[1] > 1 ? "s" : "", postfix.c_str());
            else
                sprintf(tempName, "+%d %s%s%s", skill_bonus, prefix.c_str(), name.c_str(), postfix.c_str()); break;

        case projectileWeapon:
            sprintf(tempName, "+%d %s%s [Att:%d]%s", skill_bonus, prefix.c_str(), name.c_str(), thrNumDice/*, thrSidesDice*/, postfix.c_str());    break;

        case shield:
            sprintf(tempName, "%s%s [Blk:%d]%s", prefix.c_str(), name.c_str(), absorb_bonus, postfix.c_str());    break;

        case provisions:
            sprintf(tempName, "%d %s%s", itemNumber[1], name.c_str(), itemNumber[1] > 1 ? "s" : "");    break;

        case cheese:
            sprintf(tempName, "%d %s%s", itemNumber[1], name.c_str(), itemNumber[1] > 1 ? "s" : "");    break;

        default:
            if (itemNumber[1] > 0)
                sprintf(tempName, "%d %s%s%s%s", itemNumber[1], prefix.c_str(), name.c_str(), itemNumber[1] > 1 ? "s" : "", postfix.c_str());
            else
                sprintf(tempName, "%s%s%s", prefix.c_str(), name.c_str(), postfix.c_str());
            break;

            //sprintf(tempName, "%s", name.c_str()); break;
        }
    }
    else
    {
        if (postfix.length() > 0)
        {
            sprintf(tempName, "glowing %s%s", name.c_str(), itemNumber[1] > 1 ? "s" : "");
        }
        else
        {
            sprintf(tempName, "unidentified %s%s", name.c_str(), itemNumber[1] > 1 ? "s" : "");
        }
    }

    if (equipped)
    {
        strcat(tempName, " (equipped)");
    }

    return std::string(tempName);
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::SetDice_h2h(int nDice, int sides)
{
    h2hSidesDice = sides;
    h2hNumDice = nDice;
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::SetDice_thr(int nDice, int sides)
{
    thrSidesDice = sides;
    thrNumDice = nDice;
}

// --------------------------------------------------------------------------------------------------------------------------------

int Item::GetAttack_h2h()
{
    //int attack = 0;
   // for (int i = 0; i < h2hNumDice; i++)
    //    attack += getInt(h2hSidesDice + 1, 1);

   // return attack + skill_bonus;
    return getInt(h2hNumDice + 1, 1) + skill_bonus;
}

// --------------------------------------------------------------------------------------------------------------------------------

int Item::getAverage_h2h()
{
    return  h2hNumDice + skill_bonus; //((float)h2hSidesDice * (float)h2hNumDice) / 2.0f + (float)skill_bonus;
}

// --------------------------------------------------------------------------------------------------------------------------------

int Item::getAverage_thr()
{
    return  thrNumDice + skill_bonus;
    //return  ((float)thrSidesDice * (float)thrNumDice) / 2.0f + (float)skill_bonus;
}

// --------------------------------------------------------------------------------------------------------------------------------

int Item::GetAttack_thr()
{
  /*  int attack = 0;
    for (int i = 0; i < thrNumDice; i++)
        attack += getInt(thrSidesDice + 1, 1);

    return attack + skill_bonus;*/
    return getInt(thrNumDice + 1, 1) + skill_bonus;
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::AddBrand(bool special)
{
    int brandType = getInt(103, 0);

    if (special)
    {
        brandType = getInt(103, 92);
    }
    
    if (brandType < 91)
    {
        return;
    }
    else if (brandType < 93)
    {
        int poison = getInt(4, 1);
        SetBrand(bPoison, poison);
        if (poison == 3)
            postfix = (" of the Snake");
        else if (poison == 2)
            postfix = (" of Venom");
        else if (poison == 1)
            postfix = (" of Weak Posion");
        
        setColor(0, 255, 0);
    }

    else if (brandType < 95)
    {
        int fire = getInt(4, 1);
        SetBrand(bFire, fire);
        if (fire == 3)
            postfix = (" of the Sun");
        else if (fire == 2)
            postfix = (" of Flame");
        else if (fire == 1)
            postfix = (" of Candle Light");
        setColor(128, 0, 0);

    }
    else if (brandType < 97)
    {
        int cold = getInt(4, 1);
        SetBrand(bFrost, cold);
        if (cold == 3)
            postfix = (" of the Ice Crystal");
        else if (cold == 2)
            postfix = (" of Frost");
        else if (cold == 1)
            postfix = (" of Chills");
        setColor(0, 0, 128);
    }
    else if (brandType < 99)
    {
        int lightning = getInt(4, 1);
        SetBrand(bLightning, lightning);
        if (lightning == 3)
            postfix = (" of the Thunder God");
        else if (lightning == 2)
            postfix = (" of Lightning");
        else if (lightning == 1)
            postfix = ( " of Static");
        setColor(128, 128, 0);
    }
    else if (brandType < 101)
    {
        int acid = getInt(4, 1);
        SetBrand(bAcid, acid);
        if (acid == 3)
            postfix = (" of Zayt al-Zaj"); //acid
        else if (acid == 2)
            postfix = (" of Corrosion");    //corrosion
        else if (acid == 1)
            postfix = (" of Weak Acid"); //oil of vitriol or  Zayt al-Zaj
        setColor(128, 0, 128);
    }
}

// --------------------------------------------------------------------------------------------------------------------------------

void Item::AddResistance(bool special)
{
    if (postfix.length() > 0)
        return;

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
            postfix = (" of Strong Poison Resistance");
        else if (poison == 2)
            postfix = (" of Medium Poison Resistance");
        else if (poison == 1)
            postfix = (" of Weak Poison Resistance");
        setColor(0, 255, 0);
    }

    else if (brandType < 96)
    {
        int fire = 3;//getInt(4,1);
        SetResistance(bFire, fire);
        if (fire == 3)
            postfix = (" of Fire Resistance");
        else if (fire == 2)
            postfix = (" of Flame Retardant");
        else if (fire == 1)
            postfix = (" of the Anti-Candle");
        setColor(128, 0, 0);

    }
    else if (brandType < 98)
    {
        int cold = 3;//getInt(4,1);
        SetResistance(bFrost, cold);
        if (cold == 3)
            postfix = (" of Ice Shield");
        else if (cold == 2)
            postfix = (" of Frostproof");
        else if (cold == 1)
            postfix = (" of Frost Resistance");

        setColor(0, 0, 128);
    }
    else if (brandType < 100)
    {
        int lightning = getInt(4, 1);
        SetResistance(bLightning, lightning);
        if (lightning == 3)
            postfix = (" of Lightning Resistance");
        else if (lightning == 2)
            postfix = (" of Sparks Resistance");
        else if (lightning == 1)
            postfix = (" of Grounding");

        setColor(128, 128, 0);
    }
    else if (brandType < 102)
    {
        int acid = getInt(4, 1);
        SetResistance(bAcid, acid);
        if (acid == 3)
            postfix = (" of Acid Resistance"); //acid
        else if (acid == 2)
            postfix = (" of Corrosion Resistance");    //corrosion
        else if (acid == 1)
            postfix = (" of Low Acid Resistance"); //oil of vitriol or  Zayt al-Zaj

        setColor(128, 0, 128);
    }
}

// --------------------------------------------------------------------------------------------------------------------------------

Item& Item::wearable(bool rWearable)
{
    mWearable = rWearable;

    return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool Item::wearable()
{
    return mWearable;
}

// --------------------------------------------------------------------------------------------------------------------------------

bool Item::stackable()
{
    return mStackable;
}

// --------------------------------------------------------------------------------------------------------------------------------

Item& Item::stackable(bool rStackable)
{
    mStackable = rStackable;

    return *this;
}

// --------------------------------------------------------------------------------------------------------------------------------
