#include "standard.h"
#include <stdlib.h>
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
#include "search.h"
#include "libgte.h"
#include "error.h"
#include "win.h"
#include "collide.h"
#include "utils.h"

#define RobTest 0

/* the spline position we are currently on */
int16_t camerasplineCount = 0;
int16_t shipsplineCount = 0;

/* the spline interpolated notch we are currently on */
int16_t camerastepCount = 0;
int16_t shipstepCount = 0;

/* changeable camera coords on high up camera views */
int16_t camVx = -9900;
int16_t camVy = 9100;
int16_t camVz = 6900;

/* the timeout period high camera views */
int16_t timeOut;

/* which camera - ship spline path to use */
int16_t splineNumber;

int16_t startIceText;
int16_t startFireText;
int16_t textY;

#define ExtroMessages 0

char c1[48] = "AWESOME";
char c2[48] = " ";
char c3[48] = "YOU HAVE BEATEN";
char c4[48] = " ";
char c5[48] = "WIPEOUT";
char c6[48] = " ";
char c7[48] = "YOU ARE A TRULY";
char c8[48] = " ";
char c9[48] = "AMAZING PILOT";
char c10[48] = " ";
char c11[48] = " ";
char c12[48] = " ";
char c13[48] = "CONGRATULATIONS";
char c14[48] = " ";
char c15[48] = " ";
char c16[48] = " ";
char c17[48] = " ";
char c18[48] = "A BIG THANKS";
char c19[48] = " ";
char c20[48] = "FROM ALL OF US ON THE TEAM";
char c21[48] = " ";
char c22[48] = "LOOK OUT FOR";
char c23[48] = " ";
char c24[48] = "WIPEOUT II";
char c25[48] = " ";
char c26[48] = "COMING SOON";

char i1[48] = "CONGRATULATIONS";
char i2[48] = " ";
char i3[48] = "RAPIER CLASS";
char i4[48] = " ";
char i5[48] = "COMPETENCE ACHIEVED";
char i6[48] = " ";
char i7[48] = "YOU NOW HAVE ACCESS TO THE";
char i8[48] = " ";
char i9[48] = "FULL VENOM AND RAPIER";
char i10[48] = " ";
char i11[48] = "CHAMPIONSHIPS WITH THE ";
char i12[48] = " ";
char i13[48] = "NEWLY CONSTRUCTED CIRCUIT";
char i14[48] = " ";
char i15[48] = "FIRESTAR";
char i16[48] = " ";
char i17[48] = "WE RECOMMEND YOU";
char i18[48] = " ";
char i19[48] = "SAVE";
char i20[48] = " ";
char i21[48] = "YOUR CURRENT GAME";
char i22[48] = " ";
char i23[48] = "GOOD LUCK";

typedef struct name {
  char* words;
  int16_t colour;
  int16_t size;
} Congrats;

Congrats con[30];

#define Red 0
#define White 1

#define GlobalDistance 32767

TrackSection* FindAllNearestSection(VECTOR pos) {
  TrackSection* nearSection;
  TrackSection* trk;
  VECTOR distance;
  //	int32_t int32_t      smallestMag = 32767 * 32767;
  int32_t smallestMag = 0x7fffffff; // pretty big // PCwipeout
  //	int32_t int32_t      mag;
  int32_t mag;
  int32_t i;

  trk = track->sections;

  for (i = 0; i < track->sectionCount; i++) {
    distance.vx = pos.vx - trk->centre.vx;
    distance.vy = pos.vy - trk->centre.vy;
    distance.vz = pos.vz - trk->centre.vz;

    if ((abs(distance.vx) < GlobalDistance) &&
        (abs(distance.vy) < GlobalDistance) &&
        (abs(distance.vz) < GlobalDistance)) {
      mag = (SquareRoot0((distance.vx * distance.vx) +
                         (distance.vy * distance.vy) +
                         (distance.vz * distance.vz)));

      if ((mag < GlobalDistance) && (mag < smallestMag)) {
        smallestMag = mag;
        nearSection = trk;
      }
    }

    trk = NextIndexSection(trk);
  }

  return (nearSection);
}

