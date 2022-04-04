/***** Weapon Defines *****/
#ifndef WEAPON_H
#define WEAPON_H

#include <stdint.h>
#include "libgpu.h"
#include "libgte.h"
#include "object.h"
#include "ships.h"
#include "standard.h"
#include "track.h"

#define NO_OF_WEAPONS 29

#define NO_OF_MINES 15
#define NO_OF_ROCKETS 4
#define NO_OF_MISSILES 4
#define NO_OF_SHIELDS 3
#define NO_OF_EBOLTS 3

#define MINE_OFFSET 4
#define ROCKET_OFFSET 0
#define MISSILE_OFFSET 19
#define SHIELD_OFFSET 23
#define EBOLT_OFFSET 26

#if PAL
#define MINE_DURATION 350
#define ROCKET_DURATION 150
#define ELEC_DURATION 100
#define REV_CON_DURATION 50
#define MISSILE_DURATION 150
#define SHIELD_DURATION 150
#define FLARE_DURATION 150
#define SPECIAL_DURATION 300
#else
#define MINE_DURATION (45*FR10)
#define ROCKET_DURATION (20*FR10)
#define ELEC_DURATION (14*FR10)
#define REV_CON_DURATION (6*FR10)
#define MISSILE_DURATION (20*FR10)
#define SHIELD_DURATION (20*FR10)
#define FLARE_DURATION (20*FR10)
#define SPECIAL_DURATION (40*FR10)
#endif

#define MINE_RELEASE_RATE 3
#if PAL
#define	WEAPON_DELAY 33
#else
#define WEAPON_DELAY (FR40)
#endif

#define NO_WEAPON 0

#define ROCKET 	1
#define MISSILE 	2
#define MINE 		3
#define SPECIAL	4
#define ELEC_BOLT 5
#define FLARE 		6
#define REV_CON 	7
#define SHIELD 	8
#define TURBO 		9

#define NO_OF_WEAPON_TYPES 9

#define SHIP_EXPLODE -1
#define TRACK_EXPLODE -2
#define NON_ACTIVE 0

#define NO_ACTIVE_WEAPONS 20

/***** Weapon Grid defines *****/

#define DEACTIVATED 0
#define ACTIVE 1

typedef struct WeaponShape
{
	Object* 			shapePtr;
	int16_t				used;
} WeaponShape;

typedef struct WeaponData
{
	int16_t 			count;
	int16_t				currentWeapon;
	TrackSection 	*nearTrkSect;
	VECTOR 			acc;
	VECTOR 			vel;
	VECTOR 			pos;
	int16_t				hdg, pitch;
	int8_t		targetShip;
	char 				fireShip;
	char				type;
	char				availible;
	void (*updateWeapon)(ShipData*, WeaponShape*, struct WeaponData*, struct WeaponData*);
} WeaponData;

typedef struct WeaponGrid
{
	TrackSection 	*section;
	int16_t 			attr;
	int16_t 			cycleCount;
	int32_t				count;
	CVECTOR			colour;
} WeaponGrid;

void UpdateWeapons	(ShipData *shipIndex,WeaponShape* weaponShapes, WeaponData *weaponIndex );
void LaunchWeapon		(ShipData *shipIndex, WeaponData *weaponIndex, int32_t shipNo);
void FireWeapon		(ShipData*, WeaponData*, int32_t, char);
void MissileLock		(ShipData *playerShip, ShipData *shipIndex, Object** shipShapes, POLY_FT4*);
void initWeapons		(WeaponShape* weaponShapes, Skeleton* camPos, WeaponData *weaponIndex, POLY_FT4* );

void UpdateRocket		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void InitRocket		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void UpdateMissile	(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void InitMissile		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void UpdateMine		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void InitMine			(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void InitElecBolt		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void UpdateElecBolt	(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void InitShield		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void UpdateShield		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void InitRevCon		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void UpdateRevCon		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);
void InitSpecial		(ShipData *playerShip, WeaponShape*, WeaponData*, WeaponData*);

void TransformTransparentObject( Object* object, Skeleton* camera, int32_t shieldID);
void Weapon2Mine(WeaponData *weapon, WeaponData *weaponIndex, WeaponShape *weaponShapes);
void GetFireVector(VECTOR *result, VECTOR target, Face *facePtr, VECTOR shipPos);
void CycleWeaponGrids(ShipData *shipIndex, WeaponGrid *weaponGrid, int32_t noWeaponGrids);
void DrawWeapons(ShipData *shipIndex,WeaponShape* weaponShapes, WeaponData *weaponIndex );
void DrawTargetIcon(ShipData *playerShip, ShipData *shipIndex, Object** shipShapes, POLY_FT4 *target);
void SetTargetShip(ShipData *shipIndex, int32_t shipNo);
int32_t GetNewWeapon();
int32_t GetNewWeaponBodge();

#endif
