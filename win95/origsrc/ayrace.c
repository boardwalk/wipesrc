#define TIME_CONSTANT	33
#include <libspu.h> 								  
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
#include "hud.h"
#include "sound.h"
#include "editor.h"
#include "texplace.h"
#include "trakview.h"
#include "effects.h"
#include "fileops.h"
#include "sparks2.h"
#include "hiscores.h"
#include "options.h"
#include "global.h"
#include "sprite.h"
#include "cone.h"
#include "race.h"
#include "trackasm.h"
#include "progress.h"
#include "spline.h"
#include "vramsize.h" // PCwipeout
#include	"..\andyy\pallette.h"
#include "fri.h"

#define	_SIZE_T_DEFINED_
//#include <stdlib.h>

#include "markk\input.h"

#define START_EXTRO 0
#define UPDATE_EXTRO 1
#define DONE_EXTRO 2

#define PrmMax       64

#define SHIP_DEBUG 0

// extern u_long __bss, __bsslen, _ramsize, _stacksize;
extern long fr ;
extern ChampionShip	winTable[] ;
extern char winLoseIn;

typedef struct {
	int	width;
	int	height;
	int	vram_xpos;
	int	vram_ypos;
	char	*bitmap;
	char	*palette;
} bm;
void	WritePCX(char *fname, bm *BMptr);			// Thanks Jed (even if it is a bit flakey)
void	DumpVRamAsPCX(char *filename);

extern	u_char			ctrlparam;

char Sky = 1;
extern	char* VRam;					// PCwipeout

extern short  			ctrlsky;
extern Object			*ctrlshipShapes[];
extern Effect			*ctrleffects ;
extern TrackCamera		ctrlcamera;
extern Shadow			*ctrlShadow;
extern AutoMeshData   *ctrlautoMesh; 
extern RescueData		ctrlrescueDroid;
extern char			ctrlshowGameOver;
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
extern char			ctrlmachinePaused;
extern int				ctrlcdPause;
extern	short   startIceText;
extern	short   startFireText;
extern Speedo			ctrlspeedo;







#define _SIZE_T_DEFINED_			// PCwipeout
#include "c:\wc10\h\stdio.h"		// PCwipeout
extern short CLUT_R[256];					// PCwipeout
extern short CLUT_G[256];					// PCwipeout
extern short CLUT_B[256];					// PCwipeout
extern char		fred;							// PCwipeout

#define	GsGetVcount()	0		 	// PCwipeout

#define RobTest 0

extern combatData		ctrlpacketData;
extern ConfigData		*ctrlgameData;
extern long				ctrlmode;
extern long				ctrltimeout;
extern POLY_F4			ctrlselectBox[2];
extern DR_MODE			ctrlselectTrans[2];
extern char				ctrlconfirm;
extern char				ctrllConfirm;
extern POLY_FT4			ctrlweaponIcon[2];
extern Texture 			*ctrlweaponIconTextures[NO_OF_WEAPON_TYPES] ;
extern SPRT				ctrllifeIcons[3][2] ;
extern ShipData			*ctrlshipIndex;
extern Object			*ctrlprm[PrmMax];
extern int				ctrlchoice;
extern Object			*ctrlredLight;
extern Object			*ctrldonkey[2];
extern Texture 			*ctrleffectTextures[NO_OF_EFFECT_TEXTURES] ;
extern WeaponShape		*ctrlweaponShapes;
extern Texture			*ctrlwinLose [2] ;
extern POLY_F4			ctrlbackFrame[2];
extern DR_MODE			ctrlbackFrameTrans[2];
extern POLY_FT4			ctrlwinLosePrims[2] ;
extern ulong  			ctrltoggleDisplay;
extern long				ctrlCDPaused;
extern int				ctrlplayCredits;
extern WeaponShape    *ctrlweaponShapes;			 
extern WeaponData		*ctrlweaponIndex; 
extern DR_MODE			ctrliconTPage;
extern DR_MODE			ctrlrealTPage;
extern HiScoreData		ctrlhiScoreData;
extern int				inattract;
extern long				confirmRestart;
extern long				ctrlNeedTargetIcon;
extern POLY_FT4			ctrlTarget[2];
extern short 			Shielded;
extern short			CurrTrkNo;

