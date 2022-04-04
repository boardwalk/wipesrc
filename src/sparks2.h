#ifndef SPARKS2_H
#define SPARKS2_H

#include <stdint.h>
#include "libgpu.h"
#include "libgte.h"
#include "object.h"

#define Max3DLines 200
#define Max3DLineSkels 10

typedef struct Line3D {
  int16_t used;
  int16_t skel;

  VECTOR start;
  VECTOR pos;
  VECTOR vel;

  int32_t accl;
  int32_t friction;
  int32_t gravity;
  int32_t wallRes;

  int32_t life;

  LINE_G2 line[2];
} Line3D;

typedef struct LineSkel {
  int16_t count;
  int16_t used;

  Skeleton* skeleton;
} LineSkel;

void CreateSparks3D(Skeleton* camPos);
void StartSparks3D(VECTOR* start, int16_t numSparks, VECTOR* velocity);
void UpdateSparks3D();

#endif
