
//#include "d:\include\sys\types.h"
//#include "d:\include\libgte.h"
//#include "d:\include\libgpu.h"
#include "sys\types.h"
#include "libgte.h"
#include "libgpu.h"
#include "cgl.h"
#include <graph.h>
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <ctype.h>

//#include "gpu.h"

//	#define	DEPTH


#define VGA		0
#define EAGLE	1
#define WATCOM 2
#define PROFILE	0

#define NULL	0

#define TRUE	1
#define	ESCAPE	27
#define	RIGHT			77
#define	UP_RIGHT		73
#define	UP				72
#define	UP_LEFT		71
#define	LEFT			75
#define	DOWN_LEFT	79
#define	DOWN			80
#define	DOWN_RIGHT	81
#define	FIRE			76
#define NUMTRI 500
#define COUNTVAR	200
#define TRISIZE	10


long	AYdx[NUMTRI] , AYdy[NUMTRI];
long	AYx[NUMTRI], AYy[NUMTRI];
char	AYcol[3][NUMTRI];




int gfx_device;


CGL_RECT_ST  stScreenRect;                // screen box, used by cglClearBuffer

#ifdef	DEPTH
CGL_VERTEX3D_ST	*GVert;
#else
CGL_VERTEX2D_ST	*GVert;
#endif


CGL_COLOR_ST		*GCol;
CGL_PTR				*GouradPtr;
CGL_RENDERFIX_ST	*RenderStruct;

#ifdef	DEPTH
#define	NumVert			3
#else
#define	NumVert			2
#endif



int 	NumPol=20000*3;			// Define a max number of triangles 
int	Gcount=0;
int	numtri = 200;
int	quit;

#define	Alloc(x,s)	malloc ((x)*(s))
#define	DeAlloc(p)	free ((p))


void (*DrawFunctions[3][256])(u_long *);
void IntArray();
void NotValidPrimType(u_long *primitive);
void W_FlatTri(u_long *primitive);
void W_GouraudTri(u_long *primitive);
void W_FlatQuad(u_long *primitive);
void W_GouraudQuad(u_long *primitive);
void W_FlatLine_2(u_long *primitive);
void W_GouraudLine_2(u_long *primitive);
void W_FlatLine_3(u_long *primitive);
void W_GouraudLine_3(u_long *primitive);
void W_FlatLine_4(u_long *primitive);
void W_GouraudLine_4(u_long *primitive);

void E_FlatTri(u_long *primitive);
void E_GouraudTri(u_long *primitive);
void E_FlatQuad(u_long *primitive);
void E_GouraudQuad(u_long *primitive);
void E_FlatLine_2(u_long *primitive);
void E_GouraudLine_2(u_long *primitive);
void E_FlatLine_3(u_long *primitive);
void E_GouraudLine_3(u_long *primitive);
void E_FlatLine_4(u_long *primitive);
void E_GouraudLine_4(u_long *primitive);

struct	xycoord	points[4] = {0,0, 0,0, 0,0, 0,0};
#if 0
int rand(void)
{
	static int ret;

	return(ret++);
}
#endif
void DrawOTag(unsigned long *primitive)
{
unsigned long addr;
	addr = primitive;
	if ((addr &= 0xffffff) != 0xffffff) do
		{
		DrawPrim((u_long *)addr);
		primitive = addr;
		}
	while ((addr = getaddr(primitive)) != 0xffffff);

}

void	CheckKeyboard()
{
	char	c;

 	if (kbhit())	
	{
		c=toupper(getch());
		switch(c)
		{	
			case	0		:	c=toupper(getch());
								ReadSpecialKey(c);
								break;
			case	ESCAPE:
			case	'Q'	:	quit=TRUE;
								break;
			default		:	
								break;
		}
	}
}
	
/***************************************************************************
 *
 *
 *
 ***************************************************************************/