void UpdateShipRemote(ShipData* shipIndex, Object** shipShapes, int32_t shipNo, WeaponData* weaponIndex) {
  ShipData *myShip, *remoteShip;
  Face* facePtr;
  TrackSection *nextSection, *section;
  int32_t i;
  VECTOR targetVector, bestPath;
  VECTOR facePoint;
  int32_t temp, alpha;
  VECTOR offSetVector;
  Vector path1, path2;
  int32_t gap_mag, decide;

  offSetVector.vx = 0;
  offSetVector.vy = 0;
  offSetVector.vz = 0;

  myShip = &shipIndex[ownShip];
  remoteShip = &shipIndex[shipNo];

  if ((remoteShip->attr & FLYING) == 0) {
    /*** Find First track base section ***/
    facePtr = track->faces + remoteShip->nearTrkSect->faceStart;
    while ((facePtr->flags & TRACK_BASE) == 0) {
      facePtr++;
    }

    remoteShip->updateControl = HoldCentre;

    (remoteShip->updateControl)(&offSetVector, shipIndex, facePtr, shipNo);

    if ((remoteShip->remoteMaxThrust > remoteShip->speed)) {
      remoteShip->speed += remoteShip->remoteThrustMag;
    }

    /***************************************************************************/
    /***************************************************************************/
    /***************************************************************************/
    /***************************************************************************/

    /************************************************************************/
    /**** Make decision as to which path the craft will take at a		  	****/
    /**** junction																			****/
    /************************************************************************/

    section = remoteShip->nearTrkSect->prevSection.ptr;

    for (i = 0; i < 4; i++) {
      section = section->nextSection.ptr;
    }

    if (section->junction.index != -1) {
      if (section->junction.ptr->flags & JunctionStart) {
        decide = randy() % 2;
        if (decide == 0)
          remoteShip->attr |= JUNC_LEFT;
        else
          remoteShip->attr &= ~JUNC_LEFT;

        /* rob */
        remoteShip->attr &= ~JUNC_LEFT;
      }
    }

    section = remoteShip->nearTrkSect->prevSection.ptr;

    for (i = 0; i < 4; i++) {
      if (section->junction.index != -1) {
        if (section->junction.ptr->flags & JunctionStart) {
          if (remoteShip->attr & JUNC_LEFT) {
            section = section->junction.ptr;
          } else
            section = section->nextSection.ptr;
        } else
          section = section->nextSection.ptr;
      } else
        section = section->nextSection.ptr;
    }
    nextSection = section->nextSection.ptr;
    section = remoteShip->nearTrkSect;

    /***************************************************************************/
    /**** 					General routines - Non decision based					****/
    /***************************************************************************/

    /***** Bleed off speed as orientation changes *****/
#if 0
      remoteShip->speed -= (abs(remoteShip->speed * remoteShip->vhdg))>>14 ;	  /* 14 */
		remoteShip->speed -= (abs(remoteShip->speed * remoteShip->vpitch))>>14 ;
#endif
    if (remoteShip->nearTrkSect->flags & Jump) /**** Cure for ships getting stuck on hump lip ****/
    {
      targetVector.vx = remoteShip->nearTrkSect->centre.vx - remoteShip->nearTrkSect->prevSection.ptr->centre.vx;
      targetVector.vy = remoteShip->nearTrkSect->centre.vy - remoteShip->nearTrkSect->prevSection.ptr->centre.vy;
      targetVector.vz = remoteShip->nearTrkSect->centre.vz - remoteShip->nearTrkSect->prevSection.ptr->centre.vz;
    } else {
      targetVector.vx = nextSection->centre.vx - section->centre.vx;
      targetVector.vy = nextSection->centre.vy - section->centre.vy;
      targetVector.vz = nextSection->centre.vz - section->centre.vz;
    }

    gap_mag = GetMagnitude(targetVector);

    targetVector.vx = (targetVector.vx * remoteShip->speed) / gap_mag;
    targetVector.vy = (targetVector.vy * remoteShip->speed) / gap_mag;
    targetVector.vz = (targetVector.vz * remoteShip->speed) / gap_mag;

    path1.vx = section->centre.vx + offSetVector.vx;
    path1.vy = section->centre.vy + offSetVector.vy;
    path1.vz = section->centre.vz + offSetVector.vz;

    path2.vx = nextSection->centre.vx + offSetVector.vx;
    path2.vy = nextSection->centre.vy + offSetVector.vy;
    path2.vz = nextSection->centre.vz + offSetVector.vz;

    GetTargetPos(&bestPath, remoteShip->ppivot, path2, path1);

    remoteShip->apivot.vx = (targetVector.vx + ((bestPath.vx - remoteShip->ppivot.vx) >> 1));
    remoteShip->apivot.vy = (targetVector.vy + ((bestPath.vy - remoteShip->ppivot.vy) >> 1));
    remoteShip->apivot.vz = (targetVector.vz + ((bestPath.vz - remoteShip->ppivot.vz) >> 1));

    remoteShip->apivot.vx = (remoteShip->apivot.vx * 60) / FR60;
    remoteShip->apivot.vy = (remoteShip->apivot.vy * 60) / FR60;
    remoteShip->apivot.vz = (remoteShip->apivot.vz * 60) / FR60;

#if COLLIDE_TEST
    remoteShip->vpivot.vx = 0;
    remoteShip->vpivot.vy = 0;
    remoteShip->vpivot.vz = 0;
#else
    remoteShip->vpivot.vx += remoteShip->apivot.vx;
    remoteShip->vpivot.vy += remoteShip->apivot.vy;
    remoteShip->vpivot.vz += remoteShip->apivot.vz;
#endif
    /***** If remote has gone over speed-up *****/

    /****** Speed Test, should not make much difference ******/
    /****** to overall lap times of opposition          ******/
    facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
    facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
    facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
    alpha = PlaneLineCollision(facePoint, remoteShip->ppivot, facePtr->normal);

    if (alpha < 50)
      alpha = 50;

    remoteShip->apivot.vx = (((facePtr->normal.vx * TARGETHEIGHT * TRACK_MAGNET) / alpha) -
                             (facePtr->normal.vx * TRACK_MAGNET)) >>
                            8;
    remoteShip->apivot.vy = (((facePtr->normal.vy * TARGETHEIGHT * TRACK_MAGNET) / alpha) -
                             (facePtr->normal.vy * TRACK_MAGNET)) >>
                            8;
    remoteShip->apivot.vz = (((facePtr->normal.vz * TARGETHEIGHT * TRACK_MAGNET) / alpha) -
                             (facePtr->normal.vz * TRACK_MAGNET)) >>
                            8;

    remoteShip->apivot.vx = (remoteShip->apivot.vx * 60) / FR60;
    remoteShip->apivot.vy = (remoteShip->apivot.vy * 60) / FR60;
    remoteShip->apivot.vz = (remoteShip->apivot.vz * 60) / FR60;

    remoteShip->vpivot.vx += remoteShip->apivot.vx;
    remoteShip->vpivot.vy += remoteShip->apivot.vy;
    remoteShip->vpivot.vz += remoteShip->apivot.vz;

    temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

    remoteShip->vhdg = -ratan2(targetVector.vx, targetVector.vz) - remoteShip->hdg;
    remoteShip->vhdg += remoteShip->ahdg; /*** Result of weapon collision ***/
    remoteShip->ahdg -= remoteShip->ahdg >> 3;

    if (remoteShip->vhdg > 2048)
      remoteShip->vhdg = remoteShip->vhdg - 4096;
    else if (remoteShip->vhdg < -2048)
      remoteShip->vhdg = remoteShip->vhdg + 4096;

    remoteShip->vhdg = (remoteShip->vhdg * 60) / FR60;
    remoteShip->hdg += remoteShip->vhdg >> 4;

    remoteShip->vpitch = -ratan2(targetVector.vy, temp) - remoteShip->pitch;

    remoteShip->vpitch = (remoteShip->vpitch * 60) / FR60;

    remoteShip->pitch += remoteShip->vpitch >> 4;

    remoteShip->vroll += sar(remoteShip->vhdg, 2);
    remoteShip->vroll -= remoteShip->vroll >> 1;

    remoteShip->roll += remoteShip->vroll;
    remoteShip->roll -= remoteShip->roll >> 3;

  }

  /************************************************************************/
  /**** Ship is FLYING therefore 												  	****/
  /**** 																					****/
  /************************************************************************/

  else {
    section = remoteShip->nearTrkSect;
    section = section->nextSection.ptr;
    section = section->nextSection.ptr;
    nextSection = section->nextSection.ptr;

    remoteShip->updateControl = HoldCentre;
    (remoteShip->updateControl)(&offSetVector, myShip, facePtr, shipNo);

    if (remoteShip->remoteMaxThrust > remoteShip->speed) {
      remoteShip->speed += remoteShip->remoteThrustMag;
    }

    remoteShip->speed -= (abs(remoteShip->speed * remoteShip->vhdg)) >> 14;
    /*		remoteShip->speed -= (abs(remoteShip->speed * remoteShip->vpitch))>>14 ;
*/

    targetVector.vx = nextSection->centre.vx - section->centre.vx;
    targetVector.vy = nextSection->centre.vy - section->centre.vy;
    targetVector.vz = nextSection->centre.vz - section->centre.vz;

    gap_mag = GetMagnitude(targetVector);

    targetVector.vx = (targetVector.vx * remoteShip->speed) / gap_mag;
    /*		targetVector.vy = (targetVector.vy*remoteShip->speed)/gap_mag;*/
    targetVector.vz = (targetVector.vz * remoteShip->speed) / gap_mag;

    targetVector.vy = 500;

    GetTargetPos(&bestPath, remoteShip->ppivot, nextSection->centre, remoteShip->nearTrkSect->centre);

    remoteShip->apivot.vx = (targetVector.vx + ((bestPath.vx - remoteShip->ppivot.vx) >> 1));
    remoteShip->apivot.vy = targetVector.vy;
    remoteShip->apivot.vz = (targetVector.vz + ((bestPath.vz - remoteShip->ppivot.vz) >> 1));

    remoteShip->apivot.vx = (remoteShip->apivot.vx * 60) / FR60;
    remoteShip->apivot.vy = (remoteShip->apivot.vy * 60) / FR60;
    remoteShip->apivot.vz = (remoteShip->apivot.vz * 60) / FR60;

    remoteShip->vpivot.vx += remoteShip->apivot.vx;
    remoteShip->vpivot.vy += remoteShip->apivot.vy;
    remoteShip->vpivot.vz += remoteShip->apivot.vz;

    remoteShip->vpitch = -200 - remoteShip->pitch;

    remoteShip->pitch += remoteShip->vpitch >> 4;

    remoteShip->vhdg = -ratan2(targetVector.vx, targetVector.vz) - remoteShip->hdg;
    if (remoteShip->vhdg > 2048)
      remoteShip->vhdg = remoteShip->vhdg - 4096;
    else if (remoteShip->vhdg < -2048)
      remoteShip->vhdg = remoteShip->vhdg + 4096;
    remoteShip->hdg += remoteShip->vhdg >> 4;
  }

  remoteShip->vpivot.vx -= remoteShip->vpivot.vx >> 3;
  remoteShip->vpivot.vy -= remoteShip->vpivot.vy >> 3;
  remoteShip->vpivot.vz -= remoteShip->vpivot.vz >> 3;

  remoteShip->ppivot.vx += remoteShip->vpivot.vx >> 6;
  remoteShip->ppivot.vy += remoteShip->vpivot.vy >> 6;
  remoteShip->ppivot.vz += remoteShip->vpivot.vz >> 6;

  remoteShip->hdg = ang(remoteShip->hdg);
  remoteShip->pitch = ang(remoteShip->pitch);
  remoteShip->roll = ang(remoteShip->roll);
}

