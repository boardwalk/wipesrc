#define NAME "WIPEOUT"
#define TITLE "WIPEOUT '95"
#define DBG				0

#define WINVER 0x0400
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <dos.h>
#include <time.h>
#include <winuser.h>
#include <mmsystem.h>
#include <direct.h>
#include <vfw.h>
#include <mciavi.h>
#include <ddraw.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"
#include "winh.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#if USE_ATI
#include <math.h>
#include "cwdde.h"
#include "decl.h"

#include "ati3dcif.h"

#define IDI_WIPEOUT 101

#define  SCALEIT(x) ((x)*upres)
#define  SCALEZ(z) ((z)*zscale)

#define PERSPECTIVE_CORRECT 1

#define SMALL 0
#define LOW 1
#define MEDIUM 2
#define HIGH 3

char globalPilot[255]={0};
//#define  SCALEIT(x) (x)

PTR48 Ptr48;

typedef struct
{
	long			id;
	long 			flags;
	long			bnum;
	short			textureX, textureY;
	short 			textureH, textureW;
} TimData;

extern void *heap;
extern char winLoseIn;
extern int inmenutest;
 
#define	BLACK_NOT_DRAWN	1
#define	BLACK1_TRANSL	2
#define	BLACK2_TRANSL	3
#define	BLACK3_TRANSL	4
#define	BLACK4_TRANSL	5
#define	BLACK5_TRANSL	6
#define	BLACK6_TRANSL	7
#define	WHITE1_TRANSL	8
#define	WHITE2_TRANSL	9
#define	WHITE3_TRANSL	10
#define	WHITE4_TRANSL	11
#define	WHITE5_TRANSL	12
#define	WHITE6_TRANSL	13
#define	WHITE7_TRANSL	14
#define	WHITE8_TRANSL	15
#define	RED_TRANSL		16
#define	RED_LIGHT_FADE		16
#define	BRIGHT_RED_TRANSL	17
#define	YELLOW_TRANSL	18
#define	YELLOW_WHITE_TRANSL	19
#define	BLUE_TRANSL		20
#define	PURPLE_TRANSL	21
#define	UNUSED1_TRANSL	22
#define	UNUSED2_TRANSL	23
#define	UNUSED3_TRANSL	24
#define	UNUSED4_TRANSL	25
#define ATTRACT 1

#define MCI_MCIAVI_PLAY_FULLSCREEN 0x02000000L
#define DIVIS 0.00195312

#define TEXTURESJR 2621440

typedef struct {
	short xsize,ysize;
	float mult;
}RESOLUTION;

BOOL Rendering;
BOOL backFromDead = FALSE;

extern short rendering;
RESOLUTION reses[]={
	{320,240,1.0},
	{400,300,1.25},
	{512,384,1.6},
	{640,480,2.0}
};

short trackSize,otherSize;
short memorySize=0;
short screenres=2;
float upres=1.6;
float zscale=1.0/32768.0;

int menuTexturesIn = 0;
int Keydelay,Keyrepeat,sSaver;
short dodump = 0,FrameState=0;
short PageXOff,PageYOff;
//#pragma aux randy parm [eax] modify exact [ebx edx] value [eax];

extern long			ctrlNeedEndText;
extern long			ctrlNeedWinLose;
extern long			ctrlNeedBackPoly;
extern long			ctrlNeedRaceInfo;
extern long			ctrlNeedDeSelectScreen;
extern long			ctrlNeedNewHighScores;
extern long			ctrlNeedRacePoints;
extern long			ctrlNeedChampTable;
extern long			ctrlNeedAllVenomComplete;
extern long			ctrlNeedVenomComplete;
extern long			ctrlNeedContinueGameConfirm;
extern long			ctrlNeedEndTextVar;
extern long			ctrlNeedWinLoseVar;
extern long			ctrlNeedInGameReset;
extern long			ctrlNeedInSingleGameConfirm;
extern long			ctrlNeedInGameConfirm;
extern long			ctrlNeedTargetIcon;
extern long			ctrlCDPaused;
extern long			ctrlfirsttime;
extern long ctrlmode;
extern char			ctrlmachinePaused;
extern int				ctrlcdPause;
extern	short   startIceText;
extern	short   startFireText;

typedef struct {
	LPDIRECTDRAWSURFACE	lpDDSTex;
	DDSURFACEDESC       ddsd;
	C3D_HTX				hTX;
} TEXTURE, * PTEXTURE;

TEXTURE gTex[500]={0};

extern char JJSTextureTable[],JJSTextureMenuTable[];

char lastfile[256];

#if 0
typedef struct
{
	long size;
	void *ptr;
}testers;

testers crapper[10000]={0};

#endif
typedef struct
{
	unsigned long   tag;
	unsigned char   r0, g0, b0, code;
	short	   x0, y0;
	short	   x1, y1;
	short	   x2, y2;
	short	   x3, y3;
}	       POLY_F4;	/* Flat Quadrangle */


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
} SPRT_8;				/* 8x8 Sprite */
	       
char	    gszErrMsg[64];
BOOL	    gbCIFInit;
C3D_HRC	 ghRC;

#define VBUFSIZE 8
#pragma pack(push)
#pragma pack (8)
C3D_VTCF	testarray[VBUFSIZE];
#pragma pack(pop);

C3D_PVTCF       testlist[VBUFSIZE];
C3D_EPRIM       quad = C3D_EPRIM_NUM;
C3D_ESHADE      shade = C3D_ESH_NUM;
C3D_ETLIGHT	light = C3D_ETL_NONE;
C3D_EADST eDstAlpha = C3D_EADST_ZERO;
C3D_EASRC eSrcAlpha = C3D_EASRC_ONE;
C3D_ETPERSPCOR ePerspCorOn = C3D_ETPC_FIVE;
C3D_ETEXFILTER eTexFilteron = C3D_ETFILT_MIN2BY2_MAG2BY2;
C3D_ETEXFILTER eTexFilteroff = C3D_ETFILT_MINPNT_MAGPNT;


static BOOL FirstActivate = TRUE;


//menu graphic detail stuff - SJR
short ft=0;
char MipMap = 0;

short texpage=-1;
short numvert=0;
short tester=0;
typedef struct {
	unsigned short blue:5;
	unsigned short green:5;
	unsigned short red:5;
} BIT15;

BIT15 ATIpalette[256];
BOOL bTexEn=FALSE;
#endif
BOOL trans = FALSE;
BOOL MPEGplaying;
HWND g_hwndMCIWnd;
DWORD dwMCIWndStyle;
HINSTANCE hinst;
HWND	    hwnd;

extern char DepthFadeTble,WhitePal;
extern short int CurrTrkNo;
short randlist[2000];
short *randnum;
char *TextureBuffer;
short ctr;
short trackNum = 0;
char *SaveDirBase;
char *DataDirBase;
char *MovieDirBase;
extern UINT wDeviceID;
extern char ctrlshowMenu;
extern long	ctrlNeedInGameReset;
short WinPaused = 0;
extern int ctrlchoice;
extern long screenZ;

struct sMovie
{
	char szMoviePathName[MAX_PATH];
	int width;
	int height;
	int fullScreen;
};

BOOL shellMoviePlayingG = FALSE;
BOOL shellMovieFinishedG = TRUE;
UINT shellMovieDeviceIDG;

//SJR

float joyModXVal;
float joyModYVal;
extern short JoyCentreX, JoyCentreY;
extern short	left_scale, right_scale;
extern float	left_scale_factor, right_scale_factor;
extern unsigned short JoyXVal, JoyYVal;
short JoyButton1, JoyButton2;

HANDLE gInstance;

#define	HIGH_SENS	8
#define	MEDIUM_SENS	16
#define	LOW_SENS	32

/* Type/Structure Definitions. */
typedef struct {
	unsigned short int	jX;
	unsigned short int	jY;
	unsigned short int	jDir;
	unsigned short int	jButton1;
	unsigned short int	jButton2;
} JOYSTICK;

typedef struct {
	unsigned short int	mX;
	unsigned short int	mY;
	unsigned short int	mDir;
	unsigned short int	mButton1;
	unsigned short int	mButton2;
	unsigned short int	mButton3;
} MOUSE;

typedef struct {
	long int	JoyUp;
	long int	JoyDown;
	long int	JoyLeft;
	long int	JoyRight;
	long int	JoyButton1;
	long int	JoyButton2;
	long int	JoyButton3;
	long int	JoyButton4;
} JOYSTICK_CONFIG;

typedef struct {
	long int	MouseUp;
	long int	MouseDown;
	long int	MouseLeft;
	long int	MouseRight;
	long int	MouseLeftButton;
	long int	MouseMiddleButton;
	long int	MouseRightButton;
	long int	MouseDummy;
} MOUSE_CONFIG;

#define _DEVICE_ON 1
#define OK 0
#define SONY_PAD 0x41
extern MOUSE etc_Mouse1;
extern JOYSTICK etc_Joy1;
extern unsigned long int etc_Mouse1Register;
extern unsigned long int etc_Joystick1Register;
extern MOUSE_CONFIG etc_Mouse1Config;
extern JOYSTICK_CONFIG etc_Joy1Config;
extern char *etc_TransferBuffer1;
extern char mouse_sensitivity;
extern unsigned char etc_Mouse1Switch;		/* id = 3 */
extern unsigned char etc_Joystick1Switch;
extern unsigned char etc_InputDeviceSelect;

#define TIMER_ID	1
#define TIMER_RATE      1

//#define	GAME_WIDTH	320
//#define GAME_HEIGHT	240

extern void     Set_Device(int tmp);
extern void     init3d(void);
extern void     oldmain(void);
extern void     QuitToDos(void);
extern void     pmkey(char, char);
extern void     ResetPitch(void);

extern int      WinPitch;
extern short    WinWidth;
extern short    WinHeight;
extern short    WinHeightX2;
int	     RealPitch;
int	     RealBackPitch;
char	   *DebugScreen, *RealScreen;

LPDIRECTDRAW    lpDD;	   // DirectDraw object
LPDIRECTDRAWSURFACE lpDDSPrimary;       // DirectDraw primary surface
LPDIRECTDRAWSURFACE lpDDSBack;  // DirectDraw back surface
#if !USE_ATI
LPDIRECTDRAWPALETTE lpDDPalette;
#endif
LPDIRECTDRAWSURFACE lpBack_Buffer;      // Directdraw 320X200 Surface for the  menu thing.
BOOL	    bActive;	// is application active?
BOOL backLocked=FALSE;
extern char    *CurBuffer, *CurBufferJ;
PALETTEENTRY    ddPalette[256];
char	   *backscreen;

extern long     NeedKeys;

extern void     finiSndObjects(void);


#define GUI_STAT (0xCE * 4)

extern WORD fiforead(WORD,DWORD);
#pragma aux fiforead= \
	"push	es" \
	"mov	es,ax" \
	"mov	eax,dword ptr es:[ebx + 824]" \
	"and	eax,0x00FF0000" \
	"shr	eax,16" \
	"pop	es" \
	parm [ax] [ebx] \
	modify [ebx];

WORD fifospace(void)
{
	return(fiforead(Ptr48.Selector,Ptr48.Offset));
}
/*
 * restoreObjects
 *
 * coming back to the game...put everything back
 */

static void	restoreObjects(void)
{
	lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary);
	if(lpBack_Buffer)
		lpBack_Buffer->lpVtbl->Restore(lpBack_Buffer);

	switch (trackNum)
	{
	case 0:
		//erm...not got anywhere yet...
		break;
	case 1:
		LoadVRam("wipeout\\newgraph\\venom\\altima\\altima.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\venom\\altima\\altima.sjr");
#endif
		break;
	case 2:
		LoadVRam("wipeout\\newgraph\\venom\\karbonis\\karbonis.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\venom\\karbonis\\karbonis.sjr");
#endif
		break;
	case 3:
		LoadVRam("wipeout\\newgraph\\venom\\terramax\\terramax.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\venom\\terramax\\terramax.sjr");
#endif
		break;
	case 4:
		LoadVRam("wipeout\\newgraph\\venom\\korodera\\korodera.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\venom\\korodera\\korodera.sjr");
#endif
		break;
	case 5:
		LoadVRam("wipeout\\newgraph\\venom\\arridos\\arridos.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\venom\\arridos\\arridos.sjr");
#endif
		break;
	case 6:
		LoadVRam("wipeout\\newgraph\\venom\\silver\\silver.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\venom\\silver\\silver.sjr");
#endif
		break;
	case 7:
		LoadVRam("wipeout\\newgraph\\venom\\firestar\\firestar.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\venom\\firestar\\firestar.sjr");
#endif
		break;
	case 8:
		LoadVRam("wipeout\\newgraph\\rapier\\altima\\altima.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\rapier\\altima\\altima.sjr");
#endif
		break;
	case 9:
		LoadVRam("wipeout\\newgraph\\rapier\\karbonis\\karbonis.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\rapier\\karbonis\\karbonis.sjr");
#endif
		break;
	case 10:
		LoadVRam("wipeout\\newgraph\\rapier\\terramax\\terramax.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\rapier\\terramax\\terramax.sjr");
#endif
		break;
	case 11:
		LoadVRam("wipeout\\newgraph\\rapier\\korodera\\korodera.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\rapier\\korodera\\korodera.sjr");
#endif
		break;
	case 12:
		LoadVRam("wipeout\\newgraph\\rapier\\arridos\\arridos.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\rapier\\arridos\\arridos.sjr");
#endif
		break;
	case 13:
		LoadVRam("wipeout\\newgraph\\rapier\\silver\\silver.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\rapier\\silver\\silver.sjr");
#endif
		break;
	case 14:
		LoadVRam("wipeout\\newgraph\\rapier\\firestar\\firestar.pcx", 0);
#if USE_ATI
		SJRNewTexture("wipeout\\newgraph\\rapier\\firestar\\firestar.sjr");
#endif
		break;
	case 15:
#if USE_ATI
		SJRNewTexture ("wipeout\\newgraph\\menu\\menus.sjr");
#endif
		LoadVRam("wipeout\\newgraph\\menu\\menus.pcx",0);
		reloadScr();
		break;
	case 16://in "press enter" screen
		reloadStartScr();
		break;
	default:
		break;
	}
	if(globalPilot[0])
		LoadPilotIntoVram(globalPilot) ;
	JJSLoadSprites();
	Swap();
	Swap();

}

