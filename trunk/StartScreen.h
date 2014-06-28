// StartScreen.h: interface for the StartScreen class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STARTSCREEN_H__1D5424E9_99A9_4C84_A282_EEEEB647DB79__INCLUDED_)
#define AFX_STARTSCREEN_H__1D5424E9_99A9_4C84_A282_EEEEB647DB79__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum eStartState
{
	sIntro,
	sName,
	sClass,
	sComplete

};

class StartScreen  
{
public:
	StartScreen();
	virtual ~StartScreen();

	virtual void Display();

	eStartState state;

	virtual int PassInput(bool *keys);
	virtual void GetPlayerInfo();

	virtual int GetName(bool *keys);
	virtual int GetClass(bool *keys);

	std::string pName;
	std::string pClass;
	int length;


};

#endif // !defined(AFX_STARTSCREEN_H__1D5424E9_99A9_4C84_A282_EEEEB647DB79__INCLUDED_)
