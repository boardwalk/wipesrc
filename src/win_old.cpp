#define NAME "WIPEOUT"
#define TITLE "WIPEOUT '98"
#define DBG 0

#define STRICT
#include <windows.h>
#include <time.h>
#include <stdio.h>
#include "rgraphics.h"
#include "textures.h"
#include <winuser.h>
#include <mmsystem.h>
#include <direct.h>
#include <vfw.h>
#include <mciavi.h>
#include <ddraw.h>
#include <d3d.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "resource.h"
#include "winh.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <io.h>
#include "timc.h"
#include <math.h>
#include "decl.h"
#include "malloc.h"

#define IDI_WIPEOUT 101

#define SCALEIT(x) ((x)*upres)
#define SCALEZ(z) ((z)*zscale)

#define SMALL 0
#define LOW 1
#define MEDIUM 2
#define HIGH 3

extern TextureFormats availableTextures;

//#define  SCALEIT(x) (x)

//-----------------------------------------------------------------------------
// Global variables for using the D3D sample framework class
//-----------------------------------------------------------------------------
BOOL g_bActive = FALSE;
BOOL g_bReady = FALSE;
BOOL g_bExitting = FALSE;
extern BOOL g_bAppUseFullScreen;
BOOL g_bWindowed = TRUE;
BOOL g_bShowStats = TRUE;
RECT g_rcWindow;
extern BOOL g_bAppUseZBuffer;
extern BOOL g_bAppUseBackBuffer;
TCHAR* g_strAppTitle = TEXT("WipeOut 98");

extern LPDIRECTDRAW4 g_pDD4;
extern LPDIRECTDRAWSURFACE4 g_pddsPrimary;
extern LPDIRECTDRAWSURFACE4 g_pddsBackBuffer;
extern LPDIRECT3DDEVICE3 g_pd3dDevice;

typedef struct
{
  int32_t id;
  int32_t flags;
  int32_t bnum;
  int16_t textureX, textureY;
  int16_t textureH, textureW;
} TimData;

extern DynamicHeap* heap;
extern char winLoseIn;
extern int32_t inmenutest;

#define BLACK_NOT_DRAWN 1
#define BLACK1_TRANSL 2
#define BLACK2_TRANSL 3
#define BLACK3_TRANSL 4
#define BLACK4_TRANSL 5
#define BLACK5_TRANSL 6
#define BLACK6_TRANSL 7
#define WHITE1_TRANSL 8
#define WHITE2_TRANSL 9
#define WHITE3_TRANSL 10
#define WHITE4_TRANSL 11
#define WHITE5_TRANSL 12
#define WHITE6_TRANSL 13
#define WHITE7_TRANSL 14
#define WHITE8_TRANSL 15
#define RED_TRANSL 16
#define RED_LIGHT_FADE 16
#define BRIGHT_RED_TRANSL 17
#define YELLOW_TRANSL 18
#define YELLOW_WHITE_TRANSL 19
#define BLUE_TRANSL 20
#define PURPLE_TRANSL 21
#define UNUSED1_TRANSL 22
#define UNUSED2_TRANSL 23
#define UNUSED3_TRANSL 24
#define UNUSED4_TRANSL 25
#define ATTRACT 1

#define MCI_MCIAVI_PLAY_FULLSCREEN 0x02000000L
#define DIVIS 0.00195312

#define TEXTURESJR 2621440

typedef struct {
  int16_t xsize, ysize;
  float mult;
} RESOLUTION;

BOOL Rendering;
BOOL backFromDead = FALSE;

extern int16_t rendering;
RESOLUTION reses[] = {
    {320, 240, 1.0f},
    {400, 300, 1.25f},
    {512, 384, 1.6f},
    {640, 480, 2.0f}};

int16_t screenres = 3;
float upres = 2.0f;
float zscale = 1.0 / 65536.0f; //1.0/32768.0f;

int32_t menuTexturesIn = 0;
int32_t Keydelay, Keyrepeat, sSaver;
int16_t dodump = 0, FrameState = 0;
int16_t PageXOff, PageYOff;

extern int32_t ctrlNeedEndText;
extern int32_t ctrlNeedWinLose;
extern int32_t ctrlNeedBackPoly;
extern int32_t ctrlNeedRaceInfo;
extern int32_t ctrlNeedDeSelectScreen;
extern int32_t ctrlNeedNewHighScores;
extern int32_t ctrlNeedRacePoints;
extern int32_t ctrlNeedChampTable;
extern int32_t ctrlNeedAllVenomComplete;
extern int32_t ctrlNeedVenomComplete;
extern int32_t ctrlNeedContinueGameConfirm;
extern int32_t ctrlNeedEndTextVar;
extern int32_t ctrlNeedWinLoseVar;
extern int32_t ctrlNeedInGameReset;
extern int32_t ctrlNeedInSingleGameConfirm;
extern int32_t ctrlNeedInGameConfirm;
extern int32_t ctrlNeedTargetIcon;
extern int32_t ctrlCDPaused;
extern int32_t ctrlfirsttime;
extern int32_t ctrlmode;
extern char ctrlmachinePaused;
extern int32_t ctrlcdPause;
extern int16_t startIceText;
extern int16_t startFireText;

typedef struct {
  LPDIRECTDRAWSURFACE4 lpDDSTex;
  DDSURFACEDESC2 ddsd;
  LPDIRECT3DTEXTURE2 Texture;
} TEXTURE, *PTEXTURE;

TEXTURE gTex[500] = {0};

char JJSTextureTable[] = {0, 1, 2, 3, 4, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 16, 17, 18, 19, 20, 11, 20, 12, 13, 21, 14, 15,
                          16, 17, 18, 19};
char JJSTextureMenuTable[] = {0, 1, 2, 3, 4, 0, 1, 2, 10, 4, 5, 6, 7, 8, 9, 3, 16, 17, 18, 19, 20, 11, 20, 12, 13, 21, 14,
                              15, 16, 17, 18, 19};

char lastfile[256];

typedef struct
{
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0;
  int16_t x1, y1;
  int16_t x2, y2;
  int16_t x3, y3;
} POLY_F4; /* Flat Quadrangle */

typedef struct
{
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0, z0, pad4;
  uint8_t u0, v0;
  uint16_t clut;
  int16_t x1, y1, z1, pad5;
  uint8_t u1, v1;
  uint16_t tpage;
  int16_t x2, y2, z2, pad6;
  uint8_t u2, v2;
  uint16_t pad1;
  int16_t x3, y3, z3, pad7;
  uint8_t u3, v3;
  uint16_t pad2;
} POLY_FT4; /* Flat Textured Quadrangle */

typedef struct {
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0;
  uint8_t r1, g1, b1, pad1;
  int16_t x1, y1;
  uint8_t r2, g2, b2, pad2;
  int16_t x2, y2;
} POLY_G3; /* Gouraud Triangle */

typedef struct {
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0;
  uint8_t r1, g1, b1, pad1;
  int16_t x1, y1;
  uint8_t r2, g2, b2, pad2;
  int16_t x2, y2;
  uint8_t r3, g3, b3, pad3;
  int16_t x3, y3;
} POLY_G4; /* Gouraud Quadrangle */

typedef struct {
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0, z0, pad4;
  uint8_t u0, v0;
  uint16_t clut;
  uint8_t r1, g1, b1, p1;
  int16_t x1, y1, z1, pad5;
  uint8_t u1, v1;
  uint16_t tpage;
  uint8_t r2, g2, b2, p2;
  int16_t x2, y2, z2, pad6;
  uint8_t u2, v2;
  uint16_t pad2;
} POLY_GT3; /* Gouraud Textured Triangle */

typedef struct {
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0, z0, pad4;
  uint8_t u0, v0;
  uint16_t clut;
  uint8_t r1, g1, b1, p1;
  int16_t x1, y1, z1, pad5;
  uint8_t u1, v1;
  uint16_t tpage;
  uint8_t r2, g2, b2, p2;
  int16_t x2, y2, z2, pad6;
  uint8_t u2, v2;
  uint16_t pad2;
  uint8_t r3, g3, b3, p3;
  int16_t x3, y3, z3, pad7;
  uint8_t u3, v3;
  uint16_t pad3;
} POLY_GT4; /* Gouraud Textured Quadrangle */

typedef struct {
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0, z0, pad4;
  uint8_t u0, v0;
  uint16_t clut;
  int16_t x1, y1, z1, pad5;
  uint8_t u1, v1;
  uint16_t tpage;
  int16_t x2, y2, z2, pad6;
  uint8_t u2, v2;
  uint16_t pad1;
} POLY_FT3; /* Flat Textured Triangle */

typedef struct {
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0;
  uint8_t u0, v0;
  uint16_t clut;
  int16_t w, h;
} SPRT; /* free size Sprite */

