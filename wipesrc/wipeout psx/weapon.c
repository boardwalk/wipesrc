#include "standard.h"			 

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "scene.h"
#include "track.h"
#include "tilelib.h"
#include "ttf.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "dynam.h"
#include "combat.h"
#include "search.h"
#include "menus.h"
#include "hud.h"
#include "camera.h"
#include "draw.h"
#include "sound.h"
#include "fileops.h"
#include "sparks2.h"
#include "global.h"
#include "main.h"
#include "texplace.h"
#include "trakview.h"
#include "effects.h"
#include "hiscores.h"
#include "options.h"
#include "sprite.h"
#include "trackasm.h"
#include "fri.h"

#include	"pallette.h"

extern long				ctrlNeedTargetIcon;
extern int 				ctrlnearShip;	
extern	char			WhitePal;
extern	char			GreyPal;
extern	char			DepthFadeTble;
short					Shielded = 0;
extern TrackCamera		ctrlcamera;

void DrawInternalShield(void);


void UpdateWeapons(ShipData *shipIndex,WeaponShape* weaponShapes, WeaponData *weaponIndex )
{
	int i;

	for(i = 0; i < NO_ACTIVE_WEAPONS; i++)
	{
		if(weaponIndex[i].count > 0)
		{
			(weaponIndex[i].updateWeapon)(shipIndex, weaponShapes, &(weaponIndex[i]), weaponIndex);
		}
	}
}



void DrawWeapons(ShipData *shipIndex,WeaponShape* weaponShapes, WeaponData *weaponIndex )
{
	int i;
	WeaponData	*weapon;

	for(i = 0; i < NO_ACTIVE_WEAPONS; i++)
	{
		if((weaponIndex[i].count > 0) && (weaponIndex[i].availible))
		{
			weapon = &(weaponIndex[i]);

//			if ((weapon->type == SHIELD)) && (shipIndex[weapon->fireShip].attr & VIEW_IN))
			if ((shipIndex[weapon->fireShip].attr & SHIELDED) && (shipIndex[weapon->fireShip].attr & VIEW_IN))
			{
				DrawInternalShield();
				Shielded = 1;
				continue;
			}

			if (weapon->type == SHIELD)
				TransformStaticObjectBodge( weaponShapes[weapon->currentWeapon].shapePtr, ctrlcamera.camPos);
			else
				TransformStaticObject( weaponShapes[weapon->currentWeapon].shapePtr, ctrlcamera.camPos);
		}
	}
}


void LaunchWeapon(ShipData *shipIndex, WeaponData *weaponIndex, int shipNo)
{
	int j;

	if(gameType == SLAVE)
	{		
		if(shipIndex[ownShip].DPASectDiff > shipIndex[serialShip].DPASectDiff)
		{
			for(j = 0; j < NO_OF_SHIPS; j++)
			{	
				if (( j != ownShip) && ( j != serialShip))
				{
					switch(shipIndex[j].haveFired)
					{
						case ROCKET :
							PlayNote(SP_ROCKET, 1, 70);
							SetPitch(SP_ROCKET, 0x1000);
							break;
					
						case MISSILE :
							PlayNote(SP_MISSILE, 1, 70);
							SetPitch(SP_MISSILE, 0x1000);	 
							break;

						case MINE :
							PlayNote(SP_MINES, 1, 70);
							SetPitch(SP_MINES, 0x1000);
							break;

						case ELEC_BOLT :
							PlayNote(SP_SHOCKWAVE, 1, 70);
							SetPitch(SP_SHOCKWAVE, 0x1000);
							break;
					}
				}
			}
		}

		if(raceType == HEAD_TO_HEAD)
		{
				SetTargetShip(shipIndex, serialShip);
				FireWeapon(&(shipIndex[serialShip]), weaponIndex, serialShip, shipIndex[serialShip].haveFired);
		}
		else
		{
			for(j = 0; j < NO_OF_SHIPS; j++)
			{
				if((shipIndex[j].haveFired != NON_ACTIVE) && ( j != ownShip))
				{
					SetTargetShip(shipIndex, j);
					FireWeapon(&(shipIndex[j]), weaponIndex, j, shipIndex[j].haveFired);
				}
			}
		}
	}
	else if(gameType == MASTER)
	{
		if(shipIndex[serialShip].haveFired != NON_ACTIVE)
		{
			SetTargetShip(shipIndex, serialShip);
			FireWeapon(&(shipIndex[serialShip]), weaponIndex, serialShip, shipIndex[serialShip].haveFired);
		}
	}
}



void FireWeapon(ShipData *playerShip, WeaponData *weaponIndex, int shipNo, char decision)
{
	int i; 
	int count = 0;

	for(i = 0; i < NO_ACTIVE_WEAPONS; i++)
	{
		if(weaponIndex[i].count == 0)
		{
			switch(decision)
			{
				case ROCKET:	
					weaponIndex[i].updateWeapon = InitRocket;
					weaponIndex[i].count = WEAPON_DELAY;
					weaponIndex[i].fireShip = shipNo;
					if(shipNo == ownShip)
					{
				  		PlayNote(MISS_LAUNCH, 1, 50);
					}
					weaponIndex[i].type = ROCKET;
				 	playerShip->weaponType	= NO_WEAPON;  
					playerShip->haveFired  = ROCKET;
					return;

				case MISSILE:
					weaponIndex[i].updateWeapon = InitMissile;
					weaponIndex[i].count = WEAPON_DELAY;
					weaponIndex[i].fireShip = shipNo;
					if(shipNo == ownShip)
					{
						PlayNote(MISS_LAUNCH, 1, 50);		 
					}
					weaponIndex[i].type = MISSILE;
			  		playerShip->weaponType	= NO_WEAPON; 
					playerShip->haveFired  = MISSILE;
					return;

				case MINE:
					weaponIndex[i].updateWeapon = InitMine;
					count += MINE_RELEASE_RATE;
					weaponIndex[i].fireShip = shipNo;
					if((weaponIndex[i].fireShip != ownShip) && (weaponIndex[i].fireShip != serialShip))
					{
						weaponIndex[i].count = (count + WEAPON_DELAY);
					}
					else
					{
						weaponIndex[i].count = count;
					}
					weaponIndex[i].type = MINE;
					if(count >= (5 * MINE_RELEASE_RATE))
					{
						playerShip->weaponType = NO_WEAPON;
						playerShip->haveFired  = MINE;
						return;
					}
					break;
																 
				case SPECIAL:
					weaponIndex[i].updateWeapon = InitSpecial;
					weaponIndex[i].count = 1;
					playerShip->weaponType = NO_WEAPON;
					playerShip->haveFired  = SPECIAL;
					weaponIndex[i].fireShip = shipNo;
					return;

				case ELEC_BOLT:
					weaponIndex[i].updateWeapon = InitElecBolt;
					weaponIndex[i].count = WEAPON_DELAY;
					weaponIndex[i].fireShip = shipNo;
					weaponIndex[i].type = ELEC_BOLT;
					playerShip->haveFired  = ELEC_BOLT;
					playerShip->weaponType	= NO_WEAPON;
					if(shipNo == ownShip)
					{
						PlayNote(ELECTROBOLT, 1, 50);
					}
					return;
				
				case REV_CON:
					weaponIndex[i].updateWeapon = InitRevCon;
					weaponIndex[i].count = WEAPON_DELAY;
					weaponIndex[i].fireShip = shipNo;
					weaponIndex[i].type = REV_CON;
					playerShip->haveFired  = REV_CON;
					playerShip->weaponType	= NO_WEAPON;
					if(shipNo == ownShip)
					{
						PlayNote(ELECTROBOLT, 1, 50);
					}
					return;

				case SHIELD:
					weaponIndex[i].updateWeapon = InitShield;
					weaponIndex[i].count = SHIELD_DURATION;
					weaponIndex[i].fireShip = shipNo;
					weaponIndex[i].type = SHIELD;
					playerShip->attr |= SHIELDED;
					playerShip->haveFired  =  SHIELD;
					playerShip->weaponType = NO_WEAPON;
					return;

				case TURBO:
					playerShip->weaponType = NO_WEAPON;
#if PAL
					playerShip->vpivot.vx += (playerShip->unitVecNose.vx)<<3;
					playerShip->vpivot.vy += (playerShip->unitVecNose.vy)<<3;
					playerShip->vpivot.vz += (playerShip->unitVecNose.vz)<<3;
#else
					playerShip->vpivot.vx += (((playerShip->unitVecNose.vx)<<3)*FR60)/50;
					playerShip->vpivot.vy += (((playerShip->unitVecNose.vy)<<3)*FR60)/50;
					playerShip->vpivot.vz += (((playerShip->unitVecNose.vz)<<3)*FR60)/50;
#endif
					if(shipNo == ownShip)
					{
						PlayNote(TURBO_SOUND, 1, 100);
						chanTable[vagTable[TURBO_SOUND].channelID].s_attr.pitch = 0x0400;
						chanTable[vagTable[TURBO_SOUND].channelID].s_attr.volume.left = 0x3fff;
						chanTable[vagTable[TURBO_SOUND].channelID].s_attr.volume.right = 0x3fff;
						SpuSetVoiceAttr(&(chanTable[vagTable[TURBO_SOUND].channelID].s_attr));
					}
					return;
		
			}
		}
	}
}


