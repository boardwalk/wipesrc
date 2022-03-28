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
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "main.h"
#include "sound.h"
#include "dynam.h"
#include "combat.h"
#include "menus.h"
#include "hud.h"
#include "options.h"
#include "sparks2.h"
#include "global.h"

extern short starttrack, endtrack;

char *GetCdTrackName(int no)
{
	static char *fulltrack[] = {
		"RANDOM",
		"CAIRODROME", 
		"CARDINAL DANCER",
		"COLD COMFORT", 
		"DOH T",
		"MESSIJ",
		"OPERATIQUE",
		"TENTATIVE",
		"TRANCEVAAL",
		"WIPEOUT"};

	static char *sometrack[] = {
		"RANDOM",
		"COLD COMFORT", 
		"DOH T",
		"MESSIJ",
		"TENTATIVE",
		"WIPEOUT"};

	if(no<0)
		return("NO CD");
	else
	if (endtrack - starttrack == 7)
		return(fulltrack[no]);
	else
		return(sometrack[no]);
}


void InGameReset (int choice, ulong *toggle, char machinePaused, ConfigData *gameData, int *cdPause)
{
	int 		   	i;
	char			ingameMenu[4][9] ;
	short			yPos = 122, xPos[4] ;
	char			gap = 16 ;

	//if ((*toggle % 30)	<= 20)
	//{
		CentreText ("PAUSED", text_data(107, 90, 12), RedText) ;
	//}


	if (machinePaused == SINGLE || machinePaused == MASTER)
	{
		strcpy (ingameMenu[0], "CONTINUE") ;
		strcpy (ingameMenu[1], "RESTART") ;
		strcpy (ingameMenu[2], "QUIT") ;
		strcpy (ingameMenu[3], "CD TRACK") ;
		xPos[0] = 120 ;
		xPos[1] = 128 ;	  
		xPos[2] = 140 ;
		xPos[3] = 140 ;


		for(i = 0; i < 4; i++)
		{
			if(i == choice)
			{
				//if ((*toggle % 2) == 0)
					CentreText (&(ingameMenu[i][0]), text_data(xPos[i], yPos+(gap*i), 8), RedText) ;
				//else
					//CentreText (&(ingameMenu[i][0]), text_data(xPos[i], yPos+(gap*i), 8), WhiteText) ;
			}
			else
			{
				CentreText (&(ingameMenu[i][0]), text_data(xPos[i], yPos+(gap*i), 8), WhiteText) ;
			}

		}
		
		CentreText (GetCdTrackName(gameData->cdTrack-1), text_data(0, yPos+(gap*4), 8), RedText) ;

	}

		(*toggle) ++ ;

}


void	InGameConfirm (char *confirm, int choice, short xpad, char *lConfirm, ulong *toggle)  
{

	if (choice == 1)
	{
		CentreText ("ARE YOU SURE YOU", text_data (77, 95, 8), WhiteText) ; 
		CentreText ("WANT TO RESTART", text_data (90, 105, 8), WhiteText) ; 
	}
	else
	{
		CentreText ("ARE YOU SURE YOU", text_data (77, 95, 8), WhiteText) ; 
		CentreText ("WANT TO QUIT", text_data (105, 105, 8), WhiteText) ; 
	}

#if 0	
	if ((GetMenuKey(M_LEFT)) || (GetMenuKey(M_RIGHT)))
	{
		*lConfirm ^= 1 ;
		PlayNote(MENU_MOVE, 1, 20);
	}
#endif

	if (*lConfirm)
	{
		//if ((*toggle % 2) == 0)
			AddText ("YES", text_data (75, 120, 16), RedText) ; 
		//else
			//AddText ("YES", text_data (75, 120, 16), WhiteText) ;
		AddText ("NO", text_data (205, 120, 16), WhiteText) ; 
	}
	else
	{
		AddText ("YES", text_data (75, 120, 16), WhiteText) ; 
		//if ((*toggle % 2) == 0)
			AddText ("NO", text_data (205, 120, 16), RedText) ; 
		//else
			//AddText ("NO", text_data (205, 120, 16), WhiteText) ;
	}	

 
#if 0
	if ((GetMenuKey(M_SELECT)) || (GetMenuKey(M_START)))
		*confirm = *lConfirm ;
#endif

//	printf ("Quit yes no \n") ;

	(*toggle) ++ ;
}


