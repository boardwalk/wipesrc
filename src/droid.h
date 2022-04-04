#ifndef DROID_H
#define DROID_H

#include <stdint.h>
#include "libgte.h"
#include "object.h"
#include "ships.h"
#include "track.h"

typedef struct RescueData
{
	TrackSection 	*section;
	int16_t 			modelID;
	VECTOR 			pos;
	VECTOR 			vel;
	VECTOR 			acc;
	int16_t 			hdg, pitch, roll;
	int16_t 			vhdg, vpitch, vroll;
	int16_t				updateCount;
	void 				(*droidUpdate)(struct RescueData*, ShipData*, Object**);
} RescueData;

void UpdateRescueDroid		(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, struct WeaponData *weaponIndex);
void InitRescueDroid			(RescueData *rescueDroid, Object** shipShapes, ShipData *shipIndex);
void UpdateDroidStart		(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes);
void UpdateDroidCircle		(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes);
void SetDroidVectors			(RescueData *rescueDroid, Object** shipShapes);
void UpdateDroidRescue		(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes);

#endif
