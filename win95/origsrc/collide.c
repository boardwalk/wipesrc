#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "dynam.h"
#include "search.h"
#include "collide.h"
#include "droid.h"
#include "camera.h"
#include "menus.h"
#include "combat.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"


void CallShip2ShipCollision(ShipData *shipIndex, Object** shipShapes)
{
	int i, j;


	if(gameType == SLAVE)
	{
		if(raceType == HEAD_TO_HEAD)
		{
			 Ship2ShipCollision(shipIndex, shipShapes, serialShip, ownShip);
		}
		else
		{
			for(i = 0; i < NO_OF_SHIPS; i++)
			{
				if(i != ownShip)
					Ship2ShipCollision(shipIndex, shipShapes, i, ownShip);
			}
		}
	}
	else	  /****** May have to cancel reaction of serial ship ******/
	{
		if(raceType == HEAD_TO_HEAD)
		{
			 Ship2ShipCollision(shipIndex, shipShapes, serialShip, ownShip);
		}
		else if(raceType != TIME_TRIAL)
		{
			for(j = 0; j < (NO_OF_SHIPS-1); j++)
			{
				for(i = j+1; i < NO_OF_SHIPS; i++)
				{
		 			Ship2ShipCollision(shipIndex, shipShapes, i, j);
				}
			}
		}
	}		

}