static void	restoreTextures(void)
{
	switch (trackNum)
	{
	case 0:
		//erm...not got anywhere yet...
		break;
	case 1:
		SJRNewTexture("wipeout\\newgraph\\venom\\altima\\altima.sjr");
		break;
	case 2:
		SJRNewTexture("wipeout\\newgraph\\venom\\karbonis\\karbonis.sjr");
		break;
	case 3:
		SJRNewTexture("wipeout\\newgraph\\venom\\terramax\\terramax.sjr");
		break;
	case 4:
		SJRNewTexture("wipeout\\newgraph\\venom\\korodera\\korodera.sjr");
		break;
	case 5:
		SJRNewTexture("wipeout\\newgraph\\venom\\arridos\\arridos.sjr");
		break;
	case 6:
		SJRNewTexture("wipeout\\newgraph\\venom\\silver\\silver.sjr");
		break;
	case 7:
		SJRNewTexture("wipeout\\newgraph\\venom\\firestar\\firestar.sjr");
		break;
	case 8:
		SJRNewTexture("wipeout\\newgraph\\rapier\\altima\\altima.sjr");
		break;
	case 9:
		SJRNewTexture("wipeout\\newgraph\\rapier\\karbonis\\karbonis.sjr");
		break;
	case 10:
		SJRNewTexture("wipeout\\newgraph\\rapier\\terramax\\terramax.sjr");
		break;
	case 11:
		SJRNewTexture("wipeout\\newgraph\\rapier\\korodera\\korodera.sjr");
		break;
	case 12:
		SJRNewTexture("wipeout\\newgraph\\rapier\\arridos\\arridos.sjr");
		break;
	case 13:
		SJRNewTexture("wipeout\\newgraph\\rapier\\silver\\silver.sjr");
		break;
	case 14:
		SJRNewTexture("wipeout\\newgraph\\rapier\\firestar\\firestar.sjr");
		break;
	case 15:
		SJRNewTexture ("wipeout\\newgraph\\menu\\menus.sjr");
		reloadScr();
		break;
	default:
		break;
	}
	if(globalPilot[0])
		LoadPilotIntoVram(globalPilot) ;
	Swap();
	Swap();

}

/*
 * finiObjects
 * 
 * finished with all objects we use; release them
 */
static void     finiObjects(void)
{
#if USE_ATI
	FILE *f1;
	int i;
	
	CloseATI3DCIF();
#endif
	if (lpDD != NULL)
	{
		if (lpDDSPrimary != NULL)
		{
			lpDDSPrimary->lpVtbl->Release(lpDDSPrimary);
			lpDDSPrimary = NULL;
		}
		if (lpBack_Buffer != NULL)
		{
			lpBack_Buffer->lpVtbl->Release(lpBack_Buffer);
			lpBack_Buffer = NULL;
		}
#if USE_ATI
		for(i=0;i<500;++i)
			if (gTex[i].lpDDSTex != NULL)
			{
				gTex[i].lpDDSTex->lpVtbl->Release(gTex[i].lpDDSTex);
			}
#endif
		InvalidateRect(GetDesktopWindow(), NULL, TRUE);
		lpDD->lpVtbl->Release(lpDD);
		lpDD = NULL;
	}
	ShellSetPrefs();
	SystemParametersInfo(SPI_SETKEYBOARDDELAY,Keydelay,0,0);
	SystemParametersInfo(SPI_SETKEYBOARDSPEED,Keyrepeat,0,0);
 	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,sSaver,0,0);
#if 0
	f1=fopen("CRAPFILE","w");
	for(i=0;i<10000;++i)
		if(crapper[i].size)
			fprintf(f1,"%x %x\n",crapper[i].size,crapper[i].ptr);
	fclose(f1);
#endif
}			       /* finiObjects */

void changeScreen(void)
{
	int i;

	if(rendering)
		EndOTag();
	if (lpDDSPrimary != NULL)
	{
		lpDDSPrimary->lpVtbl->Release(lpDDSPrimary);
		lpDDSPrimary = NULL;
	}
	if (lpBack_Buffer != NULL)
	{
		lpBack_Buffer->lpVtbl->Release(lpBack_Buffer);
		lpBack_Buffer = NULL;
	}
	for(i=0;i<500;++i)
		if (gTex[i].lpDDSTex != NULL)
		{
			ATI3DCIF_TextureUnreg(gTex[i].hTX);
			gTex[i].lpDDSTex->lpVtbl->Release(gTex[i].lpDDSTex);
			gTex[i].lpDDSTex = NULL;
		}
	WinWidth = reses[screenres].xsize;
	WinHeight = reses[screenres].ysize;
	WinHeightX2 = WinHeight * 2;
	setupScreens();
	restoreTextures();
}
	
long FAR PASCAL WindowProc(HWND hWnd, UINT message,
					   WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT     ps;
	// RECT	rc;
	// SIZE	size;
	char	    keyhit;
	short DevID;
	long NotifyMsg;
	unsigned short buttons;
	unsigned long xpos, ypos;
	char temp;

	switch (message)
	{

	case WM_CREATE:
		break;

	case WM_SETCURSOR:
		SetCursor(NULL);
		return TRUE;

	case WM_TIMER:
		break;

	case MM_JOY1MOVE:
		if (bActive)
		{
			if (etc_Joystick1Switch == _DEVICE_ON)
			{

				xpos = LOWORD(lParam);
				ypos = HIWORD(lParam);
				buttons = wParam;
				etc_Joy1.jX = (short)((float)xpos * joyModXVal);
				etc_Joy1.jY = (short)((float)ypos * joyModYVal);
				etc_Joy1.jDir = 0;
				etc_Joy1.jButton1 = (buttons & JOY_BUTTON1) ? 1 : 0;
				etc_Joy1.jButton2 = (buttons & JOY_BUTTON2) ? 1 : 0;

				etc_Joystick1Register = 0;
				etc_Joystick1Register |= etc_Joy1.jDir;
				etc_Joystick1Register |= etc_Joy1.jButton1 << etc_Joy1Config.JoyButton1;
				etc_Joystick1Register |= etc_Joy1.jButton2 << etc_Joy1Config.JoyButton2;

				//Transmit data to buffer
				etc_TransferBuffer1[0] = OK;
				etc_TransferBuffer1[1] = SONY_PAD;
				etc_TransferBuffer1[2] = (char) ~(etc_Joystick1Register >> 8);

				temp = ~etc_TransferBuffer1[3];			// This lot preserves the
				temp &= 0x9F;					// brake bits, which get
				temp |= (char) (etc_Joystick1Register  & 0xFF);	// lost somehow otherwise
				etc_TransferBuffer1[3] = ~temp;
				return(0);
			}
		}
		break;

	case MM_JOY1BUTTONDOWN:
	case MM_JOY2BUTTONDOWN:
		if (bActive)
		{
			if (etc_Joystick1Switch == _DEVICE_ON)
			{

				xpos = LOWORD(lParam);
				ypos = HIWORD(lParam);
				buttons = wParam;
				etc_Joy1.jX = (short)((float)xpos * joyModXVal);
				etc_Joy1.jY = (short)((float)ypos * joyModYVal);
				etc_Joy1.jDir = 0;

				if (buttons & JOY_BUTTON1CHG)
					etc_Joy1.jButton1 = 1;
				else
					etc_Joy1.jButton2 = 1;


				etc_Joystick1Register = 0;
				etc_Joystick1Register |= etc_Joy1.jDir;
				etc_Joystick1Register |= etc_Joy1.jButton1 << etc_Joy1Config.JoyButton1;
				etc_Joystick1Register |= etc_Joy1.jButton2 << etc_Joy1Config.JoyButton2;

				//Transmit data to buffer
				etc_TransferBuffer1[0] = OK;
				etc_TransferBuffer1[1] = SONY_PAD;
				etc_TransferBuffer1[2] = (char) ~(etc_Joystick1Register >> 8);

				temp = ~etc_TransferBuffer1[3];			// This lot preserves the
				temp &= 0x9F;					// brake bits, which get
				temp |= (char) (etc_Joystick1Register  & 0xFF);	// lost somehow otherwise
				etc_TransferBuffer1[3] = ~temp;
				return(0);
			}
		}
		break;

	case MM_JOY1BUTTONUP:
	case MM_JOY2BUTTONUP:
		if (bActive)
		{
			if (etc_Joystick1Switch == _DEVICE_ON)
			{

				xpos = LOWORD(lParam);
				ypos = HIWORD(lParam);
				buttons = wParam;
				etc_Joy1.jX = (short)((float)xpos * joyModXVal);
				etc_Joy1.jY = (short)((float)ypos * joyModYVal);
				etc_Joy1.jDir = 0;

				if (buttons & JOY_BUTTON1CHG)
					etc_Joy1.jButton1 = 0;
				else
					etc_Joy1.jButton2 = 0;

				etc_Joystick1Register = 0;
				etc_Joystick1Register |= etc_Joy1.jDir;
				etc_Joystick1Register |= etc_Joy1.jButton1 << etc_Joy1Config.JoyButton1;
				etc_Joystick1Register |= etc_Joy1.jButton2 << etc_Joy1Config.JoyButton2;

				//Transmit data to buffer
				etc_TransferBuffer1[0] = OK;
				etc_TransferBuffer1[1] = SONY_PAD;
				etc_TransferBuffer1[2] = (char) ~(etc_Joystick1Register >> 8);

				temp = ~etc_TransferBuffer1[3];			// This lot preserves the
				temp &= 0x9F;					// brake bits, which get
				temp |= (char) (etc_Joystick1Register  & 0xFF);	// lost somehow otherwise
				etc_TransferBuffer1[3] = ~temp;
				return(0);
			}
		}
		break;

	case WM_MOUSEMOVE:
	case WM_LBUTTONUP:
	case WM_LBUTTONDOWN:

	case WM_RBUTTONUP:
	case WM_RBUTTONDOWN:
		if (bActive) {
			etc_Mouse1Register = 0;
			if(etc_Mouse1Switch == _DEVICE_ON)
			{
				short xtemp,fwKeys;
				char temp;

				fwKeys = wParam;
				switch(message)
				{
					case WM_LBUTTONDOWN:
						fwKeys |= MK_LBUTTON;
						break;
					case WM_RBUTTONDOWN:
						fwKeys |= MK_RBUTTON;
						break;
				}
				xtemp=LOWORD(lParam);
				if(xtemp>255)
				{
					xtemp=255;
					SetCursorPos(255,100);
				}
				xtemp-=128;
				switch(mouse_sensitivity)
				{
					case MEDIUM_SENS:
						xtemp/=2;
						break;
					case LOW_SENS:
						xtemp/=4;
						break;
				}
				if(abs(xtemp)<10)
					xtemp=0;
				etc_Mouse1.mX = xtemp+128;

				etc_Mouse1.mDir = 0;
				etc_Mouse1.mButton1 = (fwKeys & MK_LBUTTON) ? 1 : 0;
				etc_Mouse1.mButton2 = (fwKeys & MK_MBUTTON) ? 1 : 0;
				etc_Mouse1.mButton3 = (fwKeys & MK_RBUTTON) ? 1 : 0;

				etc_Mouse1Register |= etc_Mouse1.mDir;
				etc_Mouse1Register |= etc_Mouse1.mButton1 << etc_Mouse1Config.MouseLeftButton;
				etc_Mouse1Register |= etc_Mouse1.mButton2 << etc_Mouse1Config.MouseMiddleButton;
				etc_Mouse1Register |= etc_Mouse1.mButton3 << etc_Mouse1Config.MouseRightButton;

				/* Transmit Data to data buffer. */
				etc_TransferBuffer1[0] = OK;
				etc_TransferBuffer1[1] = SONY_PAD;
		//		etc_TransferBuffer1[2] = (char) ~(etc_Mouse1Register >> 8);

				temp = ~etc_TransferBuffer1[3];			// This lot preserves the
				temp &= 0x9F;					// brake bits, which get
				temp |= (char) (etc_Mouse1Register & 0xFF);	// lost somehow otherwise
				etc_TransferBuffer1[3] = ~temp;
				return(0);
			}
		}
		break;
	case WM_SYSCHAR:
	case WM_CHAR:
		return(0);
	case WM_KEYUP:
	case WM_SYSKEYUP:
		if (bActive) {
			//if (HIWORD(lParam) & 0x100)
			//{
				//pmkey(0xe0, FALSE);
			//}
			keyhit =  (TCHAR)wParam;
			//keyhit |= 0x80;
			pmkey(keyhit, FALSE);

			return(0);
		}
		return(0);
		break;
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
	        if (shellMoviePlayingG)
	        {
        		ShellMovieFinished();
		}
		if (bActive)
		{
			if(MPEGplaying)
				MPEGplaying=FALSE;

			switch (wParam)
			{
			case VK_F12:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				return 0;
			case VK_F11:
				if((GetKeyState(VK_SHIFT)<0) && (GetKeyState(VK_CONTROL)<0))
					FrameState^=1;
				return(0);
			case VK_DELETE:
				return(0);
				
			}
			if(!(HIWORD(lParam) & 0x4000))
			{
				//if (HIWORD(lParam) & 0x100)
				//{
					//pmkey(0xe0, TRUE);
				//}
				keyhit = (TCHAR) wParam;
				//keyhit &= 0x7f;
				pmkey(keyhit, TRUE);
				return 0;
			}



		}
		return(0);
	case WM_PAINT:
		if (bActive)
		{
			BeginPaint(hWnd, &ps);
			/*
		 	* GetClientRect(hWnd, &rc); GetTextExtentPoint( ps.hdc,
		 	* szMsg, lstrlen(szMsg), &size ); SetBkColor( ps.hdc, RGB(
		 	* 0, 0, 255 ) ); SetTextColor( ps.hdc, RGB( 255, 255, 0 ) );
		 	* TextOut( ps.hdc, (rc.right - size.cx)/2, (rc.bottom -
		 	* size.cy)/2, szMsg, sizeof( szMsg )-1 );
		 	*/
			EndPaint(hWnd, &ps);
		}
		break;

	case WM_DESTROY:
		finiObjects();
		finiSndObjects();
		PostQuitMessage(0);
		QuitToDos();
		break;
	case WM_ACTIVATEAPP:
		bActive = wParam;
		if (FirstActivate)
			FirstActivate = FALSE;
		else {
			if (wParam) //activate
			{
				SystemParametersInfo(SPI_SETKEYBOARDDELAY,0,0,0);
				SystemParametersInfo(SPI_SETKEYBOARDSPEED,0,0,0);
				SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,0,0,0);
				restoreObjects();
				CDRestart();
				SetForegroundWindow(hWnd);
				UpdateNotes(1);//paused
				WinPaused = 0;
				InstallTimer();
				if ((!(ctrlNeedEndText|ctrlNeedWinLose|
					ctrlNeedRaceInfo|ctrlNeedNewHighScores|
					ctrlNeedRacePoints|ctrlNeedChampTable|
					ctrlNeedAllVenomComplete|ctrlNeedVenomComplete|
					ctrlNeedContinueGameConfirm|ctrlNeedInSingleGameConfirm|
					ctrlNeedInGameReset|ctrlNeedInGameConfirm|
					ctrlNeedDeSelectScreen|startIceText|
					startFireText))&&ctrlmode!=ATTRACT)
						ctrlchoice = 0;
				backFromDead = TRUE;
 			} else { //deactivate
				SystemParametersInfo(SPI_SETKEYBOARDDELAY,Keydelay,0,0);
				SystemParametersInfo(SPI_SETKEYBOARDSPEED,Keyrepeat,0,0);
				SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,sSaver,0,0);
				CDStop();
				UpdateNotes(0);//paused
				WinPaused = 1;
				RemoveTimer();
			}

		}
		break;