void	ReadSpecialKey(char	c)
{
	switch(c)
	{
		case	UP_RIGHT		:		
									break;

		case	UP				:	numtri+=1;
									if (numtri>NUMTRI)
										numtri=NUMTRI;
									break;

		case	UP_LEFT		:											
									break;


		case	LEFT			:	
									break;

		case	DOWN_LEFT	:	
									break;

		case	DOWN			:	numtri-=1;
									if (numtri< 0)
										numtri=0;
									break;

		case	DOWN_RIGHT	:		
									break;

		case	RIGHT			:	
									break;

		case	FIRE			:			
									break;
		default				:	
									break;
	}
}





void main()
{
	POLY_G3	*poly_buf;



	int loop, count;

	printf("init things\n");
	IntArray();
	IntWatcom();
	IntEagle();



	if ((poly_buf = malloc(sizeof(POLY_G3) * NUMTRI)) == NULL)
		{
		fprintf(stderr,"malloc failed \n");
		}

	printf("set device\n");
	getch();

	Set_Device(WATCOM);
//	Set_Device(EAGLE);
	printf("device ok\n");
	

	printf("some things\n");
		
	getch();

	for (loop = 0; loop < (NUMTRI-1); loop++)
		{
		AYx[loop] = rand() % (320 - TRISIZE);
		AYy[loop] = rand() % (200 - TRISIZE);
		AYdx[loop] = (rand() % 4) + 1;
		AYdy[loop] = (rand() % 4) + 1;
		AYcol[0][loop] = (char)rand();
		AYcol[1][loop] = (char)rand();
		AYcol[2][loop] = (char)rand();
		setPolyG3(&poly_buf[loop]);
		setRGB0(&poly_buf[loop],AYcol[0][loop],AYcol[1][loop],AYcol[2][loop]);
		setRGB1(&poly_buf[loop],AYcol[0][loop],AYcol[1][loop],AYcol[2][loop]);
		setRGB2(&poly_buf[loop],AYcol[0][loop],AYcol[1][loop],AYcol[2][loop]);
		setXY3(&poly_buf[loop],AYx[loop],AYy[loop],AYx[loop] + 10,AYy[loop],AYx[loop],AYy[loop] + 10);
		setaddr(&poly_buf[loop],((u_long)&poly_buf[loop + 1] & 0xffffff));
		}

		printf("poly buff inited\n");
	
		getch();

	quit =0;
	count = COUNTVAR;
	do
		{

		for (loop = 0; loop < numtri; loop++)
			{
			AYx[loop] = (AYx[loop] + AYdx[loop]) % (320 - TRISIZE);
			AYy[loop] = (AYy[loop] + AYdy[loop]) % (200 - TRISIZE);
			setXY3(&poly_buf[loop],
					(AYx[loop] ) ,
					(AYy[loop] ) ,
					(AYx[loop] + TRISIZE ) ,
					(AYy[loop] ) ,
					(AYx[loop] ) ,
					(AYy[loop] + TRISIZE));
			}


		setaddr(&poly_buf[numtri],0xffffff);
		if (gfx_device==WATCOM)
			{
			_clearscreen(_GCLEARSCREEN);
			}
		Gcount=0;
		DrawOT(poly_buf);

	if (gfx_device==EAGLE)
		{
//		cglClearScreen();

#ifdef	DEPTH
		cglClearBuffer(CGL_FRAME_BUFFER | CGL_DEPTH_BUFFER, &stScreenRect);
#else
		cglClearBuffer(CGL_FRAME_BUFFER , &stScreenRect);
#endif
		SetPrims();
		cglSwapBuffer();	 
		}

		setaddr(&poly_buf[numtri],((u_long)&poly_buf[numtri + 1] & 0xffffff));
		CheckKeyboard();
		}
#if PROFILE
	while (count-- > 0);
#else
	while (!quit);
#endif

  
	if (gfx_device == EAGLE)
		{
		DeAlloc(GVert);
		DeAlloc(GCol);
		DeAlloc(GouradPtr);
		DeAlloc(RenderStruct);
		}
	printf("Triangle @ end = %d\n",numtri);
//	return();

}