void UpdateShipLinearInterpolateSpline(ShipData* shipIndex, Object** shipShapes, int32_t shipNo, WeaponData* weaponIndex) {
  VECTOR pos;
  VECTOR pos1;
  VECTOR step;
  Prm prm;
  int16_t subdivisions = 8;
  VECTOR targetVector;
  int32_t temp;

  if (shipstepCount == subdivisions) {
    shipsplineCount += 1;
    shipstepCount = 0;

    if (shipsplineCount >= shipSpline[splineNumber]->primitiveCount - 2) {
      CameraUpdate = UpdateCameraExtroAttract;
      return;
    }
  }

  shipstepCount++;

  prm.primitive = shipSpline[splineNumber]->primitives;
  prm.spline += shipsplineCount;

  pos.vx = prm.spline->position.vx;
  pos.vy = prm.spline->position.vy;
  pos.vz = prm.spline->position.vz;

  pos1.vx = (prm.spline + 1)->position.vx;
  pos1.vy = (prm.spline + 1)->position.vy;
  pos1.vz = (prm.spline + 1)->position.vz;

  step.vx = (pos1.vx - pos.vx) / subdivisions;
  step.vy = (pos1.vy - pos.vy) / subdivisions;
  step.vz = (pos1.vz - pos.vz) / subdivisions;

  step.vx = (pos.vx + (subdivisions * step.vx) - shipIndex[ownShip].ppivot.vx);
  step.vy = (pos.vy + (subdivisions * step.vy) - shipIndex[ownShip].ppivot.vy);
  step.vz = (pos.vz + (subdivisions * step.vz) - shipIndex[ownShip].ppivot.vz);

  shipIndex[ownShip].vpivot.vx += step.vx;
  shipIndex[ownShip].vpivot.vy += step.vy;
  shipIndex[ownShip].vpivot.vz += step.vz;

  shipIndex[ownShip].vpivot.vx -= shipIndex[ownShip].vpivot.vx >> 3;
  shipIndex[ownShip].vpivot.vy -= shipIndex[ownShip].vpivot.vy >> 3;
  shipIndex[ownShip].vpivot.vz -= shipIndex[ownShip].vpivot.vz >> 3;

  pos.vx = shipIndex[ownShip].ppivot.vx + (shipIndex[ownShip].vpivot.vx >> 7);
  pos.vy = shipIndex[ownShip].ppivot.vy + (shipIndex[ownShip].vpivot.vy >> 7);
  pos.vz = shipIndex[ownShip].ppivot.vz + (shipIndex[ownShip].vpivot.vz >> 7);

  shipIndex[ownShip].ppivot.vx = pos.vx;
  shipIndex[ownShip].ppivot.vy = pos.vy;
  shipIndex[ownShip].ppivot.vz = pos.vz;

  shipIndex[ownShip].nearTrkSect = FindAllNearestSection(pos);

  targetVector.vx = pos1.vx - pos.vx;
  targetVector.vy = pos1.vy - pos.vy;
  targetVector.vz = pos1.vz - pos.vz;

  temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

  shipIndex[ownShip].vhdg = -ratan2(targetVector.vx, targetVector.vz) - shipIndex[ownShip].hdg;

  if (shipIndex[ownShip].vhdg > 2048) {
    shipIndex[ownShip].vhdg = shipIndex[ownShip].vhdg - 4096;
  } else if (shipIndex[ownShip].vhdg < -2048) {
    shipIndex[ownShip].vhdg = shipIndex[ownShip].vhdg + 4096;
  }

  shipIndex[ownShip].vhdg -= shipIndex[ownShip].vhdg >> 3;
  shipIndex[ownShip].hdg += shipIndex[ownShip].vhdg >> 4;

  shipIndex[ownShip].vpitch = -ratan2(targetVector.vy, temp) - shipIndex[ownShip].pitch;
  shipIndex[ownShip].vpitch -= shipIndex[ownShip].vpitch >> 3;
  shipIndex[ownShip].pitch += shipIndex[ownShip].vpitch >> 4;

  shipIndex[ownShip].hdg = ang(shipIndex[ownShip].hdg);
  shipIndex[ownShip].pitch = ang(shipIndex[ownShip].pitch);
}

