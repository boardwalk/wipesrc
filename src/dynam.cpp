/************************************************************/
/******     dynam.c - Ship 2 track reaction		     	  *****/
/************************************************************/

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
#include "droid.h"
#include "menus.h"
#include "camera.h"
#include "sound.h"
#include "combat.h"
#include "sparks2.h"
#include "global.h"
#include "utils.h"
#include "libgte.h"
#include "collide.h"
#include "skeleton.h"

extern char BluePal;
extern char WhitePal;
extern char DepthFadeTble[];

void shipTrkReaction(ShipData* playerShip, Object** shipShapes) {
  int32_t alpha, noseAlpha;
  int32_t resistance;
  Face* facePtr;
  VECTOR force;
  VECTOR velDOF;
  int32_t velMag;
  VECTOR nosePos;
  int16_t brake;
  VECTOR facePoint;
  TrackSection* nextSection;
  VECTOR bestPath, distance;
  VECTOR repulseVector;
  VECTOR planePoint;
  VECTOR vec0, vec1, vec2, vec3; /* vectors between plane coords and intersection point */
  int32_t angle;

  /***** Calculate thrust vector along principle axis of ship *****/
  playerShip->thrust.vx = (playerShip->thrust_mag * playerShip->unitVecNose.vx) >> 6;
  playerShip->thrust.vy = (playerShip->thrust_mag * playerShip->unitVecNose.vy) >> 6;
  playerShip->thrust.vz = (playerShip->thrust_mag * playerShip->unitVecNose.vz) >> 6;

  velMag = GetMagnitude(playerShip->vpivot);

  playerShip->speed = velMag;

  velDOF.vx = sar(velMag * playerShip->unitVecNose.vx, 12);
  velDOF.vy = sar(velMag * playerShip->unitVecNose.vy, 12);
  velDOF.vz = sar(velMag * playerShip->unitVecNose.vz, 12);

  if (playerShip->nearTrkSect->flags & Jump) {
    facePtr = track->faces + playerShip->nearTrkSect->faceStart;

    while ((facePtr->flags & TRACK_BASE) == 0) {
      facePtr++;
    }

    facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
    facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
    facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;

    alpha = PlaneLineCollision(facePoint, playerShip->ppivot, facePtr->normal);

    planePoint.vx = playerShip->ppivot.vx - ((facePtr->normal.vx * alpha) >> 12);
    planePoint.vy = playerShip->ppivot.vy - ((facePtr->normal.vy * alpha) >> 12);
    planePoint.vz = playerShip->ppivot.vz - ((facePtr->normal.vz * alpha) >> 12);

    vec0.vx = planePoint.vx - track->vertices[(facePtr->vertex[1])].vx;
    vec0.vy = planePoint.vy - track->vertices[(facePtr->vertex[1])].vy;
    vec0.vz = planePoint.vz - track->vertices[(facePtr->vertex[1])].vz;

    vec1.vx = planePoint.vx - track->vertices[(facePtr->vertex[2])].vx;
    vec1.vy = planePoint.vy - track->vertices[(facePtr->vertex[2])].vy;
    vec1.vz = planePoint.vz - track->vertices[(facePtr->vertex[2])].vz;

    facePtr++;

    vec2.vx = planePoint.vx - track->vertices[(facePtr->vertex[0])].vx;
    vec2.vy = planePoint.vy - track->vertices[(facePtr->vertex[0])].vy;
    vec2.vz = planePoint.vz - track->vertices[(facePtr->vertex[0])].vz;

    vec3.vx = planePoint.vx - track->vertices[(facePtr->vertex[3])].vx;
    vec3.vy = planePoint.vy - track->vertices[(facePtr->vertex[3])].vy;
    vec3.vz = planePoint.vz - track->vertices[(facePtr->vertex[3])].vz;

    angle = GetAng(vec0, vec2);
    angle += GetAng(vec2, vec3);
    angle += GetAng(vec3, vec1);
    angle += GetAng(vec1, vec0);

    if ((angle <= 30000)) {
      playerShip->attr |= FLYING;
    }
  }

  /***************************************************************************/
  /*** 							Ship is held by the track								 ***/
  /***************************************************************************/

  if ((playerShip->attr & FLYING) == 0) {
    facePtr = track->faces + playerShip->nearTrkSect->faceStart;

    while ((facePtr->flags & TRACK_BASE) == 0) {
      facePtr++;
    }

    Ship2Track(playerShip, facePtr, shipShapes);

    if ((playerShip->attr & SPECIALED) == 0) {
      if (((facePtr->flags & SPEED_UP) && (playerShip->attr & LSIDE))) {
#if PAL
        playerShip->vpivot.vx += ((playerShip->nearTrkSect->nextSection.ptr->centre.vx - playerShip->nearTrkSect->centre.vx) * 60) / 50;
        playerShip->vpivot.vy += ((playerShip->nearTrkSect->nextSection.ptr->centre.vy - playerShip->nearTrkSect->centre.vy) * 60) / 50;
        playerShip->vpivot.vz += ((playerShip->nearTrkSect->nextSection.ptr->centre.vz - playerShip->nearTrkSect->centre.vz) * 60) / 50;
#else
        playerShip->vpivot.vx += ((playerShip->nearTrkSect->nextSection.ptr->centre.vx - playerShip->nearTrkSect->centre.vx) * 60) / FR60;
        playerShip->vpivot.vy += ((playerShip->nearTrkSect->nextSection.ptr->centre.vy - playerShip->nearTrkSect->centre.vy) * 60) / FR60;
        playerShip->vpivot.vz += ((playerShip->nearTrkSect->nextSection.ptr->centre.vz - playerShip->nearTrkSect->centre.vz) * 60) / FR60;
#endif
      }
      facePtr++;

      if ((facePtr->flags & SPEED_UP) && ((playerShip->attr & LSIDE) == 0)) {
#if PAL
        playerShip->vpivot.vx += ((playerShip->nearTrkSect->nextSection.ptr->centre.vx - playerShip->nearTrkSect->centre.vx) * 60) / 50;
        playerShip->vpivot.vy += ((playerShip->nearTrkSect->nextSection.ptr->centre.vy - playerShip->nearTrkSect->centre.vy) * 60) / 50;
        playerShip->vpivot.vz += ((playerShip->nearTrkSect->nextSection.ptr->centre.vz - playerShip->nearTrkSect->centre.vz) * 60) / 50;
#else
        playerShip->vpivot.vx += ((playerShip->nearTrkSect->nextSection.ptr->centre.vx - playerShip->nearTrkSect->centre.vx) * 60) / FR60;
        playerShip->vpivot.vy += ((playerShip->nearTrkSect->nextSection.ptr->centre.vy - playerShip->nearTrkSect->centre.vy) * 60) / FR60;
        playerShip->vpivot.vz += ((playerShip->nearTrkSect->nextSection.ptr->centre.vz - playerShip->nearTrkSect->centre.vz) * 60) / FR60;
#endif
      }
      facePtr--;
    }

    if ((playerShip->attr & LSIDE) == 0) {
      facePtr++;
    }

    facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
    facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
    facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
    alpha = PlaneLineCollision(facePoint, playerShip->ppivot, facePtr->normal);
    noseAlpha = alpha;

    /******************************************************************/
    /***  						Collision with floor							 ***/
    /******************************************************************/
    if (alpha <= 0) {
      PlayDirectionalNote(SHIP_2_TRACK, 1, 20, playerShip->ppivot);

      Reflection(&(playerShip->vpivot), facePtr->normal);

      playerShip->vpivot.vx -= (playerShip->vpivot.vx) >> 3;
      playerShip->vpivot.vy -= (playerShip->vpivot.vy) >> 3;
      playerShip->vpivot.vz -= (playerShip->vpivot.vz) >> 3;
#if PAL
      playerShip->vpivot.vx -= (facePtr->normal.vx * 60) / 50;
      playerShip->vpivot.vy -= (facePtr->normal.vy * 60) / 50;
      playerShip->vpivot.vz -= (facePtr->normal.vz * 60) / 50;
#else
      playerShip->vpivot.vx -= (facePtr->normal.vx * 60) / FR60;
      playerShip->vpivot.vy -= (facePtr->normal.vy * 60) / FR60;
      playerShip->vpivot.vz -= (facePtr->normal.vz * 60) / FR60;
#endif
    } else if (alpha <= 30) {
#if PAL
      playerShip->vpivot.vx += (facePtr->normal.vx * 60) / 50;
      playerShip->vpivot.vy += (facePtr->normal.vy * 60) / 50;
      playerShip->vpivot.vz += (facePtr->normal.vz * 60) / 50;
#else
      playerShip->vpivot.vx += (facePtr->normal.vx * 60) / FR60;
      playerShip->vpivot.vy += (facePtr->normal.vy * 60) / FR60;
      playerShip->vpivot.vz += (facePtr->normal.vz * 60) / FR60;
#endif
    }

    brake = (playerShip->r_brake_rot + playerShip->l_brake_rot);
    /* was << 1 */
    playerShip->apivot.vx = (velDOF.vx - playerShip->vpivot.vx) / (playerShip->skid + (brake >> 2));
    playerShip->apivot.vy = (velDOF.vy - playerShip->vpivot.vy) / (playerShip->skid + (brake >> 2));
    playerShip->apivot.vz = (velDOF.vz - playerShip->vpivot.vz) / (playerShip->skid + (brake >> 2));

    if (alpha < 50)
      alpha = 50;

    force.vx = (facePtr->normal.vx * TARGETHEIGHT * TRACK_MAGNET) / alpha;
    force.vy = (facePtr->normal.vy * TARGETHEIGHT * TRACK_MAGNET) / alpha;
    force.vz = (facePtr->normal.vz * TARGETHEIGHT * TRACK_MAGNET) / alpha;

    force.vx -= (facePtr->normal.vx * TRACK_MAGNET);
    force.vy -= (facePtr->normal.vy * TRACK_MAGNET);
    force.vz -= (facePtr->normal.vz * TRACK_MAGNET);

    force.vy += 30000;

    playerShip->apivot.vx += (force.vx + playerShip->thrust.vx) / playerShip->mass;
    playerShip->apivot.vy += (force.vy + playerShip->thrust.vy) / playerShip->mass;
    playerShip->apivot.vz += (force.vz + playerShip->thrust.vz) / playerShip->mass;

    playerShip->apivot.vx = (playerShip->apivot.vx * 60) / FR60;
    playerShip->apivot.vy = (playerShip->apivot.vy * 60) / FR60;
    playerShip->apivot.vz = (playerShip->apivot.vz * 60) / FR60;

    playerShip->vpivot.vx += playerShip->apivot.vx;
    playerShip->vpivot.vy += playerShip->apivot.vy;
    playerShip->vpivot.vz += playerShip->apivot.vz;

    /*********************************************/
    /****	 			Resistance vectors			****/
    /*********************************************/
    /*** Aveage resistance value based around 128 ***/
    resistance = (playerShip->resistance * (74 - (brake >> 3))) >> 7;

    playerShip->vpivot.vx -= playerShip->vpivot.vx / resistance;
    playerShip->vpivot.vy -= playerShip->vpivot.vy / resistance;
    playerShip->vpivot.vz -= playerShip->vpivot.vz / resistance;

    playerShip->ppivot.vx += sar(playerShip->vpivot.vx, VELOCITY_SHIFT);
    playerShip->ppivot.vy += sar(playerShip->vpivot.vy, VELOCITY_SHIFT);
    playerShip->ppivot.vz += sar(playerShip->vpivot.vz, VELOCITY_SHIFT);

    nosePos.vx = (playerShip->unitVecNose.vx >> 5) + playerShip->ppivot.vx;
    nosePos.vy = (playerShip->unitVecNose.vy >> 5) + playerShip->ppivot.vy;
    nosePos.vz = (playerShip->unitVecNose.vz >> 5) + playerShip->ppivot.vz;

    alpha = PlaneLineCollision(facePoint, nosePos, facePtr->normal);

    if (playerShip->collideCount == 0) /***** NO HUGE COLLISION *****/
    {
      noseAlpha -= alpha;

      if (alpha < 600) {
        playerShip->vpitch += (noseAlpha + 5);
      } else {
        playerShip->vpitch -= 50;
      }

      playerShip->vpitch -= playerShip->vpitch >> 2;
      brake = sar(playerShip->r_brake_rot - playerShip->l_brake_rot, 3);

      playerShip->hdg += sar(brake * playerShip->speed, 15);

      playerShip->vroll += sar(playerShip->vhdg, 5);
      playerShip->vroll -= playerShip->vroll >> 1;
    }
  }

  /***************************************************************************/
  /*** 									Ship is flying										 ***/
  /***************************************************************************/
  else {
    /************************************************************************/
    /***  				Detect the need for a rescue droid						 ***/
    /************************************************************************/

    nextSection = playerShip->nearTrkSect->nextSection.ptr;

    GetTargetPos(&bestPath, playerShip->ppivot, nextSection->centre, playerShip->nearTrkSect->centre);

    distance.vx = (bestPath.vx - playerShip->ppivot.vx);
    distance.vy = (bestPath.vy - playerShip->ppivot.vy);
    distance.vz = (bestPath.vz - playerShip->ppivot.vz);

    if (distance.vy > 0)
      distance.vy = distance.vy >> 7;
    else {
      distance.vx = distance.vx << 3;
      distance.vy = distance.vy << 3;
      distance.vz = distance.vz << 3;
    }

    if (GetMagnitude(distance) > 4000) {
      playerShip->update = UpdateRescueDroid;
      playerShip->updateCount = 500;
      playerShip->attr |= RESCUE;
    }

    brake = (playerShip->r_brake_rot + playerShip->l_brake_rot);

    playerShip->apivot.vx = (velDOF.vx - playerShip->vpivot.vx) / (MIN_RESISTANCE + (brake << 2));
    playerShip->apivot.vy = (velDOF.vy - playerShip->vpivot.vy) / (MIN_RESISTANCE + (brake << 2));
    playerShip->apivot.vz = (velDOF.vz - playerShip->vpivot.vz) / (MIN_RESISTANCE + (brake << 2));

    playerShip->apivot.vx += (playerShip->thrust.vx) / playerShip->mass;
    playerShip->apivot.vy += (GRAVITY + playerShip->thrust.vy) / playerShip->mass;
    playerShip->apivot.vz += (playerShip->thrust.vz) / playerShip->mass;

    playerShip->apivot.vx = (playerShip->apivot.vx * 60) / FR60;
    playerShip->apivot.vy = (playerShip->apivot.vy * 60) / FR60;
    playerShip->apivot.vz = (playerShip->apivot.vz * 60) / FR60;

    playerShip->vpivot.vx += playerShip->apivot.vx;
    playerShip->vpivot.vy += playerShip->apivot.vy;
    playerShip->vpivot.vz += playerShip->apivot.vz;

    /*********************************************/
    /****	 			Resistance vectors			****/
    /*********************************************/

    resistance = (playerShip->resistance * (74 - (brake >> 3))) >> 7;

    playerShip->vpivot.vx -= playerShip->vpivot.vx / resistance;
    playerShip->vpivot.vy -= playerShip->vpivot.vy / resistance;
    playerShip->vpivot.vz -= playerShip->vpivot.vz / resistance;

    playerShip->ppivot.vx += sar(playerShip->vpivot.vx, 6);
    playerShip->ppivot.vy += sar(playerShip->vpivot.vy, 6);
    playerShip->ppivot.vz += sar(playerShip->vpivot.vz, 6);

#if PAL
    playerShip->vpitch -= 60;
#else
    playerShip->vpitch -= FR50;
#endif
    playerShip->vpitch -= playerShip->vpitch >> 2;

    brake = sar(playerShip->r_brake_rot - playerShip->l_brake_rot, 3);
    playerShip->hdg += sar((brake * playerShip->speed), 15);

    playerShip->vroll += sar(playerShip->vhdg, 5);
    playerShip->vroll -= sar(playerShip->vroll, 1);
  }
  playerShip->hdg += sar(playerShip->vhdg, 6);

  playerShip->pitch += playerShip->vpitch >> 4;

  playerShip->roll += playerShip->vroll;
  playerShip->roll -= playerShip->roll >> 3;

  playerShip->hdg = ang(playerShip->hdg);
  playerShip->pitch = ang(playerShip->pitch);
  playerShip->roll = ang(playerShip->roll);

  nextSection = playerShip->nearTrkSect->prevSection.ptr;

  if ((playerShip->attr & DIRECTION) == 0) {
    if (nextSection->flags & Jump) {
      repulseVector.vx = playerShip->nearTrkSect->nextSection.ptr->centre.vx - playerShip->nearTrkSect->centre.vx;
      repulseVector.vy = playerShip->nearTrkSect->nextSection.ptr->centre.vy - playerShip->nearTrkSect->centre.vy;
      repulseVector.vz = playerShip->nearTrkSect->nextSection.ptr->centre.vz - playerShip->nearTrkSect->centre.vz;

      playerShip->vpivot.vx += repulseVector.vx << 1;
      playerShip->vpivot.vy += repulseVector.vy << 1;
      playerShip->vpivot.vz += repulseVector.vz << 1;
    }
  }
}

