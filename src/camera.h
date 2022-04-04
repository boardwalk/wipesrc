#ifndef CAMERA_H
#define CAMERA_H

#include "droid.h"
#include "ships.h"
#include "track.h"

#define DEBUG_CAM 0
#define COLLIDE_TEST 0


void InitCamera						(TrackCamera *camera, TrackSection *section);
void UpdateCameraRaceExternal		(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraRaceInternal		(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraRaceStart			(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraAttractCircle	(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraAttractInternal	(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraStaticFollow		(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraAttractMode		(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraEditorStep   	(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraDroidView			(TrackCamera*, ShipData *camShip, RescueData*);
void UpdateCameraCollideTest		(TrackCamera*, ShipData *camShip, RescueData* );
void SetCameraToRedPoly       	(TrackCamera *camera, Face *facePtr);

#endif
