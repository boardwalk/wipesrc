/* (C) Psygnosis 1994. By Jason Carl Denton
*/

#include "standard.h"
#include <stdio.h>
#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "error.h"
#include "ajs.h"

Texture* LoadTexture(const char* name, int16_t translucent) {
  Tim* tim;
  TimClut4* tc4;
  TimClut8* tc8;
  Texture* texture;
  int32_t length;
  RECT rect;
  CLUT cluts[256];

  int32_t x, y;

#if LoadMessages
//   printf( "Loading Tim File : %s\n", name );
#endif

  length = FileLength(name);
  if (length <= 0) {
    sprintf(errorString, "Tim.c:LoadTexture: File %s not found\n", name);
    Error(errorString, Warning);
    return (NULL);
  }

  tim = (Tim*)DAlloc(heap, length + 32);

  if (!tim) {
    sprintf(errorString, "Tim.c:LoadTexture(): Failed to allocate memory for file %s\n", name);
    Error(errorString, Fatal);
  }

  if (LoadFile(name, (char*)tim) != length) {
    /* File Error! */

    sprintf(errorString, "Tim.c:LoadTexture(): Failed to load file %s\n", name);
    Error(errorString, Fatal);
  }

  if (ClutType(tim) == Clut4Bit) {
    tc4 = (TimClut4*)(tim);
    texture = (Texture*)DAlloc(heap, sizeof(Texture));

    texture->type = CLUT4;

    if ((!tc4->textureX) && (!tc4->textureY)) {
      tc4->clutX = clutX;
      tc4->clutY = clutY;

      tc4->clutW = 16;
      tc4->clutH = 1;

      tc4->textureX = textureX;
      tc4->textureY = textureY;

      clutX += 16;
      if (clutX >= 384) {
        clutX = 320;
        clutY += 1;
      }

      textureY += 32;
      if (textureY >= 256) {
        textureX += 8;
        textureY = 0;
      }
    }

    texture->clutX = tc4->clutX;
    texture->clutY = tc4->clutY;
    texture->clutW = tc4->clutW;
    texture->clutH = tc4->clutH;

    texture->textureX = tc4->textureX;
    texture->textureY = tc4->textureY;
    texture->textureW = tc4->textureW;
    texture->textureH = tc4->textureH;

    x = tc4->textureX - TextureHOffset(tc4->textureX);
    y = tc4->textureY - TextureVOffset(tc4->textureY);

    texture->u0 = (x << 2);
    texture->v0 = (y);
    texture->u1 = ((x + tc4->textureW) << 2) - 1;
    texture->v1 = (y);
    texture->u2 = (x << 2);
    texture->v2 = (y + tc4->textureH) - 1;
    texture->u3 = ((x + tc4->textureW) << 2) - 1;
    texture->v3 = (y + tc4->textureH) - 1;

    texture->tsb = TSB(Clut4Bit, translucent, TPAGE(texture->textureX, texture->textureY));
    texture->cba = CBA(texture->clutX >> 4, texture->clutY);

    rect.x = tc4->clutX;
    rect.y = tc4->clutY;
    rect.w = tc4->clutW;
    rect.h = tc4->clutH;
    LoadClut4(&rect, (uint16_t*)(&tc4->clut), cluts);
    DrawSync(0);

    rect.x = tc4->textureX;
    rect.y = tc4->textureY;
    rect.w = tc4->textureW;
    rect.h = tc4->textureH;
    LoadImage4(&rect, (uint32_t*)(tc4 + 1), cluts);
    DrawSync(0);
  } else if (ClutType(tim) == Clut8Bit) {
    tc8 = (TimClut8*)(tim);
    texture = (Texture*)DAlloc(heap, sizeof(Texture));

    texture->type = CLUT8;

    if ((!tc8->textureX) && (!tc8->textureY)) {
      Error("tim.c:LoadTexture(): Cannot auto place 8 bit textures", Fatal);

#if 0
		   tc8->clutX = clutX;
		   tc8->clutY = clutY;

		   tc8->clutW = 256;
		   tc8->clutH = 1;

		   tc8->textureX = textureX;
		   tc8->textureY = textureY;

		   clutY += 1;
		   if ( clutY >= 512 )
		   {
			   clutX += 256;
			   clutY = 480;
		   }

		   textureX += 8;
		   if ( textureX >= 1024 )
		   {
		 	   textureX = 320;
			   textureY += 32;
		   }
#endif
    }

    texture->clutX = tc8->clutX;
    texture->clutY = tc8->clutY;
    texture->clutW = tc8->clutW;
    texture->clutH = tc8->clutH;

    texture->textureX = tc8->textureX;
    texture->textureY = tc8->textureY;
    texture->textureW = tc8->textureW;
    texture->textureH = tc8->textureH;

    x = tc8->textureX - TextureHOffset(tc8->textureX);
    y = tc8->textureY - TextureVOffset(tc8->textureY);

    texture->u0 = (x << 1);
    texture->v0 = (y);
    texture->u1 = ((x + tc8->textureW) << 1) - 1;
    texture->v1 = (y);
    texture->u2 = (x << 1);
    texture->v2 = (y + tc8->textureH) - 1;
    texture->u3 = ((x + tc8->textureW) << 1) - 1;
    texture->v3 = (y + tc8->textureH) - 1;

    texture->tsb = TSB(Clut8Bit, translucent, TPAGE(texture->textureX, texture->textureY));
    texture->cba = CBA(texture->clutX >> 4, texture->clutY);

    rect.x = tc8->clutX;
    rect.y = tc8->clutY;
    rect.w = tc8->clutW;
    rect.h = tc8->clutH;
    LoadClut8(&rect, (uint16_t*)(&tc8->clut), cluts);
    DrawSync(0);

    rect.x = tc8->textureX;
    rect.y = tc8->textureY;
    rect.w = tc8->textureW;
    rect.h = tc8->textureH;
    LoadImage8(&rect, (uint32_t*)(tc8 + 1), cluts);
    DrawSync(0);
  } else {
    texture = NULL;
  }

  DFree(heap, (char*)tim);

  return (texture);
}