void Ship2ShipCollision(ShipData *shipIndex, Object** shipShapes, int i, int j)
{
	int mag;
	VECTOR distance;
	VECTOR posVector;
	int collide;
	VECTOR vc; 
	int ownMass, remMass;
	VECTOR ship2ship;
	int dotProduct;
	int angle;
	VECTOR ship1React, ship2React;
	VECTOR soundPos;
	VECTOR sparkPos;

	distance.vx = (shipIndex[i].ppivot.vx - shipIndex[j].ppivot.vx)>>3;
	distance.vy = (shipIndex[i].ppivot.vy - shipIndex[j].ppivot.vy)>>3;
	distance.vz = (shipIndex[i].ppivot.vz - shipIndex[j].ppivot.vz)>>3;

	mag = GetMagnitude(distance);

	if (mag < 120) 				/*** Do line/plane collision check ***/
	{
		collide = collideLineToPlane(j, i, shipShapes, shipIndex);

		if(collide == NOSE_COLLISION)
		{
			/**********************************************************/
			/*** 			Calculate velocity of zero-momentum frame	  ***/
			/***																	  ***/
			/***							vc = m1v1 + m2v2					  ***/
			/***								  -----------					  ***/
			/***									 m1 + m2						  ***/
			/**********************************************************/

			ownMass = shipIndex[j].mass;
			remMass = shipIndex[i].mass;

			vc.vx = ((shipIndex[j].vpivot.vx * ownMass) + (shipIndex[i].vpivot.vx * remMass)) /
														(ownMass + remMass);
			vc.vy = ((shipIndex[j].vpivot.vy * ownMass) + (shipIndex[i].vpivot.vy * remMass)) /
														(ownMass + remMass);
			vc.vz = ((shipIndex[j].vpivot.vz * ownMass) + (shipIndex[i].vpivot.vz * remMass)) /
														(ownMass + remMass);


			ship1React.vx = (vc.vx - shipIndex[j].vpivot.vx)>>1; 
			ship1React.vy = (vc.vy - shipIndex[j].vpivot.vy)>>1; 
			ship1React.vz = (vc.vz - shipIndex[j].vpivot.vz)>>1; 

			ship2React.vx = (vc.vx - shipIndex[i].vpivot.vx)>>1; 
			ship2React.vy = (vc.vy - shipIndex[i].vpivot.vy)>>1; 
			ship2React.vz = (vc.vz - shipIndex[i].vpivot.vz)>>1; 
 
			shipIndex[j].ppivot.vx -= sar(shipIndex[j].vpivot.vx,6);
			shipIndex[j].ppivot.vy -= sar(shipIndex[j].vpivot.vy,6);
			shipIndex[j].ppivot.vz -= sar(shipIndex[j].vpivot.vz,6);

			shipIndex[i].ppivot.vx -= sar(shipIndex[i].vpivot.vx,6);
			shipIndex[i].ppivot.vy -= sar(shipIndex[i].vpivot.vy,6);
			shipIndex[i].ppivot.vz -= sar(shipIndex[i].vpivot.vz,6);

			/**************** YOUR CRAFT REACTION *********************/
			shipIndex[j].vpivot.vx = vc.vx + ship1React.vx;
			shipIndex[j].vpivot.vy = vc.vy + ship1React.vy;
			shipIndex[j].vpivot.vz = vc.vz + ship1React.vz;

			/*************** REMOTE CRAFT REACTION ********************/
			shipIndex[i].vpivot.vx = vc.vx + ship2React.vx;
			shipIndex[i].vpivot.vy = vc.vy + ship2React.vy;
			shipIndex[i].vpivot.vz = vc.vz + ship2React.vz;

			ship2ship.vx = shipIndex[i].ppivot.vx - shipIndex[j].ppivot.vx;
			ship2ship.vy = shipIndex[i].ppivot.vy - shipIndex[j].ppivot.vy;
			ship2ship.vz = shipIndex[i].ppivot.vz - shipIndex[j].ppivot.vz;

			shipIndex[i].vpivot.vx += ship2ship.vx<<2;
			shipIndex[i].vpivot.vy += ship2ship.vy<<2;
			shipIndex[i].vpivot.vz += ship2ship.vz<<2;

			shipIndex[j].vpivot.vx -= ship2ship.vx<<2;
			shipIndex[j].vpivot.vy -= ship2ship.vy<<2;
			shipIndex[j].vpivot.vz -= ship2ship.vz<<2;

			shipIndex[j].vpivot.vx = (shipIndex[j].vpivot.vx*60)/FR60;
			shipIndex[j].vpivot.vy = (shipIndex[j].vpivot.vy*60)/FR60;
			shipIndex[j].vpivot.vz = (shipIndex[j].vpivot.vz*60)/FR60;

			shipIndex[i].vpivot.vx = (shipIndex[i].vpivot.vx*60)/FR60;
			shipIndex[i].vpivot.vy = (shipIndex[i].vpivot.vy*60)/FR60;
			shipIndex[i].vpivot.vz = (shipIndex[i].vpivot.vz*60)/FR60;

			shipIndex[j].ppivot.vx += sar(shipIndex[j].vpivot.vx,6);
			shipIndex[j].ppivot.vy += sar(shipIndex[j].vpivot.vy,6);
			shipIndex[j].ppivot.vz += sar(shipIndex[j].vpivot.vz,6);

			shipIndex[i].ppivot.vx += sar(shipIndex[i].vpivot.vx,6);
			shipIndex[i].ppivot.vy += sar(shipIndex[i].vpivot.vy,6);
			shipIndex[i].ppivot.vz += sar(shipIndex[i].vpivot.vz,6);

			if((shipIndex[i].attr & SHIP_COLL) == 0)
			{
				if((shipIndex[j].attr & SHIP_COLL) == 0)
				{ 
					soundPos.vx = (shipIndex[i].ppivot.vx + shipIndex[j].ppivot.vx)>>1;
					soundPos.vy = (shipIndex[i].ppivot.vy + shipIndex[j].ppivot.vy)>>1;
					soundPos.vz = (shipIndex[i].ppivot.vz + shipIndex[j].ppivot.vz)>>1;
					PlayDirectionalNote(SHIP_2_SHIP, 0, 30, soundPos);
				}
			}
			shipIndex[i].attr |= SHIP_COLL;
			shipIndex[j].attr |= SHIP_COLL;

		}
	}
	else
	{
		shipIndex[i].attr &= ~SHIP_COLL;
		shipIndex[j].attr &= ~SHIP_COLL;
	}
}


/* ****************************************************************
	Line to plane collision routine

	alpha = ((p-s).n)/(d.n)

	X = s + alpha*d

	p = one point of the poly
	s = start of the line
	n = normal
	d = direction of the line
******************************************************************** */	

