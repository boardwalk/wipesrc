#ifndef COLLIDE_H
#define COLLIDE_H

#include <stdint.h>
#include "libgte.h"
#include "object.h"
#include "ships.h"
#include "track.h"

#define COLLIDE_MODEL 16
#define NOSE_COLLISION 		1
#define TAIL_COLLISION 		2
#define R_WING_COLLISION 	3
#define L_WING_COLLISION 	4

void  CallShip2ShipCollision  (ShipData *shipIndex, Object** shipShapes);
void  Ship2ShipCollision   	(ShipData *shipIndex, Object** shipShapes, int32_t, int32_t);
int32_t   collideLineToPlane   	(int32_t shipNo1, int32_t shipNo2, Object** shipShapes, ShipData *shipIndex);
int32_t   assign               	(VECTOR *vec1, VECTOR vec2);
int32_t   PlaneLineCollision   	(VECTOR plane_point, VECTOR ship_pos, SVector normal);
void  Ship2Track           	(ShipData *playerShip, Face *facePtr, Object** shipShapes );
void  WingCollision        	(ShipData *playerShip, Face *facePtr, int32_t direction, int32_t alpha);
void  NoseCollision        	(ShipData *playerShip, Face *facePtr, int32_t direction, int32_t alpha);
void  Reflection           	(VECTOR *incidence, SVector normal);
int32_t   AngleOfCollision     	(SVECTOR normal, VECTOR ship_point);
int32_t   CheckPolygonCollision	(Face *facePtr, VECTOR pos, int32_t alpha);

#endif
