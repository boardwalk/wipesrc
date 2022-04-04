/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/

#include "standard.h"
#include <stdio.h>
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "droid.h"
#include "camera.h"
#include "weapon.h"
#include "control.h"
#include "sound.h"
#include "draw.h"
#include "sparks2.h"
#include "global.h"
#include "ttf.h"
#include "trackasm.h"
#include "asmtrk.h"
#include "effects.h"
#include "scene.h"
#include "main.h"
#include "dynam.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "cone.h"
#include "race.h"
#include "fri.h"
#include "error.h"
#include "pallette.h"

#define TrackVersion 8
#define GsGetVcount() 0;

//#define AsmTransformTrackMed AsmTransformTrackLo

extern int32_t inattract;

void IntelVertex() {
  VECTOR* vert;
  int16_t i;

  /* intel the vertex data */

  vert = track->vertices;
  for (i = 0; i < track->vertexCount; i++) {
    IntelLong(&vert->vx);
    IntelLong(&vert->vy);
    IntelLong(&vert->vz);

    vert += 1;
  }
}

void IntelFace() {
  Face* face;
  int16_t i;

  /* intel the face data */

  face = track->faces;
  for (i = 0; i < track->faceCount; i++) {
    IntelShort(&face->vertex[0]);
    IntelShort(&face->vertex[1]);
    IntelShort(&face->vertex[2]);
    IntelShort(&face->vertex[3]);

    IntelShort(&face->normal.vx);
    IntelShort(&face->normal.vy);
    IntelShort(&face->normal.vz);

    face += 1;
  }
}

void CheckVersion() {
  TrackSection* section;
  int16_t i;

  section = track->sections;
  for (i = 0; i < track->sectionCount; i++) {
    if (section->version != TrackVersion) {
      Error("Convert track with track10", Fatal);
    }
    section += 1;
  }
}

void IndexToPointers() {
  TrackSection* section;
  int16_t i;

  /* change the section indices to pointers */

  section = track->sections;
  for (i = 0; i < track->sectionCount; i++) {
    section->prevSection.ptr = track->sections + section->prevSection.index;
    section->nextSection.ptr = track->sections + section->nextSection.index;

    if (section->junction.index != -1) {
      section->junction.ptr = track->sections + section->junction.index;
    }

    section += 1;
  }
}

void PointersToIndex() {
  TrackSection* section;
  int16_t i;

  /* change the section pointers back to indices */

  section = track->sections;
  for (i = 0; i < track->sectionCount; i++) {
    section->prevSection.index = section->prevSection.ptr - track->sections;
    section->nextSection.index = section->nextSection.ptr - track->sections;

    if (section->junction.index != -1) {
      section->junction.index = section->junction.ptr - track->sections;
    }

    section += 1;
  }
}

void InitVertices(char* file) {
  int32_t length;

  length = FileLength(file);
  if (length <= 0) {
    sprintf(errorString, "Track.c:InitVertices: file %s not found\n", file);
    Error(errorString, Fatal);
  }

  track->vertices = (VECTOR*)DAlloc(heap, length + 32);
  if (!track->vertices) {
    Error("Track.c:InitVertices: Failed to allocate memory for track vertices", Fatal);
  }

  if (LoadFile(file, (char*)track->vertices) != length) {
    sprintf(errorString, "Track.c:InitVertices(): Failed to load file %s\n", file);
    Error(errorString, Fatal);
  }
#if 0
	printf( "VERTICES = %d\n", length / sizeof( VECTOR ) );
#endif
  track->vertexCount = length / sizeof(VECTOR);

  IntelVertex();
}

