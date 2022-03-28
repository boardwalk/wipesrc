/**************************************************************************
 *
 * FILE     : Demo.c
 * FUNCTION	: Demo Routines To Test "EAGLE.C"
 * AUTHOR	: Jed Adams (C) 1995 
 *
 **************************************************************************/

#define	NUL	   0
#define	FLAT		0
#define	GOURAUD	!FLAT

#define	TRUE	1
#define	FALSE	0
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

#define	NumVert			2
#define	Alloc(x,s)	malloc ((x)*(s))
#define	DeAlloc(p)	free ((p))

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include "cgl.h"

int	quit=FALSE;
int	Key_Pressed;
int	Gcount=0;
int	OGcount=0;
int	X=0;
int 	NumPol=20000*3;			// Define a max number of triangles 

int	TempX;
int	TempY;
int	TempType;

int	Width;
int	Height;

int	ScrX=640;
int	ScrY=400;

CGL_UINT16			*PanelBuf;

CGL_VERTEX2D_ST	*GVert;
CGL_COLOR_ST		*GCol;
CGL_PTR				*GouradPtr;
CGL_RENDERFIX_ST	*RenderStruct;
CGL_GPCONFIG_ST	stConfig;
CGL_RENDERFIX_ST  r;
CGL_UINT32 			PTR stream[1];
CGL_COLOR_ST 		stClearColor = { 0,0,0,0 }; 
CGL_RECT_ST  stScreenRect;                // screen box, used by cglClearBuffer
CGL_UPDN_ST stDownLoad;    

int	Xps;	

/************************************************
 * the main program 
 ************************************************/

void main()
{

	clock_t begin, end;


	OpenUp();
	Info();
	

	ReRand();



		begin = clock();

//	while(!quit)

	for (X=0;X<69;X++)
		{
   
			DrawPrimitive();
			Gcount=OGcount;

			Triangle(GOURAUD,000,100,100,140,140,000,255,200,NUL,NUL,255,NUL,NUL,NUL,255);
			Triangle(GOURAUD,Xps,Xps,100,140,140,000,255,200,NUL,NUL,255,NUL,NUL,NUL,255);

			Xps++;
			
			if (Xps>=Height) Xps=0;

			CheckKeyboard();
		}

		end = clock();



	printf("%lu %lu \n",begin,end);

	printf("%lu\n",(end-begin)/CLOCKS_PER_SEC);

	printf("Number Of Triangles On Screen %d \n",Gcount);

	CloseDown();
}

void	CheckKeyboard()
{
	char	c;

 	if (kbhit())	
	{
		c=getch();
		switch(c)
		{	
			case	0		:	c=getch();
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
	
void	ReadSpecialKey(char	c)
{
	switch(c)
	{
		case	UP_RIGHT		:	ScrX+=640/32;
									ScrY+=400/32;
									SizeScreen(ScrX,ScrY);
									ReRand();
									break;
		case	UP				:										
									break;
		case	UP_LEFT		:	ScrX-=640/32;										
									ScrY-=400/32;
									SizeScreen(ScrX,ScrY);
									ReRand();
									break;
		case	LEFT			:	
									break;
		case	DOWN_LEFT	:	
									break;
		case	DOWN			:									
									break;
		case	DOWN_RIGHT	:										
									break;
		case	RIGHT			:	
									break;
		case	FIRE			:	Xps=0;
									break;
		default				:	
									break;
	}
}


void	ReRand()
{
	Gcount=0;
	for (X=0;X<4000;X++)
		{
		TempX=Random(Width-10);
		TempY=Random(Height-10);
		TempType=Random(256)/128;
		Triangle(TempType,TempX,TempY,TempX+10,TempY,TempX+5,TempY+10,Random(255),Random(255),Random(255),Random(255),Random(255),Random(255),Random(255),Random(255),Random(255));
		}
	OGcount=Gcount;
}



void	ReadTGAPanel (char *filename)
{
  FILE        *fp = NULL;

  fp = fopen(filename,"rb");
  fread(PanelBuf, 640*400, sizeof(CGL_UINT16), fp);       
  fclose(fp);

} 


#include "eagle.c"





