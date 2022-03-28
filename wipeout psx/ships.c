/************************************************************/
/******     ships.c - Ship initialisation routines		  *****/
/************************************************************/

#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "scene.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "fileops.h"
#include "sound.h"
#include "combat.h"
#include "menus.h"  
#include "sparks2.h"
#include "global.h"
#include "hiscores.h"
#include "effects.h"
#include "options.h"
#include "sprite.h"
#include "cone.h"
#include "ttf.h"
#include "hud.h"
#include "trackasm.h"
#include "dynam.h"
#include "fri.h"

extern TrackCamera		ctrlcamera;

short startLinePos[16] = 	{	27,		/* NULL 			*/
								27,		/* V - TerrorMax 	*/
								27,		/* V - Altima VII 	*/
								27,		/* R - Altima VII 	*/
								16,		/* V - Karbonis 	*/
								16,		/* R - Karbonis 	*/
								27,		/* R - TerrorMax 	*/
								16,		/* R - Korrodera 	*/
								16,		/* V - Arridos IV 	*/
								16,		/* V - SilverStream */
								27,		/* V - FireStar 	*/
								16,		/* R - Arridos IV 	*/
								16,		/* V - Korrodera 	*/
								16,		/* R - SilverStream */
								27,		/* R - FireStar 	*/
								27};	/* NULL 			*/

short behindSpeed[16]  = {		100,	/* NULL 			*/
								350,	/* V - TerrorMax 	*/
								300,	/* V - Altima VII 	*/
								500,	/* R - Altima VII 	*/
								200,	/* V - Karbonis 	*/
								500, 	/* R - Karbonis 	*/
								500, 	/* R - TerrorMax 	*/
								500,	/* R - Korrodera 	*/
								350, 	/* V - Arridos IV 	*/
								150, 	/* V - SilverStream */
								200, 	/* V - FireStar 	*/
								450,	/* R - Arridos IV 	*/
								450, 	/* V - Korrodera 	*/
#if 0
								150, 	/* R - SilverStream */
#else
								10, 	/* R - SilverStream */
#endif
								500, 	/* R - FireStar 	*/
								200};	/* NULL 			*/

short shipShapeOrder[8] = {7, 3, 5, 6, 1, 4, 0, 2};

short championOrder[8];
short raceOrder[8];
short shipOrder[8];

void SetSpread(ShipData *shipIndex, ConfigData *gameData, int position, int i);



void DrawShips(ShipData *shipIndex, Object** shipShapes, int camShip, TrackCamera camera)
{
	int 		i;
	VECTOR 		distance;
	Object* 	obj;
	int 		mag;
	SVECTOR 	toCentre;
	VECTOR 		v2;
	long 		flag;


	for(i= 0; i < NO_OF_SHIPS; i++)
	{
		distance.vx = (shipIndex[i].ppivot.vx + ctrlcamera.camPos->relative.t[0])>>3;
		distance.vy = (shipIndex[i].ppivot.vy + ctrlcamera.camPos->relative.t[1])>>3;
		distance.vz = (shipIndex[i].ppivot.vz + ctrlcamera.camPos->relative.t[2])>>3;

		mag = GetMagnitude(distance);

		toCentre.vx = (shipIndex[i].ppivot.vx + ctrlcamera.camPos->relative.t[0]);
		toCentre.vy = (shipIndex[i].ppivot.vy + ctrlcamera.camPos->relative.t[1]);
		toCentre.vz = (shipIndex[i].ppivot.vz + ctrlcamera.camPos->relative.t[2]);
	  	SetSkeletonTransform( ctrlcamera.camPos->super );	//AJY
	  	RotTrans( &toCentre, &v2, &flag );

		if(mag < 2000)
			shipIndex[i].attr |= VISIBLE;

		if((mag < 2000) && (v2.vz > 0) || (i == camShip)) 
		{
			if(shipIndex[ownShip].attr & RACING)
			{
				if(mag < 1600)
			   		obj = shipShapes[ shipIndex[i].highResTableIndex ];
				else 
			   		obj = shipShapes[ shipIndex[i].lowResTableIndex ];
			}
        	else
        	{
				obj = shipShapes[ shipIndex[i].highResTableIndex ];
			}

			SetSkeletonPosition( obj->skeleton,
				shipIndex[i].ppivot.vx,
		  		shipIndex[i].ppivot.vy,
				shipIndex[i].ppivot.vz);

			SetSkeletonDirectionHPR(obj->skeleton,
				 shipIndex[i].hdg,
				 shipIndex[i].pitch,
				 shipIndex[i].roll );
		
	 		if( ((shipIndex[i].attr & VIEW_IN) == 0) ||
	 				(shipIndex[i].attr & REMOTE_VIEW))
	 		{
				if(raceType == HEAD_TO_HEAD)
				{
					if((i == serialShip)	|| (i == ownShip))
			 			DrawObjectAsm( obj, ctrlcamera.camPos, 0, 1);
				}
				else if(raceType == TIME_TRIAL)
				{
					if((i == ownShip))
			 			DrawObjectAsm( obj, ctrlcamera.camPos, 0, 1);
				}
				else
		 			DrawObjectAsm( obj, ctrlcamera.camPos, 0, 1);
         	} 
		}
		else
		{
			shipIndex[i].attr &= ~VISIBLE;
		}
	}
}



