#include <stdlib.h>
#include <string.h>
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "menus.h"
#include "global.h"
#include "effects.h"
#include "init.h"
#include "ajs.h"
#include "win.h"
#include "error.h"
#include "libgte.h"
#include "collide.h"
#include "small_ptr.h"

extern float upres;

char tranny_table[] = {YELLOW_WHITE_TRANSL,		 // MISSILE/ROCKET EXPLODE HIT NOTHING
	WHITE2_TRANSL,	// MISSILE/ROCKET TRAIL
	YELLOW_TRANSL,	// MISSILE/ROCKEY EXPLODE HIT SHIP
	WHITE8_TRANSL,	// SHADOW NOT USED
	BLUE_TRANSL,	// EBOLT TRAIL
	WHITE8_TRANSL,	// HALO NOT USED
	PURPLE_TRANSL};	// GREENY (NOT USED?) WAS REV_CON SO USE FOR EBOLT EXPLODE

void InitEffects(Effect* effects, Skeleton *camPos, Texture** effectTextures)
{
	int16_t loc = LoadCompressedTextureSequence( "wipeout/common/effects.cmp" );
	effectTextures[FIRE] 		= TextureTable[loc++];
	effectTextures[FIRE_WHITE] = TextureTable[loc++];
	effectTextures[SMOKE] 		= TextureTable[loc++];
	effectTextures[EBOLT] 		= TextureTable[loc++];
	effectTextures[HALO] 		= TextureTable[loc++];
	effectTextures[GREENY] 		= TextureTable[loc++];

	for(int32_t i = 0; i < NO_OF_EFFECTS; i++)
	{
		effects[i].count = 0;

		for(int32_t j = 0; j < 2; j++)
		{
			SetPolyFT4(&(effects[i].sprite[j]));
			SetSemiTrans((P_TAG *)&(effects[i].sprite[j]), 1);
			SetShadeTex((P_TAG *)&(effects[i].sprite[j]), 1);

			effects[i].sprite[j].r0 = 230;	// PCwipeout
			effects[i].sprite[j].g0 = 0;
			effects[i].sprite[j].b0 = tranny_table[SMOKE];  //Default is smoke
			setTPage(&(effects[i].sprite[j]),0,3,
				effectTextures[SMOKE]->textureX,
				effectTextures[SMOKE]->textureY);
			effects[i].sprite[j].clut = effectTextures[SMOKE]->cba;
			setUV4(&(effects[i].sprite[j]),
				effectTextures[SMOKE]->u0, effectTextures[SMOKE]->v0,
				effectTextures[SMOKE]->u1, effectTextures[SMOKE]->v1,
				effectTextures[SMOKE]->u2, effectTextures[SMOKE]->v2,
				effectTextures[SMOKE]->u3, effectTextures[SMOKE]->v3) ;
		}
	}
}

void UpdateEffects(Effect* effects)
{
	int32_t 			i;
	VECTOR 		camPos;

	camPos.vx = cameraPtr->camPos->relative.t[0] ;
	camPos.vy = cameraPtr->camPos->relative.t[1] ;
	camPos.vz = cameraPtr->camPos->relative.t[2] ;

	cameraPtr->camPos->super->update = 1;
	SetSkeletonTransform( cameraPtr->camPos->super );

	for(i = 0; i < NO_OF_EFFECTS; i++)
	{
		if(effects[i].count > 0)
		{
			effects[i].pos.vx	+=	effects[i].vel.vx;
			effects[i].pos.vy	+=	effects[i].vel.vy;
			effects[i].pos.vz	+=	effects[i].vel.vz;

			effects[i].count--;
		}
		else if (effects[i].count < -6)
		{
			effects[i].count++;
		}
		else if (effects[i].count == -6)
		{
			effects[i].count = (char)randy()>>3;
		}
	}
}

