#define NEGCON 0x23
// #define NEGCON 1	  // PCwipeout
#define NORMALPAD 0
#define HRP 0
#if PAL
#define JUST_FRONT_DELAY        125
#define JUST_BEHIND_DELAY       166
#define IN_SIGHT_DELAY          83
#define STALL_DELAY                     75
#define START_TIME                      166
#define THREE                                   125
#define RED_LIGHT                               83
#define AMBER_LIGHT                     41
#define RACE_START                      0
#else
#define JUST_FRONT_DELAY        (15*FR10)
#define JUST_BEHIND_DELAY       (20*FR10)
#define IN_SIGHT_DELAY          (20*FR10)
#define STALL_DELAY                     (9 * FR10)
#define START_TIME                      (20*FR10)
#define THREE                                   (15*FR10)
#define RED_LIGHT                               (10*FR10)
#define AMBER_LIGHT			(5*FR10)
#define RACE_START 			0
#endif



#define SWITCH_TO_RACE_VIEW 100

#define STALL 					-1

typedef struct NegconBuff
{
	u_short	head;
	u_short	bits;
	char		twist;
	char		i;
	char		ii;
	char		lShift;
} NegconBuff;



void  UpdateShips(ShipData *shipIndex, WeaponShape *weaponShapes, Object** shipShapes, WeaponData*);
void  UpdateShipsSerial(ShipData *shipIndex, WeaponShape *weaponShapes, Object** shipShapes, WeaponData *weaponIndex);
int  	InitController();
void  GeneralStartProcs(ShipData *playerShip);

void  UpdatePlayerShipNeg			(ShipData*, Object**, int, WeaponData*);
void  UpdatePlayerShipNorm			(ShipData*, Object**, int, WeaponData*);
void  UpdatePlayerShipMouse			(ShipData*, Object**, int, WeaponData*);
void  UpdatePlayerShipJoystick		(ShipData*, Object**, int, WeaponData*);
void  UpdateRemoteShip				(ShipData*, Object**, int, WeaponData*);
void  UpdatePlayerShipNormStart		(ShipData*, Object**, int, WeaponData*);
void  UpdatePlayerShipNegStart		(ShipData*, Object**, int, WeaponData*);
void  UpdateRemoteShipStart			(ShipData*, Object**, int, WeaponData*);
void  UpdateShipHoldLeft			(ShipData*, Object**, int, WeaponData*);
void  UpdateRescuDroid				(ShipData*, Object**, int, WeaponData*);

void 	HoldCentre						(VECTOR *result, ShipData *shipIndex, Face *facePtr, int);
void	HoldRight						(VECTOR *result, ShipData *shipIndex, Face *facePtr, int);
void 	HoldLeft							(VECTOR *result, ShipData *shipIndex, Face *facePtr, int);
void 	BlockShip						(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int);
void 	AvoidShip						(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int);
void 	AvoidOtherShips				(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int);
void 	ZigZagShip						(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int);

void SetSkeletonDirectionHRP( Skeleton* skeleton, short hdg, short roll, short pitch  );
void SetSkeletonDirectionHPR( Skeleton* skeleton, short hdg, short pitch, short roll  );
void SetSkeletonDirectionRPH( Skeleton* skeleton, short roll, short pitch, short hdg  );

void Mouse2Neg(void);
void Joy2Neg(void);
void DoDetailKeys(void);
