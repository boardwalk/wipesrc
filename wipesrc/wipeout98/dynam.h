#define GRAVITY 			80000								/* Gravitational force 					- 800    */
#define MIN_RESISTANCE 	20	 /* 12 */
#define MAX_RESISTANCE 	74
#define VELOCITY_SHIFT 	6
#define TRACK_MAGNET		64	/* 64 */
#define MIN_WING_SPEED 	2000
#define TARGETHEIGHT 	256

#define COLLIDE 0x1
#define OUT 0
#define IN 1
#define  frac(x,y)     (((x)*(y))>>12)	 /* 9 */


void shipTrkReaction		(ShipData *playerShip, Object** shipShapes);
void getWorldPoint		(VECTOR *output, Object* object, Object* objectCollide,int32_t pointNo );
void GetTargetPos			(VECTOR *result, VECTOR pos, Vector cent1, Vector cent2);
void SetUnitVectors		(ShipData *shipIndex);
int32_t SetTrackAttributes	(TrackSection *section,
							  	int16_t i,
							  	WeaponGrid *weaponGrid,
							  	int32_t noWeaponGrids);
