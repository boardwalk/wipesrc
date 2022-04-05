/*
 * (C) Psygnosis 1994, 1995. By Jason Carl Denton & Rob Smith
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
#include "droid.h"
#include "camera.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "combat.h"
#include "libgte.h"
#include "win.h"
#include "drawot.h"
#include "ajs.h"
#include "protos.h"
#include "error.h"

#define GsGetVcount() 0
// PCwipeout
#define SHIELD_BODGE 50

#define FULL 0
#define HIGH 0
#define MEDIUM 1
#define LOW 2
#define SMALL 2

extern float halfxf;
extern float halfyf;
extern char pixdouble;
extern char ScreenSize;
extern char Resolution;

void StartOTag(void*);
void ContinueOTag();
void EndOTag();

DoubleBuffer Screen[2];
int16_t CurrentScreen;

DRAWENV DrawEnv0;
DRAWENV DrawEnv1;
DISPENV DispEnv0;
DISPENV DispEnv1;

VECTOR CameraPosition;
SVECTOR CameraDirection;
MATRIX Camera;

// uint8_t	  PrimitiveBuffer0[ 49152 * 2 ]; // PCwipeout
// uint8_t	  PrimitiveBuffer1[ 49152 * 2];  // PCwipeout
// uint8_t*	 PrimitiveBuffer[ 2 ];	  // PCwipeout
// StdPoly	prims;
// PCwipeout

/*
 * !!! NOTES ON 2D PRIMITIVES !!!
 *
 * The tag field of each 2d primitive has the highest order byte set to the same
 * value as the olen field in the TMD data!
 *
 * Null 2D primitives ( as in empty entries in an ordering table ) have their
 * highest order byte set to zero!
 *
 * The last link in the chain of 2D primitives has its pointer set to -1,
 * overall value being 0x00FFFFFF.
 *
 * The tag pointers are absolute values. To get the true address, clear the high
 * order byte and set the highest order bit, to generate 0x80 in the high
 * byte.
 *
 */

PolyF4* GenerateQuad() {
  /* Allocate and Initialise Primitive */

  POLY_F4* quad;

  quad = (POLY_F4*)DAlloc(heap, sizeof(PolyF4));

  if (!quad) {
    /* Memory Allocation Failed! */

    return NULL;
  }

  SetPolyF4(quad);

  setRGB0(quad, 0x80, 0x80, 0x80);

  setXY4(quad, 32, 32, 224, 32, 32, 208, 224, 208);

  return (PolyF4*)quad;
}

PolyF4* ClearScreen(
    int16_t red,
    int16_t green,
    int16_t blue) {
  /* Allocate and Initialise Primitive */

  POLY_F4* quad;

  quad = (POLY_F4*)(DAlloc(heap, sizeof(PolyF4)));

  if (!quad) {
    /* Memory Allocation Failed! */

    return NULL;
  }

  SetPolyF4(quad);

  setRGB0(quad, red, green, blue);

  setXY4(quad, 0, 0, 255, 0, 0, 239, 255, 239);

  return (PolyF4*)quad;
}

void InitSystem() {
  /* Initialise GPU, GTE and Pad. Set Debug level */

  InitGeom();

  /* Set Screen Geometry */
#if PAL
  // SetVideoMode(MODE_PAL); // PCwipeout
  SetDefDrawEnv(&DrawEnv0, 0, 256, 320, 256);
  SetDefDrawEnv(&DrawEnv1, 0, 0, 320, 256);
#else
  // SetVideoMode(MODE_NTSC); // PCwipeout
  SetDefDrawEnv(&DrawEnv0, 0, 240, 320, 240);
  SetDefDrawEnv(&DrawEnv1, 0, 0, 320, 240);
#endif

  /* Set up double buffers */

  Screen[0].disp = &DispEnv0;
  Screen[0].draw = &DrawEnv0;

  Screen[1].disp = &DispEnv1;
  Screen[1].draw = &DrawEnv1;

  InitGameDisplay();

  CurrentScreen = 0;

  PutDrawEnv(Screen[CurrentScreen].draw);
  PutDispEnv(Screen[CurrentScreen].disp);

  /* Enable Display */

  SetDispMask(1);

  /* Object tables */

  OT[0] = ot0;
  OT[1] = ot1;

  /*
	 * Primitive buffer
	 */

  PrimitiveBuffer[0] = PrimitiveBuffer0;
  PrimitiveBuffer[1] = PrimitiveBuffer1;

  prims.ptr = (char*)PrimitiveBuffer[CurrentScreen];
}

void InitMenuDisplay(int32_t geomOffset) {
#if PAL

  SetDefDispEnv(&DispEnv0, 0, 0, 320, 128);
  SetDefDispEnv(&DispEnv1, 0, 256, 320, 128);

  DispEnv0.screen.x = 2;
  DispEnv0.screen.y = 24;
  DispEnv0.screen.h = 256;

  DispEnv1.screen.x = 2;
  DispEnv1.screen.y = 24;
  DispEnv1.screen.h = 256;

  SetGeomOffset(160, geomOffset);
  SetGeomScreen(160);

#else

  SetDefDispEnv(&DispEnv0, 0, 0, 320, 180);
  SetDefDispEnv(&DispEnv1, 0, 240, 320, 180);
  SetGeomOffset(160, geomOffset);
  SetGeomScreen(160);

#endif

  PutDispEnv(Screen[CurrentScreen].disp);
}

void InitGameDisplay() {
#if PAL

  SetDefDispEnv(&DispEnv0, 0, 0, 320, 256);
  SetDefDispEnv(&DispEnv1, 0, 256, 320, 256);

  DispEnv0.screen.x = 2;
  DispEnv0.screen.y = 24;
  DispEnv0.screen.h = 256;

  DispEnv1.screen.x = 2;
  DispEnv1.screen.y = 24;
  DispEnv1.screen.h = 256;

  SetGeomOffset(160, 128);
  SetGeomScreen(160);

#else

  SetDefDispEnv(&DispEnv0, 0, 0, 320, 240);
  SetDefDispEnv(&DispEnv1, 0, 240, 320, 240);
  SetGeomOffset(160, 120);
  SetGeomScreen(160);

#endif
  PutDispEnv(Screen[CurrentScreen].disp);
}

void InitSonyDisplay() {
#if PAL

  SetDefDispEnv(&DispEnv0, 0, 0, 640, 256);
  SetDefDispEnv(&DispEnv1, 0, 256, 640, 256);

  DispEnv0.screen.x = 2;
  DispEnv0.screen.y = 24;
  DispEnv0.screen.h = 256;

  DispEnv1.screen.x = 2;
  DispEnv1.screen.y = 24;
  DispEnv1.screen.h = 256;

  SetGeomOffset(320, 128);
  SetGeomScreen(320);

#else

  SetDefDispEnv(&DispEnv0, 0, 0, 640, 256);
  SetDefDispEnv(&DispEnv1, 0, 256, 640, 256);
  SetGeomOffset(320, 128);
  SetGeomScreen(320);

#endif
  PutDispEnv(Screen[CurrentScreen].disp);
}

int32_t max = 0;

int16_t gpuWarning = 0;
int16_t cpuWarning = 0;

void Swap() {
  int32_t size;

  //	EndOTag();

  // WIN95 message handling
  ProcessMessages();

  primCount = 0;

  /* Count memory used for primitives */

  size = (intptr_t)prims.ptr - (intptr_t)PrimitiveBuffer[CurrentScreen];

  if (size > max) {
    max = size;
  }

  /* Swap display and drawing environments */

  CurrentScreen = (CurrentScreen + 1) & 1;

  PutDrawEnv(Screen[CurrentScreen].draw);
  PutDispEnv(Screen[CurrentScreen].disp);

  /* Start GPU drawing new frame */
  DrawOTag(OT[(CurrentScreen + 1) & 1] + OT_SIZE - 1);
  //	StartOTag(OT[(CurrentScreen + 1) & 1] + OT_SIZE - 1);
  //	ContinueOTag();
  /* Flush debug messages */

#if SystemFont
  FntFlush(-1);
#endif

  prims.ptr = (char*)PrimitiveBuffer[CurrentScreen];
}

void SetCameraPosition(int32_t x, int32_t y, int32_t z) {
  CameraPosition.vx = 0;
  CameraPosition.vy = 0;
  CameraPosition.vz = 0;

  /*
	 * TransMatrix( &Camera, &CameraPosition );
	 */

  Camera.t[0] = x;
  Camera.t[1] = y;
  Camera.t[2] = z;

  SetTransMatrix(&Camera);
}

void SetCameraDirection(int16_t x, int16_t y, int16_t z) {
  int16_t sx, sy, sz;
  int16_t cx, cy, cz;

  sx = rsin(x);
  sy = rsin(y);
  sz = rsin(z);

  cx = rcos(x);
  cy = rcos(y);
  cz = rcos(z);

  /*
	 * RotMatrix( &CameraDirection, &Camera );
	 */

  /* Top Row */

  Camera.m[0][0] = (cy * cz) >> 12;
  Camera.m[0][1] = (-cy * sz) >> 12;
  Camera.m[0][2] = -sy;

  /* Middle Row */

  Camera.m[1][0] = ((cx * sz) >> 12) - ((((sx * sy) >> 12) * cz) >> 12);
  Camera.m[1][1] = ((cx * cz) >> 12) + ((((sx * sy) >> 12) * sz) >> 12);
  Camera.m[1][2] = (-sx * cy) >> 12;

  /* Bottom Row */

  Camera.m[2][0] = ((sx * sz) >> 12) + ((((cx * sy) >> 12) * cz) >> 12);
  Camera.m[2][1] = ((sx * cz) >> 12) - ((((cx * sy) >> 12) * sz) >> 12);
  Camera.m[2][2] = (cx * cy) >> 12;

  SetRotMatrix(&Camera);
}

