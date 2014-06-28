// ActionManager.h: interface for the ActionManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTIONMANAGER_H__EAA46311_0242_4D53_A628_FD790FF7E066__INCLUDED_)
#define AFX_ACTIONMANAGER_H__EAA46311_0242_4D53_A628_FD790FF7E066__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "action.h"
#include "monster.h"
#include "StandardMonsterActions.h"

class ActionManager  
{
public:
	ActionManager();
	virtual ~ActionManager();

	Action* CreateAction(eAction type, int param1=1,int param2=0, int param3=0, int param4=0);
	Action* UpdateAction(Action* action,eAction type,int param1=0,int param2=0, int param3=0, int param4=0);

	StandardMonsterActions monsterAction;
};

#endif // !defined(AFX_ACTIONMANAGER_H__EAA46311_0242_4D53_A628_FD790FF7E066__INCLUDED_)
