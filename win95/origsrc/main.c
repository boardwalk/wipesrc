#include <libspu.h>
#include <kernel.h>
#include "standard.h"

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
#include "hud.h"
#include "iso9660.h"
#include "sparks2.h"
#include "main.h"
#include "global.h"
#include "vramsize.h" // PCwipeout

#include "dos.h"


// #include <windows.h>

#include	"markk\libetc.h"	// PCwipeout

extern char Resolution;
extern short screenres;
extern short trackNum; //SJR
extern int menuTexturesIn;
extern u_long   _stacksize;     // PCwipeout
ulong	   FastRam;	// PCwipeout

short	   drawenv_tpage = 5;      // PCwipeout
short	   BG_Colour = 0;  // PCwipeout

extern TimData *AJSLoad16BitTexture(char *filename, char location);
extern TimData *JJSLoad16BitTexture(char *filename, char location);
// extern u_long __bss, __bsslen, _ramsize, _stacksize;

#define	GsGetVcount()	0
extern char     thieving_git;

/*
 * for 2 meg mode we need this in the obj file:: 2mbyte.obj
 */
#define	      TwoMegMode	1

extern char    *VRam;	   // PCwipeout
char	    copyRight[24] = "copyright_Psygnosis_1995";
int	     security = 0x2823217b;

SPRT	    sprtPrim[2][5];
DR_MODE	 drawmode[5];

char	    inmenu = 0, Reallyinmenu = 0;
char	   *background_buffer;
char	    trackdir[21];

extern long     this_frame_count, last_frame_count, ok_to_interrupt;

extern char     andybodge;

extern char    *CurBuffer;
long	    DrawDistance;
int	     inattract = 0;
	TimData	*titleScreen;

