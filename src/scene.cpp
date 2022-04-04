/* (C) Psygnosis 1994, 1995. By Jason Carl Denton & Rob Smith
*/

#include "standard.h"
#include <stdio.h>
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "scene.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "error.h"
#include "file.h"
#include "pallette.h"

void IntelPrim(Prm prm) {
  IntelShort((int16_t*)&prm.f3->type);
  IntelShort((int16_t*)&prm.f3->flag);

  switch (prm.f3->type) {
  case TypeF3:
    IntelShort(&prm.f3->coords[0]);
    IntelShort(&prm.f3->coords[1]);
    IntelShort(&prm.f3->coords[2]);
    break;

  case TypeF4:
    IntelShort(&prm.f4->coords[0]);
    IntelShort(&prm.f4->coords[1]);
    IntelShort(&prm.f4->coords[2]);
    IntelShort(&prm.f4->coords[3]);
    break;

  case TypeFT3:
    IntelShort(&prm.ft3->coords[0]);
    IntelShort(&prm.ft3->coords[1]);
    IntelShort(&prm.ft3->coords[2]);

    IntelShort(&prm.ft3->texture);
    break;

  case TypeFT4:
    IntelShort(&prm.ft4->coords[0]);
    IntelShort(&prm.ft4->coords[1]);
    IntelShort(&prm.ft4->coords[2]);
    IntelShort(&prm.ft4->coords[3]);

    IntelShort(&prm.ft4->texture);
    break;

  case TypeG3:
    IntelShort(&prm.g3->coords[0]);
    IntelShort(&prm.g3->coords[1]);
    IntelShort(&prm.g3->coords[2]);
    break;

  case TypeG4:
    IntelShort(&prm.g4->coords[0]);
    IntelShort(&prm.g4->coords[1]);
    IntelShort(&prm.g4->coords[2]);
    IntelShort(&prm.g4->coords[3]);
    break;

  case TypeGT3:
    IntelShort(&prm.gt3->coords[0]);
    IntelShort(&prm.gt3->coords[1]);
    IntelShort(&prm.gt3->coords[2]);

    IntelShort(&prm.gt3->texture);
    break;

  case TypeGT4:
    IntelShort(&prm.gt4->coords[0]);
    IntelShort(&prm.gt4->coords[1]);
    IntelShort(&prm.gt4->coords[2]);
    IntelShort(&prm.gt4->coords[3]);

    IntelShort(&prm.gt4->texture);
    break;

  case TypeLSF3:
    IntelShort(&prm.lsf3->coords[0]);
    IntelShort(&prm.lsf3->coords[1]);
    IntelShort(&prm.lsf3->coords[2]);

    IntelShort(&prm.lsf3->normal);
    break;

  case TypeLSF4:
    IntelShort(&prm.lsf4->coords[0]);
    IntelShort(&prm.lsf4->coords[1]);
    IntelShort(&prm.lsf4->coords[2]);
    IntelShort(&prm.lsf4->coords[3]);

    IntelShort(&prm.lsf4->normal);
    break;

  case TypeLSFT3:
    IntelShort(&prm.lsft3->coords[0]);
    IntelShort(&prm.lsft3->coords[1]);
    IntelShort(&prm.lsft3->coords[2]);

    IntelShort(&prm.lsft3->normal);

    IntelShort(&prm.lsft3->texture);
    break;

  case TypeLSFT4:
    IntelShort(&prm.lsft4->coords[0]);
    IntelShort(&prm.lsft4->coords[1]);
    IntelShort(&prm.lsft4->coords[2]);
    IntelShort(&prm.lsft4->coords[3]);

    IntelShort(&prm.lsft4->normal);

    IntelShort(&prm.lsft4->texture);
    break;

  case TypeLSG3:
    IntelShort(&prm.lsg3->coords[0]);
    IntelShort(&prm.lsg3->coords[1]);
    IntelShort(&prm.lsg3->coords[2]);

    IntelShort(&prm.lsg3->normals[0]);
    IntelShort(&prm.lsg3->normals[1]);
    IntelShort(&prm.lsg3->normals[2]);
    break;

  case TypeLSG4:
    IntelShort(&prm.lsg4->coords[0]);
    IntelShort(&prm.lsg4->coords[1]);
    IntelShort(&prm.lsg4->coords[2]);
    IntelShort(&prm.lsg4->coords[3]);

    IntelShort(&prm.lsg4->normals[0]);
    IntelShort(&prm.lsg4->normals[1]);
    IntelShort(&prm.lsg4->normals[2]);
    IntelShort(&prm.lsg4->normals[3]);
    break;

  case TypeLSGT3:
    IntelShort(&prm.lsgt3->coords[0]);
    IntelShort(&prm.lsgt3->coords[1]);
    IntelShort(&prm.lsgt3->coords[2]);

    IntelShort(&prm.lsgt3->normals[0]);
    IntelShort(&prm.lsgt3->normals[1]);
    IntelShort(&prm.lsgt3->normals[2]);

    IntelShort(&prm.lsgt3->texture);
    break;

  case TypeLSGT4:
    IntelShort(&prm.lsgt4->coords[0]);
    IntelShort(&prm.lsgt4->coords[1]);
    IntelShort(&prm.lsgt4->coords[2]);
    IntelShort(&prm.lsgt4->coords[3]);

    IntelShort(&prm.lsgt4->normals[0]);
    IntelShort(&prm.lsgt4->normals[1]);
    IntelShort(&prm.lsgt4->normals[2]);
    IntelShort(&prm.lsgt4->normals[3]);

    IntelShort(&prm.lsgt4->texture);
    break;

  case TypeTSPR:
  case TypeBSPR:
    IntelShort(&prm.spr->coord);
    IntelShort(&prm.spr->width);
    IntelShort(&prm.spr->height);
    IntelShort(&prm.spr->texture);
    break;

  case TypeSpline:
    IntelLong(&prm.spline->control1.vx);
    IntelLong(&prm.spline->control1.vy);
    IntelLong(&prm.spline->control1.vz);
    IntelLong(&prm.spline->position.vx);
    IntelLong(&prm.spline->position.vy);
    IntelLong(&prm.spline->position.vz);
    IntelLong(&prm.spline->control2.vx);
    IntelLong(&prm.spline->control2.vy);
    IntelLong(&prm.spline->control2.vz);
    break;

  case TypePointLight:
    IntelLong(&prm.pointLight->position.vx);
    IntelLong(&prm.pointLight->position.vy);
    IntelLong(&prm.pointLight->position.vz);
    IntelShort(&prm.pointLight->startFalloff);
    IntelShort(&prm.pointLight->endFalloff);
    break;

  case TypeSpotLight:
    IntelLong(&prm.spotLight->position.vx);
    IntelLong(&prm.spotLight->position.vy);
    IntelLong(&prm.spotLight->position.vz);
    IntelShort(&prm.spotLight->direction.vx);
    IntelShort(&prm.spotLight->direction.vy);
    IntelShort(&prm.spotLight->direction.vz);
    IntelShort(&prm.spotLight->startFalloff);
    IntelShort(&prm.spotLight->endFalloff);
    IntelShort(&prm.spotLight->coneAngle);
    IntelShort(&prm.spotLight->spreadAngle);
    break;

  case TypeInfiniteLight:
    IntelShort(&prm.infiniteLight->direction.vx);
    IntelShort(&prm.infiniteLight->direction.vy);
    IntelShort(&prm.infiniteLight->direction.vz);
    break;

  default:
    sprintf(errorString, "Scene:IntelPrim: Bad primitive type $%x \n", prm.f3->type);
    Error(errorString, Fatal);
    break;
  }
}