#if USE_ATI
	case MM_MCINOTIFY:
	case MCIWNDM_NOTIFYMODE:
		DevID = LOWORD(lParam);
		NotifyMsg = wParam;
		MPEGplaying = FALSE;
		switch (NotifyMsg)
		{
			case MCI_NOTIFY_ABORTED://stopped for some reason
          		if (shellMoviePlayingG)
          		{
            		ShellMovieFinished();
		        }
				break;
			case MCI_NOTIFY_FAILURE://device error
          		if (shellMoviePlayingG)
          		{
            		ShellMovieFinished();
		        }
				break;
			case MCI_NOTIFY_SUCCESSFUL://horay!
				if (DevID == wDeviceID)//CD
				{
					CDPlay(CurrTrkNo);
				}

          		if (shellMoviePlayingG)
          		{
            		ShellMovieFinished();
		        }

				break;
			case MCI_NOTIFY_SUPERSEDED://not needed any more
          		if (shellMoviePlayingG)
          		{
            		ShellMovieFinished();
		        }
				break;
			default:
				break;
		}
		break;
#endif
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}			       /* WindowProc */


/*  SetSurfaceIntoATI3DCIF - sets direct draw surface to CIF
 *  Parameters : lpDDS long point to the direct draw surface to select
 */
#if USE_ATI
BOOL SetSurfaceIntoATI3DCIF(LPDIRECTDRAWSURFACE lpDDS)
{
	// set the pointer to the frame buffer address of the back surface
	// for the rendering context
	DDSURFACEDESC ddsd;
	HRESULT       ddretval;
	C3D_EPIXFMT ePixF = C3D_EPF_RGB565;

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC));
	ddsd.dwSize = sizeof (ddsd);
	ddsd.ddpfPixelFormat.dwSize = sizeof( DDPIXELFORMAT );

	ddretval = lpDDS->lpVtbl->GetSurfaceDesc(lpDDS, &ddsd);
	ddretval = lpDDS->lpVtbl->Lock (lpDDS, NULL,
							&ddsd,
							DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
							NULL);
	if (ddretval == DDERR_SURFACELOST) lpDDS->lpVtbl->Restore (lpDDS);
	
	ddretval = lpDDS->lpVtbl->Unlock (lpDDS, NULL);

	ATI3DCIF_ContextSetState (ghRC, C3D_ERS_SURF_DRAW_PTR, (C3D_PRSDATA) &(ddsd.lpSurface));
	switch(ddsd.ddpfPixelFormat.dwRGBBitCount)
	{ // select pixel format from bpp of surface!
		case 8:
			ePixF = C3D_EPF_RGB332;
			break;
		case 16:
			if ((((ddsd.ddpfPixelFormat.dwRBitMask) |
				  (ddsd.ddpfPixelFormat.dwGBitMask) |
				  (ddsd.ddpfPixelFormat.dwBBitMask)) &
				 (0x8000)) != 0)
			{ // check for 15 / 16 bpp through alpha bit!
				ePixF = C3D_EPF_RGB565;
			} else
			{
				ePixF = C3D_EPF_RGB1555;
			}
			break;
		case 32:
			ePixF = C3D_EPF_RGB8888;
			break;
		default:
			break;
	}
	
	ATI3DCIF_ContextSetState (ghRC, C3D_ERS_SURF_DRAW_PITCH, (C3D_PRSDATA)&(ddsd.dwWidth));
	ATI3DCIF_ContextSetState (ghRC, C3D_ERS_SURF_DRAW_PF, (C3D_PRSDATA)&(ePixF));

	return(TRUE);
}
#endif

void setupScreens(void)
{
	DDSURFACEDESC   ddsd;
	DDSCAPS	 ddscaps;
	HRESULT	 ddrval,ddretval;
	HDC hdc;
	RECT rc;
	int i;
	
#if USE_ATI
	ddrval = lpDD->lpVtbl->SetDisplayMode(lpDD, WinWidth, WinHeight, 16);
#else
	ddrval = lpDD->lpVtbl->SetDisplayMode(lpDD, WinWidth, WinHeight, 8);
#endif
	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rc);
	PatBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, BLACKNESS);
	ReleaseDC(hwnd , hdc);
	if (ddrval == DD_OK)
	{
#if !USE_ATI
		ddrval = lpDD->lpVtbl->CreatePalette(lpDD, DDPCAPS_8BIT, ddPalette, &lpDDPalette, NULL);
		if (ddrval == DD_OK)
		{
#endif
			// Create the primary surface with 1 back buffer
			memset(&ddsd, 0, sizeof(ddsd));
			ddsd.dwSize = sizeof(ddsd);
			ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
				DDSCAPS_FLIP |
				DDSCAPS_VIDEOMEMORY |
				DDSCAPS_COMPLEX;
#if USE_ATI
			ddsd.dwBackBufferCount = 1;
#else
			ddsd.dwBackBufferCount = 2;
#endif
			ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &lpDDSPrimary, NULL);

#if USE_ATI
			SetSurfaceIntoATI3DCIF(lpDDSPrimary);
#endif

#if !USE_ATI
			if (ddrval != DD_OK)
			{
				// Want at least 1 BackBuffer
				//   in VRAM
				ddsd.dwBackBufferCount = 1;
				ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &lpDDSPrimary, NULL);
			}
			if (ddrval != DD_OK)
			{
				// Settle for what we can get
				ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
				ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &lpDDSPrimary, NULL);
			}
#endif
			if (ddrval != DD_OK)
			{
				ShowErrorMessage(4001);
			}
#if !USE_ATI
			/* set palette for this surface */
			lpDDSPrimary->lpVtbl->SetPalette(lpDDSPrimary, lpDDPalette);
#endif
			// Get a pointer to the back buffer
			memset(&ddscaps, 0, sizeof(ddscaps));
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
			ddrval = lpDDSPrimary->lpVtbl->GetAttachedSurface(lpDDSPrimary,
						   &ddscaps,
						&lpDDSBack);
			if (ddrval != DD_OK)
			{
				ShowErrorMessage(4002);
			}
#if USE_ATI
#endif
#if !USE_ATI
		}
#endif
	}
	DDSwap();
	WinClearScreen(0);
	DDSwap();
	WinClearScreen(0);
	switch(screenres)
	{
		case SMALL:
			switch(memorySize)
			{
				case 0:
					trackSize=1;
					otherSize=0;
					break;
				case 1:
				case 2:
					trackSize=1;
					otherSize=1;
					break;
			}
			break;
		case LOW:
		case MEDIUM:
			switch(memorySize)
			{
				case 0:
					trackSize=0;
					otherSize=0;
					break;
				case 1:
				case 2:
					trackSize=1;
					otherSize=1;
					break;
			}
			break;
		case HIGH:
			switch(memorySize)
			{
				case 1:
					trackSize=1;
					otherSize=0;
					break;
				case 2:
					trackSize=1;
					otherSize=1;
					break;
			}
			break;
	}
	upres=reses[screenres].mult;
	// create 3 offscreen surfaces to cache the sprite maps
	for(i=420;i<423;++i)
	{
		ZeroMemory (&(gTex[i].ddsd), sizeof (gTex[i].ddsd));
		gTex[i].ddsd.dwSize = sizeof (gTex[i].ddsd);
		gTex[i].ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		gTex[i].ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		gTex[i].ddsd.dwWidth = 256;//SJR	// standard texture size
		gTex[i].ddsd.dwHeight = 256;//SJR	// standard height
		ddretval = lpDD->lpVtbl->CreateSurface (lpDD,&(gTex[i].ddsd), &gTex[i].lpDDSTex, NULL);
		if (ddretval != DD_OK)
		{
			ShowErrorMessage(4002);
		}
	}
	JJSLoadSprites();
}
/*
 * doInit - do work required for every instance of the application: create
 * the window, initialize data
 */
static BOOL     doInit(HANDLE hInstance, int nCmdShow)
{
	WNDCLASS	wc;
	HRESULT	 ddrval;
	C3D_PVTCF *p;
	int	     i;
	HDC hdc;
	RECT rc;
	/*
	 * Initialise palette
	 */

	for (i = 0; i < 256; i++)
	{
		ddPalette[i].peRed = i;
		ddPalette[i].peGreen = i;
		ddPalette[i].peBlue = i;
		ddPalette[i].peFlags = 0;
	}

	/*
	 * set up and register window class
	 */
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = NULL;
	wc.lpszMenuName = NAME;
	wc.lpszClassName = NAME;
	RegisterClass(&wc);

	/*
	 * create a window
	 */



	hwnd = CreateWindowEx(
			      WS_EX_TOPMOST,
			      NAME,
			      TITLE,
			      WS_POPUP,
			      0, 0,
			      GetSystemMetrics(SM_CXSCREEN),
			      GetSystemMetrics(SM_CYSCREEN),
			      NULL,
			      NULL,
			      hInstance,
			      NULL);

	if (!hwnd)
	{
		ShowErrorMessage(2000);
		return FALSE;
	}
	gInstance = hInstance;

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	hdc = GetDC(hwnd);
	GetClientRect(hwnd, &rc);
	PatBlt(hdc, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, BLACKNESS);
	ReleaseDC(hwnd , hdc);
	
	mpeganim();

	/*
	 * create the main DirectDraw object
	 */
	ddrval = DirectDrawCreate(NULL, &lpDD, NULL);
	if (ddrval == DD_OK)
	{
		// Get exclusive mode
#if USE_ATI
		ddrval = lpDD->lpVtbl->SetCooperativeLevel(lpDD, hwnd,
					DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
#else
		ddrval = lpDD->lpVtbl->SetCooperativeLevel(lpDD, hwnd,
			DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX );
#endif
		if (ddrval == DD_OK)
		{
			setupScreens();
		}
#if USE_ATI
		for (i = 0, p = testlist;i<VBUFSIZE;++i)
			*p++=&testarray[i];
#endif
	}
#if USE_ATI
	ShowCursor(FALSE);
	JJSLoadRage();
#endif
	return TRUE;
}			       /* doInit */

void	    CopyPal(char *palette, short start, short end)
{
	int	     i;

	for (i = start; i <= end; i++)
	{
		ddPalette[i].peRed = *palette++;
		ddPalette[i].peGreen = *palette++;
		ddPalette[i].peBlue = *palette++;
		ddPalette[i].peFlags = 0;
#if USE_ATI
		ATIpalette[i].red=(ddPalette[i].peRed)>>3;
		ATIpalette[i].green=(ddPalette[i].peGreen)>>3;
		ATIpalette[i].blue=(ddPalette[i].peBlue)>>3;
#endif
	}
#if !USE_ATI
	ddrval = lpDDPalette->lpVtbl->SetEntries(lpDDPalette,
						 DDSETPAL_IMMEDIATE,
						 start,
						 (end - start) + 1,
						 ddPalette);
#endif
}

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

int randy()
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

C3D_UINT32 vRamSize()
{
	C3D_3DCIFINFO meminfo;

	ZeroMemory(&meminfo, sizeof(C3D_3DCIFINFO));
	meminfo.u32Size = sizeof(C3D_3DCIFINFO);
	ATI3DCIF_GetInfo(&meminfo);
	return meminfo.u32TotalRAM;
}

/*
 * WinMain - initialization, message loop
 */
int PASCAL      WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
					LPSTR lpCmdLine, int nCmdShow)
{
	long tmp;
	int retval;

	SystemParametersInfo(SPI_GETKEYBOARDDELAY,0,&Keydelay,0);
	SystemParametersInfo(SPI_GETKEYBOARDSPEED,0,&Keyrepeat,0);
	SystemParametersInfo(SPI_GETSCREENSAVEACTIVE,0,&sSaver,0);
	SystemParametersInfo(SPI_SETKEYBOARDDELAY,0,0,0);
	SystemParametersInfo(SPI_SETKEYBOARDSPEED,0,0,0);
	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,0,0,0);
	// create an ATI CIF redering context:
	if (!InitATI3DCIF())
	{
		ShowErrorMessage(5000);
		return FALSE;
	}
	if ( CWDDE_Init () == TRUE) 

	{
		CWDDE_GetMemRegPtr(&Ptr48);
		CWDDE_Free ();
	}

	tmp=vRamSize();
	if(tmp<2300000)
		memorySize=0;
	else
	if(tmp<4500000)
		memorySize=1;
	else
		memorySize=2;
	initrandy();
	tmp = randy();
	lpCmdLine = lpCmdLine;
	hPrevInstance = hPrevInstance;
#if DEBUG_LOCKED
	DebugScreen = (char *) malloc(640 * 480);
#endif
	TextureBuffer = malloc(TEXTURESJR);
	WinWidth = reses[screenres].xsize;
	WinHeight = reses[screenres].ysize;
	WinHeightX2 = WinHeight * 2;
	hinst = hInstance;
	SaveDirBase = (char *)malloc(120);
	retval = ShellGetGameDirectory(SaveDirBase);
	if (retval != ERROR_SUCCESS)
		strcpy(SaveDirBase, "c:\wipeout");

	DataDirBase = (char *)malloc(120);
	retval = ShellGetDataDirectory(DataDirBase);
	if (retval != ERROR_SUCCESS)
		strcpy(DataDirBase, "c:\wipeout");

	MovieDirBase = (char *)malloc(120);
	retval = ShellGetMovieDirectory(MovieDirBase);
	if (retval != ERROR_SUCCESS)
		strcpy(MovieDirBase, "c:\wipeout");

	chdir(DataDirBase);
	// Initialise Direct Draw
	if (!doInit(hInstance, nCmdShow))
	{
		return FALSE;
	}

	CDStop();
	oldmain();
  	ShowCursor(TRUE);
  	joyReleaseCapture(JOYSTICKID1);
	DestroyWindow(hwnd);
	return (0);

}			       /* WinMain */

int	     ProcessMessages(void)
{
	MSG	     msg;
	do
	{
		if(backFromDead)
		{
			backFromDead=FALSE;
			while(PeekMessage(&msg,NULL,MM_JOY1MOVE,MM_JOY2BUTTONUP,PM_REMOVE));
		}			
		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))      // Clear all outstanding messages
		{
			if (GetMessage(&msg, NULL, 0, 0))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
				if(PeekMessage(&msg,NULL,MM_JOY1MOVE,MM_JOY2BUTTONUP,PM_REMOVE))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
				if (bActive)
					return (0);
			}
			else
			{
				if (bActive)
					return (1);
			}
		}
	} while (!bActive);

	if (!bActive)
		ProcessMessages();
	return (0);
}

DDSURFACEDESC   ddsd;
DDSURFACEDESC   backDdsd;
int	     old_pitch = 0;