void Set_Device(int tmp)
{


	gfx_device=tmp;

	// Initialise What Ever Gfx Device We Use

	if (tmp==WATCOM)
		{
		printf("set video mode watcom\n");
		_setvideomode(_XRES16COLOR);
		}
	if (tmp==EAGLE)
		{
  
CGL_SCREEN_ST ScreenConfig;

		printf("set video mode eagle\n");

#ifdef	DEPTH
		GVert=(CGL_VERTEX3D_ST*)Alloc(NumPol*NumVert,sizeof(CGL_VERTEX3D_ST));
#else
		GVert=(CGL_VERTEX2D_ST*)Alloc(NumPol*NumVert,sizeof(CGL_VERTEX2D_ST));
#endif

		GCol=(CGL_COLOR_ST*)Alloc(NumPol*4,sizeof(CGL_COLOR_ST));

		GouradPtr=(CGL_PTR*)Alloc(5,sizeof(CGL_PTR));

		RenderStruct=(CGL_RENDERFIX_ST*)Alloc(5,sizeof(CGL_RENDERFIX_ST));

		printf("Memory Allocated\n");

						ScreenConfig.wReference      = CGL_TOP_LEFT;
						ScreenConfig.wMode           = CGL_M640x400x70Hz;
						ScreenConfig.wColorFormat    = CGL_RGB5551;
						ScreenConfig.wColorOrder     = CGL_RGB_MODE;
						ScreenConfig.wBuffers        = CGL_DOUBLE;
#ifdef	DEPTH
						ScreenConfig.wOption         = CGL_D15S1;
					   cglSetDepthMode(CGL_LESS);
#else
						ScreenConfig.wOption         = CGL_D0S0;
#endif

						ScreenConfig.dwUB_BufferSize = 0;
						cglInitScreen(&ScreenConfig);
						cglSetDitherMode(CGL_ENABLE);
						stScreenRect.wX = 0;
						stScreenRect.wY = 0;
						stScreenRect.wWidth  = 640;
						stScreenRect.wHeight = 400;

//			cglClearScreen();
			

			printf("Eagle Is Up\n");
		}
}

void IntWatcom()
{
	DrawFunctions[WATCOM][0x20]=*W_FlatTri;			// Poly_F3
	DrawFunctions[WATCOM][0x30]=*W_GouraudTri;		// Poly_G3
	DrawFunctions[WATCOM][0x28]=*W_FlatQuad;			// Poly_F4
	DrawFunctions[WATCOM][0x38]=*W_GouraudQuad;		// Poly_G4
	DrawFunctions[WATCOM][0x40]=*W_FlatLine_2;		// Line_F2
	DrawFunctions[WATCOM][0x50]=*W_GouraudLine_2;	// Line_G2
	DrawFunctions[WATCOM][0x48]=*W_FlatLine_3;		// Line_F3
	DrawFunctions[WATCOM][0x58]=*W_GouraudLine_3;	// Line_G3
	DrawFunctions[WATCOM][0x4c]=*W_FlatLine_4;		// Line_F4
	DrawFunctions[WATCOM][0x5c]=*W_GouraudLine_4;	// Line_G4
}

void IntEagle()
{
	DrawFunctions[EAGLE][0x20]=*E_FlatTri;			// Poly_F3
	DrawFunctions[EAGLE][0x30]=*E_GouraudTri;		// Poly_G3
	DrawFunctions[EAGLE][0x28]=*E_FlatQuad;			// Poly_F4
	DrawFunctions[EAGLE][0x38]=*E_GouraudQuad;		// Poly_G4
	DrawFunctions[EAGLE][0x40]=*E_FlatLine_2;		// Line_F2
	DrawFunctions[EAGLE][0x50]=*E_GouraudLine_2;	// Line_G2
	DrawFunctions[EAGLE][0x48]=*E_FlatLine_3;		// Line_F3
	DrawFunctions[EAGLE][0x58]=*E_GouraudLine_3;	// Line_G3
	DrawFunctions[EAGLE][0x4c]=*E_FlatLine_4;		// Line_F4
	DrawFunctions[EAGLE][0x5c]=*E_GouraudLine_4;	// Line_G4
}