void MatrixMultiply(MATRIX* m1, MATRIX* m2, MATRIX* m3) {
  /*
	 * m3->m[0][0] = ( m2->m[0][0] * m1->m[0][0] + m2->m[0][1] *
	 * m1->m[1][0] + m2->m[0][2] * m1->m[2][0] ) >> 12; m3->m[0][1] = (
	 * m2->m[0][0] * m1->m[0][1] + m2->m[0][1] * m1->m[1][1] +
	 * m2->m[0][2] * m1->m[2][1] ) >> 12; m3->m[0][2] = ( m2->m[0][0] *
	 * m1->m[0][2] + m2->m[0][1] * m1->m[1][2] + m2->m[0][2] *
	 * m1->m[2][2] ) >> 12;
	 *
	 * m3->m[1][0] = ( m2->m[1][0] * m1->m[0][0] + m2->m[1][1] * m1->m[1][0]
	 * + m2->m[1][2] * m1->m[2][0] ) >> 12; m3->m[1][1] = ( m2->m[1][0] *
	 * m1->m[0][1] + m2->m[1][1] * m1->m[1][1] + m2->m[1][2] *
	 * m1->m[2][1] ) >> 12; m3->m[1][2] = ( m2->m[1][0] * m1->m[0][2] +
	 * m2->m[1][1] * m1->m[1][2] + m2->m[1][2] * m1->m[2][2] ) >> 12;
	 *
	 * m3->m[2][0] = ( m2->m[2][0] * m1->m[0][0] + m2->m[2][1] * m1->m[1][0]
	 * + m2->m[2][2] * m1->m[2][0] ) >> 12; m3->m[2][1] = ( m2->m[2][0] *
	 * m1->m[0][1] + m2->m[2][1] * m1->m[1][1] + m2->m[2][2] *
	 * m1->m[2][1] ) >> 12; m3->m[2][2] = ( m2->m[2][0] * m1->m[0][2] +
	 * m2->m[2][1] * m1->m[1][2] + m2->m[2][2] * m1->m[2][2] ) >> 12;
	 */

  m3->t[0] = m2->t[0] + ((m1->t[0] * m2->m[0][0] + m1->t[1] * m2->m[0][1] + m1->t[2] * m2->m[0][2]) >> 12);
  m3->t[1] = m2->t[1] + ((m1->t[0] * m2->m[1][0] + m1->t[1] * m2->m[1][1] + m1->t[2] * m2->m[1][2]) >> 12);
  m3->t[2] = m2->t[2] + ((m1->t[0] * m2->m[2][0] + m1->t[1] * m2->m[2][1] + m1->t[2] * m2->m[2][2]) >> 12);

  MulMatrix0(m1, m2, m3);
}

void InitialiseAmbientColour() {
  SetBackColor(0x00, 0x00, 0xff);
}

void InitialiseColourMatrix() {
  MATRIX cm;

  cm.m[0][0] = 4096;
  cm.m[1][0] = 0;
  cm.m[2][0] = 0;

  cm.m[0][1] = 0;
  cm.m[1][1] = 4096;
  cm.m[2][1] = 0;

  cm.m[0][2] = 0;
  cm.m[1][2] = 0;
  cm.m[2][2] = 4096;

  SetColorMatrix(&cm);
}

void InitialiseLightMatrix() {
  MATRIX lm;

  lm.m[0][0] = 4096;
  lm.m[0][1] = 0;
  lm.m[0][2] = 0;

  lm.m[1][0] = 0;
  lm.m[1][1] = 4096;
  lm.m[1][2] = 0;

  lm.m[2][0] = 0;
  lm.m[2][1] = 0;
  lm.m[2][2] = 4096;

  SetLightMatrix(&lm);
}

void SetupLights(
    Object* lights) {
  Prm prm;

  int16_t ambient = 0;
  int16_t diffuse = 0;

  int16_t i;

  CVECTOR colour;
  SVECTOR direction;

  MATRIX cm;
  MATRIX lm;

  prm.primitive = lights->primitives;

  for (i = 0; i < lights->primitiveCount; i++) {
    switch (prm.f3->type) {
    case TypePointLight:

      if (!ambient) {
        colour = prm.pointLight->colour;

        colour.r = colour.r >> 1;
        colour.g = colour.g >> 1;
        colour.b = colour.b >> 1;

#if 0
				printf("AMBIENT = %d %d %d\n", colour.r, colour.g, colour.b);
#endif

        SetBackColor(colour.r, colour.g, colour.b);
      }

      ambient += 1;

      prm.pointLight += 1;
      break;

    case TypeInfiniteLight:

      colour = prm.infiniteLight->colour;
      direction = prm.infiniteLight->direction;

      if (direction.vx > 4098)
        direction.vx = 4098;
      if (direction.vx < -4098)
        direction.vx = -4098;

      if (direction.vy > 4098)
        direction.vy = 4098;
      if (direction.vy < -4098)
        direction.vy = -4098;

      if (direction.vz > 4098)
        direction.vz = 4098;
      if (direction.vz < -4098)
        direction.vz = -4098;

#if 0
			printf("LIGHT = %d %d %d\n", colour.r, colour.g, colour.b);
			printf("DIRECT = %d %d %d\n", direction.vx, direction.vy, direction.vz);
#endif

      if (diffuse == 0) {
        lm.m[0][0] = direction.vx;
        lm.m[0][1] = direction.vy;
        lm.m[0][2] = direction.vz;

        cm.m[0][0] = colour.r << 4;
        cm.m[1][0] = colour.g << 4;
        cm.m[2][0] = colour.b << 4;
      } else if (diffuse == 1) {
        lm.m[1][0] = direction.vx;
        lm.m[1][1] = direction.vy;
        lm.m[1][2] = direction.vz;

        cm.m[0][1] = colour.r << 4;
        cm.m[1][1] = colour.g << 4;
        cm.m[2][1] = colour.b << 4;
      } else if (diffuse == 2) {
        lm.m[2][0] = direction.vx;
        lm.m[2][1] = direction.vy;
        lm.m[2][2] = direction.vz;

        cm.m[0][2] = colour.r << 4;
        cm.m[1][2] = colour.g << 4;
        cm.m[2][2] = colour.b << 4;
      }

      diffuse += 1;

      prm.infiniteLight += 1;
      break;

    default:
      printf("WARNING: Bad primitive $%x in lights\n", prm.f3->type);
      break;
    }
  }

  if (diffuse >= 3) {
    SetColorMatrix(&cm);
    SetLightMatrix(&lm);
  }
}

#define MaxVertices 1024

SVECTOR StaticVertex[MaxVertices];
DVECTOR StaticScreen[MaxVertices];
uint16_t StaticDepth[MaxVertices];
uint16_t StaticFlags[MaxVertices];
uint16_t StaticZ[MaxVertices];

#if 0
#define ClipFlag (0xfbfe)
#else
#define ClipFlag ((CrtClipped | ZLarge | ZNegative | ZSmall) >> 12)
#endif

#define ShortClipFlag ((XClipped | YClipped | ZLarge | ZNegative | ZSmall) >> 12)

