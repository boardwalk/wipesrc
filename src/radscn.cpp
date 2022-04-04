#include "standard.h"
#include <stdlib.h>
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
#include "scene.h"
#include "radscn.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "error.h"
#include "libgte.h"

/* reduce this later !!! */
#define ObjectListMax 128

void CalcNewRadius(TrackSection* section, Object* objects) {
  TrackSection* visSection;
  Object* object;
  MATRIX* matrix;
  Vector* sectionCentre;
  VECTOR vector;
  int32_t distance; // PCwipeout
  int16_t j;
  int16_t objectCount;
  int16_t withinLocal;
  int16_t withinGlobal;
  int16_t allCount;
  int16_t objectList[ObjectListMax];

  DFree(heap, (Ptr)(int16_t*)section->objects);
  section->objectCount = 0;

  object = objects;
  objectCount = 0;
  allCount = 0;
  while (object) {
    withinLocal = 0;
    withinGlobal = 0;

    matrix = &object->skeleton->relative;
    sectionCentre = &section->centre;

    vector.vx = matrix->t[0] - sectionCentre->vx;
    vector.vy = matrix->t[1] - sectionCentre->vy;
    vector.vz = matrix->t[2] - sectionCentre->vz;

    if (section->flags & Jump) {
      if ((abs(vector.vx) < section->globalRadius * 2) &&
          (abs(vector.vy) < section->globalRadius * 2) &&
          (abs(vector.vz) < section->globalRadius * 2)) {
        distance = SquareRoot0((vector.vx * vector.vx) +
                               (vector.vy * vector.vy) +
                               (vector.vz * vector.vz));

        distance = distance - object->extent;

        if (distance < section->globalRadius) {
          /* within the global radius */
          withinGlobal = 1;
        }
      }
    } else {
      if ((abs(vector.vx) < section->globalRadius) &&
          (abs(vector.vy) < section->globalRadius) &&
          (abs(vector.vz) < section->globalRadius)) {
        distance = SquareRoot0((vector.vx * vector.vx) +
                               (vector.vy * vector.vy) +
                               (vector.vz * vector.vz));

        distance = distance - object->extent;

        if (distance < section->globalRadius) {
          /* within the global radius */
          withinGlobal = 1;
        }
      }
    }

    if (withinGlobal) {
      /* Check distance forwards along the track */

      visSection = section;
      for (j = 0; (!withinLocal) && (j < 30); j++) {
        matrix = &object->skeleton->relative;
        sectionCentre = &visSection->centre;

        vector.vx = matrix->t[0] - sectionCentre->vx;
        vector.vy = matrix->t[1] - sectionCentre->vy;
        vector.vz = matrix->t[2] - sectionCentre->vz;

        if ((abs(vector.vx) < section->localRadius) &&
            (abs(vector.vy) < section->localRadius) &&
            (abs(vector.vz) < section->localRadius)) {
          distance = SquareRoot0((vector.vx * vector.vx) +
                                 (vector.vy * vector.vy) +
                                 (vector.vz * vector.vz));

          distance = distance - object->extent;

          if (distance < section->localRadius) {
            /* within the local radius */
            withinLocal = 1;
          }
        }
        visSection = visSection->nextSection.ptr;
      }

      /* Check distance backwards along the track */

      visSection = section;
      for (j = 0; (!withinLocal) && (j < 30); j++) {
        matrix = &object->skeleton->relative;
        sectionCentre = &visSection->centre;

        vector.vx = matrix->t[0] - sectionCentre->vx;
        vector.vy = matrix->t[1] - sectionCentre->vy;
        vector.vz = matrix->t[2] - sectionCentre->vz;

        if ((abs(vector.vx) < section->localRadius) &&
            (abs(vector.vy) < section->localRadius) &&
            (abs(vector.vz) < section->localRadius)) {
          distance = SquareRoot0((vector.vx * vector.vx) +
                                 (vector.vy * vector.vy) +
                                 (vector.vz * vector.vz));

          distance = distance - object->extent;

          if (distance < section->localRadius) {
            /* within the local radius */
            withinLocal = 1;
          }
        }
        visSection = visSection->prevSection.ptr;
      }
    }

    if (withinLocal) {
      if (objectCount < ObjectListMax) {
        objectList[objectCount++] = allCount;
      } else {
        Error("Radius.c:SceneRadiusCheck: Maximum number of objects exceeded", Fatal);
      }
    }

    object = object->next;
    allCount++;
  }

  section->objects = (int16_t*)DAlloc(heap, objectCount * sizeof(int16_t));
  if (!section->objects) {
    Error("Error:: radius.c: Not enough memory for section object list", Fatal);
  }
  section->objectCount = objectCount;

  for (j = 0; j < objectCount; j++) {
    section->objects[j] = objectList[j];
  }
}