typedef struct {
  uint32_t tag;
  uint8_t r0, g0, b0, code;
  int16_t x0, y0;
  uint8_t u0, v0;
  uint16_t clut;
} SPRT_16; /* 8x8 Sprite */

typedef SPRT_16 SPRT_8;
char gszErrMsg[64];
BOOL gbCIFInit;

static BOOL FirstActivate = TRUE;

//menu graphic detail stuff - SJR
int16_t ft = 0;
char MipMap = 0;

int16_t texpage = -1;
int16_t numvert = 0;
int16_t tester = 0;
typedef struct {
  uint16_t blue : 5;
  uint16_t green : 5;
  uint16_t red : 5;
} BIT15;

BIT15 ATIpalette[256];
BOOL bTexEn = FALSE;

BOOL trans = FALSE;
BOOL MPEGplaying;
HWND g_hwndMCIWnd;
DWORD dwMCIWndStyle;
HINSTANCE hinst;
HWND hwnd;
char globalPilot[255] = {0};

extern char DepthFadeTble[], WhitePal;
extern int16_t int32_t CurrTrkNo;
int16_t randlist[2000];
int16_t* randnum;
char* TextureBuffer;
int16_t ctr;
int16_t trackNum = 0;
char* SaveDirBase;
char* DataDirBase;
char* MovieDirBase;
extern UINT wDeviceID;
extern char ctrlshowMenu;
extern int32_t ctrlNeedInGameReset;
int16_t WinPaused = 0;
extern int32_t ctrlchoice;
extern int32_t screenZ;

struct sMovie {
  char szMoviePathName[MAX_PATH];
  int32_t width;
  int32_t height;
  int32_t fullScreen;
};

BOOL shellMoviePlayingG = FALSE;
BOOL shellMovieFinishedG = TRUE;
UINT shellMovieDeviceIDG;

//SJR

float joyModXVal;
float joyModYVal;
extern int16_t JoyCentreX, JoyCentreY;
extern int16_t left_scale, right_scale;
extern float left_scale_factor, right_scale_factor;
extern uint16_t JoyXVal, JoyYVal;
int16_t JoyButton1, JoyButton2;

HANDLE gInstance;

#define HIGH_SENS 8
#define MEDIUM_SENS 16
#define LOW_SENS 32

/* Type/Structure Definitions. */
typedef struct {
  uint16_t jX;
  uint16_t jY;
  uint16_t jDir;
  uint16_t jButton1;
  uint16_t jButton2;
} JOYSTICK;

typedef struct {
  uint16_t mX;
  uint16_t mY;
  uint16_t mDir;
  uint16_t mButton1;
  uint16_t mButton2;
  uint16_t mButton3;
} MOUSE;

typedef struct {
  int32_t JoyUp;
  int32_t JoyDown;
  int32_t JoyLeft;
  int32_t JoyRight;
  int32_t JoyButton1;
  int32_t JoyButton2;
  int32_t JoyButton3;
  int32_t JoyButton4;
} JOYSTICK_CONFIG;

typedef struct {
  int32_t MouseUp;
  int32_t MouseDown;
  int32_t MouseLeft;
  int32_t MouseRight;
  int32_t MouseLeftButton;
  int32_t MouseMiddleButton;
  int32_t MouseRightButton;
  int32_t MouseDummy;
} MOUSE_CONFIG;

#define _DEVICE_ON 1
#define OK 0
#define SONY_PAD 0x41
#define PAUSED 0
#define NOT_PAUSED 1

extern volatile MOUSE etc_Mouse1;
extern volatile JOYSTICK etc_Joy1;
extern volatile uint32_t etc_Mouse1Register;
extern volatile uint32_t etc_Joystick1Register;
extern volatile MOUSE_CONFIG etc_Mouse1Config;
extern volatile JOYSTICK_CONFIG etc_Joy1Config;
extern volatile char* etc_TransferBuffer1;
extern char mouse_sensitivity;
extern volatile uint8_t etc_Mouse1Switch; /* id = 3 */
extern volatile uint8_t etc_Joystick1Switch;
extern volatile uint8_t etc_InputDeviceSelect;

#define TIMER_ID 1
#define TIMER_RATE 1

//#define	GAME_WIDTH	320
//#define GAME_HEIGHT	240

void Set_Device(int32_t tmp);
void oldmain();
void QuitToDos();
void pmkey(char, char);
void ResetPitch();
void JJSReadTIM(char* filename, char* location, TimData* tim);
//char* DAlloc(void*   heap,int32_t size);
void LoadPilotIntoVram(char* filename);
void Swap();
int32_t JJSLoadSprites();
BOOL JJSLoadRage();
BOOL InitATI3DCIF();
void UpdateNotes(int32_t paused);

extern int32_t WinPitch;
extern int16_t WinWidth;
extern int16_t WinHeight;
extern int16_t WinHeightX2;
int32_t RealPitch;
int32_t RealBackPitch;
char *DebugScreen, *RealScreen;

LPDIRECTDRAWSURFACE4 lpBack_Buffer; // Directdraw 320X200 Surface for the  menu thing.
BOOL bActive; // is application active?
BOOL backLocked = FALSE;
extern char *CurBuffer, *CurBufferJ;
PALETTEENTRY ddPalette[256];
char* backscreen;

extern int32_t NeedKeys;

extern void finiSndObjects();

/*
 * restoreObjects
 *
 * coming back to the game...put everything back
 */

static void restoreObjects() {
  g_pddsPrimary->Restore();
  if (lpBack_Buffer)
    lpBack_Buffer->Restore();

  switch (trackNum) {
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
    SJRNewTexture("wipeout\\newgraph\\menu\\menus.sjr");
#endif
    LoadVRam("wipeout\\newgraph\\menu\\menus.pcx", 0);
    reloadScr();
    break;
  case 16: //in "press enter" screen
    reloadStartScr();
    break;
  default:
    break;
  }
  if (globalPilot[0])
    LoadPilotIntoVram(globalPilot);
  JJSLoadSprites();
  Swap();
  Swap();
}

static void restoreTextures() {
  switch (trackNum) {
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
    SJRNewTexture("wipeout\\newgraph\\menu\\menus.sjr");
    reloadScr();
    break;
  default:
    break;
  }
  if (globalPilot[0])
    LoadPilotIntoVram(globalPilot);
  Swap();
  Swap();
}

/*
 * finiObjects
 *
 * finished with all objects we use; release them
 */
static void finiObjects() {
  Cleanup3DEnvironment();
  ShellSetPrefs();
  //	SystemParametersInfo(SPI_SETKEYBOARDDELAY,Keydelay,0,0);
  //	SystemParametersInfo(SPI_SETKEYBOARDSPEED,Keyrepeat,0,0);
  //	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,sSaver,0,0);
} /* finiObjects */

void changeScreen() {
  int32_t i;

  if (g_pddsPrimary != NULL) {
    g_pddsPrimary->Release();
    //		g_pddsPrimary = NULL;
  }
  if (lpBack_Buffer != NULL) {
    lpBack_Buffer->Release();
    lpBack_Buffer = NULL;
  }
  for (i = 0; i < 500; ++i)
    if (gTex[i].lpDDSTex != NULL) {
      gTex[i].Texture->Release();
      gTex[i].lpDDSTex->Release();
      gTex[i].lpDDSTex = NULL;
    }
  WinWidth = reses[screenres].xsize;
  WinHeight = reses[screenres].ysize;
  WinHeightX2 = WinHeight * 2;
  setupScreens();
  restoreTextures();
}

