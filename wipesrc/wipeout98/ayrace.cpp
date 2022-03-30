#define TIME_CONSTANT	33
#include <stdio.h>		// PCwipeout
#include "standard.h"

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
#include "ajs.h"
#include "racectrl.h"
#include "wintime.h"
#include "wincd.h"
#include "credits.h"
#include "andyscn.h"
#include "win.h"

#define	_SIZE_T_DEFINED_
//#include <stdlib.h>

#include "input.h"

#define START_EXTRO 0
#define UPDATE_EXTRO 1
#define DONE_EXTRO 2

#define PrmMax       64

#define SHIP_DEBUG 0

// extern uint32_t __bss, __bsslen, _ramsize, _stacksize;
extern int32_t fr ;
extern ChampionShip	winTable[] ;
extern char winLoseIn;

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
void CheckIngameError();

extern	uint8_t			ctrlparam;

char Sky = 1;
extern	char* VRam;					// PCwipeout

extern int16_t  			ctrlsky;
extern Object			*ctrlshipShapes[];
extern Effect			*ctrleffects ;
extern TrackCamera		ctrlcamera;
extern Shadow			*ctrlShadow;
extern AutoMeshData   *ctrlautoMesh;
extern RescueData		ctrlrescueDroid;
extern char			ctrlshowGameOver;
extern int32_t			ctrlNeedEndText;
extern int32_t			ctrlNeedWinLose;
extern int32_t			ctrlNeedBackPoly;
extern int32_t			ctrlNeedRaceInfo;
extern int32_t			ctrlNeedDeSelectScreen;
extern int32_t			ctrlNeedNewHighScores;
extern int32_t			ctrlNeedRacePoints;
extern int32_t			ctrlNeedChampTable;
extern int32_t			ctrlNeedAllVenomComplete;
extern int32_t			ctrlNeedVenomComplete;
extern int32_t			ctrlNeedContinueGameConfirm;
extern int32_t			ctrlNeedEndTextVar;
extern int32_t			ctrlNeedWinLoseVar;
extern int32_t			ctrlNeedInGameReset;
extern int32_t			ctrlNeedInSingleGameConfirm;
extern int32_t			ctrlNeedInGameConfirm;
extern int32_t			ctrlNeedTargetIcon;
extern int32_t			ctrlCDPaused;
extern int32_t			ctrlfirsttime;
extern char			ctrlmachinePaused;
extern int32_t				ctrlcdPause;
extern	int16_t   startIceText;
extern	int16_t   startFireText;
extern Speedo			ctrlspeedo;







#define _SIZE_T_DEFINED_			// PCwipeout
extern int16_t CLUT_R[256];					// PCwipeout
extern int16_t CLUT_G[256];					// PCwipeout
extern int16_t CLUT_B[256];					// PCwipeout
extern char		fred;							// PCwipeout

#define	GsGetVcount()	0		 	// PCwipeout

#define RobTest 0

extern combatData		ctrlpacketData;
extern ConfigData		*ctrlgameData;
extern int32_t				ctrlmode;
extern int32_t				ctrltimeout;
extern POLY_F4			ctrlselectBox[2];
extern DR_MODE			ctrlselectTrans[2];
extern char				ctrlconfirm;
extern char				ctrllConfirm;
extern SPRT			ctrlweaponIcon[2];
extern Texture 			*ctrlweaponIconTextures[NO_OF_WEAPON_TYPES] ;
extern SPRT				ctrllifeIcons[3][2] ;
extern ShipData			*ctrlshipIndex;
extern Object			*ctrlprm[PrmMax];
extern int32_t				ctrlchoice;
extern Object			*ctrlredLight;
extern Object			*ctrldonkey[2];
extern Texture 			*ctrleffectTextures[NO_OF_EFFECT_TEXTURES] ;
extern WeaponShape		*ctrlweaponShapes;
extern Texture			*ctrlwinLose [2] ;
extern POLY_F4			ctrlbackFrame[2];
extern DR_MODE			ctrlbackFrameTrans[2];
extern POLY_FT4			ctrlwinLosePrims[2] ;
extern uint32_t  			ctrltoggleDisplay;
extern int32_t				ctrlCDPaused;
extern int32_t				ctrlplayCredits;
extern WeaponShape    *ctrlweaponShapes;
extern WeaponData		*ctrlweaponIndex;
extern DR_MODE			ctrliconTPage;
extern DR_MODE			ctrlrealTPage;
extern HiScoreData		ctrlhiScoreData;
extern int32_t				inattract;
extern int32_t				confirmRestart;
extern int32_t				ctrlNeedTargetIcon;
extern POLY_FT4			ctrlTarget[2];
extern int16_t 			Shielded;
extern int16_t			CurrTrkNo;

