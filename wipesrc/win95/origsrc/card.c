#include "standard.h"

//#include	<sys\file.h>
#define		LANGUAGE_C
#include	<asm.h>
#include	<kernel.h>
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "wtl.h"
#include "combat.h"
#include "menus.h"
#include "hud.h"
#include "droid.h"
#include "camera.h"
#include "fileops.h"
#include "hiscores.h"
#include "card.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
 
#define _SIZE_T_DEFINED_
#include	"w:\h\stdio.h"
#include	"markk\input.h"


extern 	HiScores	scoreTable[] ;

unsigned long ev0,ev1,ev2,ev3;
unsigned long ev10,ev11,ev12,ev13;
extern char twist ;
extern char negtwist ;
Texture *gameIcon ;

extern char mouse_sensitivity;
extern char controller;
//extern char	Sky;
extern char	ScreenSize;
extern char	Resolution;
extern long	DrawDistance;

extern unsigned short OriginalKeyCodes[16];

extern short JoyMinX, JoyMaxX;
extern short JoyMinY, JoyMaxY;
extern short JoyCentreX, JoyCentreY;
extern short left_scale, right_scale;
extern float left_scale_factor, right_scale_factor;



//#define SAVEDIR			"wipeout\\"
#define	FILE_EXTENSION	".wsf"
extern char *SaveDirBase;

#ifndef USE_ATI


