#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "combat.h"
#include "protos.h"
#include "libgte.h"
#include "ajs.h"
#include <stdio.h>

extern SVECTOR StaticVertex[];
extern DVECTOR StaticScreen[];
extern uint16_t StaticDepth[];
extern uint16_t StaticFlags[];
extern uint16_t StaticZ[];

#define ClipFlag ((CrtClipped | ZLarge | ZNegative | ZSmall) >> 12)

void DrawObjectAsm(Object* object, Skeleton* camera, int32_t roll, int16_t ShipObj) {
  // printf("DrawObjectAsm %p (%s)\n", object, object->name);
  DVECTOR* screenPtr;
  uint16_t* depthPtr;
  uint16_t* flagsPtr;

  Poly poly;

  int16_t i;

  int16_t coord0;
  int16_t coord1;
  int16_t coord2;
  int16_t coord3;

  int32_t depth;
  int32_t newdepth;

  SVECTOR* vertex;

  int16_t primitiveCount;
  int16_t vertexCount;

  /* for sprites */
  int32_t x, y;

  int16_t xSize;
  int16_t ySize;
  int16_t xSize2;
  int16_t ySize2;

  int16_t tlX;
  int16_t tlY;
  int16_t trX;
  int16_t trY;
  int16_t blX;
  int16_t blY;
  int16_t brX;
  int16_t brY;

  int16_t offsetX;
  int16_t offsetY;
  int16_t offset2X;
  int16_t offset2Y;

  int16_t tex;

  int16_t ObjDepthMod;

  if (ShipObj)
    ObjDepthMod = 0;
  else
    ObjDepthMod = 600;

  SetSkeletonTransform(object->skeleton);

  vertex = object->vertices;
  vertexCount = object->vertexCount;

  poly.prim = object->primitives;
  primitiveCount = object->primitiveCount;

  primCount += primitiveCount;

  if (vertexCount <= 128) {
    screenPtr = (DVECTOR*)(FastRam);
    depthPtr = (uint16_t*)(FastRam + 512);
    flagsPtr = (uint16_t*)(FastRam + 768);
  } else {
    screenPtr = StaticScreen;
    depthPtr = StaticDepth;
    flagsPtr = StaticFlags;
  }

  RotTransPersN(vertex, screenPtr, depthPtr, flagsPtr, vertexCount);

  for (i = 0; i < primitiveCount; i++) {
    switch (poly.prim->type) {
    case TypeGT3:

      coord0 = poly.gt3->coords[0];
      coord1 = poly.gt3->coords[1];
      coord2 = poly.gt3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT3((POLY_GT3*)prims.ptr);

          prims.gt3->cba = poly.gt3->cba;
          prims.gt3->tsb = poly.gt3->tsb;

          prims.gt3->u0 = poly.gt3->u0;
          prims.gt3->v0 = poly.gt3->v0;
          prims.gt3->u1 = poly.gt3->u1;
          prims.gt3->v1 = poly.gt3->v1;
          prims.gt3->u2 = poly.gt3->u2;
          prims.gt3->v2 = poly.gt3->v2;

          prims.gt3->colour0.r = poly.gt3->colour[0].r;
          prims.gt3->colour0.g = poly.gt3->colour[0].g;
          prims.gt3->colour0.b = poly.gt3->colour[0].b;
          prims.gt3->colour1 = poly.gt3->colour[1];
          prims.gt3->colour2 = poly.gt3->colour[2];

          prims.gt3->xy0.vx = screenPtr[coord0].vx;
          prims.gt3->xy0.vy = screenPtr[coord0].vy;
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vz = depthPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          depth += ObjDepthMod;

          if (poly.ft3->flag & ShipEngine)
            depth -= 50;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt3 += 1;
        }
      }
      poly.gt3 += 1;
      break;

    case TypeGT4:

      coord0 = poly.gt4->coords[0];
      coord1 = poly.gt4->coords[1];
      coord2 = poly.gt4->coords[2];
      coord3 = poly.gt4->coords[3];
      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT4((POLY_GT4*)prims.ptr);

          prims.gt4->cba = poly.gt4->cba;
          prims.gt4->tsb = poly.gt4->tsb;

          prims.gt4->u0 = poly.gt4->u0;
          prims.gt4->v0 = poly.gt4->v0;
          prims.gt4->u1 = poly.gt4->u1;
          prims.gt4->v1 = poly.gt4->v1;
          prims.gt4->u2 = poly.gt4->u2;
          prims.gt4->v2 = poly.gt4->v2;
          prims.gt4->u3 = poly.gt4->u3;
          prims.gt4->v3 = poly.gt4->v3;
          prims.gt4->colour0.r = poly.gt4->colour[0].r;
          prims.gt4->colour0.g = poly.gt4->colour[0].g;

          //					if(trackNo == 4 || trackNo == 5)
          //						prims.gt4->colour0.b = BLACK_NOT_DRAWN;
          //					else
          prims.gt4->colour0.b = poly.gt4->colour[0].b;
          prims.gt4->colour1 = poly.gt4->colour[1];
          prims.gt4->colour2 = poly.gt4->colour[2];
          prims.gt4->colour3 = poly.gt4->colour[3];

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vz = depthPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt4 += 1;
        }
      }
      poly.gt4 += 1;
      break;

    case TypeFT3:

      coord0 = poly.ft3->coords[0];
      coord1 = poly.ft3->coords[1];
      coord2 = poly.ft3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyFT3((POLY_FT3*)prims.ptr);

          prims.ft3->cba = poly.ft3->cba;
          prims.ft3->tsb = poly.ft3->tsb;

          prims.ft3->u0 = poly.ft3->u0;
          prims.ft3->v0 = poly.ft3->v0;
          prims.ft3->u1 = poly.ft3->u1;
          prims.ft3->v1 = poly.ft3->v1;
          prims.ft3->u2 = poly.ft3->u2;
          prims.ft3->v2 = poly.ft3->v2;

          prims.ft3->colour0.r = poly.ft3->colour.r;
          prims.ft3->colour0.g = poly.ft3->colour.g;
          prims.ft3->colour0.b = poly.ft3->colour.b;

          //prims.ft3->xy0 = screenPtr[ coord0 ];
          //prims.ft3->xy1 = screenPtr[ coord1 ];
          //prims.ft3->xy2 = screenPtr[ coord2 ];
          prims.ft3->xy0.vx = screenPtr[coord0].vx;
          prims.ft3->xy0.vy = screenPtr[coord0].vy;
          prims.ft3->xy1.vx = screenPtr[coord1].vx;
          prims.ft3->xy1.vy = screenPtr[coord1].vy;
          prims.ft3->xy2.vx = screenPtr[coord2].vx;
          prims.ft3->xy2.vy = screenPtr[coord2].vy;
          prims.ft3->xy0.vz = depthPtr[coord0];
          prims.ft3->xy1.vz = depthPtr[coord1];
          prims.ft3->xy2.vz = depthPtr[coord2];
          prims.ft3->xy0.pad = 0;
          prims.ft3->xy1.pad = 0;
          prims.ft3->xy2.pad = 0;

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.ft3 += 1;
        }
      }
      poly.ft3 += 1;
      break;

    case TypeFT4:

      coord0 = poly.ft4->coords[0];
      coord1 = poly.ft4->coords[1];
      coord2 = poly.ft4->coords[2];
      coord3 = poly.ft4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyFT4((POLY_FT4*)prims.ptr);

          prims.ft4->cba = poly.ft4->cba;
          prims.ft4->tsb = poly.ft4->tsb;

          prims.ft4->u0 = poly.ft4->u0;
          prims.ft4->v0 = poly.ft4->v0;
          prims.ft4->u1 = poly.ft4->u1;
          prims.ft4->v1 = poly.ft4->v1;
          prims.ft4->u2 = poly.ft4->u2;
          prims.ft4->v2 = poly.ft4->v2;
          prims.ft4->u3 = poly.ft4->u3;
          prims.ft4->v3 = poly.ft4->v3;

          prims.ft4->colour0.r = poly.ft4->colour.r;
          prims.ft4->colour0.g = poly.ft4->colour.g;
          prims.ft4->colour0.b = poly.ft4->colour.b;

          //prims.ft4->xy0 = screenPtr[ coord0 ];
          //prims.ft4->xy1 = screenPtr[ coord1 ];
          //prims.ft4->xy2 = screenPtr[ coord2 ];
          //prims.ft4->xy3 = screenPtr[ coord3 ];
          prims.ft4->xy0.vx = screenPtr[coord0].vx;
          prims.ft4->xy0.vy = screenPtr[coord0].vy;
          prims.ft4->xy1.vx = screenPtr[coord1].vx;
          prims.ft4->xy1.vy = screenPtr[coord1].vy;
          prims.ft4->xy2.vx = screenPtr[coord2].vx;
          prims.ft4->xy2.vy = screenPtr[coord2].vy;
          prims.ft4->xy3.vx = screenPtr[coord3].vx;
          prims.ft4->xy3.vy = screenPtr[coord3].vy;
          prims.ft4->xy0.vz = depthPtr[coord0];
          prims.ft4->xy1.vz = depthPtr[coord1];
          prims.ft4->xy2.vz = depthPtr[coord2];
          prims.ft4->xy3.vz = depthPtr[coord3];
          prims.ft4->xy0.pad = 0;
          prims.ft4->xy1.pad = 0;
          prims.ft4->xy2.pad = 0;
          prims.ft4->xy3.pad = 0;

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          depth += ObjDepthMod;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.ft4 += 1;
        }
      }
      poly.ft4 += 1;
      break;

    case TypeG3:

      coord0 = poly.g3->coords[0];
      coord1 = poly.g3->coords[1];
      coord2 = poly.g3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG3((POLY_G3*)prims.ptr);

          prims.g3->colour0.r = poly.g3->colour[0].r;
          prims.g3->colour0.g = poly.g3->colour[0].g;
          prims.g3->colour0.b = poly.g3->colour[0].b;
          prims.g3->colour1 = poly.g3->colour[1];
          prims.g3->colour2 = poly.g3->colour[2];

          prims.g3->xy0 = screenPtr[coord0];
          prims.g3->xy1 = screenPtr[coord1];
          prims.g3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          depth += ObjDepthMod;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g3 += 1;
        }
      }
      poly.g3 += 1;
      break;

    case TypeG4:

      coord0 = poly.g4->coords[0];
      coord1 = poly.g4->coords[1];
      coord2 = poly.g4->coords[2];
      coord3 = poly.g4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG4((POLY_G4*)prims.ptr);

          prims.g4->colour0.r = poly.g4->colour[0].r;
          prims.g4->colour0.g = poly.g4->colour[0].g;
          prims.g4->colour0.b = poly.g4->colour[0].b;
          prims.g4->colour1 = poly.g4->colour[1];
          prims.g4->colour2 = poly.g4->colour[2];
          prims.g4->colour3 = poly.g4->colour[3];

          prims.g4->xy0 = screenPtr[coord0];
          prims.g4->xy1 = screenPtr[coord1];
          prims.g4->xy2 = screenPtr[coord2];
          prims.g4->xy3 = screenPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          depth += ObjDepthMod;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g4 += 1;
        }
      }
      poly.g4 += 1;
      break;

    case TypeF3:

      coord0 = poly.f3->coords[0];
      coord1 = poly.f3->coords[1];
      coord2 = poly.f3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyF3((POLY_F3*)prims.ptr);

          prims.f3->colour0.r = poly.f3->colour.r;
          prims.f3->colour0.g = poly.f3->colour.g;
          prims.f3->colour0.b = poly.f3->colour.b;

          prims.f3->xy0 = screenPtr[coord0];
          prims.f3->xy1 = screenPtr[coord1];
          prims.f3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          depth += ObjDepthMod;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.f3 += 1;
        }
      }
      poly.f3 += 1;
      break;

    case TypeF4:

      coord0 = poly.f4->coords[0];
      coord1 = poly.f4->coords[1];
      coord2 = poly.f4->coords[2];
      coord3 = poly.f4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyF4((POLY_F4*)prims.ptr);

          prims.f4->colour0.r = poly.f4->colour.r;
          prims.f4->colour0.g = poly.f4->colour.g;
          prims.f4->colour0.b = poly.f4->colour.b;

          prims.f4->xy0 = screenPtr[coord0];
          prims.f4->xy1 = screenPtr[coord1];
          prims.f4->xy2 = screenPtr[coord2];
          prims.f4->xy3 = screenPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          depth += ObjDepthMod;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.f4 += 1;
        }
      }

      poly.f4 += 1;
      break;

    case TypeTSPR:

      coord0 = poly.spr->coord;

      depth = depthPtr[coord0];
      x = screenPtr[coord0].vx;
      y = screenPtr[coord0].vy;

      if (!(flagsPtr[coord0] & ClipFlag)) {
        xSize = (160 * poly.spr->width) / depth;
        ySize = (160 * poly.spr->height) / depth;

        xSize2 = xSize >> 1;
        ySize2 = ySize >> 1;

        offsetX = ((xSize2 * rcos(roll)) >> 12);
        offsetY = ((xSize2 * rsin(roll)) >> 12);

        offset2X = ((ySize * rcos(1024 - roll)) >> 12);
        offset2Y = ((ySize * rsin(1024 - roll)) >> 12);

        tlX = x - offsetX;
        tlY = y - offsetY;

        brX = tlX - offset2X;
        brY = tlY + offset2Y;

        trX = x + offsetX;
        trY = y + offsetY;

        blX = trX - offset2X;
        blY = trY + offset2Y;

        prims.ft4->xy0.vx = tlX;
        prims.ft4->xy0.vy = tlY;
        prims.ft4->xy1.vx = trX;
        prims.ft4->xy1.vy = trY;
        prims.ft4->xy2.vx = blX;
        prims.ft4->xy2.vy = blY;
        prims.ft4->xy3.vx = brX;
        prims.ft4->xy3.vy = brY;

        prims.ft4->xy0.vz = depthPtr[coord0];
        prims.ft4->xy1.vz = depthPtr[coord0];
        prims.ft4->xy2.vz = depthPtr[coord0];
        prims.ft4->xy3.vz = depthPtr[coord0];
        prims.ft4->xy0.pad = 0;
        prims.ft4->xy1.pad = 0;
        prims.ft4->xy2.pad = 0;
        prims.ft4->xy3.pad = 0;

        SetPolyFT4((POLY_FT4*)prims.ptr);

        prims.ft4->colour0.r = 230; // PCwipeout
        prims.ft4->colour0.g = 0;
        prims.ft4->colour0.b = BLACK_NOT_DRAWN;

        tex = poly.spr->texture;

        prims.ft4->u0 = TextureTable[tex]->u0;
        prims.ft4->v0 = TextureTable[tex]->v0;

        prims.ft4->u1 = TextureTable[tex]->u1;
        prims.ft4->v1 = TextureTable[tex]->v1;

        prims.ft4->u2 = TextureTable[tex]->u2;
        prims.ft4->v2 = TextureTable[tex]->v2;

        prims.ft4->u3 = TextureTable[tex]->u3;
        prims.ft4->v3 = TextureTable[tex]->v3;

        prims.ft4->tsb = TextureTable[tex]->tsb;
        prims.ft4->cba = TextureTable[tex]->cba;

        newdepth = (depth >> 2) + ObjDepthMod;

        AddPrim(OT[CurrentScreen], newdepth, (P_TAG*)prims.ptr);
        prims.ft4 += 1;
      }

      poly.spr += 1;
      break;

    case TypeBSPR:

      coord0 = poly.spr->coord;

      depth = depthPtr[coord0];
      x = screenPtr[coord0].vx;
      y = screenPtr[coord0].vy;

      if (x < -590 || x > 909) // Bodge to eliminate
        break; // BSPR messiness
      else if (y < -590 || y > 789)
        break;

      if (!(flagsPtr[coord0] & ClipFlag)) {
        xSize = (160 * poly.spr->width) / depth;
        ySize = (160 * poly.spr->height) / depth;

        xSize2 = xSize >> 1;
        ySize2 = ySize >> 1;

        offsetX = ((xSize2 * rcos(roll)) >> 12);
        offsetY = ((xSize2 * rsin(roll)) >> 12);

        offset2X = ((ySize * rcos(1024 - roll)) >> 12);
        offset2Y = ((ySize * rsin(1024 - roll)) >> 12);

        blX = x - offsetX;
        blY = y - offsetY;

        brX = x + offsetX;
        brY = y + offsetY;

        tlX = blX + offset2X;
        tlY = blY - offset2Y;

        trX = brX + offset2X;
        trY = brY - offset2Y;

        prims.ft4->xy0.vx = tlX;
        prims.ft4->xy0.vy = tlY;
        prims.ft4->xy1.vx = trX;
        prims.ft4->xy1.vy = trY;
        prims.ft4->xy2.vx = blX;
        prims.ft4->xy2.vy = blY;
        prims.ft4->xy3.vx = brX;
        prims.ft4->xy3.vy = brY;

        prims.ft4->xy0.vz = depthPtr[coord0];
        prims.ft4->xy1.vz = depthPtr[coord0];
        prims.ft4->xy2.vz = depthPtr[coord0];
        prims.ft4->xy3.vz = depthPtr[coord0];
        prims.ft4->xy0.pad = 0;
        prims.ft4->xy1.pad = 0;
        prims.ft4->xy2.pad = 0;
        prims.ft4->xy3.pad = 0;

        SetPolyFT4((POLY_FT4*)prims.ptr);

        prims.ft4->colour0.r = 230; // PCwipeout
        prims.ft4->colour0.g = 0;
        prims.ft4->colour0.b = BLACK_NOT_DRAWN;

        tex = poly.spr->texture;

        prims.ft4->u0 = TextureTable[tex]->u0;
        prims.ft4->v0 = TextureTable[tex]->v0;

        prims.ft4->u1 = TextureTable[tex]->u1;
        prims.ft4->v1 = TextureTable[tex]->v1;

        prims.ft4->u2 = TextureTable[tex]->u2;
        prims.ft4->v2 = TextureTable[tex]->v2;

        prims.ft4->u3 = TextureTable[tex]->u3;
        prims.ft4->v3 = TextureTable[tex]->v3;

        prims.ft4->tsb = TextureTable[tex]->tsb;
        prims.ft4->cba = TextureTable[tex]->cba;

        newdepth = (depth >> 2) + ObjDepthMod;

        AddPrim(OT[CurrentScreen], newdepth, (P_TAG*)prims.ptr);
        prims.ft4 += 1;
      }

      poly.spr += 1;
      break;

    default:
      break;
    }
  }
}

