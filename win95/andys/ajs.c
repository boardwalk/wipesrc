//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Some 'PlayStation on the PC' functions
// Andy Satterthwaite, March / April 1995
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// OK these are the finished functions I've written. Hope it all works ...
// There may be problems in LoadImage, StoreImage and MoveImage 'cause I haven't tested them
// they look ok though.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// typedefs to get includes working under watcom

#include <sys\types.h>
#include <libgte.h>

#define _SIZE_T_DEFINED_
#include "c:\wc10\h\stdio.h"
#include "c:\wc10\h\stdlib.h"
#include	"..\andyy\pallette.h"

#define	MAIN_PAL	0
#define	SHIELD_PAL	1


extern short rendering;

#define addPrim(ot, p)	setaddr(p, getaddr(ot)), setaddr(ot, p)
typedef struct {
	int	width;
	int	height;
	int	vram_xpos;
	int	vram_ypos;
	char	*bitmap;
	char	*palette;
} bm;

CVECTOR	Palette[256];
char	main_palette[768];
char shield_palette[768];
char winLoseIn=0;
char	current_palette=MAIN_PAL;

char winPic[35840];
char losePic[35840];
extern char inmenu,Reallyinmenu;
extern	char *VRam;
extern	char *DepthFadeTble;
extern unsigned int	this_frame, last_frame;

//#define	LOADTEXTURES	1					// standard load stuff (LoadImage4, LoadImage8 etc.);
//#define	LOADAS24BIT	1						// if you want to load vram as 1 byte red, 1 byte green & 1 byte blue (requires 6 mb vram)
#define	BLANKFUNCTIONS	1					// if loading vram as pcx

// The playstation includes

#include <libgpu.h>	// with function prototypes to be written by me removed
// #include "c:\wc10\h\i86.h"


// System global variables and defines

#define VRAM_WIDTH	2048		//	VRAM_WIDTH is used to represent the width of the PSX video ram
						  		//	within the PC. It is set to 2048 (1024 words) it is used in the
						  		//	LoadImage, StoreImage and MoveImage functions to enable x,y
						  		//	style access to the ram.

u_long	*video_start_addr;		//	video_start_addr is a pointer to the PC equivalent of the 0,0
						  		//	co-ordinate in PSX video ram. This is used in LoadImage,
								//	StoreImage and MoveImage

DISPENV	g_DispEnv;				//	g_DispEnv is used to represent the PSX display environment, it
								//	is used in GetDispEnv to get the system values - this function
								//	is NOT used in RidgeRacer

DRAWENV	g_DrawEnv;				//	g_DrawEnv is used to represent the PSX drawing environment, it
								//	is used in GetDrawEnv to get the system values - this function
								//	is NOT used in RidgeRacer

int		g_mask;					// global display mask variable set in SetDispMask


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Most of the in function comments are taken straight from the manuals - so blame them if they're
// crap !
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// PLAYSTATION FUNCTIONS
//~~~~~~~~~~~~~~~~~~~~~~~
// functions originally declared in libgpu.h

/*

DISPENV	  	*GetDispEnv(DISPENV *env);		// NOT USED IN RIDGERACER

DRAWENV	  	*GetDrawEnv(DRAWENV *env);		// NOT USED IN RIDGERACER

DISPENV	  	*SetDefDispEnv(DISPENV *disp, int x, int y, int w, int h);

DRAWENV	  	*SetDefDrawEnv(DRAWENV *draw, long x, long y, long w, long h);
			
void	  	SetDrawEnv(DR_ENV *dr_env, DRAWENV *env);

void	  	SetDrawMode(DR_MODE *dr_mode, int dfe, int dtd, unsigned short tpage, RECT *tw);

void	  	LoadImage(RECT	*recp, unsigned long *p);

void	  	MoveImage(RECT	*rect, int x, int y);	// NOT USED IN RIDGERACER

void	  	StoreImage(RECT	*recp, unsigned long *p);

long	  	DrawSync(long	mode);
					
void	  	ResetGraph(int	mode);

void	  	SetGraphDebug(int level); 

void	  	InterpolByte(u_char*, u_char*, long, u_char*);

*/

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef USE_ATI


void		InterpolByte(u_char *in1, u_char *in2, long p0, u_char *rtn)
{
		// undocumented gte function which returns the difference between 2
		// lots of 2 bytes based on the 12 point fixed point number p0
	
		float	p1;
		
		p1=(4096 - (float)p0) / 4096;

		rtn[0]=((in2[0]-in1[0])*p1)+in1[0];
		rtn[1]=((in2[1]-in1[1])*p1)+in1[1];
}

#endif //USE_ATI

int		DrawSync(int	mode)
{
	// function used for parallel actions on PSX
	// defined so games compile on PC
	return(1);
}

#ifndef USE_ATI

void	DrawSyncCallback(func)
void	(*func)();
{
	// function used for parallel actions on PSX
	// defined so games compile on PC
}

#endif //USE_ATI

void		ResetGraph(int	mode)
{
	// function used for parallel actions on PSX
	// defined so games compile on PC
}

