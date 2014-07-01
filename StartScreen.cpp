// StartScreen.cpp: implementation of the StartScreen class.
//
//////////////////////////////////////////////////////////////////////

#include "WorldBuilder.h"
#include "StartScreen.h"

#define VERSION "version 0.2.1"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StartScreen::StartScreen():
    state(sIntro),
    length(0),
    pName(""),
    pClass("")
{
}

StartScreen::~StartScreen()
{

}

void StartScreen::Display()
{
    WorldBuilder::textManager.ClearDisplayLines();

    WorldBuilder::textManager.SetDisplayLine(0, "The Warlock of FIRETOP MOUNTAIN: Roguelike by Corremn");
    WorldBuilder::textManager.SetDisplayLine(1, "=============================================");
    WorldBuilder::textManager.SetDisplayLine(3, VERSION);

    WorldBuilder::textManager.SetDisplayLine(5, "Deep in the caverns beneath Firetop Mountain lies an untold wealth of treasure,");
    WorldBuilder::textManager.SetDisplayLine(6, "guarded by a powerful Warlock, or so the rumour goes.");
    WorldBuilder::textManager.SetDisplayLine(7, "Several adventurers like yourself have set off for Firetop Mountain in search");
    WorldBuilder::textManager.SetDisplayLine(8, "of the Warlock's hoard. None have ever returned. Do you dare follow them?");

    WorldBuilder::textManager.SetDisplayLine(10, "You take the role of an adventurer on a quest to find the treasure of a powerful Warlock,");
    WorldBuilder::textManager.SetDisplayLine(11, "hidden deep within Firetop Mountain. People from a nearby village advise that the");
    WorldBuilder::textManager.SetDisplayLine(12, "treasure is stored in a chest with two locks, and that the keys are guarded by various");
    WorldBuilder::textManager.SetDisplayLine(13, "creatures within the dungeons. You approach the shadow of the great mountain.");

    WorldBuilder::textManager.SetDisplayLine(15, "The local villages have warned you that the entrance is well guarded...");
    int i = 18;
    WorldBuilder::textManager.SetDisplayLine(++i, "                                         ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                                         ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                                           \\`.  /'            ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                    ^^                      \\ '/ '',        ,.      ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                          ^^           |\\/|__)/ _''__.-.   /      ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                    /\\                 (bd)~~;;.,_(-~'`''.'      ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                   /-'\\  /\\    /\\       ^^      ~ ~");
    WorldBuilder::textManager.SetDisplayLine(++i, "                  /    \\/`-\\  /`-\\          ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                \\/  _   \\   \\/    \\           __,-._       ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                    o    \\   ^     \\-.''._,--' ..   `-''.._.  ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                    --,        ..    ");
    WorldBuilder::textManager.SetDisplayLine(++i, "                       '..~*       .  ");

    /*
                         \`.  /'                                                 
                          \ '/''          ,.                                  
    /\               |\/|__)/ _''__.-.   /                                   
   /-'\  /\    /\    (bd)~~;;.,_(-~'' '.'                                          
  /    \/`-\  /` \    ^^      ~ ~                                                     
 /      \   --    \           __,-._                                              
    o    \         \-.''._,--'      `-''.._.                                           
    */

#ifdef _DEBUG
   // WorldBuilder::textManager.SetDisplayLine(20, "WARNING: Dubug Version for authors eyes only!");
#endif

    WorldBuilder::textManager.SetDisplayLine(32, "Based on the book by Ian Livingstone & Steve Jackson - 1982.");
    WorldBuilder::textManager.SetDisplayLine(33, "Press '?' to get help on commands. Good Luck!");

    WorldBuilder::textManager.SetDisplayLine(39, "[space] to Start");
}

void StartScreen::GetPlayerInfo()
{
    WorldBuilder::textManager.ClearDisplayLines();

    WorldBuilder::textManager.SetDisplayLine(0, "The Warlock of Firetop Mountain");
    WorldBuilder::textManager.SetDisplayLine(1, "===============================");

    char sbuf[128];
    sprintf(sbuf, "Name: %s_", pName.c_str());
    WorldBuilder::textManager.SetDisplayLine(4, sbuf);

    WorldBuilder::textManager.SetDisplayLine(39, "Enter Name  %s", (length > 12) ? "(Max length!!!)":"");

    if (state == sClass)
    {
        WorldBuilder::textManager.SetDisplayLine(7, "Choose Class (not implemented)");
        WorldBuilder::textManager.SetDisplayLine(9, "1: Warrior");
        WorldBuilder::textManager.SetDisplayLine(10, "2: Priest");
        WorldBuilder::textManager.SetDisplayLine(11, "3: Thief");
        WorldBuilder::textManager.SetDisplayLine(12, "4: Wizard");

        WorldBuilder::textManager.SetDisplayLine(39, "Enter [1-4]");

    }
    else if (state == sComplete)
    {
        pClass = "Adventurer";

        WorldBuilder::textManager.SetDisplayLine(4, "Welcome %s the %s.", pName.c_str(), pClass.c_str());
        WorldBuilder::textManager.SetDisplayLine(39, "[space] to begin");
    }
}

int StartScreen::PassInput(bool *keys)
{
    if (state == sIntro)
    {
        if (keys[VK_SPACE])
        {
            state = sName;
            GetPlayerInfo();
        }
        return 1;
    }
    else if (state == sName)
    {
        if (keys[VK_RETURN] && length > 1)
        {
            state = sComplete;
            GetPlayerInfo();
        }
        else
        {
            GetName(keys);
            GetPlayerInfo();
        }
        return 1;
    }/*
    else if(state ==sClass)
    {
    if(GetClass(keys))
    state = sComplete;
    GetPlayerInfo();
    }*/
    else if (state == sComplete)
    {
        if (keys[VK_SPACE] || keys[VK_ESCAPE])
            return 0;
    }
    return 1;
}

int StartScreen::GetClass(bool *keys)
{
    for (int i = 49; i < 53; i++)
    {
        if (keys[i])
        {
            int gogo = i - 48;
            switch (i - 48)
            {
            case 1: pClass = "Warrior"; break;
            case 2: pClass = "Priest"; break;
            case 3: pClass = "Thief"; break;
            case 4: pClass = "Wizard"; break;
            }
            return 1;
        }
    }
    return 0;
}

int StartScreen::GetName(bool *keys)
{
    bool shift = false;

    if (keys[VK_SHIFT])
        shift = true;

    if ((keys[VK_DELETE] || keys[VK_BACK]) && length >= 0)
    {
        keys[VK_DELETE] = false;
        keys[VK_BACK] = false;

        if(length > 0)
        {
            length--;
            pName.assign(pName.c_str(), length);
        }
        return 0;
    }

    if (length > 12)
        return 0;

    if (keys[VK_SPACE])
    {
        keys[VK_SPACE] = false;
        pName += ' ';
        length++;
        return 1;
    }

    int i;
    bool found = false;
    for (i = 48; i<123; i++) //alpha keys only
    {
        if (keys[i])
        {
            found = true;
            break;
        }
    }

    if (i > 122 || found == false) //not anything
        return 0;

    char c[1];

    if (shift || i < 65)
        c[0] = i;
    else
        c[0] = i + 32;

    pName.append(c, 1);
    length++;
    return 1;
}



                                                                                   