void DrawEffects(Effect* effects)
{
	int32_t 			i;
	SVECTOR 		vertex;
	DVECTOR 		sxy;
	int32_t 			p, flag;
	int32_t 			x, y;
	int32_t 			depth;
	int16_t 		size;
	int32_t			count = 0;
	VECTOR 		camPos;

	camPos.vx = cameraPtr->camPos->relative.t[0] ;
	camPos.vy = cameraPtr->camPos->relative.t[1] ;
	camPos.vz = cameraPtr->camPos->relative.t[2] ;

	cameraPtr->camPos->super->update = 1;
	SetSkeletonTransform( cameraPtr->camPos->super );

	for(i = 0; i < NO_OF_EFFECTS; i++)
	{
		if(effects[i].count > 0)
		{
#if 0
			effects[i].pos.vx	+=	effects[i].vel.vx;
			effects[i].pos.vy	+=	effects[i].vel.vy;
			effects[i].pos.vz	+=	effects[i].vel.vz;

			effects[i].count--;
#endif

			vertex.vx = effects[i].pos.vx + camPos.vx ;
			vertex.vy = effects[i].pos.vy + camPos.vy ;
			vertex.vz = effects[i].pos.vz + camPos.vz ;

			depth = RotTransPers(&(vertex),
				&sxy,
				&p,
				&flag);

			x = sxy.vx;
			y = sxy.vy;

			if(!(flag & (CrtClipped | ZNegative | ZLarge | ZSmall)))
			{

				/** effects[i].size is the shift right */
				/** The bit mask is to set the maximum */
				/** screen size to avoid framing out   */

				size = ( 160 << effects[i].size ) / depth;

				if(size > 32 ) size = 32;
				x = x - (size >> 1);
				y = y - (size >> 1);


				effects[i].sprite[ CurrentScreen ].x0 = x;
				effects[i].sprite[ CurrentScreen ].y0 = y;
				effects[i].sprite[ CurrentScreen ].x1 = x + size;
				effects[i].sprite[ CurrentScreen ].y1 = y;
				effects[i].sprite[ CurrentScreen ].x2 = x;
				effects[i].sprite[ CurrentScreen ].y2 = y + size;
				effects[i].sprite[ CurrentScreen ].x3 = x + size;
				effects[i].sprite[ CurrentScreen ].y3 = y + size;
				effects[i].sprite[ CurrentScreen ].z0 = vertex.vz;
				effects[i].sprite[ CurrentScreen ].z1 = vertex.vz;
				effects[i].sprite[ CurrentScreen ].z2 = vertex.vz;
				effects[i].sprite[ CurrentScreen ].z3 = vertex.vz;
				effects[i].sprite[ CurrentScreen ].pad4 = 0;
				effects[i].sprite[ CurrentScreen ].pad5 = 0;
				effects[i].sprite[ CurrentScreen ].pad6 = 0;
				effects[i].sprite[ CurrentScreen ].pad7 = 0;

				AddPrim( OT[ CurrentScreen ], depth, (P_TAG*)&effects[i].sprite[ CurrentScreen ] );
			}
		}
	}
}