void		SetGraphDebug(int level)
{
	// function used for parallel actions on PSX
	// defined so games compile on PC
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FUNCTIONS WHICH JUST CALL THEIR EQUIVALENT MACROS

void	AddPrim(u_long *ordt, u_long *p)
{
	if(rendering)
		ContinueOTag();
 	addPrim(ordt, p);
}

#ifndef USE_ATI

void	AddPrims(u_long *ot, u_long *ps, u_long *pe)
{
	addPrims(ot, ps, pe);
}

#endif //USE_ATI
#ifndef USE_ATI


void	CatPrim(u_long *p0, u_long *p1)
{
	catPrim(p0, p1);
}
#endif //USE_ATI
#ifndef USE_ATI


void	DumpClut(u_short clut)
{
	dumpClut(clut);
}
#endif //USE_ATI

#ifndef USE_ATI

void	DumpTPage(u_short tpage)
{
	dumpTPage(tpage);
}
#endif //USE_ATI

#ifndef USE_ATI

u_short	GetClut(int x, int y)
{
	return(getClut(x,y));
}
#endif //USE_ATI

u_short GetTPage(int tp, int abr, int x, int y)
{
	return(getTPage(tp,abr,x,y));
}

#ifndef USE_ATI


u_long *NextPrim(u_long *p)
{
	return(nextPrim(p));
}
#endif //USE_ATI

#ifndef USE_ATI

void SetLineF2(LINE_F2 *p)
{
	setLineF2(p);
}
#endif //USE_ATI

void SetLineG2(LINE_G2 *p)
{
	setLineG2(p);
}

void SetPolyF3(POLY_F3  *p)
{
	setPolyF3(p);
}

void SetPolyFT3(POLY_FT3 *p)
{
	setPolyFT3(p);
}

void SetPolyG3(POLY_G3  *p)
{
	setPolyG3(p);
}
#if 0
void SetPolyGT(POLY_GT3 *p)
{
	setPolyGT(p);
}
#endif
void SetPolyF4(POLY_F4  *p)
{
	setPolyF4(p);
}

void SetPolyFT4(POLY_FT4 *p)
{
	setPolyFT4(p);
}

void SetPolyG4(POLY_G4  *p)
{
	setPolyG4(p);
}

void SetPolyGT4(POLY_GT4 *p)
{
	setPolyGT4(p);
}

void SetSemiTrans(u_long *p, long abe)
{
	setSemiTrans(p,abe);
}

void SetShadeTex(u_long *p, long tge)
{
	setShadeTex(p,tge);
}

#ifndef USE_ATI

void SetSprt8(SPRT_8  *p)
{
	setSprt8(p);
}
#endif //USE_ATI

#ifndef USE_ATI

void SetSprt16(SPRT_16 *p)
{
	setSprt16(p);
}

#endif //USE_ATI


void SetSprt(SPRT    *p)
{
	setSprt(p);
}

#ifndef USE_ATI

void SetTile1(TILE_1  *p)
{
	setTile1(p);
}
#endif //USE_ATI
#ifndef USE_ATI


void SetTile8(TILE_8  *p)
{
	setTile8(p);
}
#endif //USE_ATI

#ifndef USE_ATI

void SetTile16(TILE_16 *p)
{
	setTile16(p);
}

#endif //USE_ATI
#ifndef USE_ATI


void SetTile(TILE    *p)
{
	setTile(p);
}
#endif //USE_ATI

#ifndef USE_ATI

void TermPrim(u_long *p)
{
	termPrim(p);
}
#endif //USE_ATI

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef USE_ATI

DISPENV	*GetDispEnv(DISPENV *env)	// not used in RidgeRacer
{
	// Stores the current display environment in the address specified by env

	env->disp.x = g_DispEnv.disp.x;
	env->disp.y = g_DispEnv.disp.y;
	env->disp.w = g_DispEnv.disp.w;
	env->disp.h = g_DispEnv.disp.h;

	env->screen.x = g_DispEnv.screen.x;
	env->screen.y = g_DispEnv.screen.y;
	env->screen.w = g_DispEnv.screen.w;
	env->screen.h = g_DispEnv.screen.h;

	env->isinter = g_DispEnv.isinter;
	env->isrgb24 = g_DispEnv.isrgb24;

	env->pad0 = g_DispEnv.pad0;
 //	env->pad1 = g_DispEnv.pad1;
	
	return(env);
}
#endif //USE_ATI

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void				SetDispMask(int mask)
{
	// Puts display mask into the status specified by mask. The following values can be used for mask
	//	0 - Not displayed on screen
	// 1 - Displayed on screen

	g_mask=mask;
}

#ifndef USE_ATI

DRAWENV			*GetDrawEnv(DRAWENV *env)	// not used in RidgeRacer
{
	// Stores the current drawing environment in the address specified by env

	u_char	count;

	env->clip.x = g_DrawEnv.clip.x;
	env->clip.y = g_DrawEnv.clip.y;
	env->clip.w = g_DrawEnv.clip.w;
	env->clip.h = g_DrawEnv.clip.h;

	env->ofs[0] = g_DrawEnv.ofs[0];
	env->ofs[1] = g_DrawEnv.ofs[1];
		
	env->tw.x = g_DrawEnv.tw.x;
	env->tw.y = g_DrawEnv.tw.y;
	env->tw.w = g_DrawEnv.tw.w;
	env->tw.h = g_DrawEnv.tw.h;

	env->tpage = g_DrawEnv.tpage;
	env->dtd = g_DrawEnv.dtd;
	env->dfe = g_DrawEnv.dfe;
	env->isbg = g_DrawEnv.isbg;
	
	env->r0 = g_DrawEnv.r0;
	env->g0 = g_DrawEnv.g0;
	env->b0 = g_DrawEnv.b0;
	
	env->dr_env.tag = g_DrawEnv.dr_env.tag;

	for (count=0;count<15;count++)
		env->dr_env.code[count] = g_DrawEnv.dr_env.code[count];

	return(env);
}
#endif //USE_ATI

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DISPENV	*SetDefDispEnv(DISPENV *disp, int x, int y, int w, int h)
{
	// Sets display environment structure members using the point on the top left of the display
	// area and width and height of the display area (values set as in table below)
	//
	// MASK        CONTENT           VALUE
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// disp        Display Area      (x,y,w,h)
	// ds          Display Offset    (0,0)
	// isinter     Interlace Flag    0
	// isrgb       24bit Mode Flag   0

	disp->disp.x = x;
	disp->disp.y = y;
	disp->disp.w = w;
	disp->disp.h = h;

	disp->screen.x = 0;	
	disp->screen.y = 0;	
	disp->screen.w = 0;
	disp->screen.h = 0;

	disp->isinter = 0;
	disp->isrgb24 = 0;

	return(disp);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

DRAWENV	*SetDefDrawEnv(DRAWENV *draw, int x, int y, int w, int h)
{
	// Sets standard drawing environment structure members judging from the point on the left top
	// of the drawing area, and width and height of the drawing area (values set as in table below)
	//
	// MEMBER      CONTENT                                       VALUE
	//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
	// clip        Drawing area                                  (x,y,w,h)
	// ofs[2]      Drawing offset                                (x,y)
   // tw          Texture Window                                (0,0,0,0)
	// tpage[2]    Texture page (tp, abr, tx, ty)                (0,0,640,0)
	// dtd         Dither processing flag                        1 (ON)
	// dfe         Permission flag for drawing on display area	 0 (clear: OFF)
	// isbg        Draw area clear flag                          0 (clear: OFF)
	// r0,g0,b0    Background Colour                             1 (0,0,0)

	draw->clip.x = x;
	draw->clip.y = y;
	draw->clip.w = w;
	draw->clip.h = h;

	draw->ofs[0] = x;
	draw->ofs[1] = y;
		
	draw->tw.x = 0;
	draw->tw.y = 0;
	draw->tw.w = 0;
	draw->tw.h = 0;

	draw->tpage = 10; /* (0,0,640,0) */
	draw->dtd = 1;
	draw->dfe = 0;
	draw->isbg = 0;
	
	draw->r0 = 0;
	draw->g0 = 0;
	draw->b0 = 0;
	
	// ignored the dr_env bit 'cause we don't know what it does

	return(draw);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#ifndef USE_ATI


void		SetDrawEnv(DR_ENV *dr_env, DRAWENV *env)
{
	// Initialises the drawing environment change primitive. 
	// The format below was designed by me (ajs) if you don't like it - design your own !


	// FORMAT FOR DR_ENV.CODE ARRAY:
	//	2 bytes : clip.x
	// 2 bytes : clip.y

	// 2 bytes : clip.w
	// 2 bytes : clip.h

	// 2 bytes : ofs[0]
	// 2 bytes : ofs[1]

	// 2 bytes : tw.x
	// 2 bytes : tw.y

	// 2 bytes : tw.w
	// 2 bytes : tw.h

	// 2 bytes : tpage
	// 1 byte  : dtd
	// 1 byte  : dfe

	// 1 byte  : isbg
	// 1 byte  : r0
	// 1 byte  : g0
	// 1 byte  : b0

	dr_env->code[1]=env->clip.x<<16;
	dr_env->code[1]+=env->clip.y;

	dr_env->code[2]=env->clip.w<<16;
	dr_env->code[2]+=env->clip.h;

	dr_env->code[3]=env->ofs[0]<<16;
	dr_env->code[3]+=env->ofs[1];

	dr_env->code[4]=env->tw.x<<16;
	dr_env->code[4]+=env->tw.y;
	
	dr_env->code[5]=env->tw.w<<16;
	dr_env->code[5]=env->tw.h;

	dr_env->code[6]=env->tpage<<16;
	dr_env->code[6]+=env->dtd<<8;

	dr_env->code[7]=env->isbg<<24;

}
#endif //USE_ATI

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void		SetDrawMode(DR_MODE *dr_mode, int dfe, int dtd, unsigned short tpage, RECT *tw)
{
	// Initialises the draw mode primitive. 
	// The format below was designed by me (ajs) if you don't like it - design your own !

	// FORMAT FOR DR_MODE.CODE ARRAY
	// 1 byte  : dfe
	// 1 byte  : dtd
	// 2 bytes : tpage
	// 1 u_long: *tw 

	dr_mode->code[1] = (u_char)dfe<<24;
	dr_mode->code[1] += (u_char)dtd<<16;
	dr_mode->code[1] += tpage;
	dr_mode->code[0] = 0x90;			// set first byte (code byte) to 0x90
	dr_mode->code[0]=dr_mode->code[0]<<24;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef USE_ATI

int		LoadImage(RECT	*recp, unsigned long *p)
{
	/***** NOTE: THIS SHOULD NOT WORK APART FROM DIFFERENT SIZE TEXTURE PAGE COORDINATE *****/
	#ifdef NOT_A_HOPE_IN_HELL
	char	*dest;
	char	*source;

	char	tpage_start, tpage_end, tpage;
	char 	vert_split;		

	short src_start_off;
	short	x,y;
	short	width,height;	
	short	xs, xe, ys, ye;

	short	line;				
	char	*ptr;

	#define	TPAGE_WIDTH 64	// this is in WORDS, 'cause that's how the recp coord come in (I THINK!)

	printf("!!!!!!!!!!!!!       OLD LOADIMAGE        !!!!!!!!!!!!!!\n");
	// START OF CODE

	ptr=(char*)&VRam;			// pointer to start of our (steve's) VRAM

	x=recp->x;					// co-ordinates for original PS Vram
	y=recp->y;
	width=recp->w;
	height=recp->h;

	if (y>255)					// check to see if texture is split over texture pages vertically
		vert_split=0;			// i.e top half in texture pages 0-15, bottom half in corresponding
	else							// texture pages 16-32 : vert_split set to 1 if this is the case
		if ((y+height)>256)	// 0 otherwise
			vert_split=1;
		else
			vert_split=0;

	if (!vert_split)			// we're no split over texture pages vertically (easy case)
	{
		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;
		if (y>255)
		{
			tpage_start+=16;
			tpage_end+=16;
		}

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<y+height;line++)
			{
				dest=ptr+(tpage*65536)+xs+(line*256);
				source=(char*)p+src_start_off+(line*width);
	  //			memcpy(dest,source,xe-xs);
			}
			src_start_off += xe-xs;
		}
	}
	else			// split over texture pages vertically (awkward bastards!)
	{
		// FIRST STEP DO THE 0 TO 15 TOP HALF BIT THING WHATIST

		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<256;line++)
			{
				dest=ptr+(tpage*65536)+xs+(line*256);
				source=(char*)p+src_start_off+(line*width);
 //				memcpy(dest,source,xe-xs);
			}
			src_start_off += xe-xs;
		}

		// NOW DO BOTTOM HALF !!

		tpage_start+=16;
		tpage_end+=16;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=0;line<height-y;line++)
			{
				dest=ptr+(tpage*65536)+xs+(line*256);
				source=(char*)p+src_start_off+(line*width);

//				memcpy(dest,source,xe-xs);
			}
			src_start_off += xe-xs;
		}
	}
	#endif
	return(1);
}

