#include "standard.h"
#include <stdlib.h>

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "dynam.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "droid.h"
#include "camera.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "libgte.h"
#include "utils.h"

#include "libetc_markk.h"		// DLS



/**************************************************************************/
/****					Rescue droid control update routine						  ****/
/**************************************************************************/
void UpdateRescueDroid(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData *weaponIndex)
{
	ShipData 		*playerShip;
	TrackSection 	*section, *nextSection;
	static VECTOR 	target;
	VECTOR			targetVector;
	VECTOR			distance;
	int32_t 				i;

	playerShip = &(shipIndex[shipNo]);
    playerShip->attr |= FLYING;

	if(playerShip->updateCount == 500)
	{
		section = playerShip->nearTrkSect->prevSection.ptr;
		section = section->prevSection.ptr;
		section = section->prevSection.ptr;
		section = section->prevSection.ptr;
		for(i = 0; i < 10; i++)
		{
			section = section->nextSection.ptr;
			if(section->flags & Jump)
			{
				break;
			}
		}
		playerShip->nearTrkSect = section;
		nextSection = section->nextSection.ptr;

		target.vx = (section->centre.vx + nextSection->centre.vx)>>1;
		target.vy = ((section->centre.vy + nextSection->centre.vy)>>1) - 2000;
		target.vz = (section->centre.vz + nextSection->centre.vz)>>1;

		playerShip->vpivot.vx = 0;
  		playerShip->vpivot.vy = 0;
		playerShip->vpivot.vz = 0;

		playerShip->updateCount--;

	}

	nextSection = playerShip->nearTrkSect->nextSection.ptr;


	if(playerShip->attr & IN_TOW)
	{
		target.vx += (nextSection->centre.vx - target.vx)>>7;
		target.vy += (nextSection->centre.vy - target.vy)>>7;
		target.vz += (nextSection->centre.vz - target.vz)>>7;

#if PAL
		playerShip->vpivot.vx = ((target.vx - playerShip->ppivot.vx)*60)/50;
		playerShip->vpivot.vy = ((target.vy - playerShip->ppivot.vy)*60)/50;
		playerShip->vpivot.vz = ((target.vz - playerShip->ppivot.vz)*60)/50;
#else
		playerShip->vpivot.vx = ((target.vx - playerShip->ppivot.vx)*60)/FR60;
		playerShip->vpivot.vy = ((target.vy - playerShip->ppivot.vy)*60)/FR60;
		playerShip->vpivot.vz = ((target.vz - playerShip->ppivot.vz)*60)/FR60;
#endif

		targetVector.vx = nextSection->centre.vx - playerShip->nearTrkSect->centre.vx;
		targetVector.vy = nextSection->centre.vy - playerShip->nearTrkSect->centre.vy;
		targetVector.vz = nextSection->centre.vz - playerShip->nearTrkSect->centre.vz;

		playerShip->vhdg = -ratan2(targetVector.vx, targetVector.vz) - playerShip->hdg ;
		if(playerShip->vhdg > 2048) playerShip->vhdg = playerShip->vhdg - 4096;
		else if(playerShip->vhdg < -2048) playerShip->vhdg = playerShip->vhdg + 4096;
		playerShip->hdg += playerShip->vhdg>>6;
		playerShip->hdg   = ang(playerShip->hdg);


	}

	playerShip->vpivot.vx -= playerShip->vpivot.vx>>4;
  	playerShip->vpivot.vy -= playerShip->vpivot.vy>>4;
	playerShip->vpivot.vz -= playerShip->vpivot.vz>>4;

	playerShip->ppivot.vx += playerShip->vpivot.vx>>5;
	playerShip->ppivot.vy += playerShip->vpivot.vy>>5;
	playerShip->ppivot.vz += playerShip->vpivot.vz>>5;

	distance.vx = playerShip->ppivot.vx - playerShip->nearTrkSect->centre.vx;
	distance.vy = playerShip->ppivot.vy - playerShip->nearTrkSect->centre.vy;
	distance.vz = playerShip->ppivot.vz - playerShip->nearTrkSect->centre.vz;

	playerShip->pitch -= playerShip->pitch>>3;
	playerShip->roll -= playerShip->roll>>5;


  	/**** Needs PAL ****/
	if((playerShip->attr & IN_TOW) && (GetMagnitude(distance) < 800))
	{
//		if(neg1.head == 8960)
//		{
//			playerShip->update = UpdatePlayerShipNeg;
//		}
//		else
//		{
//			playerShip->update = UpdatePlayerShipNorm;
//
//		}

		if(etc_InputDeviceSelect == _KEYBOARD)
			playerShip->update = UpdatePlayerShipNorm;
		else if(etc_InputDeviceSelect == _MOUSE1)
			playerShip->update = UpdatePlayerShipMouse;
		else if(etc_InputDeviceSelect == _JOYSTICK1)
			playerShip->update = UpdatePlayerShipJoystick;

		playerShip->updateCount = 0;
		playerShip->attr &= ~RESCUE;
	}
}