int32_t	ctrl_init;
int32_t	ok_to_interrupt;
int32_t	ctrl_says_quit;
#if 0
GLOS	glo, glo1;
#endif
int32_t	last_frame_count = 0, this_frame_count = 0;
int32_t	CDRestartNeeded;
int32_t	CDloopCheck;
extern	char thieving_git;
uint32_t	start_time;
uint32_t	end_time;
uint32_t	duration;
uint32_t	timeloop;
uint32_t	ctrl_frame;
uint32_t	ctrl_remainder;
int32_t	theFrameRate;
int32_t	fr_count;
int32_t fp;


extern int32_t	NeedKeys;
extern uint16_t	pad_stack[];
extern int32_t		Curpad;

void race( combatData *packetData, ConfigData *gameData, int32_t mode, int32_t timeout )
{
	InitAJSVSync();
	ctrl_init = 0;
	ctrlpacketData = *packetData;
	ctrlgameData = gameData;		//archive off the gamedata;
	ctrlmode = mode;
	ctrltimeout = timeout;

	StopNotes();
	raceinit();
	UpdateEffects(ctrleffects);
	CdControlB(CdlStop, 0, 0);

	CheckIngameError();

	UpdateNotes(NOT_PAUSED);

	CDRestartNeeded = 0;
	ok_to_interrupt = 1;
	CDloopCheck = 0;
	start_time = HowLong();
	end_time = HowLong();
	racectrl();
	racectrl();
	racectrl();
	ctrl_frame = 1;	//exactly one frame
	ctrl_remainder  = 0;
	fr_count = 0;
#ifdef SMOOTH_CONTROL
	while (NeedKeys)
	{
		;
	}
#endif
	Curpad = 0;
	while(1)		//GAME LOOP
	{
		start_time = (HowLong() - ctrl_remainder);
		for (timeloop = 0; timeloop < ctrl_frame; timeloop++)
		{
#ifdef SMOOTH_CONTROL
			while (NeedKeys)
			{
				;
			}
			pad = pad_stack[Curpad--];
			if (Curpad < 0)
				Curpad = 0;
#endif
			racectrl();
		}
		DrawnShieldBefore = 0;		//Reset the shield prim.

		if (ctrl_says_quit)
		{
#if CD_AUDIO
			CdControlB(CdlStop, 0, 0);
#endif
			ok_to_interrupt = 0;
			return;
		}

		if (CDRestartNeeded)
		{
			CDRestart();
			CDRestartNeeded = 0;
			ctrlCDPaused = 0;
		}

		//	if (CDloopCheck++ > 20)
		//		{
		//		CDLoopCheck();
		//		CDloopCheck = 0;
		//		}
#if 0
		_disable();
		copy_glob_from_spare();
		_enable();

		xpad = glo.xpad;	//bodge to try and ensure that xpad is preserved throughout this loop
		pad = glo.pad;	//bodge to try and ensure that xpad is preserved throughout this loop
#endif
#if CD_AUDIO
		switch (ctrlCDPaused)
		{
		case 0:
			break;
		case 1:
			CdPause();
			break;
		case 2:
			CdPlay();
			ctrlCDPaused = 0;
			break;
		}
#endif

		ClearOTagR( OT[ CurrentScreen ], OT_SIZE );

		textPrim = CurrentScreen * 300;

#ifdef FRI
		if (ctrlfadeData.fadeFlag == FADE_ACTIVE || ctrlfadeData.fadeFlag == FADE_OUT)
			ScreenFade (&(ctrlfadeScreen[0]), &(ctrlfadeTrans[0]), &ctrlfadeData) ;
#endif

		if(ctrlmode == ATTRACT)
		{
			inattract=1;	//jed
			if(ctrlplayCredits == 1)
				ShowCredits(attractDelay);
		}


		//	if(trackNo == 13)
		//		AuroraBorialis(ctrlprm[ctrlsky]);
		if((trackNo == 8) || (trackNo == 11))
		{
			ColourRedLight(ctrlredLight);
			MoveDonkey(ctrldonkey);
		}
		else if((trackNo == 1)/* || (trackNo == 6)*/)
		{
			ColourRedLight(ctrlredLight);
		}
		//	ColourDroidLights(ctrlshipShapes[RESCUE_DROID]);

		//WEAPONS
		DrawWeapons(ctrlshipIndex, ctrlweaponShapes, ctrlweaponIndex );
		if (ctrlNeedTargetIcon == 1)
			DrawTargetIcon(&(ctrlshipIndex[ownShip]), ctrlshipIndex, ctrlshipShapes, ctrlTarget);

		//Effects
		AddEffects(ctrleffects, ctrlweaponIndex, ctrleffectTextures, ctrlshipIndex);
		DrawEffects(ctrleffects);



		//LANDSCAPE
		DrawLandscape( &(ctrlshipIndex[ownShip]), &ctrlcamera );
		//SHADOW
		if(ctrlshipIndex[ownShip].attr & RACING)
			DrawShadow(ctrlshipShapes, ctrlshipIndex, ctrlShadow);
		//SHIPS
		DrawShips(ctrlshipIndex, ctrlshipShapes, ownShip, ctrlcamera);


		//TRACK
		if( RobTest || ((ctrlshipIndex[ownShip].attr & RACING) == 0) )
		{
			DrawAllTrack( &ctrlcamera, ctrlautoMesh );
		}
		else
		{
			DrawNewTrack( &ctrlcamera, ctrlautoMesh );
		}

		//DROID
		SetSkeletonPosition( ctrlshipShapes[RESCUE_DROID]->skeleton,
			ctrlrescueDroid.pos.vx,
			ctrlrescueDroid.pos.vy,
			ctrlrescueDroid.pos.vz);

		SetSkeletonDirectionHPR(ctrlshipShapes[RESCUE_DROID]->skeleton,
			ctrlrescueDroid.hdg,
			ctrlrescueDroid.pitch,
			ctrlrescueDroid.roll );

		ctrlshipShapes[RESCUE_DROID]->skeleton->update = 1;
		TransformStaticObject( ctrlshipShapes[RESCUE_DROID], ctrlcamera.camPos);



		//All game overs
		if ((ctrlshowGameOver == 1) || (gameData->showGameOver == 1) || (gameData->lostGame == 1))
			CentreText ("GAME OVER",text_data (60, 112, 16),RedText) ;





		//The Sky
		ctrlprm[ctrlsky]->skeleton->update = 1;
		if (Sky)
			DrawObjectFixedAsm( ctrlprm[ ctrlsky ], ctrlcamera.camPos, 8191 );

		//Text at end.
		if (ctrlNeedEndText)
		{
			switch (ctrlNeedEndTextVar)
			{
			case 1:
				CentreText ("CONGRATULATIONS",text_data (15, 20, 8),RedText) ;
				break;
			case 2:
				CentreText ("BAD LUCK",text_data (15, 20, 8),RedText) ;
				break;
			case 3:
				CentreText ("FAILED TO QUALIFY",text_data (15, 20, 8),RedText) ;
				break;
			default:
				CentreText ("LOST THE VARIABLE",text_data (15, 20, 8),RedText) ;
				break;
			}
		}
		//Pictures at end
		if (ctrlNeedWinLose)
		{
			setWinLose();
			DisplayWinLose (ctrlwinLose[ctrlNeedWinLoseVar], &(ctrlwinLosePrims[0]), 180, 50, 140, RACE) ;
		}
		//High score bollox
		if (ctrlNeedRaceInfo)
			DisplayRaceInfo (gameData, &ctrlhiScoreData) ;
		if (ctrlNeedNewHighScores)
			NewHighScoreDraw (gameData, &ctrlhiScoreData) ;
		if (ctrlNeedRacePoints)
			DisplayRacePoints () ;
		if (ctrlNeedChampTable)
			DisplayChampTable () ;
		if (ctrlNeedAllVenomComplete)
			DisplayAllVenomComplete () ;
		if (ctrlNeedVenomComplete)
			DisplayVenomComplete () ;
		if (ctrlNeedContinueGameConfirm)
			ContinueGameConfirm (&(ctrlhiScoreData.confirm), xpad, &(ctrlhiScoreData.lConfirm), &ctrltoggleDisplay) ;
		if (ctrlNeedInSingleGameConfirm)
			RestartGameConfirm (&(ctrlhiScoreData.confirm), xpad, &(ctrlhiScoreData.lConfirm), &ctrltoggleDisplay) ;
		if (ctrlNeedInGameReset)
			InGameReset(ctrlchoice, &ctrltoggleDisplay, ctrlmachinePaused, gameData, &ctrlcdPause);
		if (ctrlNeedInGameConfirm)
			InGameConfirm (&ctrlconfirm, ctrlchoice, xpad, &ctrllConfirm, &ctrltoggleDisplay) ;
		if (ctrlNeedDeSelectScreen)
			DeSelectScreen (ctrlselectBox, ctrlselectTrans) ;
		if ( startIceText )
		{
			ShowIceCongratulations( );
		}
		if ( startFireText )
		{
			ShowFireCongratulations( );
		}


		//Semi trans bit.
		if (ctrlNeedBackPoly)
			BackPoly (&(ctrlbackFrame[0]), &(ctrlbackFrameTrans[0]), 180, 50, 128, 140) ;


		//HUD display
		if(ctrlshipIndex[ownShip].attr & RACING)
		{
			if (gameData->gameType == CHAMPIONSHIP)
				DisplayLives (&(ctrllifeIcons[0]), gameData) ;

			DrawWeaponSprtIcons(ctrlweaponIcon, ctrlweaponIconTextures, &(ctrlshipIndex[ownShip]),&ctrliconTPage, &ctrlrealTPage );
			DrawHud(ctrlshipIndex, &ctrlspeedo, ctrlchoice, gameData);
		}

		Swap( );
		//do
		//{
		end_time = HowLong();
		//}
		//while((end_time - start_time) <=TIME_CONSTANT);	//never faster than 30fps
		duration = end_time - start_time;
		if (fr_count++ > 5)
		{
			fr_count=0;
			theFrameRate = (1000 / (duration - ctrl_remainder));
		}
		ctrl_frame = duration / TIME_CONSTANT;
		ctrl_remainder = duration - (ctrl_frame * TIME_CONSTANT);


	}/*Main while loop*/
}

