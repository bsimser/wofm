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
#include "UserCommand.h"
#include "DeathMessage.h"
#include "MagicScreen.h"
#include "MonsterAI.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

WorldBuilder::WorldBuilder() :
first_update(true),
Title("Warlock of Firetop Mountain RL (c) 2014 Corremn")
{
    max_num_levels = MAX_DUNGEON_LEVELS;
    turns = 0;
}

int WorldBuilder::Initialise(const char* title)
{
    static int test = 1;
    SetState(sStart);

    Title = title;

    // Ask The User Which Screen Mode They Prefer
    //	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
    {
        scene.FullScreen(false);							// Windowed Mode
    }

    try
    {
        // Create Our OpenGL Window
        if (test)
        {
            if (!scene.CreateGLWindow((char *)title, 800, 600, 32, scene.FullScreen()))
            {
                throw std::exception("OpenGL failure");
                // Quit If Window Was Not Created
            }
            test = 0;
        }
        //display is created now lets create a dungeon
        if (!dungeonManager.Initialise())
        {
            dungeonManager.PrintDungeons();
            return 0;
        }
        if (!spellManager.Initialise())
            return 0;

        if (!monsterManager.Initialise())
            return 0;

        if (!itemManager.Initialise())
            return 0;

        bool sucess = false;

        dungeonManager.BuildCompleteDungeon(max_num_levels); //creates and populates the dungeon

        //////////////////

        itemManager.PrintItems();
        spellManager.PrintSpells();

        //initial sight
        monsterManager.Player()->UpdateSightRange();

        //intialias state
        SetState(sStart);

        //display character generation
        start.Display();

        //add effects to world
        scene.AddShadows(true);
        scene.ShadowStrength(0.5f);
        dungeonManager.PrintDungeons();
        monsterManager.PrintMonsters();
    }
    catch (const std::exception & ex)
    {
        MessageBox(NULL, ex.what(), "Exception", MB_ICONEXCLAMATION);
        dungeonManager.PrintDungeons();
        return 0;
    }
    catch (...)
    {
        MessageBox(NULL, "Unknown error encounted initialising scene", "Exception", MB_ICONEXCLAMATION);
        return 0;
    }

    return 1;
}

// from this function 
int WorldBuilder::Run()
{
    CompleteUserCommands();//do any pending commands I.e run

    try
    {
        //depending on state either show the map or show text.

        if (State() == sMessages || State() == sInventory || State() == sHelpScreen ||
            State() == sDeath || State() == sStart || State() == sLookMore || State() == sSelectSpell ||
            State() == sRest || State() == sResting)//	 || State() == sLeave))
            scene.DisplayMap(false);
        else

            scene.DisplayMap(true);

        UpdateMap(); //update scene (items,map,npc,pc etc) ??

        RenderScene();

        if (monsterManager.Player()->miss_turn == 1)
        {
            turns++;
            monsterManager.Player()->miss_turn = 0;;
        }
    }
    catch (const std::exception & ex)
    {
        MessageBox(NULL, ex.what(), "Exception", MB_ICONEXCLAMATION);
        SetState(sNormal);
        //World.getTextManager().newLine((char*)ex.what());
    }
    catch (...)
    {
        MessageBox(NULL, "Unknown error encounted updating scene", "Exception", MB_ICONEXCLAMATION);
        SetState(sNormal);
    }
    return 1;
}

int WorldBuilder::CompleteUserCommands()
{
    //automatic commands
    if (State() == sRunning)
    {
        UserCommand command;
        int ret = command.Run();
        if (ret == 0)
            state = sNormal;
        else if (ret == 1)
            turns++;
        else if (ret == 2)
        {//do nothing
        }
        monsterManager.Player()->UpdateSightRange();
        //		Sleep(1);
    }
    else if (State() == sResting)
    {
        int ret = restLevel.Resting(GetCurrentLevel());
        if (ret == 0)
        {
            monsterManager.Player()->UpdateSightRange();
            state = sNormal;
        }
    }

    return 1;
}

int WorldBuilder::Stop()
{
    dungeonManager.DeleteDungeon(0);

    scene.KillGLWindow();									// Kill The Window
    return 1;
}