int collideLineToPlane(int shipNo1, int shipNo2, Object** shipShapes, ShipData *shipIndex)
{
	int i;						/* index count (planes) */
	int count;					/* index count (lines) */
   Poly        poly;
   short*      coords;
   short       primitiveCount;

	VECTOR p1, p2, p3;		/* the three plane points of other ship */

	VECTOR a, b, c, d;		/* four vector points of the hero ship */

	VECTOR shifted;			/* centre of the other ships plane */

	int factor2;				/* intermediate calculation terms */
	VECTOR factor1;
	VECTOR term1;

	VECTOR res_vec;			/* resultant intersection vector */
	int alpha;					/* the above alpha value */

	VECTOR DC, BC, AC ;	   /* position vectors of hero ship */
	VECTOR   BD, AB;
	VECTOR 	  AD;

	VECTOR start_vec;			/* is assigned a, b, c or d from above */
	VECTOR pos_vec;			/* is assigned DC, BC, AC, BD, AB or AD */

	VECTOR plane1;				/* normal of plane of other ship */

	VECTOR P1P2;	  			/* 2 position vectors of other ship */
	VECTOR P1P3;				/* (all that is needed to get normal) */

	VECTOR vec0, vec1, vec2;/* vectors between plane coords and intersection point */
	int angle;					/* total angle */

	short			high1, high2;
	short			low1, low2;


	high1 = shipIndex[shipNo1].highResTableIndex;
	low1 	= shipIndex[shipNo1].lowResTableIndex;
	high2 = shipIndex[shipNo2].highResTableIndex;
	low2 	= shipIndex[shipNo2].lowResTableIndex;


	/** Get 4 points of collision model relative to the  **/
	/**                   camera								  **/

	getWorldPoint(&a, shipShapes[high2], shipShapes[low2],0);
	getWorldPoint(&b, shipShapes[high2], shipShapes[low2],1);
	getWorldPoint(&c, shipShapes[high2], shipShapes[low2],2);
	getWorldPoint(&d, shipShapes[high2], shipShapes[low2],3);


	/**    Calculate the 6 line vectors for the model    **/

	MinusVector(a, c, &AC);
	MinusVector(b, c, &BC);
	MinusVector(d, c, &DC);	
	MinusVector(a, b, &AB);
	MinusVector(b, d, &BD);
	MinusVector(a, d, &AD);

   poly.prim = shipShapes[low2]->primitives;
   primitiveCount = shipShapes[low2]->primitiveCount;

	for(i = 0; i < primitiveCount; i++)		/* for all 4 planes of the enemy ship */
	{
      switch ( poly.prim->type )
      {
			case TypeF3:
	  		coords = poly.f3->coords;

			getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
			getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
			getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
      
			poly.f3++;

  			break;

			case TypeG3:
      	coords = poly.g3->coords;									 

			getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
			getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
			getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
     	 
			poly.g3++;
			break;

			case TypeFT3:
	  		coords = poly.ft3->coords;

			getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
			getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
			getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
      
			poly.ft3++;

  			break;

			case TypeGT3:
	  		coords = poly.gt3->coords;

			getWorldPoint(&p1, shipShapes[high1], shipShapes[low1], coords[0]);
			getWorldPoint(&p2, shipShapes[high1], shipShapes[low1], coords[1]);
			getWorldPoint(&p3, shipShapes[high1], shipShapes[low1], coords[2]);
      
			poly.gt3++;

  			break;

			default:
				break;
		}
	
		/* ********************************************** */
		/*        Find polyGon line vectors					  */
		/* ********************************************** */
		MinusVector(p1, p2, &P1P2);
		MinusVector(p1, p3, &P1P3);
		
		/* ********************************************** */
		/*             Find plane equations					  */
		/* ********************************************** */
		CrossProduct(&plane1, P1P2, P1P3); 

		for (count=0; count<6; count++)
		{
			switch(count)
			{
				case 0:
					assign(&start_vec, b);
					assign(&pos_vec, BC);
					break;
				case 1:
					assign(&start_vec, a);
					assign(&pos_vec, AC);
					break;
				case 2:
					assign(&start_vec, d);
					assign(&pos_vec, DC);
					break;
				case 3:
					assign(&start_vec, a);
					assign(&pos_vec, AB);
					break;
				case 4:
					assign(&start_vec, a);
					assign(&pos_vec, AD);
					break;
				case 5:
					assign(&start_vec, b);
					assign(&pos_vec, BD);
					break;
			}

			MinusVector(start_vec, p1, &factor1);		  /* (p-s) */
			alpha = (ScalarProduct(factor1, plane1) << 12);		/* (p-s).n */
			factor2 = (ScalarProduct(pos_vec, plane1) );	/* d.n */
			if (factor2 != 0)
			{
				alpha = alpha/factor2;

				term1.vx = ((alpha * pos_vec.vx) >> 12);
				term1.vy = ((alpha * pos_vec.vy) >> 12);
				term1.vz = ((alpha * pos_vec.vz) >> 12);

				res_vec.vx = start_vec.vx + term1.vx;
				res_vec.vy = start_vec.vy + term1.vy;
				res_vec.vz = start_vec.vz + term1.vz;


				if ( (alpha>=0) && (alpha<=4096) )
				{			
					vec0.vx = res_vec.vx - p1.vx; 
					vec0.vy = res_vec.vy - p1.vy; 
					vec0.vz = res_vec.vz - p1.vz; 

					vec1.vx = res_vec.vx - p2.vx; 
					vec1.vy = res_vec.vy - p2.vy; 
					vec1.vz = res_vec.vz - p2.vz; 

					vec2.vx = res_vec.vx - p3.vx; 
					vec2.vy = res_vec.vy - p3.vy; 
					vec2.vz = res_vec.vz - p3.vz; 

					angle =  GetAng(vec0, vec1);
					angle += GetAng(vec1, vec2);
					angle += GetAng(vec2, vec0);

 					if ( (angle >= 30000) )
					{
						return 1;
					}
				}
 			}
		}
	}
	return 0;
}