void reloadStartScr(void)
{
	int i;
	LoadVRam("wipeout\\newgraph\\menu\\font.pcx", 0);
#if USE_ATI
	titleScreen = JJSLoad16BitTexture("wipeout\\textures\\wiptitle.tim", VRAM);
#else
	titleScreen = AJSLoad16BitTexture("wipeout\\textures\\wiptitle.pcx", VRAM);
#endif
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

void	    oldmain(void)
{
	int	     i;
	combatData      packetData;
	ConfigData      gameData;
	struct EXEC     execData;
	char	    filename[20] = {"\\NTSCANIM.EXE;1"};
	// TimData				*titleScreen ;
	unsigned long  *ptr;
	int	     mode, timeout;
	int	     toggle = 0;
	TimData	*legalScreen;
	POLY_FT4	titlePrim[2];
	POLY_FT4	title1Prim[2];
	char	    loadPath[48];
	char	    loadNo[2];
	int	     titleTimeout;
	int	     TOut;
	static int      firstTime = 1;
	char	   *nl = (char *) 0x0417;
	char	    lfilename[80];

	/*****************************************************/
	/*******  START OF SYSTEM CONFIGURATION CHECK  *******/
	/*****************************************************/

	// syscheck();

	/*****************************************************/
	/********  END OF SYSTEM CONFIGURATION CHECK  ********/
	/*****************************************************/





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

	FastRam = (ulong) make_data_cache();    // PCwipeout
	etc_InputDeviceSelect = _KEYBOARD;      // PCwipeout
	Set_Device(0);	  // PCwipeout
	init3d();
	CdInit();	       // PCWipeout
	CheckError();


	ResetCallback();

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

#if CD_FS
	anim(filename);
#endif

	ShellGetPrefs();
	StartPAD();
	LoadVRam("wipeout\\newgraph\\menu\\font.pcx", 0);

	InitGameDisplay();
#if USE_ATI
	titleScreen = JJSLoad16BitTexture("wipeout\\textures\\wiptitle.tim", SCREEN);
#else
	titleScreen = AJSLoad16BitTexture("wipeout\\textures\\wiptitle.pcx", SCREEN);
#endif
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

#ifdef PC_DEMO
	// while(1)
	// {
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
		titleScreen = Load16BitTexture("wipeout\\textures\\paltitle.tim", VRAM);
#else
		LoadVRam("wipeout\\newgraph\\menu\\font.pcx", 0);
#if USE_ATI
		titleScreen = JJSLoad16BitTexture("wipeout\\textures\\wiptitle.tim", VRAM);
#else
		titleScreen = AJSLoad16BitTexture("wipeout\\textures\\wiptitle.pcx", VRAM);
#endif
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
						UpdateNotes();
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

		// THIS IS THE PLACE TO BE
		   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

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
		strcpy(loadPath, "wipeout\\textures\\load");

		if (gameData.actTrackNum <= 9)
		{
			sprintf(loadNo, "%d", gameData.actTrackNum);
		}
		else
		{
			sprintf(loadNo, "%2d", gameData.actTrackNum);
		}
		strcat(loadPath, loadNo);
#if USE_ATI
		strcat(loadPath, ".tim");
#else
		strcat(loadPath, ".pcx");
#endif
		ClearOTagR(OT[CurrentScreen], OT_SIZE);
#if USE_ATI
		JJSLoad16BitTexture(loadPath, SCREEN);
#else
		AJSLoad16BitTexture(loadPath, SCREEN);
#endif
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
			LoadVRam("wipeout\\newgraph\\venom\\altima\\altima.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\venom\\altima\\altima.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\venom\\altima\\altima.sjr", 0);
#endif
			strcpy(trackdir, "venom\\altima\\");
			break;
		case 2:
			LoadVRam("wipeout\\newgraph\\venom\\karbonis\\karbonis.pcx", 0);
#if USE_ATI
//			JJSNewTexture("wipeout\\newgraph\\venom\\karbonis\\karbonis.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\venom\\karbonis\\karbonis.sjr", 0);
#endif
			strcpy(trackdir, "venom\\karbonis\\");
			break;
		case 3:
			LoadVRam("wipeout\\newgraph\\venom\\terramax\\terramax.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\venom\\terramax\\terramax.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\venom\\terramax\\terramax.sjr", 0);
#endif
			strcpy(trackdir, "venom\\terramax\\");
			break;
		case 4:
			LoadVRam("wipeout\\newgraph\\venom\\korodera\\korodera.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\venom\\korodera\\korodera.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\venom\\korodera\\korodera.sjr", 0);
#endif
			strcpy(trackdir, "venom\\korodera\\");
			break;
		case 5:
			LoadVRam("wipeout\\newgraph\\venom\\arridos\\arridos.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\venom\\arridos\\arridos.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\venom\\arridos\\arridos.sjr", 0);
#endif
			strcpy(trackdir, "venom\\arridos\\");
			break;
		case 6:
			LoadVRam("wipeout\\newgraph\\venom\\silver\\silver.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\venom\\silver\\silver.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\venom\\silver\\silver.sjr", 0);
#endif
			strcpy(trackdir, "venom\\silver\\");
			break;
		case 7:
			LoadVRam("wipeout\\newgraph\\venom\\firestar\\firestar.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\venom\\firestar\\firestar.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\venom\\firestar\\firestar.sjr", 0);
#endif
			strcpy(trackdir, "venom\\firestar\\");
			break;
		case 8:
			LoadVRam("wipeout\\newgraph\\rapier\\altima\\altima.pcx", 0);
#if USE_ATI
			//JJSNewtexture("wipeout\\newgraph\\rapier\\altima\\altima.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\rapier\\altima\\altima.sjr", 0);
#endif
			strcpy(trackdir, "rapier\\altima\\");
			break;
		case 9:
			LoadVRam("wipeout\\newgraph\\rapier\\karbonis\\karbonis.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\rapier\\karbonis\\karbonis.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\rapier\\karbonis\\karbonis.sjr", 0);
#endif
			strcpy(trackdir, "rapier\\karbonis\\");
			break;
		case 10:
			LoadVRam("wipeout\\newgraph\\rapier\\terramax\\terramax.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\rapier\\terramax\\terramax.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\rapier\\terramax\\terramax.sjr", 0);
#endif
			strcpy(trackdir, "rapier\\terramax\\");
			break;
		case 11:
			LoadVRam("wipeout\\newgraph\\rapier\\korodera\\korodera.pcx", 0);
#if USE_ATI
			//JJSNewtexture("wipeout\\newgraph\\rapier\\korodera\\korodera.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\rapier\\korodera\\korodera.sjr", 0);
#endif
			strcpy(trackdir, "rapier\\korodera\\");
			break;
		case 12:
			LoadVRam("wipeout\\newgraph\\rapier\\arridos\\arridos.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\rapier\\arridos\\arridos.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\rapier\\arridos\\arridos.sjr", 0);
#endif
			strcpy(trackdir, "rapier\\arridos\\");
			break;
		case 13:
			LoadVRam("wipeout\\newgraph\\rapier\\silver\\silver.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\rapier\\silver\\silver.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\rapier\\silver\\silver.sjr", 0);
#endif
			strcpy(trackdir, "rapier\\silver\\");
			break;
		case 14:
			LoadVRam("wipeout\\newgraph\\rapier\\firestar\\firestar.pcx", 0);
#if USE_ATI
			//JJSNewTexture("wipeout\\newgraph\\rapier\\firestar\\firestar.bmp", 0);
			SJRNewTexture("wipeout\\newgraph\\rapier\\firestar\\firestar.sjr", 0);
#endif
			strcpy(trackdir, "rapier\\firestar\\");
			break;
		default:
			printf("what track ?\n");
			exit(0);
			break;
		}

		/* InitializeGame (&packetData, &gameData) ; */

#if 0
		if (gameType == MASTER || gameType == SLAVE)
		{

			if (gameType == MASTER)
				packetData.combatControl = NewMaster;

			if (gameType == SLAVE)
				packetData.combatControl = NewSlave;
		}

#endif
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
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(0, 256, 512, 256, 0, 0, 0);
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(512, 0, 512, 256, 0, 0, 0);
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);

	clearVram = BlockFill(512, 256, 512, 256, 0, 0, 0);
	DrawPrim(clearVram);
	DrawSync(0);
	DFree(heap, (char *) clearVram);