#endif //USE_ATI


//*****************************************************************
#ifndef USE_ATI


int		LoadImage16(RECT	*recp, unsigned long *p, CLUT *cluts)
{
	/***** NOTE: THIS SHOULD WORK APART FROM DIFFERENT SIZE TEXTURE PAGE COORDINATE *****/
	char	*dest;
	u_short	*source;

	char	tpage_start, tpage_end, tpage;
	char 	vert_split;		

	short src_start_off;
	short	x,y;
	short	width,height;	
	short	xs, xe, ys, ye;
	char red, green, blue;
	u_short val;

	u_short *source_word;
	short	line;				
	char	*ptr;

	char	used[16];
	int 	loop_used;
	CLUT	*clutp;

	printf("!!!!!! LOAD IMAGE 16 !!!!!!\n");
	#define	TPAGE_WIDTH 64	// this is in WORDS, 'cause that's how the recp coord come in (I THINK!)

	// START OF CODE

	ptr=(char*)&VRam;			// pointer to start of our (steve's) VRAM

	x=recp->x;					// co-ordinates for original PS Vram
	y=recp->y;
	width=recp->w;
	height=recp->h;

	if (y>255)					// check to see if texture is split over texture pages vertically
		vert_split=0;			// i.e top half in texture pages 0-15, bottom half in corresponding
	else							// texture pages 16-32 : vert_split set to 1 if this is the case
		if ((y+height)>256)	// 0 otherwise
			vert_split=1;
		else
			vert_split=0;

	if (!vert_split)			// we're no split over texture pages vertically (easy case)
	{
		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;
		if (y>255)
		{
			tpage_start+=16;
			tpage_end+=16;
			y -= 256;
		}

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<y+height;line++)
			{
				dest=ptr+(tpage*65536)+(xs)+(line*256);				// destination in 4 bit co-ords 
				source=(u_short*)p+src_start_off+((line-y)*width);		// source in 16 bit co-ords  
				for (source_word = source; source_word <(source + (xe-xs));source_word++)
				{
					clutp = (CLUT *)source_word;
					val=(*source_word);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
				}
			}
			src_start_off += (xe-xs);									//64 coords is 128 bytes!
		}
	}
	else			// split over texture pages vertically (awkward bastards!)
	{
		// FIRST STEP DO THE 0 TO 15 TOP HALF BIT THING WHATIST

		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<256;line++)
			{
				dest=ptr+(tpage*65536)+xs+(line*256);
				source=(u_short*)p+src_start_off+((line-y)*(width));
				for (source_word = source; source_word <(source + (xe-xs));source_word++)
				{
					clutp = (CLUT *)source_word;
					val=(*source_word);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
				}
			}
			src_start_off += (xe-xs);
		}

		// NOW DO BOTTOM HALF !!

		tpage_start+=16;
		tpage_end+=16;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=0;line<height-(256-y);line++)      			//(256-y) is the amount drawn in the top half
			{
				dest=ptr+(tpage*65536)+(xs)+(line*256);
				source=(u_short*)p+src_start_off+((line+256-y)*(width));
				for (source_word = source; source_word <(source + (xe-xs) );source_word++)
				{
					clutp = (CLUT *)source_word;
					val=(*source_word);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
					*dest++ = GET_PALLETE(clutp->red << 3, clutp->green << 3, clutp->blue << 3);
				}
			}
			src_start_off += (xe-xs);
		}
	}


	return(1);
}
#endif //USE_ATI