/**************************************************************************/
/****					Rescue droid control update routine						  ****/
/**************************************************************************/
void UpdateDroidStart(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes)
{
#if 1
	if(rescueDroid->updateCount == 800)
	{
		PlayDirectionalNote(START_SIREN, 0, -1, rescueDroid->pos);
	}
	else
		AdjustDirectionalNote(START_SIREN, 0, 0, rescueDroid->pos);
#endif
	rescueDroid->updateCount--;

	if(rescueDroid->updateCount < 400)
	{
	 	rescueDroid->acc.vx = (-rsin(rescueDroid->hdg) * rcos(rescueDroid->pitch))>>14;
		rescueDroid->acc.vy = 0;
	 	rescueDroid->acc.vz = (rcos(rescueDroid->hdg) * rcos(rescueDroid->pitch))>>14;

		rescueDroid->vhdg = 0;

	}

	else if(rescueDroid->updateCount < 710)
	{
	 	rescueDroid->acc.vx = (-rsin(rescueDroid->hdg) * rcos(rescueDroid->pitch))>>15;
		rescueDroid->acc.vy = -140;
	 	rescueDroid->acc.vz = (rcos(rescueDroid->hdg) * rcos(rescueDroid->pitch))>>15;

		rescueDroid->vhdg = -8;

	}

	else if(rescueDroid->updateCount == 770)
	{
		rescueDroid->acc.vy -= 90;
		rescueDroid->vhdg = 8;

	}

	SetDroidVectors(rescueDroid, shipShapes);

	if(rescueDroid->updateCount == 0)
	{
	 	KillNote(START_SIREN);
		rescueDroid->updateCount = 800;
		rescueDroid->droidUpdate = UpdateDroidCircle;
		rescueDroid->pos.vx = rescueDroid->section->centre.vx;
		rescueDroid->pos.vy = -3000;
		rescueDroid->pos.vz = rescueDroid->section->centre.vz;
	}


}

void UpdateDroidCircle(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes)
{
	TrackSection* 		nextSection;
	VECTOR 				targetVector;
	VECTOR				target;
	int16_t					targetHdg;
	int16_t					quickestTurn, turn;

	nextSection = rescueDroid->section->nextSection.ptr;

	target.vx = (rescueDroid->section->centre.vx + nextSection->centre.vx) >> 1;
	target.vy = rescueDroid->section->centre.vy - 3000;
	target.vz = (rescueDroid->section->centre.vz + nextSection->centre.vz) >> 1;

   targetVector.vx = target.vx - rescueDroid->pos.vx;
	targetVector.vy = target.vy - rescueDroid->pos.vy;
	targetVector.vz = target.vz - rescueDroid->pos.vz;

	targetHdg = -ratan2( targetVector.vx, targetVector.vz );

	quickestTurn =  targetHdg - rescueDroid->hdg;

	if(rescueDroid->hdg < 0)
		turn =  targetHdg - (rescueDroid->hdg + 4096);
	else
		turn =  targetHdg - (rescueDroid->hdg - 4096);

	if(abs(turn) < abs(quickestTurn))
	{
		rescueDroid->vhdg = turn >> 6;
	}
	else
	{
		rescueDroid->vhdg = quickestTurn >> 6;
	}

	rescueDroid->acc.vx = (-rsin(rescueDroid->hdg) * rcos(rescueDroid->pitch))>>15;
	rescueDroid->acc.vy = targetVector.vy>>6;
	rescueDroid->acc.vz = (rcos(rescueDroid->hdg) * rcos(rescueDroid->pitch))>>15;

	if(shipIndex->attr & RESCUE)
	{
		rescueDroid->droidUpdate = UpdateDroidRescue;
		rescueDroid->updateCount = 800;
		PlayNote(TRACTOR_BEAM, 0, -1);
		CameraUpdate = UpdateCameraDroidView;
		shipIndex->attr |= REMOTE_VIEW;
		if(shipIndex->nearTrkSect->flags & Jump)
			cameraPtr->section = shipIndex->nearTrkSect->nextSection.ptr;
		else
			cameraPtr->section = shipIndex->nearTrkSect;
	}

	SetDroidVectors(rescueDroid, shipShapes);

	AdjustDirectionalNote(START_SIREN, 0, 0, rescueDroid->pos);

}