void TransformStaticObjectHole(Object* object, Skeleton* camera) {
  printf("TransformStaticObjectHole %p (%s)\n", object, object->name);
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

  SVECTOR* vertex;

  int16_t primitiveCount;
  int16_t vertexCount;

  /* experimental light sourced stuff */

  SVECTOR* normal;

  SVECTOR nsv0, nsv1, nsv2, nsv3;
  VECTOR nv0, nv1, nv2, nv3;

  int32_t flagN0, flagN1, flagN2, flagN3;

  MATRIX matrix;
  VECTOR trans;

  CVECTOR cv, c;

  SetSkeletonTransform(object->skeleton);

  vertex = object->vertices;
  vertexCount = object->vertexCount;

  normal = object->normals;

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

  /* Rotate and translate the vertices */

  RotTransPersN(vertex, screenPtr, depthPtr, flagsPtr, vertexCount);

  /* Set the rotation matrix for rotating the normals */

  trans.vx = 0;
  trans.vy = 0;
  trans.vz = 0;

  TransMatrix(&matrix, &trans);
  SetTransMatrix(&matrix);

  /* Process the primitives */

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
          prims.gt4->colour0.b = poly.gt4->colour[0].b;
          prims.gt4->colour1 = poly.gt4->colour[1];
          prims.gt4->colour2 = poly.gt4->colour[2];
          prims.gt4->colour3 = poly.gt4->colour[3];

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
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
          prims.ft3->colour0.g = 0;
          prims.ft3->colour0.b = BLACK_NOT_DRAWN;

          prims.ft3->xy0.vx = screenPtr[coord0].vx;
          prims.ft3->xy0.vy = screenPtr[coord0].vy;
          prims.ft3->xy0.vz = depthPtr[coord0];
          prims.ft3->xy1.vx = screenPtr[coord1].vx;
          prims.ft3->xy1.vy = screenPtr[coord1].vy;
          prims.ft3->xy1.vz = depthPtr[coord1];
          prims.ft3->xy2.vx = screenPtr[coord2].vx;
          prims.ft3->xy2.vy = screenPtr[coord2].vy;
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
          prims.ft4->colour0.g = 0;
          prims.ft4->colour0.b = BLACK_NOT_DRAWN;

          prims.ft4->xy0.vx = screenPtr[coord0].vx;
          prims.ft4->xy0.vy = screenPtr[coord0].vy;
          prims.ft4->xy0.vz = depthPtr[coord0];
          prims.ft4->xy1.vx = screenPtr[coord1].vx;
          prims.ft4->xy1.vy = screenPtr[coord1].vy;
          prims.ft4->xy1.vz = depthPtr[coord1];
          prims.ft4->xy2.vx = screenPtr[coord2].vx;
          prims.ft4->xy2.vy = screenPtr[coord2].vy;
          prims.ft4->xy2.vz = depthPtr[coord2];
          prims.ft4->xy3.vx = screenPtr[coord3].vx;
          prims.ft4->xy3.vy = screenPtr[coord3].vy;
          prims.ft4->xy3.vz = depthPtr[coord3];
          prims.ft4->xy0.pad = 0;
          prims.ft4->xy1.pad = 0;
          prims.ft4->xy2.pad = 0;
          prims.ft4->xy3.pad = 0;

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

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
          prims.g3->colour0.g = 0;
          prims.g3->colour0.b = BLACK_NOT_DRAWN;
          prims.g3->colour1 = poly.g3->colour[1];
          prims.g3->colour2 = poly.g3->colour[2];

          prims.g3->xy0 = screenPtr[coord0];
          prims.g3->xy1 = screenPtr[coord1];
          prims.g3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

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
          prims.g4->colour0.g = 0;
          prims.g4->colour0.b = BLACK_NOT_DRAWN;
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
          prims.f3->colour0.g = 0;
          prims.f3->colour0.b = BLACK_NOT_DRAWN;
          prims.f3->xy0 = screenPtr[coord0];
          prims.f3->xy1 = screenPtr[coord1];
          prims.f3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

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
          prims.f4->colour0.g = 0;
          prims.f4->colour0.b = BLACK_NOT_DRAWN;
          prims.f4->xy0 = screenPtr[coord0];
          prims.f4->xy1 = screenPtr[coord1];
          prims.f4->xy2 = screenPtr[coord2];
          prims.f4->xy3 = screenPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.f4 += 1;
        }
      }

      poly.f4 += 1;
      break;

      /* LIGHT SOURCED POLYGONS */

    case TypeLSGT3:

      coord0 = poly.lsgt3->coords[0];
      coord1 = poly.lsgt3->coords[1];
      coord2 = poly.lsgt3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT3((POLY_GT3*)prims.ptr);

          prims.gt3->cba = poly.lsgt3->cba;
          prims.gt3->tsb = poly.lsgt3->tsb;

          prims.gt3->u0 = poly.lsgt3->u0;
          prims.gt3->v0 = poly.lsgt3->v0;
          prims.gt3->u1 = poly.lsgt3->u1;
          prims.gt3->v1 = poly.lsgt3->v1;
          prims.gt3->u2 = poly.lsgt3->u2;
          prims.gt3->v2 = poly.lsgt3->v2;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          c.r = poly.lsgt3->colour[0].r >> 1;
          c.g = poly.lsgt3->colour[0].g >> 1;
          c.b = poly.lsgt3->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt3->colour0);

          c.r = poly.lsgt3->colour[1].r >> 1;
          c.g = poly.lsgt3->colour[1].g >> 1;
          c.b = poly.lsgt3->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt3->colour1);

          c.r = poly.lsgt3->colour[2].r >> 1;
          c.g = poly.lsgt3->colour[2].g >> 1;
          c.b = poly.lsgt3->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt3->colour2);

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

      poly.lsgt3 += 1;
      break;

    case TypeLSGT4:

      coord0 = poly.lsgt4->coords[0];
      coord1 = poly.lsgt4->coords[1];
      coord2 = poly.lsgt4->coords[2];
      coord3 = poly.lsgt4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT4((POLY_GT4*)prims.ptr);

          prims.gt4->cba = poly.lsgt4->cba;
          prims.gt4->tsb = poly.lsgt4->tsb;

          prims.gt4->u0 = poly.lsgt4->u0;
          prims.gt4->v0 = poly.lsgt4->v0;
          prims.gt4->u1 = poly.lsgt4->u1;
          prims.gt4->v1 = poly.lsgt4->v1;
          prims.gt4->u2 = poly.lsgt4->u2;
          prims.gt4->v2 = poly.lsgt4->v2;
          prims.gt4->u3 = poly.lsgt4->u3;
          prims.gt4->v3 = poly.lsgt4->v3;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsgt4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          c.r = poly.lsgt4->colour[0].r >> 1;
          c.g = poly.lsgt4->colour[0].g >> 1;
          c.b = poly.lsgt4->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt4->colour0);

          c.r = poly.lsgt4->colour[1].r >> 1;
          c.g = poly.lsgt4->colour[1].g >> 1;
          c.b = poly.lsgt4->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt4->colour1);

          c.r = poly.lsgt4->colour[2].r >> 1;
          c.g = poly.lsgt4->colour[2].g >> 1;
          c.b = poly.lsgt4->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt4->colour2);

          c.r = poly.lsgt4->colour[3].r >> 1;
          c.g = poly.lsgt4->colour[3].g >> 1;
          c.b = poly.lsgt4->colour[3].b >> 1;
          NormalColorCol(&nsv3, &c, &prims.gt4->colour3);

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy3.vz = depthPtr[coord3];
          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt4 += 1;
        }
      }

      poly.lsgt4 += 1;
      break;

    case TypeLSG3:

      coord0 = poly.lsg3->coords[0];
      coord1 = poly.lsg3->coords[1];
      coord2 = poly.lsg3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG3((POLY_G3*)prims.ptr);

          /* Light sourcing */

          RotTrans(&normal[poly.lsg3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          NormalColorCol(&nsv0, &poly.lsg3->colour[0], &prims.g3->colour0);
          NormalColorCol(&nsv1, &poly.lsg3->colour[1], &prims.g3->colour1);
          NormalColorCol(&nsv2, &poly.lsg3->colour[2], &prims.g3->colour2);

          prims.g3->xy0 = screenPtr[coord0];
          prims.g3->xy1 = screenPtr[coord1];
          prims.g3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g3 += 1;
        }
      }

      poly.lsg3 += 1;
      break;

    case TypeLSG4:

      coord0 = poly.lsg4->coords[0];
      coord1 = poly.lsg4->coords[1];
      coord2 = poly.lsg4->coords[2];
      coord3 = poly.lsg4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG4((POLY_G4*)prims.ptr);

#if 0
					prims.g4->colour0.r = 230;      // PCwipeout
					prims.g4->colour0.g = 0;
					prims.g4->colour0.b = 0;
					prims.g4->colour1 = poly.lsg4->colour[1];
					prims.g4->colour2 = poly.lsg4->colour[2];
					prims.g4->colour3 = poly.lsg4->colour[3];
#else
          /* Light sourcing */

          RotTrans(&normal[poly.lsg4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsg4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          NormalColorCol(&nsv0, &poly.lsg4->colour[0], &cv);
          prims.g4->colour0.r = 230; // PCwipeout
          prims.g4->colour0.g = 0;
          prims.g4->colour0.b = 0;

          NormalColorCol(&nsv1, &poly.lsg4->colour[1], &prims.g4->colour1);
          NormalColorCol(&nsv2, &poly.lsg4->colour[2], &prims.g4->colour2);
          NormalColorCol(&nsv3, &poly.lsg4->colour[3], &prims.g4->colour3);
#endif

          prims.g4->xy0 = screenPtr[coord0];
          prims.g4->xy1 = screenPtr[coord1];
          prims.g4->xy2 = screenPtr[coord2];
          prims.g4->xy3 = screenPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g4 += 1;
        }
      }

      poly.lsg4 += 1;
      break;

    default:

      printf("Bad primitive type %d\n", poly.prim->type);

      break;
    }
  }
}