void intarray()
{
	int x,y;
		for (y=0;y<3;y++)
		{
			for (x=0;x<256;x++)
			{
			DrawFunctions[y][x]=*NotValidPrimType;
			}
		}
}

void DrawPrim(u_long *primitive)
{
	DrawFunctions[gfx_device][((P_TAG *)primitive)->code](primitive);
}


void NotValidPrimType(u_long *primitive)
{
printf("Primitive Type %x not supported In Current Code\n",((P_TAG *)primitive)->code);
}	

#define GPU_printf printf
#define GET_PALLETE(a,b,c)	((a & 0xe0) | ((b & 0xe0) >>3) | ((c & 0xc0) >> 5))



// Basic PSX gfx prims using WATCOM Gfx Library

void W_FlatTri(u_long *primitive)
{
POLY_F3	*poly;
	poly = (POLY_F3 *)primitive;

	points[0].xcoord=poly->x0;
	points[0].ycoord=poly->y0;
	points[1].xcoord=poly->x1;
	points[1].ycoord=poly->y1;
	points[2].xcoord=poly->x2;
	points[2].ycoord=poly->y2;
	_setcolor(GET_PALLETE(poly->r0,poly->g0,poly->b0));
	_polygon(_GBORDER,3,&points);
	

}

void W_GouraudTri(u_long *primitive)
{
POLY_G3	*poly;
	poly = (POLY_G3 *)primitive;

	points[0].xcoord=poly->x0;
	points[0].ycoord=poly->y0;
	points[1].xcoord=poly->x1;
	points[1].ycoord=poly->y1;
	points[2].xcoord=poly->x2;
	points[2].ycoord=poly->y2;
	_setcolor(GET_PALLETE(poly->r0,poly->g0,poly->b0));
	_polygon(_GFILLINTERIOR,3,&points);

}

void W_FlatQuad(u_long *primitive)
{
POLY_F4	*poly;
	poly = (POLY_F4 *)primitive;

	points[0].xcoord=poly->x0;
	points[0].ycoord=poly->y0;
	points[1].xcoord=poly->x1;
	points[1].ycoord=poly->y1;
	points[2].xcoord=poly->x3;
	points[2].ycoord=poly->y3;
	points[3].xcoord=poly->x2;
	points[3].ycoord=poly->y2;
	_setcolor(1);
	_polygon(_GBORDER,4,&points);
}

void W_GouraudQuad(u_long *primitive)
{
POLY_G4	*poly;
	poly = (POLY_G4 *)primitive;

	points[0].xcoord=poly->x0;
	points[0].ycoord=poly->y0;
	points[1].xcoord=poly->x1;
	points[1].ycoord=poly->y1;
	points[2].xcoord=poly->x3;
	points[2].ycoord=poly->y3;
	points[3].xcoord=poly->x2;
	points[3].ycoord=poly->y2;
	_setcolor(2);
	_polygon(_GFILLINTERIOR,4,&points);
}

void W_FlatLine_2(u_long *primitive)
{
LINE_F2	*poly;
	poly = (LINE_F2 *)primitive;

	_setcolor(3);
	_moveto(poly->x0,poly->y0);
	_lineto(poly->x1,poly->y1);
}

void W_GouraudLine_2(u_long *primitive)
{
LINE_G2	*poly;
	poly = (LINE_G2 *)primitive;

	_setcolor(4);
	_moveto(poly->x0,poly->y0);
	_lineto(poly->x1,poly->y1);
}

void W_FlatLine_3(u_long *primitive)
{
LINE_F3	*poly;
	poly = (LINE_F3 *)primitive;

	_setcolor(3);
	_moveto(poly->x0,poly->y0);
	_lineto(poly->x1,poly->y1);
	_lineto(poly->x2,poly->y2);
}

void W_GouraudLine_3(u_long *primitive)
{
LINE_G3	*poly;
	poly = (LINE_G3 *)primitive;

	_setcolor(4);
	_moveto(poly->x0,poly->y0);
	_lineto(poly->x1,poly->y1);
	_lineto(poly->x2,poly->y2);
}

