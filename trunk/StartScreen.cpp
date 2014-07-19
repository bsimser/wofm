// --------------------------------------------------------------------------------------------------------------------------------
//  DEMISERL
//  Copyright 2014 Corremn
//
// $LastChangedBy$ 
// $LastChangedDate$ 
// $LastChangedRevision$ 
// $HeadURL: $ 
// --------------------------------------------------------------------------------------------------------------------------------

#include "WorldBuilder.h"
#include "StartScreen.h"

#define VERSION "version 1.0.0 Release Candidate 01"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

StartScreen::StartScreen():
    state(sIntro),
    length(0),
    pName(""),
    pClass("Adventurer")
{
}

StartScreen::~StartScreen()
{

}

void StartScreen::Display()
{
    World.getTextManager().ClearDisplayLines();

    World.getTextManager().SetDisplayLine(0, "The Warlock of FIRETOP MOUNTAIN: Roguelike by Corremn");
    World.getTextManager().SetDisplayLine(1, "=====================================================");
    World.getTextManager().SetDisplayLine(3, VERSION);

    World.getTextManager().SetDisplayLine(5, "Deep in the caverns beneath Firetop Mountain lies an untold wealth of treasure,");
    World.getTextManager().SetDisplayLine(6, "guarded by a powerful Warlock, or so the rumour goes.");
    World.getTextManager().SetDisplayLine(7, "Several adventurers like yourself have set off for Firetop Mountain in search");
    World.getTextManager().SetDisplayLine(8, "of the Warlock's hoard. None have ever returned. Do you dare follow them?");

    World.getTextManager().SetDisplayLine(10, "You take the role of an adventurer on a quest to find the treasure of a powerful Warlock,");
    World.getTextManager().SetDisplayLine(11, "hidden deep within Firetop Mountain. People from a nearby village advise that the");
    World.getTextManager().SetDisplayLine(12, "treasure is stored in a chest with two locks, and that the keys are guarded by various");
    World.getTextManager().SetDisplayLine(13, "creatures within the dungeons. You approach the shadow of the great mountain.");

    World.getTextManager().SetDisplayLine(15, "The local villagers have warned you that the entrance is well guarded...");
    int i = 18;
    World.getTextManager().SetDisplayLine(++i, "                                         ");
    World.getTextManager().SetDisplayLine(++i, "                                         ");
    World.getTextManager().SetDisplayLine(++i, "                                           \\`.  /'            ");
    World.getTextManager().SetDisplayLine(++i, "                    ^^                      \\ '/ '',        ,.      ");
    World.getTextManager().SetDisplayLine(++i, "                          ^^           |\\/|__)/ _''__.-.   /      ");
    World.getTextManager().SetDisplayLine(++i, "                    /\\                 (bd)~~;;.,_(-~'`''.'      ");
    World.getTextManager().SetDisplayLine(++i, "                   /-'\\  /\\    /\\       ^^      ~ ~");
    World.getTextManager().SetDisplayLine(++i, "                  /    \\/`-\\  /`-\\          ");
    World.getTextManager().SetDisplayLine(++i, "                \\/  _   \\   \\/    \\           __,-._       ");
    World.getTextManager().SetDisplayLine(++i, "                    o    \\   ^     \\-.''._,--' ..   `-''.._.  ");
    World.getTextManager().SetDisplayLine(++i, "                    --,        ..    ");
    World.getTextManager().SetDisplayLine(++i, "                       '..~*       .  ");

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
   // World.getTextManager().SetDisplayLine(20, "WARNING: Dubug Version for authors eyes only!");
#endif

    World.getTextManager().SetDisplayLine(32, "Based on the book by Ian Livingstone & Steve Jackson - 1982.");
    World.getTextManager().SetDisplayLine(33, "Press '?' to get help on commands. Good Luck!");
    World.getTextManager().SetDisplayLine(34, "Press 'F1' for fullscreen.");
    World.getTextManager().SetDisplayLine(34, "corremn@gmail.com");

    World.getTextManager().SetDisplayLine(39, "[space] to Start");
}

void StartScreen::GetPlayerInfo()
{
    World.getTextManager().ClearDisplayLines();

    World.getTextManager().SetDisplayLine(0, "The Warlock of Firetop Mountain");
    World.getTextManager().SetDisplayLine(1, "===============================");

    if (state == sName)
    {
        char sbuf[128];
        sprintf(sbuf, "  Name: %s_", pName.c_str());
        World.getTextManager().SetDisplayLine(4, sbuf);

        World.getTextManager().SetDisplayLine(39, "Enter Name  %s", (length > 12) ? "(Max length!!!)":"");
    }
    if (state == sClass)
    {
        World.getTextManager().SetDisplayLine(4, "  Choose Class:");
        World.getTextManager().SetDisplayLine(6, "    1: Adventurer");
        World.getTextManager().SetDisplayLine(7, "    2: Warlock");

        World.getTextManager().SetDisplayLine(39, "Choose Class [1-2]");
    }
    else if (state == sComplete)
    {
        World.getTextManager().SetDisplayLine(4, "  Welcome %s the %s.", pName.c_str(), pClass.c_str());

        if (pClass == "Warlock")
        {
            if (fopen("Zagor", "r") || fopen("Warlock", "r"))
            {
                World.getTextManager().SetDisplayLine(6, "  Press 'z' in game to access your spells. ");
            }
        }
        World.getTextManager().SetDisplayLine(39, "[space] to begin");
    }
}

int StartScreen::PassInput(bool *keys)
{
    if (state == sIntro)
    {
        if (keys[VK_SPACE])
        {
            if (fopen("warlock", "r"))
            {
                state = sClass;
            }
            else
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
    }
    else if(state == sClass)
    {
        if (GetClass(keys))
        {
            state = sName;
        }
        GetPlayerInfo();
    }
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
            case 1: pClass = "Adventurer"; break;
            //case 2: pClass = "Priest"; break;
            //case 3: pClass = "Thief"; break;
            case 2: pClass = "Warlock"; break;
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



                                                                                   