int32_t FAR PASCAL WndProc(HWND hWnd, UINT message,
                           WPARAM wParam, LPARAM lParam) {
  PAINTSTRUCT ps;
  // RECT	rc;
  // SIZE	size;
  char keyhit;
  int16_t DevID;
  int32_t NotifyMsg;
  uint16_t buttons;
  uint32_t xpos, ypos;
  char temp;

  switch (message) {
  case WM_CREATE:
    break;

  case WM_SETCURSOR:
    SetCursor(NULL);
    return TRUE;

  case WM_TIMER:
    break;

  case MM_JOY1MOVE:
    if (bActive) {
      if (etc_Joystick1Switch == _DEVICE_ON) {
        xpos = LOWORD(lParam);
        ypos = HIWORD(lParam);
        buttons = wParam;
        etc_Joy1.jX = (int16_t)((float)xpos * joyModXVal);
        etc_Joy1.jY = (int16_t)((float)ypos * joyModYVal);
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
        etc_TransferBuffer1[2] = (char)~(etc_Joystick1Register >> 8);

        temp = ~etc_TransferBuffer1[3]; // This lot preserves the
        temp &= 0x9F; // brake bits, which get
        temp |= (char)(etc_Joystick1Register & 0xFF); // lost somehow otherwise
        etc_TransferBuffer1[3] = ~temp;
        return 0;
      }
    }
    break;

  case MM_JOY1BUTTONDOWN:
  case MM_JOY2BUTTONDOWN:
    if (bActive) {
      if (etc_Joystick1Switch == _DEVICE_ON) {
        xpos = LOWORD(lParam);
        ypos = HIWORD(lParam);
        buttons = wParam;
        etc_Joy1.jX = (int16_t)((float)xpos * joyModXVal);
        etc_Joy1.jY = (int16_t)((float)ypos * joyModYVal);
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
        etc_TransferBuffer1[2] = (char)~(etc_Joystick1Register >> 8);

        temp = ~etc_TransferBuffer1[3]; // This lot preserves the
        temp &= 0x9F; // brake bits, which get
        temp |= (char)(etc_Joystick1Register & 0xFF); // lost somehow otherwise
        etc_TransferBuffer1[3] = ~temp;
        return 0;
      }
    }
    break;

  case MM_JOY1BUTTONUP:
  case MM_JOY2BUTTONUP:
    if (bActive) {
      if (etc_Joystick1Switch == _DEVICE_ON) {
        xpos = LOWORD(lParam);
        ypos = HIWORD(lParam);
        buttons = wParam;
        etc_Joy1.jX = (int16_t)((float)xpos * joyModXVal);
        etc_Joy1.jY = (int16_t)((float)ypos * joyModYVal);
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
        etc_TransferBuffer1[2] = (char)~(etc_Joystick1Register >> 8);

        temp = ~etc_TransferBuffer1[3]; // This lot preserves the
        temp &= 0x9F; // brake bits, which get
        temp |= (char)(etc_Joystick1Register & 0xFF); // lost somehow otherwise
        etc_TransferBuffer1[3] = ~temp;
        return 0;
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
      if (etc_Mouse1Switch == _DEVICE_ON) {
        int16_t xtemp, fwKeys;
        char temp;

        fwKeys = wParam;
        switch (message) {
        case WM_LBUTTONDOWN:
          fwKeys |= MK_LBUTTON;
          break;
        case WM_RBUTTONDOWN:
          fwKeys |= MK_RBUTTON;
          break;
        }
        xtemp = LOWORD(lParam);
        if (xtemp > 255) {
          xtemp = 255;
          SetCursorPos(255, 100);
        }
        xtemp -= 128;
        switch (mouse_sensitivity) {
        case MEDIUM_SENS:
          xtemp /= 2;
          break;
        case LOW_SENS:
          xtemp /= 4;
          break;
        }
        if (abs(xtemp) < 10)
          xtemp = 0;
        etc_Mouse1.mX = xtemp + 128;

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

        temp = ~etc_TransferBuffer1[3]; // This lot preserves the
        temp &= 0x9F; // brake bits, which get
        temp |= (char)(etc_Mouse1Register & 0xFF); // lost somehow otherwise
        etc_TransferBuffer1[3] = ~temp;
        return 0;
      }
    }
    break;
  case WM_SYSCHAR:
  case WM_CHAR:
    return 0;
  case WM_KEYUP:
  case WM_SYSKEYUP:
    if (bActive) {
      //if (HIWORD(lParam) & 0x100)
      //{
      //pmkey(0xe0, FALSE);
      //}
      keyhit = (TCHAR)wParam;
      //keyhit |= 0x80;
      pmkey(keyhit, FALSE);

      return 0;
    }
    return 0;
    break;
  case WM_KEYDOWN:
  case WM_SYSKEYDOWN:
    if (shellMoviePlayingG) {
      //       		ShellMovieFinished();
    }
    if (bActive) {
      if (MPEGplaying)
        MPEGplaying = FALSE;

      switch (wParam) {
      case VK_F12:
        PostMessage(hWnd, WM_CLOSE, 0, 0);
        return 0;
      case VK_F11:
        if ((GetKeyState(VK_SHIFT) < 0) && (GetKeyState(VK_CONTROL) < 0))
          FrameState ^= 1;
        return 0;
      case VK_DELETE:
        return 0;
      }
      if (!(HIWORD(lParam) & 0x4000)) {
        //if (HIWORD(lParam) & 0x100)
        //{
        //pmkey(0xe0, TRUE);
        //}
        keyhit = (TCHAR)wParam;
        //keyhit &= 0x7f;
        pmkey(keyhit, TRUE);
        return 0;
      }
    }
    return 0;
  case WM_PAINT:
    if (bActive) {
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
#if 0
		if (FirstActivate)
			FirstActivate = FALSE;
		else {
			if (wParam) //activate
			{
#if 0
				SystemParametersInfo(SPI_SETKEYBOARDDELAY,0,0,0);
				SystemParametersInfo(SPI_SETKEYBOARDSPEED,0,0,0);
				SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,0,0,0);
#endif
				restoreObjects();
				CDRestart();
				SetForegroundWindow(hWnd);
				UpdateNotes(NOT_PAUSED);//paused
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
#if 0
				SystemParametersInfo(SPI_SETKEYBOARDDELAY,Keydelay,0,0);
				SystemParametersInfo(SPI_SETKEYBOARDSPEED,Keyrepeat,0,0);
				SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,sSaver,0,0);
#endif
				CDStop();
				UpdateNotes(PAUSED);//paused
				WinPaused = 1;
				RemoveTimer();
			}

		}
#endif
    break;

#if USE_ATI
  case MM_MCINOTIFY:
  case MCIWNDM_NOTIFYMODE:
    DevID = LOWORD(lParam);
    NotifyMsg = wParam;
    MPEGplaying = FALSE;
    switch (NotifyMsg) {
    case MCI_NOTIFY_ABORTED: //stopped for some reason
      if (shellMoviePlayingG) {
        //            		ShellMovieFinished();
      }
      break;
    case MCI_NOTIFY_FAILURE: //device error
      if (shellMoviePlayingG) {
        //            		ShellMovieFinished();
      }
      break;
    case MCI_NOTIFY_SUCCESSFUL: //horay!
      if (DevID == wDeviceID) //CD
      {
        CDPlay(CurrTrkNo);
      }

      if (shellMoviePlayingG) {
        //            		ShellMovieFinished();
      }

      break;
    case MCI_NOTIFY_SUPERSEDED: //not needed any more
      if (shellMoviePlayingG) {
        //            		ShellMovieFinished();
      }
      break;
    default:
      break;
    }
    break;
#endif
  }
  return DefWindowProc(hWnd, message, wParam, lParam);
} /* WindowProc */

void setupScreens() {
  HRESULT ddrval = DD_OK, ddretval;
  int32_t i;

  DDSwap();
  WinClearScreen(0);
  DDSwap();
  WinClearScreen(0);
  upres = reses[screenres].mult;
  // create 3 offscreen surfaces to cache the sprite maps
  for (i = 420; i < 423; ++i) {
    memcpy(&gTex[i].ddsd, &availableTextures.surface1555, sizeof(DDSURFACEDESC2));
    gTex[i].ddsd.dwWidth = 256; //SJR	// standard texture size
    gTex[i].ddsd.dwHeight = 256; //SJR	// standard height
    ddretval = g_pDD4->CreateSurface(&gTex[i].ddsd, &gTex[i].lpDDSTex, NULL);
    if (ddretval != DD_OK) {
      ShowErrorMessage(4002);
    }
    gTex[i].lpDDSTex->QueryInterface(IID_IDirect3DTexture2, (void**)&gTex[i].Texture);
  }
  JJSLoadSprites();
}
/*
 * doInit - do work required for every instance of the application: create
 * the window, initialize data
 */
static BOOL doInit(HANDLE hInstance, int32_t nCmdShow) {
  int32_t i;
  /*
	 * Initialise palette
	 */

  for (i = 0; i < 256; i++) {
    ddPalette[i].peRed = i;
    ddPalette[i].peGreen = i;
    ddPalette[i].peBlue = i;
    ddPalette[i].peFlags = 0;
  }

  //	mpeganim();

  /*
	 * create the main DirectDraw object
	 */
  setupScreens();
  ShowCursor(FALSE);
  return TRUE;
} /* doInit */

void CopyPal(char* palette, int16_t start, int16_t end) {
  int32_t i;

  for (i = start; i <= end; i++) {
    ddPalette[i].peRed = *palette++;
    ddPalette[i].peGreen = *palette++;
    ddPalette[i].peBlue = *palette++;
    ddPalette[i].peFlags = 0;
  }
}

void initrandy() {
  int16_t i;
  time_t t;

  t = time(NULL);

  srand(t);

  for (i = 0; i < 2000; i++)
    randlist[i] = rand() & 0x7FFF;

  randnum = &randlist[0];
  ;
  ctr = 0;
}

