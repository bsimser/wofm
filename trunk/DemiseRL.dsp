# Microsoft Developer Studio Project File - Name="DemiseRL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=DemiseRL - Win32 Debug_non
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DemiseRL.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DemiseRL.mak" CFG="DemiseRL - Win32 Debug_non"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DemiseRL - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "DemiseRL - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "DemiseRL - Win32 Debug_non" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DemiseRL - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX- /O2 /I "freetype/include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /U "NO_MONSTERS" /U "NO_ITEMS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "NDEBUG"
# ADD RSC /l 0x1009 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 libfreetype.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /libpath:"freetype\lib"

!ELSEIF  "$(CFG)" == "DemiseRL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "freetype/include" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_DEBUG" /U "NO_MONSTERS" /U "NO_ITEMS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 libfreetype.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"freetype\lib"

!ELSEIF  "$(CFG)" == "DemiseRL - Win32 Debug_non"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "DemiseRL___Win32_Debug_non"
# PROP BASE Intermediate_Dir "DemiseRL___Win32_Debug_non"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "DemiseRL___Win32_Debug_non"
# PROP Intermediate_Dir "DemiseRL___Win32_Debug_non"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /I "freetype/include" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "_DEBUG" /U "NO_MONSTERS" /U "NO_ITEMS" /FR /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "freetype/include" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /U "NO_MONSTERS" /U "NO_ITEMS" /U "_DEBUG" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x1009 /d "_DEBUG"
# ADD RSC /l 0x1009 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 libfreetype.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"freetype\lib"
# ADD LINK32 libfreetype.lib opengl32.lib glu32.lib glaux.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"freetype\lib"

!ENDIF 

# Begin Target

# Name "DemiseRL - Win32 Release"
# Name "DemiseRL - Win32 Debug"
# Name "DemiseRL - Win32 Debug_non"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Graphics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\FreeTypeFont.cpp

!IF  "$(CFG)" == "DemiseRL - Win32 Release"

!ELSEIF  "$(CFG)" == "DemiseRL - Win32 Debug"

# ADD CPP /I "freetype\include"

!ELSEIF  "$(CFG)" == "DemiseRL - Win32 Debug_non"

# ADD BASE CPP /I "freetype\include"
# ADD CPP /I "freetype\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\FreeTypeFont.h
# End Source File
# Begin Source File

SOURCE=.\OpenGLSceneGen.cpp
# End Source File
# Begin Source File

SOURCE=.\OpenGLSceneGen.h
# End Source File
# Begin Source File

SOURCE=.\SceneGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneGenerator.h
# End Source File
# End Group
# Begin Group "Dungeon Map"

# PROP Default_Filter ""
# Begin Group "Generator"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DungeonGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\DungeonGenerator.h
# End Source File
# Begin Source File

SOURCE=.\SpecialLevelGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecialLevelGenerator.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DungeonLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\DungeonLevel.h
# End Source File
# Begin Source File

SOURCE=.\DungeonManager.cpp
# End Source File
# Begin Source File

SOURCE=.\DungeonManager.h
# End Source File
# Begin Source File

SOURCE=.\Terrain.cpp
# End Source File
# Begin Source File

SOURCE=.\Terrain.h
# End Source File
# End Group
# Begin Group "Misc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\coord.h
# End Source File
# Begin Source File

SOURCE=.\NumberGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\NumberGenerator.h
# End Source File
# End Group
# Begin Group "Monsters"

# PROP Default_Filter ""
# Begin Group "Types"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Monster.cpp
# End Source File
# Begin Source File

SOURCE=.\Monster.h
# End Source File
# Begin Source File

SOURCE=.\RandomMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\RandomMonster.h
# End Source File
# Begin Source File

SOURCE=.\SpecialGuardians.cpp
# End Source File
# Begin Source File

SOURCE=.\SpecialGuardians.h
# End Source File
# Begin Source File

SOURCE=.\UndeadMonster.cpp
# End Source File
# Begin Source File

SOURCE=.\UndeadMonster.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MonsterAI.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterAI.h
# End Source File
# Begin Source File

SOURCE=.\MonsterManager.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterManager.h
# End Source File
# End Group
# Begin Group "Items"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Item.cpp
# End Source File
# Begin Source File

SOURCE=.\Item.h
# End Source File
# Begin Source File

SOURCE=.\ItemManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ItemManager.h
# End Source File
# Begin Source File

SOURCE=.\MonsterItems.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterItems.h
# End Source File
# End Group
# Begin Group "WorldManager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DemiseRLmain.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldBuilder.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldBuilder.h
# End Source File
# End Group
# Begin Group "Actions or Commands"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Action.cpp
# End Source File
# Begin Source File

SOURCE=.\Action.h
# End Source File
# Begin Source File

SOURCE=.\ActionManager.cpp
# End Source File
# Begin Source File

SOURCE=.\ActionManager.h
# End Source File
# Begin Source File

SOURCE=.\LevelChange.cpp
# End Source File
# Begin Source File

SOURCE=.\LevelChange.h
# End Source File
# Begin Source File

SOURCE=.\StandardMonsterActions.cpp
# End Source File
# Begin Source File

SOURCE=.\StandardMonsterActions.h
# End Source File
# Begin Source File

SOURCE=.\UserCommand.cpp
# End Source File
# Begin Source File

SOURCE=.\UserCommand.h
# End Source File
# End Group
# Begin Group "Screens"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DeathMessage.cpp
# End Source File
# Begin Source File

SOURCE=.\DeathMessage.h
# End Source File
# Begin Source File

SOURCE=.\InventoryManager.cpp
# End Source File
# Begin Source File

SOURCE=.\InventoryManager.h
# End Source File
# Begin Source File

SOURCE=.\MonsterInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\MonsterInfo.h
# End Source File
# Begin Source File

SOURCE=.\StartScreen.cpp
# End Source File
# Begin Source File

SOURCE=.\StartScreen.h
# End Source File
# Begin Source File

SOURCE=.\TextManager.cpp
# End Source File
# Begin Source File

SOURCE=.\TextManager.h
# End Source File
# End Group
# Begin Group "LOS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\LOS\Fraction.cpp
# End Source File
# Begin Source File

SOURCE=.\LOS\Fraction.h
# End Source File
# Begin Source File

SOURCE=.\LOS\LineOfSight.cpp
# End Source File
# Begin Source File

SOURCE=.\LOS\LineOfSight.h
# End Source File
# End Group
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\TaskList.txt
# End Source File
# End Target
# End Project