void UpdateCameraLinearInterpolateSpline(TrackCamera* camera, ShipData* ship, RescueData* rescueDroid) {
  VECTOR pos;
  VECTOR pos1;
  VECTOR step;
  VECTOR targetVector;
  int32_t temp;
  Prm prm;
  int16_t subdivisions = 8;

  if (camerastepCount == subdivisions) {
    camerasplineCount += 1;
    camerastepCount = 0;

    if (camerasplineCount >= camSpline[splineNumber]->primitiveCount - 1) {
      CameraUpdate = UpdateCameraExtroAttract;
      return;
    }
  }

  camerastepCount++;

  prm.primitive = camSpline[splineNumber]->primitives;
  prm.spline += camerasplineCount;

  pos.vx = prm.spline->position.vx;
  pos.vy = prm.spline->position.vy;
  pos.vz = prm.spline->position.vz;

  pos1.vx = (prm.spline + 1)->position.vx;
  pos1.vy = (prm.spline + 1)->position.vy;
  pos1.vz = (prm.spline + 1)->position.vz;

  step.vx = (pos1.vx - pos.vx) / subdivisions;
  step.vy = (pos1.vy - pos.vy) / subdivisions;
  step.vz = (pos1.vz - pos.vz) / subdivisions;

  step.vx = (pos.vx + (subdivisions * step.vx) + camera->camPos->relative.t[0]) >> 7;
  step.vy = (pos.vy + (subdivisions * step.vy) + camera->camPos->relative.t[1]) >> 7;
  step.vz = (pos.vz + (subdivisions * step.vz) + camera->camPos->relative.t[2]) >> 7;

  camera->vel.vx += step.vx;
  camera->vel.vy += step.vy;
  camera->vel.vz += step.vz;

  camera->vel.vx -= camera->vel.vx >> 3;
  camera->vel.vy -= camera->vel.vy >> 3;
  camera->vel.vz -= camera->vel.vz >> 3;

  pos.vx = -camera->camPos->relative.t[0] + (camera->vel.vx);
  pos.vy = -camera->camPos->relative.t[1] + (camera->vel.vy);
  pos.vz = -camera->camPos->relative.t[2] + (camera->vel.vz);
  /*
   camera->section = FindAllNearestSection( pos );
*/
  camera->section = FindNearestSection(camera->section, pos);

  targetVector.vx = ship[ownShip].ppivot.vx - pos.vx;
  targetVector.vy = ship[ownShip].ppivot.vy - pos.vy;
  targetVector.vz = ship[ownShip].ppivot.vz - pos.vz;

  temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

  SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

  camera->vhdg = ratan2(targetVector.vx, targetVector.vz) - camera->hdg;

  if (camera->vhdg > 2048) {
    camera->vhdg = camera->vhdg - 4096;
  } else if (camera->vhdg < -2048) {
    camera->vhdg = camera->vhdg + 4096;
  }

  camera->vhdg -= camera->vhdg >> 3;

  camera->hdg += camera->vhdg >> 4;

  camera->vpitch = ratan2(targetVector.vy, temp) - camera->pitch;
  camera->vpitch -= camera->vpitch >> 3;
  camera->pitch += camera->vpitch >> 4;

  camera->hdg = ang(camera->hdg);
  camera->pitch = ang(camera->pitch);

  /*
   camera->hdg = ratan2( targetVector.vx, targetVector.vz );
 	camera->pitch = ratan2( targetVector.vy, temp );
*/
  SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

  camera->camPos->super->update = 1;
}

/*

This is the camera high up looking down

*/
void UpdateCameraHighUp(TrackCamera* camera, ShipData* ship, RescueData* rescueDroid) {
  VECTOR pos;
  int32_t temp;
  VECTOR targetVector;

  camera->section = ship[ownShip].nearTrkSect;

  timeOut--;
  if (timeOut <= 0) {
    CameraUpdate = UpdateCameraHighUp;
    CameraUpdate = UpdateCameraExtroAttract;
  }

  pos.vx = ship[ownShip].ppivot.vx - camVx;
  pos.vy = ship[ownShip].ppivot.vy - camVy;
  pos.vz = ship[ownShip].ppivot.vz - camVz;

  targetVector.vx = ship[ownShip].ppivot.vx - pos.vx;
  targetVector.vy = ship[ownShip].ppivot.vy - pos.vy;
  targetVector.vz = ship[ownShip].ppivot.vz - pos.vz;

  temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

  SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

  camera->hdg = ratan2(targetVector.vx, targetVector.vz);
  camera->pitch = ratan2(targetVector.vy, temp);
  SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

  camera->camPos->super->update = 1;

#if 0
   if ( startIceText )
   {
      ShowIceCongratulations( );
   }
#endif
}

/*

This is the camera high up looking down

*/
void UpdateCameraStationary(TrackCamera* camera, ShipData* ship, RescueData* rescueDroid) {
  VECTOR pos;
  int32_t temp;
  VECTOR targetVector;

  timeOut--;
  if (timeOut <= 0) {
    CameraUpdate = UpdateCameraExtroAttract;
  }

  pos.vx = camera->section->centre.vx - camVx;
  pos.vy = camera->section->centre.vy - camVy;
  pos.vz = camera->section->centre.vz - camVz;

  targetVector.vx = ship[ownShip].ppivot.vx - pos.vx;
  targetVector.vy = ship[ownShip].ppivot.vy - pos.vy;
  targetVector.vz = ship[ownShip].ppivot.vz - pos.vz;

  temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

  SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

  camera->hdg = ratan2(targetVector.vx, targetVector.vz);
  camera->pitch = ratan2(targetVector.vy, temp);
  SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

  camera->camPos->super->update = 1;

#if 0
   if ( startIceText )
   {
      ShowIceCongratulations( );
   }
#endif
}