void InitFaces(char* file) {
  int32_t length;
  int16_t i;
  CVECTOR cols;

  length = FileLength(file);
  if (length <= 0) {
    sprintf(errorString, "Track.c:InitFaces: file %s not found\n", file);
    Error(errorString, Fatal);
  }

  track->faces = (Face*)DAlloc(heap, length + 32);
  if (!track->faces) {
    Error("Track.c:InitFaces: Failed to allocate memory for track faces", Fatal);
  }

  if (LoadFile(file, (char*)track->faces) != length) {
    sprintf(errorString, "Track.c:InitFaces(): Failed to load file %s\n", file);
    Error(errorString, Fatal);
  }
#if 0
	printf( "FACES = %d\n", length / sizeof( Face ) );
#endif
  track->faceCount = length / sizeof(Face);

  IntelFace();

  /* copy the original colors */

  track->colCopy = (Colors*)DAlloc(heap, track->faceCount * sizeof(Colors));
  if (!track->faces) {
    Error("Track.c:InitFaces: Failed to allocate memory for copy of colors", Fatal);
  }

  for (i = 0; i < track->faceCount; i++) {
    cols.r = track->faces[i].red;
    cols.g = track->faces[i].green;
    cols.b = track->faces[i].blue;

    track->faces[i].red = FindNearestColourIndex(&cols);
    track->faces[i].green = 0;
    track->faces[i].blue = BLACK_NOT_DRAWN;
    track->colCopy[i].red = track->faces[i].red;
    track->colCopy[i].green = track->faces[i].green;
    track->colCopy[i].blue = track->faces[i].blue;
  }
}

void InitSections(char* file) {
  int32_t length;

  length = FileLength(file);
  if (length <= 0) {
    sprintf(errorString, "Track.c:InitSections: file %s not found\n", file);
    Error(errorString, Fatal);
  }

  track->sections = (TrackSection*)DAlloc(heap, length + 32);
  if (!track->sections) {
    Error("Track.c:InitSections: Failed to allocate memory for track sections", Fatal);
  }

  if (LoadFile(file, (char*)track->sections) != length) {
    sprintf(errorString, "Track.c:InitSections(): Failed to load file %s\n", file);
    Error(errorString, Fatal);
  }
#if 0
	printf("SECTIONS = %d\n", length / sizeof( TrackSection ) );
#endif
  track->sectionCount = length / sizeof(TrackSection);

  IndexToPointers();

#if 1
  CheckVersion();
#endif
}

void InitViewList(char* file) {
  int32_t length;
  TrackSection* section;
  int32_t total;
  int16_t i, j;

  length = FileLength(file);
  if (length <= 0) {
    sprintf(errorString, "Track.c:InitViewList: file %s not found\n", file);
    Error(errorString, Fatal);
  }

  track->viewList = (int16_t*)DAlloc(heap, length + 32);
  if (!track->viewList) {
    Error("Track.c:InitViewList: Failed to allocate memory for view list", Fatal);
  }

  if (LoadFile(file, (char*)track->viewList) != length) {
    sprintf(errorString, "Track.c:InitViewList(): Failed to load file %s\n", file);
    Error(errorString, Fatal);
  }

  for (i = 0; i < length / 2; i++) {
    IntelShort(&track->viewList[i]);
  }

  section = track->sections;
  total = 0;
  for (i = 0; i < track->sectionCount; i++) {
    for (j = 0; j < 3; j++) {
      section->northViewSection[j] = track->viewList + total;
      total += section->northViewCount[j];

      section->southViewSection[j] = track->viewList + total;
      total += section->southViewCount[j];

      section->eastViewSection[j] = track->viewList + total;
      total += section->eastViewCount[j];

      section->westViewSection[j] = track->viewList + total;
      total += section->westViewCount[j];

      section->allViewSection[j] = track->viewList + total;
      total += section->allViewCount[j];
    }

    section++;
  }

  if ((total * 2) != length) {
    printf("\n");
    printf("total  %d\n", total);
    printf("length %d\n", length);
    Error("track.c::InitViewList():problem with view list", Fatal);
  }
}

TrackSection* NextIndexSection(TrackSection* section) {
  section += 1;

  if (section > track->sections + track->sectionCount - 1) {
    section = track->sections;
  }

  return (section);
}

TrackSection* PrevIndexSection(TrackSection* section) {
  section -= 1;

  if (section < track->sections) {
    section = track->sections + track->sectionCount - 1;
  }

  return (section);
}

