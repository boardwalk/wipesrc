//#include <libspu.h>
#include "standard.h"
#include <stdlib.h>

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "tilelib.h"
#include "ttf.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "main.h"
#include "dynam.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "sound.h"
#include "editor.h"
#include "texplace.h"
#include "trakview.h"
#include "effects.h"
#include "fileops.h"
#include "sparks2.h"
#include "options.h"
#include "global.h"
#include "sprite.h"
#include "cone.h"
#include "race.h"
#include "trackasm.h"
#include "progress.h"
#include "spline.h"
#include "vramsize.h" // PCwipeout
#include	"pallette.h"
#include "fri.h"
#include "credits.h"
#include "init.h"
#include "win.h"
#include "ajs.h"
#include "collide.h"

#define	_SIZE_T_DEFINED_
//#include "\wc10\h\stdlib.h"

#include "input.h"

#define START_EXTRO 0
#define UPDATE_EXTRO 1
#define DONE_EXTRO 2

#define PrmMax       64
#define	AJSDELAY		 60*30 // arbitrary number - linked to frame rate ? HOPE SO !
//#define	AJSDELAY		 20 // arbitrary number - linked to frame rate ? HOPE SO !

#define SHIP_DEBUG 0

// extern uint32_t __bss, __bsslen, _ramsize, _stacksize;
extern int32_t fr ;
extern ChampionShip	winTable[] ;
extern int16_t starttrack,endtrack,CurrTrkNo;

typedef struct {
	int32_t	width;
	int32_t	height;
	int32_t	vram_xpos;
	int32_t	vram_ypos;
	char	*bitmap;
	char	*palette;
} bm;
void	WritePCX(char *fname, bm *BMptr);			// Thanks Jed (even if it is a bit flakey)
void	DumpVRamAsPCX(char *filename);

extern	char* VRam;					// PCwipeout
extern	int32_t	  inattract;
extern	int16_t   startIceText;
extern	int16_t   startFireText;
#define _SIZE_T_DEFINED_			// PCwipeout
#include <stdio.h>		// PCwipeout
int16_t CLUT_R[256];					// PCwipeout
int16_t CLUT_G[256];					// PCwipeout
int16_t CLUT_B[256];					// PCwipeout
char	BluePal;					// PCwipeout
char	WhitePal;					// PCwipeout
char	GreyPal;					// PCwipeout
char		fred;							// PCwipeout
int32_t	ajscount=0;						// oh shit another global flag

#define	GsGetVcount()	0		 	// PCwipeout


#define RobTest 0







//All race locals now globals, as RaceCtrl has to be re-entrant
TrackData      ctrltrackStart;
BlkFill*       ctrlclear[ 2 ];
TexTemplate*   ctrltexTemplate;
Object*        ctrlprm[ PrmMax ];
Object*        ctrlobj;
int16_t          ctrllibraryTextures;
int16_t          ctrlprmCount;
int32_t           ctrl_i, ctrl_j, ctrl_k;
int16_t          ctrlnumLib;

AutoMeshData   *ctrlautoMesh;
Cone           ctrlcone;
int32_t           ctrlbefore;
int16_t          ctrlcount;
int16_t          ctrlweapCount;
TrackSection   *ctrlnextSection;

#if EditorOn
Editor         ctrleditor;
TexPlace       ctrltexPlace;
Object         *ctrltexHighlight;
int16_t          ctrlface;
int32_t           ctrlbeforeVcount;
#endif



/**************************************************/
/****** 				Dave additions					 *****/
/**************************************************/

TrackCamera		ctrlcamera;
Skeleton*      ctrlcamDir;

Object			*ctrlshipShapes[(NO_OF_SHIPS * 2) + 1];
ShipData			*ctrlshipIndex;
RescueData		ctrlrescueDroid;

WeaponShape    *ctrlweaponShapes;
WeaponData		*ctrlweaponIndex;

WeaponGrid		*ctrlweaponGrid ;

int32_t 				ctrlnoWeaponGrids;
Texture 			*ctrlweaponIconTextures[NO_OF_WEAPON_TYPES] ;

int16_t  			ctrlsky;
int32_t 			ctrlchoice;
POLY_F4			ctrlselectBox[2];
DR_MODE			ctrlselectTrans[2];
TrackSection    *ctrlsection;
Face			*ctrlfacePtr;
VECTOR          ctrlfacePoint;
Effect			*ctrleffects ;
Texture 		*ctrleffectTextures[NO_OF_EFFECT_TEXTURES] ;
int32_t	 			ctrlcdTrack;
int32_t				ctrlnoCdTracks;
POLY_FT4		ctrlTarget[2];
TIMlist  		*ctrltimPtr;
Shadow			*ctrlShadow;
//POLY_FT4		ctrlweaponIcon[2];
SPRT			ctrlweaponIcon[2];
DR_MODE			ctrliconTPage;			// DLS
DR_MODE			ctrlrealTPage;			// DLS
uint8_t			ctrlparam;
Object		   *ctrllostad[20]; /* It wouldn`t suprise me ! */
int32_t				ctrlstadCount;
int16_t			ctrlsectCount;
int16_t			ctrlcheck;
int16_t			ctrljcheck;
int16_t 			ctrlji;
TrackSection   *ctrljsection;
int16_t 			ctrlbuttonPressed;
Object*        	ctrlredLight;
Object*        	ctrldonkey[2];
int32_t				ctrldoneExtro;
int32_t				ctrlplayCredits;
int32_t				ctrlgameOverTime;
int32_t				ctrlcdPause;
int32_t 			ctrlnearShip;


#if SHIP_DEBUG
POLY_F3			ctrlmapShips[8];
LINE_F2			ctrltrackMap[60];
#endif

/**************************************************/
/****** 				Stu additions					 *****/
/**************************************************/
Speedo			ctrlspeedo;
int32_t            	ctrlframe_cnt;
uint32_t  			ctrltoggleDisplay;
char			ctrlshowMenu;
char			ctrlconfirm;
char			ctrllConfirm;
HiScoreData 	ctrlhiScoreData	;
POLY_F4			ctrlfadeScreen[2];
DR_MODE			ctrlfadeTrans[2];
FadeData		ctrlfadeData ;
SPRT			ctrllifeIcons[3][2] ;
//	POLY_FT4			ctrllifeIcons[3][2] ;
char			ctrlrec[8] ;
char			ctrlmachinePaused;
char			ctrlactivated;
int32_t				ctrlchoiceData;
char			ctrlquitFlag;
Texture			*ctrlwinLose [2] ;
POLY_FT4		ctrlwinLosePrims[2] ;
int32_t				ctrlquitTwoPlayer;
POLY_F4			ctrlbackFrame[2];
DR_MODE			ctrlbackFrameTrans[2];
char			ctrlshowGameOver;


combatData		ctrlpacketData;
ConfigData		*ctrlgameData;
int32_t			ctrlmode;
int32_t			ctrltimeout;

/*          Andy Y's additions*/
int32_t			ctrlNeedEndText;
int32_t			ctrlNeedWinLose;
int32_t			ctrlNeedBackPoly;
int32_t			ctrlNeedRaceInfo;
int32_t			ctrlNeedDeSelectScreen;
int32_t			ctrlNeedNewHighScores;
int32_t			ctrlNeedRacePoints;
int32_t			ctrlNeedChampTable;
int32_t			ctrlNeedAllVenomComplete;
int32_t			ctrlNeedVenomComplete;
int32_t			ctrlNeedContinueGameConfirm;
int32_t			ctrlNeedEndTextVar;
int32_t			ctrlNeedWinLoseVar;
int32_t			ctrlNeedInGameReset;
int32_t			ctrlNeedInSingleGameConfirm;
int32_t			ctrlNeedInGameConfirm;
int32_t			ctrlNeedTargetIcon;
int32_t			ctrlCDPaused;
int32_t			ctrlfirsttime;
extern	int32_t	CDRestartNeeded;