void DrawObjectFixedAsm(Object* object, Skeleton* camera, int32_t depth) {
  // printf("DrawObjectFixedAsm %p (%s)\n", object, object->name);
  DVECTOR* screenPtr;
  uint16_t* depthPtr;
  uint16_t* flagsPtr;

  Poly poly;

  int16_t i;

  int16_t coord0;
  int16_t coord1;
  int16_t coord2;
  int16_t coord3;

  SVECTOR* vertex;

  int16_t primitiveCount;
  int16_t vertexCount;

  SetSkeletonTransform(object->skeleton);

  vertex = object->vertices;
  vertexCount = object->vertexCount;

  poly.prim = object->primitives;
  primitiveCount = object->primitiveCount;

  primCount += primitiveCount;

  if (vertexCount <= 128) {
    screenPtr = (DVECTOR*)(FastRam);
    depthPtr = (uint16_t*)(FastRam + 512);
    flagsPtr = (uint16_t*)(FastRam + 768);
  } else {
    screenPtr = StaticScreen;
    depthPtr = StaticDepth;
    flagsPtr = StaticFlags;
  }

  RotTransPersN(vertex, screenPtr, depthPtr, flagsPtr, vertexCount);

  for (i = 0; i < primitiveCount; i++) {
    switch (poly.prim->type) {
    case TypeGT3:

      coord0 = poly.gt3->coords[0];
      coord1 = poly.gt3->coords[1];
      coord2 = poly.gt3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT3((POLY_GT3*)prims.ptr);

          prims.gt3->cba = poly.gt3->cba;
          prims.gt3->tsb = poly.gt3->tsb;

          prims.gt3->u0 = poly.gt3->u0;
          prims.gt3->v0 = poly.gt3->v0;
          prims.gt3->u1 = poly.gt3->u1;
          prims.gt3->v1 = poly.gt3->v1;
          prims.gt3->u2 = poly.gt3->u2;
          prims.gt3->v2 = poly.gt3->v2;

          prims.gt3->colour0.r = poly.gt3->colour[0].r;
          prims.gt3->colour0.g = poly.gt3->colour[0].g;
          prims.gt3->colour0.b = poly.gt3->colour[0].b;
          prims.gt3->colour1 = poly.gt3->colour[1];
          prims.gt3->colour2 = poly.gt3->colour[2];

          prims.gt3->xy0.vx = screenPtr[coord0].vx;
          prims.gt3->xy0.vy = screenPtr[coord0].vy;
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vz = depthPtr[coord2];

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt3 += 1;
        }
      }
      poly.gt3 += 1;
      break;

    case TypeGT4:

      coord0 = poly.gt4->coords[0];
      coord1 = poly.gt4->coords[1];
      coord2 = poly.gt4->coords[2];
      coord3 = poly.gt4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT4((POLY_GT4*)prims.ptr);

          prims.gt4->cba = poly.gt4->cba;
          prims.gt4->tsb = poly.gt4->tsb;

          prims.gt4->u0 = poly.gt4->u0;
          prims.gt4->v0 = poly.gt4->v0;
          prims.gt4->u1 = poly.gt4->u1;
          prims.gt4->v1 = poly.gt4->v1;
          prims.gt4->u2 = poly.gt4->u2;
          prims.gt4->v2 = poly.gt4->v2;
          prims.gt4->u3 = poly.gt4->u3;
          prims.gt4->v3 = poly.gt4->v3;

          prims.gt4->colour0.r = poly.gt4->colour[0].r;
          prims.gt4->colour0.g = poly.gt4->colour[0].g;

          //					if(trackNo == 4 || trackNo == 5)
          //						prims.gt4->colour0.b = BLACK_NOT_DRAWN;
          //					else
          prims.gt4->colour0.b = poly.gt4->colour[0].b;
          prims.gt4->colour1 = poly.gt4->colour[1];
          prims.gt4->colour2 = poly.gt4->colour[2];
          prims.gt4->colour3 = poly.gt4->colour[3];

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vz = depthPtr[coord3];

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt4 += 1;
        }
      }
      poly.gt4 += 1;
      break;

    case TypeFT3:

      coord0 = poly.ft3->coords[0];
      coord1 = poly.ft3->coords[1];
      coord2 = poly.ft3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyFT3((POLY_FT3*)prims.ptr);

          prims.ft3->cba = poly.ft3->cba;
          prims.ft3->tsb = poly.ft3->tsb;

          prims.ft3->u0 = poly.ft3->u0;
          prims.ft3->v0 = poly.ft3->v0;
          prims.ft3->u1 = poly.ft3->u1;
          prims.ft3->v1 = poly.ft3->v1;
          prims.ft3->u2 = poly.ft3->u2;
          prims.ft3->v2 = poly.ft3->v2;

          prims.ft3->colour0.r = 230; // PCwipeout
          prims.ft3->colour0.g = 0;
          prims.ft3->colour0.b = 0;

          //prims.ft3->xy0 = screenPtr[ coord0 ];
          //prims.ft3->xy1 = screenPtr[ coord1 ];
          //prims.ft3->xy2 = screenPtr[ coord2 ];
          prims.ft3->xy0.vx = screenPtr[coord0].vx;
          prims.ft3->xy0.vy = screenPtr[coord0].vy;
          prims.ft3->xy1.vx = screenPtr[coord1].vx;
          prims.ft3->xy1.vy = screenPtr[coord1].vy;
          prims.ft3->xy2.vx = screenPtr[coord2].vx;
          prims.ft3->xy2.vy = screenPtr[coord2].vy;
          prims.ft3->xy0.vz = depthPtr[coord0];
          prims.ft3->xy1.vz = depthPtr[coord1];
          prims.ft3->xy2.vz = depthPtr[coord2];
          prims.ft3->xy0.pad = 0;
          prims.ft3->xy1.pad = 0;
          prims.ft3->xy2.pad = 0;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.ft3 += 1;
        }
      }

      poly.ft3 += 1;
      break;

    case TypeFT4:

      coord0 = poly.ft4->coords[0];
      coord1 = poly.ft4->coords[1];
      coord2 = poly.ft4->coords[2];
      coord3 = poly.ft4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyFT4((POLY_FT4*)prims.ptr);

          prims.ft4->cba = poly.ft4->cba;
          prims.ft4->tsb = poly.ft4->tsb;

          prims.ft4->u0 = poly.ft4->u0;
          prims.ft4->v0 = poly.ft4->v0;
          prims.ft4->u1 = poly.ft4->u1;
          prims.ft4->v1 = poly.ft4->v1;
          prims.ft4->u2 = poly.ft4->u2;
          prims.ft4->v2 = poly.ft4->v2;
          prims.ft4->u3 = poly.ft4->u3;
          prims.ft4->v3 = poly.ft4->v3;

          prims.ft4->colour0.r = 230; // PCwipeout
          prims.ft4->colour0.g = 0;
          prims.ft4->colour0.b = 0;

          //prims.ft4->xy0 = screenPtr[ coord0 ];
          //prims.ft4->xy1 = screenPtr[ coord1 ];
          //prims.ft4->xy2 = screenPtr[ coord2 ];
          //prims.ft4->xy3 = screenPtr[ coord3 ];
          prims.ft4->xy0.vx = screenPtr[coord0].vx;
          prims.ft4->xy0.vy = screenPtr[coord0].vy;
          prims.ft4->xy1.vx = screenPtr[coord1].vx;
          prims.ft4->xy1.vy = screenPtr[coord1].vy;
          prims.ft4->xy2.vx = screenPtr[coord2].vx;
          prims.ft4->xy2.vy = screenPtr[coord2].vy;
          prims.ft4->xy3.vx = screenPtr[coord3].vx;
          prims.ft4->xy3.vy = screenPtr[coord3].vy;
          prims.ft4->xy0.vz = depthPtr[coord0];
          prims.ft4->xy1.vz = depthPtr[coord1];
          prims.ft4->xy2.vz = depthPtr[coord2];
          prims.ft4->xy3.vz = depthPtr[coord3];
          prims.ft4->xy0.pad = 0;
          prims.ft4->xy1.pad = 0;
          prims.ft4->xy2.pad = 0;
          prims.ft4->xy3.pad = 0;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.ft4 += 1;
        }
      }
      poly.ft4 += 1;
      break;

    case TypeG3:

      coord0 = poly.g3->coords[0];
      coord1 = poly.g3->coords[1];
      coord2 = poly.g3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG3((POLY_G3*)prims.ptr);

          prims.g3->colour0.r = 230; // PCwipeout
          prims.g3->colour0.g = 0;
          prims.g3->colour0.b = 0;
          prims.g3->colour1 = poly.g3->colour[1];
          prims.g3->colour2 = poly.g3->colour[2];

          prims.g3->xy0 = screenPtr[coord0];
          prims.g3->xy1 = screenPtr[coord1];
          prims.g3->xy2 = screenPtr[coord2];

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g3 += 1;
        }
      }
      poly.g3 += 1;
      break;

    case TypeG4:

      coord0 = poly.g4->coords[0];
      coord1 = poly.g4->coords[1];
      coord2 = poly.g4->coords[2];
      coord3 = poly.g4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG4((POLY_G4*)prims.ptr);

          prims.g4->colour0.r = 230; // PCwipeout
          prims.g4->colour0.g = 0;
          prims.g4->colour0.b = 0;
          prims.g4->colour1 = poly.g4->colour[1];
          prims.g4->colour2 = poly.g4->colour[2];
          prims.g4->colour3 = poly.g4->colour[3];

          prims.g4->xy0 = screenPtr[coord0];
          prims.g4->xy1 = screenPtr[coord1];
          prims.g4->xy2 = screenPtr[coord2];
          prims.g4->xy3 = screenPtr[coord3];

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g4 += 1;
        }
      }
      poly.g4 += 1;
      break;

    case TypeF3:

      coord0 = poly.f3->coords[0];
      coord1 = poly.f3->coords[1];
      coord2 = poly.f3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyF3((POLY_F3*)prims.ptr);

          prims.f3->colour0.r = 230; // PCwipeout
          prims.f3->colour0.g = 0;
          prims.f3->colour0.b = 0;

          prims.f3->xy0 = screenPtr[coord0];
          prims.f3->xy1 = screenPtr[coord1];
          prims.f3->xy2 = screenPtr[coord2];

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.f3 += 1;
        }
      }
      poly.f3 += 1;
      break;

    case TypeF4:

      coord0 = poly.f4->coords[0];
      coord1 = poly.f4->coords[1];
      coord2 = poly.f4->coords[2];
      coord3 = poly.f4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3])))
        ; // & ClipFlag ) )
      {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyF4((POLY_F4*)prims.ptr);

          prims.f4->colour0.r = 230; // PCwipeout
          prims.f4->colour0.g = 0;
          prims.f4->colour0.b = 0;

          prims.f4->xy0 = screenPtr[coord0];
          prims.f4->xy1 = screenPtr[coord1];
          prims.f4->xy2 = screenPtr[coord2];
          prims.f4->xy3 = screenPtr[coord3];

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.f4 += 1;
        }
      }

      poly.f4 += 1;
      break;

    default:
      break;
    }
  }
}