///////////////// DISPLAY SPECIFIC //////////////////////////////
int WorldBuilder::ToggleFullScreen(int width, int height)
{
    scene.KillGLWindow();							// Kill Our Current Window
    scene.FullScreen(!scene.FullScreen());			// Toggle Fullscreen / Windowed Mode

    // Recreate Our OpenGL Window ( Modified )
    if (!scene.CreateGLWindow((char *)Title.c_str(), width, height, 32, scene.FullScreen()))
    {
        return 0;			// Quit If Window Was Not Created
    }

    return 1;
}

void WorldBuilder::Resize(WPARAM lParam, LPARAM wParam)
{
    scene.ReSizeGLScene(LOWORD(lParam), HIWORD(lParam));

    if (!first_update)
    {
        Run();
        RenderScene();
    }
}

//////////////////////////////////////////////////////////////

void WorldBuilder::UpdateMap()
{
    if ((old_turns < turns || first_update) && (state != sStart || state != sRest || state != sResting))
    {
        monsterManager.UpdateMonsters(&dungeonManager.level(GetCurrentLevel()), &actionManager);
        old_turns = turns;
        first_update = false;
    }
    old_turns = turns;

    scene.dLevel = &dungeonManager.level(World.GetCurrentLevel());
}

void WorldBuilder::UpdateStatusBar()
{
    //add text lines
    char line1[128];
    MONSTERLIST::iterator player = monsterManager.getMonsterList().begin();

    char name[32];
    sprintf(name, "%s  Level %d", player->monster.name.c_str(), player->experience_level);

    sprintf(line1, "%-20s  Stamina: %2d/%d  Skill: %2d(%+d)  Luck: %-2d(%+d)  Dungeon Level: %s", name,

        //sprintf(line1, "%-20s  Skill: %2d(%+d)  Stamina: %2d/%d  Luck: %-2d(%+d)   Dungeon Level: %s", name,

        //sprintf(line1,"%s Level %d Skill: %2d(%d)  Stamina: %2d     Turns: %4d  DL: %d XP: %d Mode %d",
        //sprintf(line1, "%-20s  Skill: %2d(%d)  Stamina: %2d(%d)  Luck: %-2d(%d)   Dungeon Level:%d  Turns: %d", name,
        //sprintf(line1,"%-20s  Skill: %2d(%d) Stamina: %2d(%d) Luck: %-2d  X:%d Y:%d" ,name,

        //"",player->monster.name,
        //player->experience_level,
        player->Stamina(), player->monster.MaxStamina(),
        player->AdjustedSkill(),
        player->AdjustedSkill() - player->Skill(),
        player->Luck(),
        player->luck_penalty,
        //player->pos.x,
        //player->pos.y);
        World.getDungeonManager().getLevelName(GetCurrentLevel()).c_str());
    //turns);

    //GetCurrentLevel(),		
    //player->experience,
    //State());

    textManager.display_line2 = textManager.display_line;

    if (state == sStart || state == sDeath)
        textManager.display_line1 = "";
    else
        textManager.display_line1 = line1;
}

void WorldBuilder::RenderScene()
{
    if (State() != sMap)
        scene.CreateOffset(monsterManager.Player()->pos);
    else
        scene.CreateOffset(map_Coord);

    UpdateStatusBar();

    scene.DrawGLScene();					// Draw The Scene
    scene.SwapBuffers();				// Swap Buffers (Double Buffering)
}

