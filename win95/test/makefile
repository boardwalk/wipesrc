#OPT = -oneatx
WINFLAGS =  $(OPT) -D_WIN32 -DDEBUG -d2 -W3 -zq -ic:\gdk\sdk\inc;c:\gdk\sdk\SAMPLES\WATINC;C:\WATCOM\h\nt;\mstools\include;c:\gdk\sdk\samples\misc;C:\WATCOM\h;C:\WATCOM\h\win -bt=NT -fh
ASM		= tasm 													  
ASMFLAG	= /t /mx /p /m9 /zi /oi
CFLAGS = -d2 $(OPT) -dWIPEOUTPC=1 -we $(DEMO) -i=c:\andyy\psx26\include -bt=NT
CC=wcc386


test1.exe: win.obj 3dpoly.obj flatclip.obj textclip.obj sprite.obj drawpoly.obj init3d.obj drawot.obj tri.obj makefile linklist
	wlink @linklist.
	copy test1.exe debug

win.obj: win.c
	wcc386  $(WINFLAGS) win.c

3dpoly.obj: 3dpoly.asm   
		$(ASM) $(ASMFLAG) -dWIPEOUT=1 $(COMPILE_PATH)3dpoly.asm
flatclip.obj: flatclip.asm   
		$(ASM) $(ASMFLAG) $(COMPILE_PATH)flatclip.asm
textclip.obj: textclip.asm   
		$(ASM) $(ASMFLAG) $(COMPILE_PATH)textclip.asm
sprite.obj: sprite.asm   
		$(ASM) $(ASMFLAG) $(COMPILE_PATH)sprite.asm
drawpoly.obj: drawpoly.asm   
		$(ASM) $(ASMFLAG) -dWIPEOUT=1 $(COMPILE_PATH)drawpoly.asm
init3d.obj:	init3d.c
		$(CC)  $(CFLAGS) $(COMPILE_PATH)init3d.c

drawot.obj:	drawot.c
		$(CC) $(CFLAGS) $(COMPILE_PATH)drawot.c
tri.obj:	tri.c
		$(CC) $(CFLAGS) $(COMPILE_PATH)tri.c