int assign(VECTOR *vec1, VECTOR vec2)
{
	vec1->vx = vec2.vx;
	vec1->vy = vec2.vy;
	vec1->vz = vec2.vz;
	return(1); // PCwipeout
}

int PlaneLineCollision (VECTOR plane_point, VECTOR ship_pos, SVector normal)
{
	int dot_product, norm_dot_product ;
	int coll_data ;

	dot_product = (plane_point.vx - ship_pos.vx) * normal.vx;
	dot_product += (plane_point.vy - ship_pos.vy) * normal.vy;
	dot_product += (plane_point.vz - ship_pos.vz) * normal.vz;

	norm_dot_product = ( ( (-normal.vx) * normal.vx) >> 12 ) + 
							 ( ( (-normal.vy) * normal.vy) >> 12 ) +
						    ( ( (-normal.vz) * normal.vz) >> 12 );

	coll_data = dot_product / norm_dot_product ;

	return (coll_data) ;
}


void Ship2Track(ShipData *playerShip, Face *facePtr, Object** shipShapes )
{
	VECTOR 			nosePos, wingPos;
	int 				alpha;
	VECTOR 			direction;
	int 				downTrack;
	TrackSection 	*trackPtr;
	int 				toFace;
	VECTOR 			toFaceVector;
	VECTOR 			test;
	int 				i;
	VECTOR 			facePoint;
	Face 				*rightFacePtr;
	int 				collide;
	Face				*facePtr2;

	nosePos.vx = playerShip->ppivot.vx + (playerShip->unitVecNose.vx>>3);
	nosePos.vy = playerShip->ppivot.vy + (playerShip->unitVecNose.vy>>3);
	nosePos.vz = playerShip->ppivot.vz + (playerShip->unitVecNose.vz>>3);

	wingPos.vx = playerShip->ppivot.vx - (playerShip->unitVecWing.vx>>4) - (playerShip->unitVecNose.vx>>4);
	wingPos.vy = playerShip->ppivot.vy - (playerShip->unitVecWing.vy>>4) - (playerShip->unitVecNose.vy>>4);
	wingPos.vz = playerShip->ppivot.vz - (playerShip->unitVecWing.vz>>4) - (playerShip->unitVecNose.vz>>4);


	trackPtr = playerShip->nearTrkSect->nextSection.ptr;

	direction.vx = trackPtr->centre.vx - playerShip->nearTrkSect->centre.vx;
	direction.vy = trackPtr->centre.vy - playerShip->nearTrkSect->centre.vy;
	direction.vz = trackPtr->centre.vz - playerShip->nearTrkSect->centre.vz;

	downTrack = 	( ( direction.vx * playerShip->unitVecNose.vx ) +
						  ( direction.vy * playerShip->unitVecNose.vy ) +
						  ( direction.vz * playerShip->unitVecNose.vz ) ) ;

	if(downTrack < 0) playerShip->attr &= ~DIRECTION;
	else playerShip->attr |= DIRECTION;

	toFaceVector.vx =  track->vertices[(facePtr->vertex[0])].vx - track->vertices[(facePtr->vertex[1])].vx;
	toFaceVector.vy =  track->vertices[(facePtr->vertex[0])].vy - track->vertices[(facePtr->vertex[1])].vy;
	toFaceVector.vz =  track->vertices[(facePtr->vertex[0])].vz - track->vertices[(facePtr->vertex[1])].vz;

	direction.vx = playerShip->nearTrkSect->centre.vx - playerShip->ppivot.vx;
	direction.vy = playerShip->nearTrkSect->centre.vy - playerShip->ppivot.vy;
	direction.vz = playerShip->nearTrkSect->centre.vz - playerShip->ppivot.vz;

	toFace = 	((direction.vx * toFaceVector.vx ) +
					( direction.vy * toFaceVector.vy ) +
					( direction.vz * toFaceVector.vz ) ) ;

	facePtr--;

	/******** Check against left hand side of track ********/

	if(toFace > 0)		
	{
		playerShip->attr |= LSIDE;
 		if((facePtr == (track->faces + playerShip->nearTrkSect->faceStart)) || ( playerShip->nearTrkSect->junction.index != 0))
		{	
			facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
			facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
			facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

	 		alpha = PlaneLineCollision (facePoint, nosePos, facePtr->normal); 
	
			if (alpha <= 0)
			{
				if(playerShip->nearTrkSect->flags & JunctionStart)
				{
					collide = CheckPolygonCollision(facePtr, nosePos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, -downTrack, alpha);
					}
					else
					{
 						facePtr2 = track->faces + playerShip->nearTrkSect->nextSection.ptr->faceStart;
						collide = CheckPolygonCollision(facePtr2, nosePos, alpha);
						if(collide == 1)
						{
							NoseCollision(playerShip, facePtr, -downTrack, alpha);
						}
					}
				}
				else if(playerShip->nearTrkSect->flags & JunctionEnd)
				{
					collide = CheckPolygonCollision(facePtr, nosePos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, -downTrack, alpha);
					}
					else
					{
 						facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
						collide = CheckPolygonCollision(facePtr2, nosePos, alpha);
						if(collide == 1)
						{
							NoseCollision(playerShip, facePtr, -downTrack, alpha);
						}
					}
				}

				else
					NoseCollision(playerShip, facePtr, -downTrack, alpha);
				return;
			}

			alpha = PlaneLineCollision (facePoint, wingPos, facePtr->normal);

			if (alpha <= 0)
			{
				if((playerShip->nearTrkSect->flags & JunctionStart)
						|| (playerShip->nearTrkSect->flags & JunctionEnd))
				{
					collide = CheckPolygonCollision(facePtr, wingPos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, -downTrack, alpha);
					}
				}
				else
				WingCollision(playerShip, facePtr, -downTrack, alpha);
				return;
			}
  	
			wingPos.vx = playerShip->ppivot.vx + (playerShip->unitVecWing.vx>>4) - (playerShip->unitVecNose.vx>>4); 
			wingPos.vy = playerShip->ppivot.vy + (playerShip->unitVecWing.vy>>4) - (playerShip->unitVecNose.vy>>4);
			wingPos.vz = playerShip->ppivot.vz + (playerShip->unitVecWing.vz>>4) - (playerShip->unitVecNose.vz>>4);

			alpha = PlaneLineCollision (facePoint, wingPos, facePtr->normal);
			if (alpha <= 0)
			{
				if((playerShip->nearTrkSect->flags & JunctionStart)
						|| (playerShip->nearTrkSect->flags & JunctionEnd))
				{
					collide = CheckPolygonCollision(facePtr, wingPos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, -downTrack, alpha);
					}
				}
				else
				WingCollision(playerShip, facePtr, -downTrack, alpha);
				return;
			}

		}
	}
		/****************************************************************************/
		/***** 						Collision check against 2nd wall						*****/
		/****************************************************************************/
	else
	{
		playerShip->attr &= ~LSIDE;

		facePtr++;
		while(facePtr->flags & TRACK_BASE)
		{
			facePtr++;
		}

		if((playerShip->nearTrkSect->faceCount > 3) || (playerShip->nearTrkSect->junction.index != 0))
		{	

			facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
			facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
			facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
			alpha = PlaneLineCollision (facePoint, nosePos, facePtr->normal);
			if (alpha <= 0)
			{
				if(playerShip->nearTrkSect->flags & JunctionStart)
				{
					collide = CheckPolygonCollision(facePtr, nosePos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, downTrack, alpha);
					}
					else
					{
 						facePtr2 = track->faces + playerShip->nearTrkSect->nextSection.ptr->faceStart;
						facePtr2 += 3;
						collide = CheckPolygonCollision(facePtr2, nosePos, alpha);
						if(collide == 1)
						{
							NoseCollision(playerShip, facePtr, -downTrack, alpha);
						}
					}
				}
				else if(playerShip->nearTrkSect->flags & JunctionEnd)
				{
					collide = CheckPolygonCollision(facePtr, nosePos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, -downTrack, alpha);
					}
					else
					{
 						facePtr2 = track->faces + playerShip->nearTrkSect->prevSection.ptr->faceStart;
						facePtr2 += 3;
						collide = CheckPolygonCollision(facePtr2, nosePos, alpha);
						if(collide == 1)
						{
							NoseCollision(playerShip, facePtr2, -downTrack, alpha);
						}
					}
				}
				else
					NoseCollision(playerShip, facePtr, downTrack, alpha);
				return;
			}
			alpha = PlaneLineCollision (facePoint, wingPos, facePtr->normal);
			if (alpha <= 0)
			{
				if((playerShip->nearTrkSect->flags & JunctionStart)
						|| (playerShip->nearTrkSect->flags & JunctionEnd))
				{
					collide = CheckPolygonCollision(facePtr, wingPos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, downTrack, alpha);
					}
				}
				else
				WingCollision(playerShip, facePtr, downTrack, alpha);
				return;
			}
  	
			wingPos.vx = playerShip->ppivot.vx + (playerShip->unitVecWing.vx>>4) - (playerShip->unitVecNose.vx>>4);
			wingPos.vy = playerShip->ppivot.vy + (playerShip->unitVecWing.vy>>4) - (playerShip->unitVecNose.vy>>4);
			wingPos.vz = playerShip->ppivot.vz + (playerShip->unitVecWing.vz>>4) - (playerShip->unitVecNose.vz>>4);
			alpha = PlaneLineCollision (facePoint, wingPos, facePtr->normal);
			if (alpha <= 0)
			{
				if((playerShip->nearTrkSect->flags & JunctionStart) ||
						(playerShip->nearTrkSect->flags & JunctionEnd))
				{
					collide = CheckPolygonCollision(facePtr, wingPos, alpha);
					if(collide == 1)
					{
						NoseCollision(playerShip, facePtr, downTrack, alpha);
					}
				}

				else
				WingCollision(playerShip, facePtr, downTrack, alpha);
				return;
			}
		}
	}
}