long	ctrl_init;
long	ok_to_interrupt;
long	ctrl_says_quit;
#if 0
GLOS	glo, glo1;
#endif
long	last_frame_count = 0, this_frame_count = 0;
long	CDRestartNeeded;
long	CDloopCheck;
extern	char thieving_git;
unsigned long	start_time;
unsigned long	end_time;
unsigned long	duration;
unsigned long	timeloop;
unsigned long	ctrl_frame;
unsigned long	ctrl_remainder;
long	theFrameRate;
long	fr_count;
int fp;


extern long	NeedKeys;
extern unsigned short	pad_stack[];
extern int		Curpad;

void race( combatData *packetData, ConfigData *gameData, int mode, int timeout )
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

	BlackPig();
	CheckIngameError();

	UpdateNotes(NOT_PAUSED);

#if CD_AUDIO

		  	c_attr.cd.volume.left  = 150 * gameData->currSliderPos;	/* Master volume (left) */
		  	c_attr.cd.volume.right  = 150 * gameData->currSliderPos;	/* Master volume (right) */
		  	SpuSetCommonAttr (&c_attr);
#ifdef PORT_SOUND
    		SfxSetCdVolume(ConvCDVol(c_attr.cd.volume.left,c_attr.cd.volume.right));
#endif

			ctrlparam = CdlModeRept|CdlModeDA;	
			CdControlB(CdlSetmode, &ctrlparam, 0);	

			if(gameData->cdTrack == 1)
			{
//			   CdControlB(CdlSetloc, (u_char *)&loc[(rand()%(ntoc-1)) + 2], 0);	// DLS
				CurrTrkNo = (rand() % (ntoc - 1)) + 2;
			}
			else
			{
//			   CdControlB(CdlSetloc, (u_char *)&loc[gameData->cdTrack], 0);		// DLS
				CurrTrkNo = gameData->cdTrack;
			}

			CdControlB(CdlPlay, 0, 0);  
#endif

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
		theFrameRate = (1000 / (duration - ctrl_remainder)) + 2;
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

	u_char			param;
	int				i;


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
	SpuSetReverbDepth(&r_attr);

	CameraUpdate = UpdateCameraRaceStart;	 
	CDRestartNeeded = 1;
}



void QuitGame()
{
	int i;

	for(i = 0; i < 10; i++)
	{
//		VSync(0);
   	c_attr.cd.volume.left  -= c_attr.cd.volume.left >> 3 ;	/* Master volume (left) */
   	c_attr.cd.volume.right -= c_attr.cd.volume.right >> 3;	/* Master volume (right) */
   	SpuSetCommonAttr (&c_attr);
#ifdef PORT_SOUND
    SfxSetCdVolume(ConvCDVol(c_attr.cd.volume.left,c_attr.cd.volume.right));
#endif
		UpdateNotes();
	}

	Shielded = 0;
	KillNotes();
//#if CD_AUDIO
//	CdControlB(CdlStop, 0, 0);  
//#endif
	r_attr.depth.left = 2000;
	r_attr.depth.right = 2000;
	SpuSetReverbDepth(&r_attr);
//	VSync(0);
	return ;
}

#ifndef USE_ATI