void loadShipData(TrackSection* section, ShipData *shipIndex, Object** shipShapes, Skeleton* camPos)
{
	int 				i;
	short 			shipTextures, droidTextures, colTextures;
   Object*        obj;
	TIMlist  		*timPtr;
	char				shipFile[256];
	char 				colFile[256];
	char				No[1];
	short				count = 0;

	/******************************************************/
	/** Load compressed texture files, first to allow    **/
	/** temporary memory to be allocated					  **/
	/******************************************************/

	strcpy( shipFile, "wipeout\\common\\allsh" );
	strcat( shipFile, ".cmp");
   shipTextures = TextureTableCount;
	timPtr = LoadCompressedTextureSequence( shipFile );
   LoadCmpFiles( timPtr ); 

	colTextures = TextureTableCount;
	timPtr = LoadCompressedTextureSequence( "wipeout\\common\\alcol.cmp" );
	LoadCmpFiles( timPtr ); 

	droidTextures = TextureTableCount;
	timPtr = LoadCompressedTextureSequence( "wipeout\\common\\rescu.cmp" );
	LoadCmpFiles( timPtr ); 

	strcpy( shipFile, "wipeout\\common\\allsh" );
	strcat( shipFile, ".prm");

	shipShapes[0] = LoadPrm( shipFile, shipTextures );	 

   obj = shipShapes[ 0 ];

   while ( obj )
   {	
		shipIndex[count].highResTableIndex = shipShapeOrder[count];

		obj->skeleton->super = camPos;

		shipShapes[count] = obj;

		count += 1;

      obj = obj->next;
   }
	strcpy( shipFile, "wipeout\\common\\alcol" );
	strcat( shipFile, ".prm");

	shipShapes[ NO_OF_SHIPS ] = LoadPrm( shipFile, colTextures );	 
   obj = shipShapes[ NO_OF_SHIPS ];

   while ( obj )
   {	
		shipIndex[count - NO_OF_SHIPS].lowResTableIndex = count;

		obj->skeleton->super = camPos;

		shipShapes[count] = obj;

		count += 1;

      obj = obj->next;
   }
#if 0
	for(i = 0; i < NO_OF_SHIPS; i++)
	{	  
		strcpy( colFile, "wipeout\\common\\shp" );
		sprintf( No, "%1d", i+1 );	
		strcat( shipFile, No);
		strcat( colFile, No);
		strcat( colFile, "c.prm");
								 
		shipIndex[i].lowResTableIndex = NO_OF_SHIPS+i;
 		shipShapes[NO_OF_SHIPS+i] = LoadPrm( colFile, shipTextures );

      obj = shipShapes[ NO_OF_SHIPS+i ];
		obj->skeleton = shipShapes[ i ]->skeleton;
	}
#endif

   shipShapes[RESCUE_DROID] = LoadPrm( "wipeout\\common\\rescu.prm", droidTextures );

   obj = shipShapes[ RESCUE_DROID ];
   while ( obj )
   {	
		obj->skeleton->super = camPos;
      obj = obj->next;
   }

}



void initShipData(TrackSection* section, ShipData *shipIndex, ConfigData *gameData);