void MissileLock(ShipData *playerShip, ShipData *shipIndex, Object** shipShapes, POLY_FT4 *target)
{
	int 						i;
	int 						distance, shortestDistance;
	SVECTOR 					*vertex;
	DVECTOR 					sxy;
	long 						p, flag;
	short 					x, y;
	int 						nearShip;	
   Poly        			poly;
   short*      			coords;
	TrackSection			*section;
	

	shortestDistance = 999;

	for( i = (NO_OF_SHIPS-1); i >= 0; i--)
	{
		if( ((raceType == HEAD_TO_HEAD) && ((i == ownShip ) || (i == serialShip)))	 ||
			(raceType != HEAD_TO_HEAD))
		{
			if(i != ownShip)
			{
				/*** Is Player ship upon a junction branch **/

				if(playerShip->nearTrkSect->flags & JunctionSection)
				{
					/*** If remote ship on same branch the attempt to target ***/
					if(shipIndex[i].nearTrkSect->flags & JunctionSection)
					{
						distance = shipIndex[i].nearTrkSect->secNo - playerShip->nearTrkSect->secNo;

						if((distance < shortestDistance) && (distance > 0))
						{
							shortestDistance = distance;
  						nearShip = i;
						}
					}
					else
					{
						/*** Remote ship is not same branch ***/
						section = playerShip->nearTrkSect;

						for(distance = 0; distance < 10; distance++)
						{
							section = section->nextSection.ptr;
							if(shipIndex[i].nearTrkSect == section)
							{
								if((distance < shortestDistance) && (distance > 0))
								{
									shortestDistance = distance;
  								nearShip = i;
									break;
								}
							}
						}
					}
				}
				else
				{
					if(shipIndex[i].nearTrkSect->flags & JunctionSection)
					{
						section = playerShip->nearTrkSect;
						for(distance = 0; distance < 10; distance++)
						{
							if(section->junction.index != -1)
							{
								section = section->junction.ptr;
							}
							else
							{
								section = section->nextSection.ptr;
							}
							if(shipIndex[i].nearTrkSect == section)
							{
								if((distance < shortestDistance) && (distance > 0))
								{
									shortestDistance = distance;
  								nearShip = i;
									break;
								}
							}
						}
					}
					else
					{
						distance = shipIndex[i].nearTrkSect->secNo - playerShip->nearTrkSect->secNo;

						if((distance < shortestDistance) && (distance > 0))
						{
							shortestDistance = distance;
  						nearShip = i;
						}
					}
				}
			}
		}
	}

	if(shortestDistance < 10)
	{
		ctrlNeedTargetIcon = 1;
		ctrlnearShip = nearShip;	

		playerShip->targetShip = nearShip;
	}
	else playerShip->targetShip = 0xFF;
}

void DrawTargetIcon(ShipData *playerShip, ShipData *shipIndex, Object** shipShapes, POLY_FT4 *target)
{
	SVECTOR 					*vertex;
	DVECTOR 					sxy;
	long 						p, flag;
	short 					x, y;
   Poly        			poly;
   short*      			coords;

	shipShapes[shipIndex[ctrlnearShip].highResTableIndex]->skeleton->update = 1;
    SetSkeletonTransform( shipShapes[shipIndex[ctrlnearShip].highResTableIndex]->skeleton );
    vertex = shipShapes[shipIndex[ctrlnearShip].highResTableIndex]->vertices;
  	poly.prim = shipShapes[shipIndex[ctrlnearShip].highResTableIndex]->primitives;
    coords = poly.f3->coords;

	RotTransPers(	vertex,
						&sxy,
						&p,
						&flag);
	x = sxy.vx;
	y = sxy.vy;


	if(!(flag & (CrtClipped | ZNegative | ZLarge | ZSmall)))
	{
		setXY4(&(target[CurrentScreen]),
				x - 8 , y - 8,
				x + 7 , y - 8,
				x - 8 , y + 7,
				x + 7 , y + 7);
		target->z0 = vertex->vz;
		target->z1 = vertex->vz;
		target->z2 = vertex->vz;
		target->z3 = vertex->vz;
		target->pad4 = 0;
		target->pad5 = 0;
		target->pad6 = 0;
		target->pad7 = 0;
    	//AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(target[CurrentScreen]) );
    	ClipPolyFT4( OT[ CurrentScreen ] , ( ulong* ) &(target[CurrentScreen]) );

	}
}

void initWeapons(WeaponShape* weaponShapes, Skeleton* camPos, WeaponData *weaponIndex, POLY_FT4 *target )
{
	int 			i, j, k;
	Object* 		obj;
	short 		weaponTextures;
	Texture 		*targetTexture ;
   Poly        poly;
   short       primitiveCount;
   StdPoly     prim;
	long			length;
   Prm         prm;
   char*       start;
   Object*     object;
	TIMlist 		*timPtr;

	weaponTextures = TextureTableCount;
	timPtr = LoadCompressedTextureSequence( "wipeout\\common\\mine.cmp" );
	LoadCmpFiles( timPtr ); 

   length = FileLength( "wipeout\\common\\rock.prm"  );
	weaponShapes[ROCKET_OFFSET].shapePtr = LoadPrm( "wipeout\\common\\rock.prm", weaponTextures );
  	for(i = 1; i < MINE_OFFSET; i++)
	{
   	start = DAlloc( heap, length + 32 );
   	prm.ptr = start;
   	weaponShapes[i].shapePtr = prm.object;
		memcpy(start, weaponShapes[ROCKET_OFFSET].shapePtr, length);
      object = prm.object++;
     	object->skeleton = prm.skeleton++;
     	object->vertices = prm.svector;
     	prm.svector += object->vertexCount;
     	object->normals = prm.svector;
     	prm.svector += object->normalCount;
     	object->primitives = prm.primitive;
	}

   length = FileLength(  "wipeout\\common\\mine.prm" );
	weaponShapes[MINE_OFFSET].shapePtr = LoadPrm( "wipeout\\common\\mine.prm", weaponTextures );
  	for(i = MINE_OFFSET+1; i < MISSILE_OFFSET; i++)
	{
   	start = DAlloc( heap, length + 32 );
   	prm.ptr = start;
   	weaponShapes[i].shapePtr = prm.object;
		memcpy(start, weaponShapes[MINE_OFFSET].shapePtr, length);
      object = prm.object++;
     	object->skeleton = prm.skeleton++;
     	object->vertices = prm.svector;
     	prm.svector += object->vertexCount;
     	object->normals = prm.svector;
     	prm.svector += object->normalCount;
     	object->primitives = prm.primitive;

	}

   length = FileLength(  "wipeout\\common\\miss.prm" );
	weaponShapes[MISSILE_OFFSET].shapePtr = LoadPrm( "wipeout\\common\\miss.prm", weaponTextures );
  	for(i = MISSILE_OFFSET+1; i < SHIELD_OFFSET; i++)
	{
   	start = DAlloc( heap, length + 32 );
   	prm.ptr = start;
   	weaponShapes[i].shapePtr = prm.object;
		memcpy(start, weaponShapes[MISSILE_OFFSET].shapePtr, length);
      object = prm.object++;
     	object->skeleton = prm.skeleton++;
     	object->vertices = prm.svector;
     	prm.svector += object->vertexCount;
     	object->normals = prm.svector;
     	prm.svector += object->normalCount;
     	object->primitives = prm.primitive;
	}

   length = FileLength(  "wipeout\\common\\shld.prm" );
	weaponShapes[SHIELD_OFFSET].shapePtr = LoadPrm( "wipeout\\common\\shld.prm", weaponTextures );
  	for(i = SHIELD_OFFSET+1; i < SHIELD_OFFSET+NO_OF_SHIELDS; i++)
	{
   	start = DAlloc( heap, length + 32 );
   	prm.ptr = start;
   	weaponShapes[i].shapePtr = prm.object;
		memcpy(start, weaponShapes[SHIELD_OFFSET].shapePtr, length);
      object = prm.object++;
     	object->skeleton = prm.skeleton++;
     	object->vertices = prm.svector;
     	prm.svector += object->vertexCount;
     	object->normals = prm.svector;
     	prm.svector += object->normalCount;
     	object->primitives = prm.primitive;
	}

   length = FileLength(  "wipeout\\common\\ebolt.prm" );
	weaponShapes[EBOLT_OFFSET].shapePtr = LoadPrm( "wipeout\\common\\ebolt.prm", weaponTextures );
  	for(i = EBOLT_OFFSET+1; i < EBOLT_OFFSET+NO_OF_EBOLTS; i++)
	{
   	start = DAlloc( heap, length + 32 );
   	prm.ptr = start;
   	weaponShapes[i].shapePtr = prm.object;
		memcpy(start, weaponShapes[EBOLT_OFFSET].shapePtr, length);
      object = prm.object++;
     	object->skeleton = prm.skeleton++;
     	object->vertices = prm.svector;
     	prm.svector += object->vertexCount;
     	object->normals = prm.svector;
     	prm.svector += object->normalCount;
     	object->primitives = prm.primitive;
	}


  	for(i = 0; i < NO_OF_WEAPONS; i++)
	{
		if((i >= SHIELD_OFFSET) && (i <EBOLT_OFFSET)) // previously we had this at <=EBOLT_OFFSET - but PSX version doesn't
		{
	      poly.prim = weaponShapes[i].shapePtr->primitives;
  		   primitiveCount = weaponShapes[i].shapePtr->primitiveCount;

      	for ( k = 0; k < primitiveCount; k++ )
      	{
         	switch ( poly.prim->type )
         	{
               case TypeG3 :
						for(j = 0; j < 2; j++)
						{
               	 /*	prim.g3 = poly.g3->tri + j;*/
							poly.g3->colour[ 0 ].r = 230;
							poly.g3->colour[ 0 ].g = 0;
							switch(i) // PCwipeout
							{
								case	MISSILE_OFFSET:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
								case	ROCKET_OFFSET:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
								case	EBOLT_OFFSET:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
								default:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
							}

							poly.g3->colour[ 1 ].b = 255;
							poly.g3->colour[ 2 ].b = 255;
						 /*	SetSemiTrans(prim.g3, 1);	*/
						}
            		poly.g3 += 1;
						break	;

            	case TypeG4 :
						for(j = 0; j < 2; j++)
						{
/*               	 	prim.g4 = poly.g4->quad + j; */
							poly.g4->colour[ 0 ].r = 230;
							poly.g4->colour[ 0 ].g = 0;
							switch(i) // PCwipeout
							{
								case	MISSILE_OFFSET:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
								case	ROCKET_OFFSET:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
								case	EBOLT_OFFSET:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
								default:
									poly.g4->colour[ 0 ].b = BLACK_NOT_DRAWN;
									break;
							}


							poly.g4->colour[ 1 ].b = 255;
							poly.g4->colour[ 2 ].b = 255;
							poly.g4->colour[ 3 ].b = 255;
/*					  	 	SetSemiTrans(prim.g4, 1); */		 
						}
            		poly.g4 += 1;
						break	;
             }
			}
		}

      weaponShapes[i].used = 0;
      obj = weaponShapes[i].shapePtr;
      while ( obj )
      {
         obj->skeleton->super = camPos;
         obj = obj->next;
      }
	}
  	for(i = 0; i < NO_ACTIVE_WEAPONS; i++)
	{
		weaponIndex[i].count = 0;
		weaponIndex[i].availible = 0;
		weaponIndex[i].type = NO_WEAPON;
	}

	for(i = 0; i < 64*3; i++)
	{
		for(j = 0; j < 2; j++)
		{
			 SetDrawMode(&(shieldParams[i][j]), 1, 1, FONT_TPAGE, 0);  //PCWipeout
		}
	}

	targetTexture = LoadTexture ( "wipeout\\textures\\target2.tim", 1 ) ;

	for(i = 0; i < 2; i++)
	{
		SetPolyFT4(&(target[i]));			

		target[i].r0 = 230;
		target[i].g0 = 0;
		target[i].b0 = BLACK_NOT_DRAWN;

		setUV4(&(target[i]),
			targetTexture->u0, targetTexture->v0, 
			targetTexture->u1, targetTexture->v1, 
			targetTexture->u2, targetTexture->v2, 
			targetTexture->u3, targetTexture->v3) ;

		setTPage(&(target[i]),0,3,targetTexture->textureX,targetTexture->textureY);
		SetSemiTrans(&(target[i]),	1);

   	target[i].clut = targetTexture->cba;
	}
}


