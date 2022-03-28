#include <stdlib.h>
#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "dynam.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "search.h" // PCwipeout
#include "utils.h"
#include "libgte.h"

static short target = 0;

void InitCamera(TrackCamera *camera, TrackSection *section)
{
	int i;
	VECTOR pos;
	
	camera->section = section;
	for(i = 0; i < 10; i++)
	{
		camera->section = camera->section->nextSection.ptr;
	}
	pos.vx = camera->section->centre.vx;
	pos.vy = camera->section->centre.vy;
	pos.vz = camera->section->centre.vz;
	SetSkeletonPosition( camera->camPos, -pos.vx, -pos.vy, -pos.vz );
	camera->hdg = 0;
	camera->pitch = 0;
	camera->roll = 0;
	
	camera->vel.vx = 0;
	camera->vel.vy = 0;
	camera->vel.vz = 0;
	
}

void UpdateCameraRaceExternal(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	VECTOR 	pos;
	VECTOR 	bestPath;
	VECTOR 	distance;
	VECTOR 	acc;
	int 		mag;
	static   int vel = 0;
	short    hdg;
	short    pitch;
	
	TrackSection *nextSection;
			 
	pos.vx = camShip->ppivot.vx - (camShip->unitVecNose.vx>>2);
	pos.vy = camShip->ppivot.vy - (camShip->unitVecNose.vy>>2);	 
	pos.vz = camShip->ppivot.vz - (camShip->unitVecNose.vz>>2);
	pos.vy -= 200;
	
	camera->section = (TrackSection*)FindNearestSection(camera->section, pos);
	
	nextSection = camera->section->nextSection.ptr;
	
	
	GetTargetPos(&bestPath, pos, nextSection->centre, camera->section->centre);
	
	distance.vx = pos.vx - bestPath.vx;
	distance.vy = pos.vy - bestPath.vy;
	distance.vz = pos.vz - bestPath.vz;
	
	acc.vx = distance.vx;
	acc.vy = distance.vy;	 
	acc.vz = distance.vz;
	
	acc.vx = (acc.vx * 60) / FR60;
	acc.vy = (acc.vy * 60) / FR60;	 
	acc.vz = (acc.vz * 60) / FR60;
	
	camera->vel.vx -= acc.vx>>6;
	camera->vel.vy -= acc.vy>>6;	 
	camera->vel.vz -= acc.vz>>6;
	
	camera->vel.vx -= sar(camera->vel.vx,3);
	camera->vel.vy -= sar(camera->vel.vy,3);
	camera->vel.vz -= sar(camera->vel.vz,3);
	
	pos.vx += camera->vel.vx;
	pos.vy += camera->vel.vy;
	pos.vz += camera->vel.vz;
	
	mag = GetMagnitude(distance);
	
	vel += (mag>>7);
	vel  -= vel >> 4;
	pos.vy -= vel;
	
	SetSkeletonPosition( camera->camPos, -pos.vx, -pos.vy, -pos.vz );
	
	camera->pitch = -camShip->pitch;
	camera->roll = 0;
	
	camera->hdg = -( camShip->hdg );
	SetSkeletonDirection( camera->camPos->super, camera->pitch, camera->hdg, 0 ); 
	
	camera->camPos->super->update = 1;
}

void UpdateCameraRaceInternal(TrackCamera *camera, ShipData *camShip, RescueData* rescueDroid)
{
	short    sx, sy, sz;
	short    cx, cy, cz;
	VECTOR   unitFinVector;
	
	camera->section = camShip->nearTrkSect;
	
	sy = rsin( camShip->hdg );
	sx = rsin( camShip->pitch );
	sz = rsin( camShip->roll );
	cy = rcos( camShip->hdg );
	cx = rcos( camShip->pitch );
	cz = rcos( camShip->roll );
	
	unitFinVector.vx = ((cy * sz) >> 12) - ((((sy * sx) >> 12) * cz) >> 12);
	unitFinVector.vy = (cx * cz) >> 12;
	unitFinVector.vz = ((sy * sz) >> 12) + ((((cy * sx) >> 12) * cz) >> 12);
	
	SetSkeletonPosition(camera->camPos, -(camShip->ppivot.vx - (unitFinVector.vx>>5)),
		-(camShip->ppivot.vy - (unitFinVector.vy>>5)),
		-(camShip->ppivot.vz - (unitFinVector.vz>>5)));
	
	camera->pitch = -camShip->pitch;
	camera->hdg = -camShip->hdg;
	camera->roll = -camShip->roll;
	SetSkeletonDirectionRPH(camera->camPos->super, -camShip->roll, -camShip->pitch, -camShip->hdg); 
	
	camera->camPos->super->update = 1;
}