int32_t randy() {
  int16_t num;

  num = *randnum++;

  ctr++;
  if (ctr == 2000) {
    randnum = &randlist[0];
    ;
    ctr = 0;
  }

  return num;
}

int32_t vRamSize() {
  return 0;
}

//-----------------------------------------------------------------------------
// Name: InitializeWindowEnvironment()
// Desc: This function initializes our standard Windows' stuff
//-----------------------------------------------------------------------------
HWND InitializeWindowEnvironment(HINSTANCE hInst) {
  int32_t winx, winy;

  WNDCLASS wndClass = {CS_HREDRAW | CS_VREDRAW, WndProc, 0, 0, hInst,
                       LoadIcon(hInst, MAKEINTRESOURCE(IDI_APPLICATION)),
                       LoadCursor(NULL, IDC_ARROW),
                       (HBRUSH)GetStockObject(WHITE_BRUSH), NULL,
                       TEXT("Render Window")};
  RegisterClass(&wndClass);

  if (g_bWindowed) {
    winx = 650;
    winy = 510;
  } else {
    winx = 640;
    winy = 480;
  }
  HWND hWnd = CreateWindow(TEXT("Render Window"), g_strAppTitle,
                           WS_OVERLAPPEDWINDOW, CW_USEDEFAULT,
                           CW_USEDEFAULT, winx, winy, 0L, 0L, hInst, 0L);

  ShowWindow(hWnd, SW_SHOWNORMAL);
  UpdateWindow(hWnd);

  return hWnd;
}

/*
 * WinMain - initialization, message loop
 */
int32_t PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrevInstance,
                       LPSTR lpCmdLine, int32_t nCmdShow) {
  int32_t tmp;
  int32_t retval;
  BOOL b_Voodoo;
  // Create our main window
  if (!strcmp(lpCmdLine, "-win"))
    g_bWindowed = TRUE;
  else
    g_bWindowed = FALSE;
  // Create our main window
  if (!strcmp(lpCmdLine, "-voodoo"))
    b_Voodoo = TRUE;
  else
    b_Voodoo = FALSE;
  HWND hWnd = InitializeWindowEnvironment(hInst);

  // Save the window size/pos for switching modes
  GetWindowRect(hWnd, &g_rcWindow);

  if (FAILED(renderInitialise(hWnd, !g_bWindowed, b_Voodoo))) {
    Cleanup3DEnvironment();
    return 0;
  }

  tmp = vRamSize();
  initrandy();
  tmp = randy();
  lpCmdLine = lpCmdLine;
  hPrevInstance = hPrevInstance;
#if DEBUG_LOCKED
  DebugScreen = (char*)malloc(640 * 480);
#endif
  TextureBuffer = (char*)malloc(TEXTURESJR);
  WinWidth = reses[screenres].xsize;
  WinHeight = reses[screenres].ysize;
  WinHeightX2 = WinHeight * 2;
  hinst = hInst;
  SaveDirBase = (char*)malloc(120);
  retval = ShellGetGameDirectory(SaveDirBase);
  if (retval != ERROR_SUCCESS)
    strcpy(SaveDirBase, "c:\\");

  DataDirBase = (char*)malloc(120);
  retval = ShellGetDataDirectory(DataDirBase);
  if (retval != ERROR_SUCCESS)
    strcpy(DataDirBase, "c:\\");

  MovieDirBase = (char*)malloc(120);
  retval = ShellGetMovieDirectory(MovieDirBase);
  if (retval != ERROR_SUCCESS)
    strcpy(MovieDirBase, "c:\\");

  chdir(DataDirBase);
  // Initialise Direct Draw
  if (!doInit(hInst, nCmdShow)) {
    return FALSE;
  }

  CDStop();
  oldmain();
  ShowCursor(TRUE);
  joyReleaseCapture(JOYSTICKID1);
  DestroyWindow(hwnd);
  return 0;

} /* WinMain */

int32_t ProcessMessages() {
  MSG msg;
  do {
    if (backFromDead) {
      backFromDead = FALSE;
      while (PeekMessage(&msg, NULL, MM_JOY1MOVE, MM_JOY2BUTTONUP, PM_REMOVE))
        ;
    }
    while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE)) // Clear all outstanding messages
    {
      if (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        if (PeekMessage(&msg, NULL, MM_JOY1MOVE, MM_JOY2BUTTONUP, PM_REMOVE)) {
          TranslateMessage(&msg);
          DispatchMessage(&msg);
        }
        if (bActive)
          return 0;
      } else {
        if (bActive)
          return 1;
      }
    }
  } while (!bActive);

  if (!bActive)
    ProcessMessages();
  return 0;
}

DDSURFACEDESC2 ddsd;
DDSURFACEDESC2 backDdsd;
int32_t old_pitch = 0;

int32_t LockBackBuffer() {
  LPBYTE ptr;
  // int32_t			i;
  HRESULT ddrval;
  if (bActive && !backLocked) {
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    ddrval = g_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
#if DEBUG_LOCKED
    WinPitch = WinWidth;
    RealPitch = ddsd.lPitch;
#else
    WinPitch = ddsd.lPitch;
#endif
    if (WinPitch != old_pitch) // pitch has changed
    {
      //			ResetPitch();
      old_pitch = WinPitch;
    }

    if (ddrval == DDERR_SURFACEBUSY) {
      MessageBeep(0);
      return 1;
    }
    if (ddrval == DDERR_SURFACELOST) {
      MessageBeep(0);
      g_pddsBackBuffer->Restore();
      return 1;
    }

    ptr = (LPBYTE)ddsd.lpSurface;
    if (ptr == NULL)
      return 1;
      // Set my render buffers to the back screen
#if DEBUG_LOCKED
    CurBuffer = (char*)DebugScreen;
    CurBufferJ = (char*)DebugScreen;
    RealScreen = (char*)ptr;
#else
    CurBuffer = (char*)ptr;
    CurBufferJ = (char*)ptr;
#endif
    backLocked = TRUE;
  }

  return 0;
}

void WinBeep() {
  MessageBeep(0);
}

void UnlockBackBuffer() {
#if DEBUG_LOCKED
  RECT rect;

  rect.left = 0;
  rect.top = 0;
  rect.right = WinWidth;
  rect.bottom = WinHeight;

  // memcpy(RealScreen, (DebugScreen + (640*150)), ((WinHeight - 150) *	   WinWidth));
  PitchMemCpy(RealScreen, DebugScreen, &rect);
  memset(DebugScreen, 0, (WinHeight * WinWidth));
#endif
  if (bActive && backLocked) {
    g_pddsBackBuffer->Unlock(NULL);
    backLocked = FALSE;
  }
}

void BltClearToScreen() {
  RECT dest_rect, src_rect;
  DWORD dwFlags;
  DDBLTFX DDBltFX;
  HRESULT ddrval;

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

  ddrval = g_pddsBackBuffer->Blt(&dest_rect, lpBack_Buffer, &src_rect, dwFlags, &DDBltFX);
  if (ddrval != DD_OK) {
    // Msg("Blt copy failed err=%d", ddrval);
    switch (ddrval) {
    case DDERR_SURFACELOST:
      MessageBeep(0);
      break;
    case DDERR_SURFACEBUSY:
      MessageBeep(0);
      break;
    };
  }
}

void DDSwap() {
  if (bActive) {
    renderFlipSurface();
  }
}

void DrawIT() {
  DDSURFACEDESC2 ddsd;
  LPBYTE ptr;
  // int32_t			i;
  HRESULT ddrval;
  static int32_t err_count = 0;

  // Flip surfaces
  if (bActive) {
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);

    ddrval = g_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);

    if (ddrval == DDERR_SURFACEBUSY) {
      err_count++;
    }
    if (ddrval == DDERR_SURFACELOST) {
      MessageBeep(0);
      g_pddsBackBuffer->Restore();
    }

    ptr = (LPBYTE)ddsd.lpSurface;
    if (ptr == NULL)
      return;
    // Set my render buffers to the back screen
    CurBuffer = (char*)ptr;
    CurBufferJ = (char*)ptr;

    // Render_Triangles();
    // Move_Triangles();
    g_pddsBackBuffer->Unlock(NULL);

    while (1) {
      HRESULT ddrval;
      ddrval = g_pddsPrimary->Flip(NULL, 0);
      if (ddrval == DD_OK) {
        break;
      }
      if (ddrval == DDERR_SURFACELOST) {
        ddrval = g_pddsPrimary->Restore();
        if (ddrval != DD_OK) {
          break;
        }
      }
      if (ddrval != DDERR_WASSTILLDRAWING) {
        break;
      }
    }
  }
}

void WinQuit() {
#if DEBUG_LOCKED
  free(DebugScreen);
#endif
  finiObjects();
  finiSndObjects();
  PostQuitMessage(0);
}