void Restart(	ShipData 		*shipIndex,
			 ConfigData 		*gameData,
			 WeaponData 		*weaponIndex,
			 WeaponShape 	*weaponShapes,
			 Effect			*effects,
			 RescueData		*rescueDroid,
			 Object			**shipShapes)
{

	int32_t				i;


	if(winLoseIn)
		JJSLoadSprites();
	Shielded = 0;
	KillNotes();
	StopNotes();
	initShipData(track->sections, shipIndex, gameData);
	InitRescueDroid(rescueDroid, shipShapes, &(shipIndex[ownShip]));
	cameraPtr->section = shipIndex[ownShip].nearTrkSect;
	ResetStartBoom();
	for(i = 0; i < NO_ACTIVE_WEAPONS; i++)
	{
		weaponIndex[i].count = 0;
		weaponIndex[i].type = NO_WEAPON;
	}
	for(i = 0; i < NO_OF_WEAPONS; i++)
	{
		weaponShapes[i].used = 0;
	}
	for(i = 0; i < NO_OF_EFFECTS; i++)
	{
		effects[i].count = 0;
	}


	r_attr.depth.left = 2000;
	r_attr.depth.right = 2000;

	CameraUpdate = UpdateCameraRaceStart;
	CDRestartNeeded = 1;
}



