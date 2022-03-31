//#include <libspu.h>
#include "wipesrc/psx26/include/kernel.h"
#include "standard.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ttf.h"
#include "ships.h"
#include "weapon.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "main.h"
#include "fileops.h"
#include "sound.h"
#include "control.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "iso9660.h"
#include "sparks2.h"
#include "main.h"
#include "global.h"
#include "vramsize.h" // PCwipeout
#include "ajs.h"
#include "drawot.h"
#include "decl.h"
#include "win.h"
#include "libgte.h"
#include "sfx.h"
#include "ayrace.h"
#include "datacash.h"
#include "timc.h"
#include "regread.h"
#include "card.h"

// #include "dos.h"


// #include <windows.h>

#include	"wipesrc/win95/markk/libetc.h"	// PCwipeout
void	    CheckError();

extern char Resolution;
extern int16_t screenres;
extern int16_t trackNum; //SJR
extern int32_t menuTexturesIn;
extern uint32_t   _stacksize;     // PCwipeout
uintptr_t	   FastRam;	// PCwipeout

int16_t	   drawenv_tpage = 5;      // PCwipeout
int16_t	   BG_Colour = 0;  // PCwipeout

extern TimData *AJSLoad16BitTexture(const char *filename, char location);
extern TimData *JJSLoad16BitTexture(const char *filename, char location);
// extern uint32_t __bss, __bsslen, _ramsize, _stacksize;

#define	GsGetVcount()	0
extern char     thieving_git;

/*
 * for 2 meg mode we need this in the obj file:: 2mbyte.obj
 */
#define	      TwoMegMode	1

extern char    *VRam;	   // PCwipeout
char	    copyRight[/*24*/] = "copyright_Psygnosis_1995";
int32_t	     security = 0x2823217b;

SPRT	    sprtPrim[2][5];
DR_MODE	 drawmode[5];

char	    inmenu = 0, Reallyinmenu = 0;
char	   *background_buffer;
char	    trackdir[21];

extern int32_t     this_frame_count, last_frame_count, ok_to_interrupt;

extern char     andybodge;

extern char    *CurBuffer;
int32_t	    DrawDistance;
int32_t	     inattract = 0;
	TimData	*titleScreen;

void reloadStartScr(void)
{
	int32_t i;
	LoadVRam("wipeout/newgraph/menu/font.pcx", 0);
	titleScreen = JJSLoad16BitTexture("wipeout/textures/wiptitle.tim", VRAM);
	inmenu = 1;
	CopyPal(shield_palette, 0, 255);
	for (i = 0; i < 2; i++)
	{
		ClearOTagR(OT[CurrentScreen], OT_SIZE);
		Swap();
	}
	inmenu = 1;
	for (i = 0; i < 2; i++)
	{
		CentreText("PRESS ENTER", text_data(0, 200, 8), RedText);
		Swap();
	}
	InitAJSVSync();
}