void UpdateCameraRaceStart(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	VECTOR pos;
	short    sx, sy, sz;
	short    cx, cy, cz;
	VECTOR   targetVector;
	
	/*** Set to final position ***/
	pos.vx = camShip->ppivot.vx - (camShip->unitVecNose.vx>>2);
	pos.vy = camShip->ppivot.vy - (camShip->unitVecNose.vy>>2);
	pos.vz = camShip->ppivot.vz - (camShip->unitVecNose.vz>>2);
	
	pos.vx += ((rsin((camShip->updateCount - SWITCH_TO_RACE_VIEW)*3)) );
	pos.vy -= (2 * (camShip->updateCount - SWITCH_TO_RACE_VIEW)) + 200;
	pos.vz += ((rsin((camShip->updateCount - SWITCH_TO_RACE_VIEW)*3)) );
	
	if(camShip->updateCount == START_TIME)
	{
		camera->section = camShip->nearTrkSect;
	}
	else
	{
		camera->section = (TrackSection*)FindNearestSection(camera->section, pos);
	}
	
	SetSkeletonPosition( camera->camPos, -pos.vx, -pos.vy, -pos.vz );
	
	camera->pitch = -sar( camShip->pitch, 1 );
	
	targetVector.vx = camShip->ppivot.vx - pos.vx;
	targetVector.vy = camShip->ppivot.vy - pos.vy;
	targetVector.vz = camShip->ppivot.vz - pos.vz;
	
	camera->hdg = ratan2( targetVector.vx, targetVector.vz );
	
	SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0); 
	
	camera->camPos->super->update = 1;
	
	if(camShip->updateCount == SWITCH_TO_RACE_VIEW)
	{
		camShip->attr |= VIEW_IN;
		CameraUpdate = UpdateCameraRaceInternal;	 
	}	
}

void UpdateCameraAttractCircle(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	VECTOR pos;
	int hdg, pitch, temp;
	VECTOR targetVector;
	short    sx, sy, sz;
	short    cx, cy, cz;
	VECTOR   unitFinVector;
	
	camera->section = camShip->nearTrkSect;
	
	if(target == 0)
	{								
		target = 150;
	}
	target--;
	if(target == 0)
	{
		CameraUpdate = UpdateCameraAttractMode;
	}
	
	sy = rsin( camShip->hdg );
	sx = rsin( camShip->pitch );
	sz = rsin( camShip->roll );
	cy = rcos( camShip->hdg );
	cx = rcos( camShip->pitch );
	cz = rcos( camShip->roll );
	
	unitFinVector.vx = ((cy * sz) >> 12) - ((((sy * sx) >> 12) * cz) >> 12);
	unitFinVector.vy = (cx * cz) >> 12;
	unitFinVector.vz = ((sy * sz) >> 12) + ((((cy * sx) >> 12) * cz) >> 12);
	
	pos.vx = camShip->ppivot.vx + (rsin(camShip->hdg)/6);
	pos.vy = camShip->ppivot.vy + ((camShip->pitch>>2)-200);
	pos.vz = camShip->ppivot.vz - (rcos(camShip->hdg)/6);
	
	pos.vx += rsin(target<<3)>>3;
	pos.vy -= 200;
	pos.vz += rcos(target<<3)>>3;
	
	pos.vx -= unitFinVector.vx>>4;
	pos.vy -= unitFinVector.vy>>4;
	pos.vz -= unitFinVector.vz>>4;
	
	targetVector.vx = camShip->ppivot.vx - pos.vx;
	targetVector.vy = camShip->ppivot.vy - pos.vy;
	targetVector.vz = camShip->ppivot.vz - pos.vz;
	
	temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);
	
	SetSkeletonPosition( camera->camPos, -pos.vx, -pos.vy, -pos.vz );
	
	camera->hdg = ratan2(targetVector.vx, targetVector.vz);
	camera->pitch = ratan2(targetVector.vy, temp);
	SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0); 
	
	camera->camPos->super->update = 1;
	
}

void UpdateCameraDroidView(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	VECTOR  pos;
	int            hdg, pitch, temp;
	VECTOR         targetVector;
	TrackSection   *section;
	VECTOR         av;
	
	pos.vx = camera->section->centre.vx + 300;
	pos.vy = camera->section->centre.vy - 1500;
	pos.vz = camera->section->centre.vz + 300;
	
	av.vx = rescueDroid->pos.vx;
	av.vy = rescueDroid->pos.vy;
	av.vz = rescueDroid->pos.vz;
	
	targetVector.vx = av.vx - pos.vx;
	targetVector.vy = av.vy - pos.vy;
	targetVector.vz = av.vz - pos.vz;
	
	temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);
	
	SetSkeletonPosition( camera->camPos, -pos.vx, -pos.vy, -pos.vz );
	
	camera->pitch = ratan2( targetVector.vy, temp );
	camera->hdg = ratan2( targetVector.vx, targetVector.vz );
	SetSkeletonDirection( camera->camPos->super, camera->pitch, camera->hdg, 0 ); 
	
	camera->camPos->super->update = 1;
}



