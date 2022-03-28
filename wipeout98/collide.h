#define COLLIDE_MODEL 16
#define NOSE_COLLISION 		1
#define TAIL_COLLISION 		2
#define R_WING_COLLISION 	3
#define L_WING_COLLISION 	4

void  CallShip2ShipCollision  (ShipData *shipIndex, Object** shipShapes);
void  Ship2ShipCollision   	(ShipData *shipIndex, Object** shipShapes, int, int);
int   collideLineToPlane   	(int shipNo1, int shipNo2, Object** shipShapes, ShipData *shipIndex);
int   assign               	(VECTOR *vec1, VECTOR vec2);
int   PlaneLineCollision   	(VECTOR plane_point, VECTOR ship_pos, SVector normal);
void  Ship2Track           	(ShipData *playerShip, Face *facePtr, Object** shipShapes );
void  WingCollision        	(ShipData *playerShip, Face *facePtr, int direction, int alpha);
void  NoseCollision        	(ShipData *playerShip, Face *facePtr, int direction, int alpha);
void  Reflection           	(VECTOR *incidence, SVector normal);
int   AngleOfCollision     	(SVECTOR normal, VECTOR ship_point);
int   CheckPolygonCollision	(Face *facePtr, VECTOR pos, int alpha);