#endif
}

#ifndef USE_ATI

void	    UserInt(void)   /* not really used Steve's dodgey timer fn */
{
	// PCwipeout
}
#endif //USE_ATI

void	    UserAYInt(void)
{
	this_frame_count++;
	ProcessMessages();
	// if (ok_to_interrupt)
	// racectrl();
}

#ifndef USE_ATI

void	    AJSShowMenuBackground(TimData * tim)
{
	char	    x_offset;

	x_offset = tim->textureX % 64;


	SetSprt(&(sprtPrim[CurrentScreen][0]));
	setXY0(&(sprtPrim[CurrentScreen][0]), 0, 0);
	setWH(&(sprtPrim[CurrentScreen][0]), 64, 200);
	setUV0(&(sprtPrim[CurrentScreen][0]), x_offset, 0);
	sprtPrim[CurrentScreen][0].r0 = 0;
	sprtPrim[CurrentScreen][0].g0 = 0;
	sprtPrim[CurrentScreen][0].b0 = 0;
	AddPrim(OT[CurrentScreen], (SPRT *) & (sprtPrim[CurrentScreen][0]));

	SetDrawMode((DR_MODE *) & drawmode[0], 0, 0, GetTPage(0, 0, tim->textureX, tim->textureY), 0);
	AddPrim(OT[CurrentScreen], (DR_MODE *) & drawmode[0]);


	SetSprt(&(sprtPrim[CurrentScreen][1]));
	setXY0(&(sprtPrim[CurrentScreen][1]), 64, 0);
	setWH(&(sprtPrim[CurrentScreen][1]), 64, 200);
	setUV0(&(sprtPrim[CurrentScreen][1]), x_offset, 0);
	sprtPrim[CurrentScreen][1].r0 = 0;
	sprtPrim[CurrentScreen][1].g0 = 0;
	sprtPrim[CurrentScreen][1].b0 = 0;
	AddPrim(OT[CurrentScreen], (SPRT *) & (sprtPrim[CurrentScreen][1]));

	SetDrawMode((DR_MODE *) & drawmode[1], 0, 0, GetTPage(0, 0, tim->textureX, tim->textureY) + 1, 0);
	AddPrim(OT[CurrentScreen], (DR_MODE *) & drawmode[1]);


	SetSprt(&(sprtPrim[CurrentScreen][2]));
	setXY0(&(sprtPrim[CurrentScreen][2]), 128, 0);
	setWH(&(sprtPrim[CurrentScreen][2]), 64, 200);
	setUV0(&(sprtPrim[CurrentScreen][2]), x_offset, 0);
	sprtPrim[CurrentScreen][2].r0 = 0;
	sprtPrim[CurrentScreen][2].g0 = 0;
	sprtPrim[CurrentScreen][2].b0 = 0;
	AddPrim(OT[CurrentScreen], (SPRT *) & (sprtPrim[CurrentScreen][2]));

	SetDrawMode((DR_MODE *) & drawmode[2], 0, 0, GetTPage(0, 0, tim->textureX, tim->textureY) + 2, 0);
	AddPrim(OT[CurrentScreen], (DR_MODE *) & drawmode[2]);


	SetSprt(&(sprtPrim[CurrentScreen][3]));
	setXY0(&(sprtPrim[CurrentScreen][3]), 192, 0);
	setWH(&(sprtPrim[CurrentScreen][3]), 64, 200);
	setUV0(&(sprtPrim[CurrentScreen][3]), x_offset, 0);
	sprtPrim[CurrentScreen][3].r0 = 0;
	sprtPrim[CurrentScreen][3].g0 = 0;
	sprtPrim[CurrentScreen][3].b0 = 0;
	AddPrim(OT[CurrentScreen], (SPRT *) & (sprtPrim[CurrentScreen][3]));

	SetDrawMode((DR_MODE *) & drawmode[3], 0, 0, GetTPage(0, 0, tim->textureX, tim->textureY) + 3, 0);
	AddPrim(OT[CurrentScreen], (DR_MODE *) & drawmode[3]);


	SetSprt(&(sprtPrim[CurrentScreen][4]));
	setXY0(&(sprtPrim[CurrentScreen][4]), 256, 0);
	setWH(&(sprtPrim[CurrentScreen][4]), 64, 200);
	setUV0(&(sprtPrim[CurrentScreen][4]), x_offset, 0);
	sprtPrim[CurrentScreen][4].r0 = 0;
	sprtPrim[CurrentScreen][4].g0 = 0;
	sprtPrim[CurrentScreen][4].b0 = 0;
	AddPrim(OT[CurrentScreen], (SPRT *) & (sprtPrim[CurrentScreen][4]));

	SetDrawMode((DR_MODE *) & drawmode[4], 0, 0, GetTPage(0, 0, tim->textureX, tim->textureY) + 4, 0);
	AddPrim(OT[CurrentScreen], (DR_MODE *) & drawmode[4]);
}

#endif //USE_ATI

void	    CheckError()
{
	if (thieving_git)
	{
		ResetVMode();
		printf("\n");
		printf("     *********************************************************************\n");
		printf("     *			  PIRACY IS THEFT			  *\n");
		printf("     *								   *\n");
		printf("     * Please insert original wipEout(PC) CD-ROM in CD drive and restart *\n");
		printf("     *								   *\n");
		printf("     *	if you experience any problems contact Psygnosis on	*\n");
		printf("     *		       (+44) (0)151 282-3000		       *\n");
		printf("     *********************************************************************\n");
		printf("\n");
		exit(1);
	}
}


void	    QuitToDos(void)
{
#ifdef DOS_VERSION
	ResetVMode();
#else
	WinQuit();
#endif
	RemoveTimer();
	SfxUnInit(1);
	CdControlB(CdlStop, 0, 0);
	exit(0);
}
