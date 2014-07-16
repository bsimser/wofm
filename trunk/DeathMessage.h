// DeathMessage.h: interface for the DeathMessage class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DEATHMESSAGE_H__F9F9626C_0FD6_460E_B14F_0575D7631A0E__INCLUDED_)
#define AFX_DEATHMESSAGE_H__F9F9626C_0FD6_460E_B14F_0575D7631A0E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

typedef struct 
{
	bool gold;
	bool barracks;
	bool river;
	bool undead;
    bool maze;
    bool cheese;
	bool entered;
	bool zagor;
	int level;

}Achievements;

class DeathMessage  
{
public:
	DeathMessage();

	int ShowDeath(int success);

	void SetDeathMessage(char * msg,...);
	Achievements done;

private:
	std::string death_message;

};

#endif // !defined(AFX_DEATHMESSAGE_H__F9F9626C_0FD6_460E_B14F_0575D7631A0E__INCLUDED_)
