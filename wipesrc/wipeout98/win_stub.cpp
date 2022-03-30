#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

typedef int BOOL;
#define		TRUE			1
#define		FALSE			0

typedef struct {
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0;
	unsigned char	u0, v0;	unsigned short	clut;
	short	w,	h;
} SPRT;					/* free size Sprite */

typedef struct {
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0;
	unsigned char	u0, v0;	unsigned short	clut;
} SPRT_16;				/* 8x8 Sprite */

typedef SPRT_16 SPRT_8;

typedef struct
{
	unsigned long   tag;
	unsigned char   r0, g0, b0, code;
	short	   x0, y0;
	short	   x1, y1;
	short	   x2, y2;
	short	   x3, y3;
}	       POLY_F4;	/* Flat Quadrangle */

typedef struct {
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0;
	unsigned char	r1, g1, b1, pad1;
	short	x1,	y1;
	unsigned char	r2, g2, b2, pad2;
	short	x2,	y2;
} POLY_G3;				/* Gouraud Triangle */

typedef struct {
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0;
	unsigned char	r1, g1, b1, pad1;
	short	x1,	y1;
	unsigned char	r2, g2, b2, pad2;
	short	x2,	y2;
	unsigned char	r3, g3, b3, pad3;
	short	x3,	y3;
} POLY_G4;				/* Gouraud Quadrangle */

typedef struct {
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, y0, z0, pad4;
	unsigned char	u0, v0;
	unsigned short	clut;
	short	x1,	y1, z1, pad5;
	unsigned char	u1, v1;
	unsigned short	tpage;
	short	x2,	y2, z2, pad6;
	unsigned char	u2, v2;
	unsigned short	pad1;
} POLY_FT3;				/* Flat Textured Triangle */

typedef struct
{
	long			id;
	long 			flags;
	long			bnum;
	short			textureX, textureY;
	short 			textureH, textureW;
} TimData;

typedef struct
{
	unsigned long   tag;
	unsigned char   r0, g0, b0, code;
	short	   x0, y0, z0, pad4;
	unsigned char   u0, v0;
	unsigned short  clut;
	short	   x1, y1, z1, pad5;
	unsigned char   u1, v1;
	unsigned short  tpage;
	short	   x2, y2, z2, pad6;
	unsigned char   u2, v2;
	unsigned short  pad1;
	short	   x3, y3, z3, pad7;
	unsigned char   u3, v3;
	unsigned short  pad2;
}	       POLY_FT4;       /* Flat Textured Quadrangle */

typedef struct {
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0, z0, pad4;
	unsigned char	u0, v0;	unsigned short	clut;
	unsigned char	r1, g1, b1, p1;
	short	x1,	y1, z1, pad5;
	unsigned char	u1, v1;	unsigned short	tpage;
	unsigned char	r2, g2, b2, p2;
	short	x2,	y2, z2, pad6;
	unsigned char	u2, v2;	unsigned short	pad2;
} POLY_GT3;				/* Gouraud Textured Triangle */

typedef struct {
	unsigned long	tag;
	unsigned char	r0, g0, b0, code;
	short	x0, 	y0, z0, pad4;
	unsigned char	u0, v0;	unsigned short	clut;
	unsigned char	r1, g1, b1, p1;
	short	x1,	y1, z1, pad5;
	unsigned char	u1, v1;	unsigned short	tpage;
	unsigned char	r2, g2, b2, p2;
	short	x2,	y2, z2, pad6;
	unsigned char	u2, v2;	unsigned short	pad2;
	unsigned char	r3, g3, b3, p3;
	short	x3,	y3, z3, pad7;
	unsigned char	u3, v3;	unsigned short	pad3;
} POLY_GT4;				/* Gouraud Textured Quadrangle */

short ft=0;
char MipMap = 0;
char *SaveDirBase;
char *DataDirBase;
char *MovieDirBase;
char globalPilot[255]={0};
short FrameState=0;
int menuTexturesIn = 0;
long numoff=0;
long numsmall=0;
short screenres=3;
short trackNum = 0;
float upres=2.0f;

void JJS_Sprite(SPRT *sp) {}
void JJS_Sprite8(SPRT_8 *sp) {}

char *LockPilot(void)
{
    return NULL;
}

void UnlockPilot(void)
{}

BOOL JJSJoyConfig(void)
{ return FALSE; }

void changeScreen(void)
{}

void setRenderEnd(void)
{}

void	    JJSDrawPolyF3(POLY_F4 * p)
{}

void	    JJSDrawPolyF4(POLY_F4 * p)
{}

void	    JJSDrawPolyG3(POLY_G3 * p)
{}

void	    JJSDrawPolyG4(POLY_G4 * p)
{}

void	    JJSDrawPolyFT3(POLY_FT3 * p)
{}

void	    JJSDrawPolyFT4(POLY_FT4 * p)
{}

void	    JJSDrawPolyGT3(POLY_GT3 * p)
{}

void	    JJSDrawPolyGT4(POLY_GT4 * p)
{}

BOOL SJRNewTexture(char *filename)
{ return FALSE; }

void centreMouse(void)
{}

int JJSLoadSprites (void)
{ return 0; }

int	     LockBackBuffer(void)
{ return 0; }

void	    UnlockBackBuffer(void)
{}

void	    WinClearScreen(char colour)
{}

void setRenderBegin(void)
{}

int	     ProcessMessages(void)
{ return 0; }

void	    BltClearToScreen(void)
{}

TimData *JJSLoad16BitTexture(char *filename, char location)
{ return NULL; }

short randlist[2000];
short *randnum;
short ctr;

void initrandy()
{
	short i;
	time_t t;

	t=time(NULL);

	srand(t);

	for (i = 0; i < 2000; i++)
		randlist[i] = rand() & 0x7FFF;

	randnum = &randlist[0];;
	ctr = 0;
}

int randy(void)
{
	short num;

	num = *randnum++;

	ctr++;
	if (ctr == 2000)
	{
		randnum = &randlist[0];;
		ctr = 0;
	}

	return num;

}

void	    DDSwap(void)
{}

void	    CopyPal(char *palette, short start, short end)
{}

void	    WinQuit(void)
{}

void sendList(void)
{}

void oldmain(void);

extern short    WinWidth;
extern short    WinHeight;
extern short    WinHeightX2;

int main(int argc, char** argv)
{
	initrandy();
	WinWidth = 640;
	WinHeight = 480;
	WinHeightX2 = WinHeight * 2;

	SaveDirBase = (char *)malloc(120);
	strcpy(SaveDirBase, ".");

	DataDirBase = (char *)malloc(120);
	strcpy(DataDirBase, ".");

	MovieDirBase = (char *)malloc(120);
	strcpy(MovieDirBase, ".");

	chdir(DataDirBase);

	oldmain();
	return 0;
}