void WorldBuilder::ProcessCommand(bool *keys)
{
    int i = 0;
    UserCommand command;

    int dir;

    MonsterData *player = monsterManager.Player();

    if (!keys[VK_SPACE] && State() != sMore)
    {
        textManager.clear();
        keys[VK_SPACE] = false;
    }

    /*	if(player->monster.stamina<=0 && state!=sDeath) //dont do anything, your dead
        {
        textManager.MoreStep();
        textManager.MoreStep();
        turns++;
        }*/

    //else if(monsterManager.Player()->miss_turn==1)
    //{
    //	turns++;
    //monsterManager.Player()->miss_turn = 1;
    //}
    //start processeing commands

    if (State() == sInventory)
    {
        int ret = inventoryManager.InventoryCommand(keys);
        if (ret == 0) //exit inventory
            SetState(sNormal);
        else if (ret == 3)  //action that took a turn and exit inventory
        {
            turns++;
            SetState(sNormal);
        }
        else if (ret == 2)  //action that took a turn I.e drop
            turns++;
        //return;
    }
    else if (State() == sLeave)
    {
        if (keys[VK_Y])
        {
            World.getDeathMessage().SetDeathMessage("left the great mountain. ");
            World.getDeathMessage().ShowDeath(1);
            Sleep(500);
            World.SetState(sDeath);
        }
        else SetState(sNormal);
    }
    else if (State() == sMore)
    {
        //if(keys[VK_SPACE]) 
        {
            textManager.MoreStep();
        }
    }
    else if (State() == sRest)
    {
        if (keys[VK_X] || keys[VK_ESCAPE])
        {
            SetState(sNormal);
            keys[VK_X] = false;
            keys[VK_ESCAPE] = false;
        }
        if (keys[VK_R])
        {
            keys[VK_R] = false;
            restLevel.ResetRest();
            state = sResting;
        }
    }
    else if (State() == sMessages)
    {
        if (keys[VK_X] || keys[VK_ESCAPE])
        {
            SetState(sNormal);
            keys[VK_ESCAPE] = false;
            keys[VK_X] = false;
        }
    }
    else if (State() == sStart)
    {
        int ret = start.PassInput(keys);

        if (ret == 0)
        {
            SetState(sNormal);
            monsterManager.Player()->monster.name = start.pName;
            //strcpy(monsterManager.Player()->monster.class,);
            if (start.pClass == "Warlock")
            {
                player->inventory.push_back(*World.getItemManager().CreateItem(0, cards, 0));
            }
        }
    }

    else if (State() == sLookMore)
    {
        if (keys[VK_X] || keys[VK_ESCAPE])
        {
            SetState(sLooking);
            keys[VK_X] = false;
            keys[VK_ESCAPE] = false;
        }
    }
    else if (State() == sMap)
    {
        if (keys[VK_X] || keys[VK_ESCAPE])
        {
            SetState(sNormal);
            keys[VK_X] = false;
            keys[VK_ESCAPE] = false;
        }
    }

    else if (State() == sUse)
    {
        if (command.UseItem(keys)) //cancelled
        {
            turns++;
        }
        SetState(sNormal);
    }

    else if (State() == sDeath)
    {
        if (keys[VC_X])
        {
            exit(0);
            //	SetState(sNormal);
        }
    }

    else if (State() == sLooking)
    {
        if (!command.Look(keys)) //cancelled
        {
            SetState(sNormal);
        }
        else return;
    }

    else if (State() == sFire)
    {
        if (keys[VK_SPACE] || keys[VK_F]) //target acquired
        {
            if (command.ThrowTarget(aFire))
                turns++;
            SetState(sNormal);
        }
        else if (!command.Look(keys, true)) //cancelled
        {
            SetState(sNormal);
        }
        else
        {
            return;
        }
        dungeonManager.CurrentLevel()->ClearPath();
    }
    else if (State() == sThrow)
    {
        //command.FireItem();
        if (keys[VK_SPACE]) //target acquired
        {
            if (command.ThrowItem())
                turns++;
            SetState(sNormal);
        }
        else if (!command.Look(keys, true)) //cancelled
        {
            SetState(sNormal);
        }
        else
        {
            return;
        }
        dungeonManager.CurrentLevel()->ClearPath();
    }
    else if (state == sClosing)
    {
        int ret = command.Close(keys);
        if (ret == 0) //cancelled	
            state = sNormal;
        else if (ret == 1) //opened
        {
            state = sNormal;
            turns++;
        }
    }
    else if (state == sRunning)
    {
        if (!command.RunDirection(command.GetDirection(keys)))
            state = sNormal;
    }
    else if (state == sFlee)
    {
        if (command.FleeCommand(command.GetDirection(keys)))
            turns++;
        state = sNormal;
    }

    else if (state == sHelpScreen)
    {
        if (!command.HelpCommand(keys))
            state = sNormal;

    }
    else if (State() == sSelectSpell)
    {
        MagicScreen magic;
        int ret = magic.SelectSpell(keys);
        if (ret == CANCELED)
            SetState(sNormal);
        else if (ret == NORMAL)
        {
            SetState(sNormal);
            turns++;

        }
        else if (ret == SELECT_TARGET)
        {
            command.FireItem(false);
            SetState(sTargetSpell);

        }
        //else if(ret == INVALID_INPUT) do nothing
    }
    else if (State() == sTargetSpell)
    {
        if (keys[VK_SPACE] || keys[VK_A] || keys[VK_B] || keys[VK_C] || keys[VK_D]) //target acquired
        {
            if (command.CastSpellAtTarget())
                turns++;
            SetState(sNormal);
        }
        else if (!command.Look(keys, true)) //cancelled
        {
            SetState(sNormal);
        }
        else
            return; //clear all keys
        dungeonManager.CurrentLevel()->ClearPath();
    }
    else if (State() == sQuitting)
    {
        if (keys[VK_Y])
        {
            monsterManager.Player()->monster.stamina = 0;
            World.getDeathMessage().SetDeathMessage("gave up. ");
            turns++;
        }
        else
            SetState(sNormal);
    }
    //SHIFT COMMANDS FIRST
    else if (keys[VK_SHIFT])
    {
        if (keys[VC_COMMA]) //'<'
        {
            if (command.ClimbStairs(dUp))
            {
                //int l = GetCurrentLevel();
                //if (l != 1 && l != 4 && l != 7 && l != 10)
                //   turns++; //monsters get first attack except coming from rest level
            }
            keys[VC_COMMA] = false;
            keys[VK_SHIFT] = false;
        }
        else if (keys[VK_FULLSTOP]) //'>'
        {
            if (command.ClimbStairs(dDown))
                turns++;
            keys[VK_FULLSTOP] = false;
            keys[VK_SHIFT] = false;
        }
        else if (keys[VK_QUESTION])
        {
            command.DisplayHelp();
            state = sHelpScreen;
            keys[VK_QUESTION] = false;
            keys[VK_SHIFT] = false;
        }
        else if (keys[VK_F])
        {
            if (command.Flee())
                state = sFlee;
            keys[VK_F] = false;
        }
        else if (keys[VK_Q])
        {
            state = sQuitting;

            keys[VK_Q] = false;
            textManager.newLine("Really quit? Press [y] to quit. ");
        }
    }

    //END SHIFT

    //CTRL COMMANDS 
    else if (keys[VK_CONTROL])
    {
        if (keys[VK_P]) //'<'
        {
            state = sMessages;
            textManager.ShowMessages();
            keys[VK_P] = false;
            keys[VK_CONTROL] = false;
        }
    }
    //END CTRL

    else if (keys[VK_FULLSTOP])
    {
        SetState(sRunning);
        command.InitRun();

        keys[VK_FULLSTOP] = false;
    }
    else if (keys[VK_L]) //look
    {
        command.Look();
        SetState(sLooking);
        keys[VK_L] = false;

    }
    else if ((dir = command.GetDirection(keys)) != dNone)
    {
        if (State() != sNormal)
            return;

        if (command.MoveCommand(dir))
            turns++;
    }

    else if (keys[VK_U])
    {
        command.UseItem();
        state = sUse;
        keys[VK_U] = false;
    }
    else if (keys[VK_F])
    {
        if (command.FireItem())
            state = sFire;
        keys[VK_F] = false;
    }
    else if (keys[VK_D])
    {
        SetState(sInventory);

        inventoryManager.ShowInventory(0);
        inventoryManager.setState(InventoryManager::eDrop);
        inventoryManager.DropMessage();

        keys[VK_D] = false;
    }
    /*else if(keys[VK_D] || keys[VK_Q])
    {
    if(command.Drink())
    state = sFire;
    keys[VK_D]=keys[VK_Q]=false;
    }*/
    else if (keys[VK_C])
    {
        state = sClosing;
        keys[VK_C] = false;
        World.getTextManager().newLine("Close what?[dir] - [x] to cancel. ");
    }

    else if (keys[VC_COMMA] || keys[VC_G]) //pickup
    {
        if (dungeonManager.level(World.GetCurrentLevel()).getCell(player->getPosition()->x, player->getPosition()->y).getItem())
        {
            player->NextAction(actionManager.UpdateAction(&player->action, aPickup, 0));
            turns++;
        }
        keys[VC_COMMA] = false;
        keys[VC_G] = false;
    }

    else if (keys[VK_I]) //inventory look
    {
        monsterManager.Player()->Items();

        inventoryManager.ShowInventory(0);

        SetState(sInventory);
        keys[VK_I] = false;
    }
    else if (keys[VK_E]) //inventory look
    {
        monsterManager.Player()->Items();

        inventoryManager.ShowInventory(1);

        SetState(sInventory);
        keys[VK_E] = false;
    }
    else if (keys[VK_Z]) //no spell cast
    {
        if (World.getMonsterManager().getMonsterItems().GetInventoryItem(player, cards) != NULL)
        {
            MagicScreen spell;
            spell.Display();

            SetState(sSelectSpell);
        }
        else
            textManager.newLine("You have nothing that allows you to cast spells. ");
        keys[VK_Z] = false;
    }
#ifdef _DEBUG //god commands - debug only

    else if (keys[VK_1] || keys[VK_2]) //debug keys
    {
        command.Debug(keys);
        keys[VK_1] = false;
        keys[VK_2] = false;
    }

    else if (keys[VK_S])
    {
        static bool showall = false;
        showall = !showall;
        scene.ShowAll(showall);
        World.getTextManager().newLine("God view toggled. ");
        keys[VK_S] = false;
    }
    else if (keys[VK_3])
    {
        player->monster.stamina = player->monster.MaxStamina();
    }

    else if (keys[VK_T]) //transfer soul
    {
        monsterManager.Player()->monster.stamina = 0;
        World.getDeathMessage().SetDeathMessage("commited suicide because of a hidden suicide key, sorry. ");
        turns++;
        keys[VK_T] = false;
    }
#endif

    if (State() != sStart)
    {
        monsterManager.Player()->UpdateSightRange();
        monsterManager.Player()->Heal();
    }

}

