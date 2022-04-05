/* (C) Psygnosis 1994. By Jason Carl Denton
*/

#include <stdio.h>
#include "malloc.h"
#include "object.h"
#include "skeleton.h"
#include "global.h"
#include "libgte.h"
#include "small_ptr.h"

Skeleton* NewSkeleton() {
  Skeleton* skeleton;

  skeleton = (Skeleton*)DAlloc(heap, sizeof(Skeleton));
  if (!skeleton) {
    /* Memory allocation failed! */

    printf("Skeleton.c:SkeletonRoot(): Failed to allocate skeleton\n");
    return NULL;
  }

  /* Set pointers */

  skeleton->super = NULL;
  skeleton->sub = NULL;
  skeleton->next = NULL;

  /* Set translation matrix */

  skeleton->relative.t[0] = 0;
  skeleton->relative.t[1] = 0;
  skeleton->relative.t[2] = 0;

  /* Set rotation matrix top row */

  skeleton->relative.m[0][0] = 4096;
  skeleton->relative.m[0][1] = 0;
  skeleton->relative.m[0][2] = 0;

  /* Set rotation matrix middle row */

  skeleton->relative.m[1][0] = 0;
  skeleton->relative.m[1][1] = 4096;
  skeleton->relative.m[1][2] = 0;

  /* Set rotation matrix bottom row */

  skeleton->relative.m[2][0] = 0;
  skeleton->relative.m[2][1] = 4096;
  skeleton->relative.m[2][2] = 0;

  /* Set flag to indicate absolute matrix needs recalculating */

  skeleton->update = 1;

  return skeleton;
}

void SetSkeletonPosition(Skeleton* skeleton, int32_t x, int32_t y, int32_t z) {
  skeleton->relative.t[0] = x;
  skeleton->relative.t[1] = y;
  skeleton->relative.t[2] = z;

  /* Mark skeleton for update */

  skeleton->update = 1;
}

void SetSkeletonDirection(Skeleton* skeleton, int16_t xa, int16_t ya, int16_t za) {
  int16_t sx, sy, sz;
  int16_t cx, cy, cz;

  sx = rsin(xa);
  sy = rsin(ya);
  sz = rsin(za);

  cx = rcos(xa);
  cy = rcos(ya);
  cz = rcos(za);

  /* Top Row */

  skeleton->relative.m[0][0] = (cy * cz) >> 12;
  skeleton->relative.m[0][1] = (-cy * sz) >> 12;
  skeleton->relative.m[0][2] = -sy;

  /* Middle Row */

  skeleton->relative.m[1][0] = ((cx * sz) >> 12) - ((((sx * sy) >> 12) * cz) >> 12);
  skeleton->relative.m[1][1] = ((cx * cz) >> 12) + ((((sx * sy) >> 12) * sz) >> 12);
  skeleton->relative.m[1][2] = (-sx * cy) >> 12;

  /* Bottom Row */

  skeleton->relative.m[2][0] = ((sx * sz) >> 12) + ((((cx * sy) >> 12) * cz) >> 12);
  skeleton->relative.m[2][1] = ((sx * cz) >> 12) - ((((cx * sy) >> 12) * sz) >> 12);
  skeleton->relative.m[2][2] = (cx * cy) >> 12;

  /* Mark skeleton for update */

  skeleton->update = 1;
}

MATRIX* GetSkeletonMatrix(Skeleton* skeleton) {
  MATRIX* m1;
  MATRIX* m2;
  MATRIX* m3;

  if (!(skeleton->super)) {
    return &skeleton->relative;
  } else {
    if (skeleton->update) {
      /* Need to update the absolute matrix */

      m1 = &skeleton->relative;
      m2 = GetSkeletonMatrix(skeleton->super);
      m3 = &skeleton->absolute;

      m3->t[0] = m2->t[0] + ((m1->t[0] * m2->m[0][0] + m1->t[1] * m2->m[0][1] + m1->t[2] * m2->m[0][2]) >> 12);
      m3->t[1] = m2->t[1] + ((m1->t[0] * m2->m[1][0] + m1->t[1] * m2->m[1][1] + m1->t[2] * m2->m[1][2]) >> 12);
      m3->t[2] = m2->t[2] + ((m1->t[0] * m2->m[2][0] + m1->t[1] * m2->m[2][1] + m1->t[2] * m2->m[2][2]) >> 12);

      MulMatrix0(m2, m1, m3);

      skeleton->update = 0;
    }

    return &skeleton->absolute;
  }
}

void SetSkeletonTransform(Skeleton* skeleton) {
  MATRIX* matrix;

  matrix = GetSkeletonMatrix(skeleton);

  SetTransMatrix(matrix);
  SetRotMatrix(matrix);
}