void QuitGame()
{
	int32_t i;

	for(i = 0; i < 10; i++)
	{
		c_attr.cd.volume.left  -= c_attr.cd.volume.left >> 3 ;	/* Master volume (left) */
		c_attr.cd.volume.right -= c_attr.cd.volume.right >> 3;	/* Master volume (right) */
		UpdateNotes(NOT_PAUSED);
	}

	Shielded = 0;
	KillNotes();
	r_attr.depth.left = 2000;
	r_attr.depth.right = 2000;
	return ;
}


void WritePCX(char *fname, bm *BMptr)
{
	FILE	*fp;
	char	*ptr;
	uint8_t	temp_char,match,count;
	uint32_t temp_int;
	uint32_t size,i;

	fp=fopen(fname,"wb");
	if (fp !=NULL)
	{
		temp_char = 10;						// Manifacturers Code
		fwrite(&temp_char, 1, 1, fp);
		temp_char = 5;							// Pcx Version 5
		fwrite(&temp_char, 1, 1, fp);
		temp_char = 1;							// Encoding Always 1 = RLE
		fwrite(&temp_char, 1, 1, fp);
		temp_char = 8;							// Bits Per Pixel 8 ( Bloody Hope So ! )
		fwrite(&temp_char, 1, 1, fp);

		temp_int = 0;
		fwrite(&temp_int, 2, 1, fp);		// X and Y Pos In Ram
		fwrite(&temp_int, 2, 1, fp);

		temp_int=(BMptr->width)-1;
		fwrite(&temp_int, 2, 1, fp);		// Width

		temp_int=(BMptr->height)-1;
		fwrite(&temp_int, 2, 1, fp);		// Height

		temp_int = BMptr->width;			// Hres Size
		fwrite(&temp_int, 2, 1, fp);

		temp_int = BMptr->height;			// Vres Size
		fwrite(&temp_int, 2, 1, fp);

		temp_int = 0;							// Clear 16 Colour Pallete ?? ( Why The Fuck Is It Here ??? )
		i=48/2;
		while(i--)
		{
			temp_int ++;
			fwrite(&temp_int, 2, 1, fp);
		}

		temp_char = 0x00;						// Reserved
		fwrite(&temp_char, 1, 1, fp);

		temp_char = 0x1;						// View Mode Byte
		fwrite(&temp_char, 1, 1, fp);

		temp_int=BMptr->width;				// Bytes Per Line
		fwrite(&temp_int, 2, 1, fp);

		temp_int	= 0x0001;						// Palette Type
		fwrite(&temp_int, 2, 1, fp);

		temp_int = 0;							// Write Reserved Bytes
		i=58/2;
		while (i--)
		{
			fwrite(&temp_int, 2, 1, fp);
		}
		ptr=BMptr->bitmap;					// Write the actual image
		size=(BMptr->height * BMptr->width);

		count=0;
		match=*ptr;

		i=size-1;
		while (i--)
		{
			temp_char=*ptr++;
			if ((temp_char==match) && (count <63))
			{
				count++;
			}
			else
			{
				if ((count==1) && (match <192))
				{
					fwrite(&match,1,1,fp);
				}
				else
				{
					count +=192;
					fwrite(&count, 1, 1, fp);
					fwrite(&match, 1, 1, fp);
				}
				count = 1;
				match=temp_char;
			}
		}

		temp_char = 12;
		fwrite(&temp_char, 1, 1, fp);

		fwrite(BMptr->palette, 1, 768, fp);
		fclose(fp);
	}
}