//*****************************************************************




//*****************************************************************8

#ifndef USE_ATI

void				MoveImage(RECT	*rect, int x, int y)	// not used in RidgeRacer
{
	// The rectangular area of the frame buffer specified by rect is transmitted to the rectangular
	// area of the same size which starts at (x,y). The content of them source is preserves. If the
	// source and destination areas over lap, normal operation is not guaranteed

	#ifdef ajs
	u_short	count;
	u_long	*source;
	u_long	*dest;
	u_short	width;
	u_long	val;

	source=video_start_addr+(rect->x*2)+(rect->y*VRAM_WIDTH);
	dest=video_start_addr+(x*2)+(y*VRAM_WIDTH);
	width=rect->w*2;

	for (count=0;count<rect->h;count++)
	{
		val=count*VRAM_WIDTH;
		memcpy(dest+val,source+val,width);
	}
	#endif
}
#endif //USE_ATI

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		StoreImage(RECT	*recp, unsigned long *p)
{
	// Transfers the rectangular area in the frame bufffer specified by recp to the contiguous area
	// in main memory at address p

	#ifdef ajs
	u_short	count;
	u_long	*source;
	u_short	width;
	u_long	val;

	source=video_start_addr+(recp->x*2)+(recp->y*VRAM_WIDTH);
	width=recp->w*2;

	for (count=0;count<recp->h;count++)
	{
		val=count*VRAM_WIDTH;
		memcpy(p+val,source+val,recp->w);
	}
	#endif
}