void UpdateDroidRescue(RescueData *rescueDroid, ShipData *shipIndex, Object** shipShapes)
{
	VECTOR 			target;
	VECTOR 			distance;

	TrackSection* nextSection;

	if(rescueDroid->updateCount == 800)
	{

		if((rescueDroid->section == shipIndex->nearTrkSect) ||
			(rescueDroid->section == shipIndex->nearTrkSect->prevSection.ptr))
		{
			shipIndex->attr &= ~IN_TOW;

		}
		else
		{
			rescueDroid->section = shipIndex->nearTrkSect;
			nextSection = rescueDroid->section->nextSection.ptr;

			rescueDroid->pos.vx = (rescueDroid->section->centre.vx + nextSection->centre.vx) >> 1;
			rescueDroid->pos.vy = rescueDroid->section->centre.vy - 3000;
			rescueDroid->pos.vz = (rescueDroid->section->centre.vz + nextSection->centre.vz) >> 1;
			shipIndex->attr &= ~IN_TOW;
		}

		rescueDroid->vel.vx = 0;
		rescueDroid->vel.vy = 0;
		rescueDroid->vel.vz = 0;

		rescueDroid->acc.vx = 0;
		rescueDroid->acc.vy = 0;
		rescueDroid->acc.vz = 0;

		rescueDroid->updateCount = 799;
	}

	target.vx = shipIndex->ppivot.vx;
	target.vy = shipIndex->ppivot.vy - 350;
	target.vz = shipIndex->ppivot.vz;

	rescueDroid->vhdg = 0;
	rescueDroid->hdg = shipIndex->hdg;

	distance.vx = (target.vx - rescueDroid->pos.vx);
	distance.vy = (target.vy - rescueDroid->pos.vy);
	distance.vz = (target.vz - rescueDroid->pos.vz);

	rescueDroid->vel.vx = (distance.vx)<<4;
	rescueDroid->vel.vy = (distance.vy)<<4;
	rescueDroid->vel.vz = (distance.vz)<<4;

	if((rescueDroid->vel.vx < 129) && (rescueDroid->vel.vx > 0)) rescueDroid->vel.vx	= 129;
	if((rescueDroid->vel.vy < 129) && (rescueDroid->vel.vy > 0)) rescueDroid->vel.vy	= 129;
	if((rescueDroid->vel.vz < 129) && (rescueDroid->vel.vz > 0)) rescueDroid->vel.vz	= 129;

	if((rescueDroid->vel.vx > -129) && (rescueDroid->vel.vx < 0)) rescueDroid->vel.vx	= -129;
	if((rescueDroid->vel.vy > -129) && (rescueDroid->vel.vy < 0)) rescueDroid->vel.vy	= -129;
	if((rescueDroid->vel.vz > -129) && (rescueDroid->vel.vz < 0)) rescueDroid->vel.vz	= -129;

	AdjustDirectionalNote(START_SIREN, 0, 0, rescueDroid->pos);

	if(shipIndex->attr & IN_TOW)
	{

		rescueDroid->acc.vx = 0;
		rescueDroid->acc.vy = 0;
		rescueDroid->acc.vz = 0;

		rescueDroid->vel.vx = 0;
		rescueDroid->vel.vy = 0;
		rescueDroid->vel.vz = 0;

		rescueDroid->pos.vx = target.vx;
		rescueDroid->pos.vy = target.vy;
		rescueDroid->pos.vz = target.vz;
	}

	if((GetMagnitude(distance) < 1) && ((shipIndex->attr & IN_TOW) == 0))
	{
		shipIndex->attr |= IN_TOW;

		rescueDroid->acc.vx = 0;
		rescueDroid->acc.vy = 0;
		rescueDroid->acc.vz = 0;

		rescueDroid->vel.vx = 0;
		rescueDroid->vel.vy = 0;
		rescueDroid->vel.vz = 0;

		rescueDroid->pos.vx = target.vx;
		rescueDroid->pos.vy = target.vy;
		rescueDroid->pos.vz = target.vz;

	}

	if((shipIndex->attr & RESCUE) == 0)
	{
	 	KillNote(START_SIREN);
		KillNote(TRACTOR_BEAM);
		rescueDroid->droidUpdate = UpdateDroidCircle;
		rescueDroid->updateCount = 800;
		shipIndex->attr &= ~REMOTE_VIEW;

		if(shipIndex->attr & VIEW_IN)
			CameraUpdate = UpdateCameraRaceInternal;
	  	else
			CameraUpdate = UpdateCameraRaceExternal;

		while((rescueDroid->section->flags & Jump) == 0)
		{
		  rescueDroid->section = rescueDroid->section->prevSection.ptr;
		}
	}

	SetDroidVectors(rescueDroid, shipShapes);

}



