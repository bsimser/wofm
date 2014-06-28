// TextManager.cpp: implementation of the TextManager class.
//
//////////////////////////////////////////////////////////////////////
#pragma warning(disable : 4786) 


#include "TextManager.h"
#include "WorldBuilder.h"



#define STEP 90	//max characters on screen before the -more- is used

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TextManager::TextManager() :
death_print(0)
{
    newLine("The Warlock of Firetop Mountain bids you welcome. 'Mahahahahah!!'");
    m_step = 0;

}

TextManager::~TextManager()
{

}


void TextManager::ClearDisplayLines()
{
    for (int i = 0; i < 40; i++)
        SetDisplayLine(i, " ");
}

int TextManager::newLine(char * fmt, ...)
{
    char		text[256];				// Holds Our String
    va_list		ap;						// Pointer To List Of Arguments


    if (fmt == NULL)					// If There's No Text
        return 0;							// Do Nothing

    va_start(ap, fmt);					// Parses The String For Variables
    vsprintf(text, fmt, ap);			// And Converts Symbols To Actual Numbers
    va_end(ap);

    m_step = 0;
    display_line.append(text);


    if (messageList.size() > 0 && strcmp(text, messageList.back().c_str()) == 0) //bunch up messages of the same 
    {
        messageList.back().append(" x2");
    }

    else
        messageList.push_back(text);

    if (messageList.size() > 30)
        messageList.pop_front();
    return 1;

}

int  TextManager::clear()
{
    display_line = "";
    return 1;
}

void TextManager::MoreStep(int clear)
{
    if (clear)
        m_step = 0;
    else
        m_step++;

}

int  TextManager::More()
{
    std::string temp;//= display_line.copy(0,10);;

    char tt[STEP + 1];
    int	step = m_step*STEP;
    display_line1.erase();

    eDisplayState preState = WorldBuilder::State();
    if (preState == sMore)
        preState = sNormal;

    int count = display_line.size() - STEP * m_step;

    if (display_line.size() > STEP && count > STEP) //use more flag
    {
        display_line.copy(tt, STEP, 0 + step);
        display_line1.append(tt, STEP);
        display_line1 += " -more-";

        WorldBuilder::SetState(sMore);
    }
    else if (display_line.size() != count && count > 0) //last line after more
    {
        display_line.copy(tt, count, 0 + step);
        display_line1.append(tt, count);
        WorldBuilder::SetState(preState);
    }
    else	//no need to use more
    {
        display_line1 = display_line;
        WorldBuilder::SetState(preState);
        m_step = 0;
    }

    return 1;
}

std::string * TextManager::GetMessageLine(int id)
{
    More();
    return &display_line1;
}

std::string * TextManager::GetDisplayLine(int id)
{
    return &display_lines[id];
}

void TextManager::SetDisplayLine(int id, char * str, ...)
{
    if (id >= 40) 
        return;

    char		text[256];				// Holds Our String
    va_list		ap;						// Pointer To List Of Arguments

    if (str == NULL)					// If There's No Text
        return;							// Do Nothing

    va_start(ap, str);					// Parses The String For Variables
    vsprintf(text, str, ap);			// And Converts Symbols To Actual Numbers
    va_end(ap);

    display_lines[id] = text;
}

void TextManager::ShowMessages()
{
    STRINGLIST::iterator it;

    WorldBuilder::textManager.ClearDisplayLines();

    SetDisplayLine(0, "Previous Messages");
    SetDisplayLine(1, "=================");
    SetDisplayLine(2, "");

    int i = 3;

    for (it = messageList.begin(); it != messageList.end(); it++, i++)
    {
        SetDisplayLine(i, (char *)it->c_str());
    }

    SetDisplayLine(39, "[+/-] scroll - [x] to exit");

}

void TextManager::PrintDisplay(const char * filename, int print_lines)
{
    if (print_lines > 40)
        print_lines = 40;

    std::ofstream ofile;

    std::string new_file = filename;
    new_file.append(".txt");

    ofile.open(new_file.c_str());

    if (!ofile.is_open())
    {
        char err[128];
        sprintf(err, "Could not open %s", new_file.c_str());
        throw std::exception(err);
    }

    for (int i = 0; i < print_lines; i++)
    {
        ofile << display_lines[i] << std::endl;
    }
    ofile << std::endl;

    //STRINGLIST::iterator inv;

  /*  STRINGLIST::iterator it;
    for (it = messageList.begin(); it != messageList.end(); it++)
    {
        ofile << it->c_str() << std::endl;
    }*/

    ofile.close();
}
