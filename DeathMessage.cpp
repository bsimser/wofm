// DeathMessage.cpp: implementation of the DeathMessage class.
//
//////////////////////////////////////////////////////////////////////

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

DeathMessage::~DeathMessage()
{

}

int DeathMessage::ShowDeath(int success)
{

    bool has_curse = false;

    ITEMLIST::iterator it;

    int print_lines = 0;

    for (it = WorldBuilder::monsterManager.Player()->inventory.begin(); it != WorldBuilder::monsterManager.Player()->inventory.end(); it++)
    {

        //if(it->itemNumber[0] >0 &&it->itemNumber[1] >0 && it->itemNumber[2] >0)
        //{
        //	death_message = "later died of the Curse of Zagor. ";
        //}
        if (it->type == gold)
            done.gold = 1;

    }

    WorldBuilder::textManager.ClearDisplayLines();

    if (success)
    {
        WorldBuilder::textManager.SetDisplayLine(0, "Escaped");
        WorldBuilder::textManager.SetDisplayLine(1, "=======");
    }
    else
    {
        WorldBuilder::textManager.SetDisplayLine(0, "A great adventurer has fallen");
        WorldBuilder::textManager.SetDisplayLine(1, "=============================");
    }
    WorldBuilder::textManager.SetDisplayLine(2, "");

    WorldBuilder::textManager.SetDisplayLine(3, "%s the %s %s", WorldBuilder::monsterManager.Player()->Name().c_str(), WorldBuilder::start.pClass.c_str(), death_message.c_str());

    WorldBuilder::textManager.SetDisplayLine(4, "He survived for %d turns.", WorldBuilder::GetTurns());

    WorldBuilder::textManager.SetDisplayLine(5, "He achieved level %d with %d experience.", WorldBuilder::monsterManager.Player()->experience_level,
        WorldBuilder::monsterManager.Player()->experience);
    WorldBuilder::textManager.SetDisplayLine(6, "He killed %d monsters out of %d.", WorldBuilder::monsterManager.Player()->Kills(),
        WorldBuilder::monsterManager.GetNumMonsters() - 1);

    WorldBuilder::textManager.SetDisplayLine(8, "Skill: %d, Stamina: %d, Luck: %d ", WorldBuilder::monsterManager.Player()->monster.skill, WorldBuilder::monsterManager.Player()->monster.MaxStamina(), WorldBuilder::monsterManager.Player()->monster.luck);

    print_lines = 9;


    if (success)
    {
        if (done.gold)
            WorldBuilder::textManager.SetDisplayLine(10, "%s retires rich and famous. What a hero!", WorldBuilder::monsterManager.Player()->Name().c_str());
        else if (done.zagor)
            WorldBuilder::textManager.SetDisplayLine(10, "%s leaves the mountain and the treasure behind.", WorldBuilder::monsterManager.Player()->Name().c_str());
        else
            WorldBuilder::textManager.SetDisplayLine(10, "%s leaves and never to looks back. Rather pointless really!", WorldBuilder::monsterManager.Player()->Name().c_str());
        print_lines = 11;
    }

    else
    {
        if (!done.zagor)
            WorldBuilder::textManager.SetDisplayLine(10, "As darkness surrounds %s, the last sound he hears is laughter. ", WorldBuilder::monsterManager.Player()->Name().c_str());
        else
            WorldBuilder::textManager.SetDisplayLine(10, "So much promise, so many enemies. ", WorldBuilder::monsterManager.Player()->Name().c_str());
        print_lines = 11;
    }

    int i = print_lines + 1;
    if (done.entered)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "He entered Firetop Mountain.");
        i++;
    }
    if (done.barracks)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "He found the orcish barracks.");
        i++;
    }
    if (done.river)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "He crossed a large underground river.");
        i++;
    }
    if (done.undead)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "He passed through an underground cemetery.");
        i++;
    }
    if (done.maze)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "He discovered the maze of Zagor.");
        i++;
    }
    if (done.zagor)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "He defeated Zagor the evil Warlock.");
        i++;
    }
    if (done.gold)
    {
        WorldBuilder::textManager.SetDisplayLine(i, "He found Zagors Gold!");
        i++;
    }
    //equipment
    i++;
    WorldBuilder::textManager.SetDisplayLine(i++, "Equipment");
    ITEMLIST::iterator inv;
    i++;
    for (inv = WorldBuilder::monsterManager.Player()->inventory.begin(); inv != WorldBuilder::monsterManager.Player()->inventory.end(); inv++, i++)
    {
        if (inv->equipped)
        {
            WorldBuilder::textManager.SetDisplayLine(i, inv->GetName());
        }
        else i--;
    }
    i++;
    /*WorldBuilder::textManager.SetDisplayLine(i++, "Inventory");

    for (inv = WorldBuilder::monsterManager.Player()->inventory.begin(); inv != WorldBuilder::monsterManager.Player()->inventory.end(); inv++, i++)
    {
        if (!inv->equipped)
        {
            WorldBuilder::textManager.SetDisplayLine(i, inv->GetName());
        }
        else i--;
    }*/

    WorldBuilder::textManager.SetDisplayLine(++i, "Messages");
    STRINGLIST::reverse_iterator rStrIt;
    i += 6; //show five messages
    int numMesStr = 3;
    for (rStrIt = WorldBuilder::textManager.messageList.rbegin(); rStrIt != WorldBuilder::textManager.messageList.rend(); rStrIt++, i--)
    {
        WorldBuilder::textManager.SetDisplayLine(i, (char *)(*rStrIt).c_str());
        if (numMesStr++ == 7)
            break;
    }
    i += 5;

    WorldBuilder::textManager.SetDisplayLine(35, "Output to %s.txt", WorldBuilder::monsterManager.Player()->monster.name.c_str());

    WorldBuilder::textManager.SetDisplayLine(39,/*[i] see inventory,*/"[x] to quit");

    WorldBuilder::textManager.PrintDisplay(WorldBuilder::monsterManager.Player()->monster.name.c_str(), i);
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