/**********************************************************/
/** Returns the position of a point in space relative to **/
/** 								the camera							**/
/**********************************************************/

void getWorldPoint(VECTOR* output, Object* object, Object* objectCollide, int32_t pointNo) {
  int32_t flag;
  SVECTOR* vertex;

  SetSkeletonTransform(object->skeleton);

  vertex = objectCollide->vertices;

  RotTrans(vertex + pointNo, output, &flag);
}

void GetTargetPos(VECTOR* result, VECTOR pos, Vector cent1, Vector cent2) {
  int32_t a_mag, b_mag, c_mag;
  VECTOR a, b, c;
  int32_t a_sq, b_sq, c_sq;
  int32_t cos_alpha;
  int32_t angle;

  a.vx = pos.vx - cent1.vx;
  a.vy = pos.vy - cent1.vy;
  a.vz = pos.vz - cent1.vz;

  b.vx = cent2.vx - cent1.vx;
  b.vy = cent2.vy - cent1.vy;
  b.vz = cent2.vz - cent1.vz;

  c.vx = pos.vx - cent2.vx;
  c.vy = pos.vy - cent2.vy;
  c.vz = pos.vz - cent2.vz;

  a_mag = GetMagnitude(a);
  b_mag = GetMagnitude(b);
  c_mag = GetMagnitude(c);

  a_sq = (a_mag * a_mag);
  b_sq = (b_mag * b_mag);
  c_sq = (c_mag * c_mag);

  if ((a_mag != 0) && (b_mag != 0)) {
    cos_alpha = ((-b_sq - (a_sq + c_sq)) << 12) / ((a_mag * b_mag) << 1);
    angle = arccos(cos_alpha) >> 2;
    c_mag = ((rcos(angle) * a_mag) >> 12);
    result->vx = cent1.vx - ((b.vx * c_mag) / b_mag);
    result->vy = cent1.vy - ((b.vy * c_mag) / b_mag);
    result->vz = cent1.vz - ((b.vz * c_mag) / b_mag);
  }
}