void	ResetCallback()
{
	// don't you just love these empty functions
}

void SetBlockFill(BLK_FILL *p)
{
	setBlockFill(p);
}

void	SetPolyGT3(POLY_GT3 *p)
{
	setPolyGT3(p);
}

#ifdef EMERGENCY_BACKUP

long ratan2(long y, long x)
{
 return((long)((atan2((double) y, (double) x))* 652));	
}

void RotTransPersN(SVECTOR *v0,DVECTOR *v1,u_short *sz,u_short *flag,long n)
{

int loop;
SVECTOR *ip;
DVECTOR *op;
long	crap;
u_short *z;	
u_short *f;
long	long_flag=0;

// printf("\n\n!!!!!!!!! RotTransPersN !!!!!!!!!!!\n\n");

ip = v0;
op = v1;
z = sz;
f = flag;
crap= NULL;

	for (loop =0; loop < n; loop++)
	{
		*sz = (u_short)RotTransPers(ip++,(long *)op,&crap,&long_flag);
		sz++;
 		*flag = (short)(long_flag >> 12);
		flag++;
		op++;
	}
}

#endif


//*****************************************************************

void LoadClut4(RECT	*rect, u_short *clutptr, CLUT *cluts)
{
	int loop;
	u_short *ptr;

	ptr = (u_short *)cluts;
	for (loop =0;loop < 16 ;loop++)
	{
		*ptr++ = *clutptr++;
	}
}