void initShipData(TrackSection* section, ShipData *shipIndex,ConfigData *gameData)
{
	int 				i, j;
	Face 				*face;
	VECTOR 			targetVector;
	TrackSection* 	nextSection;
	VECTOR			facePoint;
 	TrackSection* 	startSection[8];
	short 			position = 1;
	short 			tempPos = 1;
	short 			used[8] = { 0, 0, 0, 0, 0, 0, 0, 0};
	int 				index;
	int 				count;

	/***********************************************************************************/
	/** 							Ship ATTRIBUTES					  								 *****/
	/***********************************************************************************/
	/******   TEAM 1 -     Acc 			***													 *****/
	/******                Top Speed		****													 *****/
	/******   			   Armour			*****													 *****/
	/******   			   Turn Rate		**														 *****/
	/***********************************************************************************/
	/******   TEAM 2 -     Acc 			*****													 *****/
	/******                Top Speed		**														 *****/
	/******   			   Armour			***													 *****/
	/******   			   Turn Rate		****													 *****/
	/***********************************************************************************/
	/******   TEAM 3 -     Acc 			**														 *****/
	/******                Top Speed		*****													 *****/
	/******   			   Armour			****													 *****/
	/******   			   Turn Rate		***													 *****/
	/***********************************************************************************/
	/******   TEAM 4 -     Acc 			****													 *****/
	/******                Top Speed		***													 *****/
	/******   			   Armour			**														 *****/
	/******   			   Turn Rate		*****													 *****/
	/***********************************************************************************/


	/**** 											Team Specific								 ****/
	/**** Ship No						0		1		2		3		4		5		6		7	 ****/

	short massBeg[8] 			=	{	150,	150,	150,	150,	150,	150,	150,	150	};

	short maxThrustBeg[8] 	=	{	790,	790,	850,	850,	850,	850,	790,  790	};	
	short resistanceBeg[8] 	=	{	140,	140,	134,	134,	140,	140,	134,	134	};
	short headingIncBeg[8] 	=	{	160,	160,	140,	140,	120,	120,	180,	180	};
	short maxHeadingBeg[8] 	=	{	2560,	2560,	1920,	1920,	1920,	1920,	2560,	2560	};
	short skidBeg[8] 		  	=	{	12,	12,	20,	20,	24,	24,	12,	12		};

	short massPro[8] 			=	{	150,	150,	150,	150,	150,	150,	150,	150	};
	short maxThrustPro[8] 	=	{	1200,	1200,	1400,	1400,	1400,	1400,	1200, 1200	};	
	short resistancePro[8] 	=	{	140,	140,	140,	140,	130,	130,	130,	130	};
	short headingIncPro[8] 	=	{	160,	160,	120,	120,	140,	140,	180,	180	};
	short maxHeadingPro[8] 	=	{	2560,	2560,	1920,	1920,	1920,	1920,	2560,	2560	};
	short skidPro[8] 		  	=	{	10,	10,	14,	14,	16,	16,	8,		8		};


	/**** 							Remote Control Specific	 						 ****/
	/**** Ship No				0		1		2		3		4		5		6			 ****/

	short RMaxThrustBeg[7] = {	2550,	2600,	2630,	2660,	2700,	2720,	2750	};
	short RThrustMagBeg[7] = {	44,	45,	45,	46,	47,	48,	49		};
	short fightBackBeg[7]  = {	1,		1,		1,		1,		1,		1,		1		};

	short RMaxThrustPro[7] = {	3750,	3780,	3800,	3850,	3900,	3950,	4000	};
	short RThrustMagPro[7] = {	50,	53,	55,	57,	60,	62,	65		};
	short fightBackPro[7]  = {	1,		1,		1,		1,		1,		1,		1		};


#if 0			/************* Disable Rapier Class *************/
	gameData->class = 0;
#endif

	crossedLine = 0;	
   crossedLineSerial = 0;
	if (gameType == SINGLE)
	{
		if(((gameData->gameType == CHAMPIONSHIP) && (gameData->trackNum == 1))	 ||
			((gameData->gameType == CHAMPIONSHIP) && (gameData->trackNum == 8)) ||
			(gameData->gameType != CHAMPIONSHIP))

		{
			for(i = 0; i < NO_OF_SHIPS; i++)
			{
				if(i != ownShip)
				{
					while(1)
					{
						index = randy()%8;
						if(index != 0)
						{
							if( used[index] == 0)
							{ 	
								championOrder[index] = i;
								raceOrder[index] = i;
								shipOrder[i] = index;
								used[index] = 1;
								break;
							}
						}
					}
				}
			}

			championOrder[0] = ownShip;		
			raceOrder[0] = ownShip;		
			shipOrder[ownShip] = 0;
		}

		else if(gameData->gameType == CHAMPIONSHIP)
		{
			raceOrder[7] = championOrder[7];
			shipOrder[raceOrder[7]] = 7;

			if((randy()%2) == 0)
			{
				raceOrder[5] = championOrder[5];
				raceOrder[6] = championOrder[6];
			}
			else
			{
				raceOrder[6] = championOrder[5];
				raceOrder[5] = championOrder[6];
			}

			shipOrder[raceOrder[5]] = 5;
			shipOrder[raceOrder[6]] = 6;

			for( i = 1; i < 5; i++)
			{
				while(1)
				{
					index = (randy()%4)+1;
					{
						if(used[index] == 0)
						{
							used[index] = 1;
							raceOrder[i] = championOrder[index];
							break;
						}	
					}
				}	
			}
			shipOrder[raceOrder[4]] = 4;
			shipOrder[raceOrder[3]] = 3;
			shipOrder[raceOrder[2]] = 2;
			shipOrder[raceOrder[1]] = 1;
		
			shipOrder[ownShip] = 0;		
			raceOrder[0] = ownShip;		
		}
	}
	else
	{
		count = 2;
		for(i = 0; i < NO_OF_SHIPS; i++)
		{
			if(( i != ownShip) && (i != serialShip))
			{
				shipOrder[i] = count;
				count++;
			}
		}
		if(gameType == MASTER)
		{
		 	shipOrder[ownShip] = 0;
		 	shipOrder[serialShip] = 1;
		}	
		if(gameType == SLAVE)
		{
		 	shipOrder[ownShip] = 1;
		 	shipOrder[serialShip] = 0;
		}	

	}
//	for(i = 0; i < NO_OF_SHIPS; i++)
//	{
//		printf("SHIP NO = %d\n", raceOrder[i]);
//	}

	if(gameType == SINGLE)
	{
		tempPos = 1;
	}
	else
		tempPos = 2;

	for( i = 0; i < startLinePos[gameData->actTrackNum] - 15; i++)
	{
	  	section = section->nextSection.ptr;		
   }
   for(i = 0; i < NO_OF_SHIPS; i++)
	{									 
		startSection[i] = section;
		section = section->nextSection.ptr; 		
		if((i % 2) == 0)
			section = section->nextSection.ptr; 		
	}
	if(gameType == SLAVE)
	{
		startSection[1] = startSection[1]->prevSection.ptr; 
		startSection[1] = startSection[1]->prevSection.ptr; 
	}

   for(i = 0; i < NO_OF_SHIPS; i++)
	{									 
		shipIndex[i].vpivot.vx = 0;
		shipIndex[i].vpivot.vy = 0;
		shipIndex[i].vpivot.vz = 0;
		shipIndex[i].apivot.vx = 0;
		shipIndex[i].apivot.vy = 0;
		shipIndex[i].apivot.vz = 0;
		shipIndex[i].hdg = 0;
		shipIndex[i].pitch = 0;
		shipIndex[i].roll = 0;
		shipIndex[i].vhdg = 0;
		shipIndex[i].vpitch = 0;
		shipIndex[i].vroll = 0;
		shipIndex[i].ahdg = 0;
		shipIndex[i].thrust_mag = 0;
		shipIndex[i].r_brake_rot = 0;
		shipIndex[i].l_brake_rot = 0;
		shipIndex[i].attr = 0;
		shipIndex[i].attr |= RACING;
		shipIndex[i].attr |= VISIBLE;
		shipIndex[i].attr |= DIRECTION;
		shipIndex[i].combatAttr = 0;
		shipIndex[i].collideCount = 0;
	 	shipIndex[i].weaponType = NO_WEAPON; 
		shipIndex[i].lapNo = 0;
		shipIndex[i].prevLapNo = 0;
		shipIndex[i].speed = 0;	 
		shipIndex[i].electroCount = 0;	 
		shipIndex[i].revConCount = 0;	 
 				  
		if(gameData->class == 0)
		{
			shipIndex[i].mass = massBeg[i];
			shipIndex[i].max_thrust = maxThrustBeg[i];
			shipIndex[i].skid = skidBeg[i];
		}
		else
		{
			shipIndex[i].mass = massPro[i];
			shipIndex[i].max_thrust = maxThrustPro[i];
			shipIndex[i].skid = skidPro[i];
		}

		if(gameData->class == 0)
		{
			shipIndex[i].headingInc = (headingIncBeg[i]*60)/FR60;
			shipIndex[i].maxHeading = (maxHeadingBeg[i]*60)/FR60;
		}
		else
		{
			shipIndex[i].headingInc = (headingIncPro[i]*60)/FR60;
			shipIndex[i].maxHeading = (maxHeadingPro[i]*60)/FR60;
		}

		if(gameData->class == 0)
		{
			shipIndex[i].resistance = resistanceBeg[i];
			shipIndex[i].fightBack = fightBackBeg[i];
		}
		else
		{
			shipIndex[i].resistance = resistancePro[i];
			shipIndex[i].fightBack = fightBackPro[i];
		}


		for(j = 0; j < 5; j++)
			shipIndex[i].lapTimes[j] = 0;

		shipIndex[i].updateCount = START_TIME;
		shipIndex[i].targetShip = 0xFF;


		if(i == ownShip)
		{
			if(neg1.head == 8960)
				shipIndex[i].update = UpdatePlayerShipNegStart;	 
	  		else 
	  			shipIndex[i].update = UpdatePlayerShipNormStart;	 
		}
 		else 
			shipIndex[i].update = UpdateRemoteShipStart;

		if(gameType == SINGLE)
		{
			if(i == ownShip)
			{
				position = 0;
				shipIndex[i].position = position;
				shipIndex[i].remoteMaxThrust = 2900;
				shipIndex[i].remoteThrustMag = 46;
				shipIndex[i].fightBack = 		 0;
			}
			else
			{
				position = shipOrder[i];
				tempPos++;
				shipIndex[i].position =	position;

				if(gameData->class == 0)
				{
					if(slowCraft == 1)
						shipIndex[i].remoteMaxThrust = RMaxThrustBeg[position-1] >> 1;	  
					else
						shipIndex[i].remoteMaxThrust = RMaxThrustBeg[position-1];	  

					shipIndex[i].remoteThrustMag = RThrustMagBeg[position-1];	  
					shipIndex[i].fightBack = 		 fightBackBeg[position-1];		  
				}
				else
				{
					if(slowCraft == 1)
						shipIndex[i].remoteMaxThrust = RMaxThrustPro[position-1] >> 1;	  
					else
						shipIndex[i].remoteMaxThrust = RMaxThrustPro[position-1];	  
						
					shipIndex[i].remoteThrustMag = RThrustMagPro[position-1];	  
					shipIndex[i].fightBack = 		 fightBackPro[position-1];		  
				}

			}

			shipIndex[i].nearTrkSect = startSection[position];
			shipIndex[i].prevShipSect = startSection[position];
			SetSpread(shipIndex, gameData, position, i);
		}
		else if(gameType == MASTER)
		{
			if(i == ownShip)
			{
				position = 0;
				shipIndex[i].position = position;
				shipIndex[i].remoteMaxThrust = 2900;
				shipIndex[i].remoteThrustMag = 46;
				shipIndex[i].fightBack = 		 0;
			}
			else if(i == serialShip)
			{
				position = 1;
				shipIndex[i].position = position;
				shipIndex[i].remoteMaxThrust = 2900;
				shipIndex[i].remoteThrustMag = 46;
				shipIndex[i].fightBack = 		 0;
			}
			else
			{
				position = shipOrder[i];
				tempPos++;
				shipIndex[i].position =	position;

				if(gameData->class == 0)
				{
					if(slowCraft == 1)
						shipIndex[i].remoteMaxThrust = RMaxThrustBeg[position-1] >> 1;
					else
						shipIndex[i].remoteMaxThrust = RMaxThrustBeg[position-1];

					shipIndex[i].remoteThrustMag = RThrustMagBeg[position-1];
					shipIndex[i].fightBack = 		 fightBackBeg[position-1];
				}
				else
				{
					if(slowCraft == 1)
						shipIndex[i].remoteMaxThrust = RMaxThrustPro[position-1] >> 1;
					else
						shipIndex[i].remoteMaxThrust = RMaxThrustPro[position-1];

					shipIndex[i].remoteThrustMag = RThrustMagPro[position-1];
					shipIndex[i].fightBack = 		 fightBackPro[position-1];
				}
			}

			shipIndex[i].nearTrkSect = startSection[position];
			shipIndex[i].prevShipSect = startSection[position];
			SetSpread(shipIndex, gameData, position, i);

		}
		else
		{
			if(i == ownShip)
			{
				position = 1;
				shipIndex[i].position = position;
				shipIndex[i].remoteMaxThrust = 2900;
				shipIndex[i].remoteThrustMag = 46;
				shipIndex[i].fightBack = 		 0;
			}
			else if(i == serialShip)
			{
				position = 0;
				shipIndex[i].position = position;
				shipIndex[i].remoteMaxThrust = 2900;
				shipIndex[i].remoteThrustMag = 46;
				shipIndex[i].fightBack = 		 0;
			}
			else
			{
				position = shipOrder[i];
				tempPos++;
				shipIndex[i].position =	position;
				if(gameData->class == 0)
				{
					if(slowCraft == 1)
						shipIndex[i].remoteMaxThrust = RMaxThrustBeg[position-1] >> 1;
					else
						shipIndex[i].remoteMaxThrust = RMaxThrustBeg[position-1];
					shipIndex[i].remoteThrustMag = RThrustMagBeg[position-1];
					shipIndex[i].fightBack = 		 fightBackBeg[position-1];
				}
				else
				{
					if(slowCraft == 1)
						shipIndex[i].remoteMaxThrust = RMaxThrustPro[position-1] >> 1;
					else
						shipIndex[i].remoteMaxThrust = RMaxThrustPro[position-1];

					shipIndex[i].remoteThrustMag = RThrustMagPro[position-1];
					shipIndex[i].fightBack = 		 fightBackPro[position-1];
				}

			}

			shipIndex[i].nearTrkSect = startSection[position];
			shipIndex[i].prevShipSect = startSection[position];
			SetSpread(shipIndex, gameData, position, i);
		}


	   face = track->faces + startSection[position]->faceStart;
		face++; 
		if((position % 2) != 0) face++;

		facePoint.vx = (track->vertices[(face->vertex[0])].vx + track->vertices[(face->vertex[2])].vx)>>1;
		facePoint.vy = (track->vertices[(face->vertex[0])].vy + track->vertices[(face->vertex[2])].vy)>>1;
		facePoint.vz = (track->vertices[(face->vertex[0])].vz + track->vertices[(face->vertex[2])].vz)>>1;
		shipIndex[i].ppivot.vx = facePoint.vx + ((face->normal.vx*200)>>12);
		shipIndex[i].ppivot.vy = facePoint.vy + ((face->normal.vy*200)>>12);
		shipIndex[i].ppivot.vz = facePoint.vz + ((face->normal.vz*200)>>12);

		shipIndex[i].nearSecNo = startSection[position]->secNo;
		shipIndex[i].prevSecNo = startSection[position]->secNo;
		shipIndex[i].DPASectDiff = startSection[position]->secNo;

	  	nextSection = startSection[position]->nextSection.ptr;		
		targetVector.vx = nextSection->centre.vx - startSection[position]->centre.vx;
		targetVector.vy = nextSection->centre.vy - startSection[position]->centre.vy;
		targetVector.vz = nextSection->centre.vz - startSection[position]->centre.vz;
		shipIndex[i].hdg = -ratan2(targetVector.vx, targetVector.vz);
	}
}	