void AddEffects(Effect* effects, WeaponData *weaponIndex, Texture** effectTextures, ShipData *shipIndex)
{
	int32_t i, j, k;
	int32_t exploType;
	int16_t noToRelease = 0;

	for(i = 0; i < NO_ACTIVE_WEAPONS; i++)
	{
		/***********************************************************************/
		/**** 			Add Fire + Smoke Trails for Missiles & Rockets		  ****/
		/***********************************************************************/

		if((weaponIndex[i].type == ROCKET) || (weaponIndex[i].type == MISSILE) || (weaponIndex[i].type == ELEC_BOLT) ||
			(weaponIndex[i].type == REV_CON))
		{
			if(weaponIndex[i].type == ELEC_BOLT)
				exploType = EBOLT;
			else if(weaponIndex[i].type == REV_CON)
				exploType = GREENY;
			else
				exploType = SMOKE;

			if(weaponIndex[i].count > 0)
			{
				for(j = 0; j < NO_OF_EFFECTS; j++)
				{
					if(effects[j].count == 0)
					{
						if(noToRelease == 0)
						{
							effects[j].pos.vx = weaponIndex[i].pos.vx;
							effects[j].pos.vy = weaponIndex[i].pos.vy;
							effects[j].pos.vz = weaponIndex[i].pos.vz;
							effects[j].vel.vx = (int8_t)randy()>>4;
							effects[j].vel.vy = (int8_t)randy()>>4;
							effects[j].vel.vz = (int8_t)randy()>>4;
							effects[j].count = 20 + (randy() % 10);
							for(k = 0; k < 2; k++)
							{
								effects[j].size = 7;
								effects[j].sprite[k].b0 = tranny_table[exploType];  //Set the colour
								effects[j].sprite[k].clut = effectTextures[exploType]->cba;
								setUV4(&(effects[j].sprite[k]),
									effectTextures[exploType]->u0, effectTextures[exploType]->v0,
									effectTextures[exploType]->u1, effectTextures[exploType]->v1,
									effectTextures[exploType]->u2, effectTextures[exploType]->v2,
									effectTextures[exploType]->u3, effectTextures[exploType]->v3) ;
							}
						}
						else if(noToRelease == 1)
						{
							effects[j].pos.vx = weaponIndex[i].pos.vx - (weaponIndex[i].vel.vx>>1);
							effects[j].pos.vy = weaponIndex[i].pos.vy - (weaponIndex[i].vel.vy>>1);
							effects[j].pos.vz = weaponIndex[i].pos.vz - (weaponIndex[i].vel.vz>>1);
							effects[j].vel.vx = 0;
							effects[j].vel.vy = 0;
							effects[j].vel.vz = 0;
							effects[j].count = 5;
							for(k = 0; k < 2; k++)
							{
								effects[j].size = 7;
								if(weaponIndex[i].type == ELEC_BOLT)
								{
									effects[j].sprite[k].clut = effectTextures[EBOLT]->cba;
								}
								else if(weaponIndex[i].type == REV_CON)
								{
									effects[j].sprite[k].clut = effectTextures[GREENY]->cba;
								}
								else
								{
									effects[j].sprite[k].clut = effectTextures[FIRE]->cba;
								}
								effects[j].sprite[k].b0 = tranny_table[exploType];  //Set the colour
								setUV4(&(effects[j].sprite[k]),
									effectTextures[exploType]->u0, effectTextures[exploType]->v0,
									effectTextures[exploType]->u1, effectTextures[exploType]->v1,
									effectTextures[exploType]->u2, effectTextures[exploType]->v2,
									effectTextures[exploType]->u3, effectTextures[exploType]->v3) ;
							}
						}
						else if(noToRelease == 2)
						{
							effects[j].pos.vx = weaponIndex[i].pos.vx - (weaponIndex[i].vel.vx>>2);
							effects[j].pos.vy = weaponIndex[i].pos.vy - (weaponIndex[i].vel.vy>>2);
							effects[j].pos.vz = weaponIndex[i].pos.vz - (weaponIndex[i].vel.vz>>2);
							effects[j].vel.vx = (int8_t)randy()>>4;
							effects[j].vel.vy = (int8_t)randy()>>4;
							effects[j].vel.vz = (int8_t)randy()>>4;
							effects[j].count = 20 + (randy() % 10);
							for(k = 0; k < 2; k++)
							{
								effects[j].size = 7;
								effects[j].sprite[k].clut = effectTextures[exploType]->cba;
								effects[j].sprite[k].b0 = tranny_table[exploType];  //Set the colour
								setUV4(&(effects[j].sprite[k]),
									effectTextures[exploType]->u0, effectTextures[exploType]->v0,
									effectTextures[exploType]->u1, effectTextures[exploType]->v1,
									effectTextures[exploType]->u2, effectTextures[exploType]->v2,
									effectTextures[exploType]->u3, effectTextures[exploType]->v3) ;
							}
						}
						noToRelease++;
						if(noToRelease == 3) break;
					}
				}
			}
		}

		noToRelease = 0;

		if((weaponIndex[i].count == SHIP_EXPLODE) ||
			(weaponIndex[i].count == TRACK_EXPLODE)	)
		{
			if(weaponIndex[i].count == SHIP_EXPLODE)
			{
				if(weaponIndex[i].type == ELEC_BOLT)
				{
					exploType = GREENY; // PCwipeout NOW PURPLE
				}
				else if(weaponIndex[i].type == REV_CON)
				{
					exploType = GREENY;
				}
				else
					exploType = FIRE_WHITE;
			}
			else
			{
				if(weaponIndex[i].type == ELEC_BOLT)
				{
					exploType = EBOLT;
				}
				else if(weaponIndex[i].type == REV_CON)
				{
					exploType = GREENY;
				}
				else
					exploType = FIRE;
			}

			for(j = 0; j < NO_OF_EFFECTS; j++)
			{
				if(effects[j].count == 0)
				{
					effects[j].pos.vx = weaponIndex[i].pos.vx;
					effects[j].pos.vy = weaponIndex[i].pos.vy;
					effects[j].pos.vz = weaponIndex[i].pos.vz;

					effects[j].vel.vx = (int8_t)randy()>>2;
					effects[j].vel.vy = (int8_t)randy()>>2;
					effects[j].vel.vz = (int8_t)randy()>>2;

					if(weaponIndex[i].count == SHIP_EXPLODE)
					{
						effects[j].vel.vx += (int16_t)(shipIndex[weaponIndex[i].targetShip].vpivot.vx >> 6);
						effects[j].vel.vy += (int16_t)(shipIndex[weaponIndex[i].targetShip].vpivot.vy >> 6);
						effects[j].vel.vz += (int16_t)(shipIndex[weaponIndex[i].targetShip].vpivot.vz >> 6);
					}

					effects[j].count = -((((char)randy())>>4) + 8);

					for(k = 0; k < 2; k++)
					{
						effects[j].size = 8;
						effects[j].sprite[k].b0 = tranny_table[exploType];  //Set the colour
						setUV4(&(effects[j].sprite[k]),
							effectTextures[EBOLT]->u0, effectTextures[EBOLT]->v0,
							effectTextures[EBOLT]->u1, effectTextures[EBOLT]->v1,
							effectTextures[EBOLT]->u2, effectTextures[EBOLT]->v2,
							effectTextures[EBOLT]->u3, effectTextures[EBOLT]->v3) ;
						effects[j].sprite[k].clut = effectTextures[exploType]->cba;
					}
					noToRelease++;
					if(noToRelease == 30) return;

				}
			}
			weaponIndex[i].count = NON_ACTIVE;
		}
	}
}

void ColourStartBoom(int32_t noToLight)
{
	int32_t			      i, j;
	char 			      r, g, b;

	Prm               libPoly;
	Object            *obj;


	for( i=0; i<startBoomCount; i++ )
	{
		obj = startBoom[i];

		libPoly.primitive = obj->primitives;

		if(noToLight > 1)
		{
			for(j = 1; j < noToLight; j++)
			{
				libPoly.gt4 += 1;
			}
		}

		if(noToLight == 1)
		{
			r = 0xff;
			g = 0x00;
			b = 0x00;
		}
		else if(noToLight == 2)
		{
			r = 0xff;
			g = 0x80;
			b = 0x00;
		}
		else if(noToLight == 3)
		{
			r = 0x00;
			g = 0xff;
			b = 0x00;
		}

		libPoly.gt4->colour[ 0 ].r = r;
		libPoly.gt4->colour[ 0 ].g = g;
		libPoly.gt4->colour[ 0 ].b = b;

		libPoly.gt4->colour[ 1 ].r = r;
		libPoly.gt4->colour[ 1 ].g = g;
		libPoly.gt4->colour[ 1 ].b = b;

		libPoly.gt4->colour[ 2 ].r = r;
		libPoly.gt4->colour[ 2 ].g = g;
		libPoly.gt4->colour[ 2 ].b = b;

		libPoly.gt4->colour[ 3 ].r = r;
		libPoly.gt4->colour[ 3 ].g = g;
		libPoly.gt4->colour[ 3 ].b = b;
	}
}