void LoadClut8(RECT	*rect, u_short *clutptr, CLUT *cluts)
{
	int loop;
	u_short *ptr;

	ptr = (u_short *)cluts;
	for (loop =0;loop < 256 ;loop++)
	{
		*ptr++ = *clutptr++;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef LOADTEXTURES

int		LoadImage8(RECT	*recp, unsigned long *p, CLUT *cluts)
{
	/***** NOTE: THIS SHOULD WORK APART FROM DIFFERENT SIZE TEXTURE PAGE COORDINATE *****/
	char	*dest;
	char	*source;

	char	tpage_start, tpage_end, tpage;
	char 	vert_split;		

	short src_start_off;
	short	x,y;
	short	width,height;	
	short	xs, xe, ys, ye;
	char red, green, blue;
	char val;

	char *source_byte;
	short	line;				
	char	*ptr;

	char	used[16];
	int 	loop_used;

	printf("!!!!!! LOAD IMAGE 8 !!!!!!\n");
	#define	TPAGE_WIDTH 64	// this is in WORDS, 'cause that's how the recp coord come in (I THINK!)

	// START OF CODE

	ptr=(char*)&VRam;			// pointer to start of our (steve's) VRAM

	x=recp->x;					// co-ordinates for original PS Vram
	y=recp->y;
	width=recp->w;
	height=recp->h;

	if (y>255)					// check to see if texture is split over texture pages vertically
		vert_split=0;			// i.e top half in texture pages 0-15, bottom half in corresponding
	else							// texture pages 16-32 : vert_split set to 1 if this is the case
		if ((y+height)>256)	// 0 otherwise
			vert_split=1;
		else
			vert_split=0;

	if (!vert_split)			// we're no split over texture pages vertically (easy case)
	{
		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;
		if (y>255)
		{
			tpage_start+=16;
			tpage_end+=16;
			y -= 256;
		}

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<y+height;line++)
			{
				dest=ptr+(tpage*65536)+(xs*2)+(line*256);				// destination in 4 bit co-ords 
				source=(char*)p+src_start_off+((line-y)*(width*2));		// source in 16 bit co-ords  Width must be in bytes
				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)
				{
					val = (*source_byte) & 0xff;   		/*low 8 bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);

//					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
//											 (char)(cluts[val].green)<<3,
//											 (char)(cluts[val].blue)<<3);
				}
			}
			src_start_off += ((xe-xs) * 2);									//64 coords is 128 bytes!
		}
	}
	else			// split over texture pages vertically (awkward bastards!)
	{
		// FIRST STEP DO THE 0 TO 15 TOP HALF BIT THING WHATIST

		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<256;line++)
			{
				dest=ptr+(tpage*65536)+(xs * 2)+(line*256);
				source=(char*)p+src_start_off+((line-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) * 2));source_byte++) //*2 for 16bit pixel values
				{
					val = (*source_byte) & 0xff;   		/*low 8 bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);

//					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
//											 (char)(cluts[val].green)<<3,
//											 (char)(cluts[val].blue)<<3);
				}
			}
			src_start_off += ((xe-xs) *2);
		}

		// NOW DO BOTTOM HALF !!

		tpage_start+=16;
		tpage_end+=16;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=0;line<height-(256-y);line++)      			//(256-y) is the amount drawn in the top half
			{
				dest=ptr+(tpage*65536)+(xs * 2)+(line*256);
				source=(char*)p+src_start_off+((line+256-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)
				{
					val = (*source_byte) & 0xff;   		/*low 8 bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);

//					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
//											 (char)(cluts[val].green)<<3,
//											 (char)(cluts[val].blue)<<3);
				}
			}
			src_start_off += ((xe-xs) * 2);
		}
	}
	return(1);
}

//*****************************************************************


int		LoadImage4(RECT	*recp, unsigned long *p, CLUT *cluts)
{
 	char	*dest;
	char	*source;

	char	tpage_start, tpage_end, tpage;
	char 	vert_split;		

	short src_start_off;
	short	x,y;
	short	width,height;	
	short	xs, xe, ys, ye;
	char red, green, blue;
	char val;

	char *source_byte;
	short	line;				
	char	*ptr;

	char	used[16];
	int 	loop_used;

	#define	TPAGE_WIDTH 64	// this is in WORDS, 'cause that's how the recp coord come in (I THINK!)

	// START OF CODE

	ptr=(char*)&VRam;			// pointer to start of our (steve's) VRAM

	x=recp->x;					// co-ordinates for original PS Vram
	y=recp->y;
	width=recp->w;
	height=recp->h;

	if (y>255)					// check to see if texture is split over texture pages vertically
		vert_split=0;			// i.e top half in texture pages 0-15, bottom half in corresponding
	else							// texture pages 16-32 : vert_split set to 1 if this is the case
		if ((y+height)>256)	// 0 otherwise
			vert_split=1;
		else
			vert_split=0;

	if (!vert_split)			// we're no split over texture pages vertically (easy case)
	{
		tpage_start=x/TPAGE_WIDTH;				// get the starting texture page
		tpage_end=(x+width-1)/TPAGE_WIDTH;	// get the end texture page
		if (y>255)
		{
			tpage_start+=16;						// if in bottom half of VRam
			tpage_end+=16;
			y -= 256;
		}

		src_start_off = 0;						// the x co-ord of where we are in our source buffer
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;		 // starting x co-ord in texture page
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;	// end x co-ord in texture page
			else	xe=TPAGE_WIDTH;

			for (line=y;line<y+height;line++)
			{
				dest=ptr+(tpage*65536)+(xs*4)+(line*256);				// destination in 4 bit co-ords 
				source=(char*)p+src_start_off+((line-y)*(width*2));		// source in 16 bit co-ords  Width must be in bytes

				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)	// read in bytes for whole line
				{																					// converting from 4 bit to 8 bit on the way
					val = (*source_byte) & 0xf;   		/*low 4 bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);


					val = ((*source_byte) & 0xf0)>>4;  /*high four bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char) (cluts[val].green)<<3,
											 (char) (cluts[val].blue)<<3);
				}
			}
			src_start_off += ((xe-xs) * 2);									//64 coords is 128 bytes!	// increment the source start offset
		}
	}
	else			// split over texture pages vertically (awkward bastards!)
	{
		// FIRST STEP DO THE 0 TO 15 TOP HALF BIT THING WHATIST

		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<256;line++)
			{
				dest=ptr+(tpage*65536)+(xs * 4)+(line*256);
				source=(char*)p+src_start_off+((line-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) * 2));source_byte++) //*2 for 16bit pixel values
				{
					val = (*source_byte) & 0xf;   		/*low 4 bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);
					val = ((*source_byte) & 0xf0)>>4;  /*high four bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);
				}
			}
			src_start_off += ((xe-xs) *2);
		}

		// NOW DO BOTTOM HALF !!

		tpage_start+=16;
		tpage_end+=16;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=0;line<height-(256-y);line++)      			//(256-y) is the amount drawn in the top half
			{
				dest=ptr+(tpage*65536)+(xs * 4)+(line*256);
				source=(char*)p+src_start_off+((line+256-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)
				{
					val = (*source_byte) & 0xf;   		/*low 4 bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);
					val = ((*source_byte) & 0xf0)>>4;  /*high four bits*/
					*dest++ = GET_PALLETE((char)(cluts[val].red)<<3,
											 (char)(cluts[val].green)<<3,
											 (char)(cluts[val].blue)<<3);
				}

			}
			src_start_off += ((xe-xs) * 2);
		}
	}
	return(1);
}