void UpdateCameraExtroAttract(TrackCamera* camera, ShipData* camShip, RescueData* rescueDroid) {
  int16_t i;
  static int16_t count = 0;
  TrackSection* startSection;
  TrackSection* nextSection;
  VECTOR targetVector;
  Prm prm;
  VECTOR pos;
  int32_t temp;

  /* firestar */
  if (extroLevel == 14) {
#if 0
      count = 2;
#endif
    switch (count) {
    case 0:
      /* set the remote ships to somewhere around the track */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = track->sections + i + 50;
        camShip[i].ppivot.vx = (track->sections + i + 50)->centre.vx;
        camShip[i].ppivot.vy = (track->sections + i + 50)->centre.vy;
        camShip[i].ppivot.vz = (track->sections + i + 50)->centre.vz;
      }

      /* the spline we want to play */
      splineNumber = 0;

      /* set the initial ship position */
      camShip[ownShip].update = UpdateShipLinearInterpolateSpline;

      prm.primitive = shipSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      camShip[ownShip].nearTrkSect = FindAllNearestSection(pos);
      camShip[ownShip].ppivot.vx = pos.vx;
      camShip[ownShip].ppivot.vy = pos.vy;
      camShip[ownShip].ppivot.vz = pos.vz;

      /* reset ship direction */
      camShip[ownShip].roll = 0;
      camShip[ownShip].vroll = 0;

      /* set the initial camera position */
      prm.primitive = camSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

      camera->section = FindAllNearestSection(pos);

      /* set the camera direction */
      targetVector.vx = camShip[ownShip].ppivot.vx - pos.vx;
      targetVector.vy = camShip[ownShip].ppivot.vy - pos.vy;
      targetVector.vz = camShip[ownShip].ppivot.vz - pos.vz;

      temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

      camera->hdg = ratan2(targetVector.vx, targetVector.vz);
      camera->pitch = ratan2(targetVector.vy, temp);

      SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

      camera->camPos->super->update = 1;

      /* initialize the camera velocities */
      camera->vel.vx = 0;
      camera->vel.vy = 0;
      camera->vel.vz = 0;

      /* reset the spline counts */
      shipsplineCount = 0;
      shipstepCount = 0;

      camerasplineCount = 0;
      camerastepCount = 0;

      /* set the camera update */
      CameraUpdate = UpdateCameraLinearInterpolateSpline;

      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 1:
      /* high camera */
      /* put the remotes out of the way */

      startFireText = 1;
      textY = 300;

      for (i = 0; i < NO_OF_SHIPS; i++) {
        startSection = track->sections + 0 + i + 1;

        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = startSection;
        camShip[i].ppivot.vx = startSection->centre.vx;
        camShip[i].ppivot.vy = startSection->centre.vy;
        camShip[i].ppivot.vz = startSection->centre.vz;

        nextSection = startSection->nextSection.ptr;
        targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
        targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
        targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
        camShip[i].hdg = -ratan2(targetVector.vx, targetVector.vz);
      }

      startSection = track->sections + 50;

      camShip[ownShip].update = UpdateShipRemote;
      camShip[ownShip].nearTrkSect = startSection;
      camShip[ownShip].ppivot.vx = startSection->centre.vx;
      camShip[ownShip].ppivot.vy = startSection->centre.vy;
      camShip[ownShip].ppivot.vz = startSection->centre.vz;

      camVx = 4350;
      camVy = 9850;
      camVz = 4400;
#if PAL
      timeOut = 25 * 13;
#else
      timeOut = FR30 * 13;
#endif
      CameraUpdate = UpdateCameraHighUp;
      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 2:
      /* spline */

      /* set the remote ships to somewhere around the track */

      startFireText = 0;

      for (i = 0; i < NO_OF_SHIPS; i++) {
        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = track->sections + i;
        camShip[i].ppivot.vx = (track->sections + i)->centre.vx;
        camShip[i].ppivot.vy = (track->sections + i)->centre.vy;
        camShip[i].ppivot.vz = (track->sections + i)->centre.vz;
      }

      /* the spline we want to play */
      splineNumber = 1;

      /* set the initial ship position */
      camShip[ownShip].update = UpdateShipLinearInterpolateSpline;

      prm.primitive = shipSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      camShip[ownShip].nearTrkSect = FindAllNearestSection(pos);
      camShip[ownShip].ppivot.vx = pos.vx;
      camShip[ownShip].ppivot.vy = pos.vy;
      camShip[ownShip].ppivot.vz = pos.vz;

      /* reset ship direction */
      camShip[ownShip].roll = 0;
      camShip[ownShip].vroll = 0;

      /* set the initial camera position */
      prm.primitive = camSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

      camera->section = FindAllNearestSection(pos);

      /* set the camera direction */
      targetVector.vx = camShip[ownShip].ppivot.vx - pos.vx;
      targetVector.vy = camShip[ownShip].ppivot.vy - pos.vy;
      targetVector.vz = camShip[ownShip].ppivot.vz - pos.vz;

      temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

      camera->hdg = ratan2(targetVector.vx, targetVector.vz);
      camera->pitch = ratan2(targetVector.vy, temp);

      SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

      camera->camPos->super->update = 1;

      /* initialize the camera velocities */
      camera->vel.vx = 0;
      camera->vel.vy = 0;
      camera->vel.vz = 0;

      /* reset the spline counts */
      shipsplineCount = 0;
      shipstepCount = 0;

      camerasplineCount = 0;
      camerastepCount = 0;

      /* set the camera update */
      CameraUpdate = UpdateCameraLinearInterpolateSpline;

      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 3:
      /* spline */

      /* set the remote ships to somewhere around the track */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = track->sections + i;
        camShip[i].ppivot.vx = (track->sections + i)->centre.vx;
        camShip[i].ppivot.vy = (track->sections + i)->centre.vy;
        camShip[i].ppivot.vz = (track->sections + i)->centre.vz;
      }

      /* the spline we want to play */
      splineNumber = 2;

      /* set the initial ship position */
      camShip[ownShip].update = UpdateShipLinearInterpolateSpline;

      prm.primitive = shipSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      camShip[ownShip].nearTrkSect = FindAllNearestSection(pos);
      camShip[ownShip].ppivot.vx = pos.vx;
      camShip[ownShip].ppivot.vy = pos.vy;
      camShip[ownShip].ppivot.vz = pos.vz;

      /* reset ship direction */
      camShip[ownShip].roll = 0;
      camShip[ownShip].vroll = 0;

      /* set the initial camera position */
      prm.primitive = camSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

      camera->section = FindAllNearestSection(pos);

      /* set the camera direction */
      targetVector.vx = camShip[ownShip].ppivot.vx - pos.vx;
      targetVector.vy = camShip[ownShip].ppivot.vy - pos.vy;
      targetVector.vz = camShip[ownShip].ppivot.vz - pos.vz;

      temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

      camera->hdg = ratan2(targetVector.vx, targetVector.vz);
      camera->pitch = ratan2(targetVector.vy, temp);

      SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

      camera->camPos->super->update = 1;

      /* initialize the camera velocities */
      camera->vel.vx = 0;
      camera->vel.vy = 0;
      camera->vel.vz = 0;

      /* reset the spline counts */
      shipsplineCount = 0;
      shipstepCount = 0;

      camerasplineCount = 0;
      camerastepCount = 0;

      /* set the camera update */
      CameraUpdate = UpdateCameraLinearInterpolateSpline;

      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 4:
      /* spline */

      /* set the remote ships to somewhere around the track */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = track->sections + i;
        camShip[i].ppivot.vx = (track->sections + i)->centre.vx;
        camShip[i].ppivot.vy = (track->sections + i)->centre.vy;
        camShip[i].ppivot.vz = (track->sections + i)->centre.vz;
      }

      /* the spline we want to play */
      splineNumber = 3;

      /* set the initial ship position */
      camShip[ownShip].update = UpdateShipLinearInterpolateSpline;

      prm.primitive = shipSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      camShip[ownShip].nearTrkSect = FindAllNearestSection(pos);
      camShip[ownShip].ppivot.vx = pos.vx;
      camShip[ownShip].ppivot.vy = pos.vy;
      camShip[ownShip].ppivot.vz = pos.vz;

      /* reset ship direction */
      camShip[ownShip].roll = 0;
      camShip[ownShip].vroll = 0;

      /* set the initial camera position */
      prm.primitive = camSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

      camera->section = FindAllNearestSection(pos);

      /* set the camera direction */
      targetVector.vx = camShip[ownShip].ppivot.vx - pos.vx;
      targetVector.vy = camShip[ownShip].ppivot.vy - pos.vy;
      targetVector.vz = camShip[ownShip].ppivot.vz - pos.vz;

      temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

      camera->hdg = ratan2(targetVector.vx, targetVector.vz);
      camera->pitch = ratan2(targetVector.vy, temp);

      SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

      camera->camPos->super->update = 1;

      /* initialize the camera velocities */
      camera->vel.vx = 0;
      camera->vel.vy = 0;
      camera->vel.vz = 0;

      /* reset the spline counts */
      shipsplineCount = 0;
      shipstepCount = 0;

      camerasplineCount = 0;
      camerastepCount = 0;

      /* set the camera update */
      CameraUpdate = UpdateCameraLinearInterpolateSpline;

      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 5:
      //            printf( "\nfinished firestar extro \n" );
      finishedExtro = 1;

      break;

    default:
      Error("spline.c::UpdateCameraExtroAttract():Reached default in firestar", Fatal);
      break;
    }

    count++;