void TransformStaticObject(Object* object, Skeleton* camera) {
  // printf("TransformStaticObject %p (%s)\n", object, object->name);

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

  SVECTOR* vertex;

  int16_t primitiveCount;
  int16_t vertexCount;

  /* experimental light sourced stuff */

  SVECTOR* normal;

  SVECTOR nsv0, nsv1, nsv2, nsv3;
  VECTOR nv0, nv1, nv2, nv3;

  int32_t flagN0, flagN1, flagN2, flagN3;

  MATRIX matrix;
  VECTOR trans;

  CVECTOR cv, c;

  SetSkeletonTransform(object->skeleton);

  vertex = object->vertices;
  vertexCount = object->vertexCount;

  normal = object->normals;

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

  /* Rotate and translate the vertices */

  RotTransPersN(vertex, screenPtr, depthPtr, flagsPtr, vertexCount);

  /* Set the rotation matrix for rotating the normals */

  trans.vx = 0;
  trans.vy = 0;
  trans.vz = 0;

  TransMatrix(&matrix, &trans);
  SetTransMatrix(&matrix);

  /* Process the primitives */

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
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy2.vz = depthPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

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
          prims.gt4->colour0.b = poly.gt4->colour[0].b;
          prims.gt4->colour1 = poly.gt4->colour[1];
          prims.gt4->colour2 = poly.gt4->colour[2];
          prims.gt4->colour3 = poly.gt4->colour[3];

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
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

          //prims.ft3->xy0 = screenPtr[coord0];
          //prims.ft3->xy1 = screenPtr[coord1];
          //prims.ft3->xy2 = screenPtr[coord2];
          prims.ft3->xy0.vx = screenPtr[coord0].vx;
          prims.ft3->xy0.vy = screenPtr[coord0].vy;
          prims.ft3->xy0.vz = depthPtr[coord0];
          prims.ft3->xy1.vx = screenPtr[coord1].vx;
          prims.ft3->xy1.vy = screenPtr[coord1].vy;
          prims.ft3->xy1.vz = depthPtr[coord1];
          prims.ft3->xy2.vx = screenPtr[coord2].vx;
          prims.ft3->xy2.vy = screenPtr[coord2].vy;
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

          //prims.ft4->xy0 = screenPtr[coord0];
          //prims.ft4->xy1 = screenPtr[coord1];
          //prims.ft4->xy2 = screenPtr[coord2];
          //prims.ft4->xy3 = screenPtr[coord3];
          prims.ft4->xy0.vx = screenPtr[coord0].vx;
          prims.ft4->xy0.vy = screenPtr[coord0].vy;
          prims.ft4->xy0.vz = depthPtr[coord0];
          prims.ft4->xy1.vx = screenPtr[coord1].vx;
          prims.ft4->xy1.vy = screenPtr[coord1].vy;
          prims.ft4->xy1.vz = depthPtr[coord1];
          prims.ft4->xy2.vx = screenPtr[coord2].vx;
          prims.ft4->xy2.vy = screenPtr[coord2].vy;
          prims.ft4->xy2.vz = depthPtr[coord2];
          prims.ft4->xy3.vx = screenPtr[coord3].vx;
          prims.ft4->xy3.vy = screenPtr[coord3].vy;
          prims.ft4->xy3.vz = depthPtr[coord3];
          prims.ft4->xy0.pad = 0;
          prims.ft4->xy1.pad = 0;
          prims.ft4->xy2.pad = 0;
          prims.ft4->xy3.pad = 0;

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

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

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.f4 += 1;
        }
      }

      poly.f4 += 1;
      break;

      /* LIGHT SOURCED POLYGONS */

    case TypeLSGT3:

      coord0 = poly.lsgt3->coords[0];
      coord1 = poly.lsgt3->coords[1];
      coord2 = poly.lsgt3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT3((POLY_GT3*)prims.ptr);

          prims.gt3->cba = poly.lsgt3->cba;
          prims.gt3->tsb = poly.lsgt3->tsb;

          prims.gt3->u0 = poly.lsgt3->u0;
          prims.gt3->v0 = poly.lsgt3->v0;
          prims.gt3->u1 = poly.lsgt3->u1;
          prims.gt3->v1 = poly.lsgt3->v1;
          prims.gt3->u2 = poly.lsgt3->u2;
          prims.gt3->v2 = poly.lsgt3->v2;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          c.r = poly.lsgt3->colour[0].r >> 1;
          c.g = poly.lsgt3->colour[0].g >> 1;
          c.b = poly.lsgt3->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt3->colour0);

          c.r = poly.lsgt3->colour[1].r >> 1;
          c.g = poly.lsgt3->colour[1].g >> 1;
          c.b = poly.lsgt3->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt3->colour1);

          c.r = poly.lsgt3->colour[2].r >> 1;
          c.g = poly.lsgt3->colour[2].g >> 1;
          c.b = poly.lsgt3->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt3->colour2);

          prims.gt3->xy0.vx = screenPtr[coord0].vx;
          prims.gt3->xy0.vy = screenPtr[coord0].vy;
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy2.vz = depthPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt3 += 1;
        }
      }

      poly.lsgt3 += 1;
      break;

    case TypeLSGT4:

      coord0 = poly.lsgt4->coords[0];
      coord1 = poly.lsgt4->coords[1];
      coord2 = poly.lsgt4->coords[2];
      coord3 = poly.lsgt4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT4((POLY_GT4*)prims.ptr);

          prims.gt4->cba = poly.lsgt4->cba;
          prims.gt4->tsb = poly.lsgt4->tsb;

          prims.gt4->u0 = poly.lsgt4->u0;
          prims.gt4->v0 = poly.lsgt4->v0;
          prims.gt4->u1 = poly.lsgt4->u1;
          prims.gt4->v1 = poly.lsgt4->v1;
          prims.gt4->u2 = poly.lsgt4->u2;
          prims.gt4->v2 = poly.lsgt4->v2;
          prims.gt4->u3 = poly.lsgt4->u3;
          prims.gt4->v3 = poly.lsgt4->v3;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsgt4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          c.r = poly.lsgt4->colour[0].r >> 1;
          c.g = poly.lsgt4->colour[0].g >> 1;
          c.b = poly.lsgt4->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt4->colour0);

          c.r = poly.lsgt4->colour[1].r >> 1;
          c.g = poly.lsgt4->colour[1].g >> 1;
          c.b = poly.lsgt4->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt4->colour1);

          c.r = poly.lsgt4->colour[2].r >> 1;
          c.g = poly.lsgt4->colour[2].g >> 1;
          c.b = poly.lsgt4->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt4->colour2);

          c.r = poly.lsgt4->colour[3].r >> 1;
          c.g = poly.lsgt4->colour[3].g >> 1;
          c.b = poly.lsgt4->colour[3].b >> 1;
          NormalColorCol(&nsv3, &c, &prims.gt4->colour3);

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy3.vz = depthPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt4 += 1;
        }
      }

      poly.lsgt4 += 1;
      break;

    case TypeLSG3:

      coord0 = poly.lsg3->coords[0];
      coord1 = poly.lsg3->coords[1];
      coord2 = poly.lsg3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG3((POLY_G3*)prims.ptr);

#if 0
					prims.g3->colour0.r = 230;      // PCwipeout
					prims.g3->colour0.g = 0;
					prims.g3->colour0.b = 0;
					prims.g3->colour1 = poly.lsg3->colour[1];
					prims.g3->colour2 = poly.lsg3->colour[2];
#else
          /* Light sourcing */

          RotTrans(&normal[poly.lsg3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          NormalColorCol(&nsv0, &poly.lsg3->colour[0], &cv);
          prims.g3->colour0.r = poly.lsg3->colour[0].r; // PCwipeout
          prims.g3->colour0.g = 0;
          prims.g3->colour0.b = 0;

          NormalColorCol(&nsv1, &poly.lsg3->colour[1], &prims.g3->colour1);
          NormalColorCol(&nsv2, &poly.lsg3->colour[2], &prims.g3->colour2);
#endif

          prims.g3->xy0 = screenPtr[coord0];
          prims.g3->xy1 = screenPtr[coord1];
          prims.g3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g3 += 1;
        }
      }

      poly.lsg3 += 1;
      break;

    case TypeLSG4:

      coord0 = poly.lsg4->coords[0];
      coord1 = poly.lsg4->coords[1];
      coord2 = poly.lsg4->coords[2];
      coord3 = poly.lsg4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG4((POLY_G4*)prims.ptr);

#if 0
					prims.g4->colour0.r = 230;      // PCwipeout
					prims.g4->colour0.g = 0;
					prims.g4->colour0.b = 0;
					prims.g4->colour1 = poly.lsg4->colour[1];
					prims.g4->colour2 = poly.lsg4->colour[2];
					prims.g4->colour3 = poly.lsg4->colour[3];
#else
          /* Light sourcing */

          RotTrans(&normal[poly.lsg4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsg4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          NormalColorCol(&nsv0, &poly.lsg4->colour[0], &cv);
          prims.g4->colour0.r = poly.lsg4->colour[0].r; // PCwipeout
          prims.g4->colour0.g = 0;
          prims.g4->colour0.b = 0;

          NormalColorCol(&nsv1, &poly.lsg4->colour[1], &prims.g4->colour1);
          NormalColorCol(&nsv2, &poly.lsg4->colour[2], &prims.g4->colour2);
          NormalColorCol(&nsv3, &poly.lsg4->colour[3], &prims.g4->colour3);
#endif

          prims.g4->xy0 = screenPtr[coord0];
          prims.g4->xy1 = screenPtr[coord1];
          prims.g4->xy2 = screenPtr[coord2];
          prims.g4->xy3 = screenPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g4 += 1;
        }
      }

      poly.lsg4 += 1;
      break;

    default:

      printf("Bad primitive type %d\n", poly.prim->type);

      break;
    }
  }
}

PolyF4* Quad(
    int16_t red,
    int16_t green,
    int16_t blue,
    int16_t border) {
  /* Allocate and Initialise Primitive */

  POLY_F4* quad;

  quad = (POLY_F4*)(DAlloc(heap, sizeof(PolyF4)));

  if (!quad) {
    /* Memory Allocation Failed! */

    printf("Main.c:Quad(): Failed to allocate PolyF4\n");
    return NULL;
  }

  SetPolyF4(quad);

  setRGB0(quad, red, green, blue);

  setXY4(quad,
         border, border,
         256 - border, border,
         border, 240 - border,
         256 - border, 240 - border);

  return (PolyF4*)quad;
}