void raceinit()
{
	ConfigData		*gameData;
	CVECTOR			colvector;

	gameData = ctrlgameData;
	if (ctrl_init == 0)
	{
		ctrl_says_quit = 0;
		ctrlprmCount = 0;
		ctrlnumLib = 0;
#if EditorOn
		ctrltexHighlight = NULL;
		ctrlface = 0;
#endif
		ctrlCDPaused = 0;
		ctrlnoWeaponGrids = 0;
		ctrlchoice = -1;
		ctrlcdTrack = 1;
		ctrlcheck = 1;
		ctrljcheck = 1;
		ctrlbuttonPressed = 0;
		ctrlstadCount = 0;
		ctrlplayCredits = 0;
		ctrldoneExtro = START_EXTRO;
		ctrlgameOverTime = 0;
		ctrlcdPause = 0;
		ctrlframe_cnt = 1;
		ctrltoggleDisplay = 0;
		ctrlshowMenu = 1;
		ctrlconfirm = 0;
		ctrllConfirm = 0;
		ctrlmachinePaused = SINGLE;
		ctrlactivated = 0;
		ctrlchoiceData = -1;
		ctrlquitFlag = 0;
		ctrlquitTwoPlayer = 0;
		ctrlshowGameOver = 0;
		ctrlfirsttime = 1;
		finishedExtro = 0;


		ctrlautoMesh = ( AutoMeshData* )DAlloc( heap, sizeof(AutoMeshData ) + 32 );

		ctrlautoMesh->triCount = 0;
		ctrlautoMesh->quadCount = 0;
		ctrlautoMesh->colourMeshed = 0;
		ctrlautoMesh->autoMeshedOn = 1;

#if PAL
		ctrlclear[ 0 ] = BlockFill( 0, 0, 320, 256, 0x00, 0x00, 0x00 );
		ctrlclear[ 1 ] = BlockFill( 0, 256, 320, 256, 0x00, 0x00, 0x00 );
#else
		ctrlclear[ 0 ] = BlockFill( 0, 0, 320, 240, 0x00, 0x00, 0x00 );
		ctrlclear[ 1 ] = BlockFill( 0, 240, 320, 240, 0x00, 0x00, 0x00 );
#endif


		inattract = 0;
		InitCredits();

		colvector.b = 255;
		colvector.r = 0;
		colvector.g = 0;

		BluePal = FindNearestColourIndex(&colvector);

		colvector.b = 255;
		colvector.r = 255;
		colvector.g = 255;

		WhitePal = FindNearestColourIndex(&colvector);

		colvector.b = 64;
		colvector.r = 64;
		colvector.g = 64;

		GreyPal = FindNearestColourIndex(&colvector);


		/**********************************************************/
		/*** 						Initialise Camera						  ***/
		/**********************************************************/

		ctrlcamera.camPos = NewSkeleton( );
		SetSkeletonPosition( ctrlcamera.camPos, 0, 0, 0 );
		SetSkeletonDirection( ctrlcamera.camPos, 0, 0, 0 );
		ctrlcamDir = NewSkeleton( );
		SetSkeletonPosition( ctrlcamDir, 0, 0, 0 );
		SetSkeletonDirection( ctrlcamDir, 0, 0, 0 );
		ctrlcamera.camPos->super = ctrlcamDir;

		cameraPtr = &ctrlcamera;

		track = &ctrltrackStart;

		/**********************************************************/
		/*** 						Initialise Track						  ***/
		/**********************************************************/
		//   printf( "Initialise Track\n" );
		getPath(trackPath, gameData->actTrackNum, "/track.trv");
		InitVertices( trackPath );
		getPath(trackPath, gameData->actTrackNum, "/track.trf");
		InitFaces( trackPath );
		getPath(trackPath, gameData->actTrackNum, "/track.trs2");
		InitSections( trackPath );
		getPath(trackPath, gameData->actTrackNum, "/track.vew");
		InitViewList( trackPath );


		extroLevel = 0;

		/* firestar */
		if ( gameData->actTrackNum == 14 )
		{
			//      printf ( "loading firestar extro\n" );
			extroLevel = 14;

			camSpline[ 0 ] = LoadPrm( "wipeout/common/cam1s.prm", 0 );
			camSpline[ 0 ]->skeleton->super = ctrlcamera.camPos;

			shipSpline[ 0 ] = LoadPrm( "wipeout/common/shp1s.prm", 0 );
			shipSpline[ 0 ]->skeleton->super = ctrlcamera.camPos;


			camSpline[ 1 ] = LoadPrm( "wipeout/common/cam2s.prm", 0 );
			camSpline[ 1 ]->skeleton->super = ctrlcamera.camPos;

			shipSpline[ 1 ] = LoadPrm( "wipeout/common/shp2s.prm", 0 );
			shipSpline[ 1 ]->skeleton->super = ctrlcamera.camPos;


			camSpline[ 2 ] = LoadPrm( "wipeout/common/cam3s.prm", 0 );
			camSpline[ 2 ]->skeleton->super = ctrlcamera.camPos;

			shipSpline[ 2 ] = LoadPrm( "wipeout/common/shp3s.prm", 0 );
			shipSpline[ 2 ]->skeleton->super = ctrlcamera.camPos;


			camSpline[ 3 ] = LoadPrm( "wipeout/common/cam4s.prm", 0 );
			camSpline[ 3 ]->skeleton->super = ctrlcamera.camPos;

			shipSpline[ 3 ] = LoadPrm( "wipeout/common/shp4s.prm", 0 );
			shipSpline[ 3 ]->skeleton->super = ctrlcamera.camPos;
		}


		/* silverstream */
		if ( gameData->actTrackNum == 13 )
		{
			//      printf ( "loading silverstream extro\n" );
			extroLevel = 13;

			camSpline[ 4 ] = LoadPrm( "wipeout/common/cam5s.prm", 0 );
			camSpline[ 4 ]->skeleton->super = ctrlcamera.camPos;

			shipSpline[ 4 ] = LoadPrm( "wipeout/common/shp5s.prm", 0 );
			shipSpline[ 4 ]->skeleton->super = ctrlcamera.camPos;


			camSpline[ 5 ] = LoadPrm( "wipeout/common/cam6s.prm", 0 );
			camSpline[ 5 ]->skeleton->super = ctrlcamera.camPos;

			shipSpline[ 5 ] = LoadPrm( "wipeout/common/shp6s.prm", 0 );
			shipSpline[ 5 ]->skeleton->super = ctrlcamera.camPos;


			camSpline[ 6 ] = LoadPrm( "wipeout/common/cam7s.prm", 0 );
			camSpline[ 6 ]->skeleton->super = ctrlcamera.camPos;

			shipSpline[ 6 ] = LoadPrm( "wipeout/common/shp7s.prm", 0 );
			shipSpline[ 6 ]->skeleton->super = ctrlcamera.camPos;

		}

		//   printf( "Initialise track library Textures\n" );
		ctrllibraryTextures = TextureTableCount;
		getPath(trackPath, gameData->actTrackNum, "/library.cmp");
		ctrltimPtr = LoadCompressedTextureSequence( trackPath );
		LoadCmpFiles( ctrltimPtr );

		//   printf( "Initialise track library Texture templates\n" );
		getPath(trackPath, gameData->actTrackNum, "/library.ttf");
		ctrltexTemplate = LoadTtfFile( trackPath, &ctrlnumLib, ctrllibraryTextures );

		ctrlsection = track->sections;

		ctrl_i = 0;
		NoOfMainSections = 0;

		/*** Reserve memory for weapons functions ***/
		ctrlweaponGrid = ( WeaponGrid* )DAlloc( heap, (sizeof( WeaponGrid) * 64) + 32 );
		ctrlweaponShapes = ( WeaponShape* )DAlloc( heap, (sizeof( WeaponShape) * NO_OF_WEAPONS) + 32 );
		ctrlweaponIndex = ( WeaponData* )DAlloc( heap, (sizeof( WeaponData) * NO_ACTIVE_WEAPONS) + 32 );

		while(ctrlcheck)
		{
			ctrlnoWeaponGrids = SetTrackAttributes(ctrlsection, ctrl_i, ctrlweaponGrid, ctrlnoWeaponGrids);
			ctrlsection->secNo = ctrl_i;
			if(ctrlsection->junction.index != -1)
			{
				ctrljsection = ctrlsection;
				ctrlji = ctrl_i;
				ctrlsection = ctrlsection->junction.ptr;
				while(ctrljcheck)
				{
					ctrl_i++;
					ctrlnoWeaponGrids = SetTrackAttributes(ctrlsection, ctrl_i, ctrlweaponGrid, ctrlnoWeaponGrids);
					ctrlsection->secNo = ctrl_i;
					ctrlsection = ctrlsection->nextSection.ptr;
					if(ctrlsection->junction.index != -1) ctrljcheck = 0;
				}
				ctrljcheck = 1;
				ctrlsection = ctrljsection;
				ctrl_i = ctrlji;
			}
			ctrl_i++;
			NoOfMainSections++;
			ctrlsection = ctrlsection->nextSection.ptr;
			if(ctrlsection == track->sections) ctrlcheck = 0;
		}

		InitSpeedo(&ctrlspeedo);
		InitText();

		ctrlshipIndex = ( ShipData* )DAlloc( heap, ((sizeof( ShipData) + 32) * (NO_OF_SHIPS )) );
		loadShipData(track->sections, ctrlshipIndex, ctrlshipShapes, ctrlcamera.camPos);
		ctrlsky = InitScene(ctrlprm, &ctrlprmCount, ctrlcamera.camPos, gameData);

		InitStartBoom(ctrlprm, ctrlprmCount);

		//ctrlstadCount = InitCrowdSound(ctrlprm, ctrlprmCount, ctrllostad, ctrlstadCount);

		if(trackNo == 8 || trackNo == 11)
		{
			InitRedLight(ctrlprm, ctrlprmCount, &ctrlredLight);
			InitDonkey(ctrlprm, ctrlprmCount, ctrldonkey);
		}
		else if(trackNo == 1 /*|| trackNo == 6*/)
		{
			InitRedLight(ctrlprm, ctrlprmCount, &ctrlredLight);
		}



#if EditorOn
		//   printf( "Initialise Editor\n" );

		ctrleditor.iconTextures = TextureTableCount;
		LoadWtlFiles( "wipeout/common/icons.tex" );

		ctrltexHighlight = CreateTexHighlight( );
		ctrltexHighlight->skeleton->super = ctrlcamera.camPos;  /* from offset */

		InitEditor( &ctrleditor );

		ctrltexPlace.libraryTextures = ctrllibraryTextures;
		ctrltexPlace.numLib = ctrlnumLib;
		ctrltexPlace.emptyTexture = ctrleditor.iconTextures + fEmpty;
		InitTexPlace( &ctrltexPlace, ctrltexTemplate );
#endif


		InitCamera(&ctrlcamera, track->sections);

		ctrlShadow = InitShadow(ctrlcamera.camPos, ctrleffectTextures);

		if(trackNo == 13)
			InitAuroraBorialis(ctrlprm[ctrlsky]);

		InitWeaponSprtIcons(ctrlweaponIcon, ctrlweaponIconTextures);
		//   printf("after wicons\n");
		switch (gameData->shipNum)
		{
		case 0:	InitScreenTex (ctrlwinLose, "dekka") ;
			break ;

		case 1: 	InitScreenTex (ctrlwinLose, "chang") ;
			break ;

		case 2:	InitScreenTex (ctrlwinLose, "arial") ;
			break ;

		case 3:	InitScreenTex (ctrlwinLose, "anast") ;
			break ;

		case 4:	InitScreenTex (ctrlwinLose, "solar") ;
			break ;

		case 5:	InitScreenTex (ctrlwinLose, "arian") ;
			break ;

		case 6:	InitScreenTex (ctrlwinLose, "sophi") ;
			break ;

		case 7:	InitScreenTex (ctrlwinLose, "paul") ;
			break ;
		}
		//   printf("after win lose\n");

		initShipData(track->sections, ctrlshipIndex, gameData);
		InitRescueDroid(&ctrlrescueDroid, ctrlshipShapes, &(ctrlshipIndex[ownShip]));

		initWeapons(ctrlweaponShapes, ctrlcamera.camPos, ctrlweaponIndex, ctrlTarget);

		ctrleffects = ( Effect* )DAlloc( heap, (sizeof( Effect ) * NO_OF_EFFECTS) + 32 );
		InitEffects(ctrleffects, ctrlcamera.camPos, ctrleffectTextures);

#if SHIP_DEBUG
		InitMapShips(ctrlmapShips, ctrltrackMap);
#endif
		CameraUpdate = UpdateCameraRaceStart;





		spareMax = 0;

		/* Initialize hi-score and game */

		InitHiScoreData (&ctrlhiScoreData) ;

		gameData->defaultMenu = -2 ;
		gameData->Lives = 3 ;
		gameData->Position = 8 ;
		gameData->EndOfGame = 0 ;
		gameData->gameOverCount = 0 ;

		ctrlfadeData.fadeCount = FADE_INACTIVE ;
		ctrlfadeData.fadeFlag = FADE_INACTIVE ;

		ctrlhiScoreData.confirm = 2 ;
		ctrlhiScoreData.lConfirm = 0 ;


		for (ctrl_i=0; ctrl_i<8; ctrl_i++ )
		{
			ctrlcone.object[ ctrl_i ] = ctrlshipShapes[ ctrlshipIndex[ ctrl_i ].highResTableIndex ];
			ctrlcone.ship[ ctrl_i ] = &ctrlshipIndex[ ctrl_i ];
		}

		InitCone( &ctrlcone );






#if RobTest
		if ( 1 )
		{
			ctrlmode = ATTRACT;

			ownShip = 4;

			if(ctrlmode == ATTRACT)
			{
				//         printf( "init attract\n" );

				ctrlshipIndex[ownShip].attr &= ~RACING;
				CameraUpdate = UpdateCameraExtroAttract;

				for(ctrl_i = 0; ctrl_i < NO_OF_SHIPS; ctrl_i++)
				{
					ctrlshipIndex[ctrl_i].remoteMaxThrust = 3160;
					ctrlshipIndex[ctrl_i].remoteThrustMag = 32;
					ctrlshipIndex[ctrl_i].speed = 3160;
				}

				attractDelay = 8000 * 30;
			}
		}
#else
		if(ctrlmode == ATTRACT)
		{
			inattract=1;	//jed
			ctrlshipIndex[ownShip].update = UpdatePlayerShipNormStart;
			ctrlshipIndex[ownShip].updateCount = START_TIME;
			ctrlshipIndex[ownShip].remoteMaxThrust = 3160;
			ctrlshipIndex[ownShip].remoteThrustMag = 32;
			ctrlshipIndex[ownShip].speed = 3160;
			ctrlshipIndex[ownShip].attr &= ~VIEW_IN;
			ctrlshipIndex[ownShip].attr &= ~RACING;
			CameraUpdate = UpdateCameraAttractMode;

			if((rand()%3) == 0)
			{
				ctrlplayCredits = 1;
				attractDelay = 75 * FR30;		//PCWipeout
			}
			else
			{
				attractDelay = 60 * FR30; // was 30*30 but this was too int16_t I think
			}
			//				if(ctrlplayCredits == 1)
		}
#endif



		ctrlnextSection = ctrlcamera.section;

#if SystemFont
		InitFonts( );
#endif




		ctrlshipIndex[ownShip].haveFired = NON_ACTIVE ;

		if (gameType != SINGLE)
			ctrlshipIndex[serialShip].haveFired = NON_ACTIVE ;

		SetNegconPresets (gameData) ;
		SetNormPadPresets (gameData) ;




#if 0
		VsyncEnd( );
#endif

		// 	printf( "Heap free before init track tiles: %d\n", heap->free );

		//   printf( "Initialise Track Tiles\n" );
		AsmGenerateTiles( ctrltexTemplate, ctrlnumLib );

		// 	printf( "Heap free before main loop: %d\n", heap->free );

#if RobTest
		printf ( "\n" );
		printf ( "fire\n" );
		printf( "camSpline 0 prim %d\n", camSpline[ 0 ]->primitiveCount );
		printf( "shipSpline 0 prim %d\n", shipSpline[ 0 ]->primitiveCount );
		printf ( "\n" );
		printf( "camSpline 1 prim %d\n", camSpline[ 1 ]->primitiveCount );
		printf( "shipSpline 1 prim %d\n", shipSpline[ 1 ]->primitiveCount );
		printf ( "\n" );
		printf( "camSpline 2 prim %d\n", camSpline[ 2 ]->primitiveCount );
		printf( "shipSpline 2 prim %d\n", shipSpline[ 2 ]->primitiveCount );
		printf ( "\n" );
		printf( "camSpline 3 prim %d\n", camSpline[ 3 ]->primitiveCount );
		printf( "shipSpline 3 prim %d\n", shipSpline[ 3 ]->primitiveCount );
		printf ( "\n" );
		printf ( "ice\n" );
		printf( "camSpline 4 prim %d\n", camSpline[ 4 ]->primitiveCount );
		printf( "shipSpline 4 prim %d\n", shipSpline[ 4 ]->primitiveCount );
		printf ( "\n" );
		printf( "camSpline 5 prim %d\n", camSpline[ 5 ]->primitiveCount );
		printf( "shipSpline 5 prim %d\n", shipSpline[ 5 ]->primitiveCount );
		printf ( "\n" );
		printf( "camSpline 6 prim %d\n", camSpline[ 6 ]->primitiveCount );
		printf( "shipSpline 6 prim %d\n", shipSpline[ 6 ]->primitiveCount );
#endif


		if ( gameData->actTrackNum == 13 )
		{
			InitIceCongratulations( );
		}

		if ( gameData->actTrackNum == 14 )
		{
			InitFireCongratulations( );
		}

		// PCWIPEOUT PALETTE SHIT

		inmenu=0;
		for( ctrl_i = 0; ctrl_i < 2; ctrl_i++)
		{
			ClearOTagR( OT[ CurrentScreen ], OT_SIZE );
			Swap( );		//Should have no problems with this on interrupt
		}
		CopyPal(main_palette,0,255);
		InitAJSVSync();

	}

}

