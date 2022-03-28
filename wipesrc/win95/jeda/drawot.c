#include <sys\types.h>
#include <libgte.h>
#include <libgpu.h>
//#include "cgl.h"				//jed
#include <graph.h>
#include <conio.h>
#include <stdio.h>
//#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>	  
#include "..\andyy\pcgraph.h"
#include	<graph.h>
#include	<math.h>
#include "..\win\winh.h"

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

extern void	DoubleX(unsigned char*,unsigned char*,int);	//jed
#pragma aux DoubleX=												\
"DoubleXLoop:	mov	cl,[eax]"								\
		"mov	ch,[ebx]"											\
		"cmp	ch,0"													\
		"jne	SkipIt"												\
		"mov	[ebx],cl"											\
"SkipIt:	mov	ch,[ebx+1]"										\
		"cmp	ch,0"													\
		"jne	SkipIt1"												\
		"mov	[ebx+1],cl"											\
"SkipIt1:	add	eax,1"										\
		"add	ebx,2"												\
		"sub	dx,1"													\
		"cmp	dx,0"													\
		"jne	DoubleXLoop"										\
		parm	[eax] [ebx] [dx]									\
		modify	[eax ebx ecx edx];

extern void	CopyLine(unsigned char*,unsigned char*,int);	//jed
#pragma aux CopyLine=											\
"CopyLoop:	mov	cl,[eax]"									\
		"mov	ch,[ebx]"											\
		"cmp	ch,0"													\
		"jne	SkipIt2"												\
		"mov	[ebx],cl"											\
"SkipIt2:	add	eax,1"										\
		"add	ebx,1"												\
		"sub	dx,1"													\
		"cmp	dx,0"													\
		"jne	CopyLoop"											\
		parm	[eax] [ebx] [dx]									\
		modify	[eax ebx ecx edx];

//						 Source ?       Dest ?	 Width
extern void	DoubleXSpesh(unsigned char*,unsigned char*,int);	//jed
#pragma aux DoubleXSpesh=												\
"DoubleXLoopS:	mov	cl,[eax]"								\
		"mov	ch,[ebx]"											\
		"cmp	cl,0"													\
		"je		SkipItS"												\
		"mov	[ebx],cl"											\
		"mov	[ebx+1],cl"											\
"SkipItS:	add	eax,1"										\
		"add	ebx,2"												\
		"sub	dx,1"													\
		"cmp	dx,0"													\
		"jne	DoubleXLoopS"										\
		parm	[eax] [ebx] [dx]									\
		modify	[eax ebx ecx edx];

extern void	CopyLineSpesh(unsigned char*,unsigned char*,int);	//jed
#pragma aux CopyLineSpesh=											\
"CopyLoopS:	mov	cl,[eax]"									\
		"mov	ch,[ebx]"											\
		"cmp	cl,0"													\
		"je		SkipItS2"												\
		"mov	[ebx],cl"											\
"SkipItS2:	add	eax,1"										\
		"add	ebx,1"												\
		"sub	dx,1"													\
		"cmp	dx,0"													\
		"jne	CopyLoopS"											\
		parm	[eax] [ebx] [dx]									\
		modify	[eax ebx ecx edx];



#pragma aux SetVMode modify [eax ebx ecx edx edi esi];
#pragma aux DrawPolyF3 parm [edi] modify [eax ebx ecx edx edi esi];
#pragma aux DrawPolyFT3 parm [edi] modify [eax ebx ecx edx edi esi];
#pragma aux DrawPolyF4 parm [edi] modify [eax ebx ecx edx edi esi];
#pragma aux DrawPolyFT4 parm [edi] modify [eax ebx ecx edx edi esi];
#pragma aux DrawPolyGT4 parm [edi] modify [eax ebx ecx edx edi esi];
#pragma aux S_Sprite parm [edi];
#pragma aux S_Sprite8 parm [edi];
#pragma aux S_Sprite16 parm [edi];
#pragma aux Mask parm [edi];
#pragma aux GSwap modify [eax ebx ecx edx edi esi];
#pragma aux Pen parm [eax] [ecx] [edx] [ebx] modify exact [eax ebx ecx edx esi edi];
#pragma aux ClearBuf parm [al]
#pragma aux Randy parm [eax] modify exact [ebx edx];

#define LET_STEVE_CLIP

static unsigned long	_SCSize;
static unsigned long	_SCAddress;
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
void IntArray();
void NotValidPrimType(u_long *primitive);
extern void DrawPolyF3(u_long *);
extern void DrawPolyFT3(u_long *);
extern void DrawPolyF4(u_long *);
extern void DrawPolyFT4(u_long *);
extern void DrawPolyGT4(u_long *);

