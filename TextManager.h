// TextManager.h: interface for the TextManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXTMANAGER_H__3613E612_B1B7_4D45_8388_4D57B782146E__INCLUDED_)
#define AFX_TEXTMANAGER_H__3613E612_B1B7_4D45_8388_4D57B782146E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <list>

typedef std::list<std::string> STRINGLIST;

class TextManager  
{
public:
	TextManager();
	virtual ~TextManager();

	int More();
	void MoreStep(int clear = 0);

	std::string *  GetDisplayLine(int id);
	void SetDisplayLine(int id,char * str,...);
	std::string *  GetMessageLine(int id);
	void PrintDisplay(const char* name, int print_lines);

	void ClearDisplayLines();

	void ShowMessages();
		
	int newLine(char * text, ...);
	int clear();

	int m_step;

    /**
     * @brief The message list
     */
	STRINGLIST messageList;

	std::string display_line;

	std::string display_line1;
	std::string display_line2;

private:
	std::string display_lines[40]; //display text instead of map
	int death_print;

};

#endif // !defined(AFX_TEXTMANAGER_H__3613E612_B1B7_4D45_8388_4D57B782146E__INCLUDED_)