void ResetStartBoom()
{
	Object* 		 obj;
	int32_t			 i, j;

	Prm          libPoly;


	for( i=0; i<startBoomCount; i++ )
	{
		obj = startBoom[i];

		libPoly.primitive = obj->primitives; // may have to be libpoly.primitives

		for(j = 0; j < 3; j++)
		{
			libPoly.gt4->colour[ 0 ].r = 0;
			libPoly.gt4->colour[ 0 ].g = 0;
			libPoly.gt4->colour[ 0 ].b = 0;

			libPoly.gt4->colour[ 1 ].r = 0;
			libPoly.gt4->colour[ 1 ].g = 0;
			libPoly.gt4->colour[ 1 ].b = 0;

			libPoly.gt4->colour[ 2 ].r = 0;
			libPoly.gt4->colour[ 2 ].g = 0;
			libPoly.gt4->colour[ 2 ].b = 0;

			libPoly.gt4->colour[ 3 ].r = 0;
			libPoly.gt4->colour[ 3 ].g = 0;
			libPoly.gt4->colour[ 3 ].b = 0;

			libPoly.gt4++;
		}
	}

}

void InitStartBoom(Object** prm, int16_t prmCount)
{
	int32_t 			i;
	Object      *obj;

	startBoomCount = 0;

	for( i = 0; i < prmCount; i++)
	{
		obj = prm[ i ];
		while ( obj )
		{
			if(!strncmp(obj->name, "start", 5))
			{
				startBoom[ startBoomCount ] = obj;
				startBoomCount++;
			}

			if(startBoomCount > 4)
			{
				Error( "Too many start booms !!!!!\n", Fatal );
			}

			obj = obj->next;
		}
	}
}

void InitRedLight(Object** prm, int16_t prmCount, Object **redLight)
{
	int32_t 			i;
	Object      *obj;

	for( i = 0; i < prmCount; i++)
	{
		obj = prm[ i ];
		while ( obj )
		{
			if(!strncmp(obj->name, "red", 3))
			{
				*redLight = obj;
			}
			obj = obj->next;
		}
	}
}

void ColourRedLight(Object *redLight)
{
	int16_t 			      r;
	static int16_t			count = 0;

	Prm               libPoly;



	count += 140;
	r = (rsin(count) >> 5) + 128;
	if(r > 255) r= 255;

	libPoly.primitive = redLight->primitives;

	libPoly.gt4->colour[ 0 ].r = (char)r;
	libPoly.gt4->colour[ 1 ].r = (char)r;
	libPoly.gt4->colour[ 2 ].r = (char)r;
	libPoly.gt4->colour[ 3 ].r = (char)r;
	libPoly.gt4->colour[ 0 ].g = 0x00;
	libPoly.gt4->colour[ 1 ].g = 0x00;
	libPoly.gt4->colour[ 2 ].g = 0x00;
	libPoly.gt4->colour[ 3 ].g = 0x00;
	libPoly.gt4->colour[ 0 ].b = 0x00;
	libPoly.gt4->colour[ 1 ].b = 0x00;
	libPoly.gt4->colour[ 2 ].b = 0x00;
	libPoly.gt4->colour[ 3 ].b = 0x00;

}

void InitDonkey(Object** prm, int16_t prmCount, Object **donkey)
{
	int32_t 			i;
	Object      *obj;
	int32_t 			count = 0;

	for( i = 0; i < prmCount; i++)
	{
		obj = prm[ i ];
		while ( obj )
		{
			if(!strncmp(obj->name, "donkey", 6))
			{
				donkey[count] = obj;
				count++;

				//				printf("Donkey = %s \n", &(obj->name) );
			}
			obj = obj->next;
		}
	}
}

void MoveDonkey(Object **donkey)
{
	int32_t 				i;
	Object      	*obj;
	static int16_t	count = 0;

	for( i = 0; i < 2; i++)
	{
		obj = donkey[ i ];

		SetSkeletonDirection(obj->skeleton,
			rsin(count)>>3,
			0,
			0 );

	}
	count += 20;
}


