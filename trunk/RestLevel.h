// RestLevel.h: interface for the RestLevel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RESTLEVEL_H__DD933A6C_8B90_4365_975D_CE3C08ADCD20__INCLUDED_)
#define AFX_RESTLEVEL_H__DD933A6C_8B90_4365_975D_CE3C08ADCD20__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class RestLevel  
{
public:
	RestLevel();
	virtual ~RestLevel();

	void Rest(int level);
	int Resting(int level);
	void ResetRest(){rest_count = 0;sleep="";encounterFlag=0;};
	void Encounter();

	int GetOldLevel(){return old_level;};
	int restComplete[4];

private:
	int rest_count;
	std::string sleep;
	int old_level;
	int encounterFlag;




};

#endif // !defined(AFX_RESTLEVEL_H__DD933A6C_8B90_4365_975D_CE3C08ADCD20__INCLUDED_)
