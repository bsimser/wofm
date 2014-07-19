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
#include "DeathMessage.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

DeathMessage::DeathMessage()
{
    SetDeathMessage("was killed by a severe case of death. ");
    done.gold = 0;
    done.barracks = 0;
    done.river = 0;
    done.undead = 0;
    done.maze = 0;
    done.level = 0;
    done.entered = 0;
    done.zagor = 0;
}

int DeathMessage::ShowDeath(int success)
{
    bool has_curse = false;

    ITEMLIST::iterator it;

    int print_lines = 0;

    for (it = World.getMonsterManager().Player()->inventory.begin(); it != World.getMonsterManager().Player()->inventory.end(); it++)
    {

        //if(it->itemNumber[0] >0 &&it->itemNumber[1] >0 && it->itemNumber[2] >0)
        //{
        //	death_message = "later died of the Curse of Zagor. ";
        //}
        if (it->type == gold)
        {
            done.gold = 1;
            fopen("warlock", "w");
        }
    }

    World.getTextManager().ClearDisplayLines();

    if (success)
    {
        World.getTextManager().SetDisplayLine(0, "Escaped");
        World.getTextManager().SetDisplayLine(1, "=======");
    }
    else
    {
        World.getTextManager().SetDisplayLine(0, "A great adventurer has fallen");
        World.getTextManager().SetDisplayLine(1, "=============================");
    }
    World.getTextManager().SetDisplayLine(2, "");

    World.getTextManager().SetDisplayLine(3, "%s the %s %s", World.getMonsterManager().Player()->Name().c_str(), World.getStartScreen().pClass.c_str(), death_message.c_str());

    World.getTextManager().SetDisplayLine(4, "He survived for %d turns.", World.GetTurns());

    World.getTextManager().SetDisplayLine(5, "He achieved level %d with %d experience.", World.getMonsterManager().Player()->experience_level,
        World.getMonsterManager().Player()->experience);
    World.getTextManager().SetDisplayLine(6, "He killed %d monsters out of %d.", World.getMonsterManager().Player()->Kills(),
        World.getMonsterManager().GetNumMonsters() - 1);

    World.getTextManager().SetDisplayLine(8, "Skill: %d, Stamina: %d, Luck: %d ", World.getMonsterManager().Player()->monster.skill, World.getMonsterManager().Player()->monster.MaxStamina(), World.getMonsterManager().Player()->monster.luck);

    print_lines = 9;

    if (success)
    {
        if (done.gold)
            World.getTextManager().SetDisplayLine(10, "%s retires rich and famous. What a hero!", World.getMonsterManager().Player()->Name().c_str());
        else if (done.zagor)
            World.getTextManager().SetDisplayLine(10, "%s leaves the mountain and the treasure behind.", World.getMonsterManager().Player()->Name().c_str());
        else
            World.getTextManager().SetDisplayLine(10, "%s leaves and never to looks back. Rather pointless really!", World.getMonsterManager().Player()->Name().c_str());
        print_lines = 11;
    }

    else
    {
        if (!done.zagor)
            World.getTextManager().SetDisplayLine(10, "As darkness surrounds %s, the last sound he hears is laughter. ", World.getMonsterManager().Player()->Name().c_str());
        else
            World.getTextManager().SetDisplayLine(10, "So much promise, so many enemies. ", World.getMonsterManager().Player()->Name().c_str());
        print_lines = 11;
    }

    int i = print_lines + 1;
    if (done.entered)
    {
        World.getTextManager().SetDisplayLine(i, "He entered Firetop Mountain.");
        i++;
    }
    if (done.barracks)
    {
        World.getTextManager().SetDisplayLine(i, "He found the orcish barracks.");
        i++;
    }
    if (done.river)
    {
        World.getTextManager().SetDisplayLine(i, "He crossed a large underground river.");
        i++;
    }
    if (done.undead)
    {
        World.getTextManager().SetDisplayLine(i, "He passed through an underground cemetery.");
        i++;
    }
    if (done.maze)
    {
        World.getTextManager().SetDisplayLine(i, "He discovered the maze of Zagor.");
        i++;
    }
    if (done.cheese)
    {
        World.getTextManager().SetDisplayLine(i, "You threw cheese at a wizard.");
        i++;
    }
    if (done.zagor)
    {
        World.getTextManager().SetDisplayLine(i, "He defeated Zagor the evil Warlock.");
        i++;
    }
    if (done.gold)
    {
        World.getTextManager().SetDisplayLine(i, "He found Zagors Gold!");
        i++;
    }
    //equipment
    i++;
    World.getTextManager().SetDisplayLine(i++, "Equipment");
    ITEMLIST::iterator inv;
    i++;
    for (inv = World.getMonsterManager().Player()->inventory.begin(); inv != World.getMonsterManager().Player()->inventory.end(); inv++, i++)
    {
        if (inv->equipped)
        {
            World.getTextManager().SetDisplayLine(i, "%s", inv->GetName().c_str());
        }
        else i--;
    }
    i++;
    /*World.getTextManager().SetDisplayLine(i++, "Inventory");

    for (inv = World.getMonsterManager().Player()->inventory.begin(); inv != World.getMonsterManager().Player()->inventory.end(); inv++, i++)
    {
        if (!inv->equipped)
        {
            World.getTextManager().SetDisplayLine(i, inv->GetName().c_str());
        }
        else i--;
    }*/

    World.getTextManager().SetDisplayLine(++i, "Messages");
    STRINGLIST::reverse_iterator rStrIt;
    i += 6; //show five messages
    int numMesStr = 3;
    for (rStrIt = World.getTextManager().messageList.rbegin(); rStrIt != World.getTextManager().messageList.rend(); rStrIt++, i--)
    {
        World.getTextManager().SetDisplayLine(i, (char *)(*rStrIt).c_str());
        if (numMesStr++ == 7)
            break;
    }
    i += 5;

    World.getTextManager().SetDisplayLine(35, "Output to %s.txt", World.getMonsterManager().Player()->monster.name.c_str());

    World.getTextManager().SetDisplayLine(39,/*[i] see inventory,*/"[x] to quit");

    World.getTextManager().PrintDisplay(World.getMonsterManager().Player()->monster.name.c_str(), i);
    return 1;
}

void DeathMessage::SetDeathMessage(char * msg, ...)
{
    char		text[256];				// Holds Our String
    va_list		ap;						// Pointer To List Of Arguments

    if (msg == NULL)					// If There's No Text
        return;							// Do Nothing

    va_start(ap, msg);					// Parses The String For Variables
    vsprintf(text, msg, ap);			// And Converts Symbols To Actual Numbers
    va_end(ap);

    death_message = text;
}


