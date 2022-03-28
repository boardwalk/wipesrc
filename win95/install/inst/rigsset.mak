# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=RIGSSET - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to RIGSSET - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "RIGSSET - Win32 Release" && "$(CFG)" !=\
 "RIGSSET - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Rigsset.mak" CFG="RIGSSET - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RIGSSET - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RIGSSET - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "RIGSSET - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "RIGSSET - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "WinRel"
# PROP BASE Intermediate_Dir "WinRel"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "WinRel"
# PROP Intermediate_Dir "WinRel"
OUTDIR=.\WinRel
INTDIR=.\WinRel

ALL : "$(OUTDIR)\Rigsset.exe"

CLEAN : 
	-@erase ".\WinRel\Rigsset.exe"
	-@erase ".\WinRel\dinstall.obj"
	-@erase ".\WinRel\dinstall.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "f:\dxsdk\sdk\inc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "f:\dxsdk\sdk\inc" /D "WIN32" /D "NDEBUG"\
 /D "_WINDOWS" /Fp"$(INTDIR)/Rigsset.pch" /YX /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\WinRel/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/dinstall.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Rigsset.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Rigsset.pdb" /machine:I386 /out:"$(OUTDIR)/Rigsset.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dinstall.obj" \
	"$(INTDIR)/dinstall.res"

"$(OUTDIR)\Rigsset.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "RIGSSET - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "WinDebug"
# PROP BASE Intermediate_Dir "WinDebug"
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "WinDebug"
# PROP Intermediate_Dir "WinDebug"
OUTDIR=.\WinDebug
INTDIR=.\WinDebug

ALL : "$(OUTDIR)\Rigsset.exe"

CLEAN : 
	-@erase ".\WinDebug\vc40.pdb"
	-@erase ".\WinDebug\vc40.idb"
	-@erase ".\WinDebug\Rigsset.exe"
	-@erase ".\WinDebug\dinstall.obj"
	-@erase ".\WinDebug\dinstall.res"
	-@erase ".\WinDebug\Rigsset.ilk"
	-@erase ".\WinDebug\Rigsset.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /ML /W3 /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /FR /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "f:\dxsdk\sdk\inc" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# SUBTRACT CPP /Fr
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "f:\dxsdk\sdk\inc" /D "WIN32" /D\
 "_DEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/Rigsset.pch" /YX /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\WinDebug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/dinstall.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Rigsset.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib /nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Rigsset.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Rigsset.exe" 
LINK32_OBJS= \
	"$(INTDIR)/dinstall.obj" \
	"$(INTDIR)/dinstall.res"

"$(OUTDIR)\Rigsset.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "RIGSSET - Win32 Release"
# Name "RIGSSET - Win32 Debug"

!IF  "$(CFG)" == "RIGSSET - Win32 Release"

!ELSEIF  "$(CFG)" == "RIGSSET - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\dinstall.rc
DEP_RSC_DINST=\
	".\setup.ico"\
	".\3drage.bmp"\
	".\wipeout.bmp"\
	".\dinstall.h"\
	

"$(INTDIR)\dinstall.res" : $(SOURCE) $(DEP_RSC_DINST) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\dinstall.c
DEP_CPP_DINSTA=\
	{$(INCLUDE)}"\dsetup.h"\
	".\dinstall.h"\
	

"$(INTDIR)\dinstall.obj" : $(SOURCE) $(DEP_CPP_DINSTA) "$(INTDIR)"


# End Source File
# End Target
# End Project
################################################################################
