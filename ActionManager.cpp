// ActionManager.cpp: implementation of the ActionManager class.
//
//////////////////////////////////////////////////////////////////////

#include "ActionManager.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ActionManager::ActionManager()
{

}

ActionManager::~ActionManager()
{

}


Action* ActionManager::CreateAction(eAction type, int param1, int param2, int param3, int param4)
{
    Action *action = new Action;
    action->CreateAction(type);

    switch (type)
    {
    case aMove: action->param1 = param1; action->param2 = param2; action->param2 = param2; break; //direction
    case aAttack: action->param1 = param1; action->param2 = param2; break; //direction
    }

    return action;
}

Action* ActionManager::UpdateAction(Action* action, eAction type, int param1, int param2, int param3, int param4)
{
    action->CreateAction(type);

    switch (type)
    {
    case aMove: action->param1 = param1; action->param2 = param2; action->param3 = param3;  break; //direction
    case aAttack: action->param1 = param1; break; //direction
    case aDrop: action->param1 = param1; break; //direction
    case aFire: action->param1 = param1; action->param2 = param2; break; //direction
    default: action->param1 = param1; action->param2 = param2; action->param3 = param3; action->param4 = param4; break;

    }

    return action;
}

