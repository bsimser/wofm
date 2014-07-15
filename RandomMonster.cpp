// RandomMonster.cpp: implementation of the RandomMonster class.
//
//////////////////////////////////////////////////////////////////////

#include "RandomMonster.h"
#include "numbergenerator.h"

using namespace Random;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

RandomMonster::RandomMonster() :Monster()
{
}

int RandomMonster::Create(int level, int type)
{
    if (level > 9)
        level /= level + 1;

    if (type == random)
        type = getInt(troll, goblin);

    if (type == troll && level < 2)
        type = goblin;

    switch (type)
    {
    case goblin:	return	createGoblin(level);    break;
    case dwarf:	    return 	createDwarf(level);     break;
    case spider:	return 	createSpider(level);    break;
    case troll:	    return  createTroll(level);     break;
    default:	    return  createRat(level);       break;
    }

    return 0;
}


int RandomMonster::createGoblin(int level)
{
    symbol = 'g';
    name = ("goblin");

    setColor(0, 0, 200 - level * 5);

    stamina = 6 + getInt(3, 1) + (level / 3);
    skill = 6 + getInt(3, 1) + (level / 3);;
    luck = 6 + getInt(7, 1) + (level / 3);;

    sight_range = 6;
    max_stamina = stamina;

    humanoid = true;
    setDescription("A nasty smelly goblin. ");

    experience = 5 + level / 2;

    return 1;
}

int RandomMonster::createRat(int level)
{
    symbol = 'r';
    name = ("rat");

    setColor(128, 64, 32);

    stamina = 6 + getInt(3, 1) + (level / 3);
    skill   = 6 + getInt(3, 1) + (level / 3);;
    luck    = 6 + getInt(7, 1) + (level / 3);;

    sight_range = 6;
    max_stamina = stamina;

    setDescription("A large dog-sized rat with razor sharp teeth. ");

    experience = 5 + level / 2;

    return 1;
}

int RandomMonster::createTroll(int level)
{
    symbol = 'T';
    name = ("troll");
    setDescription("A strong beast with teeth and claws. ");

    setColor(128, 128, 0);

    stamina = 15 + getInt(3, 1) + (level / 3);
    skill   = 8 + getInt(3, 1) + (level / 3);
    luck    = 6 + getInt(7, 1) + (level / 3);

    sight_range = 6;
    max_stamina = stamina;

    humanoid = true;

    experience = 5 + level / 2;
    SetResistance(bFire, -10);

    return 1;
}
int RandomMonster::createDwarf(int level)
{
    symbol = 'd';
    name = ("dwarf");
    setDescription("This dwarf eyes you with suspicion. 'Do ya ave ma gold?' ");

    setColor(150, 75, 0);

    stamina = 9 + getInt(3, 1) + (level / 3);
    skill = 7 + getInt(3, 1) + (level / 3);;
    luck = 6 + getInt(7, 1) + (level / 3);;

    sight_range = 6;
    max_stamina = stamina;

    humanoid = true;

    experience = 5 + level / 2;

    return 1;
}
int RandomMonster::createSpider(int level)
{
    symbol = 'S';
    name = ("spider");
    setDescription("A large arachnid with an alien gleam in its eyes. ");

    setColor(120, 150, 0);

    stamina = 5 + getInt(3, 1) + (level / 3);
    skill   = 6 + getInt(3, 1) + (level / 3);;
    luck    = 6 + getInt(7, 1) + (level / 3);;

    sight_range = 6;
    max_stamina = stamina;

    humanoid = 0;

    experience = 5 + level / 2;
    SetBrand(bPoison, 3);
    SetResistance(bPoison, 10);

    return 1;
}
