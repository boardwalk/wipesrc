/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/
#ifndef SCENE_H
#define SCENE_H

#include <stdint.h>
#include "object.h"

#define ToIntel 0
#define ToUnix 1

Object* LoadPrm(const char* name, int16_t textureStart);

void IntelObject(char* start, char* finish, int16_t convert);

void IntelShort(int16_t* svalue);

void IntelLong(int32_t* lvalue);

#endif
