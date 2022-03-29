#include "wipesrc/psx26/include/sys/types.h"
#include "wipesrc/psx26/include/libgte.h"
#include "wipesrc/psx26/include/libgpu.h"
//#include "cgl.h"				//jed
//#include <graph.h>
// #include <conio.h>
#include <stdio.h>
//#include <stdlib.h>
// #include <malloc.h>
#include <ctype.h>
#include "pcgraph.h"
//#include	<graph.h>
#include	<math.h>
#include "winh.h"
#include "drawot.h"
#include "win.h"

#define VGA		0
#define EAGLE	1
#define WATCOM 2

extern	char	Reallyinmenu;	//	jed & dave
extern	int	inattract;//jed
extern	char	inmenu;	 //jed
extern	int	mode;		 //jed

extern int				WinPitch;
extern short			WinWidth;
extern short			WinHeight;
extern short			WinHeightX2;
extern long numoff,numsmall;
long maxoff=0,maxsmall=0;


#define LET_STEVE_CLIP

// static unsigned long	_SCSize;
// static unsigned long	_SCAddress;
static int dev = 0;


int gfx_device;

char	halfx=0;	 						//jed
char	halfy=0;							//jed
float	halfxf=0.0;						//jed
float	halfyf=0.0;						//jed
char	pixdouble=0;					//jed

int	screenw=0;						//jed
int	screenh=0;						//jed

#define	Alloc(x,s)	malloc ((x)*(s))
#define	DeAlloc(p)	free ((p))

void JJS_Sprite(SPRT *sp);
void JJS_Sprite8(SPRT_8 *sp);
void AYDrawFT4(u_long *primitive);
void AYDrawF3(u_long *primitive);
void AYDrawG3(u_long *primitive);
void AYDrawGT3(u_long *primitive);
void AYDrawFT3(u_long *primitive);
void AYDrawF4(u_long *primitive);
void AYDrawG4(u_long *primitive);
void AYDrawGT4(u_long *primitive);
void sprite8(u_long *primtive);
void sprite(u_long *primtive);
void tile(u_long *primtive);
void ChangeDrawMode(u_long *primitive);
void MaskSprite(u_long *primitive);
void (*DrawFunctions[256])(u_long *);
void intarray(void);
void IntVga(void);
void NotValidPrimType(u_long *primitive);
extern void DrawPolyF3(u_long *);
extern void DrawPolyFT3(u_long *);
extern void DrawPolyF4(u_long *);
extern void DrawPolyFT4(u_long *);
extern void DrawPolyGT4(u_long *);

char *CurBuffer;
char *CurBufferJ;//jed
char	*tempbuffer;

extern	char *JRam;			//jed

unsigned long JJSaddr;
short rendering=0;

void DrawOTag(P_TAG *primitive)
{
	unsigned long addr;

#if	DEBUG_LOCKED
	CurBuffer = DebugScreen;
#else

	while (LockBackBuffer())
	{
		ProcessMessages(); 	//loop until we can get that back buffer
		;
	}
	UnlockBackBuffer();	//Finished drawing
#endif
	CurBufferJ=CurBuffer;						//jed
	tempbuffer=CurBuffer;
	setRenderBegin();
	numoff=0;
	numsmall=0;
	addr = (unsigned long) primitive;

	if (addr != 0xffffffff) do
	{
		if (((P_TAG *) addr)->code != 0) //Case of OT stuff
			DrawPrim((unsigned long *) addr);

		primitive = (P_TAG *) addr;
	}
	while ((addr = getaddr(primitive)) != 0xffffffff);

#if DEBUG_LOCKED
	while (LockBackBuffer())
	{
		ProcessMessages(); 	//loop until we can get that back buffer
		;
	}
#endif
	sendList();
	setRenderEnd();
//	UnlockBackBuffer();	//Finished drawing

	if(numoff>maxoff)
		maxoff=numoff;
	if(numsmall>maxsmall)
		maxsmall=numsmall;
}

void Set_Device(int tmp)
{
	if (dev==0)
	{
		dev++;
		gfx_device=tmp;
		intarray();
		//		SetVMode();
		IntVga();
		//		MakePal332();
	}
}



void IntVga(void)
{
	DrawFunctions[0x20]=*AYDrawF3;		// Poly_F3
	DrawFunctions[0x24]=*AYDrawFT3;		// Poly_FT3
	DrawFunctions[0x30]=*AYDrawG3;		// Poly_G3
	DrawFunctions[0x34]=*AYDrawGT3;		// Poly_GT3
	DrawFunctions[0x28]=*AYDrawF4;		// Poly_F4
	DrawFunctions[0x2c]=*AYDrawFT4;		// Poly_FT4
	DrawFunctions[0x38]=*AYDrawG4;		// Poly_G4
	DrawFunctions[0x3c]=*AYDrawGT4;		// Poly_GT4
	DrawFunctions[0x60]=*tile;			// Tile
	DrawFunctions[0x64]=*sprite;		// Free sized sprite
	DrawFunctions[0x74]=*sprite8;		// 8 sized sprite
	DrawFunctions[0x78]=*MaskSprite;	// Single-colour sprite
	DrawFunctions[0x7c]=*MaskSprite;	// Single-colour 8x8 sprite
	DrawFunctions[0x90]=*ChangeDrawMode;// Changes in draw mode
}



void intarray(void)
{
	int x;


	for (x=0;x<256;x++)
		DrawFunctions[x]=*NotValidPrimType;
}