extern int16_t startLinePos[];

void SetUnitVectors(ShipData* shipIndex) {
  int16_t sx, sy, sz;
  int16_t cx, cy, cz;
  int32_t i;
  VECTOR direction;
  VECTOR toFaceVector;
  int32_t toFace;
  Face* facePtr;

  for (i = 0; i < NO_OF_SHIPS; i++) {
    /*	  	if(i != serialShip)*/

    if (((raceType == HEAD_TO_HEAD) && (i == ownShip)) ||
        (i != serialShip)) {
      sy = rsin(shipIndex[i].hdg);
      cy = rcos(shipIndex[i].hdg);
      sx = rsin(shipIndex[i].pitch);
      cx = rcos(shipIndex[i].pitch);
      sz = rsin(shipIndex[i].roll);
      cz = rcos(shipIndex[i].roll);

      shipIndex[i].unitVecNose.vx = -frac(sy, cx);
      shipIndex[i].unitVecNose.vy = -sx;
      shipIndex[i].unitVecNose.vz = frac(cy, cx);

      shipIndex[i].unitVecWing.vx = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
      shipIndex[i].unitVecWing.vy = -(sz * cx) >> 12;
      shipIndex[i].unitVecWing.vz = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);

      shipIndex[i].prevShipSect = shipIndex[i].nearTrkSect;
      SectionSearch(&(shipIndex[i]));

      shipIndex[i].prevSecNo = shipIndex[i].prevShipSect->secNo;
      shipIndex[i].nearSecNo = shipIndex[i].nearTrkSect->secNo;
    }

    if (((raceType == HEAD_TO_HEAD) && (i == ownShip)) ||
        (raceType != HEAD_TO_HEAD)) {
      facePtr = track->faces + shipIndex[i].nearTrkSect->faceStart;

      while ((facePtr->flags & TRACK_BASE) == 0) {
        facePtr++;
      }

      toFaceVector.vx = track->vertices[(facePtr->vertex[0])].vx - track->vertices[(facePtr->vertex[1])].vx;
      toFaceVector.vy = track->vertices[(facePtr->vertex[0])].vy - track->vertices[(facePtr->vertex[1])].vy;
      toFaceVector.vz = track->vertices[(facePtr->vertex[0])].vz - track->vertices[(facePtr->vertex[1])].vz;

      direction.vx = shipIndex[i].nearTrkSect->centre.vx - shipIndex[i].ppivot.vx;
      direction.vy = shipIndex[i].nearTrkSect->centre.vy - shipIndex[i].ppivot.vy;
      direction.vz = shipIndex[i].nearTrkSect->centre.vz - shipIndex[i].ppivot.vz;

      toFace = ((direction.vx * toFaceVector.vx) +
                (direction.vy * toFaceVector.vy) +
                (direction.vz * toFaceVector.vz));

      if (toFace > 0)
        shipIndex[i].attr |= LSIDE;
      else
        shipIndex[i].attr &= ~LSIDE;
    }
  }
}

