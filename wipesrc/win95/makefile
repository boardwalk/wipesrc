LINK=wlink
LIBCC=wcc386
CC=wcc386
RESCOMP=wrc
	
#DEMO=-dPC_DEMO=1
DEMO=
EXE=wipeout.exe
PROGRAM=wipeout
ASM		= tasm 													  
ASMINC=asm.i
OPT = -oneatxi
ASMFLAG	= /t /mx /p /m9 /zi /oi /dUSE_ATI=1
CFLAGS = -fpi87 -ei -d2 -dUSE_ATI=1 -dWIPEOUTPC=1 -zp4 $(DEMO) -we -i=..\psx26\include -s
#CFLAGS = -ei -5r -dUSE_ATI=1 -dWIPEOUTPC=1 -we $(DEMO) $(OPT)  -fp5 -fpi87 -zp4 -i=..\psx26\include -bt=NT -s
#had -5

CFLAGSNOZP = -d2 -dWIPEOUTPC=1 $(DEMO) -we $(OPT) -5 -fp5 -fpi87 -i=..\psx26\include

WINFLAGS =  -fpi87 -ei -dUSE_ATI=1 -D_WIN32 -DDEBUG -d2 -W4 -zq -zp4 -s
#WINFLAGS =  -fpi87 -ei $(OPT) -dUSE_ATI=1 -D_WIN32 -W4 -zq -zp4 -s

#LINK=ccpsx -Xo$80010000 -Xm
#LIBCC=ccpsx -O2 -c
#CC=ccpsx -O2 -c
#DBGCC=ccpsx -g -c
#ASM=asmpsx /l /zd
#ASMINC=asm.i

# chuck these in after main in the OBJ list
# asmtrk.obj \
# asmscn.obj \
# newscn.obj \
# asmfar.obj \	  

OBJ=main.obj\
 global.obj\
 object.obj\
 malloc.obj\
 file.obj\
 tim.obj\
 skeleton.obj\
 track.obj\
 wtl.obj\
 draw.obj\
 ttf.obj\
 camera.obj\
 ships.obj\
 control.obj\
 utils.obj\
 search.obj\
 dynam.obj\
 hud.obj\
 scene.obj\
 weapon.obj\
 collide.obj\
 sound.obj\
 init.obj\
 grabber.obj\
 options.obj\
 radscn.obj\
 radtrk.obj\
 editor.obj\
 texplace.obj\
 getvram.obj\
 error.obj\
 trakview.obj\
 effects.obj\
 handlers.obj\
 combat.obj\
 lzss.obj\
 iso9660.obj\
 menus.obj\
 hiscores.obj\
 dpa.obj\
 trackasm.obj\
 card.obj\
 spritewo.obj\
 sparks2.obj\
 cone.obj\
 crack.obj\
 pad.obj\
 droid.obj\
 animover.obj\
 credits.obj\
 ayrace.obj\
 racectrl.obj\
 spline.obj


INC=origsrc\global.h \
 origsrc\object.h \
 origsrc\malloc.h \
 origsrc\standard.h \
 origsrc\file.h \
 origsrc\tim.h \
 origsrc\skeleton.h \
 origsrc\track.h \
 origsrc\wtl.h \
 origsrc\draw.h \
 origsrc\ttf.h \
 origsrc\camera.h \
 origsrc\ships.h \
 origsrc\control.h \
 origsrc\scene.h \
 origsrc\weapon.h \
 origsrc\radscn.h \
 origsrc\radtrk.h \
 origsrc\editor.h \
 origsrc\texplace.h \
 origsrc\getvram.h \
 origsrc\pad.h \
 origsrc\error.h \
 origsrc\trakview.h \
 origsrc\combat.h \
 origsrc\fileops.h \
 origsrc\iso9660.h \
 origsrc\menus.h \
 origsrc\dpa.h \
 origsrc\trackasm.h \
 origsrc\sprite.h \
 origsrc\sparks2.h \
 origsrc\cone.h \
 origsrc\crack.h \
 origsrc\credits.h \
 origsrc\spline.h

ASMINC=asm.i

OBJJED	= drawot.obj loadall.obj
OBJAJS	= ajs.obj libspu.obj syscheck.obj
OBJCE	= libsnd.obj 
OBJAJY	= pdenv.obj	fileacc.obj vsync.obj datacash.obj blankfn.obj pallette.obj clearot.obj andyscn.obj tims.obj winclip.obj
OBJSW	= 3dpoly.obj textclip.obj flatclip.obj sprite.obj drawpoly.obj init3d.obj timer.obj
OBJDLS	= libgte.obj libgtes.obj
OBJMK	= etcvars.obj pmkey.obj pmjoy.obj pmmouse.obj initpad.obj startpad.obj stoppad.obj handler.obj config.obj timc.obj debug.obj
#OBJMKSER = multiwo.obj serialwo.obj modemwo.obj netwo.obj comio.obj pmcom.obj modem.obj netio.obj rmnet.obj merror.obj grfont.obj
OBJWIN = win.obj wintime.obj winsnd.obj wave.obj wincd.obj regread.obj
HWIN = win\winh.h