extern	char *CurBuffer;
extern	char *CurBufferJ;//jed
char	*tempbuffer;

extern	char *JRam;			//jed

struct	xycoord	points[4] = {0,0, 0,0, 0,0, 0,0};

unsigned long JJSaddr;
short rendering=0;

void StartOTag(P_TAG *primitive)
{
	while (LockBackBuffer())
		ProcessMessages(); 	//loop until we can get that back buffer

	CurBufferJ=CurBuffer;
	tempbuffer=CurBuffer;
	SetContext();
	JJSaddr = (unsigned long) primitive;
	if(JJSaddr != 0xffffffff)
		rendering=1;
}

void ContinueOTag(void)
{
	P_TAG *primitive;

	if (JJSaddr != 0xffffffff)
		while(fifospace()>10)
		{
			if (((P_TAG *) JJSaddr)->code != 0) //Case of OT stuff
				DrawPrim((unsigned long *) JJSaddr);
	
			primitive = (P_TAG *) JJSaddr;
			if((JJSaddr = getaddr(primitive)) == 0xffffffff)
			{
				sendList();
				setRenderEnd();
				rendering=0;
				break;
			}
		}
		
}

void EndOTag(void)
{
	while(rendering)
	{
		ContinueOTag();
	}
	UnlockBackBuffer();	//Finished drawing
}

void DrawOTag(P_TAG *primitive)
{
	unsigned long addr;
	long		*testPtr;
	int			loop;

#if	DEBUG_LOCKED
	CurBuffer = DebugScreen;
#else

   while (LockBackBuffer())
   {
   ProcessMessages(); 	//loop until we can get that back buffer
	;
   }
   //UnlockBackBuffer();	//Finished drawing
#endif
	CurBufferJ=CurBuffer;						//jed
	tempbuffer=CurBuffer;
#if USE_ATI
	SetContext();
	setRenderBegin();
#endif
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
#if USE_ATI
	sendList();
	setRenderEnd();
#endif
   UnlockBackBuffer();	//Finished drawing

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



void IntVga()
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



void intarray()
{
	int x,y;


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
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
			fred.x3 = poly->x2/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
			fred.y3 = poly->y2/halfyf;
		}
	}
//jed	
#endif
	fred.r0 = poly->r0;
	fred.g0 = poly->g0;
	fred.b0 = poly->b0;
#if USE_ATI
	JJSDrawPolyF3(&fred);
#else
	DrawPolyF4((u_long*)&fred);
#endif
}



void AYDrawFT3(u_long *primitive)
{
	POLY_FT3	*poly;
#if USE_ATI
	POLY_FT3	fred;
#else
	POLY_FT4	fred;
#endif
	poly = (POLY_FT3 *)primitive;

#if USE_ATI
	fred = *poly;
//jed
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0  = poly->x0/halfxf;
			fred.x1  = poly->x1/halfxf;
			fred.x2  = poly->x2/halfxf;
		}	
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0  = poly->y0/halfyf;
			fred.y1  = poly->y1/halfyf;
			fred.y2  = poly->y2/halfyf;
		}
	}
//jed
#endif
	JJSDrawPolyFT3(&fred);
#else
	fred.x0  = poly->x0;
	fred.x1  = poly->x1;
	fred.x2  = poly->x2;
	fred.x3  = poly->x2;

	fred.y0  = poly->y0;
	fred.y1  = poly->y1;
	fred.y2  = poly->y2;
	fred.y3  = poly->y2;

//jed
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0  = poly->x0/halfxf;
			fred.x1  = poly->x1/halfxf;
			fred.x2  = poly->x2/halfxf;
			fred.x3  = poly->x2/halfxf;
		}	
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0  = poly->y0/halfyf;
			fred.y1  = poly->y1/halfyf;
			fred.y2  = poly->y2/halfyf;
			fred.y3  = poly->y2/halfyf;
		}
	}
//jed
#endif
	fred.u0 = poly->u0;
	fred.u1 = poly->u1;
	fred.u2 = poly->u2;
	fred.u3 = poly->u2;

	fred.v0 = poly->v0;
	fred.v1 = poly->v1;
	fred.v2 = poly->v2;
	fred.v3 = poly->v2;

	fred.clut = poly->clut;
	fred.tpage = poly->tpage;

	fred.r0 = poly->b0;
	fred.g0 = poly->g0;
	fred.b0 = poly->b0;

	DrawPolyFT4((u_long*)&fred);
