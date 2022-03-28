typedef struct RescueData
{
	TrackSection 	*section;
	short 			modelID;
	VECTOR 			pos;
	VECTOR 			vel;
	VECTOR 			acc;
	short 			hdg, pitch, roll;
	short 			vhdg, vpitch, vroll;
	short				updateCount;
	void 				(*droidUpdate)(struct RescueData*, ShipData*, Object**);
} RescueData;

void UpdateRescueDroid		(ShipData *shipIndex, Object** shipShapes, int shipNo, struct WeaponData *weaponIndex);
void InitRescueDroid			(RescueData *rescueDroid, Object** shipShapes, ShipData *shipIndex);
void UpdateDroidStart		(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes);
void UpdateDroidCircle		(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes);
void SetDroidVectors			(RescueData *rescueDroid, Object** shipShapes);
void UpdateDroidRescue		(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes);