#endif		// 16 bit dump stuff
#ifdef	LOADAS24BIT
int		LoadImage8(RECT	*recp, unsigned long *p, CLUT *cluts)
{
	char	*dest;
	char	*source;

	char	tpage_start, tpage_end, tpage;
	char 	vert_split;		

	short src_start_off;
	short	x,y;
	short	width,height;	
	short	xs, xe, ys, ye;
	char red, green, blue;
	char val;

	char *source_byte;
	short	line;				
	char	*ptr;

	char	used[16];
	int 	loop_used;

	printf("!!!!!! LOAD IMAGE 8 !!!!!!\n");
	#define	TPAGE_WIDTH 64	// this is in WORDS, 'cause that's how the recp coord come in (I THINK!)

	// START OF CODE

	ptr=(char*)&VRam;			// pointer to start of our (steve's) VRAM

	x=recp->x;					// co-ordinates for original PS Vram
	y=recp->y;
	width=recp->w;
	height=recp->h;

	if (y>255)					// check to see if texture is split over texture pages vertically
		vert_split=0;			// i.e top half in texture pages 0-15, bottom half in corresponding
	else							// texture pages 16-32 : vert_split set to 1 if this is the case
		if ((y+height)>256)	// 0 otherwise
			vert_split=1;
		else
			vert_split=0;

	if (!vert_split)			// we're no split over texture pages vertically (easy case)
	{
		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;
		if (y>255)
		{
			tpage_start+=16;
			tpage_end+=16;
			y -= 256;
		}

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<y+height;line++)
			{
				dest=(char*)(ptr+((long)tpage*196608)+((long)xs*6)+((long)line*768));						// 65536 = size of tpage, 2 = number of pixels per word of source, 256 = width of VRAM
//				dest=ptr+(tpage*65536)+(xs*2)+(line*256);						// 65536 = size of tpage, 2 = number of pixels per word of source, 256 = width of VRAM
				source=(char*)p+src_start_off+((line-y)*(width*2));		// source in 16 bit co-ords  Width must be in bytes
				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)
				{
					val = (*source_byte) & 0xff;   		/*low 8 bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3,
					*dest++ = (char)(cluts[val].blue)<<3;
				}
			}
			src_start_off += ((xe-xs) * 2);									//64 coords is 128 bytes!
		}
	}
	else			// split over texture pages vertically (awkward bastards!)
	{
		// FIRST STEP DO THE 0 TO 15 TOP HALF BIT THING WHATIST

		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<256;line++)
			{
				dest=(char*)(ptr+((long)tpage*196608)+((long)xs*6)+((long)line*768));						// 65536 = size of tpage, 2 = number of pixels per word of source, 256 = width of VRAM
//				dest=ptr+(tpage*65536)+(xs * 2)+(line*256);
				source=(char*)p+src_start_off+((line-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) * 2));source_byte++) //*2 for 16bit pixel values
				{
					val = (*source_byte) & 0xff;   		/*low 8 bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3,
					*dest++ = (char)(cluts[val].blue)<<3;
				}
			}
			src_start_off += ((xe-xs) *2);
		}

		// NOW DO BOTTOM HALF !!

		tpage_start+=16;
		tpage_end+=16;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=0;line<height-(256-y);line++)      			//(256-y) is the amount drawn in the top half
			{
				dest=(char*)(ptr+((long)tpage*196608)+((long)xs*6)+((long)line*768));						// 65536 = size of tpage, 2 = number of pixels per word of source, 256 = width of VRAM
//				dest=ptr+(tpage*65536)+(xs * 2)+(line*256);
				source=(char*)p+src_start_off+((line+256-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)
				{
					val = (*source_byte) & 0xff;   		/*low 8 bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3,
					*dest++ = (char)(cluts[val].blue)<<3;
				}
			}
			src_start_off += ((xe-xs) * 2);
		}
	}
	return(1);
}

//*****************************************************************

int		LoadImage4(RECT	*recp, unsigned long *p, CLUT *cluts)
{
 	char	*dest;
	char	*source;

	char	tpage_start, tpage_end, tpage;
	char 	vert_split;		

	short src_start_off;
	short	x,y;
	short	width,height;	
	short	xs, xe, ys, ye;
	char red, green, blue;
	char val;

	char *source_byte;
	short	line;				
	char	*ptr;

	char	used[16];
	int 	loop_used;

	#define	TPAGE_WIDTH 64	// this is in WORDS, 'cause that's how the recp coord come in (I THINK!)

	// START OF CODE

	ptr=(char*)&VRam;			// pointer to start of our (steve's) VRAM

	x=recp->x;					// co-ordinates for original PS Vram
	y=recp->y;
	width=recp->w;
	height=recp->h;

	if (y>255)					// check to see if texture is split over texture pages vertically
		vert_split=0;			// i.e top half in texture pages 0-15, bottom half in corresponding
	else							// texture pages 16-32 : vert_split set to 1 if this is the case
		if ((y+height)>256)	// 0 otherwise
			vert_split=1;
		else
			vert_split=0;

	if (!vert_split)			// we're no split over texture pages vertically (easy case)
	{
		tpage_start=x/TPAGE_WIDTH;				// get the starting texture page
		tpage_end=(x+width-1)/TPAGE_WIDTH;	// get the end texture page
		if (y>255)
		{
			tpage_start+=16;						// if in bottom half of VRam
			tpage_end+=16;
			y -= 256;
		}

		src_start_off = 0;						// the x co-ord of where we are in our source buffer
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;		 // starting x co-ord in texture page
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;	// end x co-ord in texture page
			else	xe=TPAGE_WIDTH;

			for (line=y;line<y+height;line++)
			{
				dest=(char*)(ptr+((long)tpage*196608)+((long)xs*12)+((long)line*768));						// 65536 = size of tpage, 2 = number of pixels per word of source, 256 = width of VRAM
//				dest=ptr+(tpage*65536)+(xs*4)+(line*256);				// destination in 4 bit co-ords 
				source=(char*)p+src_start_off+((line-y)*(width*2));		// source in 16 bit co-ords  Width must be in bytes

				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)	// read in bytes for whole line
				{																					// converting from 4 bit to 8 bit on the way
					val = (*source_byte) & 0xf;   		/*low 4 bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3,
					*dest++ = (char)(cluts[val].blue)<<3;


					val = ((*source_byte) & 0xf0)>>4;  /*high four bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3;
					*dest++ = (char)(cluts[val].blue)<<3;
				}
			}
			src_start_off += ((xe-xs) * 2);									//64 coords is 128 bytes!	// increment the source start offset
		}
	}
	else			// split over texture pages vertically (awkward bastards!)
	{
		// FIRST STEP DO THE 0 TO 15 TOP HALF BIT THING WHATIST

		tpage_start=x/TPAGE_WIDTH;
		tpage_end=(x+width-1)/TPAGE_WIDTH;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=y;line<256;line++)
			{
				dest=(char*)(ptr+((long)tpage*196608)+((long)xs*12)+((long)line*768));						// 65536 = size of tpage, 2 = number of pixels per word of source, 256 = width of VRAM
//				dest=ptr+(tpage*65536)+(xs * 4)+(line*256);
				source=(char*)p+src_start_off+((line-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) * 2));source_byte++) //*2 for 16bit pixel values
				{
					val = (*source_byte) & 0xf;   		/*low 4 bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3,
					*dest++ = (char)(cluts[val].blue)<<3;


					val = ((*source_byte) & 0xf0)>>4;  /*high four bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3;
					*dest++ = (char)(cluts[val].blue)<<3;
				}
			}
			src_start_off += ((xe-xs) *2);
		}

		// NOW DO BOTTOM HALF !!

		tpage_start+=16;
		tpage_end+=16;

		src_start_off = 0;
		for (tpage=tpage_start;tpage<=tpage_end;tpage++ )
		{
			if (tpage==tpage_start)	xs=x%TPAGE_WIDTH;
			else	xs=0;

			if (tpage==tpage_end)	xe=((x+width-1)%TPAGE_WIDTH)+1;
			else	xe=TPAGE_WIDTH;

			for (line=0;line<height-(256-y);line++)      			//(256-y) is the amount drawn in the top half
			{
				dest=(char*)(ptr+((long)tpage*196608)+((long)xs*12)+((long)line*768));						// 65536 = size of tpage, 2 = number of pixels per word of source, 256 = width of VRAM
//				dest=ptr+(tpage*65536)+(xs * 4)+(line*256);
				source=(char*)p+src_start_off+((line+256-y)*(width*2));
				for (source_byte = source; source_byte <(source + ((xe-xs) *2));source_byte++)
				{
					val = (*source_byte) & 0xf;   		/*low 4 bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3,
					*dest++ = (char)(cluts[val].blue)<<3;


					val = ((*source_byte) & 0xf0)>>4;  /*high four bits*/
					*dest++ = (char)(cluts[val].red)<<3;
					*dest++ = (char)(cluts[val].green)<<3;
					*dest++ = (char)(cluts[val].blue)<<3;
				}
			}
			src_start_off += ((xe-xs) * 2);
		}
	}
	return(1);
}