int CheckPolygonCollision(Face *facePtr, VECTOR pos, int alpha)
{

	VECTOR 			planePoint;
	VECTOR 			vec0, vec1, vec2, vec3;/* vectors between plane coords and intersection point */
	VECTOR 			centre;
	int 				angle;
	int 				collide = 1;

	planePoint.vx = pos.vx - ((facePtr->normal.vx * alpha)>>12);
	planePoint.vy = pos.vy - ((facePtr->normal.vy * alpha)>>12);
	planePoint.vz = pos.vz - ((facePtr->normal.vz * alpha)>>12);

	vec0.vx = planePoint.vx - track->vertices[(facePtr->vertex[1])].vx; 
	vec0.vy = planePoint.vy - track->vertices[(facePtr->vertex[1])].vy; 
	vec0.vz = planePoint.vz - track->vertices[(facePtr->vertex[1])].vz; 

	vec1.vx = planePoint.vx - track->vertices[(facePtr->vertex[2])].vx; 
	vec1.vy = planePoint.vy - track->vertices[(facePtr->vertex[2])].vy; 
	vec1.vz = planePoint.vz - track->vertices[(facePtr->vertex[2])].vz; 

	vec2.vx = planePoint.vx - track->vertices[(facePtr->vertex[0])].vx; 
	vec2.vy = planePoint.vy - track->vertices[(facePtr->vertex[0])].vy; 
	vec2.vz = planePoint.vz - track->vertices[(facePtr->vertex[0])].vz; 

	vec3.vx = planePoint.vx - track->vertices[(facePtr->vertex[3])].vx; 
	vec3.vy = planePoint.vy - track->vertices[(facePtr->vertex[3])].vy; 
	vec3.vz = planePoint.vz - track->vertices[(facePtr->vertex[3])].vz; 

	angle =  GetAng(vec0, vec2);
	angle += GetAng(vec2, vec3);
	angle += GetAng(vec3, vec1);
	angle += GetAng(vec1, vec0);

	FntPrint("angle = %d\n", angle);

	if ( (angle <= 30000) )
	{
		collide = 0;
	}
	return(collide);
}