void IntelLong(int32_t* lvalue) {
  char* cvalue;
  char temp;

  cvalue = (char*)lvalue;

  temp = cvalue[0];
  cvalue[0] = cvalue[3];
  cvalue[3] = temp;

  temp = cvalue[1];
  cvalue[1] = cvalue[2];
  cvalue[2] = temp;
}

void IntelShort(int16_t* svalue) {
  char* cvalue;
  char temp;

  cvalue = (char*)svalue;

  temp = cvalue[0];
  cvalue[0] = cvalue[1];
  cvalue[1] = temp;
}

Object* LoadPrm(const char* name, int16_t textureStart) {
  Object* object;
  Object* lastObject;
  Object* firstObject;

  Prm prm;

  int32_t length;

  int16_t i, j;

  char* start;
  char* finish;

  int16_t uOffset;
  int16_t vOffset;

  Texture* texture;

  //   printf( "Loading prm file %s\n", name);

#if LoadMessages
#endif

  length = FileLength(name);

  if (length <= 0) {
    sprintf(errorString, "scene.c:LoadPrm: File %s not found\n", name);
    Error(errorString, Fatal);
  }

  start = DAlloc(heap, length + 32);
  finish = start + length;

  if (!start) {
    /* Memory Allocation Failed! */

    sprintf(errorString, "scene.c:LoadPrm(): Failed to allocate memory for file %s\n", name);
    Error(errorString, Fatal);
  }

  if (LoadFile(name, start) != length) {
    /* File Error! */

    sprintf(errorString, "scene.c:LoadPrm(): Failed to load file %s\n", name);
    Error(errorString, Fatal);
  }

  prm.ptr = start;
  firstObject = prm.object;
  lastObject = NULL;

  while (prm.ptr < finish) {
#if 0
      pollhost( );
#endif
    /* Object */

    object = prm.object++;

    IntelShort(&object->vertexCount);
    IntelShort(&object->normalCount);
    IntelShort(&object->primitiveCount);
    IntelLong(&object->extent);
    IntelShort(&object->flags);

    /* Skeleton */

    object->skeleton = prm.skeleton++;

    for (i = 0; i < 3; i++) {
      for (j = 0; j < 3; j++) {
        IntelShort(&object->skeleton->relative.m[i][j]);
      }

      IntelLong(&object->skeleton->relative.t[i]);
      IntelLong(&object->skeleton->absolute.t[i]);
    }

    IntelShort(&object->skeleton->update);

    /* BSP */

    object->bspTree = NULL;

    /* Next */

    object->next = NULL;

    /* Vertices */

    object->vertices = prm.svector;
    prm.svector += object->vertexCount;

    for (i = 0; i < object->vertexCount; i++) {
      IntelShort(&object->vertices[i].vx);
      IntelShort(&object->vertices[i].vy);
      IntelShort(&object->vertices[i].vz);
    }

    /* Normals */

    object->normals = prm.svector;
    prm.svector += object->normalCount;

    for (i = 0; i < object->normalCount; i++) {
      IntelShort(&object->normals[i].vx);
      IntelShort(&object->normals[i].vy);
      IntelShort(&object->normals[i].vz);
    }

    /* Primitives */

    object->primitives = prm.primitive;

    for (i = 0; i < object->primitiveCount; i++) {
      IntelPrim(prm);

      switch (prm.f3->type) {
      case TypeF3:
        prm.f3->colour.r = FindNearestColourIndex(&(prm.f3->colour));
        prm.f3->colour.g = 0;
        prm.f3->colour.b = 0;
        prm.f3 += 1;
        break;

      case TypeF4:
        prm.f4->colour.r = FindNearestColourIndex(&(prm.f4->colour));
        prm.f4->colour.g = 0;
        prm.f4->colour.b = 0;
        prm.f4 += 1;
        break;

      case TypeFT3:
        prm.ft3->texture += textureStart;
        texture = TextureTable[prm.ft3->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        prm.ft3->colour.r = 255;
        prm.ft3->colour.g = 0;
        prm.ft3->colour.b = 0;
        prm.ft3->cba = texture->cba;
        prm.ft3->tsb = texture->tsb;
        prm.ft3->u0 += uOffset;
        prm.ft3->v0 += vOffset;
        prm.ft3->u1 += uOffset;
        prm.ft3->v1 += vOffset;
        prm.ft3->u2 += uOffset;
        prm.ft3->v2 += vOffset;

        prm.ft3 += 1;
        break;

      case TypeFT4:
        prm.ft4->texture += textureStart;
        texture = TextureTable[prm.ft4->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        prm.ft4->colour.r = 255;
        prm.ft4->colour.g = 0;
        prm.ft4->colour.b = 0;
        prm.ft4->cba = texture->cba;
        prm.ft4->tsb = texture->tsb;
        prm.ft4->u0 += uOffset;
        prm.ft4->v0 += vOffset;
        prm.ft4->u1 += uOffset;
        prm.ft4->v1 += vOffset;
        prm.ft4->u2 += uOffset;
        prm.ft4->v2 += vOffset;
        prm.ft4->u3 += uOffset;
        prm.ft4->v3 += vOffset;

        prm.ft4 += 1;
        break;

      case TypeG3:
        prm.g3->colour[0].r = FindNearestColourIndex(&(prm.g3->colour[0]));
        prm.g3->colour[0].g = 0;
        prm.g3->colour[0].b = 0;
        prm.g3 += 1;
        break;

      case TypeG4:
        prm.g4->colour[0].r = FindNearestColourIndex(&(prm.g4->colour[0]));
        prm.g4->colour[0].g = 0;
        prm.g4->colour[0].b = 0;
        prm.g4 += 1;
        break;

      case TypeGT3:
        prm.gt3->texture += textureStart;
        texture = TextureTable[prm.gt3->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        //			   prm.gt3->colour[0].r = 255;
        //			   prm.gt3->colour[0].g = 0;
        //			   prm.gt3->colour[0].b = 0;
        prm.gt3->cba = texture->cba;
        prm.gt3->tsb = texture->tsb;
        prm.gt3->u0 += uOffset;
        prm.gt3->v0 += vOffset;
        prm.gt3->u1 += uOffset;
        prm.gt3->v1 += vOffset;
        prm.gt3->u2 += uOffset;
        prm.gt3->v2 += vOffset;

        prm.gt3 += 1;
        break;

      case TypeGT4:
        prm.gt4->texture += textureStart;
        texture = TextureTable[prm.gt4->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        prm.gt4->cba = texture->cba;
        prm.gt4->tsb = texture->tsb;
        prm.gt4->u0 += uOffset;
        prm.gt4->v0 += vOffset;
        prm.gt4->u1 += uOffset;
        prm.gt4->v1 += vOffset;
        prm.gt4->u2 += uOffset;
        prm.gt4->v2 += vOffset;
        prm.gt4->u3 += uOffset;
        prm.gt4->v3 += vOffset;

        prm.gt4 += 1;
        break;

      case TypeLSF3:
        prm.lsf3 += 1;
        break;

      case TypeLSF4:
        prm.lsf4 += 1;
        break;

      case TypeLSFT3:
        prm.lsft3->texture += textureStart;
        texture = TextureTable[prm.lsft3->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        prm.lsft3->cba = texture->cba;
        prm.lsft3->tsb = texture->tsb;
        prm.lsft3->u0 += uOffset;
        prm.lsft3->v0 += vOffset;
        prm.lsft3->u1 += uOffset;
        prm.lsft3->v1 += vOffset;
        prm.lsft3->u2 += uOffset;
        prm.lsft3->v2 += vOffset;

        prm.lsft3 += 1;
        break;

      case TypeLSFT4:
        prm.lsft4->texture += textureStart;
        texture = TextureTable[prm.lsft4->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        prm.lsft4->cba = texture->cba;
        prm.lsft4->tsb = texture->tsb;
        prm.lsft4->u0 += uOffset;
        prm.lsft4->v0 += vOffset;
        prm.lsft4->u1 += uOffset;
        prm.lsft4->v1 += vOffset;
        prm.lsft4->u2 += uOffset;
        prm.lsft4->v2 += vOffset;
        prm.lsft4->u3 += uOffset;
        prm.lsft4->v3 += vOffset;

        prm.lsft4 += 1;
        break;

      case TypeLSG3:
        prm.lsg3 += 1;
        break;

      case TypeLSG4:
        prm.lsg4 += 1;
        break;

      case TypeLSGT3:
        prm.lsgt3->texture += textureStart;
        texture = TextureTable[prm.lsgt3->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        prm.lsgt3->cba = texture->cba;
        prm.lsgt3->tsb = texture->tsb;
        prm.lsgt3->u0 += uOffset;
        prm.lsgt3->v0 += vOffset;
        prm.lsgt3->u1 += uOffset;
        prm.lsgt3->v1 += vOffset;
        prm.lsgt3->u2 += uOffset;
        prm.lsgt3->v2 += vOffset;

        prm.lsgt3 += 1;
        break;

      case TypeLSGT4:
        prm.lsgt4->texture += textureStart;
        texture = TextureTable[prm.lsgt4->texture];

        uOffset = texture->u0;
        vOffset = texture->v0;

        prm.lsgt4->cba = texture->cba;
        prm.lsgt4->tsb = texture->tsb;
        prm.lsgt4->u0 += uOffset;
        prm.lsgt4->v0 += vOffset;
        prm.lsgt4->u1 += uOffset;
        prm.lsgt4->v1 += vOffset;
        prm.lsgt4->u2 += uOffset;
        prm.lsgt4->v2 += vOffset;
        prm.lsgt4->u3 += uOffset;
        prm.lsgt4->v3 += vOffset;

        prm.lsgt4 += 1;
        break;

      case TypeTSPR:
      case TypeBSPR:
        prm.spr->texture += textureStart;
        prm.spr += 1;
        break;

      case TypeSpline:
        prm.spline += 1;
        break;

      case TypePointLight:
        prm.pointLight += 1;
        break;

      case TypeSpotLight:
        prm.spotLight += 1;
        break;

      case TypeInfiniteLight:
        prm.infiniteLight += 1;
        break;

      default:
        sprintf(errorString, "Bad primitive type %x in $%s \n", prm.f3->type, name);
        Error(errorString, Fatal);
        break;
      }
    }

    /* Bodged check for rubbish at end of file ( prm.ptr <= finish ) */

    if (lastObject) {
      lastObject->next = object;
    }

    lastObject = object;
  }

  return (firstObject);
}
