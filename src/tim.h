/* (C) Psygnosis 1994. By Jason Carl Denton
*/
#ifndef TIM_H
#define TIM_H

#include <stdint.h>
/* DEFINES FOR DRAWING PRIMITIVE */

#define POLYGON 0x20
#define LINE 0x40
#define SPRITE 0x60

#define GOURAUD 0x10
#define QUAD 0x08
#define TEXTURE 0x04
#define TRANSPARENT 0x02
#define NOBRIGHT 0x01

#define DOUBLE 0x02
#define NOLIGHT 0x01

#define CLUT4 0x00
#define CLUT8 0x01
#define TRUE15 0x10

typedef struct
{
  int16_t type;
  int16_t tsb;
  int16_t cba;
  int16_t clutX;
  int16_t clutY;
  int16_t clutW;
  int16_t clutH;
  int16_t textureX;
  int16_t textureY;
  int16_t textureW;
  int16_t textureH;
  int16_t u0;
  int16_t v0;
  int16_t u1;
  int16_t v1;
  int16_t u2;
  int16_t v2;
  int16_t u3;
  int16_t v3;
} Texture;

typedef struct Tim {
  /* Header */

  int32_t id;

  /* Flags */

  int32_t flags;

} Tim;

typedef struct TimClut8 {
  /* Header */

  int32_t id;

  /* Flags */

  int32_t flags;

  /* Clut */

  int32_t clutBytes;

  int16_t clutX;
  int16_t clutY;
  int16_t clutW;
  int16_t clutH;

  int16_t clut[256];

  /* Tetxure */

  int32_t textureBytes;

  int16_t textureX;
  int16_t textureY;
  int16_t textureW;
  int16_t textureH;

} TimClut8;

typedef struct TimClut4 {
  /* Header */

  int32_t id;

  /* Flags */

  int32_t flags;

  /* Clut */

  int32_t clutBytes;

  int16_t clutX;
  int16_t clutY;
  int16_t clutW;
  int16_t clutH;

  int16_t clut[16];

  /* Tetxure */

  int32_t textureBytes;

  int16_t textureX;
  int16_t textureY;
  int16_t textureW;
  int16_t textureH;

} TimClut4;

#define Clut4Bit 0
#define Clut8Bit 1
#define Clut15Bit 2

#define NoClut 0
#define HasClut 1

#define TPAGE(x, y) ((x >> 6) + ((y & 0x100) >> 4))
#define CBA(x, y) ((y << 6) | x)
#define TSB(c, a, t) ((c << 7) | (a << 5) | t)

#define TextureHOffset(x) (x & 0xffc0)
#define TextureVOffset(y) (y & 0x100)

#define ClutType(t) (t->flags & 7)

/* Forward Procedure Declarations */

Texture* LoadTexture(const char* name, int16_t translucent);

#endif