void UpdateCameraAttractInternal(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	short    sx, sy, sz;
	short    cx, cy, cz;
	VECTOR   unitFinVector;
	
	
	if(target == 0)
	{								
		target = 150;
	}
	target--;
	if(target == 0)
	{
		CameraUpdate = UpdateCameraAttractMode;
	}
	
	camera->section = camShip->nearTrkSect;
	
	sy = rsin( camShip->hdg );
	sx = rsin( camShip->pitch );
	sz = rsin( camShip->roll );
	cy = rcos( camShip->hdg );
	cx = rcos( camShip->pitch );
	cz = rcos( camShip->roll );
	
	unitFinVector.vx = ((cy * sz) >> 12) - ((((sy * sx) >> 12) * cz) >> 12);
	unitFinVector.vy = (cx * cz) >> 12;
	unitFinVector.vz = ((sy * sz) >> 12) + ((((cy * sx) >> 12) * cz) >> 12);
	
	SetSkeletonPosition(camera->camPos, -(camShip->ppivot.vx - (unitFinVector.vx>>5)),
		-(camShip->ppivot.vy - (unitFinVector.vy>>5)),
		-(camShip->ppivot.vz - (unitFinVector.vz>>5)));
	
	camera->pitch = -camShip->pitch;
	camera->hdg = -camShip->hdg;
	SetSkeletonDirectionRPH(camera->camPos->super, -camShip->roll, -camShip->pitch, -camShip->hdg); 
	
	camera->camPos->super->update = 1;
}


void UpdateCameraStaticFollow(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	static VECTOR pos;
	int hdg, pitch, temp;
	VECTOR targetVector;
	TrackSection *section;
	int i;
	
	if(target == 0)
	{								
		section = camShip->nearTrkSect->nextSection.ptr;
		for(i = 0; i < 10; i++)
		{
			section = section->nextSection.ptr;
		}		
		
		camera->section = section;
		
		pos.vx = section->centre.vx;
		pos.vy = section->centre.vy - 500;
		pos.vz = section->centre.vz;
		target = 150;
	}
	target--;
	if(target == 0)
	{
		CameraUpdate = UpdateCameraAttractMode;
	}
	targetVector.vx = camShip->ppivot.vx - pos.vx;
	targetVector.vy = camShip->ppivot.vy - pos.vy;
	targetVector.vz = camShip->ppivot.vz - pos.vz;
	
	temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);
	
	SetSkeletonPosition( camera->camPos, -pos.vx, -pos.vy, -pos.vz );
	
	camera->hdg = ratan2(targetVector.vx, targetVector.vz);
	camera->pitch = ratan2(targetVector.vy, temp);
	SetSkeletonDirection(camera->camPos->super, camera->pitch, camera->hdg, 0); 
	
	camera->camPos->super->update = 1;
}

void UpdateCameraAttractMode(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	switch(rand()%2)
	{
	case 0: 
		CameraUpdate = UpdateCameraAttractCircle;
		camShip->attr &= ~VIEW_IN;
		target = 0;
		break;
		
	case 1: 
		CameraUpdate = UpdateCameraStaticFollow;
		camShip->attr &= ~VIEW_IN;
		target = 0;
		break;
	}
	(CameraUpdate)(camera, camShip, rescueDroid);
}

void UpdateCameraEditorStep(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid)
{
	short          sx, sy, sz;
	short          cx, cy, cz;
	VECTOR         unitFinVector;
	VECTOR         pos;
	Face           *facePtr;
	
	facePtr = track->faces + camera->section->faceStart;
	while((facePtr->flags & TRACK_BASE) == 0)
	{
		facePtr++ ;
	}
	
	pos.vx = camera->section->centre.vx;
	pos.vy = camera->section->centre.vy;
	pos.vz = camera->section->centre.vz;
	
	pos.vx += ((facePtr->normal.vx*200)>>12);
	pos.vy += ((facePtr->normal.vy*200)>>12);
	pos.vz += ((facePtr->normal.vz*200)>>12);
	
	SetSkeletonPosition(camera->camPos, -pos.vx, -pos.vy, -pos.vz);
	
	SetSkeletonDirectionRPH(camera->camPos->super, 0, camera->pitch, camera->hdg); 
	camera->camPos->super->update = 1;
}
