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
extern	int32_t	inattract;//jed
extern	char	inmenu;	 //jed
extern	int32_t	mode;		 //jed

extern int32_t				WinPitch;
extern int16_t			WinWidth;
extern int16_t			WinHeight;
extern int16_t			WinHeightX2;
extern int32_t numoff,numsmall;
int32_t maxoff=0,maxsmall=0;


#define LET_STEVE_CLIP

// static uint32_t	_SCSize;
// static uint32_t	_SCAddress;
static int32_t dev = 0;


int32_t gfx_device;

char	halfx=0;	 						//jed
char	halfy=0;							//jed
float	halfxf=0.0;						//jed
float	halfyf=0.0;						//jed
char	pixdouble=0;					//jed

int32_t	screenw=0;						//jed
int32_t	screenh=0;						//jed

#define	Alloc(x,s)	malloc ((x)*(s))
#define	DeAlloc(p)	free ((p))

void JJS_Sprite(SPRT *sp);
void JJS_Sprite8(SPRT_8 *sp);
void AYDrawFT4(P_TAG *primitive);
void AYDrawF3(P_TAG *primitive);
void AYDrawG3(P_TAG *primitive);
void AYDrawGT3(P_TAG *primitive);
void AYDrawFT3(P_TAG *primitive);
void AYDrawF4(P_TAG *primitive);
void AYDrawG4(P_TAG *primitive);
void AYDrawGT4(P_TAG *primitive);
void sprite8(P_TAG *primtive);
void sprite(P_TAG *primtive);
void tile(P_TAG *primtive);
void ChangeDrawMode(P_TAG *primitive);
void MaskSprite(P_TAG *primitive);
void (*DrawFunctions[256])(P_TAG *);
void intarray();
void IntVga();
void NotValidPrimType(P_TAG *primitive);
extern void DrawPolyF3(uint32_t *);
extern void DrawPolyFT3(uint32_t *);
extern void DrawPolyF4(uint32_t *);
extern void DrawPolyFT4(uint32_t *);
extern void DrawPolyGT4(uint32_t *);

char *CurBuffer;
char *CurBufferJ;//jed
char	*tempbuffer;

extern	char *JRam;			//jed

uint32_t JJSaddr;
int16_t rendering=0;

void DrawOTag(P_TAG *primitive)
{
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

	while (primitive != nullptr)
	{
		if (primitive->code != 0) //Case of OT stuff
			DrawPrim(primitive);

		primitive = primitive->addr;
	}

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

void Set_Device(int32_t tmp)
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



void IntVga()
{
	DrawFunctions[0x20]=AYDrawF3;		// Poly_F3
	DrawFunctions[0x24]=AYDrawFT3;		// Poly_FT3
	DrawFunctions[0x30]=AYDrawG3;		// Poly_G3
	DrawFunctions[0x34]=AYDrawGT3;		// Poly_GT3
	DrawFunctions[0x28]=AYDrawF4;		// Poly_F4
	DrawFunctions[0x2c]=AYDrawFT4;		// Poly_FT4
	DrawFunctions[0x38]=AYDrawG4;		// Poly_G4
	DrawFunctions[0x3c]=AYDrawGT4;		// Poly_GT4
	DrawFunctions[0x60]=tile;			// Tile
	DrawFunctions[0x64]=sprite;		// Free sized sprite
	DrawFunctions[0x74]=sprite8;		// 8 sized sprite
	DrawFunctions[0x78]=MaskSprite;	// Single-colour sprite
	DrawFunctions[0x7c]=MaskSprite;	// Single-colour 8x8 sprite
	DrawFunctions[0x90]=ChangeDrawMode;// Changes in draw mode
}



void intarray()
{
	int32_t x;

	for (x=0;x<256;x++)
		DrawFunctions[x]=NotValidPrimType;
}

void DrawPrim(P_TAG *primitive)
{
	uint8_t func = (((P_TAG *)primitive)->code) & 0xfc;
	// if (func != 0) {
	// 	printf("DrawPrim() func=%x\n", func);
	// }
	DrawFunctions[func](primitive); //put 0xfc to mask out transparency and texture lighting
}



void NotValidPrimType(P_TAG *primitive)
{
		printf("Prim x%x will never be supported In Current Code\n", primitive->code);
}



#define GPU_printf printf

#define GET_PALLETE(a,b,c)	((a & 0xe0) | ((b & 0xe0) >>3) | ((c & 0xc0) >> 5))
//#define GET_PALLETE(r,g,b)	(((r+g+b)/192) << 6) | ((r & 0xc0) >> 2) | ((g & 0xc0) >> 4) | ((b & 0xc0) >> 6)



void AYDrawF3(P_TAG *primitive)
{
	JJSDrawPolyF3((POLY_F3 *)primitive);
}



void AYDrawFT3(P_TAG *primitive)
{
	JJSDrawPolyFT3((POLY_FT3 *)primitive);
}



void AYDrawG3(P_TAG *primitive)
{
	JJSDrawPolyG3((POLY_G3 *)primitive);
}



void AYDrawGT3(P_TAG *primitive)
{
	JJSDrawPolyGT3((POLY_GT3 *)primitive);
}

void sprite(P_TAG *primitive)
{
	SPRT 			*ip;
	SPRT 			op;
	extern int16_t	drawenv_tpage;

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



void sprite8(P_TAG *primitive)
{
	SPRT_8 *ip;
	SPRT_8	op;
	extern int16_t	drawenv_tpage;

	ip = (SPRT_8 *)primitive;

	op.x0 = ip->x0;
	op.y0 = ip->y0;

	op.u0 = ip->u0;
	op.v0 = ip->v0;

	op.clut = drawenv_tpage;
	JJS_Sprite8(&op);
}



void AYDrawF4(P_TAG *primitive)
{
	JJSDrawPolyF4((POLY_F4 *)primitive);
}



void AYDrawFT4(P_TAG *primitive)
{
	JJSDrawPolyFT4((POLY_FT4 *)primitive);
}



void AYDrawGT4(P_TAG *primitive)
{
	JJSDrawPolyGT4((POLY_GT4 *)primitive);
}



void AYDrawG4(P_TAG *primitive)
{
	JJSDrawPolyG4((POLY_G4 *)primitive);
}



void tile(P_TAG *primitive)
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
	*(int32_t *)&op.r0 = *(int32_t *)&ip->r0;
	//	op.r0 = GET_PALLETE(ip->r0, ip->g0,ip->b0); //Now Set Before Here

	JJSDrawPolyF4(&op);

}

void ChangeDrawMode(P_TAG *primitive)
{
	extern int16_t	drawenv_tpage;
	DR_MODE			*temp;


	temp = (DR_MODE *)primitive;
	drawenv_tpage = temp->code[1] & 0x0000FFFF;
}


void MaskSprite(P_TAG *primitive)
{
	SPRT 			*ip;
	SPRT 			op;
	extern int16_t	drawenv_tpage;

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