#if RobTest
    if (count > 4) {
      count = 0;
    }
#endif
  }

  /* silverstream */
  if (extroLevel == 13) {
#if 0
      count = 4;
#endif
    switch (count) {
    case 0:
      /* spline */

      /* set the remote ships to somewhere around the track */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = track->sections + i + 100;
        camShip[i].ppivot.vx = (track->sections + i + 100)->centre.vx;
        camShip[i].ppivot.vy = (track->sections + i + 100)->centre.vy;
        camShip[i].ppivot.vz = (track->sections + i + 100)->centre.vz;
      }

      /* the spline we want to play */
      splineNumber = 4;

      /* set the initial ship position */
      camShip[ownShip].update = UpdateShipLinearInterpolateSpline;

      prm.primitive = shipSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      camShip[ownShip].nearTrkSect = FindAllNearestSection(pos);
      camShip[ownShip].ppivot.vx = pos.vx;
      camShip[ownShip].ppivot.vy = pos.vy;
      camShip[ownShip].ppivot.vz = pos.vz;

      /* reset ship direction */
      camShip[ownShip].roll = 0;
      camShip[ownShip].vroll = 0;

      /* set the initial camera position */
      prm.primitive = camSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

      camera->section = FindAllNearestSection(pos);

      /* set the camera direction */
      targetVector.vx = camShip[ownShip].ppivot.vx - pos.vx;
      targetVector.vy = camShip[ownShip].ppivot.vy - pos.vy;
      targetVector.vz = camShip[ownShip].ppivot.vz - pos.vz;

      temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

      camera->hdg = ratan2(targetVector.vx, targetVector.vz);
      camera->pitch = ratan2(targetVector.vy, temp);

      SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

      camera->camPos->super->update = 1;

      /* initialize the camera velocities */
      camera->vel.vx = 0;
      camera->vel.vy = 0;
      camera->vel.vz = 0;

      /* reset the spline counts */
      shipsplineCount = 0;
      shipstepCount = 0;

      camerasplineCount = 0;
      camerastepCount = 0;

      /* set the camera update */
      CameraUpdate = UpdateCameraLinearInterpolateSpline;

      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 1:
      /* stationary camera */
      /* put the remotes out of the way */

      startIceText = 1;
      textY = 300;

      for (i = 0; i < NO_OF_SHIPS; i++) {
        startSection = track->sections + 90 + i + 1;

        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = startSection;
        camShip[i].ppivot.vx = startSection->centre.vx;
        camShip[i].ppivot.vy = startSection->centre.vy;
        camShip[i].ppivot.vz = startSection->centre.vz;

        nextSection = startSection->nextSection.ptr;
        targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
        targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
        targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
        camShip[i].hdg = -ratan2(targetVector.vx, targetVector.vz);
      }

      startSection = track->sections + 75;

      camShip[ownShip].update = UpdateShipRemote;
      camShip[ownShip].nearTrkSect = startSection;
      camShip[ownShip].ppivot.vx = startSection->centre.vx;
      camShip[ownShip].ppivot.vy = startSection->centre.vy;
      camShip[ownShip].ppivot.vz = startSection->centre.vz;

      camVx = 2000;
      camVy = 2750;
      camVz = 0;

#if PAL
      timeOut = 25 * 3;
#else
      timeOut = FR30 * 3;
#endif

      camera->section = startSection + 12;

      CameraUpdate = UpdateCameraStationary;
      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 2:
      /* stationary camera */
      /* put the remotes out of the way */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        startSection = track->sections + 90 + i + 1;

        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = startSection;
        camShip[i].ppivot.vx = startSection->centre.vx;
        camShip[i].ppivot.vy = startSection->centre.vy;
        camShip[i].ppivot.vz = startSection->centre.vz;

        nextSection = startSection->nextSection.ptr;
        targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
        targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
        targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
        camShip[i].hdg = -ratan2(targetVector.vx, targetVector.vz);
      }

      startSection = track->sections + 120;

      camShip[ownShip].update = UpdateShipRemote;
      camShip[ownShip].nearTrkSect = startSection;
      camShip[ownShip].ppivot.vx = startSection->centre.vx;
      camShip[ownShip].ppivot.vy = startSection->centre.vy;
      camShip[ownShip].ppivot.vz = startSection->centre.vz;

      camVx = 2250;
      camVy = 1500;
      camVz = -1500;

#if PAL
      timeOut = 25 * 6;
#else
      timeOut = FR30 * 6;