void WingCollision(ShipData *playerShip, Face *facePtr, int direction, int alpha)
{
	VECTOR reaction;
	SVECTOR collisionVector;
	int angle;
	VECTOR wingPos;
	int magnitude;

	collisionVector.vx = playerShip->nearTrkSect->centre.vx - track->vertices[(facePtr->vertex[2])].vx;
	collisionVector.vy = playerShip->nearTrkSect->centre.vy - track->vertices[(facePtr->vertex[2])].vy;
	collisionVector.vz = playerShip->nearTrkSect->centre.vz - track->vertices[(facePtr->vertex[2])].vz;

	angle = AngleOfCollision(collisionVector, playerShip->unitVecNose);

	Reflection(&(playerShip->vpivot), facePtr->normal);

 	playerShip->ppivot.vx -= playerShip->vpivot.vx>>6;		 
	playerShip->ppivot.vy -= playerShip->vpivot.vy>>6;
	playerShip->ppivot.vz -= playerShip->vpivot.vz>>6;

 	playerShip->vpivot.vx -= playerShip->vpivot.vx>>1;		 
	playerShip->vpivot.vy -= playerShip->vpivot.vy>>1;
	playerShip->vpivot.vz -= playerShip->vpivot.vz>>1;

 	playerShip->vpivot.vx += (facePtr->normal.vx);		 
	playerShip->vpivot.vy += (facePtr->normal.vy);
	playerShip->vpivot.vz += (facePtr->normal.vz);


	magnitude = (abs(angle)*playerShip->speed) >> 16;	

	if(direction > 0)	playerShip->vroll += magnitude;	
	else playerShip->vroll 					-= magnitude;

	if(direction > 0)
	{
		wingPos.vx = playerShip->ppivot.vx + (playerShip->unitVecWing.vx>>4) - (playerShip->unitVecNose.vx>>4);
		wingPos.vy = playerShip->ppivot.vy + (playerShip->unitVecWing.vy>>4) - (playerShip->unitVecNose.vy>>4);
		wingPos.vz = playerShip->ppivot.vz + (playerShip->unitVecWing.vz>>4) - (playerShip->unitVecNose.vz>>4);
	}
	else
	{
		wingPos.vx = playerShip->ppivot.vx - (playerShip->unitVecWing.vx>>4) - (playerShip->unitVecNose.vx>>4);
		wingPos.vy = playerShip->ppivot.vy - (playerShip->unitVecWing.vy>>4) - (playerShip->unitVecNose.vy>>4);
		wingPos.vz = playerShip->ppivot.vz - (playerShip->unitVecWing.vz>>4) - (playerShip->unitVecNose.vz>>4);
	}

	PlayDirectionalNote(SHIP_2_TRACK, 1, 20, wingPos);

	/***** Magnitude returns approx between 0 & 1000    *****/
	/***** scale this and shift to a figure around zero *****/

	magnitude = magnitude - 500;
	magnitude = magnitude << 2;

	SetVolume(SHIP_2_TRACK, chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.left + magnitude,
									chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.right + magnitude);

#if 0
	chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.left += magnitude;
	chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.right += magnitude;
	SpuSetVoiceAttr(&(chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr));
#endif
}