void DrawPrim(u_long *primitive)
{
	static int i;
	if ((((P_TAG *)primitive)->code & 0xfc)== 0x24)
	{
		i++;
	}
	DrawFunctions[(((P_TAG *)primitive)->code) & 0xfc](primitive); //put 0xfc to mask out transparency and texture lighting
}



void NotValidPrimType(u_long *primitive)
{
	//	printf("Prim x%x will never be supported In Current Code\n",((P_TAG *)primitive)->code);
}



#define GPU_printf printf

#define GET_PALLETE(a,b,c)	((a & 0xe0) | ((b & 0xe0) >>3) | ((c & 0xc0) >> 5))
//#define GET_PALLETE(r,g,b)	(((r+g+b)/192) << 6) | ((r & 0xc0) >> 2) | ((g & 0xc0) >> 4) | ((b & 0xc0) >> 6)



void AYDrawF3(u_long *primitive)
{
	POLY_F3	*poly;
	POLY_F4 fred;

	poly = (POLY_F3 *)primitive;

	fred.x0 = poly->x0;
	fred.x1 = poly->x1;
	fred.x2 = poly->x2;
	fred.x3 = poly->x2;

	fred.y0 = poly->y0;
	fred.y1 = poly->y1;
	fred.y2 = poly->y2;
	fred.y3 = poly->y2;

	//jed
	fred.r0 = poly->r0;
	fred.g0 = poly->g0;
	fred.b0 = poly->b0;
	JJSDrawPolyF3(&fred);
}



void AYDrawFT3(u_long *primitive)
{
	POLY_FT3	*poly;
	POLY_FT3	fred;
	poly = (POLY_FT3 *)primitive;

	fred = *poly;
	//jed
	JJSDrawPolyFT3(&fred);
}



void AYDrawG3(u_long *primitive)
{
	POLY_G3	*poly;
	POLY_G3 fred;

	poly = (POLY_G3 *)primitive;

	fred = *poly;
	//jed
	JJSDrawPolyG3(&fred);
}



void AYDrawGT3(u_long *primitive)
{
	POLY_GT3	*poly;
	POLY_GT3	fred;

	poly = (POLY_GT3 *)primitive;

	fred = *poly;
	JJSDrawPolyGT3(&fred);
}

void sprite(u_long *primitive)
{
	SPRT 			*ip;
	SPRT 			op;
	extern short	drawenv_tpage;

	ip = (SPRT *)primitive;

	op.x0 = ip->x0;
	op.y0 = ip->y0;
	op.u0 = ip->u0;
	op.v0 = ip->v0;
	op.w = ip->w;
	op.h = ip->h;
	op.r0 = ip->r0;
	op.g0 = ip->g0;
	op.b0 = ip->b0;

	op.clut = drawenv_tpage;
	JJS_Sprite(&op);
}



void sprite8(u_long *primitive)
{
	SPRT_8 *ip;
	SPRT_8	op;
	extern short	drawenv_tpage;

	ip = (SPRT_8 *)primitive;

	op.x0 = ip->x0;
	op.y0 = ip->y0;

	op.u0 = ip->u0;
	op.v0 = ip->v0;

	op.clut = drawenv_tpage;
	JJS_Sprite8(&op);
}



void AYDrawF4(u_long *primitive)
{
	POLY_F4	*poly;
	POLY_F4	fred;


	poly = (POLY_F4 *)primitive;

	fred = *poly;
	JJSDrawPolyF4(&fred);
}



void AYDrawFT4(u_long *primitive)
{
	POLY_FT4	*poly;

	poly = (POLY_FT4 *)primitive;

	JJSDrawPolyFT4(poly);
}



void AYDrawGT4(u_long *primitive)
{
	POLY_GT4	*poly;

	poly = (POLY_GT4 *)primitive;

	JJSDrawPolyGT4(poly);
}



void AYDrawG4(u_long *primitive)
{
	POLY_G4	*poly;
	POLY_G4 fred;

	poly = (POLY_G4 *)primitive;

	fred = *poly;
	JJSDrawPolyG4(&fred);
}



void tile(u_long *primitive)
{
	TILE 	*ip;
	POLY_F4	op;

	ip->w *=2;
	ip->h *=2;
	ip = (TILE *)primitive;
	op.x0 = ip->x0*2;
	op.x1 = ip->x0 + ip->w;
	op.x2 = ip->x0*2;
	op.x3 = ip->x0 + ip->w;
	op.y0 = ip->y0*2;
	op.y1 = ip->y0 *2;
	op.y2 = ip->y0 + ip->h;
	op.y3 = ip->y0 + ip->h;
	*(long *)&op.r0 = *(long *)&ip->r0;
	//	op.r0 = GET_PALLETE(ip->r0, ip->g0,ip->b0); //Now Set Before Here

	JJSDrawPolyF4(&op);

}

void ChangeDrawMode(u_long *primitive)
{
	extern short	drawenv_tpage;
	DR_MODE			*temp;


	temp = (DR_MODE *)primitive;
	drawenv_tpage = temp->code[1] & 0x0000FFFF;
}


void MaskSprite(u_long *primitive)
{
	SPRT 			*ip;
	SPRT 			op;
	extern short	drawenv_tpage;

	ip = (SPRT *)primitive;

	op.x0 = ip->x0;
	op.y0 = ip->y0;
	op.u0 = ip->u0;
	op.v0 = ip->v0;

	if(ip->code == 0x78)
	{
		op.w = ip->w;
		op.h = ip->h;
	}
	else
	{
		op.w = 8;
		op.h = 8;
	}

	op.clut = drawenv_tpage;
	op.r0 = ip->r0;
	sendList();
	setRenderEnd();
}