void UpdateRocket(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData* weapon, WeaponData* weaponIndex)
{ 
 	VECTOR 		distance;
	Face 			*facePtr;
	int 			i, mag;
	VECTOR 		facePoint;
	int 			alpha;

	weapon->nearTrkSect = FindNearestSection(weapon->nearTrkSect, weapon->pos);


	facePtr = track->faces + weapon->nearTrkSect->faceStart;
	
	while((facePtr->flags & TRACK_BASE) == 0)
	{
		facePtr++;
	}
					 
	weapon->vel.vx += weapon->acc.vx;
	weapon->vel.vy += weapon->acc.vy;
	weapon->vel.vz += weapon->acc.vz;
		  
	weapon->vel.vx -= weapon->vel.vx>>5;
	weapon->vel.vy -= weapon->vel.vy>>5;
	weapon->vel.vz -= weapon->vel.vz>>5;

	weapon->pos.vx += weapon->vel.vx;
	weapon->pos.vy += weapon->vel.vy;
	weapon->pos.vz += weapon->vel.vz;

	facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
	facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
	facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

	alpha = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);

	if(alpha < 2000)
	{
		weapon->pos.vx += (((200 - alpha) * (int)facePtr->normal.vx) >> 13);
		weapon->pos.vy += (((200 - alpha) * (int)facePtr->normal.vy) >> 13);
		weapon->pos.vz += (((200 - alpha) * (int)facePtr->normal.vz) >> 13);
	}

	weapon->count--;

	if(weapon->count == 0)
	{
		weaponShapes[weapon->currentWeapon].used = 0;
		weaponIndex[weapon->currentWeapon].availible = 0;
	}

	SetSkeletonPosition((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					weapon->pos.vx,
		  			weapon->pos.vy,
					weapon->pos.vz);

   weaponShapes[weapon->currentWeapon].shapePtr->skeleton->update = 1;

#if FRI
	TransformStaticObject( weaponShapes[weapon->currentWeapon].shapePtr, cameraPtr->camPos);
#endif

	/***********************************************************/
	/****   Perform collision checks against other ships     ***/
	/***********************************************************/

	for (i = 0; i < NO_OF_SHIPS; i++)
	{
		if ( ((i != weapon->fireShip) && (raceType != HEAD_TO_HEAD)) ||
			((raceType == HEAD_TO_HEAD) && ((i == ownShip) || (i == serialShip) ) && (i != weapon->fireShip) ) )
		{
			distance.vx = (shipIndex[i].ppivot.vx - weapon->pos.vx)>>2;
			distance.vy = (shipIndex[i].ppivot.vy - weapon->pos.vy)>>2;
			distance.vz = (shipIndex[i].ppivot.vz - weapon->pos.vz)>>2;
		
			mag = GetMagnitude(distance);
	 
			if(mag < 100)
			{
				if((shipIndex[i].attr & SHIELDED) == 0)
				{

					if(i != ownShip)
					{
						shipIndex[i].speed = shipIndex[i].speed>>5;
						shipIndex[i].vroll += randy()%250;
						shipIndex[i].ahdg = (randy()-16384)>>1;
					}
					else 
					{
						shipIndex[i].vpivot.vx -= (shipIndex[i].vpivot.vx*3)>>2;
						shipIndex[i].vpivot.vy -= (shipIndex[i].vpivot.vy*3)>>2;
						shipIndex[i].vpivot.vz -= (shipIndex[i].vpivot.vz*3)>>2;
						shipIndex[i].vroll += randy()%250;
						shipIndex[i].ahdg = (randy()-16384)>>1;
						SetShake(20);
					}
				}
				weapon->count = SHIP_EXPLODE;
				weapon->targetShip = i;
				weaponShapes[weapon->currentWeapon].used = 0;
				weaponIndex[weapon->currentWeapon].availible = 0;

				if(i != ownShip)
				{
					PlayDirectionalNote(EXPLOSION_1, 0, 40, weapon->pos);
				}
				else
				{
					PlayNote(EXPLOSION_1, 0, 40);
					chanTable[vagTable[EXPLOSION_1].channelID].s_attr.volume.left = 0x3fff;
					chanTable[vagTable[EXPLOSION_1].channelID].s_attr.volume.right = 0x3fff;
					SpuSetVoiceAttr(&(chanTable[vagTable[EXPLOSION_1].channelID].s_attr));
				}	
				return;
			}
		}
	}
	/***********************************************************/
	/****   Perform collision checks against track 			   ***/
	/***********************************************************/
  

	facePtr = track->faces + weapon->nearTrkSect->faceStart;

	if((weapon->nearTrkSect->flags & Jump) == 0)
	{
		for(i = 0; i < weapon->nearTrkSect->faceCount; i++)
		{
			facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
			facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
			facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
			mag = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);
			if(mag <= 0)
			{
				weapon->count = TRACK_EXPLODE;
				weaponShapes[weapon->currentWeapon].used = 0;
				weaponIndex[weapon->currentWeapon].availible = 0;
				PlayDirectionalNote(EXPLOSION_2, 0, 40, weapon->pos);
				return;
			}
			facePtr++;
		}
	} 

	Weapon2Mine(weapon, weaponIndex, weaponShapes);
	

}

void UpdateMissile(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData* weapon, WeaponData* weaponIndex)
{
 	VECTOR 				distance;
	VECTOR 				targetVector;
	VECTOR 				bestPath;
	Face 					*facePtr;
	int 					i, mag, temp, vhdg, vpitch;
	TrackSection 		*nextSection;
	VECTOR 				facePoint;
	int 					alpha;

	if(weapon->targetShip > -1)
	{
		targetVector.vx = (shipIndex[weapon->targetShip].ppivot.vx - weapon->pos.vx)>>3;
		targetVector.vy = (shipIndex[weapon->targetShip].ppivot.vy - weapon->pos.vy)>>3;
		targetVector.vz = (shipIndex[weapon->targetShip].ppivot.vz - weapon->pos.vz)>>3;
		temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

		vhdg 	 = ((-ratan2(targetVector.vx, targetVector.vz)) - weapon->hdg);
		vpitch = ((-ratan2(targetVector.vy, temp)) - weapon->pitch);
	}
	else
	{
		vhdg 	 = 0;
		vpitch = 0;
	}

	weapon->nearTrkSect = FindNearestSection(weapon->nearTrkSect, weapon->pos);

	if(vhdg > 2048) vhdg = vhdg - 4096;
	else if(vhdg < -2048) vhdg = vhdg + 4096;
	if(vpitch > 2048) vpitch = vpitch - 4096;
	else if(vpitch < -2048) vpitch = vpitch + 4096;

	weapon->hdg +=	vhdg>>2;
	weapon->pitch +=	vpitch>>2;

	weapon->hdg = ang(weapon->hdg);
	weapon->pitch = ang(weapon->pitch);

	weapon->acc.vx = (-rsin(weapon->hdg) * rcos(weapon->pitch))>>16;
	weapon->acc.vy = -rsin(weapon->pitch)>>4;
	weapon->acc.vz = (rcos(weapon->hdg) * rcos(weapon->pitch))>>16;

  	weapon->acc.vx = (weapon->acc.vx*60)/FR60;
  	weapon->acc.vy = (weapon->acc.vy*60)/FR60;
  	weapon->acc.vz = (weapon->acc.vz*60)/FR60;

	weapon->vel.vx += weapon->acc.vx;
	weapon->vel.vy += weapon->acc.vy;
	weapon->vel.vz += weapon->acc.vz;

	weapon->vel.vx -= weapon->vel.vx>>2;
	weapon->vel.vy -= weapon->vel.vy>>2;
	weapon->vel.vz -= weapon->vel.vz>>2;

	weapon->pos.vx += weapon->vel.vx;
	weapon->pos.vy += weapon->vel.vy;
	weapon->pos.vz += weapon->vel.vz;

	facePtr = track->faces + weapon->nearTrkSect->faceStart;
	
	while((facePtr->flags & TRACK_BASE) == 0)
	{
		facePtr++;
	}

	facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
	facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
	facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

	alpha = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);

	if(alpha < 2000)
	{
		weapon->pos.vx += (((200 - alpha) * (int)facePtr->normal.vx) >> 13);
		weapon->pos.vy += (((200 - alpha) * (int)facePtr->normal.vy) >> 13);
		weapon->pos.vz += (((200 - alpha) * (int)facePtr->normal.vz) >> 13);
	}

	weapon->count--;

	if(weapon->count == NON_ACTIVE)
	{
		weaponShapes[weapon->currentWeapon].used = 0;
		weaponIndex[weapon->currentWeapon].availible = 0;
	}

	SetSkeletonPosition((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					weapon->pos.vx,
		  			weapon->pos.vy,
					weapon->pos.vz);

	SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					weapon->hdg,
					weapon->pitch,
					0);

  weaponShapes[weapon->currentWeapon].shapePtr->skeleton->update = 1;

