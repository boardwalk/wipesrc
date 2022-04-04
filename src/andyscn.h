#ifndef ANDYSCN_H
#define ANDYSCN_H

#include <stdint.h>
#include "object.h"

void DrawObjectFixedAsm(Object* object, Skeleton* camera, int32_t depth);
void DrawObjectAsm(Object* object, Skeleton* camera, int32_t roll, int16_t ShipObj);

#endif
