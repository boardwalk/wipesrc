/* (C) Psygnosis 1994. By Jason Carl Denton
*/
#ifndef SKELETON_H
#define SKELETON_H

#include <stdint.h>
#include "libgte.h"
#include "object.h"

Skeleton* NewSkeleton();

void SetSkeletonPosition(Skeleton* skeleton, int32_t x, int32_t y, int32_t z);

void SetSkeletonDirection(Skeleton* skeleton, int16_t xa, int16_t ya, int16_t za);

MATRIX* GetSkeletonMatrix(Skeleton* skeleton);

void SetSkeletonTransform(Skeleton* skeleton);

#endif