#thelot: $(PROGRAM) 
thelot: $(PROGRAM) 

winsnd.exe: winsnd.obj
		$(LINK) @winsnd.lnk

$(PROGRAM): $(EXE)
	$(RESCOMP) $(PROGRAM)

#$(PROGRAM): $(OBJ) $(OBJJED) $(OBJMK) $(OBJSW) $(OBJDLS) $(OBJAJS) $(OBJAJY) $(OBJCE) $(SYS1) $(SYS2)  $(OBJWIN) makefile linklist
$(EXE): $(OBJ) $(OBJJED) $(OBJMK) $(OBJSW) $(OBJDLS) $(OBJAJS) $(OBJAJY) $(OBJCE) $(SYS1) $(SYS2) $(OBJWIN) makefile linklist
        $(LINK) @linklist.

#asmtrk.obj: asmtrk.s $(ASMINC)
#   $(ASM) asmtrk.s,asmtrk.obj,asmtrk.sym,asmtrk.lst

#asmscn.obj: asmscn.s $(ASMINC)
#   $(ASM) asmscn.s,asmscn.obj,asmscn.sym,asmscn.lst

#newscn.obj: newscn.s $(ASMINC)
#   $(ASM) newscn.s,newscn.obj,newscn.sym,newscn.lst

#asmfar.obj: asmfar.s $(ASMINC)
#   $(ASM) asmfar.s,asmfar.obj,asmfar.sym,asmfar.lst

main.obj: origsrc\main.c $(INC) makefile
        $(CC) $(CFLAGS) origsrc\main.c

global.obj: origsrc\global.c $(INC)
        $(CC) $(CFLAGS) origsrc\global.c

track.obj: origsrc\track.c $(INC) makefile
        $(CC) $(CFLAGS) origsrc\track.c

wtl.obj: origsrc\wtl.c $(INC)
        $(CC) $(CFLAGS) origsrc\wtl.c

draw.obj: origsrc\draw.c $(INC)
        $(CC) $(CFLAGS) origsrc\draw.c

ttf.obj: origsrc\ttf.c $(INC)
        $(CC) $(CFLAGS) origsrc\ttf.c

camera.obj: origsrc\camera.c $(INC)
        $(CC) $(CFLAGS) origsrc\camera.c

ships.obj: origsrc\ships.c $(INC)
        $(CC) $(CFLAGS) origsrc\ships.c

control.obj: origsrc\control.c $(INC)
        $(CC) $(CFLAGS) origsrc\control.c

utils.obj: origsrc\utils.c $(INC)
        $(CC) $(CFLAGS) origsrc\utils.c

search.obj: origsrc\search.c $(INC)
        $(CC) $(CFLAGS) origsrc\search.c

dynam.obj: origsrc\dynam.c $(INC)
        $(CC) $(CFLAGS) origsrc\dynam.c

hud.obj: origsrc\hud.c $(INC)
        $(CC) $(CFLAGS) origsrc\hud.c

scene.obj: origsrc\scene.c $(INC) makefile
        $(CC) $(CFLAGS) origsrc\scene.c

weapon.obj: origsrc\weapon.c $(INC)
        $(CC) $(CFLAGS) origsrc\weapon.c

collide.obj: origsrc\collide.c $(INC)
        $(CC) $(CFLAGS) origsrc\collide.c

sound.obj: origsrc\sound.c $(INC)
        $(CC) $(CFLAGS) origsrc\sound.c

race.obj: origsrc\race.c $(INC)
        $(CC) $(CFLAGS) origsrc\race.c

ayrace.obj: origsrc\ayrace.c $(INC) origsrc\fri.h
        $(CC) $(CFLAGS) origsrc\ayrace.c

racectrl.obj: origsrc\racectrl.c $(INC) origsrc\fri.h
        $(CC) $(CFLAGS) origsrc\racectrl.c

init.obj: origsrc\init.c $(INC)
        $(CC) $(CFLAGS) origsrc\init.c

grabber.obj: origsrc\grabber.c $(INC)
        $(CC) $(CFLAGS) origsrc\grabber.c

options.obj: origsrc\options.c $(INC)
        $(CC) $(CFLAGS) origsrc\options.c

radscn.obj: origsrc\radscn.c $(INC)
        $(CC) $(CFLAGS) origsrc\radscn.c

