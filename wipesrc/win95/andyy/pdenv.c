#define _SIZE_T

#include <dos.h>
#include <mem.h>
#include	<i86.h>
#include	<graph.h>

#include "common.h"
#include "..\jeda\cgl.h"
#include "pcgraph.h"

extern short game_stat;
extern char Sky;

#define	INTERACTIVE			0
#define	ATTRACT				1

int curvidmode = 0xff;				/*don't care what mode we're coming from*/
CLIP vidclip;
CLIP curclip;
CLIP curclipS;	// jed
short	width, height, widthBytes;
unsigned actStart, visStart;
short 	olddispy;

char	*j_buff;

unsigned long	_SCAddress;
unsigned long	_SCSize;

extern	char	halfx;	// Jed
extern	char	halfy;	// etc.
extern	float	halfxf;
extern	float	halfyf;

extern	int		inattract;	 		//jed
extern	int		mode;				//jed
extern	char	inmenu;				//jed
extern	char	Reallyinmenu; 		// DLS
extern	char	*background_buffer;
extern	char 	*CurBuffer;

extern int	WinPitch;
extern int	WinWidth;
extern int	WinHeight;
extern int	WinHeightX2;


extern void Scopy();
#pragma aux Scopy=						\
		"mov	eax,_SCAddress"	\
		"mov	edi,0a0000h"		\
		"mov	esi,eax"				\
		"mov	ecx,_SCSize"		\
		"rep movsd"					\
		modify [eax ebx ecx edx esi edi];

extern void Sclear();
#pragma aux Sclear=							\
		"mov	ebx,_SCAddress"		\
		"mov	edi,ebx"					\
		"mov	ecx,_SCSize"			\
		"xor	eax,eax"					\
		"rep	stosd"					\
		modify [eax ebx ecx edx esi edi];


DISPENV *PutDispEnv(DISPENV *env)
{
	extern short BG_Colour;


	curvidmode = env->disp.w;

#ifdef DOS_VERSION
#ifdef	WIPEOUTPC
	if (env->disp.y != olddispy)
	{
		GSwap();
		if (inmenu==1 || Reallyinmenu==1)
		{
			memcpy(CurBuffer,&(background_buffer[0]),320*200);

		}
		else
		{
			ClearBuf(BG_Colour);
		}
	}
#else
	if (env->disp.y != olddispy)
		{
		GSwap();
		if (game_stat > 1)
			ClearGameBuf(BG_Colour);
		else
			ClearBuf(BG_Colour);
		}
#endif
#else //Assume Win '95
#ifdef	WIPEOUTPC
	if (env->disp.y != olddispy)
	{
		DDSwap();

		if (inmenu==1 || Reallyinmenu==1)
		{
			BltClearToScreen();
		}
		else
		{
			WinClearScreen(0);
		}
	}
#else
	if (env->disp.y != olddispy)
		{
			DDSwap();

		   while (LockBackBuffer())
	   		{					  
	   		ProcessMessages(); 	//loop until we can get that back buffer
			;
	   		}
			if (game_stat > 1)
		    	memset(CurBuffer,BG_Colour,320*200);
			else
		    	memset(CurBuffer,0,320*200);
	    	UnlockBackBuffer();	//Finished drawing
		}
#endif
#endif
	olddispy = env->disp.y;

	return(env);
}

DRAWENV *PutDrawEnv(DRAWENV *env)
{
	int 			loop;
	long			*screen;


	curclip.tx = env->clip.x;
	curclip.ty = (env->clip.y > 240) ? env->clip.y - 240 : env->clip.y;
	if (env->clip.y >= 240)
		curclip.ty = env->clip.y - 240;
	else
		curclip.ty = env->clip.y;

	curclip.w = env->clip.w;
	curclip.h = env->clip.h;


	curclip.bx = curclip.tx + curclip.w;
	curclip.by = curclip.ty + curclip.h;

	curclipS.tx = 0;
	curclipS.ty = 0;
	curclipS.bx = WinWidth - 1;
	curclipS.by = WinHeight -1;
	curclipS.w = WinWidth -1;
	curclipS.h = WinHeight -1;

	curclip.tx = 0;
	curclip.ty = 0;
	curclip.bx = WinWidth -1;
	curclip.by = WinHeight -1 ;
	curclip.w = WinWidth -1;
	curclip.h = WinHeight -1;

	if (inattract==0)		//jed
	{
		if (inmenu!=1)		//jed
		{
			if (halfx!=0)	//jed
			{
				if ((320/halfxf)<320)
				{
					curclip.bx = 320/(halfxf);	//jed
					curclip.w = 320/(halfxf);	//jed
				}
			}
			if (halfy!=0)			//jed
			{
				if ((240/halfyf)<240)
				{
					curclip.by = 240/(halfyf);	//jed
					curclip.h = 240/(halfyf);	//jed
				}
			}		
		}	
	}									//jed
	return(env);
}


