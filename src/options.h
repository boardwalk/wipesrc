#ifndef OPTIONS_H
#define OPTIONS_H

#include <stdint.h>
#include "libgpu.h"
#include "menus.h"

#define FADE_INACTIVE 0
#define FADE_ACTIVE 1
#define FADE_COMPLETE 2
#define FADE_OUT 3

typedef struct
{
  int16_t fadeCount;
  char fadeFlag;
} FadeData;

void InGameReset(int32_t choice, uint32_t*, char, ConfigData*, int32_t*);
void InGameConfirm(char* confirm, int32_t choice, int16_t xpad, char* lConfirm, uint32_t*);
void DeSelectScreen(POLY_F4*, DR_MODE*);
void ScreenFade(POLY_F4*, DR_MODE*, FadeData*);

#endif
