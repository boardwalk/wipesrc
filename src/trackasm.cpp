/* (C) Psygnosis 1994. By Jason Carl Denton
*/

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include "standard.h"
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ttf.h"
#include "trackasm.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "asmtrk.h"
#include "libgte.h"
#include "ajs.h"

#define UseFastRam 1

void Test16Bit(int32_t* Test16bitX, int32_t* Test16bitY, int32_t* Test16bitZ);

#define DepthShift 2
#define DepthMod 0

extern char TrackDirection(TrackCamera* camera);
extern char WhitePal;
extern char DepthFadeTble;
extern int16_t Shielded;
char str[40];

Surface TrackSurface[24][2]; /* 20k */

PolyFT4* TrackLo[2];
PolyFT4* TrackMed[2];
PolyFT4* TrackHi[2];

int16_t TrackLoCount = 0;
int16_t TrackMedCount = 0;
int16_t TrackHiCount = 0; /* 82k total */

PolyFT4 TrackLo0[TrackLoTiles]; /* 17k */
PolyFT4 TrackLo1[TrackLoTiles];
/* 9k */
PolyFT4 TrackMed0[TrackMedTiles];
PolyFT4 TrackMed1[TrackMedTiles];
/* 36k */
PolyFT4 TrackHi0[TrackHiTiles];
PolyFT4 TrackHi1[TrackHiTiles];
extern int16_t startLinePos[];

/* other bss stuff */
/* sound                                        21k */
/* display text                                 20k */
/* ot                                           8k */

/* Primitives for lo res track
Assuming max of 32 sections by 8 faces
which is 256 polys in total
*/

#define ClipFlags (CrtClipped | ZLarge | ZNegative | ZSmall)
#define ShortClipFlags ((CrtClipped | ZLarge | ZNegative | ZSmall) >> 12)
#define MaxJumps 10

void AsmLowResTile(
    TexTemplate* Template,
    int16_t index,
    int16_t reversed) {
  Tile* tile;
  Texture* texture;

  /* Create Tile */

  texture = TextureTable[Template[index].tex1[0]];

  tile = &TrackSurface[index][reversed].low[0];

  tile->cba = texture->cba;
  tile->tsb = texture->tsb;

  if (reversed) {
    tile->u0 = texture->u1;
    tile->u1 = texture->u0;
    tile->u2 = texture->u3;
    tile->u3 = texture->u2;

    tile->v0 = texture->v1;
    tile->v1 = texture->v0;
    tile->v2 = texture->v3;
    tile->v3 = texture->v2;
  } else {
    tile->u0 = texture->u0;
    tile->u1 = texture->u1;
    tile->u2 = texture->u2;
    tile->u3 = texture->u3;

    tile->v0 = texture->v0;
    tile->v1 = texture->v1;
    tile->v2 = texture->v2;
    tile->v3 = texture->v3;
  }
}

void AsmMediumResTile(
    TexTemplate* Template,
    int16_t index,
    int16_t reversed) {
  Tile* tile;
  Texture* texture;
  int16_t i;
  int16_t tex;

  /* Create Tile */

  for (i = 0; i < 4; i++) {
    if (reversed) {
      tex = (i & 2) + 1 - (i & 1);
    } else {
      tex = i;
    }

    texture = TextureTable[Template[index].tex4[tex]];
    tile = &TrackSurface[index][reversed].med[i];

    tile->cba = texture->cba;
    tile->tsb = texture->tsb;

    if (reversed) {
      tile->u0 = texture->u1;
      tile->u1 = texture->u0;
      tile->u2 = texture->u3;
      tile->u3 = texture->u2;

      tile->v0 = texture->v1;
      tile->v1 = texture->v0;
      tile->v2 = texture->v3;
      tile->v3 = texture->v2;
    } else {
      tile->u0 = texture->u0;
      tile->u1 = texture->u1;
      tile->u2 = texture->u2;
      tile->u3 = texture->u3;

      tile->v0 = texture->v0;
      tile->v1 = texture->v1;
      tile->v2 = texture->v2;
      tile->v3 = texture->v3;
    }
  }
}

void AsmHighResTile(
    TexTemplate* Template,
    int16_t index,
    int16_t reversed) {
  Tile* tile;
  Texture* texture;
  int16_t i;
  int16_t tex;

  /* Create Tile */

  for (i = 0; i < 16; i++) {
    if (reversed) {
      tex = (3 - (i & 3)) + (i & 12);
    } else {
      tex = i;
    }

    texture = TextureTable[Template[index].tex16[tex]];
    tile = &TrackSurface[index][reversed].high[i];

    tile->cba = texture->cba;
    tile->tsb = texture->tsb;

    if (reversed) {
      tile->u0 = texture->u1;
      tile->u1 = texture->u0;
      tile->u2 = texture->u3;
      tile->u3 = texture->u2;

      tile->v0 = texture->v1;
      tile->v1 = texture->v0;
      tile->v2 = texture->v3;
      tile->v3 = texture->v2;
    } else {
      tile->u0 = texture->u0;
      tile->u1 = texture->u1;
      tile->u2 = texture->u2;
      tile->u3 = texture->u3;

      tile->v0 = texture->v0;
      tile->v1 = texture->v1;
      tile->v2 = texture->v2;
      tile->v3 = texture->v3;
    }
  }
}

void AsmGenerateTiles(
    TexTemplate* texTemplate,
    int16_t numLib) {
  int16_t i;
  PolyFT4* ft4;

  for (i = 0; i < numLib; i++) {
    AsmLowResTile(texTemplate, i, 0);
    AsmLowResTile(texTemplate, i, 1);

    AsmMediumResTile(texTemplate, i, 0);
    AsmMediumResTile(texTemplate, i, 1);

    AsmHighResTile(texTemplate, i, 0);
    AsmHighResTile(texTemplate, i, 1);
  }

  TrackLo[0] = TrackLo0;
  TrackLo[1] = TrackLo1;

  TrackMed[0] = TrackMed0;
  TrackMed[1] = TrackMed1;

  TrackHi[0] = TrackHi0;
  TrackHi[1] = TrackHi1;

  for (i = 0; i < TrackLoTiles; i++) {
    ft4 = &TrackLo0[i];
    SetPolyFT4((POLY_FT4*)ft4);

    ft4 = &TrackLo1[i];
    SetPolyFT4((POLY_FT4*)ft4);
  }

  for (i = 0; i < TrackMedTiles; i++) {
    ft4 = &TrackMed0[i];
    SetPolyFT4((POLY_FT4*)ft4);

    ft4 = &TrackMed1[i];
    SetPolyFT4((POLY_FT4*)ft4);
  }

  for (i = 0; i < TrackHiTiles; i++) {
    ft4 = &TrackHi0[i];
    SetPolyFT4((POLY_FT4*)ft4);

    ft4 = &TrackHi1[i];
    SetPolyFT4((POLY_FT4*)ft4);
  }
}

void AsmNextFrame() {
  /* Initialise lo track polygon counter */
  TrackLoCount = 0;

  /* Initialise med track polygon counter */
  TrackMedCount = 0;

  /* Initialise med track polygon counter */
  TrackHiCount = 0;

  /* Initialise mesh memory pointer */
  if (CurrentScreen == 0) {
    MeshPtr = MeshRam0;
  } else {
    MeshPtr = MeshRam1;
  }
}