#if FRI
  	
	TransformStaticObject( weaponShapes[weapon->currentWeapon].shapePtr, cameraPtr->camPos);

#endif

	/***********************************************************/
	/****   Perform collision checks against other ships     ***/
	/***********************************************************/

	for (i = 0; i < NO_OF_SHIPS; i++)
	{
		if ( ((i != weapon->fireShip) && (raceType != HEAD_TO_HEAD)) ||
			((raceType == HEAD_TO_HEAD) && ((i == ownShip) || (i == serialShip) ) && (i != weapon->fireShip) ) )
		{
			distance.vx = (shipIndex[i].ppivot.vx - weapon->pos.vx)>>3;
			distance.vy = (shipIndex[i].ppivot.vy - weapon->pos.vy)>>3;
			distance.vz = (shipIndex[i].ppivot.vz - weapon->pos.vz)>>3;
		
			mag = GetMagnitude(distance);
	 
			if(mag < 100)
			{
				if((shipIndex[i].attr & SHIELDED) == 0)
				{
					if(i != ownShip)
					{
						shipIndex[i].speed = shipIndex[i].speed>>5;
						shipIndex[i].vroll += 5000;
						shipIndex[i].ahdg = (randy()-16384)>>1;
					}
					else 
					{
						shipIndex[i].vpivot.vx -= (shipIndex[i].vpivot.vx*3)>>2;
						shipIndex[i].vpivot.vy -= (shipIndex[i].vpivot.vy*3)>>2;
						shipIndex[i].vpivot.vz -= (shipIndex[i].vpivot.vz*3)>>2;
						shipIndex[i].vroll += randy()%250;
						shipIndex[i].ahdg = (randy()-16384)>>1;
						SetShake(20);
					}
				}
				weapon->count = SHIP_EXPLODE;
				weapon->targetShip = i;
				weaponShapes[weapon->currentWeapon].used = 0;
				weaponIndex[weapon->currentWeapon].availible = 0;

				PlayDirectionalNote(EXPLOSION_1, 0, 40, weapon->pos);
				return;

			}
		}
	}
	/***********************************************************/
	/****   Perform collision checks against track 			   ***/
	/***********************************************************/
  
	facePtr = track->faces + weapon->nearTrkSect->faceStart;

	for(i = 0; i < weapon->nearTrkSect->faceCount; i++)
	{
		facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
		facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
		facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
		mag = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);
		if(mag <= 0)
		{
			weapon->count = TRACK_EXPLODE;
			weaponShapes[weapon->currentWeapon].used = 0;
			weaponIndex[weapon->currentWeapon].availible = 0;
			PlayDirectionalNote(EXPLOSION_2, 0, 40, weapon->pos);
			return;
		}
		facePtr++;
	}	

	Weapon2Mine(weapon, weaponIndex, weaponShapes);
}

void InitMissile(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData* weapon, WeaponData* weaponIndex)
{
	int 				i;
	Face				*facePtr;

	if((weapon->fireShip != ownShip) && (weapon->fireShip != serialShip))
	{
		if(weapon->count > 1)
		{
			weapon->count--;
			return;
		}
		else
		{
			weapon->count = MISSILE_DURATION;
		}
	}
	else
	{
		weapon->count = MISSILE_DURATION;
	}
	weapon->nearTrkSect = shipIndex[weapon->fireShip].nearTrkSect;
	for(i = 0; i < NO_OF_MISSILES; i++)
	{
		if(weaponShapes[MISSILE_OFFSET + i].used == 0)
		{
			weapon->currentWeapon = MISSILE_OFFSET + i;
			weaponShapes[MISSILE_OFFSET + i].used = 1;
			weapon->targetShip = shipIndex[weapon->fireShip].targetShip;

	 		facePtr = track->faces + shipIndex[weapon->fireShip].nearTrkSect->faceStart;
	
			while((facePtr->flags & TRACK_BASE) == 0)
			{
				facePtr++;
			}

			GetFireVector(		&weapon->acc,
							  	 	shipIndex[weapon->fireShip].unitVecNose,
				 					facePtr, 
									shipIndex[weapon->fireShip].ppivot 	);

			weapon->acc.vx = (weapon->acc.vx*60)/FR60;
			weapon->acc.vy = (weapon->acc.vy*60)/FR60;
			weapon->acc.vz = (weapon->acc.vz*60)/FR60;

			weapon->vel.vx = (shipIndex[weapon->fireShip].vpivot.vx>>6);
			weapon->vel.vy = (shipIndex[weapon->fireShip].vpivot.vy>>6);
			weapon->vel.vz = (shipIndex[weapon->fireShip].vpivot.vz>>6);

		  	weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx;
			weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy;
			weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz;

			SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					shipIndex[weapon->fireShip].hdg,
					shipIndex[weapon->fireShip].pitch,
					shipIndex[weapon->fireShip].roll);

			weapon->hdg   = shipIndex[weapon->fireShip].hdg;
			weapon->pitch = shipIndex[weapon->fireShip].pitch;
			weapon->updateWeapon = UpdateMissile;
			weapon->availible = 1;	//AJY
			return;	
		}
	}	
	weapon->count = NON_ACTIVE;
}

void UpdateMine(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData *weapon, WeaponData* weaponIndex)
{
 	VECTOR 				distance;
	int 					ossilation;
	int 					i, mag;
   Poly        		poly;
   StdPoly     		prim;
	short					r;


	weapon->count--;

	if(weapon->count == NON_ACTIVE)
	{
		weaponShapes[weapon->currentWeapon].used = 0;
		weaponIndex[weapon->currentWeapon].availible = 0;
		return;
	}

	ossilation = sar(rsin((weapon->currentWeapon*400) + (weapon->count*40)),9);

	weapon->pos.vx += ossilation;

	weapon->hdg += 128;

#if 1
   poly.prim = weaponShapes[weapon->currentWeapon].shapePtr->primitives;
	r = (rsin(weapon->count*140) >> 6) + 230;
	if(r > 255) r= 255;
   for ( i = 0; i < 8; i++ )
   {
      switch ( poly.prim->type )
      {
         case TypeGT3 :
					poly.gt3->colour[ 0 ].r = 230;
					poly.gt3->colour[ 1 ].r = (char)r;
					poly.gt3->colour[ 2 ].r = (char)r;
					poly.gt3->colour[ 0 ].g = 0;
					poly.gt3->colour[ 1 ].g = 0x40;
					poly.gt3->colour[ 2 ].g = 0x40;
					poly.gt3->colour[ 0 ].b = 0;
					poly.gt3->colour[ 1 ].b = 0;
					poly.gt3->colour[ 2 ].b = 0;
            poly.gt3 += 1;
				break;

		}
	}
#endif

	SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					weapon->hdg,
					0,
					0);

	SetSkeletonPosition((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					weapon->pos.vx,
		  			weapon->pos.vy,
					weapon->pos.vz);

   weaponShapes[weapon->currentWeapon].shapePtr->skeleton->update = 1;
#if 0

	TransformStaticObject( weaponShapes[weapon->currentWeapon].shapePtr, cameraPtr->camPos);
#endif

	/***********************************************************/
	/****   Perform collision checks against other ships     ***/
	/***********************************************************/
  
	for (i = 0; i < NO_OF_SHIPS; i++)
	{
		if ( ((i != weapon->fireShip) && (raceType != HEAD_TO_HEAD)) ||
			((raceType == HEAD_TO_HEAD) && ((i == ownShip) || (i == serialShip) ) && (i != weapon->fireShip) ) )
		{
			distance.vx = (shipIndex[i].ppivot.vx - weapon->pos.vx)>>2;
			distance.vy = (shipIndex[i].ppivot.vy - weapon->pos.vy)>>2;
			distance.vz = (shipIndex[i].ppivot.vz - weapon->pos.vz)>>2;
		
			mag = GetMagnitude(distance);
	 
			if(mag < 100)
			{
				if((shipIndex[i].attr & SHIELDED) == 0)
				{
					weapon->count = SHIP_EXPLODE;
					weapon->targetShip = i;
					if(i != ownShip)
					{
						shipIndex[i].speed = shipIndex[i].speed>>3;
					}
					else 
					{
						shipIndex[i].vpivot.vx -= shipIndex[i].vpivot.vx>>3;
						shipIndex[i].vpivot.vy -= shipIndex[i].vpivot.vy>>3;
						shipIndex[i].vpivot.vz -= shipIndex[i].vpivot.vz>>3;
						SetShake(20);
					}
				}
				weapon->count = SHIP_EXPLODE;
				weaponShapes[weapon->currentWeapon].used = 0;
				weaponIndex[weapon->currentWeapon].availible = 0;
				PlayDirectionalNote(EXPLOSION_1, 0, 40, weapon->pos);
				return;
			}
		}
	}

}


void InitRocket(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData *weapon, WeaponData* weaponIndex)
{
	int 			i;
	Face 			*facePtr;

	if((weapon->fireShip != ownShip) && (weapon->fireShip != serialShip))
	{
		if(weapon->count > 1)
		{
			weapon->count--;
			return;
		}
		else
		{
			weapon->count = ROCKET_DURATION;
		}
	}
	else
	{
		weapon->count = ROCKET_DURATION;
	}

	weapon->nearTrkSect = shipIndex[weapon->fireShip].nearTrkSect;

	for(i = 0; i < NO_OF_ROCKETS; i++)
	{
		if(weaponShapes[ROCKET_OFFSET + i].used == 0)
		{
			weapon->currentWeapon = ROCKET_OFFSET + i;
			weaponShapes[ROCKET_OFFSET + i].used = 1;

	 		facePtr = track->faces + shipIndex[weapon->fireShip].nearTrkSect->faceStart;
	
			while((facePtr->flags & TRACK_BASE) == 0)
			{
				facePtr++;
			}

			GetFireVector(		&weapon->acc,
							  	 	shipIndex[weapon->fireShip].unitVecNose,
				 					facePtr, 
									shipIndex[weapon->fireShip].ppivot 	);

			weapon->acc.vx = (weapon->acc.vx*60)/FR60;
			weapon->acc.vy = (weapon->acc.vy*60)/FR60;
			weapon->acc.vz = (weapon->acc.vz*60)/FR60;

			weapon->vel.vx = (shipIndex[weapon->fireShip].vpivot.vx>>6);
			weapon->vel.vy = (shipIndex[weapon->fireShip].vpivot.vy>>6);
			weapon->vel.vz = (shipIndex[weapon->fireShip].vpivot.vz>>6);

			weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx;
			weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy;
			weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz;

			SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
							shipIndex[weapon->fireShip].hdg,
							shipIndex[weapon->fireShip].pitch,
							shipIndex[weapon->fireShip].roll);


			weapon->updateWeapon = UpdateRocket;
			weapon->availible = 1;	//AJY
			return;	
		}
	}	
	weapon->count = NON_ACTIVE;
}