#endif
}



void AYDrawG3(u_long *primitive)
{
	POLY_G3	*poly;
#if USE_ATI
	POLY_G3 fred;
#else
	POLY_F4	fred;
#endif

	poly = (POLY_G3 *)primitive;

#if USE_ATI
	fred = *poly;
//jed
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
		}
	}
//jed
#endif
	JJSDrawPolyG3(&fred);
#else
	fred.x0 = poly->x0;
	fred.x1 = poly->x1;
	fred.x2 = poly->x2;
	fred.x3 = poly->x2;

	fred.y0 = poly->y0;
	fred.y1 = poly->y1;
	fred.y2 = poly->y2;
	fred.y3 = poly->y2;

#if 0
//jed
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
			fred.x3 = poly->x2/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
			fred.y3 = poly->y2/halfyf;
		}
	}
//jed
#endif
	fred.r0 = poly->r0;
	fred.g0 = poly->g0;
	fred.b0 = poly->b0;

	setPolyF4(&fred);

	DrawPolyF4((u_long*)&fred);
#endif
}



void AYDrawGT3(u_long *primitive)
{
	POLY_GT3	*poly;
#if USE_ATI
	POLY_GT3	fred;
#else
	POLY_FT4	fred;
#endif

	poly = (POLY_GT3 *)primitive;

#if USE_ATI
	fred = *poly; 
//jed
#if 0
	if (inattract==0)
	{	
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
		}
	}

#endif
//jed
	JJSDrawPolyGT3(&fred);
#else    
	fred.x0 = poly->x0;
	fred.x1 = poly->x1;
	fred.x2 = poly->x2;
	fred.x3 = poly->x2;
	
	fred.y0 = poly->y0;
	fred.y1 = poly->y1;
	fred.y2 = poly->y2;
	fred.y3 = poly->y2;

//jed
#if 0
	if (inattract==0)
	{	
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
			fred.x3 = poly->x2/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
			fred.y3 = poly->y2/halfyf;
		}
	}

#endif
//jed
	fred.u0 = poly->u0;
	fred.u1 = poly->u1;
	fred.u2 = poly->u2;
	fred.u3 = poly->u2;

	fred.v0 = poly->v0;
	fred.v1 = poly->v1;
	fred.v2 = poly->v2;
	fred.v3 = poly->v2;

	fred.clut = poly->clut;
	fred.tpage = poly->tpage;

	fred.r0 = poly->r0;
	fred.g0 = poly->b0;
	fred.b0 = poly->g0;

	setPolyFT4(&fred);
	DrawPolyFT4((u_long*)&fred);
#endif
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
#if USE_ATI
//	sendList();
//	setRenderEnd();
	JJS_Sprite(&op);
#else
	S_Sprite(&op);
#endif
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
#if USE_ATI
//	sendList();
//	setRenderEnd();
	JJS_Sprite8(&op);
#else
	S_Sprite8(&op);
#endif
}



void AYDrawF4(u_long *primitive)
{
	POLY_F4	*poly;
	POLY_F4	fred;


	poly = (POLY_F4 *)primitive;

#if USE_ATI
	fred = *poly;
#else
	fred.x0 = poly->x0;
	fred.x1 = poly->x1;
	fred.x2 = poly->x2;
	fred.x3 = poly->x3;

	fred.y0 = poly->y0;
	fred.y1 = poly->y1;
	fred.y2 = poly->y2;
	fred.y3 = poly->y3;	

#endif
//jed
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
			fred.x3 = poly->x3/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
			fred.y3 = poly->y3/halfyf;	
		}
	}

//jed
#endif
#if USE_ATI
	JJSDrawPolyF4(&fred);
#else
	*(long *)&fred.r0 = *(long *)&poly->r0;

	setPolyF4(&fred);
	DrawPolyF4((u_long*)&fred);
#endif
}



void AYDrawFT4(u_long *primitive)
{
	POLY_FT4	*poly;

	poly = (POLY_FT4 *)primitive;

//jed
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			poly->x0=poly->x0/halfxf;
			poly->x1=poly->x1/halfxf;
			poly->x2=poly->x2/halfxf;
			poly->x3=poly->x3/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			poly->y0=poly->y0/halfyf;
			poly->y1=poly->y1/halfyf;
			poly->y2=poly->y2/halfyf;
			poly->y3=poly->y3/halfyf;
		}
	}
#endif
//jed
#if USE_ATI
	JJSDrawPolyFT4(poly);
#else
	setPolyFT4(poly);
	DrawPolyFT4((u_long*)poly);
#endif
}