TrackSection* AsmTransformTrackLo(TrackSection* firstSection,
                                  Skeleton* camera, int16_t loCount, int16_t dir) {
  /*
Variables with greatest need for optimisation
Listed in decreasing order of priority
	*/

  VECTOR* vertex;

#if UseFastRam
  VECTOR* campos = (VECTOR*)(FastRam); /* 1 vector = 16 bytes */
  SVECTOR* svector = (SVECTOR*)(campos + 1); /* 4 svectors = 32 bytes */
  DVECTOR* screen = (DVECTOR*)(svector + 4); /* 4 dvectors = 16 bytes */
  int32_t* depth = (int32_t*)(screen + 4); /* 4 longs = 16 bytes */
  int32_t* flag = (int32_t*)(depth + 4); /* 4 longs = 16 bytes */
  int32_t* interpolate = (int32_t*)(flag + 4); /* 4 longs = 16 bytes */
#else
  VECTOR camposData;
  VECTOR* campos = &camposData;
  SVECTOR svector[4];
  DVECTOR screen[4];
  int32_t depth[4];
  int32_t flag[4];
  int32_t interpolate[4];
#endif

  uint16_t* depth2 = (uint16_t*)(interpolate + 4); /* 9 shorts = 18 bytes */
  uint16_t* flag2 = (uint16_t*)(depth2 + 4); /* 9 shorts = 18 bytes */

  int16_t i, j;

  TrackSection* section;
  Face* face;

  VECTOR* vertices;
  int16_t faceCount;

  PolyFT4* ft4;

  Surface* surface;

  Tile* tile;

  int32_t Test16bitX;
  int32_t Test16bitY;
  int32_t Test16bitZ;
  int16_t var;

  /* Set transformation matrix */

  SetSkeletonTransform(camera->super);

  /* Copy camera position into fast ram */

  campos->vx = camera->relative.t[0];
  campos->vy = camera->relative.t[1];
  campos->vz = camera->relative.t[2];

  /* Calculate non-meshed track sections */

  vertices = track->vertices;
  section = firstSection;

  for (i = 0; i < loCount; i++) {
    face = track->faces + section->faceStart;
    faceCount = section->faceCount;

    for (j = 0; j < faceCount; j++) {
      vertex = vertices + face->vertex[0];
      Test16bitX = vertex->vx + campos->vx;
      Test16bitY = vertex->vy + campos->vy;
      Test16bitZ = vertex->vz + campos->vz;
      Test16Bit(&Test16bitX, &Test16bitY, &Test16bitZ);

      svector[0].vx = (int16_t)Test16bitX;
      svector[0].vy = (int16_t)Test16bitY;
      svector[0].vz = (int16_t)Test16bitZ;

      vertex = vertices + face->vertex[1];
      Test16bitX = vertex->vx + campos->vx;
      Test16bitY = vertex->vy + campos->vy;
      Test16bitZ = vertex->vz + campos->vz;
      Test16Bit(&Test16bitX, &Test16bitY, &Test16bitZ);

      svector[1].vx = (int16_t)Test16bitX;
      svector[1].vy = (int16_t)Test16bitY;
      svector[1].vz = (int16_t)Test16bitZ;

      vertex = vertices + face->vertex[2];
      Test16bitX = vertex->vx + campos->vx;
      Test16bitY = vertex->vy + campos->vy;
      Test16bitZ = vertex->vz + campos->vz;
      Test16Bit(&Test16bitX, &Test16bitY, &Test16bitZ);

      svector[2].vx = (int16_t)Test16bitX;
      svector[2].vy = (int16_t)Test16bitY;
      svector[2].vz = (int16_t)Test16bitZ;

      vertex = vertices + face->vertex[3];
      Test16bitX = vertex->vx + campos->vx;
      Test16bitY = vertex->vy + campos->vy;
      Test16bitZ = vertex->vz + campos->vz;
      Test16Bit(&Test16bitX, &Test16bitY, &Test16bitZ);

      svector[3].vx = (int16_t)Test16bitX;
      svector[3].vy = (int16_t)Test16bitY;
      svector[3].vz = (int16_t)Test16bitZ;

      RotTransPersN(svector, screen, depth2, flag2, 4);

      if (face->flags & TRACK_BASE)
        var = 1;
      else
        var = 0;

      //		if ( !( *flag & ClipFlags ) )
      if (!((flag2[0] | flag2[1] | flag2[2] | flag2[3]) & ShortClipFlags)) {
        ft4 = &TrackLo[CurrentScreen][TrackLoCount++];
        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture)
          surface = &TrackSurface[face->texture][1];
        else
          surface = &TrackSurface[face->texture][0];

        tile = &surface->low[0];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        if (NormalClip(screen[0], screen[1], screen[2]) < 0) {
          ft4->u0 = tile->u0;
          ft4->v0 = tile->v0;
          ft4->u1 = tile->u1;
          ft4->v1 = tile->v1;
          ft4->u2 = tile->u2;
          ft4->v2 = tile->v2;
          ft4->u3 = tile->u3;
          ft4->v3 = tile->v3;

          switch (dir) {
          case ShipForwards:
            screen[0].vy -= var;
            screen[1].vy -= var;
            break;
          case ShipBackwards:
            screen[0].vy += var;
            screen[1].vy += var;
            break;
          case ShipRight:
            screen[0].vy += var;
            screen[3].vy += var;
            break;
          case ShipLeft:
            screen[0].vy -= var;
            screen[3].vy -= var;
            break;
          }

          ft4->xy0.vx = screen[1].vx;
          ft4->xy0.vy = screen[1].vy;
          ft4->xy0.vz = depth2[1];
          ft4->xy1.vx = screen[0].vx;
          ft4->xy1.vy = screen[0].vy;
          ft4->xy1.vz = depth2[0];
          ft4->xy2.vx = screen[2].vx;
          ft4->xy2.vy = screen[2].vy;
          ft4->xy2.vz = depth2[2];
          ft4->xy3.vx = screen[3].vx;
          ft4->xy3.vy = screen[3].vy;
          ft4->xy3.vz = depth2[3];
          ft4->xy0.pad = 0;
          ft4->xy1.pad = 0;
          ft4->xy2.pad = 0;
          ft4->xy3.pad = 0;
          //ft4->xy1 = screen[ 0 ];
          //ft4->xy2 = screen[ 2 ];
          //ft4->xy3 = screen[ 3 ];
        } else {
          ft4->u1 = tile->u1;
          ft4->v1 = tile->v1;
          ft4->u0 = tile->u0;
          ft4->v0 = tile->v0;
          ft4->u3 = tile->u3;
          ft4->v3 = tile->v3;
          ft4->u2 = tile->u2;
          ft4->v2 = tile->v2;

          switch (dir) {
          case ShipForwards:
            screen[0].vy -= var;
            screen[1].vy -= var;
            break;
          case ShipBackwards:
            screen[0].vy += var;
            screen[1].vy += var;
            break;
          case ShipRight:
            screen[0].vy += var;
            screen[3].vy += var;
            break;
          case ShipLeft:
            screen[0].vy -= var;
            screen[3].vy -= var;
            break;
          }

          //ft4->xy1 = screen[ 1 ];
          //ft4->xy0 = screen[ 0 ];
          //ft4->xy3 = screen[ 2 ];
          //ft4->xy2 = screen[ 3 ];
          ft4->xy1.vx = screen[1].vx;
          ft4->xy1.vy = screen[1].vy;
          ft4->xy1.vz = depth2[1];
          ft4->xy0.vx = screen[0].vx;
          ft4->xy0.vy = screen[0].vy;
          ft4->xy0.vz = depth2[0];
          ft4->xy3.vx = screen[2].vx;
          ft4->xy3.vy = screen[2].vy;
          ft4->xy3.vz = depth2[2];
          ft4->xy2.vx = screen[3].vx;
          ft4->xy2.vy = screen[3].vy;
          ft4->xy2.vz = depth2[3];
          ft4->xy0.pad = 0;
          ft4->xy1.pad = 0;
          ft4->xy2.pad = 0;
          ft4->xy3.pad = 0;
        }

        /* Just use one corner for the depth info */

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //            AddPrim(OT[ CurrentScreen ],  ( *depth ), ( P_TAG* ) ft4 );
        AddPrim(OT[CurrentScreen], ((depth2[0] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      }

      face++;
    }

    section = section->nextSection.ptr;
  }

  return section;
}

TrackSection* AsmTransformTrackMed(
    TrackSection* firstSection,
    Skeleton* camera,
    int16_t medCount,
    int16_t dir) {
  /*
Variables with greatest need for optimisation
Listed in decreasing order of priority
	*/

#if UseFastRam
  VECTOR* campos = (VECTOR*)(FastRam); /* 1 vector = 16 bytes */
  VECTOR* vector = (VECTOR*)(campos + 1); /* 9 vectors = 144 bytes */
  SVECTOR* svector = (SVECTOR*)(vector + 9); /* 9 svectors = 72 bytes */
  DVECTOR* screen = (DVECTOR*)(svector + 9); /* 9 dvectors = 36 bytes */
  uint16_t* depth = (uint16_t*)(screen + 9); /* 9 shorts = 18 bytes */
  uint16_t* flag = (uint16_t*)(depth + 9); /* 9 shorts = 18 bytes */
#else
  VECTOR camposData;
  VECTOR* campos = &camposData;
  SVECTOR svector[9];
  DVECTOR screen[9];
  uint16_t depth[9];
  uint16_t flag[9];
  VECTOR meshVector[9];
#endif

  /* original 32 bit vertices */

  int16_t i, j;

  TrackSection* section;
  Face* face;

  VECTOR* vertices;
  int16_t faceCount;

  PolyFT4* ft4;

  Surface* surface;

  Tile* tile;
  int16_t var;

  /* Set transformation matrix */

  SetSkeletonTransform(camera->super);

  /* Copy camera position into fast ram */

  campos->vx = camera->relative.t[0];
  campos->vy = camera->relative.t[1];
  campos->vz = camera->relative.t[2];

  /* Calculate Med res meshed track sections */

  vertices = track->vertices;
  section = firstSection;

  for (i = 0; i < medCount; i++) {
    face = track->faces + section->faceStart;
    faceCount = section->faceCount;

    for (j = 0; j < faceCount; j++) {
      /* get original 32 bit faces coords */

      vector[0] = *(vertices + face->vertex[1]);
      vector[2] = *(vertices + face->vertex[0]);
      vector[6] = *(vertices + face->vertex[2]);
      vector[8] = *(vertices + face->vertex[3]);

      /* mesh top */

      vector[1].vx = (vector[0].vx + vector[2].vx) >> 1;
      vector[1].vy = (vector[0].vy + vector[2].vy) >> 1;
      vector[1].vz = (vector[0].vz + vector[2].vz) >> 1;

      /* mesh left */

      vector[3].vx = (vector[0].vx + vector[6].vx) >> 1;
      vector[3].vy = (vector[0].vy + vector[6].vy) >> 1;
      vector[3].vz = (vector[0].vz + vector[6].vz) >> 1;

      /* mesh right */

      vector[5].vx = (vector[2].vx + vector[8].vx) >> 1;
      vector[5].vy = (vector[2].vy + vector[8].vy) >> 1;
      vector[5].vz = (vector[2].vz + vector[8].vz) >> 1;

      /* mesh bottom */

      vector[7].vx = (vector[6].vx + vector[8].vx) >> 1;
      vector[7].vy = (vector[6].vy + vector[8].vy) >> 1;
      vector[7].vz = (vector[6].vz + vector[8].vz) >> 1;

      /* mesh centre */

      vector[4].vx = (vector[3].vx + vector[5].vx) >> 1;
      vector[4].vy = (vector[3].vy + vector[5].vy) >> 1;
      vector[4].vz = (vector[3].vz + vector[5].vz) >> 1;

      /* make SVECTORs */

      svector[0].vx = vector[0].vx + campos->vx;
      svector[0].vy = vector[0].vy + campos->vy;
      svector[0].vz = vector[0].vz + campos->vz;

      svector[1].vx = vector[1].vx + campos->vx;
      svector[1].vy = vector[1].vy + campos->vy;
      svector[1].vz = vector[1].vz + campos->vz;

      svector[2].vx = vector[2].vx + campos->vx;
      svector[2].vy = vector[2].vy + campos->vy;
      svector[2].vz = vector[2].vz + campos->vz;

      svector[3].vx = vector[3].vx + campos->vx;
      svector[3].vy = vector[3].vy + campos->vy;
      svector[3].vz = vector[3].vz + campos->vz;

      svector[4].vx = vector[4].vx + campos->vx;
      svector[4].vy = vector[4].vy + campos->vy;
      svector[4].vz = vector[4].vz + campos->vz;

      svector[5].vx = vector[5].vx + campos->vx;
      svector[5].vy = vector[5].vy + campos->vy;
      svector[5].vz = vector[5].vz + campos->vz;

      svector[6].vx = vector[6].vx + campos->vx;
      svector[6].vy = vector[6].vy + campos->vy;
      svector[6].vz = vector[6].vz + campos->vz;

      svector[7].vx = vector[7].vx + campos->vx;
      svector[7].vy = vector[7].vy + campos->vy;
      svector[7].vz = vector[7].vz + campos->vz;

      svector[8].vx = vector[8].vx + campos->vx;
      svector[8].vy = vector[8].vy + campos->vy;
      svector[8].vz = vector[8].vz + campos->vz;

      RotTransPersN(svector, screen, depth, flag, 9);

      /* Top left polygon */

      if (!((flag[0] | flag[1] | flag[3] | flag[4]) & ShortClipFlags)) {
        ft4 = &TrackMed[CurrentScreen][TrackMedCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }
        if (face->flags & TRACK_BASE)
          var = 1;
        else
          var = 0;
        tile = &surface->med[0]; // PCwipeout, was 2

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;
        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;
        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;
        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        switch (dir) {
        case ShipForwards:
          screen[0].vy -= var;
          screen[1].vy -= var;
          break;
        case ShipBackwards:
          screen[0].vy += var;
          screen[1].vy += var;
          break;
        case ShipRight:
          screen[0].vy += var;
          screen[3].vy += var;
          break;
        case ShipLeft:
          screen[0].vy -= var;
          screen[3].vy -= var;
          break;
        }
        //ft4->xy0 = screen[ 0 ] ;
        //ft4->xy1 = screen[ 1 ] ;
        //ft4->xy2 = screen[ 3 ];
        //ft4->xy3 = screen[ 4 ];
        ft4->xy0.vx = screen[0].vx;
        ft4->xy0.vy = screen[0].vy;
        ft4->xy0.vz = depth[0];
        ft4->xy1.vx = screen[1].vx;
        ft4->xy1.vy = screen[1].vy;
        ft4->xy1.vz = depth[1];
        ft4->xy2.vx = screen[3].vx;
        ft4->xy2.vy = screen[3].vy;
        ft4->xy2.vz = depth[3];
        ft4->xy3.vx = screen[4].vx;
        ft4->xy3.vy = screen[4].vy;
        ft4->xy3.vz = depth[4];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        AddPrim(OT[CurrentScreen], ((depth[0] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      }

      /* Top right polygon */

      if (!((flag[1] | flag[2] | flag[4] | flag[5]) & ShortClipFlags)) {
        ft4 = &TrackMed[CurrentScreen][TrackMedCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->med[1]; // PCwipeout, was 3

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;
        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;
        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;
        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        switch (dir) {
        case ShipForwards:
          screen[2].vy -= var;
          screen[1].vy -= var;
          break;
        case ShipBackwards:
          screen[2].vy += var;
          screen[1].vy += var;
          break;
        case ShipRight:
          screen[2].vy -= var;
          screen[5].vy -= var;
          break;
        case ShipLeft:
          screen[3].vy += var;
          screen[5].vy += var;
          break;
        }
        //ft4->xy0 = screen[ 1 ];
        //ft4->xy1 = screen[ 2 ];
        //ft4->xy2 = screen[ 4 ];
        //ft4->xy3 = screen[ 5 ];
        ft4->xy0.vx = screen[1].vx;
        ft4->xy0.vy = screen[1].vy;
        ft4->xy0.vz = depth[1];
        ft4->xy1.vx = screen[2].vx;
        ft4->xy1.vy = screen[2].vy;
        ft4->xy1.vz = depth[2];
        ft4->xy2.vx = screen[4].vx;
        ft4->xy2.vy = screen[4].vy;
        ft4->xy2.vz = depth[4];
        ft4->xy3.vx = screen[5].vx;
        ft4->xy3.vy = screen[5].vy;
        ft4->xy3.vz = depth[5];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;
        AddPrim(OT[CurrentScreen], (depth[1] >> DepthShift) - DepthMod, (P_TAG*)ft4);
      }

      /* Bottom left polygon */

      if (!((flag[3] | flag[4] | flag[6] | flag[7]) & ShortClipFlags)) {
        ft4 = &TrackMed[CurrentScreen][TrackMedCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->med[2]; // PCwipeout, was 0

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;
        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;
        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;
        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        switch (dir) {
        case ShipForwards:
          screen[6].vy += var;
          screen[7].vy += var;
          break;
        case ShipBackwards:
          screen[6].vy -= var;
          screen[7].vy -= var;
          break;
        case ShipRight:
          screen[6].vy += var;
          screen[3].vy += var;
          break;
        case ShipLeft:
          screen[6].vy -= var;
          screen[3].vy -= var;
          break;
        }

        //ft4->xy0 = screen[ 3 ];
        //ft4->xy1 = screen[ 4 ];
        //ft4->xy2 = screen[ 6 ];
        //ft4->xy3 = screen[ 7 ];
        ft4->xy0.vx = screen[3].vx;
        ft4->xy0.vy = screen[3].vy;
        ft4->xy0.vz = depth[3];
        ft4->xy1.vx = screen[4].vx;
        ft4->xy1.vy = screen[4].vy;
        ft4->xy1.vz = depth[4];
        ft4->xy2.vx = screen[6].vx;
        ft4->xy2.vy = screen[6].vy;
        ft4->xy2.vz = depth[6];
        ft4->xy3.vx = screen[7].vx;
        ft4->xy3.vy = screen[7].vy;
        ft4->xy3.vz = depth[7];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        AddPrim(OT[CurrentScreen], ((depth[3] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      }

      /* Bottom right polygon */

      if (!((flag[4] | flag[5] | flag[7] | flag[8]) & ShortClipFlags)) {
        ft4 = &TrackMed[CurrentScreen][TrackMedCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->med[3]; // PCwipeout, was 1

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;
        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;
        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;
        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        switch (dir) {
        case ShipForwards:
          screen[7].vy += var;
          screen[8].vy += var;
          break;
        case ShipBackwards:
          screen[7].vy -= var;
          screen[8].vy -= var;
          break;
        case ShipRight:
          screen[5].vy -= var;
          screen[8].vy -= var;
          break;
        case ShipLeft:
          screen[5].vy += var;
          screen[8].vy += var;
          break;
        }
        //ft4->xy0 = screen[ 4 ];
        //ft4->xy1 = screen[ 5 ];
        //ft4->xy2 = screen[ 7 ];
        //ft4->xy3 = screen[ 8 ];
        ft4->xy0.vx = screen[4].vx;
        ft4->xy0.vy = screen[4].vy;
        ft4->xy0.vz = depth[4];
        ft4->xy1.vx = screen[5].vx;
        ft4->xy1.vy = screen[5].vy;
        ft4->xy1.vz = depth[5];
        ft4->xy2.vx = screen[7].vx;
        ft4->xy2.vy = screen[7].vy;
        ft4->xy2.vz = depth[7];
        ft4->xy3.vx = screen[8].vx;
        ft4->xy3.vy = screen[8].vy;
        ft4->xy3.vz = depth[8];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        AddPrim(OT[CurrentScreen], ((depth[4] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      }
      face++;
    }

    section = section->nextSection.ptr;
  }
  return section;
}
#undef ShortClipFlags
#define ShortClipFlags 0

TrackSection* AsmTransformTrackHi(
    TrackSection* firstSection,
    Skeleton* camera,
    AutoMeshData* autoMesh,
    int16_t highCount,
    int16_t var) {
  /*
Variables with greatest need for optimisation
Listed in decreasing order of priority
	*/

#if 0 //JJS UseFastRam
	VECTOR*              campos = ( VECTOR* )( FastRam );                /* 1 vector = 16 bytes */
	VECTOR*              vector = ( VECTOR* )( campos + 1 );             /* 25 vectors = 400 bytes */
	SVECTOR*             svector = ( SVECTOR* )( vector + 25 );          /* 25 svectors = 200 bytes */
	DVECTOR*             screen = ( DVECTOR* )( svector + 25 );          /* 25 dvectors = 100 bytes */
	uint16_t*              depth = ( uint16_t* )( screen + 25 );             /* 25 shorts = 50 bytes */
	uint16_t*              flag = ( uint16_t* )( depth + 25 );               /* 25 shorts = 50 bytes */
#else
  VECTOR camposData;
  VECTOR* campos = &camposData;
  VECTOR vector[25];
  SVECTOR svector[25];
  DVECTOR screen[25];
  uint16_t depth[25];
  uint16_t flag[25];
#endif

  /* original 32 bit vertices */

  int16_t i, j;

  TrackSection* section;
  Face* face;

  VECTOR* vertices;
  int16_t faceCount;

  PolyFT4* ft4;

  Surface* surface;

  Tile* tile;

  /* Set transformation matrix */

  SetSkeletonTransform(camera->super);

  /* Copy camera position into fast ram */

  campos->vx = camera->relative.t[0];
  campos->vy = camera->relative.t[1];
  campos->vz = camera->relative.t[2];

  /* Calculate Hi res meshed track sections */

  vertices = track->vertices;
  section = firstSection;

  for (i = 0; i < highCount; i++) {
    face = track->faces + section->faceStart;
    faceCount = section->faceCount;

    for (j = 0; j < faceCount; j++) {
      /* get original 32 bit faces coords */

      vector[0] = *(vertices + face->vertex[1]);
      vector[4] = *(vertices + face->vertex[0]);
      vector[20] = *(vertices + face->vertex[2]);
      vector[24] = *(vertices + face->vertex[3]);

      /* mesh top */

      vector[2].vx = (vector[0].vx + vector[4].vx) >> 1;
      vector[2].vy = (vector[0].vy + vector[4].vy) >> 1;
      vector[2].vz = (vector[0].vz + vector[4].vz) >> 1;

      vector[1].vx = (vector[0].vx + vector[2].vx) >> 1;
      vector[1].vy = (vector[0].vy + vector[2].vy) >> 1;
      vector[1].vz = (vector[0].vz + vector[2].vz) >> 1;

      vector[3].vx = (vector[2].vx + vector[4].vx) >> 1;
      vector[3].vy = (vector[2].vy + vector[4].vy) >> 1;
      vector[3].vz = (vector[2].vz + vector[4].vz) >> 1;

      /* mesh left */

      vector[10].vx = (vector[0].vx + vector[20].vx) >> 1;
      vector[10].vy = (vector[0].vy + vector[20].vy) >> 1;
      vector[10].vz = (vector[0].vz + vector[20].vz) >> 1;

      vector[5].vx = (vector[0].vx + vector[10].vx) >> 1;
      vector[5].vy = (vector[0].vy + vector[10].vy) >> 1;
      vector[5].vz = (vector[0].vz + vector[10].vz) >> 1;

      vector[15].vx = (vector[10].vx + vector[20].vx) >> 1;
      vector[15].vy = (vector[10].vy + vector[20].vy) >> 1;
      vector[15].vz = (vector[10].vz + vector[20].vz) >> 1;

      /* mesh right */

      vector[14].vx = (vector[4].vx + vector[24].vx) >> 1;
      vector[14].vy = (vector[4].vy + vector[24].vy) >> 1;
      vector[14].vz = (vector[4].vz + vector[24].vz) >> 1;

      vector[9].vx = (vector[4].vx + vector[14].vx) >> 1;
      vector[9].vy = (vector[4].vy + vector[14].vy) >> 1;
      vector[9].vz = (vector[4].vz + vector[14].vz) >> 1;

      vector[19].vx = (vector[14].vx + vector[24].vx) >> 1;
      vector[19].vy = (vector[14].vy + vector[24].vy) >> 1;
      vector[19].vz = (vector[14].vz + vector[24].vz) >> 1;

      /* mesh bottom */

      vector[22].vx = (vector[20].vx + vector[24].vx) >> 1;
      vector[22].vy = (vector[20].vy + vector[24].vy) >> 1;
      vector[22].vz = (vector[20].vz + vector[24].vz) >> 1;

      vector[21].vx = (vector[20].vx + vector[22].vx) >> 1;
      vector[21].vy = (vector[20].vy + vector[22].vy) >> 1;
      vector[21].vz = (vector[20].vz + vector[22].vz) >> 1;

      vector[23].vx = (vector[22].vx + vector[24].vx) >> 1;
      vector[23].vy = (vector[22].vy + vector[24].vy) >> 1;
      vector[23].vz = (vector[22].vz + vector[24].vz) >> 1;

      /* mesh centre */

      vector[12].vx = (vector[10].vx + vector[14].vx) >> 1;
      vector[12].vy = (vector[10].vy + vector[14].vy) >> 1;
      vector[12].vz = (vector[10].vz + vector[14].vz) >> 1;

      vector[7].vx = (vector[2].vx + vector[12].vx) >> 1;
      vector[7].vy = (vector[2].vy + vector[12].vy) >> 1;
      vector[7].vz = (vector[2].vz + vector[12].vz) >> 1;

      vector[11].vx = (vector[10].vx + vector[12].vx) >> 1;
      vector[11].vy = (vector[10].vy + vector[12].vy) >> 1;
      vector[11].vz = (vector[10].vz + vector[12].vz) >> 1;

      vector[13].vx = (vector[12].vx + vector[14].vx) >> 1;
      vector[13].vy = (vector[12].vy + vector[14].vy) >> 1;
      vector[13].vz = (vector[12].vz + vector[14].vz) >> 1;

      vector[17].vx = (vector[12].vx + vector[22].vx) >> 1;
      vector[17].vy = (vector[12].vy + vector[22].vy) >> 1;
      vector[17].vz = (vector[12].vz + vector[22].vz) >> 1;

      vector[6].vx = (vector[5].vx + vector[7].vx) >> 1;
      vector[6].vy = (vector[5].vy + vector[7].vy) >> 1;
      vector[6].vz = (vector[5].vz + vector[7].vz) >> 1;

      vector[8].vx = (vector[7].vx + vector[9].vx) >> 1;
      vector[8].vy = (vector[7].vy + vector[9].vy) >> 1;
      vector[8].vz = (vector[7].vz + vector[9].vz) >> 1;

      vector[16].vx = (vector[15].vx + vector[17].vx) >> 1;
      vector[16].vy = (vector[15].vy + vector[17].vy) >> 1;
      vector[16].vz = (vector[15].vz + vector[17].vz) >> 1;

      vector[18].vx = (vector[17].vx + vector[19].vx) >> 1;
      vector[18].vy = (vector[17].vy + vector[19].vy) >> 1;
      vector[18].vz = (vector[17].vz + vector[19].vz) >> 1;

      /* make SVECTORs */

      svector[0].vx = vector[0].vx + campos->vx;
      svector[0].vy = vector[0].vy + campos->vy;
      svector[0].vz = vector[0].vz + campos->vz;

      svector[1].vx = vector[1].vx + campos->vx;
      svector[1].vy = vector[1].vy + campos->vy;
      svector[1].vz = vector[1].vz + campos->vz;

      svector[2].vx = vector[2].vx + campos->vx;
      svector[2].vy = vector[2].vy + campos->vy;
      svector[2].vz = vector[2].vz + campos->vz;

      svector[3].vx = vector[3].vx + campos->vx;
      svector[3].vy = vector[3].vy + campos->vy;
      svector[3].vz = vector[3].vz + campos->vz;

      svector[4].vx = vector[4].vx + campos->vx;
      svector[4].vy = vector[4].vy + campos->vy;
      svector[4].vz = vector[4].vz + campos->vz;

      svector[5].vx = vector[5].vx + campos->vx;
      svector[5].vy = vector[5].vy + campos->vy;
      svector[5].vz = vector[5].vz + campos->vz;

      svector[6].vx = vector[6].vx + campos->vx;
      svector[6].vy = vector[6].vy + campos->vy;
      svector[6].vz = vector[6].vz + campos->vz;

      svector[7].vx = vector[7].vx + campos->vx;
      svector[7].vy = vector[7].vy + campos->vy;
      svector[7].vz = vector[7].vz + campos->vz;

      svector[8].vx = vector[8].vx + campos->vx;
      svector[8].vy = vector[8].vy + campos->vy;
      svector[8].vz = vector[8].vz + campos->vz;

      svector[9].vx = vector[9].vx + campos->vx;
      svector[9].vy = vector[9].vy + campos->vy;
      svector[9].vz = vector[9].vz + campos->vz;

      svector[10].vx = vector[10].vx + campos->vx;
      svector[10].vy = vector[10].vy + campos->vy;
      svector[10].vz = vector[10].vz + campos->vz;

      svector[11].vx = vector[11].vx + campos->vx;
      svector[11].vy = vector[11].vy + campos->vy;
      svector[11].vz = vector[11].vz + campos->vz;

      svector[12].vx = vector[12].vx + campos->vx;
      svector[12].vy = vector[12].vy + campos->vy;
      svector[12].vz = vector[12].vz + campos->vz;

      svector[13].vx = vector[13].vx + campos->vx;
      svector[13].vy = vector[13].vy + campos->vy;
      svector[13].vz = vector[13].vz + campos->vz;

      svector[14].vx = vector[14].vx + campos->vx;
      svector[14].vy = vector[14].vy + campos->vy;
      svector[14].vz = vector[14].vz + campos->vz;

      svector[15].vx = vector[15].vx + campos->vx;
      svector[15].vy = vector[15].vy + campos->vy;
      svector[15].vz = vector[15].vz + campos->vz;

      svector[16].vx = vector[16].vx + campos->vx;
      svector[16].vy = vector[16].vy + campos->vy;
      svector[16].vz = vector[16].vz + campos->vz;

      svector[17].vx = vector[17].vx + campos->vx;
      svector[17].vy = vector[17].vy + campos->vy;
      svector[17].vz = vector[17].vz + campos->vz;

      svector[18].vx = vector[18].vx + campos->vx;
      svector[18].vy = vector[18].vy + campos->vy;
      svector[18].vz = vector[18].vz + campos->vz;

      svector[19].vx = vector[19].vx + campos->vx;
      svector[19].vy = vector[19].vy + campos->vy;
      svector[19].vz = vector[19].vz + campos->vz;

      svector[20].vx = vector[20].vx + campos->vx;
      svector[20].vy = vector[20].vy + campos->vy;
      svector[20].vz = vector[20].vz + campos->vz;

      svector[21].vx = vector[21].vx + campos->vx;
      svector[21].vy = vector[21].vy + campos->vy;
      svector[21].vz = vector[21].vz + campos->vz;

      svector[22].vx = vector[22].vx + campos->vx;
      svector[22].vy = vector[22].vy + campos->vy;
      svector[22].vz = vector[22].vz + campos->vz;

      svector[23].vx = vector[23].vx + campos->vx;
      svector[23].vy = vector[23].vy + campos->vy;
      svector[23].vz = vector[23].vz + campos->vz;

      svector[24].vx = vector[24].vx + campos->vx;
      svector[24].vy = vector[24].vy + campos->vy;
      svector[24].vz = vector[24].vz + campos->vz;

      RotTransPersN(svector, screen, depth, flag, 25);

      /* polygon 0 */
      if (!((flag[0] | flag[1] | flag[5] | flag[6]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[0];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 0 ];
        //ft4->xy1 = screen[ 1 ];
        //ft4->xy2 = screen[ 5 ];
        //ft4->xy3 = screen[ 6 ];
        ft4->xy0.vx = screen[0].vx;
        ft4->xy0.vy = screen[0].vy;
        ft4->xy1.vx = screen[1].vx;
        ft4->xy1.vy = screen[1].vy;
        ft4->xy2.vx = screen[5].vx;
        ft4->xy2.vy = screen[5].vy;
        ft4->xy3.vx = screen[6].vx;
        ft4->xy3.vy = screen[6].vy;
        ft4->xy0.vz = depth[0];
        ft4->xy1.vz = depth[1];
        ft4->xy2.vz = depth[5];
        ft4->xy3.vz = depth[6];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[0] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[0] & flag[1] & flag[5] & flag[6] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[0];

        AsmAutoMesh(&svector[0], &svector[1], &svector[5], &svector[6], face, tile, autoMesh);
      }

      /* polygon 1 */

      if (!((flag[1] | flag[2] | flag[6] | flag[7]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[1];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 1 ];
        //ft4->xy1 = screen[ 2 ];
        //ft4->xy2 = screen[ 6 ];
        //ft4->xy3 = screen[ 7 ];
        ft4->xy0.vx = screen[1].vx;
        ft4->xy0.vy = screen[1].vy;
        ft4->xy1.vx = screen[2].vx;
        ft4->xy1.vy = screen[2].vy;
        ft4->xy2.vx = screen[6].vx;
        ft4->xy2.vy = screen[6].vy;
        ft4->xy3.vx = screen[7].vx;
        ft4->xy3.vy = screen[7].vy;
        ft4->xy0.vz = depth[1];
        ft4->xy1.vz = depth[2];
        ft4->xy2.vz = depth[6];
        ft4->xy3.vz = depth[7];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[1] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[1] & flag[2] & flag[6] & flag[7] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[1];

        AsmAutoMesh(&svector[1], &svector[2], &svector[6], &svector[7], face, tile, autoMesh);
      }

      /* polygon 2 */

      if (!((flag[2] | flag[3] | flag[7] | flag[8]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[2];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 2 ];
        //ft4->xy1 = screen[ 3 ];
        //ft4->xy2 = screen[ 7 ];
        //ft4->xy3 = screen[ 8 ];
        ft4->xy0.vx = screen[2].vx;
        ft4->xy0.vy = screen[2].vy;
        ft4->xy1.vx = screen[3].vx;
        ft4->xy1.vy = screen[3].vy;
        ft4->xy2.vx = screen[7].vx;
        ft4->xy2.vy = screen[7].vy;
        ft4->xy3.vx = screen[8].vx;
        ft4->xy3.vy = screen[8].vy;
        ft4->xy0.vz = depth[2];
        ft4->xy1.vz = depth[3];
        ft4->xy2.vz = depth[7];
        ft4->xy3.vz = depth[8];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[2] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[2] & flag[3] & flag[7] & flag[8] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[2];

        AsmAutoMesh(&svector[2], &svector[3], &svector[7], &svector[8], face, tile, autoMesh);
      }

      /* polygon 3 */

      if (!((flag[3] | flag[4] | flag[8] | flag[9]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[3];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 3 ];
        //ft4->xy1 = screen[ 4 ];
        //ft4->xy2 = screen[ 8 ];
        //ft4->xy3 = screen[ 9 ];
        ft4->xy0.vx = screen[3].vx;
        ft4->xy0.vy = screen[3].vy;
        ft4->xy1.vx = screen[4].vx;
        ft4->xy1.vy = screen[4].vy;
        ft4->xy2.vx = screen[8].vx;
        ft4->xy2.vy = screen[8].vy;
        ft4->xy3.vx = screen[9].vx;
        ft4->xy3.vy = screen[9].vy;
        ft4->xy0.vz = depth[3];
        ft4->xy1.vz = depth[4];
        ft4->xy2.vz = depth[8];
        ft4->xy3.vz = depth[9];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[3] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[3] & flag[4] & flag[8] & flag[9] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[3];

        AsmAutoMesh(&svector[3], &svector[4], &svector[8], &svector[9], face, tile, autoMesh);
      }

      /* polygon 4 */

      if (!((flag[5] | flag[6] | flag[10] | flag[11]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[4];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 5 ];
        //ft4->xy1 = screen[ 6 ];
        //ft4->xy2 = screen[ 10 ];
        //ft4->xy3 = screen[ 11 ];
        ft4->xy0.vx = screen[5].vx;
        ft4->xy0.vy = screen[5].vy;
        ft4->xy1.vx = screen[6].vx;
        ft4->xy1.vy = screen[6].vy;
        ft4->xy2.vx = screen[10].vx;
        ft4->xy2.vy = screen[10].vy;
        ft4->xy3.vx = screen[11].vx;
        ft4->xy3.vy = screen[11].vy;
        ft4->xy0.vz = depth[5];
        ft4->xy1.vz = depth[6];
        ft4->xy2.vz = depth[10];
        ft4->xy3.vz = depth[11];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[5] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[5] & flag[6] & flag[10] & flag[11] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[4];

        AsmAutoMesh(&svector[5], &svector[6], &svector[10], &svector[11], face, tile, autoMesh);
      }

      /* polygon 5 */

      if (!((flag[6] | flag[7] | flag[11] | flag[12]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[5];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 6 ];
        //ft4->xy1 = screen[ 7 ];
        //ft4->xy2 = screen[ 11 ];
        //ft4->xy3 = screen[ 12 ];
        ft4->xy0.vx = screen[6].vx;
        ft4->xy0.vy = screen[6].vy;
        ft4->xy1.vx = screen[7].vx;
        ft4->xy1.vy = screen[7].vy;
        ft4->xy2.vx = screen[11].vx;
        ft4->xy2.vy = screen[11].vy;
        ft4->xy3.vx = screen[12].vx;
        ft4->xy3.vy = screen[12].vy;
        ft4->xy0.vz = depth[6];
        ft4->xy1.vz = depth[7];
        ft4->xy2.vz = depth[11];
        ft4->xy3.vz = depth[12];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[6] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[6] & flag[7] & flag[11] & flag[12] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[5];

        AsmAutoMesh(&svector[6], &svector[7], &svector[11], &svector[12], face, tile, autoMesh);
      }

      /* polygon 6 */

      if (!((flag[7] | flag[8] | flag[12] | flag[13]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[6];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 7 ];
        //ft4->xy1 = screen[ 8 ];
        //ft4->xy2 = screen[ 12 ];
        //ft4->xy3 = screen[ 13 ];
        ft4->xy0.vx = screen[7].vx;
        ft4->xy0.vy = screen[7].vy;
        ft4->xy1.vx = screen[8].vx;
        ft4->xy1.vy = screen[8].vy;
        ft4->xy2.vx = screen[12].vx;
        ft4->xy2.vy = screen[12].vy;
        ft4->xy3.vx = screen[13].vx;
        ft4->xy3.vy = screen[13].vy;
        ft4->xy0.vz = depth[7];
        ft4->xy1.vz = depth[8];
        ft4->xy2.vz = depth[12];
        ft4->xy3.vz = depth[13];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[7] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[7] & flag[8] & flag[12] & flag[13] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[6];

        AsmAutoMesh(&svector[7], &svector[8], &svector[12], &svector[13], face, tile, autoMesh);
      }

      /* polygon 7 */

      if (!((flag[8] | flag[9] | flag[13] | flag[14]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[7];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 8 ];
        //ft4->xy1 = screen[ 9 ];
        //ft4->xy2 = screen[ 13 ];
        //ft4->xy3 = screen[ 14 ];
        ft4->xy0.vx = screen[8].vx;
        ft4->xy0.vy = screen[8].vy;
        ft4->xy1.vx = screen[9].vx;
        ft4->xy1.vy = screen[9].vy;
        ft4->xy2.vx = screen[13].vx;
        ft4->xy2.vy = screen[13].vy;
        ft4->xy3.vx = screen[14].vx;
        ft4->xy3.vy = screen[14].vy;
        ft4->xy0.vz = depth[8];
        ft4->xy1.vz = depth[9];
        ft4->xy2.vz = depth[13];
        ft4->xy3.vz = depth[14];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[8] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[8] & flag[9] & flag[13] & flag[14] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[7];

        AsmAutoMesh(&svector[8], &svector[9], &svector[13], &svector[14], face, tile, autoMesh);
      }

      /* polygon 8 */

      if (!((flag[10] | flag[11] | flag[15] | flag[16]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[8];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 10 ];
        //ft4->xy1 = screen[ 11 ];
        //ft4->xy2 = screen[ 15 ];
        //ft4->xy3 = screen[ 16 ];
        ft4->xy0.vx = screen[10].vx;
        ft4->xy0.vy = screen[10].vy;
        ft4->xy1.vx = screen[11].vx;
        ft4->xy1.vy = screen[11].vy;
        ft4->xy2.vx = screen[15].vx;
        ft4->xy2.vy = screen[15].vy;
        ft4->xy3.vx = screen[16].vx;
        ft4->xy3.vy = screen[16].vy;
        ft4->xy0.vz = depth[10];
        ft4->xy1.vz = depth[11];
        ft4->xy2.vz = depth[15];
        ft4->xy3.vz = depth[16];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[10] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[10] & flag[11] & flag[15] & flag[16] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[8];

        AsmAutoMesh(&svector[10], &svector[11], &svector[15], &svector[16], face, tile, autoMesh);
      }

      /* polygon 9 */

      if (!((flag[11] | flag[12] | flag[16] | flag[17]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[9];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 11 ];
        //ft4->xy1 = screen[ 12 ];
        //ft4->xy2 = screen[ 16 ];
        //ft4->xy3 = screen[ 17 ];
        ft4->xy0.vx = screen[11].vx;
        ft4->xy0.vy = screen[11].vy;
        ft4->xy1.vx = screen[12].vx;
        ft4->xy1.vy = screen[12].vy;
        ft4->xy2.vx = screen[16].vx;
        ft4->xy2.vy = screen[16].vy;
        ft4->xy3.vx = screen[17].vx;
        ft4->xy3.vy = screen[17].vy;
        ft4->xy0.vz = depth[11];
        ft4->xy1.vz = depth[12];
        ft4->xy2.vz = depth[16];
        ft4->xy3.vz = depth[17];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[11] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[11] & flag[12] & flag[16] & flag[17] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[9];

        AsmAutoMesh(&svector[11], &svector[12], &svector[16], &svector[17], face, tile, autoMesh);
      }

      /* polygon 10 */

      if (!((flag[12] | flag[13] | flag[17] | flag[18]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[10];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 12 ];
        //ft4->xy1 = screen[ 13 ];
        //ft4->xy2 = screen[ 17 ];
        //ft4->xy3 = screen[ 18 ];
        ft4->xy0.vx = screen[12].vx;
        ft4->xy0.vy = screen[12].vy;
        ft4->xy1.vx = screen[13].vx;
        ft4->xy1.vy = screen[13].vy;
        ft4->xy2.vx = screen[17].vx;
        ft4->xy2.vy = screen[17].vy;
        ft4->xy3.vx = screen[18].vx;
        ft4->xy3.vy = screen[18].vy;
        ft4->xy0.vz = depth[12];
        ft4->xy1.vz = depth[13];
        ft4->xy2.vz = depth[17];
        ft4->xy3.vz = depth[18];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[12] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[12] & flag[13] & flag[17] & flag[18] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[10];

        AsmAutoMesh(&svector[12], &svector[13], &svector[17], &svector[18], face, tile, autoMesh);
      }

      /* polygon 11 */

      if (!((flag[13] | flag[14] | flag[18] | flag[19]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[11];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 13 ];
        //ft4->xy1 = screen[ 14 ];
        //ft4->xy2 = screen[ 18 ];
        //ft4->xy3 = screen[ 19 ];
        ft4->xy0.vx = screen[13].vx;
        ft4->xy0.vy = screen[13].vy;
        ft4->xy1.vx = screen[14].vx;
        ft4->xy1.vy = screen[14].vy;
        ft4->xy2.vx = screen[18].vx;
        ft4->xy2.vy = screen[18].vy;
        ft4->xy3.vx = screen[19].vx;
        ft4->xy3.vy = screen[19].vy;
        ft4->xy0.vz = depth[13];
        ft4->xy1.vz = depth[14];
        ft4->xy2.vz = depth[18];
        ft4->xy3.vz = depth[19];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[13] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[13] & flag[14] & flag[18] & flag[19] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[11];

        AsmAutoMesh(&svector[13], &svector[14], &svector[18], &svector[19], face, tile, autoMesh);
      }

      /* polygon 12 */

      if (!((flag[15] | flag[16] | flag[20] | flag[21]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[12];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 15 ];
        //ft4->xy1 = screen[ 16 ];
        //ft4->xy2 = screen[ 20 ];
        //ft4->xy3 = screen[ 21 ];
        ft4->xy0.vx = screen[15].vx;
        ft4->xy0.vy = screen[15].vy;
        ft4->xy1.vx = screen[16].vx;
        ft4->xy1.vy = screen[16].vy;
        ft4->xy2.vx = screen[20].vx;
        ft4->xy2.vy = screen[20].vy;
        ft4->xy3.vx = screen[21].vx;
        ft4->xy3.vy = screen[21].vy;
        ft4->xy0.vz = depth[15];
        ft4->xy1.vz = depth[16];
        ft4->xy2.vz = depth[20];
        ft4->xy3.vz = depth[21];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[15] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[15] & flag[16] & flag[20] & flag[21] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[12];

        AsmAutoMesh(&svector[15], &svector[16], &svector[20], &svector[21], face, tile, autoMesh);
      }

      /* polygon 13 */

      if (!((flag[16] | flag[17] | flag[21] | flag[22]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[13];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 16 ];
        //ft4->xy1 = screen[ 17 ];
        //ft4->xy2 = screen[ 21 ];
        //ft4->xy3 = screen[ 22 ];
        ft4->xy0.vx = screen[16].vx;
        ft4->xy0.vy = screen[16].vy;
        ft4->xy1.vx = screen[17].vx;
        ft4->xy1.vy = screen[17].vy;
        ft4->xy2.vx = screen[21].vx;
        ft4->xy2.vy = screen[21].vy;
        ft4->xy3.vx = screen[22].vx;
        ft4->xy3.vy = screen[22].vy;
        ft4->xy0.vz = depth[16];
        ft4->xy1.vz = depth[17];
        ft4->xy2.vz = depth[21];
        ft4->xy3.vz = depth[22];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[16] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[16] & flag[17] & flag[21] & flag[22] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[13];

        AsmAutoMesh(&svector[16], &svector[17], &svector[21], &svector[22], face, tile, autoMesh);
      }

      /* polygon 14 */

      if (!((flag[17] | flag[18] | flag[22] | flag[23]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[14];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 17 ];
        //ft4->xy1 = screen[ 18 ];
        //ft4->xy2 = screen[ 22 ];
        //ft4->xy3 = screen[ 23 ];
        ft4->xy0.vx = screen[17].vx;
        ft4->xy0.vy = screen[17].vy;
        ft4->xy1.vx = screen[18].vx;
        ft4->xy1.vy = screen[18].vy;
        ft4->xy2.vx = screen[22].vx;
        ft4->xy2.vy = screen[22].vy;
        ft4->xy3.vx = screen[23].vx;
        ft4->xy3.vy = screen[23].vy;
        ft4->xy0.vz = depth[17];
        ft4->xy1.vz = depth[18];
        ft4->xy2.vz = depth[22];
        ft4->xy3.vz = depth[23];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[17] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[17] & flag[18] & flag[22] & flag[23] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[14];

        AsmAutoMesh(&svector[17], &svector[18], &svector[22], &svector[23], face, tile, autoMesh);
      }

      /* polygon 15 */

      if (!((flag[18] | flag[19] | flag[23] | flag[24]) & ShortClipFlags)) {
        ft4 = &TrackHi[CurrentScreen][TrackHiCount++];

        SetPolyFT4((POLY_FT4*)ft4);

        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[15];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;

        ft4->u0 = tile->u0;
        ft4->v0 = tile->v0;

        ft4->u1 = tile->u1;
        ft4->v1 = tile->v1;

        ft4->u2 = tile->u2;
        ft4->v2 = tile->v2;

        ft4->u3 = tile->u3;
        ft4->v3 = tile->v3;

        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;

        //ft4->xy0 = screen[ 18 ];
        //ft4->xy1 = screen[ 19 ];
        //ft4->xy2 = screen[ 23 ];
        //ft4->xy3 = screen[ 24 ];
        ft4->xy0.vx = screen[18].vx;
        ft4->xy0.vy = screen[18].vy;
        ft4->xy1.vx = screen[19].vx;
        ft4->xy1.vy = screen[19].vy;
        ft4->xy2.vx = screen[23].vx;
        ft4->xy2.vy = screen[23].vy;
        ft4->xy3.vx = screen[24].vx;
        ft4->xy3.vy = screen[24].vy;
        ft4->xy0.vz = depth[18];
        ft4->xy1.vz = depth[19];
        ft4->xy2.vz = depth[23];
        ft4->xy3.vz = depth[24];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        AddPrim(OT[CurrentScreen], ((depth[18] >> DepthShift) - DepthMod), (P_TAG*)ft4);
      } else if (!(flag[18] & flag[19] & flag[23] & flag[24] & ShortClipFlags)) {
        if (face->flags & FlipTexture) {
          surface = &TrackSurface[face->texture][1];
        } else {
          surface = &TrackSurface[face->texture][0];
        }

        tile = &surface->high[15];

        AsmAutoMesh(&svector[18], &svector[19], &svector[23], &svector[24], face, tile, autoMesh);
      }

      face++;
    }

    section = section->nextSection.ptr;
  }

  return section;
}

void AsmAutoMesh(
    SVECTOR* tlVector,
    SVECTOR* trVector,
    SVECTOR* blVector,
    SVECTOR* brVector,
    Face* face,
    Tile* tile,
    AutoMeshData* autoMesh) {
  PolyFT4* ft4;
  PolyFT3* ft3;

  int16_t clip;

  return;

#define top 1
#define right 2
#define bottom 4
#define left 8
#define centre 16

#define topLeft 32
#define topRight 64
#define bottomLeft 128
#define bottomRight 256

#define tNum 0
#define rNum 1
#define bNum 2
#define lNum 3
#define cNum 4

#define tlNum 5
#define trNum 6
#define blNum 7
#define brNum 8

  SVECTOR* svector = (SVECTOR*)(FastRam + 816); /* 5 * 8 = 40 bytes */
  DVECTOR* screen = (DVECTOR*)(FastRam + 856); /* 9 * 4 = 36 bytes */
  int32_t* depth = (int32_t*)(FastRam + 892); /* 9 * 4 = 36 bytes */

  int32_t* interpolate = (int32_t*)(FastRam + 928); /* 1 * 4 = 4 bytes  */
  int32_t* flag = (int32_t*)(FastRam + 932); /* 1 * 4 = 4 bytes  */

  uint8_t* uv = (uint8_t*)(FastRam + 936); /* 5 * 2 = 10 bytes */

#define tu 0
#define ru 1
#define bu 2
#define lu 3
#define cu 4

#define tv 5
#define rv 6
#define bv 7
#define lv 8
#define cv 9

  if ((autoMesh->quadCount >= MaxAutos) || (autoMesh->triCount >= MaxAutos)) {
#if 0
		Error( " too many automeshed polys \n\n", Warning );
#endif

    return;
  }

  /* Calculate 3d coords */

  svector[tNum].vx = (tlVector->vx + trVector->vx) >> 1;
  svector[tNum].vy = (tlVector->vy + trVector->vy) >> 1;
  svector[tNum].vz = (tlVector->vz + trVector->vz) >> 1;

  svector[bNum].vx = (blVector->vx + brVector->vx) >> 1;
  svector[bNum].vy = (blVector->vy + brVector->vy) >> 1;
  svector[bNum].vz = (blVector->vz + brVector->vz) >> 1;

  svector[rNum].vx = (trVector->vx + brVector->vx) >> 1;
  svector[rNum].vy = (trVector->vy + brVector->vy) >> 1;
  svector[rNum].vz = (trVector->vz + brVector->vz) >> 1;

  svector[lNum].vx = (tlVector->vx + blVector->vx) >> 1;
  svector[lNum].vy = (tlVector->vy + blVector->vy) >> 1;
  svector[lNum].vz = (tlVector->vz + blVector->vz) >> 1;

  svector[cNum].vx = (svector[lNum].vx + svector[rNum].vx) >> 1;
  svector[cNum].vy = (svector[lNum].vy + svector[rNum].vy) >> 1;
  svector[cNum].vz = (svector[lNum].vz + svector[rNum].vz) >> 1;

  /* Transform the coordinates */

  clip = 0;

  depth[cNum] = RotTransPers(
      &svector[cNum],
      &screen[cNum],
      interpolate,
      flag);

  if (!(*flag & ClipFlags)) {
    clip |= centre;
    uv[cu] = (tile->u0 + tile->u1 + tile->u2 + tile->u3) >> 2;
    uv[cv] = (tile->v0 + tile->v1 + tile->v2 + tile->v3) >> 2;
  }

  depth[tNum] = RotTransPers(
      &svector[tNum],
      &screen[tNum],
      interpolate,
      flag);

  if (!(*flag & ClipFlags)) {
    clip |= top;
    uv[tu] = (tile->u0 + tile->u1) >> 1;
    uv[tv] = (tile->v0 + tile->v1) >> 1;
  }

  depth[rNum] = RotTransPers(
      &svector[rNum],
      &screen[rNum],
      interpolate,
      flag);

  if (!(*flag & ClipFlags)) {
    clip |= right;
    uv[ru] = (tile->u1 + tile->u3) >> 1;
    uv[rv] = (tile->v1 + tile->v3) >> 1;
  }

  depth[bNum] = RotTransPers(
      &svector[bNum],
      &screen[bNum],
      interpolate,
      flag);

  if (!(*flag & ClipFlags)) {
    clip |= bottom;
    uv[bu] = (tile->u2 + tile->u3) >> 1;
    uv[bv] = (tile->v2 + tile->v3) >> 1;
  }

  depth[lNum] = RotTransPers(
      &svector[lNum],
      &screen[lNum],
      interpolate,
      flag);

  if (!(*flag & ClipFlags)) {
    clip |= left;
    uv[lu] = (tile->u0 + tile->u2) >> 1;
    uv[lv] = (tile->v0 + tile->v2) >> 1;
  }

  /* Check top, right, bottom and left before doing RotTransPers for corners */

  if (1 || (clip & top) || (clip & left)) {
    depth[tlNum] = RotTransPers(
        tlVector,
        &screen[tlNum],
        interpolate,
        flag);

    if (!(*flag & ClipFlags))
      clip |= topLeft;
  }

  if (1 || (clip & top) || (clip & right)) {
    depth[trNum] = RotTransPers(
        trVector,
        &screen[trNum],
        interpolate,
        flag);

    if (!(*flag & ClipFlags))
      clip |= topRight;
  }

  if (1 || (clip & bottom) || (clip & left)) {
    depth[blNum] = RotTransPers(
        blVector,
        &screen[blNum],
        interpolate,
        flag);

    if (!(*flag & ClipFlags))
      clip |= bottomLeft;
  }

  if (1 || (clip & bottom) || (clip & right)) {
    depth[brNum] = RotTransPers(
        brVector,
        &screen[brNum],
        interpolate,
        flag);

    if (!(*flag & ClipFlags))
      clip |= bottomRight;
  }

  /* Check if centre coord is valid */

  if (clip & centre) {
    /* Meshing the quad into quarters */

    if (clip & topLeft) {
      if ((clip & top) && (clip & left)) {
        ft4 = &autoMesh->quad[CurrentScreen][autoMesh->quadCount];

        ft4->xy0.vx = screen[tlNum].vx;
        ft4->xy0.vy = screen[tlNum].vy;
        ft4->xy1.vx = screen[tNum].vx;
        ft4->xy1.vy = screen[tNum].vy;
        ft4->xy2.vx = screen[lNum].vx;
        ft4->xy2.vy = screen[lNum].vy;
        ft4->xy3.vx = screen[cNum].vx;
        ft4->xy3.vy = screen[cNum].vy;
        ft4->xy0.vz = depth[tlNum];
        ft4->xy1.vz = depth[tNum];
        ft4->xy2.vz = depth[lNum];
        ft4->xy3.vz = depth[cNum];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        //ft4->xy1 = screen[ tNum ];
        //ft4->xy2 = screen[ lNum ];
        //ft4->xy3 = screen[ cNum ];

        ft4->u0 = tile->u0;
        ft4->u1 = uv[tu];
        ft4->u2 = uv[lu];
        ft4->u3 = uv[cu];

        ft4->v0 = tile->v0;
        ft4->v1 = uv[tv];
        ft4->v2 = uv[lv];
        ft4->v3 = uv[cv];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;
#if 1
        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;
#else
        ft4->colour0.r = 255;
        ft4->colour0.g = 63;
        ft4->colour0.b = 63;
#endif
        SetPolyFT4((POLY_FT4*)(ft4));

        AddPrim(OT[CurrentScreen], depth[cNum], (P_TAG*)ft4);

        autoMesh->quadCount += 1;
      } else if (clip & top) {
        ft3 = &autoMesh->tri[CurrentScreen][autoMesh->triCount];

        //ft3->xy0 = screen[ tlNum ];
        //ft3->xy1 = screen[ tNum ];
        //ft3->xy2 = screen[ cNum ];
        ft3->xy0.vx = screen[tNum].vx;
        ft3->xy0.vy = screen[tNum].vy;
        ft3->xy1.vx = screen[lNum].vx;
        ft3->xy1.vy = screen[lNum].vy;
        ft3->xy2.vx = screen[cNum].vx;
        ft3->xy2.vy = screen[cNum].vy;
        ft3->xy0.vz = depth[tNum];
        ft3->xy1.vz = depth[lNum];
        ft3->xy2.vz = depth[cNum];
        ft3->xy0.pad = 1;
        ft3->xy1.pad = 1;
        ft3->xy2.pad = 1;

        ft3->u0 = tile->u0;
        ft3->u1 = uv[tu];
        ft3->u2 = uv[cu];

        ft3->v0 = tile->v0;
        ft3->v1 = uv[tv];
        ft3->v2 = uv[cv];

        ft3->cba = tile->cba;
        ft3->tsb = tile->tsb;
#if 1
        ft3->colour0.r = face->red;
        ft3->colour0.g = face->green;
        ft3->colour0.b = face->blue;
#else
        ft3->colour0.r = 63;
        ft3->colour0.g = 255;
        ft3->colour0.b = 63;
#endif
        SetPolyFT3((POLY_FT3*)(ft3));

        AddPrim(OT[CurrentScreen], depth[cNum], (P_TAG*)ft3);
        autoMesh->triCount += 1;
      }
    }

    if (clip & topRight) {
      if ((clip & top) && (clip & right)) {
        ft4 = &autoMesh->quad[CurrentScreen][autoMesh->quadCount];

        //ft4->xy0 = screen[ tNum ];
        //ft4->xy1 = screen[ trNum ];
        //ft4->xy2 = screen[ cNum ];
        //ft4->xy3 = screen[ rNum ];
        ft4->xy0.vx = screen[tNum].vx;
        ft4->xy0.vy = screen[tNum].vy;
        ft4->xy1.vx = screen[trNum].vx;
        ft4->xy1.vy = screen[trNum].vy;
        ft4->xy2.vx = screen[cNum].vx;
        ft4->xy2.vy = screen[cNum].vy;
        ft4->xy3.vx = screen[rNum].vx;
        ft4->xy3.vy = screen[rNum].vy;
        ft4->xy0.vz = depth[tNum];
        ft4->xy1.vz = depth[trNum];
        ft4->xy2.vz = depth[cNum];
        ft4->xy3.vz = depth[rNum];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        ft4->u0 = uv[tu];
        ft4->u1 = tile->u1;
        ft4->u2 = uv[cu];
        ft4->u3 = uv[ru];

        ft4->v0 = uv[tv];
        ft4->v1 = tile->v1;
        ft4->v2 = uv[cv];
        ft4->v3 = uv[rv];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;
#if 1
        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;
#else
        ft4->colour0.r = 255;
        ft4->colour0.g = 63;
        ft4->colour0.b = 63;
#endif
        SetPolyFT4((POLY_FT4*)(ft4));

        AddPrim(OT[CurrentScreen], depth[cNum], (P_TAG*)ft4);
        autoMesh->quadCount += 1;
      } else if (clip & top) {
        ft3 = &autoMesh->tri[CurrentScreen][autoMesh->triCount];

        //ft3->xy0 = screen[ trNum ];
        //ft3->xy1 = screen[ cNum ];
        //ft3->xy2 = screen[ tNum ];
        ft3->xy0.vx = screen[trNum].vx;
        ft3->xy0.vy = screen[trNum].vy;
        ft3->xy1.vx = screen[cNum].vx;
        ft3->xy1.vy = screen[cNum].vy;
        ft3->xy2.vx = screen[tNum].vx;
        ft3->xy2.vy = screen[tNum].vy;
        ft3->xy0.vz = depth[trNum];
        ft3->xy1.vz = depth[cNum];
        ft3->xy2.vz = depth[tNum];
        ft3->xy0.pad = 1;
        ft3->xy1.pad = 1;
        ft3->xy2.pad = 1;

        ft3->u0 = tile->u1;
        ft3->u1 = uv[cu];
        ft3->u2 = uv[tu];

        ft3->v0 = tile->v1;
        ft3->v1 = uv[cv];
        ft3->v2 = uv[tv];

        ft3->cba = tile->cba;
        ft3->tsb = tile->tsb;
#if 1
        ft3->colour0.r = face->red;
        ft3->colour0.g = face->green;
        ft3->colour0.b = face->blue;
#else
        ft3->colour0.r = 63;
        ft3->colour0.g = 255;
        ft3->colour0.b = 63;
#endif
        SetPolyFT3((POLY_FT3*)(ft3));

        AddPrim(OT[CurrentScreen], depth[cNum], (P_TAG*)ft3);
        autoMesh->triCount += 1;
      }
    }

    if (clip & bottomLeft) {
      if ((clip & bottom) && (clip & left)) {
        ft4 = &autoMesh->quad[CurrentScreen][autoMesh->quadCount];

        //ft4->xy0 = screen[ lNum ];
        //ft4->xy1 = screen[ cNum ];
        //ft4->xy2 = screen[ blNum ];
        //ft4->xy3 = screen[ bNum ];
        ft4->xy0.vx = screen[lNum].vx;
        ft4->xy0.vy = screen[lNum].vy;
        ft4->xy1.vx = screen[cNum].vx;
        ft4->xy1.vy = screen[cNum].vy;
        ft4->xy2.vx = screen[blNum].vx;
        ft4->xy2.vy = screen[blNum].vy;
        ft4->xy3.vx = screen[bNum].vx;
        ft4->xy3.vy = screen[bNum].vy;
        ft4->xy0.vz = depth[lNum];
        ft4->xy1.vz = depth[cNum];
        ft4->xy2.vz = depth[blNum];
        ft4->xy3.vz = depth[bNum];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        ft4->u0 = uv[lu];
        ft4->u1 = uv[cu];
        ft4->u2 = tile->u2;
        ft4->u3 = uv[bu];

        ft4->v0 = uv[lv];
        ft4->v1 = uv[cv];
        ft4->v2 = tile->v2;
        ft4->v3 = uv[bv];

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;
#if 1
        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;
#else
        ft4->colour0.r = 255;
        ft4->colour0.g = 63;
        ft4->colour0.b = 63;
#endif
        SetPolyFT4((POLY_FT4*)(ft4));

        AddPrim(OT[CurrentScreen], depth[cNum], (P_TAG*)ft4);

        autoMesh->quadCount += 1;
      }
    }

    if (clip & bottomRight) {
      if ((clip & bottom) && (clip & right)) {
        ft4 = &autoMesh->quad[CurrentScreen][autoMesh->quadCount];

        ft4->xy0.vx = screen[cNum].vx;
        ft4->xy0.vy = screen[cNum].vy;
        ft4->xy1.vx = screen[rNum].vx;
        ft4->xy1.vy = screen[rNum].vy;
        ft4->xy2.vx = screen[bNum].vx;
        ft4->xy2.vy = screen[bNum].vy;
        ft4->xy3.vx = screen[brNum].vx;
        ft4->xy3.vy = screen[brNum].vy;
        ft4->xy0.vz = depth[cNum];
        ft4->xy1.vz = depth[rNum];
        ft4->xy2.vz = depth[bNum];
        ft4->xy3.vz = depth[brNum];
        ft4->xy0.pad = 1;
        ft4->xy1.pad = 1;
        ft4->xy2.pad = 1;
        ft4->xy3.pad = 1;

        //ft4->xy1 = screen[ rNum ];
        //ft4->xy2 = screen[ bNum ];
        //ft4->xy3 = screen[ brNum ];

        ft4->u0 = uv[cu];
        ft4->u1 = uv[ru];
        ft4->u2 = uv[bu];
        ft4->u3 = tile->u3;

        ft4->v0 = uv[cv];
        ft4->v1 = uv[rv];
        ft4->v2 = uv[bv];
        ft4->v3 = tile->v3;

        ft4->cba = tile->cba;
        ft4->tsb = tile->tsb;
#if 1
        ft4->colour0.r = face->red;
        ft4->colour0.g = face->green;
        ft4->colour0.b = face->blue;
#else
        ft4->colour0.r = 255;
        ft4->colour0.g = 63;
        ft4->colour0.b = 63;
#endif
        SetPolyFT4((POLY_FT4*)(ft4));

        AddPrim(OT[CurrentScreen], depth[cNum], (P_TAG*)ft4);
        autoMesh->quadCount += 1;
      }
    }
  }
}

void Test16Bit(int32_t* Test16bitX, int32_t* Test16bitY, int32_t* Test16bitZ) {
  if (*Test16bitX > 32767)
    *Test16bitX = 32767;
  else if (*Test16bitX < -32767)
    *Test16bitX = -32767;

  if (*Test16bitY > 32767)
    *Test16bitY = 32767;
  else if (*Test16bitY < -32767)
    *Test16bitY = -32767;

  if (*Test16bitZ > 32767)
    *Test16bitZ = 32767;
  else if (*Test16bitZ < -32767)
    *Test16bitZ = -32767;
}
