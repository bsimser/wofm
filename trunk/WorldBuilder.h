// WorldBuilder.h: interface for the WorldBuilder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_WORLDBUILDER_H__685DBA95_20E8_4392_B7C8_CEF79C98C238__INCLUDED_)
#define AFX_WORLDBUILDER_H__685DBA95_20E8_4392_B7C8_CEF79C98C238__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>		// Header File For Windows

#include "OpenGLSceneGen.h" 

#include "DungeonManager.h" //manage and create dungeons
#include "MonsterManager.h" //manage and create monsters
#include "ActionManager.h"	//manage and create actions
#include "TextManager.h"	//manage player messages
#include "ItemManager.h"	//manager and create items
#include "InventoryManager.h" //manage user commands from inventory
#include "DeathMessage.h"
#include "StartScreen.h"
#include ".\spellmanager.h" //cast spells
#include "RestLevel.h"
#include "NumberGenerator.h"

#define VC_G	 0x47
#define VC_I	 0x49
#define VC_COMMA 0xBC
#define VC_X	 88
#define VC_U     85
#define VK_ESC	 27


#define VK_A	 65
#define VK_B	 66
#define VK_C	 67
#define VK_D	 68
#define VK_E	 69
#define VK_F	 70
#define VK_G	 71
#define VK_H	 72
#define VK_I	 73
#define VK_J	 74
#define VK_K	 75
#define VK_L	 76
#define VK_M	 77
#define VK_N	 78
#define VK_O	 79
#define VK_P	 80
#define VK_Q	 81
#define VK_R	 82
#define VK_S	 83
#define VK_T	 84
#define VK_U	 85
#define VK_V	 86
#define VK_W	 87
#define VK_X	 88
#define VK_Y	 89
#define VK_Z	 90

#define VK_1	 49
#define VK_2	 50
#define VK_3	 51
#define VK_4	 52
#define VK_5	 53
#define VK_6	 54
#define VK_7	 55
#define VK_8	 56
#define VK_9	 57
#define VK_0	 4


#define VK_FULLSTOP 0xBE
#define VK_QUESTION 0xBF

#define CANCELED	  0
#define NORMAL		  1
#define SELECT_TARGET 2
#define INVALID_INPUT 3

enum eDisplayState
{
    sStart,
    sNormal,
    sInventory,
    sDeath,
    sLooking,
    sLookMore,
    sClosing,
    sRunning,
    sHelpScreen,
    sMore,
    sMessages,
    sUse,
    sLeave,
    sMap,
    sThrow,
    sSelectSpell,
    sTargetSpell,
    sRest,
    sResting,
    sFlee,
};

class WorldBuilder
{
public:
    WorldBuilder();
    virtual ~WorldBuilder();

    int Run();
    int Initialise(const char* title);
    int Stop();

    int ToggleFullScreen(int width, int height);

    void Resize(WPARAM lParam, LPARAM wParam);

    void ProcessCommand(bool *keys);
    int CompleteUserCommands();

    static int GetCurrentLevel(){ return current_level; };
    static int SetDungeonLevel(int new_lev){ current_level = new_lev; return current_level; };

    //int BuildDungeon(int level);

    static DungeonManager dungeonManager;
    static ActionManager actionManager;
    static TextManager textManager;
    static ItemManager itemManager;
    static MonsterManager monsterManager;
    static InventoryManager inventoryManager;
    static SpellManager spellManager;

    static DeathMessage deathMessage;
    static StartScreen start;
    static RestLevel restLevel;

    static int UpLevel(){ current_level--; return current_level; };
    static int DownLevel(){ current_level++; return current_level; };

    static int UpSpecialLevel(){ current_level -= 10; return current_level; };
    static int DownSpecialLevel(){ current_level += 10; return current_level; };
    static int DownEncounterLevel(){ current_level = 20; return current_level; };

    static int GetMaxLevels(){ return max_num_levels; };

    static int GetTurns(){ return turns; };
    static void SetState(eDisplayState st) { state = st; };
    static 	eDisplayState State(){ return state; };

private:
    OpenGLSceneGen scene;

    static 	eDisplayState state;
    static int current_level;

    void UpdateMap();
    void InventoryCommand(bool *keys);
    static long turns;
    static int max_num_levels;

    int old_turns;

    std::string Title;

    bool first_update;
    coord map_coord;
};

#endif // !defined(AFX_WORLDBUILDER_H__685DBA95_20E8_4392_B7C8_CEF79C98C238__INCLUDED_)