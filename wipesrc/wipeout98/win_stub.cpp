#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "wipesrc/psx26/include/libgte.h"
#include "wipesrc/psx26/include/libgpu.h"

typedef int32_t BOOL;
#define		TRUE			1
#define		FALSE			0

typedef struct
{
	int32_t			id;
	int32_t 			flags;
	int32_t			bnum;
	int16_t			textureX, textureY;
	int16_t 			textureH, textureW;
} TimData;

int16_t ft=0;
char MipMap = 0;
char *SaveDirBase;
char *DataDirBase;
char *MovieDirBase;
char globalPilot[255]={0};
int16_t FrameState=0;
int32_t menuTexturesIn = 0;
int32_t numoff=0;
int32_t numsmall=0;
int16_t screenres=3;
int16_t trackNum = 0;
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

int32_t JJSLoadSprites (void)
{ return 0; }

int32_t	     LockBackBuffer(void)
{ return 0; }

void	    UnlockBackBuffer(void)
{}

void	    WinClearScreen(char colour)
{}

void setRenderBegin(void)
{}

int32_t	     ProcessMessages(void)
{ return 0; }

void	    BltClearToScreen(void)
{}

TimData *JJSLoad16BitTexture(char *filename, char location)
{ return NULL; }

int16_t randlist[2000];
int16_t *randnum;
int16_t ctr;

void initrandy()
{
	int16_t i;
	time_t t;

	t=time(NULL);

	srand(t);

	for (i = 0; i < 2000; i++)
		randlist[i] = rand() & 0x7FFF;

	randnum = &randlist[0];;
	ctr = 0;
}

int32_t randy(void)
{
	int16_t num;

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

void	    CopyPal(char *palette, int16_t start, int16_t end)
{}

void	    WinQuit(void)
{}

void sendList(void)
{}

void oldmain(void);

extern int16_t    WinWidth;
extern int16_t    WinHeight;
extern int16_t    WinHeightX2;

int32_t main(int32_t argc, char** argv)
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