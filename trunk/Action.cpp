// Action.cpp: implementation of the Action class.
//
//////////////////////////////////////////////////////////////////////

#include "Action.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Action::Action() :
action_base_speed(10),
type(aWait)
{
}


void Action::CreateAction(eAction _type)
{

    type = _type;

    switch (type)
    {
    case aWait:			CreateWait(); break;
    case aMove:			CreateMove(); break;
    case aAttack:		CreateAttack(); break;
    case aPickup:		CreatePickup(); break;
    case aDrop:			CreateDrop(); break;
    case aCastSpell:	CreateCastSpell(); break;
    case aFire:			CreateFire(); break;

    }
}

void Action::CreateWait()
{
    BaseSpeed(5);
}

void Action::CreateMove()
{
    BaseSpeed(10);
}

void Action::CreateAttack()
{
    BaseSpeed(8);
}

void Action::CreatePickup()
{
    BaseSpeed(10);
}

void Action::CreateDrop()
{
    BaseSpeed(10);
}

void Action::CreateCastSpell()
{
    BaseSpeed(8);
}

void Action::CreateFire()
{
    BaseSpeed(10);
}
