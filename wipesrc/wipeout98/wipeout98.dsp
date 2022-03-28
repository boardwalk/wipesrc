# Microsoft Developer Studio Project File - Name="Wipeout98" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Wipeout98 - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Wipeout98.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wipeout98.mak" CFG="Wipeout98 - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Wipeout98 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Wipeout98 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /Zi /Ox /Ot /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D USE_ATI=1 /D "WIPEOUTPC" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ddraw.lib dxguid.lib dsound.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D USE_ATI=1 /D WIPEOUTPC=1 /YX /FD /c
# SUBTRACT CPP /X
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 dsound.lib ddraw.lib dxguid.lib winmm.lib vfw32.lib kernel32.lib user32.lib gdi32.lib winspool.lib winmm.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Wipeout98 - Win32 Release"
# Name "Wipeout98 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Ajs.cpp
# End Source File
# Begin Source File

SOURCE=.\Andyscn.cpp
# End Source File
# Begin Source File

SOURCE=.\Ayrace.cpp
# End Source File
# Begin Source File

SOURCE=.\CAMERA.Cpp
# End Source File
# Begin Source File

SOURCE=.\Card.cpp
# End Source File
# Begin Source File

SOURCE=.\Clearot.cpp
# End Source File
# Begin Source File

SOURCE=.\Collide.cpp
# End Source File
# Begin Source File

SOURCE=.\Combat.cpp
# End Source File
# Begin Source File

SOURCE=.\CONE.Cpp
# End Source File
# Begin Source File

SOURCE=.\Config.cpp
# End Source File
# Begin Source File

SOURCE=.\Control.cpp
# End Source File
# Begin Source File

SOURCE=.\Credits.cpp
# End Source File
# Begin Source File

SOURCE=.\Datacash.cpp
# End Source File
# Begin Source File

SOURCE=.\Draw.cpp
# End Source File
# Begin Source File

SOURCE=.\Drawot.cpp
# End Source File
# Begin Source File

SOURCE=.\Droid.cpp
# End Source File
# Begin Source File

SOURCE=.\Dynam.cpp
# End Source File
# Begin Source File

SOURCE=.\Effects.cpp
# End Source File
# Begin Source File

SOURCE=.\Error.cpp
# End Source File
# Begin Source File

SOURCE=.\Etcvars.cpp
# End Source File
# Begin Source File

SOURCE=.\File.cpp
# End Source File
# Begin Source File

SOURCE=.\fromasm.cpp
# End Source File
# Begin Source File

SOURCE=.\Getvram.cpp
# End Source File
# Begin Source File

SOURCE=.\GLOBAL.Cpp
# End Source File
# Begin Source File

SOURCE=.\Grabber.cpp
# End Source File
# Begin Source File

SOURCE=.\HISCORES.Cpp
# End Source File
# Begin Source File

SOURCE=.\HUD.Cpp
# End Source File
# Begin Source File

SOURCE=.\INIT.Cpp
# End Source File
# Begin Source File

SOURCE=.\Initpad.cpp
# End Source File
# Begin Source File

SOURCE=.\Libgte.cpp
# End Source File
# Begin Source File

SOURCE=.\Loadall.cpp
# End Source File
# Begin Source File

SOURCE=.\LZSS.Cpp
# End Source File
# Begin Source File

SOURCE=.\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\Malloc.cpp
# End Source File
# Begin Source File

SOURCE=.\Menus.cpp
# End Source File
# Begin Source File

SOURCE=.\newgraphics.cpp
# End Source File
# Begin Source File

SOURCE=.\Object.cpp
# End Source File
# Begin Source File

SOURCE=.\OPTIONS.Cpp
# End Source File
# Begin Source File

SOURCE=.\Pad.cpp
# End Source File
# Begin Source File

SOURCE=.\Pallette.cpp
# End Source File
# Begin Source File

SOURCE=.\Pdenv.cpp
# End Source File
# Begin Source File

SOURCE=.\Pmkey.cpp
# End Source File
# Begin Source File

SOURCE=.\Racectrl.cpp
# End Source File
# Begin Source File

SOURCE=.\Radscn.cpp
# End Source File
# Begin Source File

SOURCE=.\RADTRK.Cpp
# End Source File
# Begin Source File

SOURCE=.\regread.cpp
# End Source File
# Begin Source File

SOURCE=.\SCENE.Cpp
# End Source File
# Begin Source File

SOURCE=.\SEARCH.Cpp
# End Source File
# Begin Source File

SOURCE=.\SHIPS.Cpp
# End Source File
# Begin Source File

SOURCE=.\SKELETON.Cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Spline.cpp
# End Source File
# Begin Source File

SOURCE=.\Startpad.cpp
# End Source File
# Begin Source File

SOURCE=.\Stoppad.cpp
# End Source File
# Begin Source File

SOURCE=.\TEXPLACE.Cpp
# End Source File
# Begin Source File

SOURCE=.\textures.cpp
# End Source File
# Begin Source File

SOURCE=.\TIM.Cpp
# End Source File
# Begin Source File

SOURCE=.\Timc.cpp
# End Source File
# Begin Source File

SOURCE=.\TRACK.Cpp
# End Source File
# Begin Source File

SOURCE=.\trackasm.cpp
# End Source File
# Begin Source File

SOURCE=.\TTF.Cpp
# End Source File
# Begin Source File

SOURCE=.\UTILS.Cpp
# End Source File
# Begin Source File

SOURCE=.\Wave.cpp
# End Source File
# Begin Source File

SOURCE=.\Weapon.cpp
# End Source File
# Begin Source File

SOURCE=.\Win.cpp
# End Source File
# Begin Source File

SOURCE=.\Wincd.cpp
# End Source File
# Begin Source File

SOURCE=.\Winsnd.cpp
# End Source File
# Begin Source File

SOURCE=.\Wintime.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\ajs.h
# End Source File
# Begin Source File

SOURCE=.\andyscn.h
# End Source File
# Begin Source File

SOURCE=.\ayrace.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\credits.h
# End Source File
# Begin Source File

SOURCE=.\datacash.h
# End Source File
# Begin Source File

SOURCE=.\drawot.h
# End Source File
# Begin Source File

SOURCE=.\Effects.h
# End Source File
# Begin Source File

SOURCE=.\error.h
# End Source File
# Begin Source File

SOURCE=.\libgte.h
# End Source File
# Begin Source File

SOURCE=.\lzss.h
# End Source File
# Begin Source File

SOURCE=.\Object.h
# End Source File
# Begin Source File

SOURCE=.\protos.h
# End Source File
# Begin Source File

SOURCE=.\racectrl.h
# End Source File
# Begin Source File

SOURCE=.\regread.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\timc.h
# End Source File
# Begin Source File

SOURCE=.\Trackasm.h
# End Source File
# Begin Source File

SOURCE=.\utils.h
# End Source File
# Begin Source File

SOURCE=.\win.h
# End Source File
# Begin Source File

SOURCE=.\Wincd.h
# End Source File
# Begin Source File

SOURCE=.\winclip.h
# End Source File
# Begin Source File

SOURCE=.\winh.h
# End Source File
# Begin Source File

SOURCE=.\winsnd.h
# End Source File
# Begin Source File

SOURCE=.\wintime.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Wipeout.rc
# End Source File
# End Group
# End Target
# End Project
