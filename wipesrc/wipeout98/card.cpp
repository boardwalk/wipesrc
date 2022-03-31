#include "standard.h"
#include <string.h>
//#include	<sys\file.h>
#define		LANGUAGE_C
#include	"wipesrc/psx26/include/asm.h"
#include	"wipesrc/psx26/include/kernel.h"
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
#include "hiscores.h"
#include "hud.h"
#include "droid.h"
#include "camera.h"
#include "fileops.h"
#include "card.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "sfx.h"
#include "config.h"

#define _SIZE_T_DEFINED_
#include	<stdio.h>
#include	"../win95/markk/input.h"


extern 	HiScores	scoreTable[] ;

uint32_t ev0,ev1,ev2,ev3;
uint32_t ev10,ev11,ev12,ev13;
extern char twist ;
extern char negtwist ;
Texture *gameIcon ;

extern char mouse_sensitivity;
extern char controller;
//extern char	Sky;
extern char	ScreenSize;
extern char	Resolution;
extern int32_t	DrawDistance;

extern uint16_t OriginalKeyCodes[16];

extern int16_t JoyMinX, JoyMaxX;
extern int16_t JoyMinY, JoyMaxY;
extern int16_t JoyCentreX, JoyCentreY;
extern int16_t left_scale, right_scale;
extern float left_scale_factor, right_scale_factor;



//#define SAVEDIR			"wipeout/"
#define	FILE_EXTENSION	".wsf"
extern char *SaveDirBase;


int32_t GetCardInfo (int32_t cardNum)
{
#ifdef	PCwipeout
	int32_t 	cardStatus = 0 ;

	if (cardNum == 0)
		_card_info (0) ;
	else
		_card_info (16) ;

	cardStatus = CheckCardEvents () ;
	return (cardStatus) ;
#endif // PCwipeout
	return (0);	// PCwipeout
}


int32_t CheckHWCardEvents ()
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

int32_t CardContentStatus ()
{
#ifdef PCwipeout
	int32_t cardStatus = 0 ;

	cardStatus = CheckCardEvents () ;

	return (cardStatus) ;
#endif // PCwipeout
return(0); // PCwipeout
}

void LoadCard (int32_t cardNum)
{
#ifdef	PCwipeout
	ClearEventSW () ;
	if (cardNum == 0)
		_card_load (0) ;
	else
		_card_load (16) ;
#endif // PCwipeout
}


char SaveCardFile (char *saveName, int32_t cardNum, ConfigData *gameData, int16_t sliderVol)
{
	char	fileName[250];
	char	header[1];

	strcpy(fileName, SaveDirBase);
	strcat(fileName, "/");
	strcat(fileName, saveName);
	strcat(fileName, FILE_EXTENSION);

	if ( !WriteFile (&(header[0]), &(fileName[0]), gameData, sliderVol) )
		return (0) ;

	return (1) ;
}

void GetHeader (char *header)
{

}

char WriteFile (char *header, char *fileName, ConfigData *gameData, int16_t sliderVol)
{
	FILE	*fd;
	int32_t		temp;
	uint8_t	saveBuffer[256] ;
	int16_t			*short_ptr ;
	uint16_t	*ushort_ptr;
	uint8_t	*char_ptr ;
	int32_t			*long_ptr;


 	if((fd = fopen(fileName, "wb")) == NULL)
		return (0) ;
	else
	{
		temp = fwrite(scoreTable, ((sizeof(HiScores)) * (HiScoreTracks*2)), 1, fd);

		if (temp != 1)
			return (0) ;

		short_ptr = (int16_t *) &(saveBuffer[0]);
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
		ushort_ptr = (uint16_t *)short_ptr;
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

		char_ptr = (uint8_t *)ushort_ptr;
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

		long_ptr = (int32_t *)char_ptr;
		*long_ptr++ = DrawDistance;
		*long_ptr++ = TextureTrack;

		temp = fwrite(saveBuffer, sizeof(saveBuffer), 1, fd) ;

		if (temp != 1)
			return (0) ;

		fclose(fd);
		return (1) ;
	}
}



void LoadCardFiles (SelectionData *selectData, char loadGames[][9], int32_t cardNum)
{
#ifdef PCwipeout
 	int32_t		ret ;
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



char LoadCardData (char *loadName, int32_t cardNum, ConfigData *gameData, SelectionData *selectData)
{
	FILE	*fd;
	int32_t		temp;
	char	fileName[250];
	uint8_t	loadBuffer[256] ;
	int16_t			*short_ptr ;
	uint16_t	*ushort_ptr ;
	uint8_t	*char_ptr ;
	int32_t			*long_ptr;


	strcpy(fileName, SaveDirBase);
	strcat(fileName, "/");
	strcat(fileName, loadName);
	strcat(fileName, FILE_EXTENSION);

	if((fd = fopen(fileName, "rb")) == NULL)
	{
		printf ("Error opening file %s for read \n", fileName) ;
		return (0) ;
	}
	else
	{
		temp = fread(scoreTable, (sizeof (HiScores) * (HiScoreTracks*2)), 1, fd);

		if (temp != 1)
		{
			printf ("Error reading data from file %s\n", fileName) ;
			return (0) ;
		}

		temp = fread(loadBuffer, sizeof(loadBuffer), 1, fd);

		short_ptr = (int16_t *) &(loadBuffer[0]);
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
		ushort_ptr = (uint16_t *)short_ptr;
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

		char_ptr = (uint8_t *)ushort_ptr;
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

		long_ptr = (int32_t *)char_ptr;
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
		SfxSetVolume((char)((0x3fff - (75 * gameData->currSliderPos))>>6));

		fclose(fd);
		return (1);
	}
}