int	     LockBackBuffer(void)
{
	LPBYTE	  ptr;
	// int			i;
	HRESULT	 ddrval;
	if (bActive && !backLocked)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		ddrval = lpDDSBack->lpVtbl->Lock(lpDDSBack, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
#if DEBUG_LOCKED
		WinPitch = WinWidth;
		RealPitch = ddsd.lPitch;
#else
		WinPitch = ddsd.lPitch;
#endif
		if (WinPitch != old_pitch)      // pitch has changed
		{
			ResetPitch();
			old_pitch = WinPitch;
		}

		if (ddrval == DDERR_SURFACEBUSY)
		{
			MessageBeep(0);
			return (1);
		}
		if (ddrval == DDERR_SURFACELOST)
		{
			MessageBeep(0);
			lpDDSBack->lpVtbl->Restore(lpDDSBack);
			return (1);
		}

		ptr = (LPBYTE) ddsd.lpSurface;
		if (ptr == NULL)
			return (1);
		// Set my render buffers to the back screen
#if DEBUG_LOCKED
		CurBuffer = (char *) DebugScreen;
		CurBufferJ = (char *) DebugScreen;
		RealScreen = (char *) ptr;
#else
		CurBuffer = (char *) ptr;
		CurBufferJ = (char *) ptr;
#endif
		backLocked=TRUE;
	}

	return (0);

}

void	    WinBeep(void)
{
	MessageBeep(0);
}
void	    UnlockBackBuffer(void)
{
#if DEBUG_LOCKED
	RECT	    rect;

	rect.left = 0;
	rect.top = 0;
	rect.right = WinWidth;
	rect.bottom = WinHeight;

	// memcpy(RealScreen, (DebugScreen + (640*150)), ((WinHeight - 150) *	   WinWidth));
	PitchMemCpy(RealScreen, DebugScreen, &rect);
	memset(DebugScreen, 0, (WinHeight * WinWidth));
#endif
	if (bActive && backLocked)
	{
		lpDDSBack->lpVtbl->Unlock(lpDDSBack, ddsd.lpSurface);
		backLocked = FALSE;
	}
}


int	     LockClearBuffer(void)
{
	LPBYTE	  ptr;
	// int			i;
	HRESULT	 ddrval;
	if(!lpBack_Buffer)
	{
		DDSURFACEDESC       ddsd;

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = reses[screenres].xsize;
#if USE_ATI
		ddsd.dwHeight = reses[screenres].ysize;
#else
		ddsd.dwHeight = 200;
#endif
		ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &lpBack_Buffer, NULL);
		if (ddrval != DD_OK)
		{
			ShowErrorMessage(4002);
			return (FALSE);
		}
	}
	if (bActive)
	{
		ZeroMemory(&backDdsd, sizeof(backDdsd));
		backDdsd.dwSize = sizeof(backDdsd);

		ddrval = lpBack_Buffer->lpVtbl->Lock(lpBack_Buffer, NULL, &backDdsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

		if (ddrval == DDERR_SURFACEBUSY)
		{
			MessageBeep(0);
			return (1);
		}
		if (ddrval == DDERR_SURFACELOST)
		{
			MessageBeep(0);
			lpBack_Buffer->lpVtbl->Restore(lpBack_Buffer);
			return (1);
		}

		RealBackPitch = backDdsd.lPitch;
		ptr = (LPBYTE) backDdsd.lpSurface;
		if (ptr == NULL)
			return (1);
		// Set my render buffers to the back screen
#if DEBUG_LOCKED
		backscreen = (char *) DebugScreen;
#else
		backscreen = (char *) ptr;
#endif
	}

	return (0);

}

void	    UnlockClearBuffer(void)
{
#if DEBUG_LOCKED
	memcpy(RealScreen, DebugScreen, 320 * 240);
#endif
	if (bActive)
	{
		lpBack_Buffer->lpVtbl->Unlock(lpBack_Buffer, backDdsd.lpSurface);
	}
}

void	    BltClearToScreen(void)
{
	RECT	    dest_rect, src_rect;
	DWORD	   dwFlags;
	DDBLTFX	 DDBltFX;
	HRESULT	 ddrval;

	dest_rect.top = dest_rect.left = 0;
#if USE_ATI
	dest_rect.bottom = WinHeight;
#else
	dest_rect.bottom = 199; //WinHeight;
#endif
	dest_rect.right = WinWidth;

	src_rect.top = src_rect.left = 0;
#if USE_ATI
	src_rect.bottom = WinHeight;
#else
	src_rect.bottom = 199;
#endif
	src_rect.right = WinWidth;

	DDBltFX.dwSize = sizeof(DDBltFX);

	dwFlags = DDBLT_WAIT;

	ddrval = lpDDSBack->lpVtbl->Blt(lpDDSBack, &dest_rect, lpBack_Buffer, &src_rect, dwFlags, &DDBltFX);
	if (ddrval != DD_OK)
	{
		// Msg("Blt copy failed err=%d", ddrval);
		switch (ddrval)
		{
		case DDERR_SURFACELOST:
			MessageBeep(0);
			break;
		case DDERR_SURFACEBUSY:
			MessageBeep(0);
			break;
		}
		;
	}
}

void	    DDSwap(void)
{
	if (bActive)
	{
		while (1)
		{
			HRESULT	 ddrval;
			ddrval = lpDDSPrimary->lpVtbl->Flip(lpDDSPrimary, NULL, 0);
			if (ddrval == DD_OK)
			{
				break;
			}
			if (ddrval == DDERR_SURFACELOST)
			{
				ddrval = lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary);
				if (ddrval != DD_OK)
				{
					break;
				}
			}
			if (ddrval != DDERR_WASSTILLDRAWING)
			{
				break;
			}
		}
	}
}

void	    DrawIT(void)
{
	DDSURFACEDESC   ddsd;
	LPBYTE	  ptr;
	// int			i;
	HRESULT	 ddrval;
	static int      err_count = 0;

	// Flip surfaces
	if (bActive)
	{
		ZeroMemory(&ddsd, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);

		ddrval = lpDDSBack->lpVtbl->Lock(lpDDSBack, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

		if (ddrval == DDERR_SURFACEBUSY)
		{
			err_count++;
		}
		if (ddrval == DDERR_SURFACELOST)
		{
			MessageBeep(0);
			lpDDSBack->lpVtbl->Restore(lpDDSBack);
		}

		ptr = (LPBYTE) ddsd.lpSurface;
		if (ptr == NULL)
			return;
		// Set my render buffers to the back screen
		CurBuffer = (char *) ptr;
		CurBufferJ = (char *) ptr;

		// Render_Triangles();
		// Move_Triangles();
		lpDDSBack->lpVtbl->Unlock(lpDDSBack, ddsd.lpSurface);

		while (1)
		{
			HRESULT	 ddrval;
			ddrval = lpDDSPrimary->lpVtbl->Flip(lpDDSPrimary, NULL, 0);
			if (ddrval == DD_OK)
			{
				break;
			}
			if (ddrval == DDERR_SURFACELOST)
			{
				ddrval = lpDDSPrimary->lpVtbl->Restore(lpDDSPrimary);
				if (ddrval != DD_OK)
				{
					break;
				}
			}
			if (ddrval != DDERR_WASSTILLDRAWING)
			{
				break;
			}
		}
	}




}

void	    WinQuit(void)
{
#if DEBUG_LOCKED
	free(DebugScreen);
#endif
	finiObjects();
	finiSndObjects();
	PostQuitMessage(0);
}

void	    WinClearScreen(char colour)
{
	DDBLTFX	 ddbltfx;

	memset(&ddbltfx,0,sizeof(ddbltfx));
	ddbltfx.dwSize = sizeof(ddbltfx);
	ddbltfx.dwFillColor = colour;
	lpDDSBack->lpVtbl->Blt(lpDDSBack, NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
}

void	    PitchMemCpy(char *dest, char *source, RECT * rect)
{
	int	     loop;

	for (loop = 0; loop < (rect->bottom - rect->top); loop++)
	{
		memcpy(dest, source, (rect->right - rect->left));
		dest += RealPitch;
		source += (rect->right - rect->left);
	}
}

void	    PitchMemCpy320X200(char *dest, char *source)
{
	RECT	    rect;
	int	     loop;

	rect.left = 0;
	rect.right = WinWidth;
	rect.top = 0;
	rect.bottom = WinHeight-1;
	for (loop = 0; loop < (rect.bottom - rect.top); loop++)
	{
		memcpy(dest, source, (rect.right - rect.left));
		dest += RealBackPitch;
		source += (rect.right - rect.left);
	}
}

#if USE_ATI

void SetContext(void)
{
	ATI3DCIF_ContextSetState(ghRC, C3D_ERS_SURF_DRAW_PTR,
					   (C3D_PRSDATA) & CurBuffer);
}

/*
 *  CloseATI3DCIF - destroy the rendering context and unload the ATI3DCIF module
 */
void CloseATI3DCIF (void)
{
	// destroy ATI 3D rendering context
	if (ghRC) ATI3DCIF_ContextDestroy (ghRC);

	// terminate the ATI 3D driver
	if (gbCIFInit) ATI3DCIF_Term ();
}

/*
 * InitATI3DCIF - create and initialize the ATI 3D rendering context
 */
BOOL	    InitATI3DCIF(void)
{
	// initialize the CIF driver
	if (ATI3DCIF_Init() != C3D_EC_OK)
	{
		wsprintf(gszErrMsg, "Could not initialize 3D driver");
		return FALSE;
	}
	// set flag indicating driver is loaded
	gbCIFInit = TRUE;

	if (vRamSize() < 2000000)
	{
		ShowErrorMessage(1004);
		return FALSE;
	}

	// create the rendering context
	ghRC = ATI3DCIF_ContextCreate();
	if (!ghRC)
	{
		wsprintf(gszErrMsg, "Could not create 3D rendering context");
		return FALSE;
	}
	return TRUE;
}

long numoff=0;
long numsmall=0;

void setRenderBegin(void)
{
	if(!Rendering)
	{
		ATI3DCIF_RenderBegin(ghRC);
		Rendering=TRUE;
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_PRIM_TYPE,
						   &quad);
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_SHADE_MODE,
						   &shade);
		bTexEn = FALSE;
		texpage = 420; 
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &(gTex[texpage].hTX));
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_EN, &bTexEn);
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_PERSP_COR, &ePerspCorOn);
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_LIGHT, &light);
		if (ft&1)
			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteron);
		else
			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteroff);
		trans = FALSE;
	}
}


void setRenderEnd(void)
{
	if(Rendering)
	{
		ATI3DCIF_RenderEnd();
		Rendering=FALSE;
	}
}


void sendList(void)
{
	WORD temp;

	if(numvert)
	{
		setRenderBegin();
		ATI3DCIF_RenderPrimList(
					  testlist,
					 numvert);
		
	numvert=0;
	}
}

short jjstestsrc=0,jjstestdst=0,jjstest3=128;


BOOL SetState(C3D_EPRIM Quad, C3D_ESHADE Shade, BOOL BTexEn, BOOL Trans, C3D_ETLIGHT Light)
{
	BOOL change=FALSE,tranfix=FALSE;
	C3D_ETEXOP chkey = C3D_ETEXOP_NONE;
	static BOOL FiltOn=FALSE;

	if(Quad!=quad)
	{
		sendList();
		quad = Quad;
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_PRIM_TYPE,
						   &quad);
		change = TRUE;
	}
	if(Shade != shade)
	{
		if(!change)
			sendList();
		shade = Shade;
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_SHADE_MODE,
						   &shade);
		change = TRUE;
	}
	if(BTexEn != bTexEn)
	{
		if(!change)
			sendList();
		bTexEn = BTexEn;
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_EN, &bTexEn);
		change = TRUE;
	}
	if(Light != light)
	{
		if(!change)
			sendList();
		light = Light;
		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_LIGHT, &light);
		change = TRUE;
	}
	if(Trans != trans)
	{
		if(!change)
			sendList();
		trans = Trans;
		if(trans)
		{
			switch(trans)
			{
				case BLACK_NOT_DRAWN:
					chkey = C3D_ETEXOP_CHROMAKEY;//SJR - make 0,0,0 trans
					eDstAlpha = C3D_EADST_ZERO;
					eSrcAlpha = C3D_EASRC_ONE;
					break;
				case BLACK5_TRANSL:
				case RED_TRANSL:
				case BLACK1_TRANSL:
				case BLACK2_TRANSL:
				case BLACK4_TRANSL:		
				case BLACK6_TRANSL:
					tranfix=TRUE;
					chkey = C3D_ETEXOP_CHROMAKEY;//SJR - make 0,0,0 trans
				case BLACK3_TRANSL:
					eDstAlpha = C3D_EADST_ZERO;
					eSrcAlpha = C3D_EASRC_DSTCLR;
					break;
				case WHITE1_TRANSL:
				case WHITE2_TRANSL:
				case WHITE3_TRANSL:
				case WHITE4_TRANSL:
				case WHITE5_TRANSL:
				case WHITE6_TRANSL:
				case WHITE7_TRANSL:
				case WHITE8_TRANSL:
				case UNUSED1_TRANSL:
				default:
					eDstAlpha = C3D_EADST_ONE;
					eSrcAlpha = C3D_EASRC_INVDSTCLR;
					
					break;
			}
		}
		else
		{
			eDstAlpha = C3D_EADST_ZERO;
			eSrcAlpha = C3D_EASRC_ONE;
		}
		if(tranfix && ft && FiltOn)
		{
			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteroff);
			FiltOn=FALSE;
		}
		if(!tranfix && ft && !FiltOn)
		{
			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteron);
			FiltOn=TRUE;
		}
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_TEXOP, &chkey);
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_ALPHA_DST, &eDstAlpha);
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_ALPHA_SRC, &eSrcAlpha);

		change = TRUE;
	}
	return(change);
}

short GetThePage(short page,short u,short v)
{
	short newpage;
	char *tex=menuTexturesIn?JJSTextureMenuTable:JJSTextureTable;
	
	newpage=tex[page&0x1f];
	if(newpage>4)
		newpage+=400;
	else
	if(!menuTexturesIn)
	{
		PageXOff=u>>5;
		PageYOff=v>>5;
		newpage*=64;
		newpage+=PageXOff;
		newpage+=PageYOff<<3;
		PageXOff<<=5;
		PageYOff<<=5;
	}
	return(newpage);
}