#endif

      camera->section = startSection + 12;

      CameraUpdate = UpdateCameraStationary;
      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 3:
      /* high camera */
      /* put the remotes out of the way */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        startSection = track->sections + 50 + i + 1;

        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = startSection;
        camShip[i].ppivot.vx = startSection->centre.vx;
        camShip[i].ppivot.vy = startSection->centre.vy;
        camShip[i].ppivot.vz = startSection->centre.vz;

        nextSection = startSection->nextSection.ptr;
        targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
        targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
        targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
        camShip[i].hdg = -ratan2(targetVector.vx, targetVector.vz);
      }

      startSection = track->sections + 193;

      camShip[ownShip].update = UpdateShipRemote;
      camShip[ownShip].nearTrkSect = startSection;
      camShip[ownShip].ppivot.vx = startSection->centre.vx;
      camShip[ownShip].ppivot.vy = startSection->centre.vy;
      camShip[ownShip].ppivot.vz = startSection->centre.vz;

      nextSection = startSection->nextSection.ptr;
      targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
      targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
      targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
      camShip[ownShip].hdg = -ratan2(targetVector.vx, targetVector.vz);

      camVx = 350;
      camVy = 11100;
      camVz = -2850;

#if PAL
      timeOut = 25 * 6;
#else
      timeOut = FR30 * 6;
#endif
      CameraUpdate = UpdateCameraHighUp;
      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 4:
      /* spline */

      /* set the remote ships to somewhere around the track */

      startIceText = 0;

      for (i = 0; i < NO_OF_SHIPS; i++) {
        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = track->sections + i + 100;
        camShip[i].ppivot.vx = (track->sections + i + 100)->centre.vx;
        camShip[i].ppivot.vy = (track->sections + i + 100)->centre.vy;
        camShip[i].ppivot.vz = (track->sections + i + 100)->centre.vz;
      }

      /* the spline we want to play */
      splineNumber = 5;

      /* set the initial ship position */
      camShip[ownShip].update = UpdateShipLinearInterpolateSpline;

      prm.primitive = shipSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      camShip[ownShip].nearTrkSect = FindAllNearestSection(pos);
      camShip[ownShip].ppivot.vx = pos.vx;
      camShip[ownShip].ppivot.vy = pos.vy;
      camShip[ownShip].ppivot.vz = pos.vz;

      /* reset ship direction */
      camShip[ownShip].roll = 0;
      camShip[ownShip].vroll = 0;

      startSection = camShip[ownShip].nearTrkSect;
      nextSection = startSection->nextSection.ptr;
      targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
      targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
      targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
      camShip[ownShip].hdg = -ratan2(targetVector.vx, targetVector.vz);

      /* set the initial camera position */
      prm.primitive = camSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

      camera->section = FindAllNearestSection(pos);

      /* set the camera direction */
      targetVector.vx = camShip[ownShip].ppivot.vx - pos.vx;
      targetVector.vy = camShip[ownShip].ppivot.vy - pos.vy;
      targetVector.vz = camShip[ownShip].ppivot.vz - pos.vz;

      temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

      camera->hdg = ratan2(targetVector.vx, targetVector.vz);
      camera->pitch = ratan2(targetVector.vy, temp);

      SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

      camera->camPos->super->update = 1;

      /* initialize the camera velocities */
      camera->vel.vx = 0;
      camera->vel.vy = 0;
      camera->vel.vz = 0;

      /* reset the spline counts */
      shipsplineCount = 0;
      shipstepCount = 0;

      camerasplineCount = 0;
      camerastepCount = 0;

      /* set the camera update */
      CameraUpdate = UpdateCameraLinearInterpolateSpline;

      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 5:
      /* spline */

      /* set the remote ships to somewhere around the track */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = track->sections + i + 100;
        camShip[i].ppivot.vx = (track->sections + i + 100)->centre.vx;
        camShip[i].ppivot.vy = (track->sections + i + 100)->centre.vy;
        camShip[i].ppivot.vz = (track->sections + i + 100)->centre.vz;
      }

      /* the spline we want to play */
      splineNumber = 6;

      /* set the initial ship position */
      camShip[ownShip].update = UpdateShipLinearInterpolateSpline;

      prm.primitive = shipSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      camShip[ownShip].nearTrkSect = FindAllNearestSection(pos);
      camShip[ownShip].ppivot.vx = pos.vx;
      camShip[ownShip].ppivot.vy = pos.vy;
      camShip[ownShip].ppivot.vz = pos.vz;

      /* reset ship direction */
      camShip[ownShip].roll = 0;
      camShip[ownShip].vroll = 0;

      /* set the initial camera position */
      prm.primitive = camSpline[splineNumber]->primitives;

      pos.vx = (prm.spline)->position.vx;
      pos.vy = (prm.spline)->position.vy;
      pos.vz = (prm.spline)->position.vz;

      SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);

      camera->section = FindAllNearestSection(pos);

      /* set the camera direction */
      targetVector.vx = camShip[ownShip].ppivot.vx - pos.vx;
      targetVector.vy = camShip[ownShip].ppivot.vy - pos.vy;
      targetVector.vz = camShip[ownShip].ppivot.vz - pos.vz;

      temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

      camera->hdg = ratan2(targetVector.vx, targetVector.vz);
      camera->pitch = ratan2(targetVector.vy, temp);

      SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0);

      camera->camPos->super->update = 1;

      /* initialize the camera velocities */
      camera->vel.vx = 0;
      camera->vel.vy = 0;
      camera->vel.vz = 0;

      /* reset the spline counts */
      shipsplineCount = 0;
      shipstepCount = 0;

      camerasplineCount = 0;
      camerastepCount = 0;

      /* set the camera update */
      CameraUpdate = UpdateCameraLinearInterpolateSpline;

      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 6:
      /* high camera */
      /* put the remotes out of the way */
      for (i = 0; i < NO_OF_SHIPS; i++) {
        startSection = track->sections + 90 + i + 1;

        camShip[i].update = UpdateShipRemote;
        camShip[i].nearTrkSect = startSection;
        camShip[i].ppivot.vx = startSection->centre.vx;
        camShip[i].ppivot.vy = startSection->centre.vy;
        camShip[i].ppivot.vz = startSection->centre.vz;

        nextSection = startSection->nextSection.ptr;
        targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
        targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
        targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
        camShip[i].hdg = -ratan2(targetVector.vx, targetVector.vz);
      }

      startSection = track->sections + 335;

      camShip[ownShip].update = UpdateShipRemote;
      camShip[ownShip].nearTrkSect = startSection;
      camShip[ownShip].ppivot.vx = startSection->centre.vx;
      camShip[ownShip].ppivot.vy = startSection->centre.vy;
      camShip[ownShip].ppivot.vz = startSection->centre.vz;

      nextSection = startSection->nextSection.ptr;
      targetVector.vx = nextSection->centre.vx - startSection->centre.vx;
      targetVector.vy = nextSection->centre.vy - startSection->centre.vy;
      targetVector.vz = nextSection->centre.vz - startSection->centre.vz;
      camShip[ownShip].hdg = -ratan2(targetVector.vx, targetVector.vz);

      camShip[ownShip].vpivot.vx = targetVector.vx >> 2;
      camShip[ownShip].vpivot.vy = targetVector.vy >> 2;
      camShip[ownShip].vpivot.vz = targetVector.vz >> 2;

      camVx = 1100;
      camVy = 7850;
      camVz = 4400;

