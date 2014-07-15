// Action.h: interface for the Action class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTION_H__F9EB098E_02A2_414C_B8BD_1C4332FD4F8F__INCLUDED_)
#define AFX_ACTION_H__F9EB098E_02A2_414C_B8BD_1C4332FD4F8F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


enum eAction
{
	aWait,
	aMove,
	aAttack,
	aPickup,
	aDrop,
	aCastSpell,
    aFire,
    aThrow
};


class Action  
{
public:
	Action();

	void CreateAction(eAction _type);

	int param1;
	int param2;
	int param3;
	int param4;

	int BaseSpeed(){return action_base_speed;};

	eAction Type(){return type;};

private:

	

	void BaseSpeed(int speed){action_base_speed = speed;};

	void CreateWait(); 
	void CreateMove(); 
	void CreateAttack(); 
	void CreatePickup(); 
	void CreateDrop(); 
	void CreateCastSpell(); 
	void CreateFire(); 

	int action_base_speed;
	eAction type;

};

#endif // !defined(AFX_ACTION_H__F9EB098E_02A2_414C_B8BD_1C4332FD4F8F__INCLUDED_)