void JJSDrawSprite(SPRT_8 *sp,short w, short h)
{
	C3D_PVTCF       v;
	//C3D_PRSDATA chkey;
	short tpage;
	short nu,nv;
	SPRT_8 s2=*sp;
	
	nu=s2.u0;
	nv=s2.v0;
	if(s2.y0<0)
	{
		nv-=s2.y0;
		h+=s2.y0;
		s2.y0=0;
	}
	else
	if(s2.y0+h>=reses[screenres].ysize)
	{
		if(s2.y0>=reses[screenres].ysize)
			return;
		h=reses[screenres].ysize-s2.y0;
	}
	tpage=GetThePage(s2.clut,nu,nv);
	if(tpage==415)
		tpage=422;
	if(s2.g0 && tpage == 421)
	{
		tpage = 422;
		nv+=128;
	}
	if(texpage !=tpage)
	{
		sendList();
		texpage=tpage;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &(gTex[texpage].hTX));
	}
	if(!SetState(C3D_EPRIM_QUAD,C3D_ESH_FLAT,TRUE,s2.b0==RED_TRANSL?RED_TRANSL:BLACK_NOT_DRAWN,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
	v = &testarray[numvert];
	v->x = s2.x0;
	v->y = s2.y0;
	v->z = 0;
	v->s = nu/256.0;
	v->t = nv/256.0;
	v->w = 1.0;
	++v;
	v->x = s2.x0+w;
	v->y = s2.y0;
	v->z = 0;
	v->s = (nu+w)/256.0;
	v->t = nv/256.0;
	v->w = 1.0;
	++v;
	v->x = s2.x0+w;
	v->y = s2.y0+h;
	v->z = 0;
	v->s = (nu+w)/256.0;
	v->t = (nv+h)/256.0;
	v->w = 1.0;
	++v;
	v->x = s2.x0;
	v->y = s2.y0+h;
	v->z = 0;
	v->s = nu/256.0;
	v->t = (nv+h)/256.0;
	v->w = 1.0;
	numvert+=4;
}

void JJS_Sprite(SPRT *sp)
{
	JJSDrawSprite((SPRT_8 *)sp,sp->w,sp->h);
}

void JJS_Sprite8(SPRT_8 *sp)
{
	JJSDrawSprite(sp,8,8);
}

void SetColor(C3D_PVTCF v,char r, char g, char b)
{
	if(g)
	{
		int r;
		
		r=(&DepthFadeTble)[(g<<8) + WhitePal];
		v->r=ddPalette[r].peRed;
		v->g=ddPalette[r].peGreen;
		v->b=ddPalette[r].peBlue;
	}
	else
	if(b)
	{
		switch(b)
		{
			case BLACK2_TRANSL:
				v->r=180;
				v->g=180;
				v->b=180;
				break;
			case RED_LIGHT_FADE:
				v->r=255;
				v->g=10;
				v->b=0;
				break;
			case WHITE2_TRANSL:
			default:
				v->r=100;
				v->g=100;
				v->b=100;
				break;
		}
	}
	else
	{
		v->r=ddPalette[r].peRed;
		v->g=ddPalette[r].peGreen;
		v->b=ddPalette[r].peBlue;
		
	}
}
	
void	    JJSDrawPolyF4(POLY_F4 * p)
{
	C3D_PVTCF       v;

	if(!SetState(C3D_EPRIM_QUAD,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();


	v = &testarray[numvert];
	v->x = SCALEIT(p->x0);
	v->y = SCALEIT(p->y0);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x1);
	v->y = SCALEIT(p->y1);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x3);
	v->y = SCALEIT(p->y3);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x2);
	v->y = SCALEIT(p->y2);
	v->z = 0;
	SetColor(v,p->r0,p->g0,p->b0);
	numvert+=4;
}

void	    JJSDrawPolyG4(POLY_G4 * p)
{
	C3D_PVTCF       v;

	if(!SetState(C3D_EPRIM_QUAD,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
	v = &testarray[numvert];
	v->x = SCALEIT(p->x0);
	v->y = SCALEIT(p->y0);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x1);
	v->y = SCALEIT(p->y1);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x3);
	v->y = SCALEIT(p->y3);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x2);
	v->y = SCALEIT(p->y2);
	v->z = 0;
	SetColor(v,p->r0,p->g0,p->b0);
	numvert+=4;
}

void setst(C3D_PVTCF vv,short u,short v,short page)
{

	if(menuTexturesIn || page>=400) 
	{
		//vv->s=(u*(1.0/256.0)+(DIVIS)) * vv->w;
		//vv->t=(v*(1.0/256.0)+(DIVIS)) * vv->w;
		vv->s=(u*vv->w * (1.0/256.0))+DIVIS;
		vv->t=(v* vv->w * (1.0/256.0))+DIVIS;
	}
	else
	{
		vv->s=((u-PageXOff)*vv->w * (1.0/32.0));
		vv->t=((v-PageYOff)*vv->w * (1.0/32.0));

		/*if(vv->s<0.0)
			vv->s=0.0;
		if(vv->s>=1.00)
			vv->s=.999999;
		if(vv->t<0.0)
			vv->s=0.0;
		if(vv->t>=1.00)
			vv->s=.999999;*/
	}

}

void	    JJSDrawPolyFT4(POLY_FT4 * p)
{
	C3D_PVTCF       v;
	//C3D_PRSDATA chkey;
	short temp;

	temp = GetThePage(p->tpage,(p->u0+p->u1+p->u2+p->u3)/4,(p->v0+p->v1+p->v2+p->v3)/4);
	if(temp==417 || temp==418)	// nasty horrible bodge
	{
		if(p->u0==0 && p->v0==24)
		{
			if(temp==417)
			{
				p->u0=0;
				p->v0=64;
				p->u1=127;
				p->v1=64;
				p->u2=0;
				p->v2=203;
				p->u3=127;
				p->v3=203;
			}
			else
			{
				p->u0=128;
				p->v0=64;
				p->u1=255;
				p->v1=64;
				p->u2=128;
				p->v2=203;
				p->u3=255;
				p->v3=203;
			}
			temp=420;
		}
	}
	if(texpage != temp)
	{
		sendList();
		texpage=temp;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &(gTex[texpage].hTX));
	}
	if(!SetState(C3D_EPRIM_QUAD,C3D_ESH_FLAT,TRUE,p->b0,p->g0?C3D_ETL_MODULATE:C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
	if(p->pad4 & 0x4000)
	{
		if(!ft&1)
		{
			sendList();
			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteron);
		}
	}

	v = &testarray[numvert];
	v->z = SCALEZ(p->z0);
	if (p->pad4 & PERSPECTIVE_CORRECT)
	{
		if (p->z0 <= 0)
			v->w = 1.0;
		else
			v->w = (float)screenZ / (float)p->z0;
	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x0);
	v->y = SCALEIT(p->y0);
	setst(v,p->u0,p->v0,texpage);

	++v;
	v->z = SCALEZ(p->z1);
	if (p->pad5 & PERSPECTIVE_CORRECT)
	{
		if (p->z1 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z1;

	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x1);
	v->y = SCALEIT(p->y1);
	setst(v,p->u1,p->v1,texpage);

	++v;
	v->z = SCALEZ(p->z3);
	if (p->pad7 & PERSPECTIVE_CORRECT)
	{
		if (p->z3 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z3;

	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x3);
	v->y = SCALEIT(p->y3);
	setst(v,p->u3,p->v3,texpage);


	++v;
	v->z = SCALEZ(p->z2);
	if (p->pad6 & PERSPECTIVE_CORRECT)
	{
		if (p->z2 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z2;

	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x2);
	v->y = SCALEIT(p->y2);
	setst(v,p->u2,p->v2,texpage);

	if (p->g0)
		SetColor(v,p->r0,p->g0,p->b0);
	numvert+=4;
	if(p->pad4 & 0x4000)
	{
		if (!ft&1)
		{
			sendList();
			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteroff);
		}
	}
}

void	    JJSDrawPolyF3(POLY_F4 * p)
{
	C3D_PVTCF       v;

	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();

	v = &testarray[numvert];
	v->x = SCALEIT(p->x0);
	v->y = SCALEIT(p->y0);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x1);
	v->y = SCALEIT(p->y1);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x2);
	v->y = SCALEIT(p->y2);
	v->z = 0;
	SetColor(v,p->r0,p->g0,p->b0);
	numvert += 3;
}

void	    JJSDrawPolyG3(POLY_G3 * p)
{
	C3D_PVTCF       v;

	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();

	v = &testarray[numvert];
	v->x = SCALEIT(p->x0);
	v->y = SCALEIT(p->y0);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x1);
	v->y = SCALEIT(p->y1);
	v->z = 0;
	++v;
	v->x = SCALEIT(p->x2);
	v->y = SCALEIT(p->y2);
	v->z = 0;
	SetColor(v,p->r0,p->g0,p->b0);
	numvert += 3;
}