Shadow *InitShadow(Skeleton *camPos, Texture** effectTextures)
{
	int32_t i, j, k;
	Shadow *shadow;

	char	u[6], v[6];

	if(ownShip < 2)
		effectTextures[SHADOW] = LoadTexture ( "wipeout/textures/shad1.tim", 1 ) ;
	else if(ownShip < 4)
		effectTextures[SHADOW] = LoadTexture ( "wipeout/textures/shad2.tim", 1 ) ;
	else if(ownShip < 6)
		effectTextures[SHADOW] = LoadTexture ( "wipeout/textures/shad3.tim", 1 ) ;
	else if(ownShip < 8)
		effectTextures[SHADOW] = LoadTexture ( "wipeout/textures/shad4.tim", 1 ) ;


	shadow = ( Shadow* )DAlloc( heap, (sizeof( Shadow) * NO_OF_SHIPS) + 32 );

	u[0] = (effectTextures[SHADOW]->u0 + effectTextures[SHADOW]->u1) >> 1;
	v[0] = (effectTextures[SHADOW]->v0 + effectTextures[SHADOW]->v1) >> 1;
	u[1] = effectTextures[SHADOW]->u2;
	v[1] = effectTextures[SHADOW]->v2;
	u[2] = effectTextures[SHADOW]->u3;
	v[2] = effectTextures[SHADOW]->v3;
	u[3] = (u[0] + u[1]) >> 1;
	v[3] = (v[0] + v[1]) >> 1;
	u[4] = (u[1] + u[2]) >> 1;
	v[4] = (v[1] + v[2]) >> 1;
	u[5] = (u[2] + u[0]) >> 1;
	v[5] = (v[2] + v[0]) >> 1;


	for(i = 0; i < NO_OF_SHIPS; i++)
	{
		for(j = 0; j < 2; j++)
		{
			for(k = 0; k < 4; k++)
			{
				SetPolyFT3(&(shadow[i].poly[k][j]));
				setTPage(&(shadow[i].poly[k][j]),0, 2,
					effectTextures[SHADOW]->textureX,
					effectTextures[SHADOW]->textureY);
				shadow[i].poly[k][j].clut = effectTextures[SHADOW]->cba;

				SetShadeTex((P_TAG *)&(shadow[i].poly[k][j]), 1);

				SetSemiTrans((P_TAG *)&(shadow[i].poly[k][j]), 1);

				shadow[i].poly[k][j].r0 = 230;	// PCwipeout
				shadow[i].poly[k][j].g0 = 0;
				shadow[i].poly[k][j].b0 = BLACK5_TRANSL;
			}
			setUV3(&(shadow[i].poly[0][j]),
				u[0], v[0],
				u[3], v[3],
				u[5], v[5]);
			setUV3(&(shadow[i].poly[1][j]),
				u[3], v[3],
				u[1], v[1],
				u[4], v[4]);
			setUV3(&(shadow[i].poly[2][j]),
				u[5], v[5],
				u[3], v[3],
				u[4], v[4]);
			setUV3(&(shadow[i].poly[3][j]),
				u[5], v[5],
				u[4], v[4],
				u[2], v[2]);
		}
	}
	return(shadow);
}

