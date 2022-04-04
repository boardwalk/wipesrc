#ifndef CONTROL_H
#define CONTROL_H

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
	uint16_t	head;
	uint16_t	bits;
	char		twist;
	char		i;
	char		ii;
	char		lShift;
} NegconBuff;



void  UpdateShips(ShipData *shipIndex, WeaponShape *weaponShapes, Object** shipShapes, WeaponData*);
void  UpdateShipsSerial(ShipData *shipIndex, WeaponShape *weaponShapes, Object** shipShapes, WeaponData *weaponIndex);
int32_t  	InitController();
void  GeneralStartProcs(ShipData *playerShip);

void  UpdatePlayerShipNeg			(ShipData*, Object**, int32_t, WeaponData*);
void  UpdatePlayerShipNorm			(ShipData*, Object**, int32_t, WeaponData*);
void  UpdatePlayerShipMouse			(ShipData*, Object**, int32_t, WeaponData*);
void  UpdatePlayerShipJoystick		(ShipData*, Object**, int32_t, WeaponData*);
void  UpdateRemoteShip				(ShipData*, Object**, int32_t, WeaponData*);
void  UpdatePlayerShipNormStart		(ShipData*, Object**, int32_t, WeaponData*);
void  UpdatePlayerShipNegStart		(ShipData*, Object**, int32_t, WeaponData*);
void  UpdateRemoteShipStart			(ShipData*, Object**, int32_t, WeaponData*);
void  UpdateShipHoldLeft			(ShipData*, Object**, int32_t, WeaponData*);
void  UpdateRescuDroid				(ShipData*, Object**, int32_t, WeaponData*);

void 	HoldCentre						(VECTOR *result, ShipData *shipIndex, Face *facePtr, int32_t);
void	HoldRight						(VECTOR *result, ShipData *shipIndex, Face *facePtr, int32_t);
void 	HoldLeft							(VECTOR *result, ShipData *shipIndex, Face *facePtr, int32_t);
void 	BlockShip						(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t);
void 	AvoidShip						(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t);
void 	AvoidOtherShips				(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t);
void 	ZigZagShip						(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t);

void SetSkeletonDirectionHRP( Skeleton* skeleton, int16_t hdg, int16_t roll, int16_t pitch  );
void SetSkeletonDirectionHPR( Skeleton* skeleton, int16_t hdg, int16_t pitch, int16_t roll  );
void SetSkeletonDirectionRPH( Skeleton* skeleton, int16_t roll, int16_t pitch, int16_t hdg  );

void Mouse2Neg();
void Joy2Neg();
void DoDetailKeys();

#endif