void NoseCollision(ShipData *playerShip, Face *facePtr, int direction, int alpha)
{
	VECTOR reaction;
	SVECTOR collisionVector;
	int angle;
	VECTOR nosePos;
	int magnitude;

	collisionVector.vx = playerShip->nearTrkSect->centre.vx - track->vertices[(facePtr->vertex[2])].vx;
	collisionVector.vy = playerShip->nearTrkSect->centre.vy - track->vertices[(facePtr->vertex[2])].vy;
	collisionVector.vz = playerShip->nearTrkSect->centre.vz - track->vertices[(facePtr->vertex[2])].vz;

	angle = abs(AngleOfCollision(collisionVector, playerShip->unitVecNose));

	Reflection(&(playerShip->vpivot), facePtr->normal);

	magnitude = (playerShip->speed>>4) + 400;	
	if(direction > 0) playerShip->vhdg += magnitude;	
	else playerShip->vhdg -= magnitude;

 	playerShip->ppivot.vx -= playerShip->vpivot.vx>>6;		 
	playerShip->ppivot.vy -= playerShip->vpivot.vy>>6;
	playerShip->ppivot.vz -= playerShip->vpivot.vz>>6;

 	playerShip->vpivot.vx -= playerShip->vpivot.vx>>1;		 
	playerShip->vpivot.vy -= playerShip->vpivot.vy>>1;
	playerShip->vpivot.vz -= playerShip->vpivot.vz>>1;

 	playerShip->vpivot.vx += facePtr->normal.vx;		 
	playerShip->vpivot.vy += facePtr->normal.vy;
	playerShip->vpivot.vz += facePtr->normal.vz;

	nosePos.vx = playerShip->ppivot.vx + (playerShip->unitVecNose.vx>>3);
	nosePos.vy = playerShip->ppivot.vy + (playerShip->unitVecNose.vy>>3);
	nosePos.vz = playerShip->ppivot.vz + (playerShip->unitVecNose.vz>>3);

	PlayDirectionalNote(SHIP_2_TRACK, 1, 20, nosePos);

	/***** Magnitude returns approx between 0 & 1000    *****/
	/***** scale this and shift to a figure around zero *****/

	magnitude = magnitude - 500;
	magnitude = magnitude << 2;


	SetVolume(SHIP_2_TRACK, chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.left + magnitude,
									chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.right + magnitude);

#if 0
	chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.left += magnitude;
	chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr.volume.right += magnitude;
	SpuSetVoiceAttr(&(chanTable[vagTable[SHIP_2_TRACK].channelID].s_attr));
#endif
}