int32_t SetTrackAttributes(TrackSection* section,
                           int16_t i,
                           WeaponGrid* weaponGrid,
                           int32_t noWeaponGrids) {
  Face* facePtr;

  facePtr = track->faces + section->faceStart;

  while ((facePtr->flags & TRACK_BASE) == 0) {
    facePtr++;
  }

  if (facePtr->flags & WEAPON_GRID_LEFT) {
    if (raceType != TIME_TRIAL) {
      facePtr->red = 0xff;
      facePtr->green = 0x23;
      facePtr->blue = 0x75;
    }
    weaponGrid[noWeaponGrids].section = section;
    weaponGrid[noWeaponGrids].count = 0;
    weaponGrid[noWeaponGrids].attr = 0;
    weaponGrid[noWeaponGrids].attr |= WEAPON_GRID_LEFT;
    noWeaponGrids++;
  } else if (facePtr->flags & SPEED_UP) {
    facePtr->red = BluePal;
    facePtr->green = BLUE_SPEEDUP;
    facePtr->blue = 0;
  }
#define TINT 1
#if TINT
  else if ((facePtr->flags & TRACK_BASE) && (trackNo == 7)) {
    facePtr->red = DepthFadeTble[(256 * (WHITE4_TRANSL - 1)) + facePtr->red];
  }
#endif
  facePtr++;

#if TINT
  if ((facePtr->flags & TRACK_BASE) && (trackNo == 7)) {
    facePtr->red = DepthFadeTble[(256 * (WHITE4_TRANSL - 1)) + facePtr->red];
  }
#endif

  if (facePtr->flags & WEAPON_GRID_RIGHT) {
    if (raceType != TIME_TRIAL) {
      facePtr->red = 0x54;
      facePtr->green = 0xee;
      facePtr->blue = 0x75;
    }
    weaponGrid[noWeaponGrids].section = section;
    weaponGrid[noWeaponGrids].count = 0;
    weaponGrid[noWeaponGrids].attr = 0;
    weaponGrid[noWeaponGrids].attr |= WEAPON_GRID_RIGHT;
    noWeaponGrids++;
  } else if (facePtr->flags & SPEED_UP) {
    facePtr->red = BluePal;
    facePtr->green = BLUE_SPEEDUP;
    facePtr->blue = 0;
  }

  return (noWeaponGrids);
}