void	    JJSDrawPolyFT3(POLY_FT3 * p)
{
	C3D_PVTCF       v;
	short temp;

	temp=GetThePage(p->tpage,(p->u0+p->u1+p->u2)/3,(p->v0+p->v1+p->v2)/3);
	if(texpage!=temp)
	{
		sendList();
		texpage=temp;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &(gTex[texpage].hTX));
	}
	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,TRUE,p->b0,p->g0?C3D_ETL_MODULATE:C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();

	v = &testarray[numvert];
	v->z = SCALEZ(p->z0);
	if (p->pad4 & PERSPECTIVE_CORRECT)
	{
		if (p->z0 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z0;
	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x0);
	v->y = SCALEIT(p->y0);
	setst(v,p->u0,p->v0,texpage);

	++v;
	v->z = SCALEZ(p->z1);
	if (p->pad5 & PERSPECTIVE_CORRECT)
	{
		if (p->z1 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z1;
	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x1);
	v->y = SCALEIT(p->y1);
	setst(v,p->u1,p->v1,texpage);

	++v;
	v->z = SCALEZ(p->z2);
	if (p->pad6 & PERSPECTIVE_CORRECT)
	{
		if (p->z2 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z2;
	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x2);
	v->y = SCALEIT(p->y2);
	setst(v,p->u2,p->v2,texpage);

	if (p->g0)
		SetColor(v,p->r0,p->g0,p->b0);
	numvert +=3;
}

void	    JJSDrawPolyGT3(POLY_GT3 * p)
{
	C3D_PVTCF       v;
	short temp;
	//p->b0

	temp=GetThePage(p->tpage,(p->u0+p->u1+p->u2)/3,(p->v0+p->v1+p->v2)/3);
	if(texpage!=temp)
	{
		sendList();
		texpage=temp;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &(gTex[texpage].hTX));
	}
	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,TRUE,p->b0,p->g0?C3D_ETL_MODULATE:C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();

	v = &testarray[numvert];
	v->z = SCALEZ(p->z0);
	if (p->pad4 & PERSPECTIVE_CORRECT)
	{
		if (p->z0 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z0;
	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x0);
	v->y = SCALEIT(p->y0);
	setst(v,p->u0,p->v0,texpage);

	++v;
	v->z = SCALEZ(p->z1);
	if (p->pad5 & PERSPECTIVE_CORRECT)
	{
		if (p->z1 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z1;

	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x1);
	v->y = SCALEIT(p->y1);
	setst(v,p->u1,p->v1,texpage);

	++v;
	v->z = SCALEZ(p->z2);
	if (p->pad6 & PERSPECTIVE_CORRECT)
	{
		if (p->z2 <= 0)
			v->w =1.0;
		else
			v->w = (float)screenZ / (float)p->z2;
			
	}
	else
		v->w = 1.0;
	v->x = SCALEIT(p->x2);
	v->y = SCALEIT(p->y2);
	setst(v,p->u2,p->v2,texpage);
	numvert += 3;
}

/*
 *  LoadTexture - load a texture map from a file into a texture surface
 */
BOOL LoadTextureBMP (const char* lpszTexFilename, PTEXTURE pTex)
{
 	HRESULT             ddretval;
	C3D_UINT32			log2X = 0L;
	C3D_UINT32			log2Y = 0L;
	C3D_TMAP			TMap;
	int					k, maxlog2;
	C3D_EC				ecRetVal;
	HANDLE				hTexFile;
	BITMAPFILEHEADER	bmfh;
	BITMAPINFOHEADER	bmih;
	C3D_UINT32			bufsize;
	void*				ptmap;
	void*				bmdata;
	C3D_UINT32			pitch;
	C3D_UINT8			r, g, b;
	C3D_UINT16			rgb565;
	DWORD				bytesread;
	int	x,y;

	// validate pTex
	if (!pTex)
	{
		wsprintf (gszErrMsg, "LoadTexture %s: invalid pointer", lpszTexFilename);
		return FALSE;
	}

	// open texture map file for reading
	hTexFile = CreateFile (
		lpszTexFilename,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (hTexFile == INVALID_HANDLE_VALUE)
	{
		wsprintf (gszErrMsg, "Could not open texture map file");
		return FALSE;
	}

	// read headers and bitmap data
	// first, determine buffer size required for BITMAPINFO and data
	bufsize = GetFileSize (hTexFile, NULL);
	if (bufsize == 0xFFFFFFFFL)
	{
		wsprintf (gszErrMsg, "Error getting texture file size");
		CloseHandle (hTexFile);
		return FALSE;
	}

	// adjust bufsize for BITMAFILEHEADER
	bufsize -= sizeof (bmfh);
	ptmap = (void*) HeapAlloc (GetProcessHeap (), HEAP_ZERO_MEMORY, sizeof (char) * bufsize);
	if (!ptmap)
	{
		wsprintf (gszErrMsg, "Could not allocate memory for texture map data");
		CloseHandle (hTexFile);
		return FALSE;
	}

	// read BITMAPFILEHEADER
	if (!ReadFile (hTexFile, &bmfh, sizeof (bmfh), &bytesread, NULL) || 
		(bytesread != sizeof (bmfh)))
	{
		HeapFree (GetProcessHeap (), 0, ptmap);
		CloseHandle (hTexFile);
		wsprintf (gszErrMsg, "Error reading file header");
		return FALSE;
	}

	// read BITMAPINFO and bitmap data
	if (!ReadFile (hTexFile, ptmap, bufsize, &bytesread, NULL) || 
		(bytesread != bufsize))
	{
		HeapFree (GetProcessHeap (), 0, ptmap);
		CloseHandle (hTexFile);
		wsprintf (gszErrMsg, "Error reading bitmap header and data");
		return FALSE;
	}

	// close the texture file; we're done with it
	CloseHandle (hTexFile);

	// read BITMAPINFOHEADER
	memcpy (&bmih, ptmap, sizeof (BITMAPINFOHEADER));

	// ensure width is power of 2 <= 1024
	while (log2X <= 11)
	{
		if ((pow (2.0, (double)log2X)) == (double)bmih.biWidth) break;
		log2X++;
	}
	if (log2X == 11)
	{
		wsprintf (gszErrMsg, "Width of texture %s greater than 1024 or not a power of 2", 
			lpszTexFilename);
		HeapFree (GetProcessHeap (), 0, ptmap);
		return FALSE;
	}

	// ensure height is power of 2 <= 1024
	while (log2Y <= 11)
	{
		if ((pow (2.0, (double)log2Y)) == (double)bmih.biHeight) break;
		log2Y++;
	}
	if (log2Y == 11)
	{
		wsprintf (gszErrMsg, "Height of texture %s greater than 1024 or not a power of 2", 
			lpszTexFilename);
		HeapFree (GetProcessHeap (), 0, ptmap);
		return FALSE;
	}

	// create an offscreen surface to cache the texture map
	ZeroMemory (&(pTex->ddsd), sizeof (pTex->ddsd));
	pTex->ddsd.dwSize = sizeof (pTex->ddsd);
	pTex->ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	pTex->ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
	pTex->ddsd.dwWidth = bmih.biWidth;
	pTex->ddsd.dwHeight = bmih.biHeight;
	ddretval = lpDD->lpVtbl->CreateSurface (lpDD,&(pTex->ddsd), &pTex->lpDDSTex, NULL);
	if (ddretval != DD_OK)
	{
		wsprintf (gszErrMsg, "Could not create texture surface for file %s", lpszTexFilename);
		HeapFree (GetProcessHeap (), 0, ptmap);
		return FALSE;
	}

	// get a pointer to the texture surface
	ZeroMemory (&(pTex->ddsd), sizeof (pTex->ddsd));
	pTex->ddsd.dwSize = sizeof (pTex->ddsd);
	// lock texture to fill ddsd member
	ddretval = pTex->lpDDSTex->lpVtbl->Lock (pTex->lpDDSTex,
		NULL,
		&(pTex->ddsd),
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
		NULL);
	if (ddretval == DDERR_SURFACELOST) pTex->lpDDSTex->lpVtbl->Restore (pTex->lpDDSTex);
	// unlock surface
	ddretval = pTex->lpDDSTex->lpVtbl->Unlock (pTex->lpDDSTex,NULL);
	if (ddretval != DD_OK)
	{
		wsprintf (gszErrMsg, "Error unlocking texture surface for file %s", lpszTexFilename);
		HeapFree (GetProcessHeap (), 0, ptmap);
		pTex->lpDDSTex->lpVtbl->Release (pTex->lpDDSTex);
		pTex->lpDDSTex = NULL;
		return FALSE;
	}

	// pitch of each scan line
	if(bmih.biSizeImage)
	{
		pitch = bmih.biSizeImage / bmih.biHeight;
		// set pointer to start of bm data
		bmdata = (char*)ptmap + (bufsize - bmih.biSizeImage);
	}
	else
	{
		pitch = bmih.biWidth * 3;		// JJS for BMPs without biSizeImage
 		// set pointer to start of bm data
		bmdata = (char*)ptmap + (bufsize - pitch * bmih.biHeight);
	}

	// line counter
	for (y=((int)bmih.biHeight-1); y>=0; y--)
	{
		// scan line pixel counter
		for (x=0; x<(int)bmih.biWidth; x++)
		{
			// read RGB
			memcpy (&b, (char*)bmdata + (y*pitch)+(3*x), sizeof (C3D_UINT8));
			memcpy (&g, (char*)bmdata + (y*pitch)+(3*x)+1, sizeof (C3D_UINT8));
			memcpy (&r, (char*)bmdata + (y*pitch)+(3*x)+2, sizeof (C3D_UINT8));

			// pack color data in 16bpp format

//#define RGB_555
#ifndef RGB_555
			// pack data in RGB 555 format
			rgb565 = ((C3D_UINT32)r & 0xf8) << 8 |
					 ((C3D_UINT32)g & 0xfc) << 3 |
					 ((C3D_UINT32)b & 0xf8) >> 3;
#else
			// pack data in RGB 565 format
			rgb565 = ((C3D_UINT32)r & 0xf8) << 7 |
					 ((C3D_UINT32)g & 0xf8) << 2 |
					 ((C3D_UINT32)b & 0xf8) >> 3;
#endif
			// write texture data to texture cache
			memcpy ((char*)pTex->ddsd.lpSurface + ((bmih.biHeight-y-1)*(bmih.biWidth*2))+(2*x), &rgb565, sizeof (C3D_UINT16));
		}
	}

	// free the texture file buffer
		HeapFree (GetProcessHeap (), 0, ptmap);

	// fill a C3D_TMAP struct
	ZeroMemory (&TMap, sizeof (TMap));
	TMap.u32Size = sizeof (TMap);

	// determine the maximum log2 dimension
	maxlog2 = (int)(log2X >= log2Y? log2X : log2Y);
	for (k=0; k < maxlog2; k++)
		TMap.apvLevels[k] = pTex->ddsd.lpSurface;

	TMap.bMipMap = FALSE;
	TMap.u32MaxMapXSizeLg2 = log2X;
	TMap.u32MaxMapYSizeLg2 = log2Y;
	TMap.eTexFormat = C3D_ETF_RGB565;
//JJS	TMap.bTexTranspEnable = FALSE;
	SET_CIF_COLOR (TMap.clrTexChromaKey, 0, 0, 0, 0);

	// register the texture
	ecRetVal = ATI3DCIF_TextureReg (&TMap, &(pTex->hTX));
	if (ecRetVal != C3D_EC_OK)
	{
		wsprintf (gszErrMsg, "Error registering texture for file %s", lpszTexFilename);
		pTex->lpDDSTex->lpVtbl->Release (pTex->lpDDSTex);
		pTex->lpDDSTex = NULL;
		return FALSE;
	}

	return TRUE;
}


/*
 *  UnloadTexture - unregister texture and release its surface
 */
BOOL UnloadTexture (PTEXTURE pTex)
{
	if (!pTex)
	{
		wsprintf (gszErrMsg, "UnloadTexture: invalid pointer");
		return FALSE;
	}

	// unregister the texture if a non-NULL handle
	if (pTex->hTX)
	{
 		ATI3DCIF_TextureUnreg (pTex->hTX);
	}

	// unlock and release the texture surface
	if (pTex->lpDDSTex)
	{
		pTex->lpDDSTex->lpVtbl->Release (pTex->lpDDSTex);
		pTex->lpDDSTex = NULL;
	}

	return TRUE;;
}

char *LockPilot(void)
{
	HRESULT             ddretval;

	// lock texture to fill ddsd member
	ddretval = DDERR_SURFACELOST;
	while (ddretval == DDERR_SURFACELOST)
	{
		ddretval = gTex[420].lpDDSTex->lpVtbl->Lock (gTex[420].lpDDSTex,
			NULL,
			&(gTex[420].ddsd),
			DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
			NULL);
		if (ddretval == DDERR_SURFACELOST)
			gTex[420].lpDDSTex->lpVtbl->Restore (gTex[420].lpDDSTex);
	}
	return((char *)gTex[420].ddsd.lpSurface);
}

void UnlockPilot(void)
{
	HRESULT             ddretval;

	ddretval = gTex[420].lpDDSTex->lpVtbl->Unlock (gTex[420].lpDDSTex,NULL);
}

void ChopUp(char *to, void *from , int which)
{
	char *p1=from;
	int i,x,y;

	which &= 63;
	x=which&7;
	y=which>>3;
	p1+=y*(512*32);
	p1+=x*64;
	for(i=0;i<32;++i)
	{
		memcpy(to,p1,64);
		p1+=512;
		to+=64;
	}
			
}

BIT15 stuffem256(BIT15 *p);
#pragma aux stuffem256 	parm [esi] modify [ax bx dx];
	
void ChopUpDown(void *to, void *from , int which)
{
	BIT15 *p1,*p2;
	int i,j,x,y;
	register BIT15 fred={0,0,0};

	p1=from;
	p2=to;

	which &= 63;
	x=which&7;
	y=which>>3;
	p1+=y*(256*32);
	p1+=x*32;

	for(i=0;i<32/2;++i)
	{
		for(j=0;j<32/2;++j,p2++,p1+=2)
		{
			fred=stuffem256(p1);
//			fred.red=(p1->red+(p1+1)->red+(p1+256)->red+(p1+256+1)->red)/4;
//			fred.green=(p1->green+(p1+1)->green+(p1+256)->green+(p1+256+1)->green)/4;
//			fred.blue=(p1->blue+(p1+1)->blue+(p1+256)->blue+(p1+256+1)->blue)/4;
			*p2=fred;
		}
		p1+=256+256-32;
	}
}

void DownScale(void *to, void *from , int xsize, int ysize)
{
	BIT15 *p1,*p2;
	int i,j;
	register BIT15 fred={0,0,0};

	p1=from;
	p2=to;
	for(i=0;i<ysize/2;++i)
	{
		for(j=0;j<xsize/2;++j,p2++,p1+=2)
		{
			fred=stuffem256(p1);
//			fred.red=(p1->red+(p1+1)->red+(p1+xsize)->red+(p1+xsize+1)->red)/4;
//			fred.green=(p1->green+(p1+1)->green+(p1+xsize)->green+(p1+xsize+1)->green)/4;
//			fred.blue=(p1->blue+(p1+1)->blue+(p1+xsize)->blue+(p1+xsize+1)->blue)/4;
			*p2=fred;
		}
		p1+=xsize;
	}
}

void SetupbackBuffer(void)
{
	if(!lpBack_Buffer)
	{
		DDSURFACEDESC       ddsd;
		HRESULT ddrval;

		memset(&ddsd, 0, sizeof(ddsd));
		ddsd.dwSize = sizeof(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = WinWidth;
#if USE_ATI
		ddsd.dwHeight = WinHeight;
#else
		ddsd.dwHeight = 200;
#endif
		ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &lpBack_Buffer, NULL);
		if (ddrval != DD_OK)
		{
			ShowErrorMessage(4002);
		}
	}
}
/*
 *  LoadTexture - load a texture map from a file into a texture surface
 */

BOOL SJRLoadTextureBMPmenu (const char* lpszTexFilename)
{
	int fp;
	HRESULT             ddretval;
	C3D_TMAP			TMap;
	int					i, k, maxlog2;
	C3D_EC				ecRetVal;
	int	whichSurface,reals;
	long 				texsize;
	char 			*ptr, *p1;
	short	track,other;

	if(trackSize)
		track=256;
	else
		track=128;
	if(otherSize)
		other=256;
	else
		other=128;
	SetupbackBuffer();
	for(i=0;i<420;++i)
		if (gTex[i].lpDDSTex != NULL)
		{
			ATI3DCIF_TextureUnreg(gTex[i].hTX);
			gTex[i].lpDDSTex->lpVtbl->Release(gTex[i].lpDDSTex);
			gTex[i].lpDDSTex = NULL;
		}
	// create 5 offscreen surfaces to cache the high res track
	for(i=0;i<5;++i)
	{
		ZeroMemory (&(gTex[i].ddsd), sizeof (gTex[i].ddsd));
		gTex[i].ddsd.dwSize = sizeof (gTex[i].ddsd);
		gTex[i].ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		gTex[i].ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		gTex[i].ddsd.dwWidth = track;//SJR	// standard texture size
		gTex[i].ddsd.dwHeight = track;//SJR	// standard height
		ddretval = lpDD->lpVtbl->CreateSurface (lpDD,&(gTex[i].ddsd), &gTex[i].lpDDSTex, NULL);
		if (ddretval != DD_OK)
		{
			ShowErrorMessage(4002);
		}
	}
	// create 16 offscreen surfaces to cache the texture maps
	for(i=405;i<420;++i)
	{
		ZeroMemory (&(gTex[i].ddsd), sizeof (gTex[i].ddsd));
		gTex[i].ddsd.dwSize = sizeof (gTex[i].ddsd);
		gTex[i].ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		gTex[i].ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		gTex[i].ddsd.dwWidth = other;//SJR	// standard texture size
		gTex[i].ddsd.dwHeight = other;//SJR	// standard height
		ddretval = lpDD->lpVtbl->CreateSurface (lpDD,&(gTex[i].ddsd), &gTex[i].lpDDSTex, NULL);
		if (ddretval != DD_OK)
		{
			ShowErrorMessage(4002);
		}
	}
	texsize = 32768;//128*128 * sizeof (C3D_UINT16)

	if(strcmp(lastfile,lpszTexFilename))
	{
		fp = open(lpszTexFilename, O_RDONLY | O_BINARY);
		if (fp<0)
		{
			ShowErrorMessage(2002);
			return FALSE;
		}
	
		read(fp, (void *)TextureBuffer, TEXTURESJR);
		close(fp);
		strcpy(lastfile,lpszTexFilename);
	}

	ptr = TextureBuffer;

	for (whichSurface=0; whichSurface< 20; whichSurface++)
	{
		if(whichSurface<5)
			reals=whichSurface;
		else
			reals=whichSurface+400;
		// get a pointer to the texture surface
		ZeroMemory (&(gTex[reals].ddsd), sizeof (gTex[reals].ddsd));
		gTex[reals].ddsd.dwSize = sizeof (gTex[reals].ddsd);

		// lock texture to fill ddsd member
		ddretval = DDERR_SURFACELOST;
		while (ddretval == DDERR_SURFACELOST)
		{
			ddretval = gTex[reals].lpDDSTex->lpVtbl->Lock (gTex[reals].lpDDSTex,
				NULL,
				&(gTex[reals].ddsd),
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
				NULL);
			if (ddretval == DDERR_SURFACELOST)
				gTex[reals].lpDDSTex->lpVtbl->Restore (gTex[reals].lpDDSTex);
		}
		p1 = (char *)gTex[reals].ddsd.lpSurface;

		if(whichSurface<5)
		{
			if(trackSize)
				memcpy(p1, ptr, texsize*4);
			else
				DownScale(p1,ptr,256,256);
		}
		else
		{
			if(otherSize)
				memcpy(p1, ptr, texsize*4);
			else
				DownScale(p1,ptr,256,256);
		}
		
		ptr += texsize*4;

		ddretval = gTex[reals].lpDDSTex->lpVtbl->Unlock (gTex[reals].lpDDSTex,NULL);
		if (ddretval != DD_OK)
		{
			ShowErrorMessage(1000);
		}

		// fill a C3D_TMAP struct
		ZeroMemory (&TMap, sizeof (TMap));
		TMap.u32Size = sizeof (TMap);

		// determine the maximum log2 dimension
		if(whichSurface<5)
			maxlog2 = trackSize?8:7;
		else
			maxlog2 = otherSize?8:7;
		for (k=0; k < maxlog2; k++)
		{
			TMap.apvLevels[k] = gTex[reals].ddsd.lpSurface;
		}

		TMap.bMipMap = FALSE;
		TMap.u32MaxMapXSizeLg2 = maxlog2;
		TMap.u32MaxMapYSizeLg2 = maxlog2;
		TMap.eTexFormat = C3D_ETF_RGB1555;

		SET_CIF_COLOR (TMap.clrTexChromaKey, 0, 0, 0, 0);

		// register the texture
		ecRetVal = ATI3DCIF_TextureReg (&TMap, &(gTex[reals].hTX));
		if (ecRetVal != C3D_EC_OK)
		{
			ShowErrorMessage(1000);
		}
	}
		
	return TRUE;
}

/*
 *  LoadTexture - load a texture map from a file into a texture surface
 */

BOOL SJRLoadTextureBMP (const char* lpszTexFilename)
{
	int fp;
    HRESULT             ddretval;
	C3D_TMAP			TMap;
	int					i, k, maxlog2;
	C3D_EC				ecRetVal;
	int	whichSurface,reals;
	long 				texsize;
	char 			*ptr, *p1;
	short	track,other;

	if(trackSize)
		track=32;
	else
		track=16;
	if(otherSize)
		other=256;
	else
		other=128;

	
	if (lpBack_Buffer != NULL)
	{
		lpBack_Buffer->lpVtbl->Release(lpBack_Buffer);
		lpBack_Buffer = NULL;
	}
	for(i=0;i<420;++i)
		if (gTex[i].lpDDSTex != NULL)
		{
			ATI3DCIF_TextureUnreg(gTex[i].hTX);
			gTex[i].lpDDSTex->lpVtbl->Release(gTex[i].lpDDSTex);
			gTex[i].lpDDSTex = NULL;
		}
	for(i=0;i<320;++i)
	{
		ZeroMemory (&(gTex[i].ddsd), sizeof (gTex[i].ddsd));
		gTex[i].ddsd.dwSize = sizeof (gTex[i].ddsd);
		gTex[i].ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		gTex[i].ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		gTex[i].ddsd.dwWidth = track;//SJR	// standard texture size
		gTex[i].ddsd.dwHeight = track;//SJR	// standard height
		ddretval = lpDD->lpVtbl->CreateSurface (lpDD,&(gTex[i].ddsd), &gTex[i].lpDDSTex, NULL);
		if (ddretval != DD_OK)
		{
			ShowErrorMessage(4002);
			return FALSE;
		}
	}
	// create 16 offscreen surfaces to cache the texture maps
	for(i=405;i<420;++i)
	{
		ZeroMemory (&(gTex[i].ddsd), sizeof (gTex[i].ddsd));
		gTex[i].ddsd.dwSize = sizeof (gTex[i].ddsd);
		gTex[i].ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
		gTex[i].ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		gTex[i].ddsd.dwWidth = other;//SJR	// standard texture size
		gTex[i].ddsd.dwHeight = other;//SJR	// standard height
		ddretval = lpDD->lpVtbl->CreateSurface (lpDD,&(gTex[i].ddsd), &gTex[i].lpDDSTex, NULL);
		if (ddretval != DD_OK)
		{
			ShowErrorMessage(4002);
		}
	}
	texsize = 32768;//128*128 * sizeof (C3D_UINT16)

	if(strcmp(lastfile,lpszTexFilename))
	{
		fp = open(lpszTexFilename, O_RDONLY | O_BINARY);
		if (fp<0)
		{
			ShowErrorMessage(2002);
			return FALSE;
		}
	
		read(fp, (void *)TextureBuffer, TEXTURESJR);
		close(fp);
		strcpy(lastfile,lpszTexFilename);
	}

	ptr = TextureBuffer;

	for (whichSurface=0; whichSurface< 420; whichSurface++)
	{
		reals=whichSurface;
		if(reals>=320 && reals <405)
			continue;
		// get a pointer to the texture surface
		ZeroMemory (&(gTex[reals].ddsd), sizeof (gTex[reals].ddsd));
		gTex[reals].ddsd.dwSize = sizeof (gTex[reals].ddsd);

		// lock texture to fill ddsd member
		ddretval = DDERR_SURFACELOST;
		while (ddretval == DDERR_SURFACELOST)
		{
			ddretval = gTex[reals].lpDDSTex->lpVtbl->Lock (gTex[reals].lpDDSTex,
				NULL,
				&(gTex[reals].ddsd),
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
				NULL);
			if (ddretval == DDERR_SURFACELOST)
				gTex[reals].lpDDSTex->lpVtbl->Restore (gTex[reals].lpDDSTex);
		}
		p1 = (char *)gTex[reals].ddsd.lpSurface;

		ddretval = gTex[reals].lpDDSTex->lpVtbl->Unlock (gTex[reals].lpDDSTex,NULL);
		if (ddretval != DD_OK)
		{
			ShowErrorMessage(1000);
		}

		if(reals<320)
		{
			if(trackSize)
				ChopUp(p1, ptr, reals);
			else
				ChopUpDown(p1, ptr, reals);
		}
		else
		{
			if(otherSize)
				memcpy(p1, ptr, texsize*4);
			else
				DownScale(p1,ptr,256,256);
		}

		if(reals>320 || ((reals&63)==63))
			ptr += texsize*4;

		// fill a C3D_TMAP struct
		ZeroMemory (&TMap, sizeof (TMap));
		TMap.u32Size = sizeof (TMap);

		// determine the maximum log2 dimension
		if(whichSurface<320)
			maxlog2 = trackSize?5:4;
		else
			maxlog2 = otherSize?8:7;
		for (k=0; k < maxlog2; k++)
		{
			TMap.apvLevels[k] = gTex[reals].ddsd.lpSurface;
		}

		TMap.bMipMap = FALSE;
		TMap.u32MaxMapXSizeLg2 = maxlog2;
		TMap.u32MaxMapYSizeLg2 = maxlog2;
		TMap.eTexFormat = C3D_ETF_RGB1555;

		SET_CIF_COLOR (TMap.clrTexChromaKey, 0, 0, 0, 0);

		// register the texture
		ecRetVal = ATI3DCIF_TextureReg (&TMap, &(gTex[reals].hTX));
		if (ecRetVal != C3D_EC_OK)
		{
			ShowErrorMessage (2002);
			return FALSE;
		}
	}
		
	return TRUE;
}

/*
 *  LoadTexture - load a texture map from a file into a texture surface
 */

BOOL JJSLoadSprites (void)
{
	int fp;
    HRESULT             ddretval;
	C3D_TMAP			TMap;
	int					k, maxlog2;
	C3D_EC				ecRetVal;
	//HANDLE				hTexFile;
	C3D_UINT32			bufsize;
	int	sp;
	long 				texsize;
	char			*ptr, *bmpdata, *p1;
	static char* lpszTexFilename[3]={
		"wipeout\\newgraph\\menu\\page12.sjr",
		"wipeout\\newgraph\\menu\\page15.sjr",
		"wipeout\\newgraph\\menu\\page17.sjr"
	};

	texsize = 256 * 256 * sizeof (C3D_UINT16);

	bufsize = texsize;
	
	bmpdata = (char *)malloc(bufsize);
	if( bmpdata == NULL )
	{
		return FALSE;
	}

	for(sp=0;sp<3;++sp)
	{
		fp = open(lpszTexFilename[sp], O_RDONLY | O_BINARY);
		if (fp<0)
		{
			ShowErrorMessage(2002);
			wsprintf (gszErrMsg, "Could not open texture map file");
			return FALSE;
		}
	
		read(fp, (void *)bmpdata, bufsize);
		close(fp);
	
		ptr = bmpdata;
	
		// get a pointer to the texture surface
		ZeroMemory (&(gTex[sp+420].ddsd), sizeof (gTex[sp+420].ddsd));
		gTex[sp+420].ddsd.dwSize = sizeof (gTex[sp+420].ddsd);
	
		// lock texture to fill ddsd member
		do
		{
			ddretval = gTex[sp+420].lpDDSTex->lpVtbl->Lock (gTex[sp+420].lpDDSTex,
				NULL,
				&(gTex[sp+420].ddsd),
				DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
				NULL);
			if (ddretval == DDERR_SURFACELOST)
				gTex[sp+420].lpDDSTex->lpVtbl->Restore (gTex[sp+420].lpDDSTex);
		}while(ddretval == DDERR_SURFACELOST);
		p1 = (char *)gTex[sp+420].ddsd.lpSurface;
	
		memcpy(p1, bmpdata, texsize);

		ddretval = gTex[sp+420].lpDDSTex->lpVtbl->Unlock (gTex[sp+420].lpDDSTex,NULL);
		if (ddretval != DD_OK)
		{
			wsprintf (gszErrMsg, "Error unlocking texture surface for file %s", lpszTexFilename);
			gTex[sp+420].lpDDSTex->lpVtbl->Release (gTex[sp+420].lpDDSTex);
			return FALSE;
		}

		// fill a C3D_TMAP struct

		ZeroMemory (&TMap, sizeof (TMap));
		TMap.u32Size = sizeof (TMap);

		// determine the maximum log2 dimension
		maxlog2 = 8;
		for (k=0; k < maxlog2; k++)
		{
			TMap.apvLevels[k] = gTex[sp+420].ddsd.lpSurface;
		}

		TMap.bMipMap = FALSE;
		TMap.u32MaxMapXSizeLg2 = 8;
		TMap.u32MaxMapYSizeLg2 = 8;
		TMap.eTexFormat = C3D_ETF_RGB1555;

		SET_CIF_COLOR (TMap.clrTexChromaKey, 0, 0, 0, 0);

		// register the texture
		ecRetVal = ATI3DCIF_TextureReg (&TMap, &(gTex[sp+420].hTX));
		if (ecRetVal != C3D_EC_OK)
		{
			wsprintf (gszErrMsg, "Error registering texture for file %s", lpszTexFilename[sp]);
			gTex[sp+420].lpDDSTex->lpVtbl->Release (gTex[sp+420].lpDDSTex);
			gTex[sp+420].lpDDSTex = NULL;
			return FALSE;
		}
	}
		
	free(ptr);
	winLoseIn = 0;
	return TRUE;
}

BOOL JJSLoadRage (void)
{
	int fp;
	DDSURFACEDESC       ddsd;
	C3D_UINT32			bufsize;
	long 				texsize;
	char			*bmpdata;
	int	sp;
	static char* lpszTexFilename[2]={
		"wipeout\\newgraph\\legal.sjr",
		"wipeout\\newgraph\\rage.sjr"
	};

	HRESULT ddrval;
	LPDIRECTDRAWSURFACE tbuf;
	RECT	    dest_rect, src_rect;
	DWORD	   dwFlags;
	DDBLTFX	 DDBltFX;

	dest_rect.top = dest_rect.left = 0;
	dest_rect.bottom = WinHeight;
	dest_rect.right = WinWidth;

	src_rect.top = src_rect.left = 0;
	src_rect.bottom = 384;
	src_rect.right = 512;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = 512;
	ddsd.dwHeight = 384;
	ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &tbuf, NULL);
	if (ddrval != DD_OK)
	{
		ShowErrorMessage(4002);
		return (FALSE);
	}

	texsize = 512 * 384 * sizeof (C3D_UINT16);

	bufsize = texsize;
	

	for(sp=0;sp<2;++sp)
	{
		ZeroMemory (&ddsd, sizeof (ddsd));
		ddsd.dwSize = sizeof (ddsd);
		tbuf->lpVtbl->Lock(tbuf, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
		bmpdata = ddsd.lpSurface;
		fp = open(lpszTexFilename[sp], O_RDONLY | O_BINARY);
		if (fp<0)
		{
			ShowErrorMessage(2002);
			return FALSE;
		}
	
		read(fp, (void *)bmpdata, bufsize);
		close(fp);
		tbuf->lpVtbl->Unlock(tbuf,NULL);

		DDBltFX.dwSize = sizeof(DDBltFX);

		dwFlags = DDBLT_WAIT;
		lpDDSBack->lpVtbl->Blt(lpDDSBack, &dest_rect, tbuf, &src_rect, dwFlags, &DDBltFX);

		DDSwap();
		WinClearScreen(0);
		sleep(3);
	}
	tbuf->lpVtbl->Release(tbuf);

	return TRUE;
}

BOOL SJRNewTexture(char *filename)
{
	JJSLoadSprites();
	if(menuTexturesIn)
		return(SJRLoadTextureBMPmenu (filename));
	else
		return(SJRLoadTextureBMP (filename));
}


HINSTANCE hInstance;
HWND hwndMCIWnd;

LRESULT FAR PASCAL VideoWndProc(HWND hWnd, UINT iMsg, WPARAM wParam, 
LPARAM lParam)
{
	switch(iMsg) {
		case MM_MCINOTIFY:
			switch(wParam) {				
				case MCI_NOTIFY_SUCCESSFUL:
				case MCI_NOTIFY_ABORTED:
				case MCI_NOTIFY_SUPERSEDED:
				case MCI_NOTIFY_FAILURE:
					MCIWndClose(hwndMCIWnd);
					MCIWndDestroy(hwndMCIWnd);
					DestroyWindow(hWnd);
					break;
			}
			return 1;
		case WM_DESTROY:
			PostQuitMessage(0);
			return 1;
/*		case MCIWNDM_NOTIFYMODE:
			if(lParam == MCI_MODE_STOP) {
				MCIWndOpen(hwndMCIWnd, "n:\\mmdata\\mpeg\\test.mpg", 0);
				MCIWndPlay(hwndMCIWnd);
			}
			return 1;*/
		default:
			return (DefWindowProc(hWnd, iMsg, wParam, lParam));
	}
}




/*void ShellMoviePlay(char *pName, int width, int height, int fullScreen)
{
  char szMoviePathName[MAX_PATH];
  struct sMovie movie;
  DWORD dwAttributes;
  DWORD result;

  // Build full path name to movie file 
  strcpy(szMoviePathName, pName);

  // Convert it to short form for MCI layer 
  result = GetShortPathName(
    szMoviePathName,                // points to a null-terminated path string
    movie.szMoviePathName,          // points to a buffer to receive the null-terminated short form of the path 
    sizeof(movie.szMoviePathName)   // specifies the size of the buffer pointed to by lpszShortPath
  );

   Can only play movie if the file exists
  dwAttributes = GetFileAttributes(movie.szMoviePathName);
  if (dwAttributes == 0xffffffff)
  {
    // Movie file is not there 
    return;   
  }

  movie.width = width;
  movie.height = height;
  movie.fullScreen = fullScreen;

  shellMovieFinishedG = FALSE;

  // Ask shell thread to play movie 
  while (!PostMessage(hwnd, MESSAGE_MOVIE_PLAY, (WPARAM)0, (LPARAM)&movie) )
  {
    Sleep(50);
  }

  // Wait for movie to finish
  while (!shellMovieFinishedG)
  {
    Sleep(50);
    ShellSync(0);
  }

}*/



 /****************************************************************************
  *
  * Start a movie playing
  *    
  ****************************************************************************/

int ShellMovieStart(char *moviename)
{
  DWORD dwMCIWndStyle;
  MCI_PLAY_PARMS playInfo;
  DWORD dwErr;
  int result;
  int width, height;
  int cx, cy;

  cx = GetSystemMetrics(SM_CXSCREEN);
  cy = GetSystemMetrics(SM_CYSCREEN);
  /* Default to fail */
  result = -1;

  /* Set main app window to full screen                  */
  /* DEEP WEIRD MAGIC:                                   */
  /* Setting it to half size then full size seems to get */
  /* rid of the system busy cursor that appeared when    */
  /* DirectDraw was shut down                            */
  //SetWindowPos(hwnd, HWND_TOP, 0, 0, cx/2, cy/2, SWP_NOZORDER);
  SetWindowPos(hwnd, HWND_TOP, 0, 0, cx, cy, 0);
  UpdateWindow(hwnd);

  /* Create MCI window to play movie */
  dwMCIWndStyle = WS_CHILD | WS_VISIBLE | MCIWNDF_NOPLAYBAR | /*MCIWNDF_NOTIFYMODE | */
    CS_BYTEALIGNCLIENT | MCIWNDF_NOMENU | MCIWNDF_NOERRORDLG;

  hwndMCIWnd = MCIWndCreate(hwnd, hinst, dwMCIWndStyle, moviename);
  if (hwndMCIWnd)
  {
    /* Get the ID of the device that will play the movie */
    shellMovieDeviceIDG = MCIWndGetDeviceID(hwndMCIWnd);

    /* Size the movie window */
      width = cx;
      height = cy;

    SetWindowPos(hwndMCIWnd, HWND_TOP, (cx - width) / 2, (cy - height) / 2, width, height, NULL);

    /* Start movie play back */
    playInfo.dwCallback = MAKELONG(hwnd, 0);
    dwErr = mciSendCommand(shellMovieDeviceIDG, MCI_PLAY, MCI_NOTIFY, (DWORD)&playInfo);
    if (dwErr == 0)
    {
      /* Movie is playing */
      shellMoviePlayingG = TRUE;

      /* Ok */
      result = 0;
    }
    else
    {
      /* Movie play failed */
      ShellMovieFinished();
    }
  }

  return(result);
} 



 /****************************************************************************
  *
  * Stop movie
  *    
  ****************************************************************************/

void ShellMovieStop(void)
{
  MCIERROR mciretval;
  MCI_GENERIC_PARMS mciGenericParms;

  /* If the movie window exists */
  if (hwndMCIWnd)
  {
    /* Stop the MPEG player */
    ZeroMemory(&mciGenericParms, sizeof(mciGenericParms));
    mciretval = mciSendCommand(shellMovieDeviceIDG, MCI_STOP, MCI_WAIT, (DWORD)(LPVOID) &mciGenericParms);
  }
} 
 
 /****************************************************************************
  *
  * Cleanup after movies has finished playing
  *    
  ****************************************************************************/

void ShellMovieFinished(void)
{
  /* Close any MCI window playing the movie */
  if (hwndMCIWnd)
  {
    MCIWndClose(hwndMCIWnd);
    MCIWndDestroy(hwndMCIWnd);

    hwndMCIWnd = NULL;
  }

  /* Flag movie finished */
  shellMoviePlayingG = FALSE;
  shellMovieFinishedG = TRUE;
} 
 
 /****************************************************************************/


void mpeganim(void)
{
	int fp;
	char *mpegname;
	int namelen;
	char endchar;

	namelen = strlen(MovieDirBase) + 15;
	GetShortPathName(MovieDirBase,MovieDirBase,200);

	mpegname = (char *)malloc(namelen);

	endchar = *(MovieDirBase + strlen(MovieDirBase) - 1);
	if (endchar != '\\')
		sprintf(mpegname,"%s\\wipeout.mpg", MovieDirBase);
	else
		sprintf(mpegname,"%swipeout.mpg", MovieDirBase);

	//make sure the movie's there
	if ((fp = open(mpegname, O_RDONLY)) == -1)
		return;

	close(fp);

	ShellMovieStart(mpegname);

	while (shellMoviePlayingG)
	{
		ProcessMessages();
	}

 	free(mpegname);

}
#endif

void centreMouse(void)
{
	SetCursorPos(128,100);
}

void JJSReadJoystick(void)
{
	JOYINFO myJoy;
	MMRESULT errVal;

	errVal = joyGetPos(JOYSTICKID1,&myJoy);

	JoyXVal = myJoy.wXpos >> 8;
	JoyYVal = myJoy.wYpos >> 8;

	JoyButton1 = myJoy.wButtons & 0x0001;
	JoyButton2 = myJoy.wButtons & 0x0002;
}

BOOL JJSJoyConfig(void)
{
	JOYCAPS mycaps;
	UINT joyrange;
	MMRESULT theres;

	if(joyGetDevCaps(JOYSTICKID1,&mycaps,sizeof(mycaps)) != JOYERR_NOERROR)
		return(FALSE);

	joyrange = mycaps.wXmax - mycaps.wXmin;
	joyModXVal = 256.0/(float)joyrange;

	joySetThreshold(JOYSTICKID1, joyrange/512);

	joyrange = mycaps.wYmax - mycaps.wYmin;
	joyModYVal = 256.0/(float)joyrange;

	theres = joySetCapture(hwnd, JOYSTICKID1, 0, TRUE);


	if (theres != JOYERR_NOERROR)
		switch(theres)
		{
			case MMSYSERR_NODRIVER:
				theres = 2;
				break;
			case JOYERR_NOCANDO:
				theres = 2;
				break;
			case JOYERR_UNPLUGGED:
				theres = 2;
				return FALSE;
				break;
		}
		return TRUE;

}


void ShowErrorMessage(UINT StrNum)
{
	MSG msg;
	char Msg[80];

	ShowCursor(TRUE);
	finiObjects();

	LoadString(hinst, StrNum, Msg, 80);
	SystemParametersInfo(SPI_SETKEYBOARDDELAY,Keydelay,0,0);
	SystemParametersInfo(SPI_SETKEYBOARDSPEED,Keyrepeat,0,0);
 	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,sSaver,0,0);
	while (PeekMessage(&msg, hwnd, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE));      // Clear all outstanding messages
	MessageBox(hwnd, Msg, "Wipeout Error", MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND);
	DestroyWindow(hwnd);
	QuitToDos();
}

void ShowWarningMessage(UINT StrNum)
{
	MSG	     msg;
	char Msg[80];

	ShowCursor(TRUE);
	LoadString(hinst, StrNum, Msg, 80);
	while (PeekMessage(&msg, hwnd, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE));      // Clear all outstanding messages
	MessageBox(hwnd, Msg, "Wipeout Warning",
		MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND);
	ShowCursor(FALSE);
}

TimData *JJSLoad16BitTexture(char *filename, char location)
{
	TimData	*timSize;
	DDSURFACEDESC       ddsd;
	C3D_UINT32			bufsize;
	long 				texsize;
	char			*bmpdata;
	HRESULT ddrval;
	LPDIRECTDRAWSURFACE tbuf;
	RECT	    dest_rect, src_rect;
	DWORD	   dwFlags;
	DDBLTFX	 DDBltFX;

	SetupbackBuffer();
	dest_rect.top = dest_rect.left = 0;
	dest_rect.bottom = WinHeight;
	dest_rect.right = WinWidth;

	src_rect.top = src_rect.left = 0;
	src_rect.bottom = 240;
	src_rect.right = 320;

	memset(&ddsd, 0, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth = 320;
	ddsd.dwHeight = 240;
	ddrval = lpDD->lpVtbl->CreateSurface(lpDD, &ddsd, &tbuf, NULL);
	if (ddrval != DD_OK)
	{
		ShowErrorMessage(4002);
		return (FALSE);
	}

	texsize = 320 * 240 * sizeof (C3D_UINT16);

	bufsize = texsize;
	
	timSize = ( TimData* )DAlloc( heap, sizeof(TimData) );

	ZeroMemory (&ddsd, sizeof (ddsd));
	ddsd.dwSize = sizeof (ddsd);
	tbuf->lpVtbl->Lock(tbuf, NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
	bmpdata = ddsd.lpSurface;
	JJSReadTIM(filename,bmpdata,timSize);
	tbuf->lpVtbl->Unlock(tbuf,NULL);

	DDBltFX.dwSize = sizeof(DDBltFX);

	dwFlags = DDBLT_WAIT;
	lpBack_Buffer->lpVtbl->Blt(lpBack_Buffer, &dest_rect, tbuf, &src_rect, dwFlags, &DDBltFX);

	tbuf->lpVtbl->Release(tbuf);

	return (timSize) ;
}


/******
*
* Local Functions
*
*******/

static void ResetATIConfig ( void );


/******
*
* Global Data
*
*******/

static ATICONFIG ATIConfig;
static SharingEnabled  = FALSE;
static TimewarpEnabled = FALSE;

WORD CWDDE_InitOK = FALSE; /* must be visible to cwddea.asm */


/*--------------------------- Public Functions -----------------------------*/


WORD CWDDE_Init ( void )
  {
    HDC hDC;
    signed short int  ConfigBufSize = sizeof ( ATIConfig );

    hDC = CreateDC ( "DISPLAY", NULL, NULL, NULL );
    if ( hDC == NULL )
        return ( CWDDE_InitOK );

    if ( ExtEscape ( hDC, Control_ConfigIsSupported, 0, NULL, NULL, NULL) == -1 )
      {
        DeleteDC ( hDC );
        return ( CWDDE_InitOK );
      }        

    ResetATIConfig ();

    if ( ExtEscape ( hDC,
                  Control_GetConfiguration,
                  sizeof(ConfigBufSize),
                  (LPCSTR)&ConfigBufSize,
				  sizeof(ATIConfig),
                  (LPSTR)&ATIConfig ) == -1 )
      {
        DeleteDC ( hDC );
        return ( CWDDE_InitOK );
      }

    DeleteDC ( hDC );
    CWDDE_InitOK = TRUE;

    return ( TRUE );
  }



WORD CWDDE_Free ( void )
{
    ResetATIConfig ();
    CWDDE_InitOK = FALSE;

    return ( TRUE );
}

WORD CWDDE_EnableTimewarp ( void )
  {
    HDC hDC;

    if ( ! CWDDE_InitOK )
        return ( FALSE );

    hDC = CreateDC ( "DISPLAY", NULL, NULL, NULL );
    if ( hDC == NULL )
        return ( FALSE );

    /* Enable all the TimeWarp stuff */
 
    if ( ExtEscape ( hDC, Control_TimewarpIsSupported, 0, NULL, NULL, NULL) == -1 )
      {
        DeleteDC ( hDC );
        return ( FALSE );
      }

    if ( ExtEscape ( hDC, Control_TimewarpIsEnabled, 0, NULL, NULL, NULL) == -1 )
      {
        if ( ExtEscape ( hDC, Control_TimewarpEnable, 0, NULL, NULL, NULL) == -1 )
          {
            DeleteDC ( hDC );
            return ( FALSE );
          }
      }

    DeleteDC ( hDC );

    TimewarpEnabled = TRUE;

    return ( TRUE );
  }

DWORD CWDDE_GetPixelPitch ( void )
  {
    if ( ! CWDDE_InitOK )
        return ( DWORD_FAIL );

    return ( ATIConfig.dwEnginePitch );
  }


DWORD CWDDE_GetByteStride ( void )
  {
    if ( ! CWDDE_InitOK )
        return ( DWORD_FAIL );

    return ( ATIConfig.dwEnginePitch *
             ( ATIConfig.dwBoardBpp / 8L ) );
  }


DWORD CWDDE_GetBoardType ( void )
  {
    if ( ! CWDDE_InitOK )
        return ( DWORD_FAIL );

    return ( ATIConfig.dwBoardType );
  }


WORD CWDDE_GetAperture ( LPPTR48 lpPtr48 )
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    *lpPtr48 = ATIConfig.AperturePtr;

    return ( TRUE );
  }



WORD CWDDE_GetScreenPtr ( LPPTR48 lpPtr48 )
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    *lpPtr48 = ATIConfig.AperturePtr;
    lpPtr48->Offset += ATIConfig.DisplayOffset;

    return ( TRUE );
  }



WORD CWDDE_GetMemRegPtr ( LPPTR48 lpPtr48 )
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    *lpPtr48 = ATIConfig.MemRegPtr;

    if ( ATIConfig.MemRegPtr.Selector == 0 )
        return ( FALSE );

    return ( TRUE );
  }
DWORD CWDDE_GetDesktopWidth(void)
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwDesktopWidth );
  }