void	JedDumpVRam(char *filename, char *Pbuff, int widthr, int heightr, int RamWidth)
{
FILE		*newfile;
char		*FileBuff;

int		w1,w2;
int		cc,ct;
int		xl,yl;


	printf("Hello\n");

	FileBuff=(char *)malloc(65536);
	if (FileBuff==NULL) 
	{
		fprintf(stderr,"Error Allocating FileBuff\n");
		exit(0);
	}

	cc=0;
	newfile=fopen(filename,"wb");
	if (newfile==NULL)
	{
		fprintf(stderr,"Can't open file\n");
		exit(0);
	}

	FileBuff[cc++]=0x6d;	// The Header For A Raw is $6d6877616e680004
	FileBuff[cc++]=0x68;
	FileBuff[cc++]=0x77;
	FileBuff[cc++]=0x61;
	FileBuff[cc++]=0x6e;
	FileBuff[cc++]=0x68;
	FileBuff[cc++]=0x00;
	FileBuff[cc++]=0x04;	// Header Ends

	w1=widthr/256;
	w2=widthr&255;

	FileBuff[cc++]=w1;		// Convert Width To BigEndFormat
	FileBuff[cc++]=w2;

	w1=heightr/256;
	w2=heightr&255;

	FileBuff[cc++]=w1;
	FileBuff[cc++]=w2;

	FileBuff[cc++]=1;
	FileBuff[cc++]=0;

	FileBuff[cc++]=0xff;
	FileBuff[cc++]=0xfa;

	FileBuff[cc++]=0xff;
	FileBuff[cc++]=0xfb;

	for (ct=0;ct<14;ct++)
	{
		FileBuff[cc++]=0;
	}
	
	for (ct=0;ct<256;ct++)
	{
		FileBuff[cc++]=CLUT_R[ct];
		FileBuff[cc++]=CLUT_G[ct];
		FileBuff[cc++]=CLUT_B[ct];
	}
	
	fwrite(FileBuff,1,cc,newfile);

	for (yl=0;yl<heightr;yl++)
	{
		 fwrite(Pbuff+(yl*RamWidth),1,widthr,newfile);
	}

	fclose(newfile);
	free(FileBuff);
	exit(0);
}

#endif //USE_ATI

#ifndef USE_ATI


void	DumpVRam16bit(char *filename,char *Pbuff)
{
	FILE	*newfile;
	unsigned long count;

	printf("Dumping 16bit stuff\n");

	newfile=fopen(filename,"wb");
	if (newfile==NULL)
	{
		fprintf(stderr,"Can't open file\n");
		exit(0);
	}

	fwrite(Pbuff,VRAM_SIZE,1024,newfile);

	fclose(newfile);
	ResetVMode();
	exit(0);
}

#endif //USE_ATI

#ifndef USE_ATI


void	DumpVRamAsPCX(char *filename)
{
	char command[256];

	bm	Bitmap;
	Bitmap.width=256;
	Bitmap.height=8192;
	Bitmap.bitmap=(char *)&VRam;
	Bitmap.palette=main_palette;
	WritePCX("wipeout\\temp.pcx",&Bitmap);
	sprintf(command,"alchemy -p -o wipeout\\temp.pcx %s",filename);
	system(command);
}

#endif //USE_ATI

void WritePCX(char *fname, bm *BMptr)
{
	FILE	*fp;
	char	VGA_pal[768];				// pallette
	char	*ptr;
	unsigned char	temp_char,match,count;
	unsigned int temp_int;
	unsigned long size,i,cc;

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
	u_char			param;
ok_to_interrupt = 0;
	param = CdlModeRept|CdlModeDA;	
	CdControlB(CdlStop, 0, 0);  
	CdControlB(CdlSetmode, &param, 0);	

	if(gameData->cdTrack == 1)
	{
//	   CdControlB(CdlSetloc, (u_char *)&loc[(rand()%(ntoc-1)) + 2], 0);	// DLS
		CurrTrkNo = (rand() % (ntoc - 1)) + 3;
	}
	else
	{
//	   CdControlB(CdlSetloc, (u_char *)&loc[gameData->cdTrack], 0);		// DLS
		CurrTrkNo = gameData->cdTrack;
	}

	CdControlB(CdlPlay, 0, 0);  
ok_to_interrupt = 1;
#endif
}
#endif
void CheckIngameError()
{
	static int toggle = 0;
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
			if (xpad)
			{
				BlackPig();
				BlackPig();
			}
		}
		else
		{
			break;
		}
	}
}
	
