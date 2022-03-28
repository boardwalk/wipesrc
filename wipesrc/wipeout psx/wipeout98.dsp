# Microsoft Developer Studio Project File - Name="Wipeout98" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 5.00
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
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_ATI" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

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
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_ATI" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Wipeout98 - Win32 Release"
# Name "Wipeout98 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\Ayrace.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Camera.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Collide.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Control.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Draw.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Dynam.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Editor.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Global.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Grabber.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Hud.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Init.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Main.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Options.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Racectrl.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Radscn.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Radtrk.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\regread.c
# End Source File
# Begin Source File

SOURCE=.\Search.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ships.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sound.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Track.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Ttf.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Utils.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Wave.c
# End Source File
# Begin Source File

SOURCE=.\Weapon.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Win.c
# End Source File
# Begin Source File

SOURCE=.\Wincd.c
# End Source File
# Begin Source File

SOURCE=.\Winclip.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"
# SUBTRACT CPP /X

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Winsnd.c
# End Source File
# Begin Source File

SOURCE=.\Wintime.c
# End Source File
# Begin Source File

SOURCE=.\Wtl.c

!IF  "$(CFG)" == "Wipeout98 - Win32 Release"

!ELSEIF  "$(CFG)" == "Wipeout98 - Win32 Debug"

# ADD CPP /I "\wipesrc\psx26\include"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\winh.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