void Reflection(VECTOR *incidence, SVector normal)
{
	int temp;

	temp = 		((((int)normal.vx * -incidence->vx )>>12) +
					( (int)(normal.vy * -incidence->vy )>>12) +
					( (int)(normal.vz * -incidence->vz )>>12) );
	
	 incidence->vx = ( ( ( 2 * (int) normal.vx ) * temp)>>12 ) + incidence->vx;
	 incidence->vy = ( ( ( 2 * (int) normal.vy ) * temp)>>12 ) + incidence->vy;
	 incidence->vz = ( ( ( 2 * (int) normal.vz ) * temp)>>12 ) + incidence->vz;
}

int AngleOfCollision (SVECTOR normal, VECTOR ship_point)
{

 	int angle ;
	int dot_product ;
	int norm_dist, shpoint_dist ;
	int dist_product ;

	int tmp1, tmp2;

	ship_point.vx = ship_point.vx>>9;
	ship_point.vy = ship_point.vy>>9;
	ship_point.vz = ship_point.vz>>9;

	dot_product = 	( ( ship_point.vx * normal.vx ) +
						  ( ship_point.vy * normal.vy ) +
						  ( ship_point.vz * normal.vz ) ) ;


	norm_dist = SquareRoot0 ( ( normal.vx * normal.vx ) + 
									  ( normal.vy * normal.vy ) + 
									  ( normal.vz * normal.vz ) ) ;

	shpoint_dist = SquareRoot0 ( ( ship_point.vx * ship_point.vx ) + 
										  ( ship_point.vy * ship_point.vy ) + 
										  ( ship_point.vz * ship_point.vz ) ) ;

	dist_product = (shpoint_dist * norm_dist);

	tmp1 = dot_product << 12;
	if( dist_product != 0 )
		tmp2 = (tmp1 / dist_product);
	else
		tmp2 = 0;
	
	return tmp2;
}