radtrk.obj: origsrc\radtrk.c $(INC)
        $(CC) $(CFLAGS) origsrc\radtrk.c

editor.obj: origsrc\editor.c $(INC)
        $(CC) $(CFLAGS) origsrc\editor.c

texplace.obj: origsrc\texplace.c $(INC)
        $(CC) $(CFLAGS) origsrc\texplace.c

getvram.obj: origsrc\getvram.c $(INC)
        $(CC) $(CFLAGS) origsrc\getvram.c

error.obj: origsrc\error.c $(INC)
        $(CC) $(CFLAGS) origsrc\error.c

trakview.obj: origsrc\trakview.c $(INC)
        $(CC) $(CFLAGS) origsrc\trakview.c

effects.obj: origsrc\effects.c $(INC)
        $(CC) $(CFLAGS) origsrc\effects.c

combat.obj: origsrc\combat.c $(INC)
        $(CC) $(CFLAGS) origsrc\combat.c

handlers.obj: origsrc\handlers.c $(INC)
        $(CC) $(CFLAGS) origsrc\handlers.c

lzss.obj: origsrc\lzss.c $(INC)
        $(CC) $(CFLAGS) origsrc\lzss.c

iso9660.obj: origsrc\iso9660.c $(INC)
        $(CC) $(CFLAGS) origsrc\iso9660.c

menus.obj: origsrc\menus.c $(INC) makefile
        $(CC) $(CFLAGS) origsrc\menus.c

hiscores.obj: origsrc\hiscores.c $(INC)
        $(CC) $(CFLAGS) origsrc\hiscores.c

card.obj: origsrc\card.c $(INC)
        $(CC) $(CFLAGS) origsrc\card.c

dpa.obj: origsrc\dpa.c $(INC)
        $(CC) $(CFLAGS) origsrc\dpa.c

trackasm.obj: origsrc\trackasm.c $(INC) makefile
        $(CC) $(CFLAGS) origsrc\trackasm.c

object.obj: origsrc\object.c $(INC) makefile
        $(CC) $(CFLAGS) origsrc\object.c

malloc.obj: origsrc\malloc.c $(INC) 
        $(CC) $(CFLAGS) origsrc\malloc.c

file.obj: origsrc\file.c $(INC) 
        $(CC) $(CFLAGS) origsrc\file.c

tim.obj: origsrc\tim.c $(INC) 
        $(CC) $(CFLAGS) origsrc\tim.c

skeleton.obj: origsrc\skeleton.c $(INC) 
        $(CC) $(CFLAGS) origsrc\skeleton.c

spritewo.obj: origsrc\spritewo.c $(INC) 
        $(CC) $(CFLAGS) origsrc\spritewo.c

sparks2.obj: origsrc\sparks2.c $(INC) 
        $(CC) $(CFLAGS) origsrc\sparks2.c

cone.obj: origsrc\cone.c $(INC) 
        $(CC) $(CFLAGS) origsrc\cone.c

crack.obj: origsrc\crack.c $(INC) 
        $(CC) $(CFLAGS) origsrc\crack.c

pad.obj: origsrc\pad.c $(INC) 
        $(CC) $(CFLAGS) origsrc\pad.c

droid.obj: origsrc\droid.c $(INC) 
        $(CC) $(CFLAGS) origsrc\droid.c

animover.obj: origsrc\animover.c $(INC) 
        $(CC) $(CFLAGS) origsrc\animover.c

credits.obj: origsrc\credits.c $(INC) 
        $(CC) $(CFLAGS) origsrc\credits.c

spline.obj: origsrc\spline.c $(INC) 
        $(CC) $(CFLAGS) origsrc\spline.c

#***** OUR STUFF ****
ajs.obj:	andys\ajs.c
		$(CC)  $(CFLAGS) $(COMPILE_PATH)andys\ajs.c			 
libspu.obj:	andys\libspu.c
		$(CC)  $(CFLAGS) $(COMPILE_PATH)andys\libspu.c			 

libsnd.obj:	chrise\libsnd.c
		$(CC)  $(CFLAGS) $(COMPILE_PATH)chrise\libsnd.c

libgte.obj:	daves\libgte.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)daves\libgte.c
libgtes.obj: daves\libgtes.asm
		$(ASM)	$(ASMFLAG)	$(COMPILE_PATH)daves\libgtes.asm

drawot.obj:	jeda\drawot.c $(HWIN) makefile
		$(CC) $(CFLAGS) $(COMPILE_PATH)jeda\drawot.c
loadall.obj: jeda\loadall.c
		$(CC) $(CFLAGS) $(COMPILE_PATH)jeda\loadall.c
tims.obj: andyy\tims.asm
		$(ASM)	$(ASMFLAG)	$(COMPILE_PATH)andyy\tims.asm