extern void ReadJoystick();


void racectrl()
{
	combatData *packetData;
	ConfigData *gameData;
	int32_t mode, timeout;

	packetData = &ctrlpacketData;
	gameData = ctrlgameData;
	mode	 = ctrlmode;
	timeout = ctrltimeout;

	if (!ok_to_interrupt)
		return;


		/*/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


			###############		#############	##############	 ##############
			#			  #     #    		#	#				 #
			#			  #		#			#	#				 #
			#			 #		#			#	#				 #
			#			#		#			#	#				 #
			#		  #			#			#	#				 #
			#		 #			#			#	#				 #
			#########			#############	#				 #########
			#		 #			#			#	#				 #
			#		  #			#			#	#				 #
			#		   #		#			#	#				 #
			#			#		#			#	#				 #
			#			 #		#			#	#				 #
			#			  #		#			#	##############   ##############

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////*/






	//Reset all of my little Drawing markers
	ctrlNeedEndText = 0;
	ctrlNeedWinLose = 0;
	ctrlNeedBackPoly = 0;
	ctrlNeedRaceInfo = 0;
	ctrlNeedDeSelectScreen = 0;
	ctrlNeedNewHighScores = 0;
	ctrlNeedRacePoints = 0;
	ctrlNeedChampTable = 0;
	ctrlNeedAllVenomComplete = 0;
	ctrlNeedVenomComplete = 0;
	ctrlNeedContinueGameConfirm = 0;
	ctrlNeedInGameReset = 0;
	ctrlNeedInGameConfirm = 0;
	ctrlNeedInSingleGameConfirm = 0;
	ctrlNeedTargetIcon = 0;


	   opad=pad;
	   //	   archive_pad = pad;
	   pad = neg1.bits;
	   if((neg1.head != 0xff))
		   pad = ~pad;
	   //	   archive_xpad = xpad;				//preserve xpad from the drawing side
	   xpad=(pad^opad)&pad;				//xpad is difference between this frames
	   //pad and the last frames pad.



	   //		AJSVSync(2);
	   if (ctrlfirsttime)
	   {
		   PlayNote(CROWD, 0, -1);
		   PlayNote(CROWD2, 0, -1);
		   ctrlfirsttime = 0;
	   }

	   if((ctrlbuttonPressed == ctrltimeout) &&		//If no buttons have hit for timeout, and we were racing
		   (ctrlshipIndex[ownShip].attr & RACING))		//set up for attract mode.
	   {
		   ctrlshipIndex[ownShip].update = UpdateRemoteShip;
		   ctrlshipIndex[ownShip].updateCount = START_TIME;
		   ctrlshipIndex[ownShip].remoteMaxThrust = 3160;
		   ctrlshipIndex[ownShip].remoteThrustMag = 32;
		   ctrlshipIndex[ownShip].speed = 3160;
		   ctrlshipIndex[ownShip].attr &= ~VIEW_IN;
		   ctrlshipIndex[ownShip].attr &= ~RACING;
		   CameraUpdate = UpdateCameraAttractMode;
		   attractDelay = 30 * FR30;		//PAL 25
		   ctrlmode = ATTRACT;
		   inattract=1;	//jed
		   ctrlbuttonPressed = 0;
	   }





	   if(ctrlshipIndex[ownShip].attr & RACING)
	   {
		   if ((GetMenuKey(M_START)) && (ctrlchoice == -1))
		   {
			   if(gameData->Lives)
			   {
				   ctrlCDPaused = 1;							//If we're racing, and start key hit pause CD
				   PlayNote(MENU_SELECT, 0, 20);				// go boing
				   ctrlchoice = 0;							// set ctrlchoice to the first on the list
				   xpad = 0;									// clear the status of the pad so that the next bit does not select
			   }
		   }
	   }

	   if(ctrlchoice > -1)								// we are in pause state
	   {
		   DoDetailKeys();

		   UpdateNotes(PAUSED);						// service the held notes
		   if ((GetMenuKey(M_UP)) && ctrlshowMenu)		// hit up key
		   {
			   ctrlchoice--;
			   if (ctrlchoice < 0)
				   ctrlchoice = 3 ;					// hard code no of entries to 3!
			   PlayNote(MENU_MOVE, 0, 20);				// Play a boing
		   }
		   else if ((GetMenuKey(M_DOWN)) && ctrlshowMenu)
		   {											// ditto but down
			   ctrlchoice++;
			   if (ctrlchoice > 3)
				   ctrlchoice = 0 ;
			   PlayNote(MENU_MOVE, 0, 20);
		   }


		   if (ctrlshowMenu)
		   {
			   ctrlNeedInGameReset = 1;
			   ctrlNeedDeSelectScreen = 1;
			   ctrllConfirm = 0 ;
			   //Keyboard stuff
			   if(ctrlchoice == 3)
			   {
				   if(GetMenuKey(M_LEFT))
				   {
					   PlayNote(MENU_MOVE, 1, 2);
					   gameData->cdTrack -- ;
					   if (gameData->cdTrack < 1)
						   gameData->cdTrack = endtrack ;
					   CDRestartNeeded = 1;
					   ctrlcdPause = 1;
				   }
				   else if(GetMenuKey(M_RIGHT))
				   {
					   PlayNote(MENU_MOVE, 1, 2);
					   gameData->cdTrack ++ ;
					   if (gameData->cdTrack > endtrack)
						   gameData->cdTrack = 1 ;
					   CDRestartNeeded = 1;
					   ctrlcdPause = 1;
				   }
				   CurrTrkNo = gameData->cdTrack;
			   }
		   }
		   else
		   {
			   ctrlNeedInGameConfirm = 1;
			   if ((GetMenuKey(M_LEFT)) || (GetMenuKey(M_RIGHT)))
			   {
				   ctrllConfirm ^= 1 ;
				   PlayNote(MENU_MOVE, 1, 20);
			   }
			   //				if ((GetMenuKey(M_SELECT)) || (GetMenuKey(M_START)))
			   if (GetMenuKey(M_SELECT))
				   ctrlconfirm = ctrllConfirm ;

			   ctrlNeedDeSelectScreen = 1;
		   }

		   if (ctrlconfirm == 0)
		   {
			   ctrlshowMenu = 1 ;

			   ctrlconfirm = 2 ;
			   ctrlchoice = 0;
		   }

		   //		    if( ((GetMenuKey(M_SELECT)) || (GetMenuKey(M_START))) && (!ctrlshowMenu) && (ctrlconfirm == 1) && (ctrlmachinePaused != SLAVE))
		   if(GetMenuKey(M_SELECT) && (!ctrlshowMenu) && (ctrlconfirm == 1) && (ctrlmachinePaused != SLAVE))
		   {
			   ctrlshowMenu = 1 ;
			   ctrlconfirm = 2 ;

			   switch(ctrlchoice)
			   {
			   case 0: ctrlchoice = -1;			//Continue with the game
				   ctrlchoiceData = 0 ;		// no new data
				   break;

			   case 1: ctrlchoice = -1;		/* RESET */
				   ctrlchoiceData = 1 ;

				   if (gameData->gameType == CHAMPIONSHIP)	//Chanpionship mode, then lose a life for the prividge
				   {										// of restarting your race.
					   gameData->Lives -- ;
					   if (gameData->Lives == 0)
					   {
						   ctrlgameOverTime = 90;			//3 second
						   ctrlshowGameOver = 1 ;
						   xpad = 0 ;
						   break ;
					   }
				   }
				   Restart(	ctrlshipIndex,
					   gameData,
					   ctrlweaponIndex,
					   ctrlweaponShapes,
					   ctrleffects,
					   &ctrlrescueDroid,
					   ctrlshipShapes);

				   PlayNote(CROWD, 0, -1);
				   PlayNote(CROWD2, 0, -1);
				   break;

			   case 2: ctrlchoice = -1;
				   ctrlchoiceData = 2 ;
				   if (ctrlmachinePaused == SINGLE)
				   {
					   QuitGame();
					   gameData->defaultMenu = -2 ;
					   gameData->gameType = NO_GAME_SET ;
					   ctrl_says_quit = 1;
					   return;
				   }
				   break ;

			   }/*end switch*/
		   }/*end if (load a bollox*/

		   // 			if ( ((GetMenuKey(M_SELECT)) || (GetMenuKey(M_START))) && (ctrlshowMenu) && ctrlmachinePaused != SLAVE)
		   if (GetMenuKey(M_SELECT) && (ctrlshowMenu) && ctrlmachinePaused != SLAVE)
		   {
			   PlayNote(MENU_SELECT, 0, 20);
			   if (ctrlchoice == 1 || ctrlchoice == 2)
			   {
				   ctrlshowMenu = 0 ;
			   }
			   else if (ctrlchoice == 0)
			   {
				   ctrlCDPaused = 2;
				   ctrlchoiceData = 0 ;
				   ctrlchoice = -1 ;
			   }
		   }
        }//endif ctrlchoice > -1

        if (ctrlshowGameOver == 1)
		{

		/*			DeSelectScreen (ctrlselectBox, ctrlselectTrans) ;
			*/
			if(ctrlgameOverTime == 0)
			{
				ctrlshowGameOver = 0 ;
				gameData->newLapRecord = 0 ;
			}
			ctrlgameOverTime--;

			/*			if (GetMenuKey(M_SELECT))
			{
			ctrlshowGameOver = 0 ;
			gameData->newLapRecord = 0 ;
			}
			*/
			if (ctrlshowGameOver == 0)
			{
				QuitGame();
				gameData->defaultMenu = MAIN_MENU ;
				gameData->gameType = NO_GAME_SET ;
				ctrl_says_quit = 1;
				return;
			}

		}

		if((ctrlchoice == -1) && (ctrlshowGameOver == 0))
		{
			UpdateNotes(NOT_PAUSED);

			if(ctrlmode != ATTRACT)
			{
				if((neg1.head != 8960) && (gameType == SINGLE))
				{
					if(pad == 0)
						ctrlbuttonPressed++;
					else
						ctrlbuttonPressed = 0;
				}
				else if( (neg1.head == 8960) && (gameType == SINGLE))
				{
					if((pad != 0) ||
						(neg1.twist > 135) ||
						(neg1.twist < 122) ||
						(neg1.i > 10) ||
						(neg1.ii > 10)	||
						(neg1.lShift > 10))
						ctrlbuttonPressed = 0;
					else
						ctrlbuttonPressed++;
				}
			}


			if (gameType != SINGLE)
			{
				LaunchWeapon(ctrlshipIndex, ctrlweaponIndex, 0);
				for(ctrl_i = 0; ctrl_i < NO_OF_SHIPS; ctrl_i++)
				{
					ctrlshipIndex[ctrl_i].haveFired = NO_WEAPON;
				}
			}

			if ((gameType == MASTER) || (gameType == SINGLE))
				UpdateShips(ctrlshipIndex, ctrlweaponShapes, ctrlshipShapes, ctrlweaponIndex);
			else
				UpdateShipsSerial(ctrlshipIndex, ctrlweaponShapes, ctrlshipShapes, ctrlweaponIndex);

			(ctrlrescueDroid.droidUpdate)(&ctrlrescueDroid, &(ctrlshipIndex[ownShip]), ctrlshipShapes );

#if COLLIDE_TEST
			CameraUpdate = UpdateCameraCollideTest;
#endif
			if( RobTest ||  ctrldoneExtro == UPDATE_EXTRO )
			{
				(CameraUpdate)(&ctrlcamera, ctrlshipIndex, &ctrlrescueDroid);
			}
			else
			{
				(CameraUpdate)(&ctrlcamera, &(ctrlshipIndex[ownShip]), &ctrlrescueDroid);
			}


			UpdateCone( &ctrlcone );

			if(ctrlshipIndex[ownShip].attr & RACING)
			{
				if( ( (ctrlshipIndex[ownShip].weaponType == MISSILE) || (ctrlshipIndex[ownShip].weaponType == ELEC_BOLT) ||
					(ctrlshipIndex[ownShip].weaponType == REV_CON)	)
					&& (ctrlshipIndex[ownShip].attr & RACING) )
				{
					MissileLock(&(ctrlshipIndex[ownShip]), ctrlshipIndex, ctrlshipShapes, ctrlTarget);
				}

				ShakeScreen();
			}

			UpdateWeapons(ctrlshipIndex, ctrlweaponShapes, ctrlweaponIndex );

			if (gameType == SINGLE)
				CallShip2ShipCollision(ctrlshipIndex, ctrlshipShapes);

			//AdjustEngineSound(&(ctrlshipIndex[ownShip]), &ctrlcamera);

			//AdjustCrowdSound(ctrllostad, ctrlstadCount);
			//AdjustRemoteEngineSound(ctrlshipIndex, ctrlshipShapes, &ctrlcamera );


			UpdateEffects(ctrleffects);

			if(raceType != TIME_TRIAL)
			{
				CycleWeaponGrids(ctrlshipIndex, ctrlweaponGrid, ctrlnoWeaponGrids);
			}

		}


		if((attractDelay >= 0) && (ctrlmode == ATTRACT))
		{
			attractDelay--;
			if ((GetMenuKey(M_START)) ||
				(GetMenuKey(M_QUIT))  ||
				(GetMenuKey(M_SELECT)) ||
				(attractDelay == 0))
			{
				gameData->defaultMenu = -2 ;
				gameData->gameType = NO_GAME_SET ;
				QuitGame();
				ctrl_says_quit = 1;
				return ;
			}

		}

		else if(attractDelay > 0)
		{
			if (gameType == MASTER || gameType == SLAVE)
			{
				if((ctrlshipIndex[ownShip].lapNo >= 4) && (ctrlshipIndex[serialShip].lapNo >= 4))
					packetData->exit = 1 ;
			}


			if(gameType == SINGLE)
			{
				attractDelay--;

				if (attractDelay == 0 && gameData->gameType == SINGLE_RACE)
				{
					attractDelay = 1 ;
					if (gameData->showPilotCount != 0)
					{
						if (gameData->Position == 1)
						{
							inattract=1;	//jed
							ctrlNeedWinLose = 1;
							ctrlNeedEndText = 1;
							ctrlNeedBackPoly = 1;
							ctrlNeedRaceInfo = 1;
							ctrlNeedDeSelectScreen = 1;
							ctrlNeedEndTextVar = 1;
							ctrlNeedWinLoseVar = 1;
						}
						else
						{
							inattract=1;	//jed
							ctrlNeedWinLose = 1;
							ctrlNeedEndText = 1;
							ctrlNeedBackPoly = 1;
							ctrlNeedRaceInfo = 1;
							ctrlNeedDeSelectScreen = 1;
							ctrlNeedEndTextVar = 2;
							ctrlNeedWinLoseVar = 0;
						}

						ajscount++;
						if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
						{
							gameData->showPilotCount = 0 ;
							ajscount=0;
						}
					}
					else
					{
						gameData->newLapRecord = 0 ;

						if (gameData->EndOfGame)
						{
							if (gameData->Position == 1)
							{
								ctrlhiScoreData.highScore = CheckHiScore (gameData, &ctrlhiScoreData) ;

							}
						}

						if (ctrlhiScoreData.highScore)
						{
							ctrlhiScoreData.displayHiScore = 1 ;
							ctrlhiScoreData.highScore = 0 ;
						}
						if (ctrlhiScoreData.displayHiScore)
						{
							ctrlNeedNewHighScores = 1;
							NewHighScore (gameData, &ctrlhiScoreData) ;
							ctrlNeedDeSelectScreen = 1;
							gameData->EndOfGame = 0 ;
						}
						else
						{
							ajscount++;

							ctrlNeedInSingleGameConfirm = 1;
							if ((GetMenuKey(M_LEFT)) || (GetMenuKey(M_RIGHT)))
							{
								ctrlhiScoreData.lConfirm ^= 1 ;
								PlayNote(MENU_MOVE, 1, 20);
							}
							ajscount++;
							if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
							{
								if (ajscount>AJSDELAY) ctrlhiScoreData.lConfirm=1;
								ctrlhiScoreData.confirm = ctrlhiScoreData.lConfirm ;
								ajscount=0;
							}

							ctrlNeedDeSelectScreen = 1;

							if (ctrlhiScoreData.confirm == 1)
							{
								ctrlhiScoreData.confirm = 2 ;
								ctrlhiScoreData.lConfirm = 0 ;
								attractDelay = 0 ;
								gameData->defaultMenu = -2 ;
								gameData->gameType = NO_GAME_SET ;
								QuitGame();
								ctrl_says_quit = 1;
								return ;
							}

							if (ctrlhiScoreData.confirm == 0)
							{
								attractDelay = 0 ;
								ctrlhiScoreData.confirm = 2 ;
								ctrlhiScoreData.lConfirm = 0 ;
								inattract=0;
								CDRestartNeeded=1;
								ctrlNeedDeSelectScreen = 0;
								InitHiScoreData(&ctrlhiScoreData);

								Restart(	ctrlshipIndex,
									gameData,
									ctrlweaponIndex,
									ctrlweaponShapes,
									ctrleffects,
									&ctrlrescueDroid,
									ctrlshipShapes);
								PlayNote(CROWD, 0, -1);
								PlayNote(CROWD2, 0, -1);
							}


							//							if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
							//							{
							//								ajscount=0;
							//								gameData->newLapRecord = 0 ;
							//								ctrlhiScoreData.displayHiScore = 0 ;
							//								attractDelay = 0 ;
							//								gameData->defaultMenu = TRACK_MENU ;
							//								QuitGame();
							//								ctrl_says_quit = 1;
							//								return ;
							//							}
						}
					}

				}


				if (attractDelay == 0 && gameData->gameType == TIME_TRIAL)
				{
					attractDelay = 1 ;

					if (gameData->gameType == TIME_TRIAL)
					{

						if (gameData->showPilotCount != 0)
						{

							ctrlNeedRaceInfo = 1;
							ctrlNeedDeSelectScreen = 1;

							ajscount++;
							if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
							{
								gameData->showPilotCount = 0 ;
								ajscount=0;
							}
						}
						else
						{
							gameData->newLapRecord = 0 ;
							if (gameData->EndOfGame)
								ctrlhiScoreData.highScore = CheckHiScore (gameData, &ctrlhiScoreData) ;

							if (ctrlhiScoreData.highScore)
							{
								ctrlhiScoreData.displayHiScore = 1 ;
								ctrlhiScoreData.highScore = 0 ;
							}

							if (ctrlhiScoreData.displayHiScore)
							{
								ctrlNeedNewHighScores = 1;
								NewHighScore (gameData, &ctrlhiScoreData) ;
								ctrlNeedDeSelectScreen = 1;
								gameData->EndOfGame = 0 ;

							}
							else
							{
								ajscount++;

								ctrlNeedInSingleGameConfirm = 1;
								if ((GetMenuKey(M_LEFT)) || (GetMenuKey(M_RIGHT)))
								{
									ctrlhiScoreData.lConfirm ^= 1 ;
									PlayNote(MENU_MOVE, 1, 20);
								}
								ajscount++;
								if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
								{
									if (ajscount>AJSDELAY) ctrlhiScoreData.lConfirm=1;
									ctrlhiScoreData.confirm = ctrlhiScoreData.lConfirm ;
									ajscount=0;
								}


								ctrlNeedDeSelectScreen = 1;

								if (ctrlhiScoreData.confirm == 1)
								{
									ctrlhiScoreData.confirm = 2 ;
									ctrlhiScoreData.lConfirm = 0 ;
									attractDelay = 0 ;
									gameData->defaultMenu = -2 ;
									gameData->gameType = NO_GAME_SET ;
									QuitGame();
									ctrl_says_quit = 1;
									return ;
								}

								if (ctrlhiScoreData.confirm == 0)
								{
									attractDelay = 0 ;
									ctrlhiScoreData.confirm = 2 ;
									ctrlhiScoreData.lConfirm = 0 ;
									inattract=0;
									CDRestartNeeded=1;
									InitHiScoreData(&ctrlhiScoreData);
									Restart(	ctrlshipIndex,
										gameData,
										ctrlweaponIndex,
										ctrlweaponShapes,
										ctrleffects,
										&ctrlrescueDroid,
										ctrlshipShapes);
									PlayNote(CROWD, 0, -1);
									PlayNote(CROWD2, 0, -1);
								}


								//								ajscount++;
								//								if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
								//								{
								//									ajscount=0;
								//									gameData->newLapRecord = 0 ;
								//									ctrlhiScoreData.displayHiScore = 0 ;
								//									attractDelay = 0 ;
								//									gameData->defaultMenu = TRACK_MENU ;
								//									QuitGame();
								//									ctrl_says_quit = 1;
								//									return ;
								//
								//								}
							}
						}
						}
						}

						/****************** Do Extro ******************/
						if(attractDelay == 0 && gameData->gameType == CHAMPIONSHIP )
						{
							attractDelay = 1 ;

							if (gameData->gameType == CHAMPIONSHIP)
							{

								if (gameData->Lives == 0)	/*0*/
								{
									if (gameData->gameOverCount != 0)
									{
										ctrlNeedWinLose = 1;
										ctrlNeedEndText = 1;
										ctrlNeedBackPoly = 1;
										ctrlNeedRaceInfo = 1;
										ctrlNeedDeSelectScreen = 1;
										ctrlNeedEndTextVar = 3;
										ctrlNeedWinLoseVar = 0;

										ajscount++;
										if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
										{
											ajscount=0;
											gameData->showGameOver = 1 ;
											ctrlgameOverTime = 90;			//3 seconds
											gameData->gameOverCount = 0 ;
											gameData->newLapRecord = 0 ;
										}
									}
									else
										if (gameData->gotoMenu == 1)
										{
											gameData->gotoMenu = 0 ;
											gameData->gameType = NO_GAME_SET ;
											gameData->defaultMenu = -2 ;
											gameData->newLapRecord = 0 ;
											QuitGame();
											ctrl_says_quit = 1;
											return ;
										}
										else

											if (gameData->showGameOver == 1)
											{

												ajscount++;
												if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
												{
													ajscount=0;
													gameData->gotoMenu = 1 ;
													gameData->showGameOver = 0 ;
													gameData->newLapRecord = 0 ;
												}
											}
								}
								else
								{
									if (gameData->showPilotCount != 0)
									{
										if (gameData->Position >= 1 && gameData->Position <= 3)
										{
											ctrlNeedWinLose = 1;
											ctrlNeedEndText = 1;
											ctrlNeedBackPoly = 1;
											ctrlNeedRaceInfo = 1;
											ctrlNeedDeSelectScreen = 1;
											ctrlNeedEndTextVar = 1;
											ctrlNeedWinLoseVar = 1;
										}
										else
										{
											ctrlNeedWinLose = 1;
											ctrlNeedEndText = 1;
											ctrlNeedBackPoly = 1;
											ctrlNeedRaceInfo = 1;
											ctrlNeedDeSelectScreen = 1;
											ctrlNeedEndTextVar = 3;
											ctrlNeedWinLoseVar = 0;
										}

										ajscount++;
										if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
										{
											ajscount=0;
											gameData->showPilotCount = 0 ;
											gameData->newLapRecord = 0 ;
										}
									}
									else
										if (gameData->showPointCount != 0)
										{
											if (gameData->Position >= 1 && gameData->Position <= 3)
											{
												ctrlNeedRacePoints = 1;
												ctrlNeedDeSelectScreen = 1;
											}
											ajscount++;
											if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
											{
												ajscount=0;
												gameData->showPointCount = 0 ;
											}
										}
										else if (gameData->showChampCount != 0)
										{
											if (gameData->Position >= 1 && gameData->Position <= 3)
											{
												ctrlNeedChampTable = 1;
												ctrlNeedDeSelectScreen = 1;
											}
											ajscount++;
											if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
											{
												ajscount=0;
												gameData->showChampCount = 0 ;
											}
										}
										else if (gameData->venomComplete != 0)
										{
											if (gameData->showBonusTrack == 1)
												ctrlNeedAllVenomComplete = 1;
											else
												ctrlNeedVenomComplete = 1;

											ctrlNeedDeSelectScreen = 1;

											ajscount++;
											if ((ajscount>(AJSDELAY/2)))
											{
												ajscount=0;
												gameData->venomComplete = 0 ;
											}
										}
										else
										{
											if (gameData->Position >= 1 && gameData->Position <= 3)
											{
												if (gameData->EndOfGame)
													ctrlhiScoreData.highScore = CheckHiScore (gameData, &ctrlhiScoreData) ;
											}

											if (ctrlhiScoreData.highScore)
											{
												ctrlhiScoreData.displayHiScore = 1 ;
												ctrlhiScoreData.highScore = 0 ;
											}

											if (ctrlhiScoreData.displayHiScore)
											{
												ctrlNeedNewHighScores = 1;
												NewHighScore (gameData, &ctrlhiScoreData) ;
												ctrlNeedDeSelectScreen = 1;

												gameData->EndOfGame = 0 ;
											}
											else
											{
												if (gameData->lostGame == 1)
												{

													ajscount++;
													if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
													{
														ajscount=0;
														gameData->lostGame = 0 ;
														gameData->gotoMenu = 1 ;
													}

												}
												else
												{

													if (gameData->gameComplete == 1)
													{
														if (ctrldoneExtro == START_EXTRO)
														{
															//                                       printf( "init extro\n" );
															CameraUpdate = UpdateCameraExtroAttract;
															for(ctrl_i = 0; ctrl_i < NO_OF_SHIPS; ctrl_i++)
															{
																ctrlshipIndex[ctrl_i].remoteMaxThrust = 3160;
																ctrlshipIndex[ctrl_i].remoteThrustMag = 32;
																ctrlshipIndex[ctrl_i].speed = 3160;
															}
															ctrldoneExtro = UPDATE_EXTRO;
														}

														if(ctrldoneExtro == UPDATE_EXTRO)
														{
															ajscount++;
															if( GetMenuKey(M_SELECT) || GetMenuKey(M_START) || (ajscount>AJSDELAY))
															{
																ajscount=0;
																finishedExtro = 1;
															}
														}

														if ( finishedExtro )
														{
															//                                       printf( "\nif finished extro \n" );
															gameData->gameType = NO_GAME_SET ;
															gameData->defaultMenu = OPTION_MENU ;
															gameData->gotoMenu = 0 ;
															InitChampionship () ;
															gameData->gameComplete = 0 ;
															QuitGame();
															ctrl_says_quit = 1;
															return ;

															/* gameData->gameComplete = 0;
															gameData->gotoMenu = 1;  */
														}
													}
													else
													{

														if (gameData->gotoMenu != 0)
														{
															gameData->gameType = NO_GAME_SET ;
															gameData->defaultMenu = OPTION_MENU ;
															gameData->gotoMenu = 0 ;
															InitChampionship () ;
															gameData->gameComplete = 0 ;
															QuitGame();
															ctrl_says_quit = 1;
															return ;
														}

														gameData->EndOfGame = 0 ;
														InitHiScoreData (&ctrlhiScoreData) ;
														gameData->newLapRecord = 0 ;
														if (gameData->Position >= 1 && gameData->Position <= 3)
														{
															//													printf ("Number of track = %d \n", gameData->NumTracks) ;
															QuitGame();
															ctrl_says_quit = 1;
															return ;
														}
														else
														{
															ctrlNeedContinueGameConfirm = 1;
															if ((GetMenuKey(M_LEFT)) || (GetMenuKey(M_RIGHT)))
															{
																ctrlhiScoreData.lConfirm ^= 1 ;
																PlayNote(MENU_MOVE, 1, 20);
															}
															ajscount++;
															if ((GetMenuKey(M_SELECT))||(ajscount>AJSDELAY))
															{
																if (ajscount>AJSDELAY) ctrlhiScoreData.lConfirm=1;
																ctrlhiScoreData.confirm = ctrlhiScoreData.lConfirm ;
																ajscount=0;
															}


															ctrlNeedDeSelectScreen = 1;

															if (ctrlhiScoreData.confirm == 1)
															{
																ctrlhiScoreData.confirm = 2 ;
																ctrlhiScoreData.lConfirm = 0 ;
																attractDelay = 0 ;
																gameData->defaultMenu = -2 ;
																gameData->gameType = NO_GAME_SET ;
																QuitGame();
																ctrl_says_quit = 1;
																return ;
															}

															if (ctrlhiScoreData.confirm == 0)
															{
																attractDelay = 0 ;
																ctrlhiScoreData.confirm = 2 ;
																ctrlhiScoreData.lConfirm = 0 ;
																inattract=0;
																Restart(	ctrlshipIndex,
																	gameData,
																	ctrlweaponIndex,
																	ctrlweaponShapes,
																	ctrleffects,
																	&ctrlrescueDroid,
																	ctrlshipShapes);
																PlayNote(CROWD, 0, -1);
																PlayNote(CROWD2, 0, -1);
															}
														}
													}
										}
									}
								}
							}
						}
					}
					else
						gameData->continueConfirm = 1 ;
					}
				}

				if(ctrlhiScoreData.displayHiScore == 1)
				{

				}

				/*				else if((shipIndex[ownShip].attr & RACING) ||
				(CameraUpdate == UpdateCameraAttractInternal))
				*/
				{
					if (( ctrlmode != ATTRACT))
					{
						UpdateRaceHud(ctrlshipIndex, &ctrlspeedo, ctrlchoice, gameData);
					}

				}


				/*********************************************************************/
				/**** Draw, Mesh, AutoMesh, Reject, Don`t reject, Nurse, Bend Over  **/
				/**** Abuse & Call a Naughty boy  TRACK CODE								  **/
				/*********************************************************************/


#if SHIP_DEBUG
				DrawMapShips(ctrlmapShips, ctrlshipIndex, ctrltrackMap);
#endif


#if 0 //JJS serial
				if (gameType == MASTER || gameType == SLAVE)
				{

					if ((attractDelay > 0) && ((ctrlshipIndex[ownShip].attr & RACING) == 0))
					{
						attractDelay -- ;

						if (attractDelay == 0)
						{
							attractDelay = 1 ;

							if(ctrlshipIndex[serialShip].attr & RACING )
								CentreText ("WAITING FOR OTHER PLAYER", text_data (25, 116, 8), RedText) ;
							else
							{
								if (raceType == HEAD_TO_HEAD)
								{
									if (winTable[ownShip].racePosition > winTable[serialShip].racePosition)
									{
										ctrlNeedEndText = 1;
										ctrlNeedEndTextVar = 2;
										ctrlNeedWinLose = 1;
										ctrlNeedWinLoseVar = 0;
										ctrlNeedBackPoly = 1;
									}
									else
									{
										ctrlNeedEndText = 1;
										ctrlNeedEndTextVar = 1;
										ctrlNeedWinLose = 1;
										ctrlNeedWinLoseVar = 1;
										ctrlNeedBackPoly = 1;
									}

									ctrlNeedRaceInfo = 1;
									ctrlNeedDeSelectScreen = 1;
								}

								if (raceType == TWO_PLAYER_RACE)
								{

									if (winTable[ownShip].racePosition > winTable[serialShip].racePosition)
									{
										ctrlNeedEndText = 1;
										ctrlNeedEndTextVar = 2;
										ctrlNeedWinLose = 1;
										ctrlNeedWinLoseVar = 0;
										ctrlNeedBackPoly = 1;
									}
									else
									{
										ctrlNeedEndText = 1;
										ctrlNeedEndTextVar = 1;
										ctrlNeedWinLose = 1;
										ctrlNeedWinLoseVar = 1;
										ctrlNeedBackPoly = 1;
									}

									ctrlNeedRaceInfo = 1;
									ctrlNeedDeSelectScreen = 1;
								}
								if (GetMenuKey(M_SELECT))
								{
									attractDelay = 0 ;
									ctrlquitTwoPlayer = 1 ;
								}
							}

						}
					}

					if (ctrlquitTwoPlayer)
					{
						ctrlshipIndex[ownShip].haveFired = QUIT ;
					}


				}


				if ((gameType == MASTER ||
					gameType == SLAVE) && ctrlchoice > -1 && !ctrlactivated)
				{
					ctrlmachinePaused = MASTER ;
					ctrlactivated = 1 ;
					ctrlshipIndex[ownShip].haveFired = PAUSE ;
				}


				if (ctrlactivated && ctrlmachinePaused == MASTER)
				{

					if (ctrlchoiceData == 0)
					{
						ctrlshipIndex[ownShip].haveFired = CONT ;
						ctrlactivated = 0 ;
						ctrlmachinePaused = SINGLE ;
						ctrlchoiceData = -1 ;
					}

					if (ctrlchoiceData == 1)
					{
						ctrlshipIndex[ownShip].haveFired = RETIRE ;
						ctrlactivated = 0 ;
						ctrlmachinePaused = SINGLE ;
						ctrlchoiceData = -1 ;
						//				printf ("Master retiring \n") ;
					}

					if (ctrlchoiceData == 2)
					{
						ctrlshipIndex[ownShip].haveFired = QUIT ;
						/*
						ctrlactivated = 0 ;
						ctrlmachinePaused = SINGLE ;
						choiceData = -1 ;
						*/
					}

				}


				if (gameType == MASTER ||
					gameType == SLAVE )
				{
					if( gameType == MASTER)
					{
						ctrlshipIndex[serialShip].vpivot.vx = ctrlshipIndex[serialShip].ppivot.vx;
						ctrlshipIndex[serialShip].vpivot.vy = ctrlshipIndex[serialShip].ppivot.vy;
						ctrlshipIndex[serialShip].vpivot.vz = ctrlshipIndex[serialShip].ppivot.vz;
					}
					if( gameType == SLAVE)
					{
						for(ctrl_i = 0; ctrl_i < NO_OF_SHIPS; ctrl_i++)
						{
							if(ctrl_i != ownShip)
							{
								ctrlshipIndex[ctrl_i].vpivot.vx = ctrlshipIndex[ctrl_i].ppivot.vx;
								ctrlshipIndex[ctrl_i].vpivot.vy = ctrlshipIndex[ctrl_i].ppivot.vy;
								ctrlshipIndex[ctrl_i].vpivot.vz = ctrlshipIndex[ctrl_i].ppivot.vz;
							}
						}
					}
					packetData->controlData = &neg1 ;
					(packetData->combatControl)(packetData, &(ctrlshipIndex[0])) ;
					SetSerialData(ctrlshipIndex, ctrlshipShapes);
					CallShip2ShipCollision(ctrlshipIndex, ctrlshipShapes);
				}
#endif

				if (ctrlshipIndex[serialShip].haveFired == PAUSE && !ctrlactivated)
				{
					ctrlmachinePaused = SLAVE ;
					ctrlactivated = 1 ;
					ctrlchoice = 0 ;
				}

				if ((ctrlshipIndex[serialShip].haveFired == QUIT) || (ctrlshipIndex[ownShip].haveFired == QUIT))
				{
					ctrlquitTwoPlayer = 0 ;
					ctrlactivated = 0 ;
					ctrlmachinePaused = SINGLE ;
					ctrlchoice = -1 ;
					gameData->defaultMenu = -2 ;
					QuitGame();
					ctrl_says_quit = 1;
					return;
				}

				if (ctrlactivated && ctrlmachinePaused == SLAVE)
				{

					if (ctrlshipIndex[serialShip].haveFired == CONT)
					{
						ctrlactivated = 0 ;
						ctrlmachinePaused = SINGLE ;
						ctrlchoice = -1 ;
					}

					if (ctrlshipIndex[serialShip].haveFired == RETIRE)
					{
						//				printf ("Am retiring \n") ;
						ctrlactivated = 0 ;
						ctrlmachinePaused = SINGLE ;
						ctrlchoice = -1 ;
						ctrlfadeData.fadeFlag = FADE_ACTIVE ;
						gameData->Lives -- ;

						Restart(	ctrlshipIndex,
							gameData,
							ctrlweaponIndex,
							ctrlweaponShapes,
							ctrleffects,
							&ctrlrescueDroid,
							ctrlshipShapes);
						PlayNote(CROWD, 0, -1);
						PlayNote(CROWD2, 0, -1);

					}

					if (ctrlshipIndex[serialShip].haveFired == QUIT)
					{
						ctrlactivated = 0 ;
						ctrlmachinePaused = SINGLE ;
						ctrlchoice = -1 ;
						gameData->defaultMenu = -2 ;
						QuitGame();
						ctrl_says_quit = 1;
						return;

					}


				}

				if (ctrlmachinePaused == MASTER && ctrlchoiceData == 2)
				{
					ctrlactivated = 0 ;
					ctrlmachinePaused = SINGLE ;
					ctrlchoiceData = -1 ;
					gameData->defaultMenu = -2 ;
					QuitGame();
					ctrl_says_quit = 1;
					return;
				}



				negSelect = neg1.i;
				negQuit = neg1.ii;
				negBack = neg1.ii;
				//		copy_glob_to_spare(xpad);
				//		xpad = archive_xpad;	//bodge to restore the xpad of the current drawing frame
				//		pad = archive_pad;	//bodge to restore the xpad of the current drawing frame
				ctrl_init = 1;	//OK to start the drawing now, once we've been around once.

#ifdef PCwipeout
				fred++;
				if (fred>=4)
				{
					//			DumpVram16Bit("wipeout/vramdump/venom/firestar.rgb",&VRam);
					DumpVRamAsPCX("wipeout/vramdump.pcx");
					exit(0);
				}
#endif
} /*end of race*/
