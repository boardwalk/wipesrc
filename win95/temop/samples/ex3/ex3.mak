# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=EX3 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to EX3 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "EX3 - Win32 Release" && "$(CFG)" != "EX3 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "EX3.mak" CFG="EX3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "EX3 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "EX3 - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "EX3 - Win32 Debug"
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "EX3 - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "\RAGESDK.NEW\BIN"
# PROP Intermediate_Dir "\RAGESDK.NEW\SAMPLES\EX3\OBJ"
# PROP Target_Dir ""
OUTDIR=\RAGESDK.NEW\BIN
INTDIR=\RAGESDK.NEW\SAMPLES\EX3\OBJ

ALL : "$(OUTDIR)\EX3.exe"

CLEAN : 
	-@erase "..\..\BIN\EX3.exe"
	-@erase ".\OBJ\exutil.obj"
	-@erase ".\OBJ\PARSEASC.OBJ"
	-@erase ".\OBJ\EX3.OBJ"
	-@erase ".\OBJ\GEOM.OBJ"
	-@erase ".\OBJ\glob.obj"
	-@erase ".\OBJ\EX3.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /GX /O2 /I "\gdk\sdk\inc" /I " \ragesdk.new\inc" /I " ..\common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /ML /W3 /GX /O2 /I "\gdk\sdk\inc" /I " \ragesdk.new\inc" /I\
 " ..\common" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Fp"$(INTDIR)/EX3.pch" /YX\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=\RAGESDK.NEW\SAMPLES\EX3\OBJ/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/EX3.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/EX3.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \ragesdk.new\lib\ati3dcif.lib \gdk\sdk\lib\ddraw.lib /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib \ragesdk.new\lib\ati3dcif.lib \gdk\sdk\lib\ddraw.lib /nologo\
 /subsystem:windows /incremental:no /pdb:"$(OUTDIR)/EX3.pdb" /machine:I386\
 /out:"$(OUTDIR)/EX3.exe" 
LINK32_OBJS= \
	"$(INTDIR)/exutil.obj" \
	"$(INTDIR)/PARSEASC.OBJ" \
	"$(INTDIR)/EX3.OBJ" \
	"$(INTDIR)/GEOM.OBJ" \
	"$(INTDIR)/glob.obj" \
	"$(INTDIR)/EX3.res"

"$(OUTDIR)\EX3.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "EX3 - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "\RAGESDK.NEW\BIN"
# PROP Intermediate_Dir "\RAGESDK.NEW\SAMPLES\EX3\OBJ\DBG"
# PROP Target_Dir ""
OUTDIR=\RAGESDK.NEW\BIN
INTDIR=\RAGESDK.NEW\SAMPLES\EX3\OBJ\DBG

ALL : "$(OUTDIR)\EX3DBG.exe"

CLEAN : 
	-@erase ".\OBJ\DBG\vc40.pdb"
	-@erase ".\OBJ\DBG\vc40.idb"
	-@erase "..\..\BIN\EX3DBG.exe"
	-@erase ".\OBJ\DBG\glob.obj"
	-@erase ".\OBJ\DBG\exutil.obj"
	-@erase ".\OBJ\DBG\GEOM.OBJ"
	-@erase ".\OBJ\DBG\PARSEASC.OBJ"
	-@erase ".\OBJ\DBG\EX3.OBJ"
	-@erase ".\OBJ\DBG\EX3.res"
	-@erase "..\..\BIN\EX3DBG.ilk"
	-@erase "..\..\BIN\EX3DBG.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

"$(INTDIR)" :
    if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "\gdk\sdk\inc" /I " \ragesdk.new\inc" /I " ..\common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
CPP_PROJ=/nologo /MLd /W3 /Gm /GX /Zi /Od /I "\gdk\sdk\inc" /I\
 " \ragesdk.new\inc" /I " ..\common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /Fp"$(INTDIR)/EX3.pch" /YX /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=\RAGESDK.NEW\SAMPLES\EX3\OBJ\DBG/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/EX3.res" /d "_DEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/EX3.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib \ragesdk.new\lib\ati3dcif.lib \gdk\sdk\lib\ddraw.lib /nologo /subsystem:windows /debug /machine:I386 /out:"\RAGESDK.NEW\BIN/EX3DBG.exe"
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib\
 advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib\
 odbccp32.lib \ragesdk.new\lib\ati3dcif.lib \gdk\sdk\lib\ddraw.lib /nologo\
 /subsystem:windows /incremental:yes /pdb:"$(OUTDIR)/EX3DBG.pdb" /debug\
 /machine:I386 /out:"$(OUTDIR)/EX3DBG.exe" 
LINK32_OBJS= \
	"$(INTDIR)/glob.obj" \
	"$(INTDIR)/exutil.obj" \
	"$(INTDIR)/GEOM.OBJ" \
	"$(INTDIR)/PARSEASC.OBJ" \
	"$(INTDIR)/EX3.OBJ" \
	"$(INTDIR)/EX3.res"

"$(OUTDIR)\EX3DBG.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "EX3 - Win32 Release"
# Name "EX3 - Win32 Debug"

!IF  "$(CFG)" == "EX3 - Win32 Release"

!ELSEIF  "$(CFG)" == "EX3 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\EX3.RC
DEP_RSC_EX3_R=\
	".\resource.h"\
	

"$(INTDIR)\EX3.res" : $(SOURCE) $(DEP_RSC_EX3_R) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\EX3.CPP
DEP_CPP_EX3_C=\
	"\gdk\sdk\inc\ddraw.h"\
	".\resource.h"\
	"\ragesdk.new\inc\ati3dcif.h"\
	".\..\common\exutil.h"\
	".\..\common\glob.h"\
	".\..\common\geom.h"\
	".\..\common\parseasc.h"\
	

"$(INTDIR)\EX3.OBJ" : $(SOURCE) $(DEP_CPP_EX3_C) "$(INTDIR)"


# End Source File
################################################################################
# Begin Source File

SOURCE=\ragesdk.new\SAMPLES\COMMON\glob.cpp
DEP_CPP_GLOB_=\
	"\gdk\sdk\inc\ddraw.h"\
	"\ragesdk.new\inc\ati3dcif.h"\
	

"$(INTDIR)\glob.obj" : $(SOURCE) $(DEP_CPP_GLOB_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\ragesdk.new\SAMPLES\COMMON\GEOM.CPP
DEP_CPP_GEOM_=\
	".\..\common\geom.h"\
	

"$(INTDIR)\GEOM.OBJ" : $(SOURCE) $(DEP_CPP_GEOM_) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\ragesdk.new\SAMPLES\COMMON\exutil.cpp
DEP_CPP_EXUTI=\
	"\gdk\sdk\inc\ddraw.h"\
	"\ragesdk.new\inc\ati3dcif.h"\
	".\..\common\glob.h"\
	".\..\common\exutil.h"\
	

"$(INTDIR)\exutil.obj" : $(SOURCE) $(DEP_CPP_EXUTI) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=\ragesdk.new\SAMPLES\COMMON\PARSEASC.CPP
DEP_CPP_PARSE=\
	".\..\common\geom.h"\
	".\..\common\glob.h"\
	".\..\common\parseasc.h"\
	"\gdk\sdk\inc\ddraw.h"\
	"\ragesdk.new\inc\ati3dcif.h"\
	

"$(INTDIR)\PARSEASC.OBJ" : $(SOURCE) $(DEP_CPP_PARSE) "$(INTDIR)"
   $(CPP) $(CPP_PROJ) $(SOURCE)


# End Source File
# End Target
# End Project
################################################################################