#if 0
void CdRestart(ConfigData *gameData)
{
	//#if CD_AUDIO
#if 0
	uint8_t			param;
	ok_to_interrupt = 0;
	param = CdlModeRept|CdlModeDA;
	CdControlB(CdlStop, 0, 0);
	CdControlB(CdlSetmode, &param, 0);

	if(gameData->cdTrack == 1)
	{
		//	   CdControlB(CdlSetloc, (uint8_t *)&loc[(rand()%(ntoc-1)) + 2], 0);	// DLS
		CurrTrkNo = (rand() % (ntoc - 1)) + 3;
	}
	else
	{
		//	   CdControlB(CdlSetloc, (uint8_t *)&loc[gameData->cdTrack], 0);		// DLS
		CurrTrkNo = gameData->cdTrack;
	}

	CdControlB(CdlPlay, 0, 0);
	ok_to_interrupt = 1;
#endif
}
#endif
void CheckIngameError()
{
	static int32_t toggle = 0;
	while (1)
	{
		if (thieving_git)
		{
			opad=pad;
			pad = neg1.bits;
			if((neg1.head != 0xff))
				pad = ~pad;
			xpad=(pad^opad)&pad;				//xpad is difference between this frames

			ClearOTagR( OT[ CurrentScreen ], OT_SIZE );
			textPrim = CurrentScreen * 300;
			CentreText ("PLEASE INSERT ORIGINAL",text_data (20, 60, 8),RedText);
			CentreText ("WIPEOUT PC CD ROM",text_data (20, 72, 8),RedText);

			CentreText ("PRESS RETURN WHEN READY",text_data (20, 120, 8),(toggle++)&1);

			Swap();
		}
		else
		{
			break;
		}
	}
}