DWORD CWDDE_GetDesktopHeight(void)
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwDesktopHeight );
  }


DWORD CWDDE_GetBoardBpp ( void )
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwBoardBpp );
  }

DWORD CWDDE_GetAperatureType (void)
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwApertureType );
  }

DWORD CWDDE_GetRam ( void )
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwRealRamAvailable );
  }

DWORD CWDDE_GetMajorVersion(void)
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwMajorVersion );
  }

DWORD CWDDE_GetMinorVersion(void)
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwMinorVersion );
  }

DWORD CWDDE_GetBuildNumber(void)
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.dwBuildNumber );
  }

DWORD CWDDE_GetDisplayOffset ( void )
  {
    if ( ! CWDDE_InitOK )
        return ( FALSE );

    return ( ATIConfig.DisplayOffset );
  }


WORD CWDDE_Initialized ( void )
  {
    return ( CWDDE_InitOK );
  }

BOOL CWDDE_GetATISig(char *sig)
{
    if ( ! CWDDE_InitOK )
        return ( FALSE );
	
	strcpy(sig, (char *)ATIConfig.ATISig);
    return ( TRUE );
}

BOOL CWDDE_GetDriverName(char *dname)
{
    if ( ! CWDDE_InitOK )
        return ( FALSE );
	
	strcpy(dname, (char *)ATIConfig.DriverName);
    return ( TRUE );
}