void W_FlatLine_4(u_long *primitive)
{
LINE_F4	*poly;
	poly = (LINE_F4 *)primitive;

	_setcolor(3);
	_moveto(poly->x0,poly->y0);
	_lineto(poly->x1,poly->y1);
	_lineto(poly->x2,poly->y2);
	_lineto(poly->x3,poly->y3);
}

void W_GouraudLine_4(u_long *primitive)
{
LINE_G4	*poly;
	poly = (LINE_G4 *)primitive;

	_setcolor(4);
	_moveto(poly->x0,poly->y0);
	_lineto(poly->x1,poly->y1);
	_lineto(poly->x2,poly->y2);
	_lineto(poly->x3,poly->y3);
}

// Basic PSX gfx prims using CREATIVE'S EAGLE & CGL Library


/**********************************************************
 **	Primitive Types 
 ** Lines		Normal Flat & Gouraud
 ** Triangles	Normal Flat & Gouraud
 ** Quads		Normal Flat & Gouraud
 **
 ** Lines		Textured Flat & Gouraud
 ** Triangles	Textured Flat & Gouraud
 ** Quads		Textured Flat & Gouraud
 **
 **	Variables Needed
 ** LVert		TVert			QVert		LCol		TCol		QCol		LCount	TCount	QCount
 ** TLVert		TTVert		TQVert	TLCol		TTCol		TQCol		TLCount	TTCount	TQCount
 ** DepthCount
 **********************************************************/

/**********************************************************
 **														  x1
 ** Draw One Line Flat                        / 
 **													  x0
 **********************************************************/

void E_FlatLine_2(u_long *primitive)
{
LINE_F2	*poly;
}


/**********************************************************
 **														  x1
 ** Draw One Line Gouraud'd                   / 
 **													  x0
 **********************************************************/

void E_GouraudLine_2(u_long *primitive)
{
LINE_G2	*poly;
}

/**********************************************************
 **														   x1_
 ** Draw Two Lines Connected Together By X1	 /	    - x2
 **													  x0
 **********************************************************/

void E_FlatLine_3(u_long *primitive)
{
LINE_F3	*poly;
}


/**********************************************************
 **														   x1_
 ** Draw Two Lines Connected Together By X1	 /	    - x2 & Gouraud Em
 **													  x0
 **********************************************************/

void E_GouraudLine_3(u_long *primitive)
{
LINE_G3	*poly;
}



/**********************************************************
 **														   	  x1--x2
 ** Draw Three Lines Connected Together By X1 & X2	 /	     \ 
 **													        x0			x3
 **********************************************************/

void E_FlatLine_4(u_long *primitive)
{
LINE_F4	*poly;
}



/**********************************************************
 **														   	  x1--x2
 ** Draw Three Lines Connected Together By X1 & X2	 /	     \  & Gouraud Em
 **													        x0			x3
 **********************************************************/

void E_GouraudLine_4(u_long *primitive)
{
LINE_G4	*poly;
	poly = (LINE_G4 *)primitive;
}


/**********************************************************
 **														  x1_
 ** Draw A Triangle                           /#__--x2 
 **													  x0--
 **********************************************************/

void E_FlatTri(u_long *primitive)
{
POLY_F3	*poly;
	poly = (POLY_F3 *)primitive;

		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;

#ifdef	DEPTH
			GVert[0+(Gcount*3)].z=0;
			GVert[1+(Gcount*3)].z=0;
			GVert[2+(Gcount*3)].z=0;
#endif

		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;

Gcount++;
}

/**********************************************************
 **														  x1_
 ** Draw A Triangle & Gouraud                 /#__-- x2 
 **													  x0--
 **********************************************************/

void E_GouraudTri(u_long *primitive)
{
POLY_G3	*poly;
	poly = (POLY_G3 *)primitive;

		GVert[0+(Gcount*3)].x=poly->x0;GVert[0+(Gcount*3)].y=poly->y0;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x2;GVert[2+(Gcount*3)].y=poly->y2;

#ifdef	DEPTH
			GVert[0+(Gcount*3)].z=0;
			GVert[1+(Gcount*3)].z=0;
			GVert[2+(Gcount*3)].z=0;
#endif


		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r2;GCol[2+(Gcount*3)].bGreen=poly->g2;GCol[2+(Gcount*3)].bBlue=poly->b2;

Gcount++;
}


