#define _SIZE_T

#include <dos.h>
//#include <mem.h>
//#include	<i86.h>
//#include	<graph.h>

#include "common.h"
//#include "\wipesrc\win95\jeda\cgl.h"
#include "pcgraph.h"
#include "winh.h"
#include "win.h"

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
extern short	WinWidth;
extern short	WinHeight;
extern int	WinHeightX2;



DISPENV *PutDispEnv(DISPENV *env)
{
	extern short BG_Colour;


	curvidmode = env->disp.w;

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
	olddispy = env->disp.y;

	return(env);
}

DRAWENV *PutDrawEnv(DRAWENV *env)
{
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