#endif
#ifdef BLANKFUNCTIONS

int		LoadImage4(RECT	*recp, unsigned long *p, CLUT *cluts)
{
	return(1);
}

int		LoadImage8(RECT	*recp, unsigned long *p, CLUT *cluts)
{
	return(1);
}
#endif


void LoadVRam(char *filename, char set_pal)
{
	bm			BM;
	char		*ptr;
//	char		*pall;

	char	fadefilename[_MAX_PATH];

	char	drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char	fname[_MAX_FNAME];

	char	*palptr;
	int		loop;

//	pall=(char*)malloc(768);
	

	BM.bitmap=(char*)&VRam;
	BM.width=256;
	BM.height=5632; // was 8192
	BM.palette=main_palette;
	ReadPCX(filename,&BM,3);	// 2 = PIC, 1 = PAL, 4= DEBUG 
	
	palptr = main_palette;

	for (loop = 0; loop < 256; loop++)
	  {
	  Palette[loop].r = *palptr++;
	  Palette[loop].g = *palptr++;
	  Palette[loop].b = *palptr++;
	  }

	if (set_pal)
	{
		current_palette=MAIN_PAL;
		CopyPal(main_palette,0,255);
	}


	shield_palette[0]=main_palette[0]; // colour 0 (r0,g0,b0) should not be faded to white
	shield_palette[1]=main_palette[1];
	shield_palette[2]=main_palette[2];
	for (loop = 3; loop < 768; loop++)
	{
		if (((short)main_palette[loop]+75)>255)
			shield_palette[loop]=255;
		else
			shield_palette[loop] = main_palette[loop]+75;
	}
	_splitpath(filename,drive,dir,fname,NULL);
	_makepath(fadefilename,"",dir,fname,".dpq");

	BM.width=256;
	BM.height=96;
	BM.bitmap=(char*)&DepthFadeTble; // PCwipeout added cast
	BM.palette=main_palette;
	ReadPCX(fadefilename,&BM,2);

//	free(pall);
}

void setWinLose(void)
{
	char	*ptr;
	short	i,j;

	if(!winLoseIn)
	{
		ptr=(char*)LockPilot();

		for (i=64,j=0;i<204;i++,j++) // 140 = pcx height // 7748 before removal of 10 blank tpages
		{
			memcpy(ptr+(i*512),winPic+(j*256),256); // do we really need the offset by 10
		}

		for (i=64,j=0;i<204;i++,j++) // 7704 before removal of 10 blank tpages
		{
			memcpy(ptr+(i*512)+256,losePic+(j*256),256);
		}
	
		UnlockPilot();
		winLoseIn = 1;
	}
}

void	LoadPilotIntoVram(char *filename)
{
	FILE	*file,*fp;
	char	picfile[256];

	strcpy( picfile, filename);
	strcat( picfile, "win.sjr");
	fp=fopen(picfile,"rb");
	fread(winPic,35840,1,fp);
	fclose(fp);

	strcpy( picfile, filename);
	strcat( picfile, "los.sjr");
	fp=fopen(picfile,"rb");
	fread(losePic,35840,1,fp);
	fclose(fp);
}

void	InitAJSVSync()
{
	last_frame=this_frame;
}