BlkFill* BlockFill(
    int16_t x,
    int16_t y,
    int16_t w,
    int16_t h,
    int16_t red,
    int16_t green,
    int16_t blue) {
  /* Allocate and Initialise Primitive */

  BLK_FILL* blk;

  blk = (BLK_FILL*)(DAlloc(heap, sizeof(BlkFill)));

  if (!blk) {
    /* Memory Allocation Failed! */
    sprintf(errorString, "Main.c:BlockFill(): Failed to allocate BlkFill\n");
    Error(errorString, Fatal);
  }

  SetBlockFill(blk);

  setXY0(blk, x, y);
  setWH(blk, w, h);

  setRGB0(blk, red, green, blue);

  return (BlkFill*)blk;
}

void InitFonts() {
  /* Initialise the system font */

  // FntLoad( 960,256 );
  // SetDumpFnt( FntOpen( 10,70,310,230,0,256 ) );
}

void DoJedvars() {
  if (ScreenSize == FULL) {
    switch (Resolution) {
    case HIGH:
      halfxf = 0;
      halfyf = 0;
      pixdouble = 0;
      break;
    case MEDIUM:
      halfxf = 2;
      halfyf = 0;
      pixdouble = 1;
      break;
    case LOW:
      halfxf = 2;
      halfyf = 2;
      pixdouble = 2;
      break;
    }
  }
  if (ScreenSize == MEDIUM) {
    switch (Resolution) {
    case HIGH:
      halfxf = 1.5;
      halfyf = 1.5;
      pixdouble = 0;
      break;
    case MEDIUM:
      halfxf = 3;
      halfyf = 1.5;
      pixdouble = 1;
      break;
    case LOW:
      halfxf = 3;
      halfyf = 3;
      pixdouble = 2;
      break;
    }
  }
  if (ScreenSize == SMALL) {
    switch (Resolution) {
    case HIGH:
      halfxf = 2;
      halfyf = 2;
      pixdouble = 0;
      break;
    case MEDIUM:
      halfxf = 4;
      halfyf = 2;
      pixdouble = 1;
      break;
    case LOW:
      halfxf = 4;
      halfyf = 4;
      pixdouble = 2;
      break;
    }
  }
}

void TransformStaticObjectBodge(Object* object, Skeleton* camera) {
  // printf("TransformStaticObjectBodge %p (%s)\n", object, object->name);

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

  SVECTOR* vertex;

  int16_t primitiveCount;
  int16_t vertexCount;

  /* experimental light sourced stuff */

  SVECTOR* normal;

  SVECTOR nsv0, nsv1, nsv2, nsv3;
  VECTOR nv0, nv1, nv2, nv3;

  int32_t flagN0, flagN1, flagN2, flagN3;

  MATRIX matrix;
  VECTOR trans;

  CVECTOR cv, c;

  SetSkeletonTransform(object->skeleton);

  vertex = object->vertices;
  vertexCount = object->vertexCount;

  normal = object->normals;

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

  /* Rotate and translate the vertices */

  RotTransPersN(vertex, screenPtr, depthPtr, flagsPtr, vertexCount);

  /* Set the rotation matrix for rotating the normals */

  trans.vx = 0;
  trans.vy = 0;
  trans.vz = 0;

  TransMatrix(&matrix, &trans);
  SetTransMatrix(&matrix);

  /* Process the primitives */

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
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy2.vz = depthPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
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
          prims.gt4->colour0.b = poly.gt4->colour[0].b;
          prims.gt4->colour1 = poly.gt4->colour[1];
          prims.gt4->colour2 = poly.gt4->colour[2];
          prims.gt4->colour3 = poly.gt4->colour[3];

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy3.vz = depthPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
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

          //prims.ft3->xy0 = screenPtr[coord0];
          //prims.ft3->xy1 = screenPtr[coord1];
          //prims.ft3->xy2 = screenPtr[coord2];
          prims.ft3->xy0.vx = screenPtr[coord0].vx;
          prims.ft3->xy0.vy = screenPtr[coord0].vy;
          prims.ft3->xy0.vz = depthPtr[coord0];
          prims.ft3->xy1.vx = screenPtr[coord1].vx;
          prims.ft3->xy1.vy = screenPtr[coord1].vy;
          prims.ft3->xy1.vz = depthPtr[coord1];
          prims.ft3->xy2.vx = screenPtr[coord2].vx;
          prims.ft3->xy2.vy = screenPtr[coord2].vy;
          prims.ft3->xy2.vz = depthPtr[coord2];
          prims.ft3->xy0.pad = 0;
          prims.ft3->xy1.pad = 0;
          prims.ft3->xy2.pad = 0;

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
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

          //prims.ft4->xy0 = screenPtr[coord0];
          //prims.ft4->xy1 = screenPtr[coord1];
          //prims.ft4->xy2 = screenPtr[coord2];
          //prims.ft4->xy3 = screenPtr[coord3];
          prims.ft4->xy0.vx = screenPtr[coord0].vx;
          prims.ft4->xy0.vy = screenPtr[coord0].vy;
          prims.ft4->xy0.vz = depthPtr[coord0];
          prims.ft4->xy1.vx = screenPtr[coord1].vx;
          prims.ft4->xy1.vy = screenPtr[coord1].vy;
          prims.ft4->xy1.vz = depthPtr[coord1];
          prims.ft4->xy2.vx = screenPtr[coord2].vx;
          prims.ft4->xy2.vy = screenPtr[coord2].vy;
          prims.ft4->xy2.vz = depthPtr[coord2];
          prims.ft4->xy3.vx = screenPtr[coord3].vx;
          prims.ft4->xy3.vy = screenPtr[coord3].vy;
          prims.ft4->xy3.vz = depthPtr[coord3];
          prims.ft4->xy0.pad = 0;
          prims.ft4->xy1.pad = 0;
          prims.ft4->xy2.pad = 0;
          prims.ft4->xy3.pad = 0;

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
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

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
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

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
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

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
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

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
          prims.f4 += 1;
        }
      }

      poly.f4 += 1;
      break;

      /* LIGHT SOURCED POLYGONS */

    case TypeLSGT3:

      coord0 = poly.lsgt3->coords[0];
      coord1 = poly.lsgt3->coords[1];
      coord2 = poly.lsgt3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT3((POLY_GT3*)prims.ptr);

          prims.gt3->cba = poly.lsgt3->cba;
          prims.gt3->tsb = poly.lsgt3->tsb;

          prims.gt3->u0 = poly.lsgt3->u0;
          prims.gt3->v0 = poly.lsgt3->v0;
          prims.gt3->u1 = poly.lsgt3->u1;
          prims.gt3->v1 = poly.lsgt3->v1;
          prims.gt3->u2 = poly.lsgt3->u2;
          prims.gt3->v2 = poly.lsgt3->v2;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          c.r = poly.lsgt3->colour[0].r >> 1;
          c.g = poly.lsgt3->colour[0].g >> 1;
          c.b = poly.lsgt3->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt3->colour0);

          c.r = poly.lsgt3->colour[1].r >> 1;
          c.g = poly.lsgt3->colour[1].g >> 1;
          c.b = poly.lsgt3->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt3->colour1);

          c.r = poly.lsgt3->colour[2].r >> 1;
          c.g = poly.lsgt3->colour[2].g >> 1;
          c.b = poly.lsgt3->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt3->colour2);

          prims.gt3->xy0.vx = screenPtr[coord0].vx;
          prims.gt3->xy0.vy = screenPtr[coord0].vy;
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy2.vz = depthPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
          prims.gt3 += 1;
        }
      }

      poly.lsgt3 += 1;
      break;

    case TypeLSGT4:

      coord0 = poly.lsgt4->coords[0];
      coord1 = poly.lsgt4->coords[1];
      coord2 = poly.lsgt4->coords[2];
      coord3 = poly.lsgt4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT4((POLY_GT4*)prims.ptr);

          prims.gt4->cba = poly.lsgt4->cba;
          prims.gt4->tsb = poly.lsgt4->tsb;

          prims.gt4->u0 = poly.lsgt4->u0;
          prims.gt4->v0 = poly.lsgt4->v0;
          prims.gt4->u1 = poly.lsgt4->u1;
          prims.gt4->v1 = poly.lsgt4->v1;
          prims.gt4->u2 = poly.lsgt4->u2;
          prims.gt4->v2 = poly.lsgt4->v2;
          prims.gt4->u3 = poly.lsgt4->u3;
          prims.gt4->v3 = poly.lsgt4->v3;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsgt4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          c.r = poly.lsgt4->colour[0].r >> 1;
          c.g = poly.lsgt4->colour[0].g >> 1;
          c.b = poly.lsgt4->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt4->colour0);
          c.r = poly.lsgt4->colour[1].r >> 1;
          c.g = poly.lsgt4->colour[1].g >> 1;
          c.b = poly.lsgt4->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt4->colour1);

          c.r = poly.lsgt4->colour[2].r >> 1;
          c.g = poly.lsgt4->colour[2].g >> 1;
          c.b = poly.lsgt4->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt4->colour2);

          c.r = poly.lsgt4->colour[3].r >> 1;
          c.g = poly.lsgt4->colour[3].g >> 1;
          c.b = poly.lsgt4->colour[3].b >> 1;
          NormalColorCol(&nsv3, &c, &prims.gt4->colour3);

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy3.vz = depthPtr[coord3];
          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
          prims.gt4 += 1;
        }
      }

      poly.lsgt4 += 1;
      break;

    case TypeLSG3:

      coord0 = poly.lsg3->coords[0];
      coord1 = poly.lsg3->coords[1];
      coord2 = poly.lsg3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG3((POLY_G3*)prims.ptr);

          /* Light sourcing */

          RotTrans(&normal[poly.lsg3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          NormalColorCol(&nsv0, &poly.lsg3->colour[0], &cv);
          prims.g3->colour0.r = poly.lsg3->colour[0].r; // PCwipeout
          prims.g3->colour0.g = 0;
          prims.g3->colour0.b = 0;

          NormalColorCol(&nsv1, &poly.lsg3->colour[1], &prims.g3->colour1);
          NormalColorCol(&nsv2, &poly.lsg3->colour[2], &prims.g3->colour2);

          prims.g3->xy0 = screenPtr[coord0];
          prims.g3->xy1 = screenPtr[coord1];
          prims.g3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
          prims.g3 += 1;
        }
      }

      poly.lsg3 += 1;
      break;

    case TypeLSG4:

      coord0 = poly.lsg4->coords[0];
      coord1 = poly.lsg4->coords[1];
      coord2 = poly.lsg4->coords[2];
      coord3 = poly.lsg4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG4((POLY_G4*)prims.ptr);