/*--------------------------- Private Functions ----------------------------*/

static void ResetATIConfig ( void )
  {
    PTR48 NullPtr = { NULL, NULL, NULL };

    lstrcpy ( (char *)ATIConfig.ATISig, "" );
    lstrcpy ( (char *)ATIConfig.DriverName, "" );

    ATIConfig.dwMajorVersion     = 0;
    ATIConfig.dwMinorVersion     = 0;
    ATIConfig.dwBuildNumber      = 0;
    ATIConfig.dwDesktopWidth     = 0;
    ATIConfig.dwDesktopHeight    = 0;
    ATIConfig.dwEnginePitch      = 0;
    ATIConfig.dwRealRamAvailable = 0;
    ATIConfig.dwVGABoundary      = 0;
    ATIConfig.dwBpp              = 0;
    ATIConfig.dwBoardBpp         = 0;
    ATIConfig.dwColorFormat      = 0;
    ATIConfig.dwAlphaBitMask     = 0;
    ATIConfig.dwConfigBits       = 0;
    ATIConfig.dwAsicRevision     = 0;
    ATIConfig.dwRomVersion       = 0;
    ATIConfig.dwBoardType        = 0;
    ATIConfig.dwApertureType     = 0;
    ATIConfig.AperturePtr        = NullPtr;
    ATIConfig.DisplayOffset      = 0;
    ATIConfig.MemRegPtr          = NullPtr;
  }


#if 0

#define GUI_STAT                  0xCE * 4

WORD  wSel;

DWORD dAdd;

int   m64busy;



wSel = Ptr48.Selector;

dAdd = Ptr48.Offset;



__asm

{

  mov [m64busy], 0h

  mov ax,  word ptr [wSel]

  mov ebx, dword ptr [dAdd]

  push es

  mov  es, ax

  mov  eax, dword ptr es:[ebx + GUI_STAT]

// check if engine is actuall drawing

  and  eax, 0x01h

  jnz  short done 

  mov [m64busy], 1h

done:

  pop  es

}

#endif

#if 0
int bollo;

void *malloc(size_t size)
{
	int i;
	void *p=GlobalAlloc(GMEM_FIXED,size);

	if(size==0x110)
		bollo=3;
	for(i=0;i<10000;++i)
		if(crapper[i].size==0)
		{
			crapper[i].size=size;
			crapper[i].ptr=p;
			break;
		}
	return(p);
}

void free(void *p)
{
	int i;
	
	if(p==NULL)
		return;
	for(i=0;i<10000;++i)
		if(crapper[i].ptr == p)
		{
			crapper[i].size=0;
			crapper[i].ptr = 0;
			break;
		}
	if(i==10000)
		bollo=2;
	GlobalFree(p);
}
#endif