void WinClearScreen(char colour) {
  DDBLTFX ddbltfx;

  memset(&ddbltfx, 0, sizeof(ddbltfx));
  ddbltfx.dwSize = sizeof(ddbltfx);
  ddbltfx.dwFillColor = colour;
  g_pddsBackBuffer->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
}

void PitchMemCpy(char* dest, char* source, RECT* rect) {
  int32_t loop;

  for (loop = 0; loop < (rect->bottom - rect->top); loop++) {
    memcpy(dest, source, (rect->right - rect->left));
    dest += RealPitch;
    source += (rect->right - rect->left);
  }
}

void PitchMemCpy320X200(char* dest, char* source) {
  RECT rect;
  int32_t loop;

  rect.left = 0;
  rect.right = WinWidth;
  rect.top = 0;
  rect.bottom = WinHeight - 1;
  for (loop = 0; loop < (rect.bottom - rect.top); loop++) {
    memcpy(dest, source, (rect.right - rect.left));
    dest += RealBackPitch;
    source += (rect.right - rect.left);
  }
}

int32_t numoff = 0;
int32_t numsmall = 0;

void setRenderBegin() {
  g_pd3dDevice->BeginScene();
  if (!Rendering) {
    //		ATI3DCIF_RenderBegin(ghRC);
    Rendering = TRUE;
    //		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_PRIM_TYPE,
    //						   &quad);
    //		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_SHADE_MODE,
    //						   &shade);
    bTexEn = FALSE;
    texpage = 420;
    //		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &gTex[texpage].hTX);
    //		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_EN, &bTexEn);
    //		ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_LIGHT, &light);
    //		if (ft&1)
    //			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteron);
    //		else
    //			ATI3DCIF_ContextSetState(ghRC, C3D_ERS_TMAP_FILTER, &eTexFilteroff);
    trans = FALSE;
  }
}

void setRenderEnd() {
  g_pd3dDevice->EndScene();
  if (Rendering) {
    //		ATI3DCIF_RenderEnd();
    Rendering = FALSE;
  }
}

void sendList() {
  if (numvert) {
    setRenderBegin();
    //		ATI3DCIF_RenderPrimList((void **)testlist,numvert);

    numvert = 0;
  }
}

int16_t jjstestsrc = 0, jjstestdst = 0, jjstest3 = 128;

#if 0
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
#endif
int16_t GetThePage(int16_t page, int16_t u, int16_t v) {
  int16_t newpage;
  char* tex = menuTexturesIn ? JJSTextureMenuTable : JJSTextureTable;

  newpage = tex[page & 0x1f];
  if (newpage > 4)
    newpage += 400;
  else if (!menuTexturesIn) {
    PageXOff = u >> 5;
    PageYOff = v >> 5;
    newpage *= 64;
    newpage += PageXOff;
    newpage += PageYOff << 3;
    PageXOff <<= 5;
    PageYOff <<= 5;
  }
  return newpage;
}

void JJSDrawSprite(SPRT_8* sp, int16_t w, int16_t h) {
  static D3DTLVERTEX vert[4] = {0}, *v;
  vert[0].color = vert[1].color = vert[2].color = vert[3].color = RGB_MAKE(255, 255, 255);
  //	C3D_PVTCF       v;
  int16_t tpage;
  int16_t nu, nv;
  SPRT_8 s2 = *sp;

  nu = s2.u0;
  nv = s2.v0;
  if (s2.y0 < 0) {
    nv -= s2.y0;
    h += s2.y0;
    s2.y0 = 0;
  } else if (s2.y0 + h >= reses[screenres].ysize) {
    if (s2.y0 >= reses[screenres].ysize)
      return;
    h = reses[screenres].ysize - s2.y0;
  }
  tpage = GetThePage(s2.clut, nu, nv);
  if (tpage == 415)
    tpage = 422;
  if (s2.g0 && tpage == 421) {
    tpage = 422;
    nv += 128;
  }
  g_pd3dDevice->SetTexture(0, gTex[tpage].Texture);
  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
#if 0
	if(texpage !=tpage)
	{
		sendList();
		texpage=tpage;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &gTex[texpage].hTX);
	}
	if(!SetState(C3D_EPRIM_QUAD,C3D_ESH_FLAT,TRUE,s2.b0==RED_TRANSL?RED_TRANSL:BLACK_NOT_DRAWN,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
#endif
  v = vert;
  v->sx = s2.x0;
  v->sy = s2.y0;
  v->sz = 0;
  v->tu = (nu + 0.5f) / 256.0f;
  v->tv = (nv + 0.5f) / 256.0f;
  v->rhw = 1.0;
  ++v;
  v->sx = s2.x0 + (float)w;
  v->sy = s2.y0;
  v->sz = 0;
  v->tu = (nu + w + 0.5f) / 256.0f;
  v->tv = (nv + 0.5f) / 256.0f;
  v->rhw = 1.0;
  ++v;
  v->sx = s2.x0 + (float)w;
  v->sy = s2.y0 + (float)h;
  v->sz = 0;
  v->tu = (nu + w + 0.5f) / 256.0f;
  v->tv = (nv + h + 0.5f) / 256.0f;
  v->rhw = 1.0;
  ++v;
  v->sx = s2.x0;
  v->sy = s2.y0 + (float)h;
  v->sz = 0;
  v->tu = (nu + 0.5f) / 256.0f;
  v->tv = (nv + h + 0.5f) / 256.0f;
  v->rhw = 1.0;
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vert, 4, D3DDP_DONOTCLIP);
}

void JJS_Sprite(SPRT* sp) {
  JJSDrawSprite((SPRT_8*)sp, sp->w, sp->h);
}

void JJS_Sprite8(SPRT_8* sp) {
  JJSDrawSprite(sp, 8, 8);
}

void SetColor(D3DTLVERTEX* v, char r, char g, char b) {
  if (g) {
    int32_t r;
    if (g == 25) {
      v->color = RGB_MAKE(40, 40, 255); // Speed up
    } else {
      r = DepthFadeTble[(g << 8) + WhitePal];
      v->color = RGB_MAKE(ddPalette[r].peRed, ddPalette[r].peGreen, ddPalette[r].peBlue);
    }
  } else if (b) {
    switch (b) {
    case BLACK2_TRANSL:
      v->color = RGB_MAKE(180, 180, 180);
      break;
    case RED_LIGHT_FADE:
      v->color = RGB_MAKE(255, 10, 0);
      break;
    case WHITE2_TRANSL:
    default:
      v->color = RGB_MAKE(100, 100, 100);
      break;
    }
  } else {
    v->color = RGB_MAKE(ddPalette[r].peRed, ddPalette[r].peGreen, ddPalette[r].peBlue);
  }
}