void DrawShadow(Object **shipShapes, ShipData *shipIndex, Shadow *shadow)
{

	Face 			*facePtr;
	VECTOR 		a, b, c;
	int32_t 			i;
	int32_t 			alpha;
	VECTOR 		facePoint;
	DVECTOR		sxy[3];
	int32_t        p, flag;
	int32_t 			x[6], y[6];
	int32_t 			depth[3];
	SVECTOR 		vertex[3];

	cameraPtr->camPos->super->update = 1;
	SetSkeletonTransform( cameraPtr->camPos->super );

	for(i = 0; i < NO_OF_SHIPS; i++)
	{
		if( ((raceType == HEAD_TO_HEAD) && ((i == ownShip ) || (i == serialShip)))	 ||
			((raceType == TIME_TRIAL) && (i == ownShip)) ||
			((raceType	!= TIME_TRIAL) && (raceType != HEAD_TO_HEAD)) )
		{

			if((shipIndex[i].attr & VISIBLE) && ((shipIndex[i].attr & FLYING) == 0 ))
			{
				facePtr = track->faces + shipIndex[i].nearTrkSect->faceStart;

				while((facePtr->flags & TRACK_BASE) == 0)
				{
					facePtr++;
				}

				a.vx = (shipIndex[i].ppivot.vx + ((shipIndex[i].unitVecNose.vx*3)>>5));
				a.vy = (shipIndex[i].ppivot.vy + ((shipIndex[i].unitVecNose.vy*3)>>5));
				a.vz = (shipIndex[i].ppivot.vz + ((shipIndex[i].unitVecNose.vz*3)>>5));

				b.vx = (shipIndex[i].ppivot.vx - ((shipIndex[i].unitVecNose.vx*3)>>5)) + (shipIndex[i].unitVecWing.vx>>4);
				b.vy = (shipIndex[i].ppivot.vy - ((shipIndex[i].unitVecNose.vy*3)>>5)) + (shipIndex[i].unitVecWing.vy>>4);
				b.vz = (shipIndex[i].ppivot.vz - ((shipIndex[i].unitVecNose.vz*3)>>5)) + (shipIndex[i].unitVecWing.vz>>4);

				c.vx = (shipIndex[i].ppivot.vx - ((shipIndex[i].unitVecNose.vx*3)>>5)) - (shipIndex[i].unitVecWing.vx>>4);
				c.vy = (shipIndex[i].ppivot.vy - ((shipIndex[i].unitVecNose.vy*3)>>5)) - (shipIndex[i].unitVecWing.vy>>4);
				c.vz = (shipIndex[i].ppivot.vz - ((shipIndex[i].unitVecNose.vz*3)>>5)) - (shipIndex[i].unitVecWing.vz>>4);

				facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
				facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
				facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

				alpha = PlaneLineCollision (facePoint, a, facePtr->normal);
				a.vx -= (alpha * facePtr->normal.vx)>>12  ;
				a.vy -= (alpha * facePtr->normal.vy)>>12  ;
				a.vz -= (alpha * facePtr->normal.vz)>>12  ;

				alpha = PlaneLineCollision (facePoint, b, facePtr->normal);
				b.vx -= (alpha * facePtr->normal.vx)>>12  ;
				b.vy -= (alpha * facePtr->normal.vy)>>12  ;
				b.vz -= (alpha * facePtr->normal.vz)>>12  ;

				alpha = PlaneLineCollision (facePoint, c, facePtr->normal);
				c.vx -= (alpha * facePtr->normal.vx)>>12  ;
				c.vy -= (alpha * facePtr->normal.vy)>>12  ;
				c.vz -= (alpha * facePtr->normal.vz)>>12  ;



				vertex[0].vx = a.vx + cameraPtr->camPos->relative.t[ 0 ];
				vertex[0].vy = a.vy + cameraPtr->camPos->relative.t[ 1 ];
				vertex[0].vz = a.vz + cameraPtr->camPos->relative.t[ 2 ];

				vertex[1].vx = b.vx + cameraPtr->camPos->relative.t[ 0 ];
				vertex[1].vy = b.vy + cameraPtr->camPos->relative.t[ 1 ];
				vertex[1].vz = b.vz + cameraPtr->camPos->relative.t[ 2 ];

				vertex[2].vx = c.vx + cameraPtr->camPos->relative.t[ 0 ];
				vertex[2].vy = c.vy + cameraPtr->camPos->relative.t[ 1 ];
				vertex[2].vz = c.vz + cameraPtr->camPos->relative.t[ 2 ];


				/*** Depth returned is 1/4 the Z component sz of the screen coords 	***/
				/*** corresponding to v2														 	***/
				/*** 				Faster but inaccurate sorting	!!!!						 	***/
#if 0
				depth = RotTransPers3(	&(vertex[0]),
					&(vertex[1]),
					&(vertex[2]),
					&(sxy[0]),
					&(sxy[1]),
					&(sxy[2]),
					&p,
					&flag);

#else
				/*** 					Depth returned for each vertex 							***/
				/*** 				Slower but slightly better sorting	!!!!					***/

				depth[0] = RotTransPers(	&(vertex[0]),
					&(sxy[0]),
					&p,
					&flag);

				depth[1] = RotTransPers(	&(vertex[1]),
					&(sxy[1]),
					&p,
					&flag);

				depth[2] = RotTransPers(	&(vertex[2]),
					&(sxy[2]),
					&p,
					&flag);
#endif

				x[0] = sxy[0].vx;
				y[0] = sxy[0].vy;
				x[1] = sxy[1].vx;
				y[1] = sxy[1].vy;
				x[2] = sxy[2].vx;
				y[2] = sxy[2].vy;


				x[3] = ( x[0] + x[1] ) >> 1;
				y[3] = ( y[0] + y[1] ) >> 1;
				x[4] = ( x[1] + x[2] ) >> 1;
				y[4] = ( y[1] + y[2] ) >> 1;
				x[5] = ( x[2] + x[0] ) >> 1;
				y[5] = ( y[2] + y[0] ) >> 1;


				if(!(flag & (CrtClipped | ZNegative | ZLarge | ZSmall)))
				{
					// TODO(boardwalk) This was filling in the z values with stuff from vertex with bad indices, but
					// there doesn't seem to be any reason to fill the z with anything at all, since the primitives
					// already transformed, so I'm just clearing them instead.
					shadow[i].poly[0][ CurrentScreen ].x0 = x[0];
					shadow[i].poly[0][ CurrentScreen ].y0 = y[0];
					shadow[i].poly[0][ CurrentScreen ].x1 = x[3];
					shadow[i].poly[0][ CurrentScreen ].y1 = y[3];
					shadow[i].poly[0][ CurrentScreen ].x2 = x[5];
					shadow[i].poly[0][ CurrentScreen ].y2 = y[5];

					shadow[i].poly[0][ CurrentScreen ].z0 = 0;
					shadow[i].poly[0][ CurrentScreen ].z1 = 0;
					shadow[i].poly[0][ CurrentScreen ].z2 = 0;
					shadow[i].poly[0][ CurrentScreen ].pad4 = 0;
					shadow[i].poly[0][ CurrentScreen ].pad5 = 0;
					shadow[i].poly[0][ CurrentScreen ].pad6 = 0;

					shadow[i].poly[1][ CurrentScreen ].x0 = x[3];
					shadow[i].poly[1][ CurrentScreen ].y0 = y[3];
					shadow[i].poly[1][ CurrentScreen ].x1 = x[1];
					shadow[i].poly[1][ CurrentScreen ].y1 = y[1];
					shadow[i].poly[1][ CurrentScreen ].x2 = x[4];
					shadow[i].poly[1][ CurrentScreen ].y2 = y[4];

					shadow[i].poly[1][ CurrentScreen ].z0 = 0;
					shadow[i].poly[1][ CurrentScreen ].z1 = 0;
					shadow[i].poly[1][ CurrentScreen ].z2 = 0;
					shadow[i].poly[1][ CurrentScreen ].pad4 = 0;
					shadow[i].poly[1][ CurrentScreen ].pad5 = 0;
					shadow[i].poly[1][ CurrentScreen ].pad6 = 0;

					shadow[i].poly[2][ CurrentScreen ].x0 = x[5];
					shadow[i].poly[2][ CurrentScreen ].y0 = y[5];
					shadow[i].poly[2][ CurrentScreen ].x1 = x[3];
					shadow[i].poly[2][ CurrentScreen ].y1 = y[3];
					shadow[i].poly[2][ CurrentScreen ].x2 = x[4];
					shadow[i].poly[2][ CurrentScreen ].y2 = y[4];

					shadow[i].poly[2][ CurrentScreen ].z0 = 0;
					shadow[i].poly[2][ CurrentScreen ].z1 = 0;
					shadow[i].poly[2][ CurrentScreen ].z2 = 0;
					shadow[i].poly[2][ CurrentScreen ].pad4 = 0;
					shadow[i].poly[2][ CurrentScreen ].pad5 = 0;
					shadow[i].poly[2][ CurrentScreen ].pad6 = 0;

					shadow[i].poly[3][ CurrentScreen ].x0 = x[5];
					shadow[i].poly[3][ CurrentScreen ].y0 = y[5];
					shadow[i].poly[3][ CurrentScreen ].x1 = x[4];
					shadow[i].poly[3][ CurrentScreen ].y1 = y[4];
					shadow[i].poly[3][ CurrentScreen ].x2 = x[2];
					shadow[i].poly[3][ CurrentScreen ].y2 = y[2];

					shadow[i].poly[3][ CurrentScreen ].z0 = 0;
					shadow[i].poly[3][ CurrentScreen ].z1 = 0;
					shadow[i].poly[3][ CurrentScreen ].z2 = 0;
					shadow[i].poly[3][ CurrentScreen ].pad4 = 0;
					shadow[i].poly[3][ CurrentScreen ].pad5 = 0;
					shadow[i].poly[3][ CurrentScreen ].pad6 = 0;

					depth[0] -= 180;
					depth[1] -= 60;
					depth[2] -= 60;

					AddPrim( OT[ CurrentScreen ], depth[0] , (P_TAG*)&shadow[i].poly[0][ CurrentScreen ] );
					AddPrim( OT[ CurrentScreen ], depth[2] , (P_TAG*)&shadow[i].poly[1][ CurrentScreen ] );
					AddPrim( OT[ CurrentScreen ], depth[2] , (P_TAG*)&shadow[i].poly[2][ CurrentScreen ] );
					AddPrim( OT[ CurrentScreen ], depth[2] , (P_TAG*)&shadow[i].poly[3][ CurrentScreen ] );
				}
			}
		}
	}
}