void DrawNewTrack(TrackCamera* camera, AutoMeshData* autoMesh) {
  int16_t i;
  int16_t dir;

  autoMesh->triCount = 0;
  autoMesh->quadCount = 0;
  autoMesh->attemptedAutos = 0;

  AsmNextFrame();

  dir = TrackDirection(camera);

  if (dir == ShipForwards) {
    for (i = 0; i < camera->section->northViewCount[2]; i += 2) {
      AsmTransformTrackLo // PCwipeout
          (
              track->sections + camera->section->northViewSection[2][i],
              camera->camPos,
              camera->section->northViewSection[2][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->northViewCount[1]; i += 2) {
      AsmTransformTrackMed // PCwipeout
          (
              track->sections + camera->section->northViewSection[1][i],
              camera->camPos,
              camera->section->northViewSection[1][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->northViewCount[0]; i += 2) {
      //if ((TextureTrack) || (inattract))
      AsmTransformTrackHi // PCwipeout
          (
              track->sections + camera->section->northViewSection[0][i],
              camera->camPos,
              autoMesh,
              camera->section->northViewSection[0][i + 1],
              dir);
    }

  }

  else if (dir == ShipBackwards) {
    for (i = 0; i < camera->section->southViewCount[2]; i += 2) {
      AsmTransformTrackLo // PCwipeout
          (
              track->sections + camera->section->southViewSection[2][i],
              camera->camPos,
              camera->section->southViewSection[2][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->southViewCount[1]; i += 2) {
      AsmTransformTrackMed // PCwipeout
          (
              track->sections + camera->section->southViewSection[1][i],
              camera->camPos,
              camera->section->southViewSection[1][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->southViewCount[0]; i += 2) {
      AsmTransformTrackHi // PCwipeout
          (
              track->sections + camera->section->southViewSection[0][i],
              camera->camPos,
              autoMesh,
              camera->section->southViewSection[0][i + 1],
              dir);
    }

  } else if (dir == ShipRight) {
    for (i = 0; i < camera->section->westViewCount[2]; i += 2) {
      AsmTransformTrackLo // PCwipeout
          (
              track->sections + camera->section->westViewSection[2][i],
              camera->camPos,
              camera->section->westViewSection[2][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->westViewCount[1]; i += 2) {
      AsmTransformTrackMed // PCwipeout
          (
              track->sections + camera->section->westViewSection[1][i],
              camera->camPos,
              camera->section->westViewSection[1][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->westViewCount[0]; i += 2) {
      AsmTransformTrackHi // PCwipeout
          (
              track->sections + camera->section->westViewSection[0][i],
              camera->camPos,
              autoMesh,
              camera->section->westViewSection[0][i + 1],
              dir);
    }
  } else if (dir == ShipLeft) {
    for (i = 0; i < camera->section->eastViewCount[2]; i += 2) {
      AsmTransformTrackLo // PCwipeout
          (
              track->sections + camera->section->eastViewSection[2][i],
              camera->camPos,
              camera->section->eastViewSection[2][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->eastViewCount[1]; i += 2) {
      AsmTransformTrackMed // PCwipeout
          (
              track->sections + camera->section->eastViewSection[1][i],
              camera->camPos,
              camera->section->eastViewSection[1][i + 1],
              dir);
    }

    for (i = 0; i < camera->section->eastViewCount[0]; i += 2) {
      AsmTransformTrackHi // PCwipeout
          (
              track->sections + camera->section->eastViewSection[0][i],
              camera->camPos,
              autoMesh,
              camera->section->eastViewSection[0][i + 1],
              dir);
    }
  }
}

void DrawAllTrack(TrackCamera* camera, AutoMeshData* autoMesh) {
  int16_t i;

  autoMesh->triCount = 0;
  autoMesh->quadCount = 0;
  autoMesh->attemptedAutos = 0;
  AsmNextFrame();

  for (i = 0; i < camera->section->allViewCount[2]; i += 2) {
    AsmTransformTrackLo(
        track->sections + camera->section->allViewSection[2][i],
        camera->camPos,
        camera->section->allViewSection[2][i + 1],
        0);
  }

  for (i = 0; i < camera->section->allViewCount[1]; i += 2) {
    AsmTransformTrackMed(
        track->sections + camera->section->allViewSection[1][i],
        camera->camPos,
        camera->section->allViewSection[1][i + 1],
        0);
  }

  for (i = 0; i < camera->section->allViewCount[0]; i += 2) {
    AsmTransformTrackHi(
        track->sections + camera->section->allViewSection[0][i],
        camera->camPos,
        autoMesh,
        camera->section->allViewSection[0][i + 1],
        0);
  }
}