void JJSDrawPolyF4(POLY_F4* p) {
  static D3DTLVERTEX vert[4] = {0}, *v;

  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
  g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
//	C3D_PVTCF       v;
#if 0
	if(!SetState(C3D_EPRIM_QUAD,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();

#endif
  v = vert;
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  ++v;
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  ++v;
  v->sx = SCALEIT(p->x3);
  v->sy = SCALEIT(p->y3);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  ++v;
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vert, 4, D3DDP_DONOTLIGHT);
}

void JJSDrawPolyG4(POLY_G4* p) {
  static D3DTLVERTEX vert[4] = {0}, *v;
  //	C3D_PVTCF       v;

  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
  g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
#if 0
	if(!SetState(C3D_EPRIM_QUAD,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
#endif
  v = vert;
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  ++v;
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  v->sz = 0;
  SetColor(v, p->r1, p->g1, p->b1);
  ++v;
  v->sx = SCALEIT(p->x3);
  v->sy = SCALEIT(p->y3);
  v->sz = 0;
  SetColor(v, p->r3, p->g3, p->b3);
  ++v;
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  v->sz = 0;
  SetColor(v, p->r2, p->g2, p->b2);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vert, 4, D3DDP_DONOTLIGHT);
}

void setst(D3DTLVERTEX* vv, int16_t u, int16_t v, int16_t page) {
  if (menuTexturesIn || page >= 400) {
    vv->tu = (u * (1.0f / 256.0f));
    vv->tv = (v * (1.0f / 256.0f));
  } else {
    vv->tu = ((u - PageXOff) * (1.0f / 32.0f));
    vv->tv = ((v - PageYOff) * (1.0f / 32.0f));
  }
}

void JJSDrawPolyFT4(POLY_FT4* p) {
  static D3DTLVERTEX vert[4] = {0}, *v;
  vert[0].color = vert[1].color = vert[2].color = vert[3].color = RGB_MAKE(255, 255, 255);
  //	C3D_PVTCF       v;
  //C3D_PRSDATA chkey;
  int16_t temp;

  temp = GetThePage(p->tpage, (p->u0 + p->u1 + p->u2 + p->u3) / 4, (p->v0 + p->v1 + p->v2 + p->v3) / 4);
  if (temp == 417 || temp == 418) // nasty horrible bodge
  {
    if (p->u0 == 0 && p->v0 == 24) {
      if (temp == 417) {
        p->u0 = 0;
        p->v0 = 64;
        p->u1 = 127;
        p->v1 = 64;
        p->u2 = 0;
        p->v2 = 203;
        p->u3 = 127;
        p->v3 = 203;
      } else {
        p->u0 = 128;
        p->v0 = 64;
        p->u1 = 255;
        p->v1 = 64;
        p->u2 = 128;
        p->v2 = 203;
        p->u3 = 255;
        p->v3 = 203;
      }
      temp = 420;
    }
  }
  g_pd3dDevice->SetTexture(0, gTex[temp].Texture);
  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
#if 0
	if(texpage != temp)
	{
		sendList();
		texpage=temp;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &gTex[texpage].hTX);
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
#endif

  v = vert;

  if (p->z0 <= 0) {
    v->rhw = 1.0;
    v->sz = 0.0;
  } else {
    v->sz = SCALEZ(p->z0);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  setst(v, p->u0, p->v0, temp);

  if (p->g0)
    SetColor(v, p->r0, p->g0, p->b0);

  ++v;
  if (p->z1 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z1);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  setst(v, p->u1, p->v1, temp);

  if (p->g0)
    SetColor(v, p->r0, p->g0, p->b0);

  ++v;
  if (p->z3 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z3);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x3);
  v->sy = SCALEIT(p->y3);
  setst(v, p->u3, p->v3, temp);

  if (p->g0)
    SetColor(v, p->r0, p->g0, p->b0);

  ++v;
  if (p->z2 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z2);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  setst(v, p->u2, p->v2, temp);

  if (p->g0)
    SetColor(v, p->r0, p->g0, p->b0);

  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vert, 4, D3DDP_DONOTLIGHT);
}

void JJSDrawPolyF3(POLY_F4* p) {
  static D3DTLVERTEX vert[3] = {0}, *v;
  //	C3D_PVTCF       v;

  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
  g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_FLAT);
#if 0
	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
#endif
  v = vert;
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  ++v;
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  ++v;
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, vert, 3, D3DDP_DONOTLIGHT);
}

void JJSDrawPolyG3(POLY_G3* p) {
  static D3DTLVERTEX vert[3] = {0}, *v;
  //	C3D_PVTCF       v;

  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_DISABLE);
  g_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
#if 0
	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,FALSE,p->b0,C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
#endif
  v = vert;
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  v->sz = 0;
  SetColor(v, p->r0, p->g0, p->b0);
  ++v;
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  v->sz = 0;
  SetColor(v, p->r1, p->g1, p->b1);
  ++v;
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  v->sz = 0;
  SetColor(v, p->r2, p->g2, p->b2);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, vert, 3, D3DDP_DONOTLIGHT);
}

void JJSDrawPolyFT3(POLY_FT3* p) {
  static D3DTLVERTEX vert[3] = {0}, *v;
  vert[0].color = vert[1].color = vert[2].color = RGB_MAKE(255, 255, 255);
  //	C3D_PVTCF       v;
  int16_t temp;

  temp = GetThePage(p->tpage, (p->u0 + p->u1 + p->u2) / 3, (p->v0 + p->v1 + p->v2) / 3);
  g_pd3dDevice->SetTexture(0, gTex[temp].Texture);
  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
#if 0
	if(texpage!=temp)
	{
		sendList();
		texpage=temp;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &gTex[texpage].hTX);
	}
	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,TRUE,p->b0,p->g0?C3D_ETL_MODULATE:C3D_ETL_NONE))
		if(numvert>(VBUFSIZE-4))
			sendList();
#endif
  v = vert;
  if (p->z0 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z0);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  setst(v, p->u0, p->v0, temp);

  if (p->g0)
    SetColor(v, p->r0, p->g0, p->b0);

  ++v;
  if (p->z1 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z1);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  setst(v, p->u1, p->v1, temp);

  if (p->g0)
    SetColor(v, p->r0, p->g0, p->b0);

  ++v;
  if (p->z2 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z2);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  setst(v, p->u2, p->v2, temp);

  if (p->g0)
    SetColor(v, p->r0, p->g0, p->b0);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, vert, 3, D3DDP_DONOTLIGHT);
}

void JJSDrawPolyGT3(POLY_GT3* p) {
  static D3DTLVERTEX vert[3] = {0}, *v;
  vert[0].color = vert[1].color = vert[2].color = RGB_MAKE(255, 255, 255);
  //	C3D_PVTCF       v;
  int16_t temp;
  //p->b0

  temp = GetThePage(p->tpage, (p->u0 + p->u1 + p->u2) / 3, (p->v0 + p->v1 + p->v2) / 3);
  g_pd3dDevice->SetTexture(0, gTex[temp].Texture);
  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
#if 0
	if(texpage!=temp)
	{
		sendList();
		texpage=temp;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &gTex[texpage].hTX);
	}
	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,TRUE,p->b0,p->g0?C3D_ETL_MODULATE:C3D_ETL_NONE))
	{
		if(numvert>(VBUFSIZE-4))
			sendList();
	}
#endif
  v = vert;
  v->sz = SCALEZ(p->z0);
  if (p->z0 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z0);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  setst(v, p->u0, p->v0, temp);
  //	v->color=RGB_MAKE(p->r0,p->g0,p->b0);

  ++v;
  if (p->z1 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z1);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  setst(v, p->u1, p->v1, temp);
  //	v->color=RGB_MAKE(p->r1,p->g1,p->b1);

  ++v;
  if (p->z2 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z2);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  setst(v, p->u2, p->v2, temp);
  //	v->color=RGB_MAKE(p->r2,p->g2,p->b2);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, D3DFVF_TLVERTEX, vert, 3, D3DDP_DONOTLIGHT);
}

void JJSDrawPolyGT4(POLY_GT4* p) {
  static D3DTLVERTEX vert[4] = {0}, *v;
  vert[0].color = vert[1].color = vert[2].color = vert[3].color = RGB_MAKE(255, 255, 255);
  //	C3D_PVTCF       v;
  int16_t temp;
  //p->b0

  temp = GetThePage(p->tpage, (p->u0 + p->u1 + p->u2) / 3, (p->v0 + p->v1 + p->v2) / 3);
  g_pd3dDevice->SetTexture(0, gTex[temp].Texture);
  g_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
#if 0
	if(texpage!=temp)
	{
		sendList();
		texpage=temp;
		ATI3DCIF_ContextSetState (ghRC, C3D_ERS_TMAP_SELECT, &gTex[texpage].hTX);
	}
	if(!SetState(C3D_EPRIM_TRI,C3D_ESH_FLAT,TRUE,p->b0,p->g0?C3D_ETL_MODULATE:C3D_ETL_NONE))
	{
		if(numvert>(VBUFSIZE-4))
			sendList();
	}
#endif
  v = vert;
  v->sz = SCALEZ(p->z0);
  if (p->z0 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z0);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x0);
  v->sy = SCALEIT(p->y0);
  setst(v, p->u0, p->v0, temp);

  //	v->color=RGB_MAKE(p->r0,p->g0,p->b0);

  ++v;
  if (p->z1 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z1);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x1);
  v->sy = SCALEIT(p->y1);
  setst(v, p->u1, p->v1, temp);
  //	v->color=RGB_MAKE(p->r1,p->g1,p->b1);

  ++v;
  if (p->z3 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z3);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x3);
  v->sy = SCALEIT(p->y3);
  setst(v, p->u3, p->v3, temp);
  //	v->color=RGB_MAKE(p->r3,p->g3,p->b3);

  ++v;
  if (p->z2 <= 0) {
    v->sz = 0.0;
    v->rhw = 1.0;
  } else {
    v->sz = SCALEZ(p->z2);
    v->rhw = 1.0f / v->sz;
  }
  v->sx = SCALEIT(p->x2);
  v->sy = SCALEIT(p->y2);
  setst(v, p->u2, p->v2, temp);
  //	v->color=RGB_MAKE(p->r2,p->g2,p->b2);
  g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, D3DFVF_TLVERTEX, vert, 4, 0);
}

/*
 *  UnloadTexture - unregister texture and release its surface
 */
BOOL UnloadTexture(PTEXTURE pTex) {
  if (!pTex) {
    wsprintf(gszErrMsg, "UnloadTexture: invalid pointer");
    return FALSE;
  }

  // unlock and release the texture surface
  if (pTex->lpDDSTex) {
    pTex->lpDDSTex->Release();
    pTex->lpDDSTex = NULL;
  }

  return TRUE;
  ;
}