void AYDrawGT4(u_long *primitive)
{
	POLY_GT4	*poly;
	POLY_FT4	fred;

	poly = (POLY_GT4 *)primitive;

	fred.x0 = poly->x0;
	fred.x1 = poly->x1;
	fred.x2 = poly->x2;
	fred.x3 = poly->x3;

	fred.y0 = poly->y0;
	fred.y1 = poly->y1;
	fred.y2 = poly->y2;
	fred.y3 = poly->y3;

	fred.z0 = poly->z0;
	fred.z1 = poly->z1;
	fred.z2 = poly->z2;
	fred.z3 = poly->z3;

	fred.pad4 = poly->pad4;
	fred.pad5 = poly->pad5;
	fred.pad6 = poly->pad6;
	fred.pad7 = poly->pad7;

//jed
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
			fred.x3 = poly->x3/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
			fred.y3 = poly->y3/halfyf;
		}
	}
#endif
//jed

	fred.u0 = poly->u0;
	fred.u1 = poly->u1;
	fred.u2 = poly->u2;
	fred.u3 = poly->u3;
	fred.v0 = poly->v0;
	fred.v1 = poly->v1;
	fred.v2 = poly->v2;
	fred.v3 = poly->v3;
	fred.clut = poly->clut;
	fred.tpage = poly->tpage;
	*(long *)&fred.r0 = *(long *)&poly->r0;
//	fred.r0 = GET_PALLETE(poly->r0, poly->g0, poly->b0); //Now Set Before Here
	setPolyFT4(&fred);
#if USE_ATI
	JJSDrawPolyFT4(&fred);
#else
	DrawPolyFT4((u_long*)&fred);
#endif
}



void AYDrawG4(u_long *primitive)
{
	POLY_G4	*poly;
#if USE_ATI
	POLY_G4 fred;
#else
	POLY_F4	fred;
#endif

	poly = (POLY_G4 *)primitive;

#if USE_ATI
//jed
	fred = *poly;
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
			fred.x3 = poly->x2/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
			fred.y3 = poly->y2/halfyf;
		}
	}
#endif
	JJSDrawPolyG4((u_long*)&fred);
#else
	fred.x0 = poly->x0;
	fred.x1 = poly->x1;
	fred.x2 = poly->x2;
	fred.x3 = poly->x3;

	fred.y0 = poly->y0;
	fred.y1 = poly->y1;
	fred.y2 = poly->y2;
	fred.y3 = poly->y3;

//jed
#if 0
	if (inattract==0)
	{
		if ((halfx!=0) && (inmenu==0))
		{
			fred.x0 = poly->x0/halfxf;
			fred.x1 = poly->x1/halfxf;
			fred.x2 = poly->x2/halfxf;
			fred.x3 = poly->x3/halfxf;
		}
		if ((halfy!=0) && (inmenu==0))
		{
			fred.y0 = poly->y0/halfyf;
			fred.y1 = poly->y1/halfyf;
			fred.y2 = poly->y2/halfyf;
			fred.y3 = poly->y3/halfyf;
		}
	}
#endif
//jed

	*(long *)&fred.r0 = *(long *)&poly->r0;
//	fred.r0 = GET_PALLETE(poly->r0, poly->g0, poly->b0); //Now Set Before Here

	setPolyF4(&fred);
	DrawPolyF4((u_long*)&fred);
#endif
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

#if USE_ATI
	JJSDrawPolyF4(&op);
#else
	DrawPolyF4((u_long*)&op);
#endif

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
#if USE_ATI
	sendList();
 	setRenderEnd();
#endif
//JJS	Mask(&op);
//	S_Sprite(&op);
}	

//jed

#ifndef USE_ATI

void	Scalex(char *Sram,char *Dram)
{
int	count;
int	countx;
int	p;

	if (pixdouble==1)
	{
		for (count=0;count<screenh;count++)
		{
			if (!Reallyinmenu)
				DoubleX(Sram,Dram,screenw);
			else
				DoubleXSpesh(Sram,Dram,screenw);
			Dram+=320;
			Sram+=320;
		}
	}

	if (pixdouble==2)
	{
		for (count=0;count<screenh;count++)
		{
			if (!Reallyinmenu)
			{
				DoubleX(Sram,Dram,screenw);
				DoubleX(Sram,Dram+320,screenw);
			}
			else
			{
				DoubleXSpesh(Sram,Dram,screenw);
				DoubleXSpesh(Sram,Dram+320,screenw);
			}
			Dram+=640;
			Sram+=320;
		}
	}
}
#endif //USE_ATI

//jed