/**********************************************************
 **												-
 ** Draw A Quad By Doing 2 Triangles  |/| 
 **												-  
 **********************************************************/

void E_FlatQuad(u_long *primitive)
{
POLY_F4	*poly;
	poly = (POLY_F4 *)primitive;

// First Triangle

		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

// Second Triangle
		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x3;GVert[2+(Gcount*3)].y=poly->y3;
		GCol[0+(Gcount*3)].bRed=poly->r0;GCol[0+(Gcount*3)].bGreen=poly->g0;GCol[0+(Gcount*3)].bBlue=poly->b0;
		GCol[1+(Gcount*3)].bRed=poly->r0;GCol[1+(Gcount*3)].bGreen=poly->g0;GCol[1+(Gcount*3)].bBlue=poly->b0;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;
}

/**********************************************************
 **												-
 ** Draw A Quad By Doing 2 Triangles  |/| & Gouraud It
 **												-  
 **********************************************************/

void E_GouraudQuad(u_long *primitive)
{
POLY_G4	*poly;
	poly = (POLY_G4 *)primitive;

// First Triangle

		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x0;GVert[2+(Gcount*3)].y=poly->y0;
		GCol[0+(Gcount*3)].bRed=poly->r2;GCol[0+(Gcount*3)].bGreen=poly->g2;GCol[0+(Gcount*3)].bBlue=poly->b2;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r0;GCol[2+(Gcount*3)].bGreen=poly->g0;GCol[2+(Gcount*3)].bBlue=poly->b0;
Gcount++;

// Second Triangle
		GVert[0+(Gcount*3)].x=poly->x2;GVert[0+(Gcount*3)].y=poly->y2;
		GVert[1+(Gcount*3)].x=poly->x1;GVert[1+(Gcount*3)].y=poly->y1;
		GVert[2+(Gcount*3)].x=poly->x3;GVert[2+(Gcount*3)].y=poly->y3;
		GCol[0+(Gcount*3)].bRed=poly->r2;GCol[0+(Gcount*3)].bGreen=poly->g2;GCol[0+(Gcount*3)].bBlue=poly->b2;
		GCol[1+(Gcount*3)].bRed=poly->r1;GCol[1+(Gcount*3)].bGreen=poly->g1;GCol[1+(Gcount*3)].bBlue=poly->b1;
		GCol[2+(Gcount*3)].bRed=poly->r3;GCol[2+(Gcount*3)].bGreen=poly->g3;GCol[2+(Gcount*3)].bBlue=poly->b3;
Gcount++;
}

void SetPrims()
{
  CGL_DEPTH_ST DepthStruct;
						RenderStruct[0].wOperation     = CGL_RENDER;
						RenderStruct[0].dwNoOfVertice  = Gcount*3;
						RenderStruct[0].wPrimitiveType = CGL_TRIANGLE;
						RenderStruct[0].uPropertyEnableMask.i = 0x0000;
						RenderStruct[0].uPropertyEnableMask.u.shade = 1;

#ifdef	DEPTH
					   RenderStruct[0].uPropertyEnableMask.u.depth = 1;
					   DepthStruct.writeEnable           = CGL_ENABLE;
						RenderStruct[0].pDepth              = &DepthStruct;
#endif

						RenderStruct[0].pStencil            = NULL;
						RenderStruct[0].pBlend              = NULL;
						RenderStruct[0].pTextureEnv         = NULL;
						RenderStruct[0].pVertex             = &GVert[0];
						RenderStruct[0].pTexture            = NULL;
						RenderStruct[0].pColor              = &GCol[0];
						RenderStruct[0].pDepthCue           = NULL;
						GouradPtr[0] = &RenderStruct[0];
						cglSendStream(&GouradPtr[0],1);
}
