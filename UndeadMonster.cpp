// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------


#include "UndeadMonster.h"

#include "numbergenerator.h"

using namespace Random;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

UndeadMonster::UndeadMonster() :Monster()
{
}

UndeadMonster::~UndeadMonster()
{
}

int UndeadMonster::Create(int subtype, int level)
{
    type = 1; //mUndead;

    SetResistance(bPoison, 10);
    SetResistance(bFire, -5);
    SetResistance(bLightning, -5);

    undead = 1;
    switch (subtype)
    {
    case skeleton:	return	createSkeleton();
    case zombie:	return  createZombie();
    case ghoul:		return 	createGhoul();
    case vampire:	return 	createVampire();
    default: throw std::exception("Invalid undead type passed into monster creator"); break;
    }

    return 0;
}

int UndeadMonster::createSkeleton()
{
    symbol = 'z';
    name = ("skeleton");
    setDescription("A walking skeleton brought back to life to guard the inner mountain. ");

    setColor(230, 230, 230);

    stamina = 5 + getInt(3, 1);
    skill = 7 + getInt(3, 1);
    luck = 6 + getInt(7, 1);

    sight_range = 6;
    max_stamina = stamina;

    humanoid = true;

    experience = 5;

    return 1;
}

int UndeadMonster::createZombie()
{
    symbol = 'z';
    name = ("shambling zombie");
    setDescription("This poor soul has be returned from death to do its masters bidding. ");

    setColor(134, 85, 70);

    stamina = 6 + getInt(3, 1);
    skill = 8 + getInt(3, 1);
    luck = 6 + getInt(3, 1);

    sight_range = 5;
    max_stamina = stamina;

    humanoid = true;
    SetResistance(bFrost, 5);

    experience = 8;

    return 1;
}

int UndeadMonster::createGhoul()
{
    symbol = 'c';
    name = ("rotting ghoul");
    setDescription("A rotting corpse, no wait its moving and coming straight towards you. ");

    SetBrand(bParalysis, 2);

    setColor(128, 128, 128);

    stamina = 7 + getInt(3, 1);
    skill = 8 + getInt(3, 1);
    luck = 7 + getInt(3, 1);

    sight_range = 5;
    max_stamina = stamina;

    humanoid = true;

    experience = 10;
    SetResistance(bFrost, 5);

    return 1;
}

int UndeadMonster::createVampire()
{
    symbol = 'V';
    stamina = 8 + getInt(3, 1);
    skill = 10 + getInt(3, 1);
    luck = 8 + getInt(3, 1);

    if (Random::getInt(2, 0))
    {
        name = ("big scary vampire");
        setDescription("This gig and scary vampire is big and scary. ");
        setColor(100, 100, 200);
        experience = 15;
    }
    else
    {
        name = ("vampire");
        setDescription("A vampire with hypnotic eyes. ");
        skill--;
        setColor(100, 100, 140);
        experience = 13;
    }
    SetBrand(bConfusion, 2);

    sight_range = 5;
    max_stamina = stamina;

    humanoid = true;
    SetResistance(bFrost, 5);

    return 1;
}