andyscn.obj:	andyy\andyscn.c makefile
		$(CC)	$(CFLAGS) $(COMPILE_PATH)andyy\andyscn.c
pdenv.obj:	andyy\pdenv.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\pdenv.c
vsync.obj:	andyy\vsync.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\vsync.c
addpn4.obj: andyy\addpn4.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\addpn4.c
blankfn.obj: andyy\blankfn.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\blankfn.c
pallette.obj: andyy\pallette.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\pallette.c
clearot.obj: andyy\clearot.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\clearot.c
datacash.obj: andyy\datacash.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\datacash.c
fileacc.obj: andyy\fileacc.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)andyy\fileacc.c

rmnet.obj: markk\rmnet.asm
		$(ASM)	/ml /m2	$(COMPILE_PATH)markk\rmnet.asm
grfont.obj: markk\grfont.asm
		$(ASM)	$(ASMFLAG)	$(COMPILE_PATH)markk\grfont.asm

debug.obj: markk\debug.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\debug.c
multiwo.obj: markk\multiwo.c markk\debug.h
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\multiwo.c
serialwo.obj: markk\serialwo.c markk\debug.h
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\serialwo.c
modemwo.obj: markk\modemwo.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\modemwo.c
netwo.obj: markk\netwo.c
		$(CC)  	$(CFLAGSNOZP) $(COMPILE_PATH)markk\netwo.c
comio.obj: markk\comio.c markk\debug.h
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\comio.c
pmcom.obj: markk\pmcom.c markk\debug.h
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\pmcom.c
modem.obj: markk\modem.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\modem.c
netio.obj: markk\netio.c
		$(CC)  	$(CFLAGSNOZP) $(COMPILE_PATH)markk\netio.c
merror.obj: markk\merror.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\merror.c
etcvars.obj: markk\etcvars.obj
		copy markk\etcvars.obj
pmkey.obj: markk\pmkey.obj
		copy markk\pmkey.obj
pmjoy.obj: markk\pmjoy.obj
		copy markk\pmjoy.obj
pmmouse.obj: markk\pmmouse.obj
		copy markk\pmmouse.obj
initpad.obj: markk\initpad.obj
		copy markk\initpad.obj
startpad.obj: markk\startpad.obj
		copy markk\startpad.obj
stoppad.obj: markk\stoppad.obj
		copy markk\stoppad.obj
handler.obj: markk\handler.obj
		copy markk\handler.obj
config.obj: markk\config.obj
		copy markk\config.obj
timc.obj: markk\timc.c
		$(CC)  	$(CFLAGS) $(COMPILE_PATH)markk\timc.c

3dpoly.obj: stevew\3dpoly.asm   
		$(ASM) $(ASMFLAG) -dWIPEOUT=1 $(COMPILE_PATH)stevew\3dpoly.asm
flatclip.obj: stevew\flatclip.asm   
		$(ASM) $(ASMFLAG) $(COMPILE_PATH)stevew\flatclip.asm
textclip.obj: stevew\textclip.asm   
		$(ASM) $(ASMFLAG) $(COMPILE_PATH)stevew\textclip.asm
sprite.obj: stevew\sprite.asm makefile   
		$(ASM) $(ASMFLAG) $(COMPILE_PATH)stevew\sprite.asm
drawpoly.obj: stevew\drawpoly.asm   
		$(ASM) $(ASMFLAG) -dWIPEOUT=1 $(COMPILE_PATH)stevew\drawpoly.asm
timer.obj: stevew\timer.asm   
		$(ASM) $(ASMFLAG) -dWIPEOUT=1 $(COMPILE_PATH)stevew\timer.asm
init3d.obj:	stevew\init3d.c
		$(CC)  $(CFLAGS) $(COMPILE_PATH)stevew\init3d.c

syscheck.obj: andys\syscheck.c
		$(CC) $(COMPILE_PATH)andys\syscheck.c

win.obj: win\win.c $(HWIN) makefile
	wcc386  $(WINFLAGS) win\win.c

wincd.obj: win\wincd.c $(HWIN) makefile
	wcc386 $(WINFLAGS) win\wincd.c
	
regread.obj: win\regread.c $(HWIN) makefile
	wcc386 $(WINFLAGS) win\regread.c

winclip.obj: win\winclip.c makefile
	wcc386 $(CFLAGS) win\winclip.c
	
wintime.obj: win\wintime.c
	wcc386  $(WINFLAGS) win\wintime.c

winsnd.obj: win\winsnd.c
	wcc386  $(WINFLAGS) win\winsnd.c

wave.obj: win\wave.c
	wcc386  $(WINFLAGS) win\wave.c


all:
	@echo *** start making *** 
	@wmake /u clean
	@wmake /u $(PROGRAM)
	@echo *** e