void InitWeaponSprtIcons(SPRT *weaponIcon, Texture** weaponIconTextures)
{
	int16_t loc = LoadCompressedTextureSequence( "wipeout/common/wicons.cmp" );
	weaponIconTextures[MINE-1] 		= TextureTable[loc++];
	weaponIconTextures[MISSILE-1] 	= TextureTable[loc++];
	weaponIconTextures[ROCKET-1] 		= TextureTable[loc++];
	weaponIconTextures[SPECIAL-1]		= TextureTable[loc++];
	weaponIconTextures[ELEC_BOLT-1] 	= TextureTable[loc++];
	weaponIconTextures[FLARE-1] 		= TextureTable[loc++];
	weaponIconTextures[REV_CON-1] 	= TextureTable[loc++];
	weaponIconTextures[SHIELD-1] 		= TextureTable[loc++];
	weaponIconTextures[TURBO-1] 		= TextureTable[loc++];

	for(int32_t i = 0; i < 2; i++)
	{
		SetSprt(&(weaponIcon[i]));

		weaponIcon[i].r0 = 230;	// PCwipeout
		weaponIcon[i].g0 = 0;
		weaponIcon[i].b0 = BLACK_NOT_DRAWN;

		weaponIcon[i].x0 = 144;
		weaponIcon[i].y0 = 20;
		weaponIcon[i].w  = 32;
		weaponIcon[i].h  = 32;
	}
}