char* LockPilot() {
  HRESULT ddretval;

  // lock texture to fill ddsd member
  ddretval = DDERR_SURFACELOST;
  while (ddretval == DDERR_SURFACELOST) {
    ddretval = gTex[420].lpDDSTex->Lock(NULL,
                                        &gTex[420].ddsd,
                                        DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
                                        NULL);
    if (ddretval == DDERR_SURFACELOST)
      gTex[420].lpDDSTex->Restore();
  }
  return (char*)gTex[420].ddsd.lpSurface;
}

void UnlockPilot() {
  HRESULT ddretval;

  ddretval = gTex[420].lpDDSTex->Unlock(NULL);
}

void ChopUp(char* to, void* from, int32_t which) {
  char* p1 = (char*)from;
  int32_t i, x, y;

  which &= 63;
  x = which & 7;
  y = which >> 3;
  p1 += y * (512 * 32);
  p1 += x * 64;
  for (i = 0; i < 32; ++i) {
    memcpy(to, p1, 64);
    p1 += 512;
    to += 64;
  }
}

void SetupbackBuffer() {
  if (!lpBack_Buffer) {
    DDSURFACEDESC2 ddsd;
    HRESULT ddrval;

    memset(&ddsd, 0, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
    ddsd.dwWidth = WinWidth;
#if USE_ATI
    ddsd.dwHeight = WinHeight;
#else
    ddsd.dwHeight = 200;
#endif
    ddrval = g_pDD4->CreateSurface(&ddsd, &lpBack_Buffer, NULL);
    if (ddrval != DD_OK) {
      ShowErrorMessage(4002);
    }
  }
}
/*
 *  LoadTexture - load a texture map from a file into a texture surface
 */

BOOL SJRLoadTextureBMPmenu(const char* lpszTexFilename) {
  int32_t fp;
  HRESULT ddretval;
  int32_t i;
  int32_t whichSurface, reals;
  int32_t texsize;
  char *ptr, *p1;
  int16_t track, other;

  track = 256;
  other = 256;
  SetupbackBuffer();
  for (i = 0; i < 420; ++i)
    if (gTex[i].lpDDSTex != NULL) {
      gTex[i].Texture->Release();
      gTex[i].lpDDSTex->Release();
      gTex[i].lpDDSTex = NULL;
    }
  // create 5 offscreen surfaces to cache the high res track
  for (i = 0; i < 5; ++i) {
    memcpy(&gTex[i].ddsd, &availableTextures.surface565, sizeof(DDSURFACEDESC2));
    gTex[i].ddsd.dwWidth = track; //SJR	// standard texture size
    gTex[i].ddsd.dwHeight = track; //SJR	// standard height
    ddretval = g_pDD4->CreateSurface(&gTex[i].ddsd, &gTex[i].lpDDSTex, NULL);
    if (ddretval != DD_OK) {
      ShowErrorMessage(4002);
    }
  }
  // create 16 offscreen surfaces to cache the texture maps
  for (i = 405; i < 420; ++i) {
    memcpy(&gTex[i].ddsd, &availableTextures.surface565, sizeof(DDSURFACEDESC2));
    gTex[i].ddsd.dwWidth = other; //SJR	// standard texture size
    gTex[i].ddsd.dwHeight = other; //SJR	// standard height
    ddretval = g_pDD4->CreateSurface(&gTex[i].ddsd, &gTex[i].lpDDSTex, NULL);
    if (ddretval != DD_OK) {
      ShowErrorMessage(4002);
    }
  }
  texsize = 32768; //128*128 * sizeof (C3D_UINT16)

  if (strcmp(lastfile, lpszTexFilename)) {
    fp = open(lpszTexFilename, O_RDONLY | O_BINARY);
    if (fp < 0) {
      ShowErrorMessage(2002);
      return FALSE;
    }

    read(fp, (void*)TextureBuffer, TEXTURESJR);
    close(fp);
    strcpy(lastfile, lpszTexFilename);
  }

  {
    WORD *tp, temp;

    tp = (WORD*)TextureBuffer;
    for (i = 0; i < TEXTURESJR / 2; ++i) {
      temp = *tp;
      *tp++ = (temp & 0x1f) | ((temp & 0x7fe0) << 1);
    }
  }

  ptr = TextureBuffer;

  for (whichSurface = 0; whichSurface < 20; whichSurface++) {
    if (whichSurface < 5)
      reals = whichSurface;
    else
      reals = whichSurface + 400;
    // get a pointer to the texture surface
    ZeroMemory(&gTex[reals].ddsd, sizeof(gTex[reals].ddsd));
    gTex[reals].ddsd.dwSize = sizeof(gTex[reals].ddsd);

    // lock texture to fill ddsd member
    ddretval = DDERR_SURFACELOST;
    while (ddretval == DDERR_SURFACELOST) {
      ddretval = gTex[reals].lpDDSTex->Lock(NULL,
                                            &gTex[reals].ddsd,
                                            DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
                                            NULL);
      if (ddretval == DDERR_SURFACELOST)
        gTex[reals].lpDDSTex->Restore();
    }
    p1 = (char*)gTex[reals].ddsd.lpSurface;

    memcpy(p1, ptr, texsize * 4);

    ptr += texsize * 4;

    ddretval = gTex[reals].lpDDSTex->Unlock(NULL);
    if (ddretval != DD_OK) {
      ShowErrorMessage(1000);
    }
    gTex[reals].lpDDSTex->QueryInterface(IID_IDirect3DTexture2, (void**)&gTex[reals].Texture);
  }
  return TRUE;
}

/*
 *  LoadTexture - load a texture map from a file into a texture surface
 */

BOOL SJRLoadTextureBMP(const char* lpszTexFilename) {
  int32_t fp;
  HRESULT ddretval;
  int32_t i;
  int32_t whichSurface, reals;
  int32_t texsize;
  char *ptr, *p1;
  int16_t track, other;

  track = 32;
  other = 256;

  if (lpBack_Buffer != NULL) {
    lpBack_Buffer->Release();
    lpBack_Buffer = NULL;
  }
  for (i = 0; i < 420; ++i)
    if (gTex[i].lpDDSTex != NULL) {
      gTex[i].Texture->Release();
      gTex[i].lpDDSTex->Release();
      gTex[i].lpDDSTex = NULL;
    }
  for (i = 0; i < 320; ++i) {
    memcpy(&gTex[i].ddsd, &availableTextures.surface565, sizeof(DDSURFACEDESC2));
    gTex[i].ddsd.dwWidth = track; //SJR	// standard texture size
    gTex[i].ddsd.dwHeight = track; //SJR	// standard height
    ddretval = g_pDD4->CreateSurface(&gTex[i].ddsd, &gTex[i].lpDDSTex, NULL);
    if (ddretval != DD_OK) {
      ShowErrorMessage(4002);
      return FALSE;
    }
  }
  // create 16 offscreen surfaces to cache the texture maps
  for (i = 405; i < 420; ++i) {
    memcpy(&gTex[i].ddsd, &availableTextures.surface565, sizeof(DDSURFACEDESC2));
    gTex[i].ddsd.dwWidth = other; //SJR	// standard texture size
    gTex[i].ddsd.dwHeight = other; //SJR	// standard height
    ddretval = g_pDD4->CreateSurface(&gTex[i].ddsd, &gTex[i].lpDDSTex, NULL);
    if (ddretval != DD_OK) {
      ShowErrorMessage(4002);
    }
  }
  texsize = 32768; //128*128 * sizeof (C3D_UINT16)

  if (strcmp(lastfile, lpszTexFilename)) {
    fp = open(lpszTexFilename, O_RDONLY | O_BINARY);
    if (fp < 0) {
      ShowErrorMessage(2002);
      return FALSE;
    }

    read(fp, (void*)TextureBuffer, TEXTURESJR);
    close(fp);
    strcpy(lastfile, lpszTexFilename);
  }

  {
    WORD *tp, temp;

    tp = (WORD*)TextureBuffer;
    for (i = 0; i < TEXTURESJR / 2; ++i) {
      temp = *tp;
      *tp++ = (temp & 0x1f) | ((temp & 0x7fe0) << 1);
    }
  }

  ptr = TextureBuffer;

  for (whichSurface = 0; whichSurface < 420; whichSurface++) {
    reals = whichSurface;
    if (reals >= 320 && reals < 405)
      continue;
    // get a pointer to the texture surface
    ZeroMemory(&gTex[reals].ddsd, sizeof(gTex[reals].ddsd));
    gTex[reals].ddsd.dwSize = sizeof(gTex[reals].ddsd);

    // lock texture to fill ddsd member
    ddretval = DDERR_SURFACELOST;
    while (ddretval == DDERR_SURFACELOST) {
      ddretval = gTex[reals].lpDDSTex->Lock(NULL,
                                            &gTex[reals].ddsd,
                                            DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
                                            NULL);
      if (ddretval == DDERR_SURFACELOST)
        gTex[reals].lpDDSTex->Restore();
    }
    p1 = (char*)gTex[reals].ddsd.lpSurface;

    ddretval = gTex[reals].lpDDSTex->Unlock(NULL);
    if (ddretval != DD_OK) {
      ShowErrorMessage(1000);
    }

    if (reals < 320) {
      ChopUp(p1, ptr, reals);
    } else {
      memcpy(p1, ptr, texsize * 4);
    }

    if (reals > 320 || ((reals & 63) == 63))
      ptr += texsize * 4;

    gTex[reals].lpDDSTex->QueryInterface(IID_IDirect3DTexture2, (void**)&gTex[reals].Texture);
  }
  return TRUE;
}

/*
 *  LoadTexture - load a texture map from a file into a texture surface
 */

int32_t JJSLoadSprites() {
  int32_t fp;
  HRESULT ddretval;
  int32_t bufsize;
  int32_t sp;
  int32_t texsize;
  char *ptr, *bmpdata, *p1;
  static char* lpszTexFilename[3] = {
      "wipeout\\newgraph\\menu\\page12.3df",
      "wipeout\\newgraph\\menu\\page15.3df",
      "wipeout\\newgraph\\menu\\page17.3df"};

  texsize = 256 * 256 * sizeof(WORD);

  bufsize = texsize;

  bmpdata = (char*)malloc(bufsize);
  if (bmpdata == NULL) {
    return FALSE;
  }

  for (sp = 0; sp < 3; ++sp) {
    fp = open(lpszTexFilename[sp], O_RDONLY | O_BINARY);
    if (fp < 0) {
      ShowErrorMessage(2002);
      wsprintf(gszErrMsg, "Could not open texture map file");
      return FALSE;
    }
    read(fp, (void*)bmpdata, 55);
    read(fp, (void*)bmpdata, bufsize);
    close(fp);

    {
      WORD *tp, temp;

      tp = (WORD*)bmpdata;
      for (int32_t i = 0; i < bufsize / 2; ++i) {
        temp = *tp;
        *tp++ = (temp >> 8) | (temp << 8);
      }
    }
    ptr = bmpdata;

    // get a pointer to the texture surface
    ZeroMemory(&gTex[sp + 420].ddsd, sizeof(gTex[sp + 420].ddsd));
    gTex[sp + 420].ddsd.dwSize = sizeof(gTex[sp + 420].ddsd);

    // lock texture to fill ddsd member
    do {
      ddretval = gTex[sp + 420].lpDDSTex->Lock(NULL,
                                               &gTex[sp + 420].ddsd,
                                               DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
                                               NULL);
      if (ddretval == DDERR_SURFACELOST)
        gTex[sp + 420].lpDDSTex->Restore();
    } while (ddretval == DDERR_SURFACELOST);
    p1 = (char*)gTex[sp + 420].ddsd.lpSurface;

    memcpy(p1, bmpdata, texsize);

    ddretval = gTex[sp + 420].lpDDSTex->Unlock(NULL);
    if (ddretval != DD_OK) {
      wsprintf(gszErrMsg, "Error unlocking texture surface for file %s", lpszTexFilename);
      gTex[sp + 420].lpDDSTex->Release();
      return FALSE;
    }
  }

  free(ptr);
  winLoseIn = 0;
  return TRUE;
}

bool SJRNewTexture(char* filename) {
  JJSLoadSprites();
  if (menuTexturesIn)
    return SJRLoadTextureBMPmenu(filename);
  else
    return SJRLoadTextureBMP(filename);
}

#if 0
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
			return DefWindowProc(hWnd, iMsg, wParam, lParam);
	}
}