void UpdateElecBolt(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData* weapon, WeaponData* weaponIndex)
{ 

 	VECTOR 				distance;
	VECTOR 				targetVector;
	VECTOR 				bestPath;
	Face 					*facePtr;
	int 					i, mag, temp, vhdg, vpitch;
	TrackSection 		*nextSection;
	VECTOR 				facePoint;
	int 					alpha;

	if(weapon->targetShip > -1)
	{
		targetVector.vx = (shipIndex[weapon->targetShip].ppivot.vx - weapon->pos.vx)>>3;
		targetVector.vy = (shipIndex[weapon->targetShip].ppivot.vy - weapon->pos.vy)>>3;
		targetVector.vz = (shipIndex[weapon->targetShip].ppivot.vz - weapon->pos.vz)>>3;
		temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

		vhdg 	 = ((-ratan2(targetVector.vx, targetVector.vz)) - weapon->hdg);
		vpitch = ((-ratan2(targetVector.vy, temp)) - weapon->pitch);
	}
	else
	{
		vhdg 	 = 0;
		vpitch = 0;
	}
		if(vhdg > 2048) vhdg = vhdg - 4096;
		else if(vhdg < -2048) vhdg = vhdg + 4096;
		if(vpitch > 2048) vpitch = vpitch - 4096;
		else if(vpitch < -2048) vpitch = vpitch + 4096;

		weapon->hdg +=	vhdg>>2;
		weapon->pitch +=	vpitch>>2;

		weapon->hdg = ang(weapon->hdg);
		weapon->pitch = ang(weapon->pitch);

		weapon->acc.vx = (-rsin(weapon->hdg) * rcos(weapon->pitch))>>16;
		weapon->acc.vy = -rsin(weapon->pitch)>>4;
		weapon->acc.vz = (rcos(weapon->hdg) * rcos(weapon->pitch))>>16;

	  	weapon->acc.vx = (weapon->acc.vx*60)/FR60;
	  	weapon->acc.vy = (weapon->acc.vy*60)/FR60;
	  	weapon->acc.vz = (weapon->acc.vz*60)/FR60;

		weapon->vel.vx += weapon->acc.vx;
		weapon->vel.vy += weapon->acc.vy;
		weapon->vel.vz += weapon->acc.vz;

		weapon->vel.vx -= weapon->vel.vx>>2;
		weapon->vel.vy -= weapon->vel.vy>>2;
		weapon->vel.vz -= weapon->vel.vz>>2;

		weapon->pos.vx += weapon->vel.vx;
		weapon->pos.vy += weapon->vel.vy;
		weapon->pos.vz += weapon->vel.vz;

		weapon->nearTrkSect = FindNearestSection(weapon->nearTrkSect, weapon->pos);

		facePtr = track->faces + weapon->nearTrkSect->faceStart;
	
		while((facePtr->flags & TRACK_BASE) == 0)
		{
			facePtr++;
		}

		facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
		facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
		facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

		alpha = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);

		if(alpha < 2000)
		{
			weapon->pos.vx += (((200 - alpha) * (int)facePtr->normal.vx) >> 13);
			weapon->pos.vy += (((200 - alpha) * (int)facePtr->normal.vy) >> 13);
			weapon->pos.vz += (((200 - alpha) * (int)facePtr->normal.vz) >> 13);
		}


		weapon->count--;

		if(weapon->count == NON_ACTIVE)
		{
			weaponShapes[weapon->currentWeapon].used = 0;
			weaponIndex[weapon->currentWeapon].availible = 0;
		}

		SetSkeletonPosition((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
						weapon->pos.vx,
			  			weapon->pos.vy,
						weapon->pos.vz);

		SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
						weapon->hdg,
						weapon->pitch,
						0);

 	  weaponShapes[weapon->currentWeapon].shapePtr->skeleton->update = 1;
#if 0
  	
	TransformStaticObject( weaponShapes[weapon->currentWeapon].shapePtr, cameraPtr->camPos);
#endif

	/***********************************************************/
	/****   Perform collision checks against other ships     ***/
	/***********************************************************/
  
	for (i = 0; i < NO_OF_SHIPS; i++)
	{
		if ( ((i != weapon->fireShip) && (raceType != HEAD_TO_HEAD)) ||
			((raceType == HEAD_TO_HEAD) && ((i == ownShip) || (i == serialShip) ) && (i != weapon->fireShip) ) )
		{
			distance.vx = (shipIndex[i].ppivot.vx - weapon->pos.vx)>>3;
			distance.vy = (shipIndex[i].ppivot.vy - weapon->pos.vy)>>3;
			distance.vz = (shipIndex[i].ppivot.vz - weapon->pos.vz)>>3;
		
			mag = GetMagnitude(distance);
	 
			if(mag < 100)
			{
				if((shipIndex[i].attr & SHIELDED) == 0)
				{
					if(i != ownShip)
					{
						shipIndex[i].attr |= ELECTROED;
						shipIndex[i].electroCount = ELEC_DURATION;
					}
					else 
					{
						shipIndex[i].attr |= ELECTROED;
						shipIndex[i].electroCount = ELEC_DURATION;
					}
				}
				weapon->count = SHIP_EXPLODE;
				weapon->targetShip = i;
				weaponShapes[weapon->currentWeapon].used = 0;
				weaponIndex[weapon->currentWeapon].availible = 0;
				return;

			}
		}
	}

	/***********************************************************/
	/****   Perform collision checks against track 			   ***/
	/***********************************************************/
  
	facePtr = track->faces + weapon->nearTrkSect->faceStart;

	for(i = 0; i < weapon->nearTrkSect->faceCount; i++)
	{
		facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
		facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
		facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
		mag = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);
		if(mag <= 0)
		{
			weapon->count = TRACK_EXPLODE;
			weaponShapes[weapon->currentWeapon].used = 0;
			weaponIndex[weapon->currentWeapon].availible = 0;
			PlayDirectionalNote(EXPLOSION_2, 0, 40, weapon->pos);
			return;
		}
		facePtr++;
	}	
	Weapon2Mine(weapon, weaponIndex, weaponShapes);

}

void InitElecBolt(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData *weapon, WeaponData* weaponIndex)
{
	int i;

	if((weapon->fireShip != ownShip) && (weapon->fireShip != serialShip))
	{
		if(weapon->count > 1)
		{
			weapon->count--;
			return;
		}
		else
		{
			weapon->count = ELEC_DURATION;
		}
	}
	else
	{
		weapon->count = ELEC_DURATION;
	}

	weapon->nearTrkSect = shipIndex[weapon->fireShip].nearTrkSect;
	for(i = 0; i < NO_OF_EBOLTS; i++)
	{
		if(weaponShapes[EBOLT_OFFSET + i].used == 0)
		{
			weapon->currentWeapon = EBOLT_OFFSET + i;
			weaponShapes[EBOLT_OFFSET + i].used = 1;
			weapon->targetShip = shipIndex[weapon->fireShip].targetShip;

			weapon->acc.vx = shipIndex[weapon->fireShip].unitVecNose.vx>>8;
			weapon->acc.vy = shipIndex[weapon->fireShip].unitVecNose.vy>>8;
			weapon->acc.vz = shipIndex[weapon->fireShip].unitVecNose.vz>>8;

			weapon->acc.vx = (weapon->acc.vx*60)/FR60;
			weapon->acc.vy = (weapon->acc.vy*60)/FR60;
			weapon->acc.vz = (weapon->acc.vz*60)/FR60;

			weapon->vel.vx = (shipIndex[weapon->fireShip].vpivot.vx>>6);
			weapon->vel.vy = (shipIndex[weapon->fireShip].vpivot.vy>>6);
			weapon->vel.vz = (shipIndex[weapon->fireShip].vpivot.vz>>6);

		  	weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx;
			weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy;
			weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz;

			SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					shipIndex[weapon->fireShip].hdg,
					shipIndex[weapon->fireShip].pitch,
					shipIndex[weapon->fireShip].roll);

			weapon->hdg   = shipIndex[weapon->fireShip].hdg;
			weapon->pitch = shipIndex[weapon->fireShip].pitch;
			weapon->updateWeapon = UpdateElecBolt;
			weapon->availible = 1;	//AJY
			return;	
		}
	}	
	weapon->count = NON_ACTIVE;
}

void InitSpecial(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData *weapon, WeaponData* weaponIndex)
{
	int i;
	weapon->count--;
	for(i = 0; i < NO_OF_SHIPS; i++)
	{
		if(i != weapon->fireShip)
		{
			shipIndex[i].combatAttr |= SPECIALED;
			shipIndex[i].specialCount = SPECIAL_DURATION;
		}
	}
	weapon->availible = 1;	//AJY
}