void DrawWeaponSprtIcons(SPRT *weaponIcon, Texture** weaponIconTextures, ShipData *playerShip, DR_MODE *drawmode, DR_MODE *drawmode2)
{
	int32_t 	icon;
	RECT	tw;


	icon = playerShip->weaponType;

	if(icon != NO_WEAPON)
	{
		icon -= 1;

		SetDrawMode(drawmode2, 0, 0, FONT_TPAGE, &tw);
		AddPrim(OT[CurrentScreen], 0, (P_TAG*)drawmode2);

		weaponIcon[CurrentScreen].x0 = 144 * upres;
		weaponIcon[CurrentScreen].y0 = 20 * upres;
		weaponIcon[CurrentScreen].u0 = weaponIconTextures[icon]->u0;
		weaponIcon[CurrentScreen].v0 = weaponIconTextures[icon]->v0;
		AddPrim(OT[CurrentScreen], 0, (P_TAG*)&weaponIcon[ CurrentScreen ] );

		SetDrawMode(drawmode, 0, 0, 22, &tw);
		AddPrim(OT[CurrentScreen], 0, (P_TAG*)drawmode);
	}
}

static int32_t shakeCount = 0;

void SetShake(int32_t duration)
{
	shakeCount = duration;
}

void ShakeScreen()
{

	int32_t	tempx, tempy;

	if(shakeCount > 1)
	{
		DispEnv0.screen.x = DispEnv1.screen.x = -(randy()%shakeCount) + (shakeCount>>1);
		DispEnv0.screen.y = DispEnv1.screen.y = -(randy()%shakeCount) + (shakeCount>>1);
		shakeCount--;
	}
	else if(shakeCount == 1)
	{
		DispEnv0.screen.x = DispEnv1.screen.x = 0;
		DispEnv0.screen.y = DispEnv1.screen.y = 0;
		shakeCount--;
	}
	tempx = abs(DispEnv0.screen.x >> 3);
	tempy = abs(DispEnv0.screen.y * 80);

	//	SStart(tempx + tempy);
}

extern int16_t arccos_tab[];

GT4					*abPtr[80];
int16_t					*abCoords[80];
int16_t 		greyCoords[80];

#if 1
void InitAuroraBorialis(Object* object)
{
	int32_t			      i;
	Poly        poly;
	int16_t       primitiveCount;
	int16_t*      coords;
	int32_t 			count = 0;
	int16_t			y;
	SVECTOR*    vertex;

	poly.prim = object->primitives;
	primitiveCount = object->primitiveCount;
	vertex = object->vertices;

	for ( i=0; i< 80; i++ )
	{
		greyCoords[i] = 0;
	}

	for ( i=0; i<primitiveCount; i++ )
	{
		switch ( poly.prim->type )
		{
		case TypeGT3 :
            poly.gt3 += 1;
			break;

		case TypeGT4 :

            coords = poly.gt4->coords;
			y = vertex[coords[0]].vy;
			//				printf("y = %d\n", y);
			if(y < -6000)									 /* -8000 */
			{
				abPtr[count] = poly.gt4;
				if(y > -6800)
				{
					abCoords[count] = poly.gt4->coords;
					greyCoords[count] = -1;
				}
				else if(y < -11000)
				{
					abCoords[count] = poly.gt4->coords;
					greyCoords[count] = -2;
				}
				else
					abCoords[count] = poly.gt4->coords;
				count++;
			}
            poly.gt4 += 1;
			break;
		}

	}
	//	printf("Count = %d\n", count);
}

void AuroraBorialis(Object* object)
{
	int32_t			      i;

	Poly        poly;
	int16_t*      coords;
	static int32_t	count = 0;

	count++;

	poly.prim = object->primitives;

	for ( i = 0; i < 80; i++ )
	{
		/*   prim.gt4 = abPtr[i] + CurrentScreen;*/
		coords = abCoords[i];

		if(greyCoords[i] != -2)
		{
			abPtr[i]->colour[ 0 ].r = (rsin(coords[0]*count) >> 6) + 190;
			abPtr[i]->colour[ 0 ].g = (rsin(coords[0]*(count+54)) >> 6) + 190;
			abPtr[i]->colour[ 0 ].b = (rsin(coords[0]*(count+39)) >> 6) + 190;
		}
		if(greyCoords[i] != -2)
		{
			abPtr[i]->colour[ 1 ].r = (rsin(coords[1]*count) >> 6) + 190;
			abPtr[i]->colour[ 1 ].g = (rsin(coords[1]*(count+54)) >> 6) + 190;
			abPtr[i]->colour[ 1 ].b = (rsin(coords[1]*(count+39)) >> 6) + 190;
		}
		if(greyCoords[i] != -1)
		{
			abPtr[i]->colour[ 2 ].r = (rsin(coords[2]*count) >> 6) + 190;
			abPtr[i]->colour[ 2 ].g = (rsin(coords[2]*(count+54)) >> 6) + 190;
			abPtr[i]->colour[ 2 ].b = (rsin(coords[2]*(count+39)) >> 6) + 190;
		}

		if(greyCoords[i] != -1)
		{
			abPtr[i]->colour[ 3 ].r = (rsin(coords[3]*count) >> 6) + 190;
			abPtr[i]->colour[ 3 ].g = (rsin(coords[3]*(count+54)) >> 6) + 190;
			abPtr[i]->colour[ 3 ].b = (rsin(coords[3]*(count+39)) >> 6) + 190;
		}

		/*				printf("col = %d\n", prim.gt4->colour3.r); */

	}
}



#endif