void SceneRadiusCheck(Object* objects) {
  TrackSection* section;
  TrackSection* visSection;
  int16_t i, j;
  Object* object;
  MATRIX* matrix;
  Vector* sectionCentre;
  int16_t objectCount;
  VECTOR vector;
  //   int32_t int32_t      distance;
  int32_t distance; // PCwipeout
  int16_t objectList[ObjectListMax];
  int16_t withinLocal;
  int16_t withinGlobal;
  int16_t allCount;

  /* allocate and fill in a global object table */

  object = objects;
  objectCount = 0;
  while (object) {
    object = object->next;
    objectCount++;
  }
#if 0
   printf( "num of objects = %d\n", objectCount );
#endif

  objectTable = (Object**)DAlloc(heap, (objectCount + 1) * sizeof(Object*));
  if (!objectTable) {
    Error("radscn.c::SceneRadiusCheck(): No memory for global object table", Fatal);
  }

  object = objects;
  for (i = 0; i < objectCount; i++) {
    objectTable[i] = object;
    object = object->next;
  }

  /* do the radius check */

  section = track->sections;

  for (i = 0; i < track->sectionCount; i++) {
#if 0
      printf( "," );
#endif
    object = objects;
    objectCount = 0;
    allCount = 0;
    while (object) {
      withinLocal = 0;
      withinGlobal = 0;

      matrix = &object->skeleton->relative;
      sectionCentre = &section->centre;

      vector.vx = matrix->t[0] - sectionCentre->vx;
      vector.vy = matrix->t[1] - sectionCentre->vy;
      vector.vz = matrix->t[2] - sectionCentre->vz;

      if (section->flags & Jump) {
        if ((abs(vector.vx) < (section->globalRadius * 2)) &&
            (abs(vector.vy) < (section->globalRadius * 2)) &&
            (abs(vector.vz) < (section->globalRadius * 2))) {
          distance = SquareRoot0((vector.vx * vector.vx) +
                                 (vector.vy * vector.vy) +
                                 (vector.vz * vector.vz));

          distance = distance - object->extent;

          if (distance < section->globalRadius) {
            /* within the global radius */
            withinGlobal = 1;
          }
        }
      } else {
        if ((abs(vector.vx) < section->globalRadius) &&
            (abs(vector.vy) < section->globalRadius) &&
            (abs(vector.vz) < section->globalRadius)) {
          distance = SquareRoot0((vector.vx * vector.vx) +
                                 (vector.vy * vector.vy) +
                                 (vector.vz * vector.vz));

          distance = distance - object->extent;

          if (distance < section->globalRadius) {
            /* within the global radius */
            withinGlobal = 1;
          }
        }
      }

      if (withinGlobal) {
        /* Check distance forwards along the track */

        visSection = section;
        for (j = 0; (!withinLocal) && (j < 30); j++) {
          matrix = &object->skeleton->relative;
          sectionCentre = &visSection->centre;

          vector.vx = matrix->t[0] - sectionCentre->vx;
          vector.vy = matrix->t[1] - sectionCentre->vy;
          vector.vz = matrix->t[2] - sectionCentre->vz;

          if ((abs(vector.vx) < section->localRadius) &&
              (abs(vector.vy) < section->localRadius) &&
              (abs(vector.vz) < section->localRadius)) {
            distance = SquareRoot0((vector.vx * vector.vx) +
                                   (vector.vy * vector.vy) +
                                   (vector.vz * vector.vz));

            distance = distance - object->extent;

            if (distance < section->localRadius) {
              /* within the local radius */
              withinLocal = 1;
            }
          }
          visSection = visSection->nextSection.ptr;
        }

        /* Check distance backwards along the track */

        visSection = section;
        for (j = 0; (!withinLocal) && (j < 30); j++) {
          matrix = &object->skeleton->relative;
          sectionCentre = &visSection->centre;

          vector.vx = matrix->t[0] - sectionCentre->vx;
          vector.vy = matrix->t[1] - sectionCentre->vy;
          vector.vz = matrix->t[2] - sectionCentre->vz;

          if ((abs(vector.vx) < section->localRadius) &&
              (abs(vector.vy) < section->localRadius) &&
              (abs(vector.vz) < section->localRadius)) {
            distance = SquareRoot0((vector.vx * vector.vx) +
                                   (vector.vy * vector.vy) +
                                   (vector.vz * vector.vz));

            distance = distance - object->extent;

            if (distance < section->localRadius) {
              /* within the local radius */
              withinLocal = 1;
            }
          }
          visSection = visSection->prevSection.ptr;
        }
      }

      if (withinLocal) {
        if (objectCount < ObjectListMax) {
          objectList[objectCount++] = allCount;
        } else {
          Error("Radius.c:SceneRadiusCheck: Maximum number of objects exceeded", Fatal);
        }
      }

      object = object->next;
      allCount++;
    }

    section->objects = (int16_t*)DAlloc(heap, objectCount * sizeof(int16_t));
    if (!section->objects) {
      Error("Error:: radius.c: Not enough memory for section object list", Fatal);
    }
    section->objectCount = objectCount;

    for (j = 0; j < objectCount; j++) {
      section->objects[j] = objectList[j];
    }

#if 0
      printf( "List %d ::", i );
      for ( j=0; j<objectCount; j++ )
      {
         printf( "%6d", section->objects[ j ] );
      }
      printf( "\n" );
#endif

#if 0
      printf( "Track section %d object count %d\n", i, objectCount );
#endif
    section = NextIndexSection(section);
  }
}
