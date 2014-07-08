// UserCommand.h: interface for the UserCommand class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USERCOMMAND_H__BC2EA827_3C5A_4DA9_B6FE_038B9255531C__INCLUDED_)
#define AFX_USERCOMMAND_H__BC2EA827_3C5A_4DA9_B6FE_038B9255531C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class UserCommand  
{
public:
	int Look(bool *keys,bool show_path=false);
	int UnLook();
	int Look();
	int MoveCommand(int dir);
	int FleeCommand(int dir);
	int GetDirection(bool *keys);
	int Close(bool *keys);
	int ClimbStairs(int dir);
	int Run();
	int InitRun();
	int RunDirection(int dir);
	int DisplayHelp();
	int DisplayHelpAbout();
	int HelpCommand(bool *keys);
	int HaltRun(int x,int y,int oldx =0,int oldy=0);
	int HaltRunFeature(int x,int y,int oldx=0,int oldy=0);
	int UseItem(Item*item,int dir);
	int UseItem();
	int UseItem(bool *keys);
	int Debug(bool *keys);
	int ThrowItem();
	int ThrowTarget(eAction action);
	int CastSpellAtTarget();
	int Flee();
	void FleeCommand();

private:
    Coord autoTarget();

    static Coord look_pos;
	static int run_dir;
    static monsterData * lastTarget;
};

#endif // !defined(AFX_USERCOMMAND_H__BC2EA827_3C5A_4DA9_B6FE_038B9255531C__INCLUDED_)