void DeSelectScreen (POLY_F4* selectBox, DR_MODE* selectTrans)
{

	SetPolyF4(&(selectBox[CurrentScreen]));			
	selectBox[CurrentScreen].r0=230;
	selectBox[CurrentScreen].g0=0;
	selectBox[CurrentScreen].b0=BLACK2_TRANSL;

#if PAL
	setXY4(&(selectBox[CurrentScreen]),0 , 0,
								  320 , 0,
								  0 , 256,
								  320 , 256);
#else	
	setXY4(&(selectBox[CurrentScreen]),0 , 0,
								  320 , 0,
								  0 , 240,
								  320 , 240);
#endif

	SetSemiTrans(&(selectBox[CurrentScreen]),	1);

//	SetDrawMode(&(selectTrans[CurrentScreen]), 1, 1, GetTPage(0, 0, 0, 0), 0);
	SetDrawMode(&(selectTrans[CurrentScreen]), 1, 1, FONT_TPAGE, 0);


  	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(selectBox[CurrentScreen]) );
  	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(selectTrans[CurrentScreen]) );

}


#ifndef USE_ATI


void ScreenFade (POLY_F4* selectBox, DR_MODE* selectTrans, FadeData *fadeData)
{
	static char firstUsed = 1 ;

	if (firstUsed)
	{
 		fadeData->fadeCount = FADE_INACTIVE ;
		fadeData->fadeFlag = FADE_ACTIVE ;
		firstUsed = 0 ;
	}
	
	if (fadeData->fadeCount == 400)		/*260*/
	{
		fadeData->fadeFlag = FADE_OUT ;
		fadeData->fadeCount = 255 ;
		
/*		firstUsed = 1 ;*/
	}


	SetPolyF4(&(selectBox[CurrentScreen]));			

	if (fadeData->fadeCount >= 260)
	{
		selectBox[CurrentScreen].r0=230;
		selectBox[CurrentScreen].g0=0;
		selectBox[CurrentScreen].b0=BLACK2_TRANSL;
	}
	else
	{
		selectBox[CurrentScreen].r0=230;
		selectBox[CurrentScreen].g0=0;
		selectBox[CurrentScreen].b0=BLACK2_TRANSL;
	}

#if PAL
	setXY4(&(selectBox[CurrentScreen]),0 , 0,
								  320 , 0,
								  0 , 256,
								  320 , 256);
#else
	setXY4(&(selectBox[CurrentScreen]),0 , 0,
								  320 , 0,
								  0 , 240,
								  320 , 240);
#endif

	if (fadeData->fadeCount >= 260)
	{
		SetSemiTrans(&(selectBox[CurrentScreen]),	0);
	}
	else
		SetSemiTrans(&(selectBox[CurrentScreen]),	1);

	SetDrawMode(&(selectTrans[CurrentScreen]), 1, 1, FONT_TPAGE, 0);


  	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(selectBox[CurrentScreen]) );
  	AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(selectTrans[CurrentScreen]) );

	if (fadeData->fadeCount < 400 && fadeData->fadeFlag == FADE_ACTIVE)
		fadeData->fadeCount += 10 ;


	if (fadeData->fadeCount == 0 && fadeData->fadeFlag == FADE_OUT)
	{
		fadeData->fadeFlag = FADE_COMPLETE ;
		firstUsed = 1 ;
	}

	if (fadeData->fadeCount > 0 && fadeData->fadeFlag == FADE_OUT)
		fadeData->fadeCount -= 15 ;

}

#endif //USE_ATI