int WorldBuilder::GetCurrentLevel()
{ 
    return current_level;
}

int WorldBuilder::SetDungeonLevel(int new_lev)
{
    current_level = new_lev; 
    return current_level; 
}

//int BuildDungeon(int level);

DungeonManager & WorldBuilder::getDungeonManager() 
{ 
    return dungeonManager; 
}

ActionManager & WorldBuilder::getActionManager()
{ 
    return actionManager;
}

TextManager & WorldBuilder::getTextManager()
{ 
    return textManager;
}

ItemManager & WorldBuilder::getItemManager()
{
    return itemManager;
}

MonsterManager & WorldBuilder::getMonsterManager()
{ 
    return monsterManager; 
}

InventoryManager & WorldBuilder::getInventoryManager()
{ 
    return inventoryManager; 
}

SpellManager & WorldBuilder::getSpellManager()
{ 
    return spellManager; 
}

DeathMessage & WorldBuilder::getDeathMessage()
{ 
    return deathMessage; 
}

StartScreen & WorldBuilder::getStartScreen()
{ 
    return start; 
}

RestLevel & WorldBuilder::getRestLevel()
{ 
    return restLevel; 
}

int WorldBuilder::UpLevel()
{ 
    current_level--; 
    return current_level; 
}

int WorldBuilder::DownLevel()
{ 
    current_level++; 
    return current_level; 
}

int WorldBuilder::UpSpecialLevel()
{ 
    current_level -= 10; 
    return current_level; 
}
int WorldBuilder::DownSpecialLevel()
{ 
    current_level += 10; 
    return current_level; 
}
int WorldBuilder::DownEncounterLevel()
{ 
    current_level = 20; 
    return current_level; 
}

int WorldBuilder::GetMaxLevels()
{ 
    return max_num_levels; 
}

int WorldBuilder::GetTurns()
{ 
    return turns; 
}

void WorldBuilder::SetState(eDisplayState st)
{ 
    state = st; 
}

eDisplayState WorldBuilder::State()
{ 
    return state;
}

OpenGLSceneGen & WorldBuilder::getScene() 
{ 
    return scene; 
}