void InitRevCon(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData *weapon, WeaponData* weaponIndex)
{
	int i;

	if((weapon->fireShip != ownShip) && (weapon->fireShip != serialShip))
	{
		if(weapon->count > 1)
		{
			weapon->count--;
			return;
		}
		else
		{
			weapon->count = ELEC_DURATION;
		}
	}
	else
	{
		weapon->count = ELEC_DURATION;
	}

	weapon->nearTrkSect = shipIndex[weapon->fireShip].nearTrkSect;
	for(i = 0; i < NO_OF_EBOLTS; i++)
	{
		if(weaponShapes[EBOLT_OFFSET + i].used == 0)
		{
			weapon->currentWeapon = EBOLT_OFFSET + i;
			weaponShapes[EBOLT_OFFSET + i].used = 1;
			weapon->targetShip = shipIndex[weapon->fireShip].targetShip;

			weapon->acc.vx = shipIndex[weapon->fireShip].unitVecNose.vx>>8;
			weapon->acc.vy = shipIndex[weapon->fireShip].unitVecNose.vy>>8;
			weapon->acc.vz = shipIndex[weapon->fireShip].unitVecNose.vz>>8;

			weapon->acc.vx = (weapon->acc.vx*60)/FR60;
			weapon->acc.vy = (weapon->acc.vy*60)/FR60;
			weapon->acc.vz = (weapon->acc.vz*60)/FR60;

			weapon->vel.vx = (shipIndex[weapon->fireShip].vpivot.vx>>6);
			weapon->vel.vy = (shipIndex[weapon->fireShip].vpivot.vy>>6);
			weapon->vel.vz = (shipIndex[weapon->fireShip].vpivot.vz>>6);

		  	weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx;
			weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy;
			weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz;

			SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
					shipIndex[weapon->fireShip].hdg,
					shipIndex[weapon->fireShip].pitch,
					shipIndex[weapon->fireShip].roll);

			weapon->hdg   = shipIndex[weapon->fireShip].hdg;
			weapon->pitch = shipIndex[weapon->fireShip].pitch;
			weapon->updateWeapon = UpdateRevCon;
			weapon->availible = 1;	//AJY
			return;	
		}
	}	
	weapon->count = NON_ACTIVE;
}

void UpdateRevCon(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData* weapon, WeaponData* weaponIndex)
{ 

 	VECTOR 				distance;
	VECTOR 				targetVector;
	VECTOR 				bestPath;
	Face 					*facePtr;
	int 					i, mag, temp, vhdg, vpitch;
	TrackSection 		*nextSection;
	VECTOR 				facePoint;
	int 					alpha;

	if(weapon->targetShip > -1)
	{
		targetVector.vx = (shipIndex[weapon->targetShip].ppivot.vx - weapon->pos.vx)>>3;
		targetVector.vy = (shipIndex[weapon->targetShip].ppivot.vy - weapon->pos.vy)>>3;
		targetVector.vz = (shipIndex[weapon->targetShip].ppivot.vz - weapon->pos.vz)>>3;
		temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

		vhdg 	 = ((-ratan2(targetVector.vx, targetVector.vz)) - weapon->hdg);
		vpitch = ((-ratan2(targetVector.vy, temp)) - weapon->pitch);
	}
	else
	{
		vhdg 	 = 0;
		vpitch = 0;
	}
		if(vhdg > 2048) vhdg = vhdg - 4096;
		else if(vhdg < -2048) vhdg = vhdg + 4096;
		if(vpitch > 2048) vpitch = vpitch - 4096;
		else if(vpitch < -2048) vpitch = vpitch + 4096;

		weapon->hdg +=	vhdg>>2;
		weapon->pitch +=	vpitch>>2;

		weapon->hdg = ang(weapon->hdg);
		weapon->pitch = ang(weapon->pitch);

		weapon->acc.vx = (-rsin(weapon->hdg) * rcos(weapon->pitch))>>16;
		weapon->acc.vy = -rsin(weapon->pitch)>>4;
		weapon->acc.vz = (rcos(weapon->hdg) * rcos(weapon->pitch))>>16;

	  	weapon->acc.vx = (weapon->acc.vx*60)/FR60;
	  	weapon->acc.vy = (weapon->acc.vy*60)/FR60;
	  	weapon->acc.vz = (weapon->acc.vz*60)/FR60;

		weapon->vel.vx += weapon->acc.vx;
		weapon->vel.vy += weapon->acc.vy;
		weapon->vel.vz += weapon->acc.vz;

		weapon->vel.vx -= weapon->vel.vx>>2;
		weapon->vel.vy -= weapon->vel.vy>>2;
		weapon->vel.vz -= weapon->vel.vz>>2;

		weapon->pos.vx += weapon->vel.vx;
		weapon->pos.vy += weapon->vel.vy;
		weapon->pos.vz += weapon->vel.vz;

		weapon->nearTrkSect = FindNearestSection(weapon->nearTrkSect, weapon->pos);

		facePtr = track->faces + weapon->nearTrkSect->faceStart;
	
		while((facePtr->flags & TRACK_BASE) == 0)
		{
			facePtr++;
		}

		facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
		facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
		facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

		alpha = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);

		if(alpha < 2000)
		{
			weapon->pos.vx += (((200 - alpha) * (int)facePtr->normal.vx) >> 13);
			weapon->pos.vy += (((200 - alpha) * (int)facePtr->normal.vy) >> 13);
			weapon->pos.vz += (((200 - alpha) * (int)facePtr->normal.vz) >> 13);
		}


		weapon->count--;

		if(weapon->count == NON_ACTIVE)
		{
			weaponShapes[weapon->currentWeapon].used = 0;
			weaponIndex[weapon->currentWeapon].availible = 0;
		}

		SetSkeletonPosition((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
						weapon->pos.vx,
			  			weapon->pos.vy,
						weapon->pos.vz);

		SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
						weapon->hdg,
						weapon->pitch,
						0);

 	  weaponShapes[weapon->currentWeapon].shapePtr->skeleton->update = 1;
#if 0
  	
	TransformStaticObject( weaponShapes[weapon->currentWeapon].shapePtr, cameraPtr->camPos);
#endif

	/***********************************************************/
	/****   Perform collision checks against other ships     ***/
	/***********************************************************/
  
	for (i = 0; i < NO_OF_SHIPS; i++)
	{
		if ( ((i != weapon->fireShip) && (raceType != HEAD_TO_HEAD)) ||
			((raceType == HEAD_TO_HEAD) && ((i == ownShip) || (i == serialShip) ) && (i != weapon->fireShip) ) )
		{
			distance.vx = (shipIndex[i].ppivot.vx - weapon->pos.vx)>>3;
			distance.vy = (shipIndex[i].ppivot.vy - weapon->pos.vy)>>3;
			distance.vz = (shipIndex[i].ppivot.vz - weapon->pos.vz)>>3;
		
			mag = GetMagnitude(distance);
	 
			if(mag < 100)
			{
				if((shipIndex[i].attr & SHIELDED) == 0)
				{
					if(i != ownShip)
					{
						shipIndex[i].combatAttr |= REVCONNED;
						shipIndex[i].revConCount = REV_CON_DURATION;
					}
					else 
					{
						shipIndex[i].combatAttr |= REVCONNED;
						shipIndex[i].revConCount = REV_CON_DURATION;
					}
				}
				weapon->count = SHIP_EXPLODE;
				weapon->targetShip = i;
				weaponShapes[weapon->currentWeapon].used = 0;
				weaponIndex[weapon->currentWeapon].availible = 0;
				return;

			}
		}
	}

	/***********************************************************/
	/****   Perform collision checks against track 			   ***/
	/***********************************************************/
  
	facePtr = track->faces + weapon->nearTrkSect->faceStart;

	for(i = 0; i < weapon->nearTrkSect->faceCount; i++)
	{
		facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
		facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
		facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
		mag = PlaneLineCollision (facePoint, weapon->pos, facePtr->normal);
		if(mag <= 0)
		{
			weapon->count = TRACK_EXPLODE;
			weaponShapes[weapon->currentWeapon].used = 0;
			weaponIndex[weapon->currentWeapon].availible = 0;
			PlayDirectionalNote(EXPLOSION_2, 0, 40, weapon->pos);
			return;
		}
		facePtr++;
	}	
	Weapon2Mine(weapon, weaponIndex, weaponShapes);

}


void InitMine(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData *weapon, WeaponData* weaponIndex)
{
	int i;

  	if(weapon->count == 1)
	{
		weapon->nearTrkSect = shipIndex[weapon->fireShip].nearTrkSect;

		for(i = 0; i < NO_OF_MINES; i++)
		{
			if(weaponShapes[MINE_OFFSET + i].used == 0)
			{
				weapon->currentWeapon = MINE_OFFSET + i;
				weaponShapes[MINE_OFFSET + i].used = 1;
				weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx;
				weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy;
				weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz;
				weapon->hdg = 0;
				if(weapon->fireShip == ownShip)
				{
					PlayNote(MINE_DROP, 0, 10);
				}
				weapon->updateWeapon = UpdateMine;
				weapon->count = MINE_DURATION;
				weapon->availible = 1;	//AJY

				return;	
			}
		}	
		weapon->count = NON_ACTIVE;

	}
	else if (weapon->count > 0) weapon->count--;

}


void InitShield(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData *weapon, WeaponData* weaponIndex)
{
	int i;

	weapon->nearTrkSect = shipIndex[weapon->fireShip].nearTrkSect;
	for(i = 0; i < NO_OF_SHIELDS; i++)
	{
		if(weaponShapes[SHIELD_OFFSET + i].used == 0)
		{
			weapon->currentWeapon = SHIELD_OFFSET + i;
			weaponShapes[SHIELD_OFFSET + i].used = 1;

			weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx;
			weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy;
			weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz;

			SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
							shipIndex[weapon->fireShip].hdg,
							shipIndex[weapon->fireShip].pitch,
							shipIndex[weapon->fireShip].roll);

			if(weapon->fireShip == ownShip)
			{
				PlayNote(SHIELDS, 0, -1);
				chanTable[vagTable[SHIELDS].channelID].s_attr.pitch = 0x0800;
			SpuSetVoiceAttr(&(chanTable[vagTable[SHIELDS].channelID].s_attr));
			}
			weapon->updateWeapon = UpdateShield;
			weapon->availible = 1;	//AJY
			return;	
		}
	}	
	weapon->count = NON_ACTIVE;
}