void SetSpread(ShipData *shipIndex, ConfigData *gameData, int position, int i)
{
	switch(gameData->actTrackNum)
	{
		case 1:
			shipIndex[i].numtospeed = (position-1) * (60 + ((position-1)*11));   
			break;
		case 2:
			shipIndex[i].numtospeed = (position-1) * (80 + ((position-1)*20));   
			break;
		case 4:
			shipIndex[i].numtospeed = (position-1) * (10 + ((position-1)*8));   
			break;
		case 8:
			shipIndex[i].numtospeed = (position-1) * (80 + ((position-1)*15));   
			break;
		case 9:
			shipIndex[i].numtospeed = (position-1) * (10 + ((position-1)*8));   
			break;
		case 12:
			shipIndex[i].numtospeed = (position-1) * (40 + ((position-1)*11));   
			break;
		case 10:
			shipIndex[i].numtospeed = (position-1) * (40 + ((position-1)*11));   
			break;

		case 3:
			shipIndex[i].numtospeed = (position-1) * (80 + ((position-1)*11));   
			break;
		case 5:
			shipIndex[i].numtospeed = (position-1) * (10 + ((position-1)*8));   
			break;
		case 6:
			shipIndex[i].numtospeed = (position-1) * (60 + ((position-1)*8));   
			break;
		case 7:
			shipIndex[i].numtospeed = (position-1) * (30 + ((position-1)*11));   
			break;
		case 11:
			shipIndex[i].numtospeed = (position-1) * (30 + ((position-1)*11));   
			break;
		case 13:
			shipIndex[i].numtospeed = (position-1) * (10 + ((position-1)*8));   
			break;
		case 14:
			shipIndex[i].numtospeed = (position-1) * (40 + ((position-1)*11));   
			break;
	}

	shipIndex[i].numtospeed = (shipIndex[i].numtospeed*FR60)/60;
}