void SetDroidVectors(RescueData *rescueDroid, Object** shipShapes)
{
	rescueDroid->vel.vx += rescueDroid->acc.vx;
	rescueDroid->vel.vy += rescueDroid->acc.vy;
	rescueDroid->vel.vz += rescueDroid->acc.vz;

	rescueDroid->vel.vx -= rescueDroid->vel.vx>>3;
	rescueDroid->vel.vy -= rescueDroid->vel.vy>>3;
	rescueDroid->vel.vz -= rescueDroid->vel.vz>>3;

	rescueDroid->pos.vx += rescueDroid->vel.vx>>6;
	rescueDroid->pos.vy += rescueDroid->vel.vy>>6;
	rescueDroid->pos.vz += rescueDroid->vel.vz>>6;

	rescueDroid->hdg += rescueDroid->vhdg;
	rescueDroid->hdg = ang(rescueDroid->hdg);
}



void InitRescueDroid(RescueData *rescueDroid, Object** shipShapes, ShipData *shipIndex)
{

	rescueDroid->section = track->sections;

	while(1)
	{
		rescueDroid->section = rescueDroid->section->nextSection.ptr;
		if(rescueDroid->section->flags & Jump)
			break;
	}

	rescueDroid->pos.vx = shipIndex->ppivot.vx;
	rescueDroid->pos.vy = shipIndex->ppivot.vy - 200;
	rescueDroid->pos.vz = shipIndex->ppivot.vz;

	rescueDroid->vel.vx = 0;
	rescueDroid->vel.vy = 0;
	rescueDroid->vel.vz = 0;

	rescueDroid->acc.vx = 0;
	rescueDroid->acc.vy = 0;
	rescueDroid->acc.vz = 0;

	rescueDroid->hdg 	 = shipIndex->hdg;
	rescueDroid->pitch = shipIndex->pitch;
	rescueDroid->roll  = shipIndex->roll;

	rescueDroid->vhdg   = 0;
	rescueDroid->vpitch = 0;
	rescueDroid->vroll  = 0;

	rescueDroid->updateCount = 800;

	rescueDroid->modelID = RESCUE_DROID;

	rescueDroid->hdg = 0;
	rescueDroid->pitch = 0;
	rescueDroid->roll = 0;

	rescueDroid->droidUpdate = UpdateDroidStart;

}