/*void ShellMoviePlay(char *pName, int32_t width, int32_t height, int32_t fullScreen)
{
  char szMoviePathName[MAX_PATH];
  struct sMovie movie;
  DWORD dwAttributes;
  DWORD result;

  // Build full path name to movie file
  strcpy(szMoviePathName, pName);

  // Convert it to int16_t form for MCI layer
  result = GetShortPathName(
    szMoviePathName,                // points to a null-terminated path string
    movie.szMoviePathName,          // points to a buffer to receive the null-terminated int16_t form of the path
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

int32_t ShellMovieStart(char *moviename)
{
  DWORD dwMCIWndStyle;
  MCI_PLAY_PARMS playInfo;
  DWORD dwErr;
  int32_t result;
  int32_t width, height;
  int32_t cx, cy;

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

void ShellMovieStop()
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

void ShellMovieFinished()
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


void mpeganim()
{
	int32_t fp;
	char *mpegname;
	int32_t namelen;
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

void centreMouse() {
  SetCursorPos(128, 100);
}

void JJSReadJoystick() {
  JOYINFO myJoy;
  MMRESULT errVal;

  errVal = joyGetPos(JOYSTICKID1, &myJoy);

  JoyXVal = myJoy.wXpos >> 8;
  JoyYVal = myJoy.wYpos >> 8;

  JoyButton1 = myJoy.wButtons & 0x0001;
  JoyButton2 = myJoy.wButtons & 0x0002;
}

bool JJSJoyConfig() {
  JOYCAPS mycaps;
  UINT joyrange;
  MMRESULT theres;

  if (joyGetDevCaps(JOYSTICKID1, &mycaps, sizeof(mycaps)) != JOYERR_NOERROR)
    return false;

  joyrange = mycaps.wXmax - mycaps.wXmin;
  joyModXVal = 256.0f / (float)joyrange;

  joySetThreshold(JOYSTICKID1, joyrange / 512);

  joyrange = mycaps.wYmax - mycaps.wYmin;
  joyModYVal = 256.0f / (float)joyrange;

  theres = joySetCapture(hwnd, JOYSTICKID1, 0, TRUE);

  if (theres != JOYERR_NOERROR)
    switch (theres) {
    case MMSYSERR_NODRIVER:
      theres = 2;
      break;
    case JOYERR_NOCANDO:
      theres = 2;
      break;
    case JOYERR_UNPLUGGED:
      theres = 2;
      return false;
    }
  return true;
}

void ShowErrorMessage(int32_t StrNum) {
  MSG msg;
  char Msg[80];

  ShowCursor(TRUE);
  finiObjects();

  LoadString(hinst, StrNum, Msg, 80);
#if 0
	SystemParametersInfo(SPI_SETKEYBOARDDELAY,Keydelay,0,0);
	SystemParametersInfo(SPI_SETKEYBOARDSPEED,Keyrepeat,0,0);
 	SystemParametersInfo(SPI_SETSCREENSAVEACTIVE,sSaver,0,0);
#endif
  while (PeekMessage(&msg, hwnd, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
    ; // Clear all outstanding messages
  MessageBox(hwnd, Msg, "Wipeout Error", MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND);
  DestroyWindow(hwnd);
  QuitToDos();
}

void ShowWarningMessage(UINT StrNum) {
  MSG msg;
  char Msg[80];

  ShowCursor(TRUE);
  LoadString(hinst, StrNum, Msg, 80);
  while (PeekMessage(&msg, hwnd, WM_KEYFIRST, WM_KEYLAST, PM_REMOVE))
    ; // Clear all outstanding messages
  MessageBox(hwnd, Msg, "Wipeout Warning",
             MB_ICONEXCLAMATION | MB_OK | MB_SETFOREGROUND);
  ShowCursor(FALSE);
}

TimData* JJSLoad16BitTexture(const char* filename, char location) {
  TimData* timSize;

  DDSURFACEDESC2 ddsd;
  DWORD bufsize;
  int32_t texsize;
  char* bmpdata;
  HRESULT ddrval;
  LPDIRECTDRAWSURFACE4 tbuf;
  RECT dest_rect, src_rect;
  DWORD dwFlags;
  DDBLTFX DDBltFX;

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
  ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
  ddsd.dwWidth = 320;
  ddsd.dwHeight = 240;
  ddrval = g_pDD4->CreateSurface(&ddsd, &tbuf, NULL);
  if (ddrval != DD_OK) {
    ShowErrorMessage(4002);
    return FALSE;
  }

  texsize = 320 * 240 * sizeof(WORD);

  bufsize = texsize;

  timSize = (TimData*)DAlloc((DynamicHeap*)heap, sizeof(TimData));

  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);
  tbuf->Lock(NULL, &ddsd, DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL);
  bmpdata = (char*)ddsd.lpSurface;
  JJSReadTIM(filename, bmpdata, timSize);
  tbuf->Unlock(NULL);

  DDBltFX.dwSize = sizeof(DDBltFX);

  dwFlags = DDBLT_WAIT;
  lpBack_Buffer->Blt(&dest_rect, tbuf, &src_rect, dwFlags, &DDBltFX);

  tbuf->Release();

  return timSize;
}
