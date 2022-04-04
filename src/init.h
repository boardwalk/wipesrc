#ifndef INIT_H
#define INIT_H

#include <stdint.h>
#include "menus.h"
#include "object.h"

int16_t InitScene(Object** prm, int16_t *prmCount, Skeleton* camPos, ConfigData *gameData);
void getPath(char* trackPath, char trackNo, const char* name);
int16_t LoadCompressedTextureSequence(const char* fileName);

#endif