#if 0
					prims.g4->colour0.r = 230;      // PCwipeout
					prims.g4->colour0.g = 0;
					prims.g4->colour0.b = 0;
					prims.g4->colour1 = poly.lsg4->colour[1];
					prims.g4->colour2 = poly.lsg4->colour[2];
					prims.g4->colour3 = poly.lsg4->colour[3];
#else
          /* Light sourcing */

          RotTrans(&normal[poly.lsg4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsg4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          NormalColorCol(&nsv0, &poly.lsg4->colour[0], &cv);
          prims.g4->colour0.r = poly.lsg4->colour[0].r; // PCwipeout
          prims.g4->colour0.g = 0;
          prims.g4->colour0.b = 0;

          NormalColorCol(&nsv1, &poly.lsg4->colour[1], &prims.g4->colour1);
          NormalColorCol(&nsv2, &poly.lsg4->colour[2], &prims.g4->colour2);
          NormalColorCol(&nsv3, &poly.lsg4->colour[3], &prims.g4->colour3);
#endif

          prims.g4->xy0 = screenPtr[coord0];
          prims.g4->xy1 = screenPtr[coord1];
          prims.g4->xy2 = screenPtr[coord2];
          prims.g4->xy3 = screenPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth - SHIELD_BODGE, (P_TAG*)prims.ptr);
          prims.g4 += 1;
        }
      }

      poly.lsg4 += 1;
      break;
    default:

      printf("Bad primitive type %d\n", poly.prim->type);

      break;
    }
  }
}

// jed added this entire function used by 2 player display etc... on menu's

