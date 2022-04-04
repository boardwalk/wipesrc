#ifndef TEXPLACE_H
#define TEXPLACE_H

#include <stdint.h>
#include "libgpu.h"
#include "object.h"
#include "track.h"
#include "ttf.h"

typedef struct TexPlace {
  int16_t libraryTextures; /* index into library texture table */
  int16_t emptyTexture; /* the 'empty' texture */

  POLY_GT4 textures[2][8]; /* the actual icons */
  POLY_G4 highlight[2]; /* the cursor highlight */

  int16_t numLib; /* number of track textures in memory eg 30 */

  int16_t position; /* position of cursor highlight on mini textures */
  int16_t bank; /* the bank of textures we are on */
} TexPlace;

void InitTexPlace(TexPlace* texPlace, TexTemplate* texTemplate);
void NewBank(TexPlace* texPlace, TexTemplate* texTemplate);
void TexPlaceUpdate(TexPlace* texPlace, TrackSection* section, int16_t face, TexTemplate* texTemplate, int16_t copy);
void TexPlaceControl(TrackCamera* camera, int16_t* face, Object* texHighlight, TrackSection* nextSection);
Object* CreateTexHighlight();

#endif
