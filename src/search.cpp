#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "search.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "libgte.h"

int32_t SectionSearch(ShipData* playerShip) {
  TrackSection *trk, *nearest_section;
  int32_t smallest_mag;
  VECTOR cent2ship;
  int32_t mag;
  int32_t i;
  int16_t juncChecked = 0;
  TrackSection* junc;

  trk = playerShip->nearTrkSect;

  /****** Start search several sections before current section ******/

  for (i = 0; i < LOOK_BACK; i++) {
    trk = trk->prevSection.ptr;
  }

  /****** Find vector from ship centre to track section under	 ******/
  /****** 	      				 consideration                    ******/

  cent2ship.vx = playerShip->ppivot.vx - trk->centre.vx;
  cent2ship.vy = (playerShip->ppivot.vy - trk->centre.vy) >> 2;
  cent2ship.vz = playerShip->ppivot.vz - trk->centre.vz;

  smallest_mag = ((cent2ship.vx * cent2ship.vx) +
                  (cent2ship.vy * cent2ship.vy) + (cent2ship.vz * cent2ship.vz));

  if (trk->junction.index != -1) {
    junc = trk->junction.ptr;
    juncChecked = 1;
  }

  nearest_section = trk;

  for (i = 0; i < LOOK_AHEAD; i++) {
    trk = trk->nextSection.ptr;

    if (trk->junction.index != -1) {
      junc = trk->junction.ptr;
      juncChecked = 1;
    }

    cent2ship.vx = playerShip->ppivot.vx - trk->centre.vx;
    cent2ship.vy = (playerShip->ppivot.vy - trk->centre.vy) >> 2;
    cent2ship.vz = playerShip->ppivot.vz - trk->centre.vz;
    mag = ((cent2ship.vx * cent2ship.vx) +
           (cent2ship.vy * cent2ship.vy) + (cent2ship.vz * cent2ship.vz));

    if (mag < smallest_mag) {
      smallest_mag = mag;
      nearest_section = trk;
    }
  }

  if (juncChecked == 1) {
    trk = junc;

    for (i = 0; i < LOOK_AHEAD; i++) {
      cent2ship.vx = playerShip->ppivot.vx - trk->centre.vx;
      cent2ship.vy = (playerShip->ppivot.vy - trk->centre.vy) >> 2;
      cent2ship.vz = playerShip->ppivot.vz - trk->centre.vz;
      mag = ((cent2ship.vx * cent2ship.vx) +
             (cent2ship.vy * cent2ship.vy) + (cent2ship.vz * cent2ship.vz));

      if (mag < smallest_mag) {
        smallest_mag = mag;
        nearest_section = trk;
      }
      if (junc->flags & JunctionStart)
        trk = trk->nextSection.ptr;
      else
        trk = trk->prevSection.ptr;
    }
  }

  smallest_mag = SquareRoot0(smallest_mag);

  if (smallest_mag > 3700) {
    playerShip->attr |= FLYING;
  } else
    playerShip->attr &= ~FLYING;

  playerShip->nearTrkSect = nearest_section;
  return (smallest_mag);
}

TrackSection* FindNearestSection(TrackSection* trk, VECTOR pos) {
  TrackSection* nearest_section;
  int32_t smallest_mag;
  VECTOR cent2ship;
  int32_t mag;
  int32_t i;
  int16_t juncChecked = 0;
  TrackSection* junc;

  /****** Start search several sections before current section ******/

  for (i = 0; i < LOOK_BACK; i++) {
    trk = trk->prevSection.ptr;
  }

  /****** Find vector from ship centre to track section under	 ******/
  /****** 	      				 consideration                    ******/

  cent2ship.vx = pos.vx - trk->centre.vx;
  cent2ship.vy = pos.vy - trk->centre.vy;
  cent2ship.vz = pos.vz - trk->centre.vz;

  smallest_mag = /*SquareRoot0*/ ((cent2ship.vx * cent2ship.vx) +
                                  (cent2ship.vy * cent2ship.vy) + (cent2ship.vz * cent2ship.vz));

  nearest_section = trk;

  if (trk->junction.index != -1) {
    junc = trk->junction.ptr;
    juncChecked = 1;
  }

  for (i = 0; i < LOOK_AHEAD; i++) {
    trk = trk->nextSection.ptr;

    if (trk->junction.index != -1) {
      junc = trk->junction.ptr;
      juncChecked = 1;
    }

    cent2ship.vx = pos.vx - trk->centre.vx;
    cent2ship.vy = pos.vy - trk->centre.vy;
    cent2ship.vz = pos.vz - trk->centre.vz;
    mag = /*SquareRoot0*/ ((cent2ship.vx * cent2ship.vx) +
                           (cent2ship.vy * cent2ship.vy) + (cent2ship.vz * cent2ship.vz));

    if (mag < smallest_mag) {
      smallest_mag = mag;
      nearest_section = trk;
    }
  }

  if (juncChecked == 1) {
    trk = junc;

    for (i = 0; i < LOOK_AHEAD; i++) {
      cent2ship.vx = pos.vx - trk->centre.vx;
      cent2ship.vy = pos.vy - trk->centre.vy;
      cent2ship.vz = pos.vz - trk->centre.vz;
      mag = /*SquareRoot0*/ ((cent2ship.vx * cent2ship.vx) +
                             (cent2ship.vy * cent2ship.vy) + (cent2ship.vz * cent2ship.vz));

      if (mag < smallest_mag) {
        smallest_mag = mag;
        nearest_section = trk;
      }
      if (junc->flags & JunctionStart)
        trk = trk->nextSection.ptr;
      else
        trk = trk->prevSection.ptr;
    }
  }

  return (nearest_section);
}
