//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Some 'PlayStation on the PC' functions
// Andy Satterthwaite, March / April 1995
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// OK these are the finished functions I've written. Hope it all works ...
// There may be problems in LoadImage, StoreImage and MoveImage 'cause I haven't tested them
// they look ok though.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// typedefs to get includes working under watcom

typedef unsigned char	u_char;
typedef unsigned short	u_short;
typedef unsigned int	u_int;
typedef unsigned long	u_long;


// The playstation includes

#include <libgte.h>
#include <libgpu.h>	// with function prototypes to be written by me removed


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

void		InterpolByte(u_char *in1, u_char *in2, long p0, u_char *rtn)
{
		// undocumented gte function which returns the difference between 2
		// lots of 2 bytes based on the 12 point fixed point number p0
	
		float	p1;
		
		p1=(4096 - (float)p0) / 4096;

		rtn[0]=((in2[0]-in1[0])*p1)+in1[0];
		rtn[1]=((in2[1]-in1[1])*p1)+in1[1];
}

int		DrawSync(int	mode)
{
	// function used for parallel actions on PSX
	// defined so games compile on PC
	return(1);
}

void	DrawSyncCallback(func)
void	(*func)();
{
	// function used for parallel actions on PSX
	// defined so games compile on PC
}


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

void	AddPrim(u_long *ot, u_long *p)
{
	addPrim(ot, p);
}

void	AddPrims(u_long *ot, u_long *ps, u_long *pe)
{
	addPrims(ot, ps, pe);
}
void	CatPrim(u_long *p0, u_long *p1)
{
	catPrim(p0, p1);
}

void	DumpClut(u_short clut)
{
	dumpClut(clut);
}

void	DumpTPage(u_short tpage)
{
	dumpTPage(tpage);
}

u_short	GetClut(int x, int y)
{
	return(getClut(x,y));
}

u_short GetTPage(int tp, int abr, int x, int y)
{
	return(getTPage(tp,abr,x,y));
}

u_long *NextPrim(u_long *p)
{
	return(nextPrim(p));
}

void SetLineF2(LINE_F2 *p)
{
	setLineF2(p);
}

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

void SetPolyGT(POLY_GT3 *p)
{
	setPolyGT(p);
}

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

void SetSprt8(SPRT_8  *p)
{
	setSprt8(p);
}

void SetSprt16(SPRT_16 *p)
{
	setSprt16(p);
}

void SetSprt(SPRT    *p)
{
	setSprt(p);
}

void SetTile1(TILE_1  *p)
{
	setTile1(p);
}

void SetTile8(TILE_8  *p)
{
	setTile8(p);
}

void SetTile16(TILE_16 *p)
{
	setTile16(p);
}

void SetTile(TILE    *p)
{
	setTile(p);
}

void TermPrim(u_long *p)
{
	termPrim(p);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void				SetDispMask(int mask)
{
	// Puts display mask into the status specified by mask. The following values can be used for mask
	//	0 - Not displayed on screen
	// 1 - Displayed on screen

	g_mask=mask;
}

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

void SetDrawEnv(DR_ENV *dr_env, DRAWENV *env)
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

	dr_env->code[0]=0xff<<24;
	dr_env->code[0]+=env->r0<<16;
	dr_env->code[0]+=env->g0<<8;
	dr_env->code[0]+=env->b0;


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

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void SetDrawMode(DR_MODE *dr_mode, int dfe, int dtd, unsigned short tpage, RECT *tw)
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
	dr_mode->code[0] = 0x90000000;			// set first byte (code byte) to 0x90

//	dr_mode->code[2]=(u_long)tw;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void		LoadImage(RECT	*recp, unsigned long *p)
{
	// Transfers the contents of memory from address p to the rectangular area in the frame
	// buffer specified by recp

	u_short	count;
	u_long	*dest;
	u_short	width;
	u_long	val;

	dest=video_start_addr+(recp->x*2)+(recp->y*VRAM_WIDTH);
	width=recp->w*2;

	for (count=0;count<recp->h;count++)
	{
		val=count*VRAM_WIDTH;
		memcpy(dest+val,p+val,width);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void MoveImage(RECT	*rect, int x, int y)	// not used in RidgeRacer
{
	// The rectangular area of the frame buffer specified by rect is transmitted to the rectangular
	// area of the same size which starts at (x,y). The content of them source is preserves. If the
	// source and destination areas over lap, normal operation is not guaranteed

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
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StoreImage(RECT	*recp, unsigned long *p)
{
	// Transfers the rectangular area in the frame bufffer specified by recp to the contiguous area
	// in main memory at address p

	u_short	count;
	u_long	*source;
 	u_short	width; /*this function will never work*/
	u_long	val;

	source=video_start_addr+(recp->x*2)+(recp->y*VRAM_WIDTH);
	width=recp->w*2;

	for (count=0;count<recp->h;count++)
	{
		val=count*VRAM_WIDTH;
		memcpy(p+val,source+val,recp->w);
	}
}