#if PAL
      timeOut = 25 * 7;
#else
      timeOut = FR30 * 7;
#endif

      CameraUpdate = UpdateCameraHighUp;
      camShip[ownShip].attr &= ~VIEW_IN;
      break;

    case 7:
      //            printf( "\nfinished silverstream extro \n" );
      finishedExtro = 1;
      break;

    default:
      Error("spline.c::UpdateCameraExtroAttract():Reached default in silverstream", Fatal);
      break;
    }

    count++;

#if RobTest
    if (count > 6) {
      count = 0;
    }
#endif
  }

  if (!finishedExtro) {
    (CameraUpdate)(camera, camShip, rescueDroid);
  }
}

void InitFireCongratulations() {
  con[0].words = c1;
  con[0].colour = Red;
  con[0].size = 8;

  con[1].words = c2;
  con[1].colour = White;
  con[1].size = 8;

  con[2].words = c3;
  con[2].colour = White;
  con[2].size = 8;

  con[3].words = c4;
  con[3].colour = White;
  con[3].size = 8;

  con[4].words = c5;
  con[4].colour = Red;
  con[4].size = 8;

  con[5].words = c6;
  con[5].colour = White;
  con[5].size = 8;

  con[6].words = c7;
  con[6].colour = White;
  con[6].size = 8;

  con[7].words = c8;
  con[7].colour = White;
  con[7].size = 8;

  con[8].words = c9;
  con[8].colour = White;
  con[8].size = 8;

  con[9].words = c10;
  con[9].colour = White;
  con[9].size = 8;

  con[10].words = c11;
  con[10].colour = White;
  con[10].size = 8;

  con[11].words = c12;
  con[11].colour = White;
  con[11].size = 8;

  con[12].words = c13;
  con[12].colour = Red;
  con[12].size = 16;

  con[13].words = c14;
  con[13].colour = White;
  con[13].size = 8;

  con[14].words = c15;
  con[14].colour = White;
  con[14].size = 8;

  con[15].words = c16;
  con[15].colour = White;
  con[15].size = 8;

  con[16].words = c17;
  con[16].colour = White;
  con[16].size = 8;

  con[17].words = c18;
  con[17].colour = Red;
  con[17].size = 8;

  con[18].words = c19;
  con[18].colour = White;
  con[18].size = 8;

  con[19].words = c20;
  con[19].colour = White;
  con[19].size = 8;

  con[20].words = c21;
  con[20].colour = White;
  con[20].size = 8;

  con[21].words = c22;
  con[21].colour = White;
  con[21].size = 8;

  con[22].words = c23;
  con[22].colour = White;
  con[22].size = 8;

  con[23].words = c24;
  con[23].colour = Red;
  con[23].size = 8;

  con[24].words = c25;
  con[24].colour = White;
  con[24].size = 8;

  con[25].words = c26;
  con[25].colour = White;
  con[25].size = 8;
}

void InitIceCongratulations() {
  con[0].words = i1;
  con[0].colour = White;
  con[0].size = 8;

  con[1].words = i2;
  con[1].colour = White;
  con[1].size = 8;

  con[2].words = i3;
  con[2].colour = White;
  con[2].size = 8;

  con[3].words = i4;
  con[3].colour = White;
  con[3].size = 8;

  con[4].words = i5;
  con[4].colour = White;
  con[4].size = 8;

  con[5].words = i6;
  con[5].colour = White;
  con[5].size = 8;

  con[6].words = i7;
  con[6].colour = White;
  con[6].size = 8;

  con[7].words = i8;
  con[7].colour = White;
  con[7].size = 8;

  con[8].words = i9;
  con[8].colour = White;
  con[8].size = 8;

  con[9].words = i10;
  con[9].colour = White;
  con[9].size = 8;

  con[10].words = i11;
  con[10].colour = White;
  con[10].size = 8;

  con[11].words = i12;
  con[11].colour = White;
  con[11].size = 8;

  con[12].words = i13;
  con[12].colour = White;
  con[12].size = 8;

  con[13].words = i14;
  con[13].colour = White;
  con[13].size = 8;

  con[14].words = i15;
  con[14].colour = White;
  con[14].size = 8;

  con[15].words = i16;
  con[15].colour = White;
  con[15].size = 8;

  con[16].words = i17;
  con[16].colour = White;
  con[16].size = 8;

  con[17].words = i18;
  con[17].colour = White;
  con[17].size = 8;

  con[18].words = i19;
  con[18].colour = White;
  con[18].size = 8;

  con[19].words = i20;
  con[19].colour = White;
  con[19].size = 8;

  con[20].words = i21;
  con[20].colour = White;
  con[20].size = 8;

  con[21].words = i22;
  con[21].colour = White;
  con[21].size = 8;

  con[22].words = i23;
  con[22].colour = White;
  con[22].size = 8;
}

void ShowFireCongratulations() {
  int16_t lineY;
  int16_t i;

  for (i = 0; i < 26; i++) {
    lineY = textY + i * 12;

    if ((lineY > -10) && (lineY < 270)) {
      if (con[i].colour == White) {
        CentreText(con[i].words, text_data(0, lineY, con[i].size), WhiteText);
      } else {
        CentreText(con[i].words, text_data(0, lineY, con[i].size), RedText);
      }
    }
  }

  if (textY > -350) {
    textY -= 3;
  }
}

void ShowIceCongratulations() {
  int16_t lineY;
  int16_t i;

  for (i = 0; i < 23; i++) {
    lineY = textY + i * 12;

    if ((lineY > -10) && (lineY < 270)) {
      if (con[i].colour == White) {
        CentreText(con[i].words, text_data(0, lineY, con[i].size), WhiteText);
      } else {
        CentreText(con[i].words, text_data(0, lineY, con[i].size), RedText);
      }
    }
  }

  if (textY > -350) {
    textY -= 3;
  }
}
