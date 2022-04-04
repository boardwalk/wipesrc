#ifndef SPLINE_H
#define SPLINE_H

#include <stdint.h>
#include "droid.h"
#include "libgte.h"
#include "object.h"
#include "ships.h"
#include "track.h"
#include "weapon.h"

TrackSection* FindAllNearestSection(VECTOR pos);

void UpdateShipRemote(ShipData* shipIndex, Object** shipShapes, int32_t shipNo, WeaponData* weaponIndex);
void UpdateShipLinearInterpolateSpline(ShipData* shipIndex, Object** shipShapes, int32_t shipNo, WeaponData* weaponIndex);

void UpdateCameraLinearInterpolateSpline(TrackCamera* camera, ShipData* ship, RescueData* rescueDroid);
void UpdateCameraHighUp(TrackCamera* camera, ShipData* ship, RescueData* rescueDroid);

void UpdateCameraExtroAttract(TrackCamera* camera, ShipData* camShip, RescueData* rescueDroid);

void InitFireCongratulations();
void InitIceCongratulations();

void ShowFireCongratulations();
void ShowIceCongratulations();

#endif