void TransformStaticMenuObject(Object* object, Skeleton* camera) {
  // printf("TransformStaticMenuObject %p (%s)\n", object, object->name);

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

  SVECTOR* vertex;

  int16_t primitiveCount;
  int16_t vertexCount;

  /* experimental light sourced stuff */

  SVECTOR* normal;

  SVECTOR nsv0, nsv1, nsv2, nsv3;
  VECTOR nv0, nv1, nv2, nv3;

  int32_t flagN0, flagN1, flagN2, flagN3;

  MATRIX matrix;
  VECTOR trans;

  CVECTOR cv, c;

  SetSkeletonTransform(object->skeleton);

  vertex = object->vertices;
  vertexCount = object->vertexCount;

  normal = object->normals;

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

  /* Rotate and translate the vertices */

  RotTransPersN(vertex, screenPtr, depthPtr, flagsPtr, vertexCount);

  /* Set the rotation matrix for rotating the normals */

  trans.vx = 0;
  trans.vy = 0;
  trans.vz = 0;

  TransMatrix(&matrix, &trans);
  SetTransMatrix(&matrix);

  /* Process the primitives */

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
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy2.vz = depthPtr[coord2];

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;

          // depth = AverageZ3( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ] );

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt3 += 1;
        }
        /*else
				{
					SetPolyGT3((POLY_GT3 *) prims.ptr);     // jed

					prims.gt3->cba = poly.gt3->cba;
					prims.gt3->tsb = poly.gt3->tsb;

					prims.gt3->u0 = poly.gt3->u1;
					prims.gt3->v0 = poly.gt3->v0;
					prims.gt3->u1 = poly.gt3->u0;
					prims.gt3->v1 = poly.gt3->v1;
					prims.gt3->u2 = poly.gt3->u2;
					prims.gt3->v2 = poly.gt3->v2;

					prims.gt3->colour0.r = poly.gt3->colour[0].r;
					prims.gt3->colour0.g = poly.gt3->colour[0].g;
					prims.gt3->colour0.b = poly.gt3->colour[0].b;
					prims.gt3->colour1 = poly.gt3->colour[1];
					prims.gt3->colour2 = poly.gt3->colour[2];

					tempx = screenPtr[coord0].vx;   // jed
					tempx1 = screenPtr[coord1].vx;  // jed

					screenPtr[coord0].vx = tempx;   // jed
					screenPtr[coord1].vx = tempx1;  // jed

					prims.gt3->xy0 = screenPtr[coord0];
					prims.gt3->xy1 = screenPtr[coord1];
					prims.gt3->xy2 = screenPtr[coord2];

					// depth = AverageZ3( depthPtr[coord0 ],
					// depthPtr[ coord1 ],
					// depthPtr[ coord2 ] );

					depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;

					//AddPrim(OT[CurrentScreen], depth, (P_TAG *) prims.ptr);
					ClipPolyGT3(OT[CurrentScreen] + depth, (P_TAG *) prims.ptr);

					prims.gt3 += 1;
				}*/
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
          prims.gt4->colour0.b = poly.gt4->colour[0].b;
          prims.gt4->colour1 = poly.gt4->colour[1];
          prims.gt4->colour2 = poly.gt4->colour[2];
          prims.gt4->colour3 = poly.gt4->colour[3];

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy3.vz = depthPtr[coord3];

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2] + depthPtr[coord3]) / 4;

          // depth = AverageZ4( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ],
          // depthPtr[ coord3 ] );

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt4 += 1;
        }
        /*else
				{
					SetPolyGT4((POLY_GT4 *) prims.ptr);

					prims.gt4->cba = poly.gt4->cba;
					prims.gt4->tsb = poly.gt4->tsb;

					prims.gt4->u0 = poly.gt4->u1;
					prims.gt4->v0 = poly.gt4->v0;
					prims.gt4->u1 = poly.gt4->u0;
					prims.gt4->v1 = poly.gt4->v1;
					prims.gt4->u2 = poly.gt4->u3;
					prims.gt4->v2 = poly.gt4->v2;
					prims.gt4->u3 = poly.gt4->u2;
					prims.gt4->v3 = poly.gt4->v3;

					prims.gt4->colour0.r = poly.gt4->colour[0].r;
					prims.gt4->colour0.g = poly.gt4->colour[0].g;
					prims.gt4->colour0.b = poly.gt4->colour[0].b;
					prims.gt4->colour1 = poly.gt4->colour[1];
					prims.gt4->colour2 = poly.gt4->colour[2];
					prims.gt4->colour3 = poly.gt4->colour[3];

					tempx = screenPtr[coord0].vx;   // jed
					tempx1 = screenPtr[coord1].vx;  // jed

					screenPtr[coord0].vx = tempx;   // jed
					screenPtr[coord1].vx = tempx1;  // jed

					tempx = screenPtr[coord2].vx;   // jed
					tempx1 = screenPtr[coord3].vx;  // jed

					screenPtr[coord2].vx = tempx;   // jed
					screenPtr[coord3].vx = tempx1;  // jed

					prims.gt4->xy0 = screenPtr[coord0];
					prims.gt4->xy1 = screenPtr[coord1];
					prims.gt4->xy2 = screenPtr[coord2];
					prims.gt4->xy3 = screenPtr[coord3];

					depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2] + depthPtr[coord3]) / 4;

					// depth = AverageZ4( depthPtr[coord0 ],
					// depthPtr[ coord1 ],
					// depthPtr[ coord2 ],
					// depthPtr[ coord3 ] );

					//AddPrim(OT[CurrentScreen], depth, (P_TAG *) prims.ptr);
					ClipPolyGT4(OT[CurrentScreen] + depth, (P_TAG *) prims.ptr);

					//prims.gt4 += 1;
				}*/
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

          //prims.ft3->xy0 = screenPtr[coord0];
          //prims.ft3->xy1 = screenPtr[coord1];
          //prims.ft3->xy2 = screenPtr[coord2];
          prims.ft3->xy0.vx = screenPtr[coord0].vx;
          prims.ft3->xy0.vy = screenPtr[coord0].vy;
          prims.ft3->xy0.vz = depthPtr[coord0];
          prims.ft3->xy1.vx = screenPtr[coord1].vx;
          prims.ft3->xy1.vy = screenPtr[coord1].vy;
          prims.ft3->xy1.vz = depthPtr[coord1];
          prims.ft3->xy2.vx = screenPtr[coord2].vx;
          prims.ft3->xy2.vy = screenPtr[coord2].vy;
          prims.ft3->xy2.vz = depthPtr[coord2];
          prims.ft3->xy0.pad = 0;
          prims.ft3->xy1.pad = 0;
          prims.ft3->xy2.pad = 0;

          // depth = AverageZ3( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ] );

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.ft3 += 1;
        }
        /*else
				{
					SetPolyFT3((POLY_FT3 *) prims.ptr);

					prims.ft3->cba = poly.ft3->cba;
					prims.ft3->tsb = poly.ft3->tsb;

					prims.ft3->u0 = poly.ft3->u1;
					prims.ft3->v0 = poly.ft3->v0;
					prims.ft3->u1 = poly.ft3->u0;
					prims.ft3->v1 = poly.ft3->v1;
					prims.ft3->u2 = poly.ft3->u2;
					prims.ft3->v2 = poly.ft3->v2;

					prims.ft3->colour0.r = poly.ft3->colour.r;
					prims.ft3->colour0.g = poly.ft3->colour.g;
					prims.ft3->colour0.b = poly.ft3->colour.b;

					tempx = screenPtr[coord0].vx;   // jed
					tempx1 = screenPtr[coord1].vx;  // jed

					screenPtr[coord0].vx = tempx;   // jed
					screenPtr[coord1].vx = tempx1;  // jed

					prims.ft3->xy0 = screenPtr[coord0];
					prims.ft3->xy1 = screenPtr[coord1];
					prims.ft3->xy2 = screenPtr[coord2];

					depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;

					// depth = AverageZ3( depthPtr[coord0 ],
					// depthPtr[ coord1 ],
					// depthPtr[ coord2 ] );

					//AddPrim(OT[CurrentScreen], depth, (P_TAG *) prims.ptr);
					ClipPolyFT3(OT[CurrentScreen] + depth, (P_TAG *) prims.ptr);

					//prims.ft3 += 1;
				}*/
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

          //prims.ft4->xy0 = screenPtr[coord0];
          //prims.ft4->xy1 = screenPtr[coord1];
          //prims.ft4->xy2 = screenPtr[coord2];
          //prims.ft4->xy3 = screenPtr[coord3];
          prims.ft4->xy0.vx = screenPtr[coord0].vx;
          prims.ft4->xy0.vy = screenPtr[coord0].vy;
          prims.ft4->xy0.vz = depthPtr[coord0];
          prims.ft4->xy1.vx = screenPtr[coord1].vx;
          prims.ft4->xy1.vy = screenPtr[coord1].vy;
          prims.ft4->xy1.vz = depthPtr[coord1];
          prims.ft4->xy2.vx = screenPtr[coord2].vx;
          prims.ft4->xy2.vy = screenPtr[coord2].vy;
          prims.ft4->xy2.vz = depthPtr[coord2];
          prims.ft4->xy3.vx = screenPtr[coord3].vx;
          prims.ft4->xy3.vy = screenPtr[coord3].vy;
          prims.ft4->xy3.vz = depthPtr[coord3];
          prims.ft4->xy0.pad = 0;
          prims.ft4->xy1.pad = 0;
          prims.ft4->xy2.pad = 0;
          prims.ft4->xy3.pad = 0;

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2] + depthPtr[coord3]) / 4;

          // depth = AverageZ4( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ],
          // depthPtr[ coord3 ] );

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

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;

          // depth = AverageZ3( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ] );

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g3 += 1;
        }
        /*else
				{
					SetPolyG3((POLY_G3 *) prims.ptr);

					prims.g3->colour0.r = poly.g3->colour[0].r;
					prims.g3->colour0.g = poly.g3->colour[0].g;
					prims.g3->colour0.b = poly.g3->colour[0].b;
					prims.g3->colour1 = poly.g3->colour[1];
					prims.g3->colour2 = poly.g3->colour[2];

					tempx = screenPtr[coord0].vx;   // jed
					tempx1 = screenPtr[coord1].vx;  // jed

					screenPtr[coord0].vx = tempx;   // jed
					screenPtr[coord1].vx = tempx1;  // jed

					prims.g3->xy0 = screenPtr[coord0];
					prims.g3->xy1 = screenPtr[coord1];
					prims.g3->xy2 = screenPtr[coord2];


					depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;


					// depth = AverageZ3( depthPtr[coord0 ],
					// depthPtr[ coord1 ],
					// depthPtr[ coord2 ] );

					//AddPrim(OT[CurrentScreen], depth, (P_TAG *) prims.ptr);
					ClipPolyG3(OT[CurrentScreen] + depth, (P_TAG *) prims.ptr);

					//prims.g3 += 1;
				}*/
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

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2] + depthPtr[coord3]) / 4;

          // depth = AverageZ4( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ],
          // depthPtr[ coord3 ] );

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g4 += 1;
        }
        /*else
				{
					SetPolyG4((POLY_G4 *) prims.ptr);

					prims.g4->colour0.r = poly.g4->colour[0].r;
					prims.g4->colour0.g = poly.g4->colour[0].g;
					prims.g4->colour0.b = poly.g4->colour[0].b;
					prims.g4->colour1 = poly.g4->colour[1];
					prims.g4->colour2 = poly.g4->colour[2];
					prims.g4->colour3 = poly.g4->colour[3];

					tempx = screenPtr[coord0].vx;   // jed
					tempx1 = screenPtr[coord1].vx;  // jed

					screenPtr[coord0].vx = tempx;   // jed
					screenPtr[coord1].vx = tempx1;  // jed

					tempx = screenPtr[coord2].vx;   // jed
					tempx1 = screenPtr[coord3].vx;  // jed

					screenPtr[coord2].vx = tempx;   // jed
					screenPtr[coord3].vx = tempx1;  // jed

					prims.g4->xy0 = screenPtr[coord0];
					prims.g4->xy1 = screenPtr[coord1];
					prims.g4->xy2 = screenPtr[coord2];
					prims.g4->xy3 = screenPtr[coord3];

					depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2] + depthPtr[coord3]) / 4;

					// depth = AverageZ4( depthPtr[coord0 ],
					// depthPtr[ coord1 ],
					// depthPtr[ coord2 ],
					// depthPtr[ coord3 ] );

					//AddPrim(OT[CurrentScreen], depth, (P_TAG *) prims.ptr);
					ClipPolyG4(OT[CurrentScreen] + depth, (P_TAG *) prims.ptr);

					//prims.g4 += 1;
				}*/
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

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;

          // depth = AverageZ3( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ] );

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.f3 += 1;
        }
        /*else
				{
					SetPolyF3((POLY_F3 *) prims.ptr);

					prims.f3->colour0.r = poly.f3->colour.r;
					prims.f3->colour0.g = poly.f3->colour.g;
					prims.f3->colour0.b = poly.f3->colour.b;

					tempx = screenPtr[coord0].vx;   // jed
					tempx1 = screenPtr[coord1].vx;  // jed

					screenPtr[coord0].vx = tempx;   // jed
					screenPtr[coord1].vx = tempx1;  // jed

					prims.f3->xy0 = screenPtr[coord0];
					prims.f3->xy1 = screenPtr[coord1];
					prims.f3->xy2 = screenPtr[coord2];

					depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2]) / 3;

					// depth = AverageZ3( depthPtr[coord0 ],
					// depthPtr[ coord1 ],
					// depthPtr[ coord2 ] );

					//AddPrim(OT[CurrentScreen], depth, (P_TAG *) prims.ptr);
					ClipPolyF3(OT[CurrentScreen] + depth, (P_TAG *) prims.ptr);

					//prims.f3 += 1;
				}*/
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

          depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2] + depthPtr[coord3]) / 4;

          // depth = AverageZ4( depthPtr[coord0 ],
          // depthPtr[ coord1 ],
          // depthPtr[ coord2 ],
          // depthPtr[ coord3 ] );

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);

          prims.f4 += 1;
        }
        /*else
				{
					SetPolyF4((POLY_F4 *) prims.ptr);

					prims.f4->colour0.r = poly.f4->colour.r;
					prims.f4->colour0.g = poly.f4->colour.g;
					prims.f4->colour0.b = poly.f4->colour.b;

					tempx = screenPtr[coord0].vx;   // jed
					tempx1 = screenPtr[coord1].vx;  // jed

					screenPtr[coord0].vx = tempx;   // jed
					screenPtr[coord1].vx = tempx1;  // jed

					tempx = screenPtr[coord2].vx;   // jed
					tempx1 = screenPtr[coord3].vx;  // jed

					screenPtr[coord2].vx = tempx;   // jed
					screenPtr[coord3].vx = tempx1;  // jed

					prims.f4->xy0 = screenPtr[coord0];
					prims.f4->xy1 = screenPtr[coord1];
					prims.f4->xy2 = screenPtr[coord2];
					prims.f4->xy3 = screenPtr[coord3];

					depth = (depthPtr[coord0] + depthPtr[coord1] + depthPtr[coord2] + depthPtr[coord3]) / 4;

					// depth = AverageZ4( depthPtr[coord0 ],
					// depthPtr[ coord1 ],
					// depthPtr[ coord2 ],
					// depthPtr[ coord3 ] );

					//AddPrim(OT[CurrentScreen], depth, (P_TAG *) prims.ptr);
					ClipPolyF4(OT[CurrentScreen] + depth, (P_TAG *) prims.ptr);

					//prims.f4 += 1;

				}*/
      }

      poly.f4 += 1;
      break;

      /* LIGHT SOURCED POLYGONS */

    case TypeLSGT3:

      coord0 = poly.lsgt3->coords[0];
      coord1 = poly.lsgt3->coords[1];
      coord2 = poly.lsgt3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT3((POLY_GT3*)prims.ptr);

          prims.gt3->cba = poly.lsgt3->cba;
          prims.gt3->tsb = poly.lsgt3->tsb;

          prims.gt3->u0 = poly.lsgt3->u0;
          prims.gt3->v0 = poly.lsgt3->v0;
          prims.gt3->u1 = poly.lsgt3->u1;
          prims.gt3->v1 = poly.lsgt3->v1;
          prims.gt3->u2 = poly.lsgt3->u2;
          prims.gt3->v2 = poly.lsgt3->v2;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          c.r = poly.lsgt3->colour[0].r >> 1;
          c.g = poly.lsgt3->colour[0].g >> 1;
          c.b = poly.lsgt3->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt3->colour0);

          c.r = poly.lsgt3->colour[1].r >> 1;
          c.g = poly.lsgt3->colour[1].g >> 1;
          c.b = poly.lsgt3->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt3->colour1);

          c.r = poly.lsgt3->colour[2].r >> 1;
          c.g = poly.lsgt3->colour[2].g >> 1;
          c.b = poly.lsgt3->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt3->colour2);

          prims.gt3->xy0.vx = screenPtr[coord0].vx;
          prims.gt3->xy0.vy = screenPtr[coord0].vy;
          prims.gt3->xy0.vz = depthPtr[coord0];
          prims.gt3->xy1.vx = screenPtr[coord1].vx;
          prims.gt3->xy1.vy = screenPtr[coord1].vy;
          prims.gt3->xy1.vz = depthPtr[coord1];
          prims.gt3->xy2.vx = screenPtr[coord2].vx;
          prims.gt3->xy2.vy = screenPtr[coord2].vy;
          prims.gt3->xy2.vz = depthPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt3 += 1;
        }
      }

      poly.lsgt3 += 1;
      break;

    case TypeLSGT4:

      coord0 = poly.lsgt4->coords[0];
      coord1 = poly.lsgt4->coords[1];
      coord2 = poly.lsgt4->coords[2];
      coord3 = poly.lsgt4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyGT4((POLY_GT4*)prims.ptr);

          prims.gt4->cba = poly.lsgt4->cba;
          prims.gt4->tsb = poly.lsgt4->tsb;

          prims.gt4->u0 = poly.lsgt4->u0;
          prims.gt4->v0 = poly.lsgt4->v0;
          prims.gt4->u1 = poly.lsgt4->u1;
          prims.gt4->v1 = poly.lsgt4->v1;
          prims.gt4->u2 = poly.lsgt4->u2;
          prims.gt4->v2 = poly.lsgt4->v2;
          prims.gt4->u3 = poly.lsgt4->u3;
          prims.gt4->v3 = poly.lsgt4->v3;

          /* Light sourcing */

          RotTrans(&normal[poly.lsgt4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsgt4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsgt4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsgt4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          c.r = poly.lsgt4->colour[0].r >> 1;
          c.g = poly.lsgt4->colour[0].g >> 1;
          c.b = poly.lsgt4->colour[0].b >> 1;
          NormalColorCol(&nsv0, &c, &prims.gt4->colour0);

          c.r = poly.lsgt4->colour[1].r >> 1;
          c.g = poly.lsgt4->colour[1].g >> 1;
          c.b = poly.lsgt4->colour[1].b >> 1;
          NormalColorCol(&nsv1, &c, &prims.gt4->colour1);

          c.r = poly.lsgt4->colour[2].r >> 1;
          c.g = poly.lsgt4->colour[2].g >> 1;
          c.b = poly.lsgt4->colour[2].b >> 1;
          NormalColorCol(&nsv2, &c, &prims.gt4->colour2);

          c.r = poly.lsgt4->colour[3].r >> 1;
          c.g = poly.lsgt4->colour[3].g >> 1;
          c.b = poly.lsgt4->colour[3].b >> 1;
          NormalColorCol(&nsv3, &c, &prims.gt4->colour3);

          prims.gt4->xy0.vx = screenPtr[coord0].vx;
          prims.gt4->xy0.vy = screenPtr[coord0].vy;
          prims.gt4->xy0.vz = depthPtr[coord0];
          prims.gt4->xy1.vx = screenPtr[coord1].vx;
          prims.gt4->xy1.vy = screenPtr[coord1].vy;
          prims.gt4->xy1.vz = depthPtr[coord1];
          prims.gt4->xy2.vx = screenPtr[coord2].vx;
          prims.gt4->xy2.vy = screenPtr[coord2].vy;
          prims.gt4->xy2.vz = depthPtr[coord2];
          prims.gt4->xy3.vx = screenPtr[coord3].vx;
          prims.gt4->xy3.vy = screenPtr[coord3].vy;
          prims.gt4->xy3.vz = depthPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.gt4 += 1;
        }
      }

      poly.lsgt4 += 1;
      break;

    case TypeLSG3:

      coord0 = poly.lsg3->coords[0];
      coord1 = poly.lsg3->coords[1];
      coord2 = poly.lsg3->coords[2];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG3((POLY_G3*)prims.ptr);

