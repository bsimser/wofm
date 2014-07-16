// SpecialGuardians.cpp: implementation of the SpecialGuardians class.
//
//////////////////////////////////////////////////////////////////////

#include "SpecialGuardians.h"
#include "numbergenerator.h"

using namespace Random;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpecialGuardians::SpecialGuardians() :Monster()
{

}

SpecialGuardians::~SpecialGuardians()
{

}

int SpecialGuardians::Create(int minorType, int level)
{

    type = 7; //mGuards;
    
    switch (minorType)
    {
    case 0:	return	createGuard(level); break;
    case 1:	return  createGuard(level); break;
    case 2:	return 	createGuard(level); break;
    case 3:	return 	createGuard(level); break;
    case 4:	return 	createGuard(level); break;
    case 5:	return 	createGuard(level); break;
    case 6:	return 	createGuard(level); break;
    case 7:	return 	createGuard(level); break;
    case 8:	return 	createGuard(level); break;
    case 9:	return 	createGuard(level); break;

    default: throw std::exception("Invalid special guardians type passed into monster creator"); break;
    }

    return 0;
}

int SpecialGuardians::createGuard(int level)
{
    level -= 10;
    if (level == 0)
    {
        symbol = 'Z';
        name = ("skeleton guard");
        setDescription("Look its a walking skeleton. ");

        setColor(230, 230, 230);

        stamina = 6 + getInt(3, 1);
        skill = 7 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 8;
        max_stamina = stamina;

        humanoid = true;
        undead = true;

        SetResistance(bPoison, 10);
        SetResistance(bFrost, 5);
        SetResistance(bFire, -10);
        SetResistance(bLightning, -5);

        experience = 15;
    }
    else if (level == 1)
    {
        symbol = 'o';
        name = ("orc captain");
        setDescription("Larger than most orcs, its a leader of some type. ");

        setColor(128, 0, 0);

        stamina = 9 + getInt(5, 2);
        skill = 8 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        humanoid = true;

        experience = 15;

    }
    else if (level == 2)
    {
        symbol = 'T';
        name = ("troll");
        setDescription("A long-limbed beast with teeth and claws. It has a cunning intelligence about it. ");

        setColor(128, 64, 64);

        stamina = 15 + getInt(3, 1);
        skill = 7 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        humanoid = true;

        SetResistance(bFire, -10);
        SetResistance(bLightning, -5);

        experience = 15;
    }
    else if (level == 3)
    {
        symbol = '0';
        name = ("ogre");
        setDescription("Big strong and smelly and I think it wants to eat you. ");

        setColor(20, 80, 20);

        stamina = 14 + getInt(3, 1);
        skill = 8 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        humanoid = true;

        experience = 15;
    }
    else if (level == 4)
    {
        symbol = 'Z';
        name = ("skeleton pirate");
        setDescription("A regular skeleton but with a patch over one of it eye socket. ");

        setColor(255, 255, 155);

        stamina = 10 + getInt(3, 1);
        skill = 9 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        humanoid = true;
        undead = true;
        experience = 15;

        SetResistance(bPoison, 10);
        SetResistance(bFrost, 6);
        SetResistance(bFire, -10);

    }
    else if (level == 5)
    {
        symbol = 'S';
        name = ("giant cave spider");
        setDescription("This enormous hairy spider has venom dripping from is fangs.");

        setColor(255, 128, 64);

        stamina = 13 + getInt(3, 1);
        skill = 9 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        humanoid = 0;

        experience = 20;
        SetBrand(bPoison, 4);
        SetResistance(bPoison, 5);

    }
    else if (level == 6)
    {
        symbol = 'W';
        name = ("giant sandworm");

        setDescription("This chitinous monstrosity is a giant sandworm, its looking to swallow you whole. ");
        setColor(128, 128, 63);

        stamina = 15 + getInt(3, 1);
        skill = 10 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        humanoid = 0;

        experience = 20;

    }
    else if (level == 7)
    {
        symbol = 'V';
        name = ("wight");

        setDescription("Just another of the undead set to guard what ever is in that chest. ");

        setColor(230, 230, 230);

        stamina = 16 + getInt(3, 1);
        skill = 10 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        humanoid = true;
        undead = true;

        SetResistance(bPoison, 10);
        SetResistance(bFrost, 5);
        SetResistance(bFire, -10);

        experience = 20;

    }
    else if (level == 8)
    {
        symbol = 'M';
        name = ("blind minotaur");
        setDescription("A sightless minotaur, yet somehow it can see you with out eyes. ");

        setColor(150, 0, 0);

        stamina = 16 + getInt(3, 1);
        skill = 10 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 30;
        max_stamina = stamina;

        humanoid = true;

        experience = 20;
    }
    else if (level == 9)
    {
        symbol = 'D';
        name = ("dragon");
        setDescription("Its a dragon, you have probably read descriptions about them before. ");

        setColor(255, 0, 0);

        stamina = 18 + getInt(3, 1);
        skill = 12 + getInt(3, 1);
        luck = 6 + getInt(7, 1);

        sight_range = 6;
        max_stamina = stamina;

        SetResistance(bFire, 10);
        SetResistance(bFrost, -5);

        humanoid = 0;

        experience = 20;
    }

    return 1;
}