void	    oldmain()
{
	int32_t	     i;
	combatData      packetData;
	ConfigData      gameData;
	char	    filename[20] = {"\\NTSCANIM.EXE;1"};
	// TimData				*titleScreen ;
	int32_t	     mode, timeout;
	int32_t	     toggle = 0;
	char	    loadPath[48];
	char	    loadNo[3];
	int32_t	     titleTimeout;
	int32_t	     TOut;
	static int32_t      firstTime = 1;
	char	   *nl = (char *) 0x0417;
	char	    lfilename[80];


	heap = (DynamicHeap *) malloc(HEAP_SIZE * 1024);
	if (heap == NULL)
	{
		printf("Not Enough Memory Available (1)\n");
		exit(-1);
	}
	InitDynamicMem(heap, HEAP_SIZE * 1024);

#if 0
	background_buffer = (char *) malloc(320 * 200);
	if (background_buffer == NULL)
	{
		printf("Not Enough Memory Available (3)\n");
		exit(0);
	}
#endif

	CreateSineTable();

	FastRam = (uintptr_t) make_data_cache();    // PCwipeout
	etc_InputDeviceSelect = _KEYBOARD;      // PCwipeout
	Set_Device(0);	  // PCwipeout
	CdInit();	       // PCWipeout
	CheckError();

	InitSystem();
	// GsInitVcount( );			// PCwipeout
	ClearVram();
	InitController();

	InitialiseAmbientColour();
	InitialiseColourMatrix();
	InitialiseLightMatrix();

#ifdef DOS_VERSION
	*nl &= 0xdf;	    // Numlock off
#endif

	InstallTimer();

	InitSonyDisplay();

	// for( i = 0; i < 60; i++)	  // PCwipeout
	// {
	// VSync(0);
	// }

	ShellGetPrefs();
	StartPAD();
	LoadVRam("wipeout/newgraph/menu/font.pcx", 0);

	InitGameDisplay();
	titleScreen = JJSLoad16BitTexture("wipeout/textures/wiptitle.tim", SCREEN);
	inmenu = 1;
	CopyPal(shield_palette, 0, 255);
	for (i = 0; i < 2; i++)
	{
		ClearOTagR(OT[CurrentScreen], OT_SIZE);
		Swap();
	}
	inmenu = 1;
	for (i = 0; i < 2; i++)
		Swap();

	InitSpuSound();


	SetCharTextures(8);
	SetCharTextures(12);
	SetCharTextures(16);
	HighScore(0);

	TextureTableCount = 0;
	InitText();

#if 0		// PCwipeout

	StartCARD();
	_bu_init();

	InitCardEvents();
	ChangeClearSIO(1);
	ChangeClearPAD(0);
#endif

	srand(GsGetVcount());

	InitGameData(&gameData);

	MostRecentSaveFile(lfilename);

	if (lfilename[0] != 0)
		LoadCardData(lfilename, 0, &gameData, (SelectionData *) 0);


	SfxSetVolume((char) ((0x3fff - (75 * gameData.currSliderPos)) >> 6));

	nCon.twistFactor = 255;
//	_init_cont();

	InitPadSettings();
	InitChampionship();


#if PAL
	timeout = 25 * 120;
#else
	timeout = FR30 * 120;
#endif

#if CD_AUDIO
	//if (gameData.cdTrack == 1)
	//{
		//InitCD(loc, 0);
	//}
	//else
		//InitCD(loc, 0);
#endif

	mode = ATTRACT;

	SetCharTextures(8);
	SetCharTextures(12);
	SetCharTextures(16);

	titleTimeout = 0;

	trackNum = 16; //SJR
	if ((gameData.gameType != CHAMPIONSHIP) || (mode == ATTRACT))
	{
#if PAL
		titleScreen = Load16BitTexture("wipeout/textures/paltitle.tim", VRAM);
#else
		LoadVRam("wipeout/newgraph/menu/font.pcx", 0);
		titleScreen = JJSLoad16BitTexture("wipeout/textures/wiptitle.tim", VRAM);
		inmenu = 1;
		CopyPal(shield_palette, 0, 255);
		for (i = 0; i < 2; i++)
		{
			ClearOTagR(OT[CurrentScreen], OT_SIZE);
			Swap();
		}
		inmenu = 1;
		for (i = 0; i < 2; i++)
		{
			CentreText("PRESS ENTER", text_data(0, 200, 8), RedText);
			Swap();
		}

#endif


		InitAJSVSync();
		toggle = 0;

		while (1)
		{
			opad = pad;
			pad = neg1.bits;
			if ((neg1.head != 0xff))
				pad = ~pad;
			xpad = (pad ^ opad) & pad;

			// ClearOTagR( OT[ CurrentScreen ], OT_SIZE );
			textPrim = CurrentScreen * 300;

			titleTimeout++;

			if (firstTime == 1)
				TOut = 30;
			else
				TOut = 5;

#ifndef PC_DEMO
			if (titleTimeout >= TOut * FR30)
			{
				mode = ATTRACT;
				gameType = SINGLE;
				ownShip = randy() % 8;
				serialShip = -1;
				firstTime = 0;
				break;
			}
#endif

			if (titleTimeout > 1)
			{
				if (GetMenuKey(M_SELECT))
				{
					PlayNote(MENU_SELECT, 1, 20);
					for (i = 0; i < 10; i++)
					{
						UpdateNotes(NOT_PAUSED);
					}
					mode = INTERACTIVE;
					firstTime = 1;
					break;
				}

#if PC_DEMO
				if (GetMenuKey(M_BACK))
				{
					PlayNote(MENU_SELECT, 1, 20);
					ResetVMode();
					printf("Thanks for playing the wipEout (PC-CD) demo, (c) Psygnosis, 1995\n");
					printf("\nFull game available mid November 1995\n");
					exit(0);
				}
#endif

			}


			Swap();

			// AJSVSync(2);


			toggle++;
		}
	}

	for (i = 0; i < 2; i++)
	{
		ClearOTagR(OT[CurrentScreen], OT_SIZE);
		Swap();
	}

	if(Resolution!=screenres)
	{
		screenres=Resolution;
		changeScreen();
	}
	gameType = SINGLE;

	gameData.Lives = 3;
	gameData.EndOfGame = 0;
	gameData.continueConfirm = 1;
	gameData.Position = 8;

	/* Enable cheat permanently */

	 //gameData.RapierClass = 1 ;
	 //totalTracks = 14 ;
	 //NumTracks = 7 ;
	 //gameData.showBonusTrack = 1 ; //SJR

	/****************************/

#ifndef PC_DEMO
	while (1)
	{
#endif
		for (i = 0; i < 2; i++)
		{
			ClearOTagR(OT[CurrentScreen], OT_SIZE);
			Swap();
		}
#ifndef PC_DEMO

		if ((gameData.gameType != CHAMPIONSHIP) && (mode != ATTRACT))
		{
			SetDrawDistance(HIGH_DISTANCE);
			MenuControl(&(gameData), titleScreen, &mode, timeout);
		}

		if (andybodge == 2)
		{
			mode = ATTRACT;
			gameType = SINGLE;
			ownShip = randy() % 8;
			serialShip = -1;
			firstTime = 0;
			andybodge = 1;
		}

#endif

#if EditorOn
		ClearVram();
#endif

#if CD_FS
		close_iso_events();
		iso_init(heap);
#endif

		SetDrawDistance(DrawDistance);

		if (mode == ATTRACT)
		{
			switch (randy() % 6)
				// switch(1)
			{
			case 0:
				trackNo = 2;    // Altima
				gameData.actTrackNum = 2;
				gameData.trackNum = 1;
				break;

			case 1:
				trackNo = 4;    // karbonis
				gameData.actTrackNum = 4;
				gameData.trackNum = 2;
				break;

			case 2:
				trackNo = 1;    // terramax
				gameData.actTrackNum = 1;
				gameData.trackNum = 3;
				break;

			case 3:
				trackNo = 12;    // korodera
				gameData.actTrackNum = 12;
				gameData.trackNum = 4;
				break;

			case 4:
				trackNo = 8;    // arridos
				gameData.actTrackNum = 8;
				gameData.trackNum = 5;
				break;

			case 5:
				trackNo = 9;    // silver
				gameData.actTrackNum = 9;
				gameData.trackNum = 6;
				break;
			}
		}
		else
		{
#ifdef PC_DEMO
			trackNo = 1;
			gameData.actTrackNum = 2;
			gameData.trackNum = 1;
#else
			trackNo = gameData.actTrackNum;
#endif
		}
		CdControlB(CdlStop, 0, 0);


		/* load screen stuff */
		strcpy(loadPath, "wipeout/textures/load");

		if (gameData.actTrackNum <= 9)
		{
			sprintf(loadNo, "%d", gameData.actTrackNum);
		}
		else
		{
			sprintf(loadNo, "%2d", gameData.actTrackNum);
		}
		strcat(loadPath, loadNo);
		strcat(loadPath, ".tim");
		ClearOTagR(OT[CurrentScreen], OT_SIZE);
		JJSLoad16BitTexture(loadPath, SCREEN);
		inmenu = 0;
		CopyPal(shield_palette, 0, 255);	// PCwipeout
		for (i = 0; i < 2; i++)
		{
			ClearOTagR(OT[CurrentScreen], OT_SIZE);
			Swap();
		}
		inmenu = 1;
		for (i = 0; i < 2; i++)
			Swap();

		/*
		 * VsyncStart( loadPath );
		 */

		menuTexturesIn = 0;
		trackNum = gameData.trackNum; //for restoring - SJR
		switch (gameData.trackNum)
		{
		case 1:
			LoadVRam("wipeout/newgraph/venom/altima/altima.pcx", 0);
			SJRNewTexture("wipeout/newgraph/venom/altima/altima.sjr");
			strcpy(trackdir, "venom/altima/");
			break;
		case 2:
			LoadVRam("wipeout/newgraph/venom/karbonis/karbonis.pcx", 0);
			SJRNewTexture("wipeout/newgraph/venom/karbonis/karbonis.sjr");
			strcpy(trackdir, "venom/karbonis/");
			break;
		case 3:
			LoadVRam("wipeout/newgraph/venom/terramax/terramax.pcx", 0);
			SJRNewTexture("wipeout/newgraph/venom/terramax/terramax.sjr");
			strcpy(trackdir, "venom/terramax/");
			break;
		case 4:
			LoadVRam("wipeout/newgraph/venom/korodera/korodera.pcx", 0);
			SJRNewTexture("wipeout/newgraph/venom/korodera/korodera.sjr");
			strcpy(trackdir, "venom/korodera/");
			break;
		case 5:
			LoadVRam("wipeout/newgraph/venom/arridos/arridos.pcx", 0);
			SJRNewTexture("wipeout/newgraph/venom/arridos/arridos.sjr");
			strcpy(trackdir, "venom/arridos/");
			break;
		case 6:
			LoadVRam("wipeout/newgraph/venom/silver/silver.pcx", 0);
			SJRNewTexture("wipeout/newgraph/venom/silver/silver.sjr");
			strcpy(trackdir, "venom/silver/");
			break;
		case 7:
			LoadVRam("wipeout/newgraph/venom/firestar/firestar.pcx", 0);
			SJRNewTexture("wipeout/newgraph/venom/firestar/firestar.sjr");
			strcpy(trackdir, "venom/firestar/");
			break;
		case 8:
			LoadVRam("wipeout/newgraph/rapier/altima/altima.pcx", 0);
			SJRNewTexture("wipeout/newgraph/rapier/altima/altima.sjr");
			strcpy(trackdir, "rapier/altima/");
			break;
		case 9:
			LoadVRam("wipeout/newgraph/rapier/karbonis/karbonis.pcx", 0);
			SJRNewTexture("wipeout/newgraph/rapier/karbonis/karbonis.sjr");
			strcpy(trackdir, "rapier/karbonis/");
			break;
		case 10:
			LoadVRam("wipeout/newgraph/rapier/terramax/terramax.pcx", 0);
			SJRNewTexture("wipeout/newgraph/rapier/terramax/terramax.sjr");
			strcpy(trackdir, "rapier/terramax/");
			break;
		case 11:
			LoadVRam("wipeout/newgraph/rapier/korodera/korodera.pcx", 0);
			SJRNewTexture("wipeout/newgraph/rapier/korodera/korodera.sjr");
			strcpy(trackdir, "rapier/korodera/");
			break;
		case 12:
			LoadVRam("wipeout/newgraph/rapier/arridos/arridos.pcx", 0);
			SJRNewTexture("wipeout/newgraph/rapier/arridos/arridos.sjr");
			strcpy(trackdir, "rapier/arridos/");
			break;
		case 13:
			LoadVRam("wipeout/newgraph/rapier/silver/silver.pcx", 0);
			SJRNewTexture("wipeout/newgraph/rapier/silver/silver.sjr");
			strcpy(trackdir, "rapier/silver/");
			break;
		case 14:
			LoadVRam("wipeout/newgraph/rapier/firestar/firestar.pcx", 0);
			SJRNewTexture("wipeout/newgraph/rapier/firestar/firestar.sjr");
			strcpy(trackdir, "rapier/firestar/");
			break;
		default:
			printf("what track ?\n");
			exit(0);
			break;
		}

		/* InitializeGame (&packetData, &gameData) ; */

		textureX = 320;
		textureY = 0;

		clutX = 320;
		clutY = 256;

		TextureTableCount = 0;

		attractDelay = 0;

		tempLapTimes[0] = 0;
		tempLapTimes[1] = 0;
		tempLapTimes[2] = 0;
		tempLapTimes[3] = 0;
		tempLapTimes[4] = 0;


		gameData.newLapRecord = 0;


#ifdef	PC_DEMO
		gameType = SINGLE;
		gameData.shipNum = 7;
		gameData.gameType = SINGLE_RACE;
		ownShip = gameData.shipNum;
		serialShip = -1;
#endif

		inmenu = 0;
		Reallyinmenu = 0;


		// etc_InputDeviceSelect = _MOUSE1;	// DLS
		// StartPAD();

		race(&packetData, &gameData, mode, timeout);
		// inmenu=1;
		inattract = 0;
		mode = INTERACTIVE;

		InitDynamicMem(heap, HEAP_SIZE * 1024); /* The left over bit in
							 * the middle */
#if CD_FS
		close_iso_events();
		iso_init(heap);
#endif
		TextureTableCount = 0;
		InitText();
	}
}


void	    ClearVram(void)
{
	BlkFill	*clearVram;

#if EditorOn

	clearVram = BlockFill(0, 0, 512, 256, 0xff, 0xff, 0xff);
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(0, 256, 512, 256, 0xff, 0xff, 0xff);
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(512, 0, 512, 256, 0xff, 0xff, 0xff);
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(512, 256, 512, 256, 0xff, 0xff, 0xff);
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);
#else

	clearVram = BlockFill(0, 0, 512, 256, 0, 0, 0);
	DrawPrim((uint32_t *)clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(0, 256, 512, 256, 0, 0, 0);
	DrawPrim((uint32_t *)clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(512, 0, 512, 256, 0, 0, 0);
	DrawPrim((uint32_t *)clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(512, 256, 512, 256, 0, 0, 0);
	DrawPrim((uint32_t *)clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);
#endif
}


void	    UserAYInt(void)
{
	this_frame_count++;
	ProcessMessages();
	// if (ok_to_interrupt)
	// racectrl();
}

void	    CheckError()
{
}


void	    QuitToDos(void)
{
	WinQuit();
	RemoveTimer();
	SfxUnInit(1);
	CdControlB(CdlStop, 0, 0);
	exit(0);
}
