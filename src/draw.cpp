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
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "file.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "tilelib.h"
#include "ttf.h"
#include "texplace.h"
#include "trakview.h"
#include "trackasm.h"
#include "effects.h"
#include "fileops.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "fri.h"
#include "libgte.h"
#include "utils.h"
#include "andyscn.h"

extern TrackCamera ctrlcamera;

char TrackDirection(
    TrackCamera* camera) {
  SVECTOR reference;
  int32_t angle;
  TrackSection* nextSection;

  nextSection = camera->section->nextSection.ptr;

  reference.vx = (int16_t)(nextSection->centre.vx - camera->section->centre.vx);
  reference.vy = (int16_t)(nextSection->centre.vy - camera->section->centre.vy);
  reference.vz = (int16_t)(nextSection->centre.vz - camera->section->centre.vz);

  angle = -ratan2(reference.vx, reference.vz) + camera->hdg;
  angle = ang(angle);

  if ((angle >= -512) && (angle < 512)) {
    return ShipForwards;
  } else if ((angle >= 512) && (angle < 1536)) {
    return ShipRight;
  } else if ((angle >= 1536) || (angle < -1536)) {
    return ShipBackwards;
  } else {
    return ShipLeft;
  }
}

int32_t count = 0;

int32_t c = 0;

void DrawLandscape(
    ShipData* playerShip,
    TrackCamera* camera) {
  int16_t i;
  VECTOR result;
  int32_t flag;
  TrackSection* section;
  int32_t extent;
  SVECTOR centre;

  for (i = 0; i < MaxUnique; i++) {
    sharedPrimCount[i] = 0;
  }

  section = camera->section;

  if (section->objectCount > 0) {
    for (i = 0; i < section->objectCount; i++) {
      objectTable[section->objects[i]]->skeleton->update = 1;

      SetSkeletonTransform(objectTable[section->objects[i]]->skeleton);

      extent = objectTable[section->objects[i]]->extent;

      centre.vx = 0;
      centre.vy = 0;
      centre.vz = 0;

      RotTrans(&centre, &result, &flag);

      result.vz += extent;

      if (result.vz == 0)
        result.vz = 1;

      result.vx = (result.vx * 160) / result.vz;
      result.vy = (result.vy * 160) / result.vz;

      extent = (extent * 160) / result.vz;

      if (
          (result.vx >= -(extent + 160)) &&
          (result.vx <= (extent + 160)) &&
          (result.vy >= -(extent + 120)) &&
          (result.vy <= (extent + 120))) {
#if 1

        if (playerShip->attr & VIEW_IN) {
          DrawObjectAsm(objectTable[section->objects[i]], ctrlcamera.camPos, playerShip->roll, 0);
        } else {
          DrawObjectAsm(objectTable[section->objects[i]], ctrlcamera.camPos, 0, 0);
        }

#else

        if (playerShip->attr & VIEW_IN) {
          TransformLibraryObject(objectTable[section->objects[i]], &glo.camPos, playerShip->roll);
        } else {
          TransformLibraryObject(objectTable[section->objects[i]], &glo.camPos, 0);
        }

#endif
      }
    }
  }

  count += 1;
}
