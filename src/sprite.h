#ifndef SPRITE_H
#define SPRITE_H

#include <stdint.h>
#include "libgpu.h"
#include "libgte.h"
#include "object.h"
#include "track.h"

typedef struct Sprites {
  VECTOR pos;
  POLY_FT4 sprite[100][2];
  Skeleton* skeleton;
} Sprites;

void InitSprites(Sprites* sprites, Skeleton* camPos);
void AddSprites(Sprites* sprites, TrackSection* section);
void UpdateSprites(Sprites* sprites, TrackSection* section, int16_t hdg);

#endif