void InitCardEvents ()
{
#ifdef	PCwipeout
	ev0 = OpenEvent(SwCARD, EvSpIOE, EvMdNOINTR, NULL);
	ev1 = OpenEvent(SwCARD, EvSpERROR, EvMdNOINTR, NULL);
	ev2 = OpenEvent(SwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
	ev3 = OpenEvent(SwCARD, EvSpNEW, EvMdNOINTR, NULL);
	ev10 = OpenEvent(HwCARD, EvSpIOE, EvMdNOINTR, NULL);
	ev11 = OpenEvent(HwCARD, EvSpERROR, EvMdNOINTR, NULL);
	ev12 = OpenEvent(HwCARD, EvSpTIMOUT, EvMdNOINTR, NULL);
	ev13 = OpenEvent(HwCARD, EvSpNEW, EvMdNOINTR, NULL);

	EnableEvent (ev0) ;
	EnableEvent (ev1) ;
	EnableEvent (ev2) ;
	EnableEvent (ev3) ;
	EnableEvent (ev10) ;
	EnableEvent (ev11) ;
	EnableEvent (ev12) ;
	EnableEvent (ev13) ;
#endif // PCwipeout
}

#endif //USE_ATI

long GetCardInfo (int cardNum)
{
#ifdef	PCwipeout
	long 	cardStatus = 0 ;

	if (cardNum == 0)
		_card_info (0) ;
	else
		_card_info (16) ;

	cardStatus = CheckCardEvents () ;
	return (cardStatus) ;
#endif // PCwipeout
	return (0);	// PCwipeout
} 

#ifndef USE_ATI


long CheckCardEvents ()
{
#ifdef PCwipeout
	while(1) 
	{
		if(TestEvent(ev0)==1) 
		{         /* IOE */
			return 0;	   
		}

		if(TestEvent(ev1)==1) 
		{         /* ERROR */
			return 1;
		}

		if(TestEvent(ev2)==1) 
		{         /* TIMEOUT */
			return 2;
		}

		if(TestEvent(ev3)==1) 
		{         /* NEW CARD */
			return 3;
		}
	}
#endif //	PCwipeout
return(0); // PCwipeout
}

#endif //USE_ATI

long CheckHWCardEvents ()
{
#ifdef	PCwipeout
	while(1) 
	{
		if(TestEvent(ev10)==1) 
		{         /* IOE */
			return 0;
		}

		if(TestEvent(ev11)==1) 
		{         /* ERROR */
			return 1;
		}

		if(TestEvent(ev12)==1) 
		{         /* TIMEOUT */
			return 2;
		}

		if(TestEvent(ev13)==1) 
		{         /* NEW CARD */
			return 3;
		}
	}
#endif //	PCwipeout
return(0); // PCwipeout
}



void ClearEventHW ()
{
#ifdef	PCwipeout
	TestEvent (ev10) ;
	TestEvent (ev11) ;
	TestEvent (ev12) ;
	TestEvent (ev13) ;
#endif // PCwipeout
}

#ifndef USE_ATI


void ClearEventSW ()
{
#ifdef PCwipeout
	TestEvent (ev0) ;
	TestEvent (ev1) ;
	TestEvent (ev2) ;
	TestEvent (ev3) ;
#endif // PCwipeout
}

#endif //USE_ATI


long CardContentStatus ()
{
#ifdef PCwipeout
	long cardStatus = 0 ;

	cardStatus = CheckCardEvents () ;	
	
	return (cardStatus) ;
#endif // PCwipeout	
return(0); // PCwipeout
}

void LoadCard (int cardNum)
{
#ifdef	PCwipeout
	ClearEventSW () ;
	if (cardNum == 0)
		_card_load (0) ;
	else
		_card_load (16) ;
#endif // PCwipeout
}

#ifndef USE_ATI


long UnFormat (long cardNum)
{
#ifdef	PCwipeout
	long i;
	char buf[128];

	for(i=0; i < 128; i++)
		buf[i] = 0xff;

	for(i=0;i<1024;i++) 
	{
		ClearEventHW () ;
		_new_card();
		_card_write(cardNum,i,&buf[0]);

		if((CheckHWCardEvents ()) != 0)
			return 0;

	}
#endif // PCwipeout
	return 1;
}
#endif //USE_ATI


#ifndef USE_ATI



long QuickUnFormat (long cardNum)
{
#ifdef	PCwipeout
	char buf[128];

	buf[0] = buf[1] = 0xff;
	ClearEventHW () ;
	_new_card();
	_card_write(cardNum,0,&(buf[0]));
	if(CheckHWCardEvents () != 0)
		return 0;

#endif	// PCwipeout
	return 1;
}

#endif //USE_ATI

char SaveCardFile (char *saveName, int cardNum, ConfigData *gameData, short sliderVol)
{
	FILE 	*fd;
	char	fileName[250];
	char	header[1];

	strcpy(fileName, SaveDirBase);
	strcat(fileName, "\\");
	strcat(fileName, saveName);
	strcat(fileName, FILE_EXTENSION);

	if ( !WriteFile (&(header[0]), &(fileName[0]), gameData, sliderVol) )
		return (0) ;

	return (1) ;
}

#ifndef USE_ATI


char RenameFile1 (char *saveName, char *oldName, int cardNum, ConfigData *gameData, short sliderVol)
{
#ifdef	PCwipeout
	char origName[260], newName[260] ;
	char	header[(128*4)] ;


	MakeFileName (oldName, &(origName[0]), cardNum) ;

	MakeFileName (saveName, &(newName[0]), cardNum) ;

	GetHeader (&(header[0])) ;

	if ((rename (origName, newName)) == 0)
	{
		printf ("Error during rename operation \n") ;
		return (0) ;
	}
	
	if ( !WriteFile (&(header[0]), &(newName[0]), gamedData, sliderVol) )
		return (0) ;

#endif // PCwipeout	
	return (1) ;
}
#endif //USE_ATI

#ifndef USE_ATI


char RenameFile (char *saveName, int cardNum, ConfigData *gameData, short sliderVol) 
{
#ifdef PCwipeout
	char	header[(128*4)] ;
	char	fileName[260] ;

	GetHeader (&(header[0])) ;
	MakeFileName (saveName, &(fileName[0]), cardNum) ;
	if ( !WriteFile (&(header[0]), &(fileName[0]), gameData, sliderVol) )
		return (0) ;

#endif // PCwipeout
	return (1) ;
}

#endif //USE_ATI



#if 1
void GetHeader (char *header) 
{
	
}	
#else


#ifndef USE_ATI


void GetHeader (char *header) 
{
#ifdef PCwipeout
	short i ;
	Texture	*gameIcon ;
	RECT		iconData ;

	gameIcon = LoadTexture ("wipeout\\textures\\icon.tim", 1) ;

	for (i=0; i < (128*2); i++)
		header[i] = 0 ;

	header[0] = 'S' ;
	header[1] = 'C' ;
	header[2] = 0x11 ;
	header[3] = 1 ;

	sprintf ((&header[4]), "‚o‚r‚x‚f‚m‚n‚r‚h‚r@‚v‚h‚o‚d‚n‚t‚s@") ;

	/*	Assign icon clut to save game */

	iconData.x = gameIcon->clutX ;
	iconData.y = gameIcon->clutY ;
	iconData.w = gameIcon->clutW ;
	iconData.h = gameIcon->clutH ;
	
	StoreImage (&iconData, (ulong *)(&header[(128-32)])) ;
	DrawSync (0) ;

	/*	Assign icon to save game */

	iconData.x = gameIcon->textureX ;
	iconData.y = gameIcon->textureY ;
	iconData.w = gameIcon->textureW ;
	iconData.h = gameIcon->textureH ;
					
	StoreImage (&iconData, (ulong *)(&header[128])) ;
	DrawSync (0) ;

#endif // PCwipeout
 }
#endif
#endif //USE_ATI

#ifndef USE_ATI


void MakeFileName(char *saveName, char *fileName, int cardNum)
{
#ifdef PCwipeout
#if DEBUG
	if (cardNum == 0)
		strcpy (fileName, "bu00:BISLPS-00001WO") ;
	else
		strcpy (fileName, "bu10:BISLPS-00001WO") ;
#else
#if EUROPE
	if (cardNum == 0)
		strcpy (fileName, "bu00:BESCES-00010WO") ;
	else
		strcpy (fileName, "bu10:BESCES-00010WO") ;
#endif
#if US
	if (cardNum == 0)
		strcpy (fileName, "bu00:BASCUS-94301WO") ;
	else
		strcpy (fileName, "bu10:BASCUS-94301WO") ;
#endif

#if JAPAN
	if (cardNum == 0)
		strcpy (fileName, "bu00:BISCIS-94301WO") ;
	else
		strcpy (fileName, "bu10:BISCIS-94301WO") ;
#endif

#endif
	strcat (fileName, saveName) ;
#endif
}
#endif //USE_ATI


#ifndef USE_ATI

  
char CreateFile (char *fileName)
{
#ifdef PCwipeout
	int		fd ;

	printf ("output filename = %s \n",fileName) ;

	fd = open( fileName, O_CREAT|(1<<16) );		
	if (fd < 0) 
	{
		printf ("Error creating file \n") ;
		return (0);
	}
	close( fd );

#endif // PCwipeout
	return (1) ;
}
#endif //USE_ATI


char WriteFile (char *header, char *fileName, ConfigData *gameData, short sliderVol)
{
	FILE	*fd;
	int		temp;
	unsigned char	saveBuffer[256] ;
	short			*short_ptr ;
	unsigned short	*ushort_ptr;
	unsigned char	*char_ptr ;
	long			*long_ptr;


 	if((fd = fopen(fileName, "wb")) == NULL)
		return (0) ;
	else
	{
		temp = fwrite(scoreTable, ((sizeof(HiScores)) * (HiScoreTracks*2)), 1, fd);

		if (temp != 1)
			return (0) ;

		short_ptr = (short *) &(saveBuffer[0]);
		*short_ptr ++ = sliderVol;

		*short_ptr++ = JoyMinX;
		*short_ptr++ = JoyMaxX;
		*short_ptr++ = JoyMinY;
		*short_ptr++ = JoyMaxY;
		*short_ptr++ = JoyCentreX;
		*short_ptr++ = JoyCentreY;
		*short_ptr++ = left_scale;
		*short_ptr++ = right_scale;

	// Keys
		ushort_ptr = (unsigned short *)short_ptr;
		*ushort_ptr ++ = OriginalKeyCodes[15];		// Left
		*ushort_ptr ++ = OriginalKeyCodes[13];		// Right
		*ushort_ptr ++ = OriginalKeyCodes[12];		// Up
		*ushort_ptr ++ = OriginalKeyCodes[14];		// Down
		*ushort_ptr ++ = OriginalKeyCodes[0];		// Brake Left
		*ushort_ptr ++ = OriginalKeyCodes[1];		// Brake Right
		*ushort_ptr ++ = OriginalKeyCodes[6];		// Accel
		*ushort_ptr ++ = OriginalKeyCodes[5];		// Fire
		*ushort_ptr ++ = OriginalKeyCodes[4];		// Change View
		*ushort_ptr ++ = OriginalKeyCodes[11];		// Pause (start)

		char_ptr = (unsigned char *)ushort_ptr;
		*char_ptr ++ = controller;
		*char_ptr ++ = mouse_sensitivity;
		*char_ptr ++ = gameData->RapierClass;
		*char_ptr ++ = gameData->cdTrack;
		*char_ptr ++ = gameData->negTwist;
		*char_ptr ++ = gameData->showBonusTrack;
		*char_ptr ++ = NumTracks;
		*char_ptr ++ = totalTracks;

		//*char_ptr ++ = Sky;
		*char_ptr ++ = ScreenSize;
		*char_ptr ++ = Resolution;
	
		long_ptr = (long *)char_ptr;
		*long_ptr++ = DrawDistance;
		*long_ptr++ = TextureTrack;
		
		temp = fwrite(saveBuffer, sizeof(saveBuffer), 1, fd) ;

		if (temp != 1)
			return (0) ;

		fclose(fd);
		return (1) ;
	}
}



void LoadCardFiles (SelectionData *selectData, char loadGames[][9], int cardNum)
{
#ifdef PCwipeout
 	int		ret ;
	struct	DIRENTRY 	fileDir ;
	char		i, j ;


	for (i=0; i < MAX_GAMES; i++)
	{
		loadGames[i][0] = '\0' ;
	}

	selectData->numCardFiles = 0 ;

	if (cardNum == 0)
		ret = firstfile ("bu00:*", &fileDir) ;
	
	if (cardNum == 1)
		ret = firstfile ("bu10:*", &fileDir) ;
		

	if (ret == 0)		/* no file or an error */
	{
/*		format ("bu00:") ;*/
//		printf ("Format card \n") ;
		selectData->slotsFilled = 0 ;
		selectData->numCardFiles = 0 ;
	}
	else					/* files exist on card so find our WipeOut ones */
	{
		selectData->slotsFilled = 0 ;

		do {

/*
				printf ("filename = %s \n", (&fileDir.name[7])) ;
 				printf ("file size = %d \n", fileDir.size) ;
*/

#if DEBUG
				if ( (strncmp ((&fileDir.name[7]), "00001WO", 7)) == 0)
#else
#if EUROPE
				if ( (strncmp ((&fileDir.name[7]), "00010WO", 7)) == 0)
#else
				if ( (strncmp ((&fileDir.name[7]), "94301WO", 7)) == 0)
#endif
#endif
				{
  /*					printf ("Matched file name \n") ;*/

					loadGames[(selectData->slotsFilled)][0] = '\0' ;

					for (i=0,j=14; i < 6; i++, j++)
					{
						loadGames[(selectData->slotsFilled)][i] = fileDir.name[j] ;
				 	
					}

					loadGames[(selectData->slotsFilled)][6] = '\0' ;

/*					printf ("loadSave filename = %s \n",loadGames[(selectData->slotsFilled)] ) ;*/
					
					selectData->slotsFilled ++ ;
				
					
				} 
		
				
/*				selectData->numCardFiles ++ ;*/
				selectData->numCardFiles += (fileDir.size / 8192) ;

		} while (nextfile (&fileDir)) ;		

	}

	#endif // PCwipeout
//	printf ("Number of slots used = %d \n", selectData->numCardFiles) ;

	
}



char LoadCardData (char *loadName, int cardNum, ConfigData *gameData, SelectionData *selectData)
{
	FILE	*fd;
	int		temp;
	char	fileName[250];
	unsigned char	loadBuffer[256] ;
	short			*short_ptr ;
	unsigned short	*ushort_ptr ;
	unsigned char	*char_ptr ;
	long			*long_ptr;


	strcpy(fileName, SaveDirBase);
	strcat(fileName, "\\");
	strcat(fileName, loadName);
	strcat(fileName, FILE_EXTENSION);

	if((fd = fopen(fileName, "rb")) == NULL)
	{
		printf ("Error opening file for read \n") ;
		return (0) ;
	}
	else
	{
		temp = fread(scoreTable, (sizeof (HiScores) * (HiScoreTracks*2)), 1, fd);

		if (temp != 1)
		{
			printf ("Error reading data from file\n") ;
			return (0) ;
		}

		temp = fread(loadBuffer, sizeof(loadBuffer), 1, fd);

		short_ptr = (short *) &(loadBuffer[0]);
		gameData->currSliderPos = *short_ptr ++;
		SfxSetVolume((char)((0x3fff - (75 * gameData->currSliderPos))>>6));
#ifdef PORT_SOUND
	   SfxSetCdVolume(ConvCDVol(150*gameData->currSliderPos,150*gameData->currSliderPos));
#endif

		JoyMinX = *short_ptr++;
		JoyMaxX = *short_ptr++;
		JoyMinY = *short_ptr++;
		JoyMaxY = *short_ptr++;
		JoyCentreX = *short_ptr++;
		JoyCentreY = *short_ptr++;
		left_scale = *short_ptr++;
		right_scale = *short_ptr++;

// Read the keys back
		ushort_ptr = (unsigned short *)short_ptr;
		OriginalKeyCodes[15] = *ushort_ptr++;		// Left
		OriginalKeyCodes[13] = *ushort_ptr++;		// Right
		OriginalKeyCodes[12] = *ushort_ptr++;		// Up
		OriginalKeyCodes[14] = *ushort_ptr++;		// Down
		OriginalKeyCodes[0] = *ushort_ptr++;		// Brake Left
		OriginalKeyCodes[1] = *ushort_ptr++;		// Brake Right
		OriginalKeyCodes[6] = *ushort_ptr++;		// Accel
		OriginalKeyCodes[5] = *ushort_ptr++;		// Fire
		OriginalKeyCodes[4] = *ushort_ptr++;		// Change View
		OriginalKeyCodes[11] = *ushort_ptr++;		// Pause (start)

		char_ptr = (unsigned char *)ushort_ptr;
		controller = *char_ptr ++; 	
		mouse_sensitivity = *char_ptr ++; 	
		gameData->RapierClass = *char_ptr ++; 	
		gameData->cdTrack = *char_ptr ++;
		gameData->negTwist = *char_ptr++;
		gameData->showBonusTrack = *char_ptr++;
		NumTracks = *char_ptr++;
		totalTracks = *char_ptr++;

		//Sky = *char_ptr++;
		ScreenSize = *char_ptr++;
		Resolution = *char_ptr++;

		long_ptr = (long *)char_ptr;
		DrawDistance = *long_ptr++;
		TextureTrack = *long_ptr++;

// Load done, set the controller and sound up

		left_scale_factor = 128.0 / (float)left_scale;
		right_scale_factor = 128.0 / (float)right_scale;

		etc_InputDeviceSelect = controller;

		if(etc_InputDeviceSelect != _KEYBOARD)
			StartPAD();

// Clear the main keys
		etc_KeyboardMap[etc_KeyboardConfig[15]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[15] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[15]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[15] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[13]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[13] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[13]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[13] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[12]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[12] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[12]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[12] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[14]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[14] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[14]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[14] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[0]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[0] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[0]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[0] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[1]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[1] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[1]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[1] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[6]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[6] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[6]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[6] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[5]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[5] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[5]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[5] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[4]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[4] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[4]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[4] + 128] = 0xFFFFFFFF;

		etc_KeyboardMap[etc_KeyboardConfig[11]] = 0;
		//etc_KeyboardMap[etc_KeyboardConfig[11] + 128] = 0xFFFFFFFF;
		etc_ExtendedKeyboardMap[etc_KeyboardConfig[11]] = 0;
		//etc_ExtendedKeyboardMap[etc_KeyboardConfig[11] + 128] = 0xFFFFFFFF;

		etc_KeyboardConfig[15] = 0;		// Left
		etc_KeyboardConfig[13] = 0;		// Right
		etc_KeyboardConfig[12] = 0;		// Up
		etc_KeyboardConfig[14] = 0;		// Down
		etc_KeyboardConfig[0] = 0;		// Brake Left
		etc_KeyboardConfig[1] = 0;		// Brake Right
		etc_KeyboardConfig[6] = 0;		// Accel
		etc_KeyboardConfig[5] = 0;		// Fire
		etc_KeyboardConfig[4] = 0;		// Change View
		etc_KeyboardConfig[11] = 0;		// Pause (start)

// Set the new keys up
		SetKey(15, OriginalKeyCodes[15]);
		SetKey(13, OriginalKeyCodes[13]);
		SetKey(12, OriginalKeyCodes[12]);
		SetKey(14, OriginalKeyCodes[14]);
		SetKey(0,  OriginalKeyCodes[0]);
		SetKey(1,  OriginalKeyCodes[1]);
		SetKey(6,  OriginalKeyCodes[6]);
		SetKey(5,  OriginalKeyCodes[5]);
		SetKey(4,  OriginalKeyCodes[4]);
		SetKey(11, OriginalKeyCodes[11]);

		twist = gameData->negTwist;
		negtwist = gameData->negTwist;

	  	c_attr.cd.volume.left  = 150 * gameData->currSliderPos;	/* Master volume (left) */
	  	c_attr.cd.volume.right  = 150 * gameData->currSliderPos;	/* Master volume (right) */
	  	SpuSetCommonAttr (&c_attr);
		SfxSetVolume((char)((0x3fff - (75 * gameData->currSliderPos))>>6));

		fclose(fd);
		return (1);
	}
}