void UpdateShield(ShipData *shipIndex, WeaponShape* weaponShapes, WeaponData* weapon, WeaponData* weaponIndex)
{ 
 	VECTOR 		distance;
	Face 			*facePtr;
	int 			i, k, j, mag;
	VECTOR 		facePoint;
   Poly        poly;
   short       primitiveCount;
   StdPoly     prim;
   short*      coords;
	short			col0, col1, col2, col3;
	int 			shieldID;
	DVECTOR		screen0, screen1, screen2, screen3;


	if(shipIndex[weapon->fireShip].attr & VIEW_IN)
	{
		weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx + (shipIndex[weapon->fireShip].unitVecNose.vx>>6);
		weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy + (shipIndex[weapon->fireShip].unitVecNose.vy>>6);
		weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz + (shipIndex[weapon->fireShip].unitVecNose.vz>>6);
	}
	else
	{
		weapon->pos.vx = shipIndex[weapon->fireShip].ppivot.vx;
		weapon->pos.vy = shipIndex[weapon->fireShip].ppivot.vy;
		weapon->pos.vz = shipIndex[weapon->fireShip].ppivot.vz;
	}

	weapon->count--;

	if(weapon->count == 0)
	{
//		if (current_palette==SHIELD_PAL) 
//		{
//		 CopyPal(main_palette,0,255);
//		 current_palette=MAIN_PAL;
//		}

		weaponShapes[weapon->currentWeapon].used = 0;
		weaponIndex[weapon->currentWeapon].availible = 0;

		if(weapon->fireShip == ownShip)
		{
			KillNote(SHIELDS);
			Shielded = 0;
		}

		shipIndex[weapon->fireShip].attr &= ~SHIELDED;
//		return;
	}

	if((shipIndex[weapon->fireShip].attr & VIEW_IN) == 0)
	{
//		if (current_palette==SHIELD_PAL) 
//		{
//		 CopyPal(main_palette,0,255);
//		 current_palette=MAIN_PAL;
//		}

		SetSkeletonPosition((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
				weapon->pos.vx,
				weapon->pos.vy,
				weapon->pos.vz);

		SetSkeletonDirectionHPR((weaponShapes[weapon->currentWeapon].shapePtr->skeleton),
						shipIndex[weapon->fireShip].hdg,
						shipIndex[weapon->fireShip].pitch,
						shipIndex[weapon->fireShip].roll);

  		poly.prim = weaponShapes[weapon->currentWeapon].shapePtr->primitives;
	   	primitiveCount = weaponShapes[weapon->currentWeapon].shapePtr->primitiveCount;


		if(weapon->fireShip == ownShip)
			Shielded = 0;

	   	for ( k = 0; k < primitiveCount; k++ )
	   	{
	         switch ( poly.prim->type )
	  		 {
	   			case TypeG3 :
	        		/*	prim.g3 = poly.g3->tri + CurrentScreen;*/
	         		coords = poly.g3->coords;

					col0 = (rsin(weapon->count*coords[0])>>5) + 128;
					col1 = (rsin(weapon->count*coords[1])>>5) + 128;
					col2 = (rsin(weapon->count*coords[2])>>5) + 128;

					if(col0 > 255) col0 = 255;
					if(col1 > 255) col1 = 255;
					if(col2 > 255) col2 = 255;

					poly.g3->colour[ 0 ].r = (char)col0;
					poly.g3->colour[ 1 ].r = (char)col1;
					poly.g3->colour[ 2 ].r = (char)col2;
					poly.g3->colour[ 0 ].g = 0;
					poly.g3->colour[ 1 ].g = 0;
					poly.g3->colour[ 2 ].g = 0;
					poly.g3->colour[ 0 ].b = WHITE3_TRANSL; // PCwipeout

	      			poly.g3 += 1;
					break	;

      		case TypeG4 :
        		  /*	prim.g4 = poly.g4->quad + CurrentScreen;*/
	         		coords = poly.g4->coords;

					col0 = (rsin(weapon->count*coords[0])>>5) + 128;
					col1 = (rsin(weapon->count*coords[1])>>5) + 128;
					col2 = (rsin(weapon->count*coords[2])>>5) + 128;
					col3 = (rsin(weapon->count*coords[3])>>5) + 128;

					if(col0 > 255) col0 = 255;
					if(col1 > 255) col1 = 255;
					if(col2 > 255) col2 = 255;
					if(col3 > 255) col2 = 255;

					poly.g4->colour[ 0 ].r = (char)col0;
					poly.g4->colour[ 1 ].r = (char)col1;
					poly.g4->colour[ 2 ].r = (char)col2;
					poly.g4->colour[ 3 ].r = (char)col3;
					poly.g4->colour[ 0 ].g = 0;
					poly.g4->colour[ 1 ].g = 0;
					poly.g4->colour[ 2 ].g = 0;
					poly.g4->colour[ 3 ].g = 0;
					poly.g4->colour[ 0 ].b = WHITE3_TRANSL; // PCwipeout

	      			poly.g4 += 1;
					break	;
			}
      	}

   		weaponShapes[weapon->currentWeapon].shapePtr->skeleton->update = 1;
		shieldID = weapon->currentWeapon - SHIELD_OFFSET;
//		TransformTransparentObject( weaponShapes[weapon->currentWeapon].shapePtr, cameraPtr->camPos , shieldID);
  }
  else
  {
#if PCwipeout
		if (current_palette==MAIN_PAL) 
		{
		 CopyPal(shield_palette,0,255);
		 current_palette=SHIELD_PAL;
		}
  		poly.prim = weaponShapes[weapon->currentWeapon].shapePtr->primitives;
   	primitiveCount = weaponShapes[weapon->currentWeapon].shapePtr->primitiveCount;

		screen0.vx = 0;
		screen0.vy = 0;

		screen1.vx = 320;
		screen1.vy = 0;

#if PAL
		screen2.vx = 0;
		screen2.vy = 256;
		screen3.vx = 320;
		screen3.vy = 256;
#else
		screen2.vx = 0;
		screen2.vy = 240;
		screen3.vx = 320;
		screen3.vy = 240;
#endif

   	for ( k = 0; k < primitiveCount; k++ )
   	{
	   	switch ( poly.prim->type )
  		{
            case TypeG3 :
            	poly.g3 += 1;
					break;

      		case TypeG4 :
        		/*	prim.g4 = poly.g4->quad + CurrentScreen;*/
         		coords = poly.g4->coords;
               
               SetPolyG4( ( POLY_G4* )prims.ptr );
					SetSemiTrans(prims.ptr,	1);

               col0 = (rsin(weapon->count*coords[0]<<1)>>5) + 128;
					col1 = (rsin(weapon->count*coords[1]<<1)>>5) + 128;
					col2 = (rsin(weapon->count*coords[2]<<1)>>5) + 128;
					col3 = (rsin(weapon->count*coords[3]<<1)>>5) + 128;

					if(col0 > 255) col0 = 255;
					if(col1 > 255) col1 = 255;
					if(col2 > 255) col2 = 255;
					if(col3 > 255) col2 = 255;

					prims.g4->colour0.r = (char)col0;
					prims.g4->colour1.r = (char)col1;
					prims.g4->colour2.r = (char)col2;
					prims.g4->colour3.r = (char)col3;

               prims.g4->colour0.g = 0;
					prims.g4->colour1.g = 0;
					prims.g4->colour2.g = 0;
					prims.g4->colour3.g = 0;

               prims.g4->colour0.b = RED_TRANSL;

               prims.g4->xy0 = screen0;
               prims.g4->xy1 = screen1;
               prims.g4->xy2 = screen2;
               prims.g4->xy3 = screen3;
					
//   				AddPrim( OT[ CurrentScreen ] , ( ulong* ) prims.prim );
               prims.g4 += 1;

               shieldID = weapon->currentWeapon - SHIELD_OFFSET;
//      			AddPrim( OT[ CurrentScreen ] , ( ulong* ) &(shieldParams[(64 * shieldID)][CurrentScreen]));
               prims.drmode += 1;
  
           /*	poly.g4 += 1;*/
					return;
			
				default:
					break;
			}
      }
#endif
   }

}
POLY_F4	shieldPoly;

void DrawInternalShield(void)
{
	if (!DrawnShieldBefore)
	{
		setXY4(&shieldPoly,0,0,320,0,0,240,320,240);
		if (TextureTrack)
			setRGB0(&shieldPoly,0,0,WHITE2_TRANSL);
		else
			setRGB0(&shieldPoly,0,0,WHITE1_TRANSL);
		setPolyF4(&shieldPoly);
#if 0 //USE_ATI
		SetSemiTrans(&shieldPoly,1);
#endif
		AddPrim(OT[CurrentScreen] + 1,&shieldPoly); //Plus 1 to prevent hud from being affected
	}
	DrawnShieldBefore = 1;
}

void CycleWeaponGrids(ShipData *shipIndex, WeaponGrid *weaponGrid, int noWeaponGrids)
{
	int 			i, j;
	Face 			*facePtr;
	SVECTOR     tempColour;
	static		char	colourCount=CYCLE_START;
	
	colourCount++;
	if	(colourCount==CYCLE_END)	colourCount=CYCLE_START;
	for(i = 0; i < noWeaponGrids; i++)
	{
		facePtr = track->faces + weaponGrid[i].section->faceStart ;

		while((facePtr->flags & TRACK_BASE) == 0)
		{
			facePtr++ ;
		}

		for(j = 0; j < NO_OF_SHIPS; j++)
		{
			if((shipIndex[j].combatAttr & SPECIALED) == 0)
			{
				if(((facePtr->flags & WEAPON_GRID_LEFT)  && (shipIndex[j].attr & LSIDE)))
 				{
					if((weaponGrid[i].section == shipIndex[j].nearTrkSect)
							&& ( shipIndex[j].weaponType == NO_WEAPON) &&
							(weaponGrid[i].count >= 0))
					{
						weaponGrid[i].count = -30;
						if(j == ownShip)
						{
							PlayNote(POWER_UP, 0, 40);
							if (shipIndex[j].weaponType==SHIELD)
							{
								//shipIndex[j].weaponType = GetNewWeaponBodge();
								shipIndex[j].weaponType = GetNewWeapon();
							}
							else
								shipIndex[j].weaponType = GetNewWeapon();
						}
						else
							shipIndex[j].weaponType = 1;

					}
				}
			}
			facePtr++ ;
			if((shipIndex[j].combatAttr & SPECIALED) == 0)
			{
				if((facePtr->flags & WEAPON_GRID_RIGHT)  && ((shipIndex[j].attr & LSIDE) == 0))
 				{
					if((weaponGrid[i].section == shipIndex[j].nearTrkSect)
							&& ( shipIndex[j].weaponType == NO_WEAPON) &&
							(weaponGrid[i].count >= 0))
					{
						weaponGrid[i].count = -30;
						if(j == ownShip)
						{
							PlayNote(POWER_UP, 0, 40);
							if (shipIndex[j].weaponType==SHIELD)
								shipIndex[j].weaponType = GetNewWeaponBodge();
							else
								shipIndex[j].weaponType = GetNewWeapon();
						}
						else
							shipIndex[j].weaponType = 1;
					}
				}
			}
			facePtr-- ;
		}

		if((weaponGrid[i].count >= 0) && ((shipIndex[ownShip].combatAttr & SPECIALED) == 0))
		{
			tempColour.vx = (rsin(weaponGrid[i].cycleCount)>>5) + 128;
			tempColour.vy = (rcos(weaponGrid[i].cycleCount)>>5) + 128;
			tempColour.vz = (rsin(-weaponGrid[i].cycleCount)>>5) + 128;

			if(tempColour.vx > 255) tempColour.vx = 255;
			if(tempColour.vy > 255) tempColour.vy = 255;
			if(tempColour.vz > 255) tempColour.vz = 255;

#if 1 //!USE_ATI
			if (TextureTrack)
				{
				weaponGrid[i].colour.r = 230; // PCwipeout
				weaponGrid[i].colour.g = colourCount;
				weaponGrid[i].colour.b = 0;
				}
			else
			  {
				weaponGrid[i].colour.r = (&DepthFadeTble)[(colourCount<<8) + CYCLE_START + WhitePal]; // PCwipeout
				weaponGrid[i].colour.g = 0;
				weaponGrid[i].colour.b = 0;
			  }
#else
			weaponGrid[i].colour.r = tempColour.vx;
			weaponGrid[i].colour.g = tempColour.vy;
			weaponGrid[i].colour.b = tempColour.vz;
#endif
		}
		else
		{
			if (TextureTrack)
				{
#if 0 //USE_ATI
				weaponGrid[i].colour.r = 255;
				weaponGrid[i].colour.g = 255;
				weaponGrid[i].colour.b = 255;
#else
				weaponGrid[i].colour.r = 230; // PCwipeout
				weaponGrid[i].colour.g = 0;
				weaponGrid[i].colour.b = 0;
#endif
				}
			else
			  {
#if 0 //USE_ATI
				weaponGrid[i].colour.r = 128;
				weaponGrid[i].colour.g = 128;
				weaponGrid[i].colour.b = 128;
#else
				weaponGrid[i].colour.r = GreyPal; // PCwipeout
				weaponGrid[i].colour.g = 0;
				weaponGrid[i].colour.b = 0;
#endif
			  }
		}

		if(weaponGrid[i].count < 0)
		{
			weaponGrid[i].count++;
		}
		weaponGrid[i].cycleCount += 32;

		if(weaponGrid[i].attr & WEAPON_GRID_LEFT)
		{
			facePtr->red =   weaponGrid[i].colour.r;
			facePtr->green = weaponGrid[i].colour.g;
			facePtr->blue =  weaponGrid[i].colour.b;
		}
		if(weaponGrid[i].attr & WEAPON_GRID_RIGHT)
		{
			facePtr++ ;
			facePtr->red =   weaponGrid[i].colour.r;
			facePtr->green = weaponGrid[i].colour.g;
			facePtr->blue =  weaponGrid[i].colour.b;
		}
	}
}




#define     ClipFlag    ( ( CrtClipped | ZLarge | ZNegative | ZSmall ) >> 12 )
extern SVECTOR     StaticVertex[  ];
extern DVECTOR     StaticScreen[  ];
extern ushort      StaticDepth[  ];
extern ushort      StaticFlags[  ];

void GetFireVector(VECTOR *result, VECTOR target, Face *facePtr, VECTOR shipPos)
{
	int 			alpha, origAlpha;
	VECTOR 		facePoint;

	facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
	facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
	facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

	target.vx = shipPos.vx + (target.vx>>6);
	target.vy = shipPos.vy + (target.vy>>6);
	target.vz = shipPos.vz + (target.vz>>6);

	alpha = PlaneLineCollision (facePoint, target, facePtr->normal);
	origAlpha = PlaneLineCollision (facePoint, shipPos, facePtr->normal);

	alpha = (alpha * 31) >> 5;
	
	alpha -= origAlpha;

	result->vx = target.vx - ((alpha * facePtr->normal.vx)>>12);
	result->vy = target.vy - ((alpha * facePtr->normal.vy)>>12);
	result->vz = target.vz - ((alpha * facePtr->normal.vz)>>12);

	result->vx -= shipPos.vx;
	result->vy -= shipPos.vy;
	result->vz -= shipPos.vz;
}

void Weapon2Mine(WeaponData *weapon, WeaponData *weaponIndex, WeaponShape *weaponShapes)
{
	int 			i;
	VECTOR		distance;
	int 			mag;
	
	for(i = 0; i < NO_ACTIVE_WEAPONS; i++)
	{
		if((weaponIndex[i].type == MINE) && (weaponIndex[i].count > 0))
		{
			distance.vx = (weaponIndex[i].pos.vx - weapon->pos.vx)>>2;
			distance.vy = (weaponIndex[i].pos.vy - weapon->pos.vy)>>2;
			distance.vz = (weaponIndex[i].pos.vz - weapon->pos.vz)>>2;
		
			mag = GetMagnitude(distance);
	 
			if(mag < 30)
			{
				weaponIndex->count = NON_ACTIVE;
				weaponShapes[weaponIndex->currentWeapon].used = 0;
				weapon->count = TRACK_EXPLODE;
				weaponShapes[weapon->currentWeapon].used = 0;
				PlayDirectionalNote(EXPLOSION_2, 0, 40, weapon->pos);
				return;
			}
		}
	}
}


int GetNewWeapon()
{
	int index;
	int weaponType;

	if(gameType == SINGLE)
	{
		index = randy() % 65;

		if(index < 17)
			weaponType = ROCKET;
		else if(index < 35)
			weaponType = MINE;
		else if(index < 45)
			weaponType = SHIELD;
		else if(index < 53)
			weaponType = MISSILE;
		else if(index < 59)
			weaponType = TURBO;
		else if(index < 65)
			weaponType = ELEC_BOLT;
	}
	else
	{
		index = randy() % 70;		 /* 70 */

		if(index < 17)
			weaponType = ROCKET;
		else if(index < 35)
			weaponType = MINE;
		else if(index < 45)
			weaponType = SHIELD;
		else if(index < 53)
			weaponType = MISSILE;
		else if(index < 59)
			weaponType = TURBO;
		else if(index < 65)
			weaponType = ELEC_BOLT;
		else if(index < 69)
			weaponType = REV_CON;
		else if(index < 70)		 /* 70 */
			weaponType = SPECIAL;
	}

	return(weaponType);
}

int GetNewWeaponBodge()
{
	int index;
	int weaponType;

	if(gameType == SINGLE)
	{
		index = randy() % 60;

		if(index < 27)
			weaponType = ROCKET;
		else if(index < 40)
			weaponType = MISSILE;
		else if(index < 50)
			weaponType = TURBO;
		else if(index < 60)
			weaponType = ELEC_BOLT;
	}
	else
	{
		index = randy() % 65;		 /* 70 */

		if(index < 27)
			weaponType = ROCKET;
		else if(index < 40)
			weaponType = MISSILE;
		else if(index < 50)
			weaponType = TURBO;
		else if(index < 60)
			weaponType = ELEC_BOLT;
		else if(index < 69)
			weaponType = REV_CON;
		else if(index < 70)		 /* 70 */
			weaponType = SPECIAL;
	}
	return(weaponType);
}



SetTargetShip(ShipData *shipIndex, int shipNo)
{
	int 						i;
	int 						distance, shortestDistance;
	int 						nearShip;	
	TrackSection			*section;
	int 						target;
	int 						start;

		shortestDistance = 999;

		for( i = NO_OF_SHIPS-1; i >= 0; i--)
		{
			if ( ((i != shipNo) && (raceType != HEAD_TO_HEAD)) ||
				((raceType == HEAD_TO_HEAD) && ((i == ownShip) || (i == serialShip) ) && (i != shipNo) ) )
			{
				/*** Is Player ship upon a junction branch **/

				if(shipIndex[shipNo].nearTrkSect->flags & JunctionSection)
				{
					/*** If remote ship on same branch the attempt to target ***/
					if(shipIndex[i].nearTrkSect->flags & JunctionSection)
					{
						distance = shipIndex[i].nearTrkSect->secNo - shipIndex[shipNo].nearTrkSect->secNo;

						if((distance < shortestDistance) && (distance > 0))
						{
							shortestDistance = distance;
	 						nearShip = i;
						}
					}
					else
					{
						/*** Remote ship is not same branch ***/
						section = shipIndex[shipNo].nearTrkSect;

						for(distance = 0; distance < 10; distance++)
						{
							section = section->nextSection.ptr;
							if(shipIndex[i].nearTrkSect == section)
							{
								if((distance < shortestDistance) && (distance > 0))
								{
									shortestDistance = distance;
	 								nearShip = i;
									break;
								}
							}
						}
					}
				}
				else
				{
					if(shipIndex[i].nearTrkSect->flags & JunctionSection)
					{
						section = shipIndex[shipNo].nearTrkSect;
						for(distance = 0; distance < 10; distance++)
						{
							if(section->junction.index != -1)
							{
								section = section->junction.ptr;
							}
							else
							{
								section = section->nextSection.ptr;
							}
							if(shipIndex[i].nearTrkSect == section)
							{
								if((distance < shortestDistance) && (distance > 0))
								{
									shortestDistance = distance;
	 								nearShip = i;
									break;
								}
							}
						}
					}
					else
					{
						distance = shipIndex[i].nearTrkSect->secNo - shipIndex[shipNo].nearTrkSect->secNo;

						if((distance < shortestDistance) && (distance > 0))
						{
							shortestDistance = distance;
	 						nearShip = i;
						}
					}
				}
			}
		}

		if(shortestDistance < 10)
		{
			shipIndex[shipNo].targetShip = nearShip;
		}
		else shipIndex[shipNo].targetShip = 0xFF;
		
}