#if 0
					prims.g3->colour0.r = 230;      // PCwipeout
					prims.g3->colour0.g = 0;
					prims.g3->colour0.b = 0;
					prims.g3->colour1 = poly.lsg3->colour[1];
					prims.g3->colour2 = poly.lsg3->colour[2];
#else
          /* Light sourcing */

          RotTrans(&normal[poly.lsg3->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg3->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg3->normals[2]], &nv2, &flagN2);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          NormalColorCol(&nsv0, &poly.lsg3->colour[0], &cv);
          prims.g3->colour0.r = poly.lsg3->colour[0].r; // PCwipeout
          prims.g3->colour0.g = 0;
          prims.g3->colour0.b = 0;

          NormalColorCol(&nsv1, &poly.lsg3->colour[1], &prims.g3->colour1);
          NormalColorCol(&nsv2, &poly.lsg3->colour[2], &prims.g3->colour2);
#endif

          prims.g3->xy0 = screenPtr[coord0];
          prims.g3->xy1 = screenPtr[coord1];
          prims.g3->xy2 = screenPtr[coord2];

          depth = AverageZ3(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g3 += 1;
        }
      }

      poly.lsg3 += 1;
      break;

    case TypeLSG4:

      coord0 = poly.lsg4->coords[0];
      coord1 = poly.lsg4->coords[1];
      coord2 = poly.lsg4->coords[2];
      coord3 = poly.lsg4->coords[3];

      if (!((flagsPtr[coord0] |
             flagsPtr[coord1] |
             flagsPtr[coord2] |
             flagsPtr[coord3]) &
            ClipFlag)) {
        if (NormalClip(screenPtr[coord0],
                       screenPtr[coord1],
                       screenPtr[coord2]) > 0) {
          SetPolyG4((POLY_G4*)prims.ptr);

#if 0
					prims.g4->colour0.r = 230;      // PCwipeout
					prims.g4->colour0.g = 0;
					prims.g4->colour0.b = 0;
					prims.g4->colour1 = poly.lsg4->colour[1];
					prims.g4->colour2 = poly.lsg4->colour[2];
					prims.g4->colour3 = poly.lsg4->colour[3];
#else
          /* Light sourcing */

          RotTrans(&normal[poly.lsg4->normals[0]], &nv0, &flagN0);
          RotTrans(&normal[poly.lsg4->normals[1]], &nv1, &flagN1);
          RotTrans(&normal[poly.lsg4->normals[2]], &nv2, &flagN2);
          RotTrans(&normal[poly.lsg4->normals[3]], &nv3, &flagN3);

          nsv0.vx = nv0.vx;
          nsv0.vy = nv0.vy;
          nsv0.vz = nv0.vz;

          nsv1.vx = nv1.vx;
          nsv1.vy = nv1.vy;
          nsv1.vz = nv1.vz;

          nsv2.vx = nv2.vx;
          nsv2.vy = nv2.vy;
          nsv2.vz = nv2.vz;

          nsv3.vx = nv3.vx;
          nsv3.vy = nv3.vy;
          nsv3.vz = nv3.vz;

          NormalColorCol(&nsv0, &poly.lsg4->colour[0], &cv);
          prims.g4->colour0.r = poly.lsg4->colour[0].r; // PCwipeout
          prims.g4->colour0.g = 0;
          prims.g4->colour0.b = 0;

          NormalColorCol(&nsv1, &poly.lsg4->colour[1], &prims.g4->colour1);
          NormalColorCol(&nsv2, &poly.lsg4->colour[2], &prims.g4->colour2);
          NormalColorCol(&nsv3, &poly.lsg4->colour[3], &prims.g4->colour3);
#endif

          prims.g4->xy0 = screenPtr[coord0];
          prims.g4->xy1 = screenPtr[coord1];
          prims.g4->xy2 = screenPtr[coord2];
          prims.g4->xy3 = screenPtr[coord3];

          depth = AverageZ4(depthPtr[coord0],
                            depthPtr[coord1],
                            depthPtr[coord2],
                            depthPtr[coord3]);

          AddPrim(OT[CurrentScreen], depth, (P_TAG*)prims.ptr);
          prims.g4 += 1;
        }
      }

      poly.lsg4 += 1;
      break;
    default:

      printf("Bad primitive type %d\n", poly.prim->type);

      break;
    }
  }
}
