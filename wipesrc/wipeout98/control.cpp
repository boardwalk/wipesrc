#include "standard.h"
#include <stdio.h>
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
#include "utils.h"
#include "libgte.h"
#include "search.h"
#include "effects.h"
#include "win.h"
#include "collide.h"

//#include "markk\libetc.h"		// DLS
// #include "dos.h"
//#include "graph.h"
#include "wipesrc/win95/markk/input.h"

extern int16_t ft,screenres;


extern int16_t behindSpeed[];
extern int16_t shipOrder[];

#define FULL		0
#define HIGH		3
#define	MEDIUM 		2
#define LOW 		1
#define SMALL		0

extern char	 ScreenSize;
extern char	 Resolution;
extern char  PersCorr;
extern char  PersCorrLvl;

extern int16_t JoyMinX, JoyMaxX;
extern int16_t JoyMinY, JoyMaxY;
extern int16_t JoyCentreX, JoyCentreY;
extern int16_t left_scale, right_scale;
extern float left_scale_factor, right_scale_factor;
extern int16_t JoyCentreFudge;


/**********************************************************************/
/***					Call update routine for each ship					  ***/
/**********************************************************************/
void UpdateShips(ShipData *shipIndex, WeaponShape *weaponShapes, Object** shipShapes, WeaponData *weaponIndex)
{
	int32_t 		i;


	SetUnitVectors(shipIndex);

	if(raceType == HEAD_TO_HEAD)
	{
		(shipIndex[ownShip].update)(shipIndex, shipShapes, ownShip, weaponIndex);
	}
	else
	{
		for(i = 0; i < NO_OF_SHIPS; i++)
		{
			if (i != serialShip)
			{
				(shipIndex[i].update)(shipIndex, shipShapes, i, weaponIndex);
			}
		}
	}
}



void UpdateShipsSerial(ShipData *shipIndex, WeaponShape *weaponShapes, Object** shipShapes, WeaponData *weaponIndex)
{
	int16_t    sx, sy, sz;
	int16_t    cx, cy, cz;


	sy = rsin( shipIndex[ownShip].hdg );
	sx = rsin( shipIndex[ownShip].pitch );
	sz = rsin( shipIndex[ownShip].roll );
	cy = rcos( shipIndex[ownShip].hdg );
	cx = rcos( shipIndex[ownShip].pitch );
	cz = rcos( shipIndex[ownShip].roll );

	shipIndex[ownShip].unitVecNose.vx = -frac(sy,cx);
	shipIndex[ownShip].unitVecNose.vy = -sx;
	shipIndex[ownShip].unitVecNose.vz = frac(cy,cx);

	shipIndex[ownShip].unitVecWing.vx = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
	shipIndex[ownShip].unitVecWing.vy = -(sz * cx) >> 12;
	shipIndex[ownShip].unitVecWing.vz = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);

	shipIndex[ownShip].prevShipSect = shipIndex[ownShip].nearTrkSect;
	SectionSearch(&(shipIndex[ownShip]));

	shipIndex[ownShip].prevSecNo = shipIndex[ownShip].prevShipSect->secNo;
	shipIndex[ownShip].nearSecNo = shipIndex[ownShip].nearTrkSect->secNo;

	(shipIndex[ownShip].update)(shipIndex, shipShapes, ownShip, weaponIndex);
}



/**********************************************************************/
/***			  Initialise Controller and determine type  				  ***/
/**********************************************************************/
int32_t InitController()
{
	InitPAD(&neg1, 8, &neg2, 0);
	StartPAD();
	//	ChangeClearPAD(0);
	//  	VSync(0);
	if(neg1.head == 8960)
	{
		return(NEGCON);
	}
	else return(NORMALPAD);
}


/**************************************************************************/
/****																						  ****/
/****									START ROUTINES									  ****/
/****																						  ****/
/**************************************************************************/

void GeneralStartProcs(ShipData *playerShip)
{
	static 	int32_t vc;

	if(playerShip->updateCount == START_TIME)
	{
		PlayNote(REMOTE, 0, -1);		 /*** Reserve channel for remote craft ***/
		chanTable[vagTable[REMOTE].channelID].s_attr.volume.left = 0;
		chanTable[vagTable[REMOTE].channelID].s_attr.volume.right = 0;

		PlayNote(REMOTE2, 0, -1);	 	 /*** Reserve channel for remote craft ***/
		chanTable[vagTable[REMOTE2].channelID].s_attr.volume.left = 0;
		chanTable[vagTable[REMOTE2].channelID].s_attr.volume.right = 0;
	}

	if(playerShip->updateCount == START_TIME)
	{
		PlayNote(ENG_RUMBLE, 0, -1);
		SetVolume(ENG_RUMBLE, ENG_RUMBLE_VOL, ENG_RUMBLE_VOL);

		PlayNote(ENG_AIR_INTAKE, 0, -1);
		SetVolume(ENG_AIR_INTAKE, 0, 0);

		PlayNote(ENG_THRUST, 0, -1);
		SetVolume(ENG_THRUST, 50, 50);

		PlayNote(REMOTE_ENG, 0, -1);
		SetVolume(REMOTE_ENG, 0, 0);

		PlayNote(TURBULENCE, 0, -1);
		SetVolume(TURBULENCE, 0, 0);

	}

	playerShip->updateCount--;

	if(playerShip->updateCount == RACE_START )
	{
		ColourStartBoom(3);
		PlayNote(SP_GO, 1, 30);
		SetPitch(SP_GO, 0x1000);
	}
	else if(playerShip->updateCount <= AMBER_LIGHT )
	{
		ColourStartBoom(2);
	}
	else if(playerShip->updateCount <= RED_LIGHT )
	{
		ColourStartBoom(1);
	}

	if(playerShip->updateCount == AMBER_LIGHT )
	{
		PlayNote(SP_1, 1, 30);
		SetPitch(SP_1, 0x1000);
	}
	else if(playerShip->updateCount == RED_LIGHT )
	{
		PlayNote(SP_2, 1, 30);
		SetPitch(SP_2, 0x1000);
	}
	else if(playerShip->updateCount == THREE )
	{
		PlayNote(SP_3, 1, 30);
		SetPitch(SP_3, 0x1000);
	}

}


/**************************************************************************/
/****					Start control routine for normal pad					  ****/
/**************************************************************************/


void UpdatePlayerShipNormStart(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData* weaponIndex)
{
	ShipData 		*playerShip;
	static int32_t			startPos;


	playerShip = &(shipIndex[shipNo]);

	GeneralStartProcs(playerShip);

	if(playerShip->updateCount == START_TIME-1)
		startPos = playerShip->ppivot.vy;

	playerShip->ppivot.vy = startPos + (rsin(playerShip->updateCount*80) >> 7);

	if(playerShip->updateCount == 0)
	{
		if(playerShip->attr & RACING)
		{
			playerShip->updateCount = STALL_DELAY;

			if(etc_InputDeviceSelect == _KEYBOARD)
				playerShip->update = UpdatePlayerShipNorm;
			else if(etc_InputDeviceSelect == _MOUSE1)
				playerShip->update = UpdatePlayerShipMouse;
			else if(etc_InputDeviceSelect == _JOYSTICK1)
				playerShip->update = UpdatePlayerShipJoystick;
		}
		else
			playerShip->update = UpdateRemoteShip;
	}

	SetBrake(playerShip, pCon.lBrake, pCon.rBrake);
	DoDetailKeys();

#ifdef PORT_MOUSE
	if(etc_InputDeviceSelect == _MOUSE1)
	{
		if(etc_Mouse1.mX != 128 || etc_Mouse1.mY != 100)
		{
			etc_regs.w.ax = 0x0004;			// Centre mouse
			etc_regs.w.cx = 160;
			etc_regs.w.dx = 100;
			int386(_MOUSE_INT, &etc_regs, &etc_regs);
		}
	}
#else
	if(etc_InputDeviceSelect == _MOUSE1)
	{
		centreMouse();
	}
#endif
	if(playerShip->attr & RACING)
		SetView(playerShip, pCon.view);

	if(etc_InputDeviceSelect == _JOYSTICK1)
		Joy2Neg();

    if ((pad & pCon.thrust) || (pad & nCon.thrust))
		playerShip->thrust_mag += (FR15U+1);
	else if (playerShip->thrust_mag > FR15U) playerShip->thrust_mag -= (FR15U+1);

	if(playerShip->thrust_mag > playerShip->max_thrust)
		playerShip->thrust_mag = playerShip->max_thrust;
}



/**************************************************************************/
/****					Race control update routine for normal pad			  ****/
/**************************************************************************/
void UpdatePlayerShipNorm(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData *weaponIndex)
{
	ShipData 		*playerShip;
	static int32_t 		maxThrust;
	static int16_t	revLeft = 0;
	static int16_t	revRight = 0;


	playerShip = &(shipIndex[shipNo]);

	if(playerShip->electroCount > 0)
		playerShip->electroCount--;

	if(playerShip->electroCount == 0)
		playerShip->attr &= ~ELECTROED;

	if(playerShip->revConCount > 0)
		playerShip->revConCount--;

	if(playerShip->revConCount == 0)
		playerShip->combatAttr &= ~REVCONNED;

	if(playerShip->specialCount > 0)
		playerShip->specialCount--;

	if(playerShip->specialCount == 0)
		playerShip->combatAttr &= ~SPECIALED;

	if((playerShip->combatAttr & REVCONNED) && (revLeft == 0))
	{
		pCon.left 		= PadRight0;
		pCon.right 		= PadLeft0;
		revLeft 			= pCon.lBrake;
		revRight			= pCon.rBrake;
		pCon.lBrake 	= revRight;
		pCon.rBrake 	= revLeft;
	}
	else if(((playerShip->combatAttr & REVCONNED) == 0) && (revLeft != 0))
	{
		pCon.left 		= PadLeft0;
		pCon.right 		= PadRight0;
		pCon.lBrake 	= revLeft;
		pCon.rBrake 	= revRight;
		revLeft 			= 0;
		revRight 		= 0;
	}

	if (pad & pCon.left)		/** Inc = 128 , max 2048 **/
	{
		if(playerShip->vhdg >= 0) playerShip->vhdg += playerShip->headingInc;
		else if(playerShip->vhdg < 0) playerShip->vhdg += (playerShip->headingInc<<1);
	}
	else if (pad & pCon.right)
	{
		if(playerShip->vhdg <= 0) playerShip->vhdg -= playerShip->headingInc;
		else if(playerShip->vhdg > 0) playerShip->vhdg -= (playerShip->headingInc<<1);
	}
	else
	{
		if(playerShip->vhdg > 0) playerShip->vhdg -= playerShip->headingInc;
		else if(playerShip->vhdg < 0) playerShip->vhdg += playerShip->headingInc;
	}
	if(playerShip->vhdg < -playerShip->maxHeading) playerShip->vhdg = -playerShip->maxHeading;
	if(playerShip->vhdg > playerShip->maxHeading) playerShip->vhdg = playerShip->maxHeading;


	if(playerShip->attr & ELECTROED)
	{
		if(playerShip->attr & VIEW_IN)
			SetShake(2);

		playerShip->vhdg += (randy()%600)-300;
	}

	if (pad & pCon.down) playerShip->vpitch += FR30U;
	if (pad & pCon.up) playerShip->vpitch -= FR30U;

	/**** Check for Stall and set thrust ***/
	if(playerShip->updateCount == STALL_DELAY)
	{
		if((playerShip->thrust_mag >= 680) &&
			(playerShip->thrust_mag <= 700))
		{
			playerShip->thrust_mag = 1800;
			maxThrust = 1800;
		}
		else if(playerShip->thrust_mag < 680)
			maxThrust = playerShip->max_thrust;

		else 	maxThrust = 200;
	}

	if(playerShip->updateCount > 0)
	{
		playerShip->updateCount--;
	}
	else
		maxThrust = playerShip->max_thrust;

	if(maxThrust < 500) maxThrust += (randy()%11)>>1;


	if (pad & pCon.thrust)
	{
		playerShip->thrust_mag += (FR15U+1);
	}
	else if (playerShip->thrust_mag > FR7U) playerShip->thrust_mag -= (FR7U+1);

	if(playerShip->thrust_mag > maxThrust)
		playerShip->thrust_mag = maxThrust;

	if((playerShip->attr & ELECTROED) && ((randy() % 80) == 0))
		playerShip->thrust_mag -= playerShip->thrust_mag >> 2;

	DoDetailKeys();			// DLS

	SetBrake(playerShip, pCon.lBrake, pCon.rBrake);
	SetView(playerShip, pCon.view);
	SetFire(playerShip, pCon.fire, weaponIndex, shipNo);
	shipTrkReaction(playerShip, shipShapes);
}



/**************************************************************************/
/****					Start control routine for negcon pad					  ****/
/**************************************************************************/
void UpdatePlayerShipNegStart(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData* weaponIndex)
{
	ShipData 			*playerShip;
	static int32_t 			viewPad;
	int32_t					thrust;
	static int32_t			startPos;
	int32_t					targetThrust;
	int16_t 				thrustVal;
	static 				char negView = 0;


	playerShip = &(shipIndex[shipNo]);

	GeneralStartProcs(playerShip);

	if(playerShip->updateCount == START_TIME-1)
	{
		startPos = playerShip->ppivot.vy;
	}

	playerShip->ppivot.vy = startPos + (rsin(playerShip->updateCount*80) >> 7);

	if(playerShip->updateCount == 0)
	{
		if(playerShip->attr & RACING)
		{
			playerShip->updateCount = STALL_DELAY;
			playerShip->update = UpdatePlayerShipNeg;
		}
		else
			playerShip->update = UpdateRemoteShip;

	}

	/**************************************/
	/* Read Air Brake control information */
	/**************************************/
	if (neg1.lShift > 200)	playerShip->r_brake_rot += 32;
	else if(playerShip->r_brake_rot > 0) playerShip->r_brake_rot -= 32;
	if (playerShip->r_brake_rot > 256)
		playerShip->r_brake_rot = 256;
	if ((neg1.bits & NEG_R_SHIFT) == 0)	playerShip->l_brake_rot += 32;
	else if(playerShip->l_brake_rot > 0) playerShip->l_brake_rot -= 32;
	if (playerShip->l_brake_rot > 256)
		playerShip->l_brake_rot = 256;


	/**************************************/
	/* Read view control information      */
	/**************************************/
	if(nCon.type & D_VIEW)
	{
		if ((xpad & nCon.view))
		{
			if((playerShip->attr & VIEW_IN) == 0)
			{
				CameraUpdate = UpdateCameraRaceInternal;
				playerShip->attr |= VIEW_IN;
			}
			else
			{
				CameraUpdate = UpdateCameraRaceExternal;
				playerShip->attr &= ~VIEW_IN;
			}
		}
	}
	else
	{
		if((neg1.ii > 100) && (negView <= 100))
		{
			negView = neg1.ii;
			if((playerShip->attr & VIEW_IN) == 0)
			{
				CameraUpdate = UpdateCameraRaceInternal;
				playerShip->attr |= VIEW_IN;
			}
			else
			{
				CameraUpdate = UpdateCameraRaceExternal;
				playerShip->attr &= ~VIEW_IN;
			}
		}
		else
			negView = neg1.ii;
	}

	if((nCon.type & A_THRUST_I) || (nCon.type & A_THRUST_II))
	{
		if(nCon.type & A_THRUST_I)
			thrustVal = neg1.i;
		else if(nCon.type & A_THRUST_II)
			thrustVal = neg1.ii;

		targetThrust = (playerShip->max_thrust * thrustVal) / 200;
		if(targetThrust > playerShip->max_thrust)
			targetThrust = playerShip->max_thrust;

		if(thrustVal > 0)
		{
			if (targetThrust >= playerShip->thrust_mag)
			{
				if((targetThrust - playerShip->thrust_mag) < (FR15U+1))
					thrust = targetThrust - playerShip->thrust_mag;
				else
					thrust = (FR15U+1);

				playerShip->thrust_mag += thrust;
			}
			else
			{
				playerShip->thrust_mag -= (FR15U+1);
			}

		}
		else if (playerShip->thrust_mag > FR15U) playerShip->thrust_mag -= (FR15U+1);
	}

	else
	{

		if (pad & nCon.thrust)
		{
			playerShip->thrust_mag += (FR15U+1);
		}
		else if (playerShip->thrust_mag > FR15U) playerShip->thrust_mag -= (FR15U+1);
	}

	if(playerShip->thrust_mag > playerShip->max_thrust)
		playerShip->thrust_mag = playerShip->max_thrust;

}



/**************************************************************************/
/****					Race control update routine for negcon		   ****/
/**************************************************************************/
void UpdatePlayerShipNeg(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData *weaponIndex)
{
	ShipData 		*playerShip;
	static int32_t 		weaponPad;
	static int32_t 		viewPad;
	int32_t				thrust;
	int32_t 				targetThrust;
	int16_t				targetVhdg;
	static 			char negView = 0;
	static 			char negFireI = 0;
	static 			char negFireII = 0;
	static int32_t 		maxThrust;
	int16_t				thrustVal;


	playerShip = &(shipIndex[shipNo]);

	if(playerShip->electroCount > 0)
		playerShip->electroCount--;

	if(playerShip->electroCount == 0)
		playerShip->attr &= ~ELECTROED;

	if(playerShip->revConCount > 0)
		playerShip->revConCount--;

	if(playerShip->revConCount == 0)
		playerShip->combatAttr &= ~REVCONNED;

	if(playerShip->specialCount > 0)
		playerShip->specialCount--;

	if(playerShip->specialCount == 0)
		playerShip->combatAttr &= ~SPECIALED;

	if(playerShip->attr & ELECTROED)
	{
		if(playerShip->attr & VIEW_IN)
			SetShake(2);

		playerShip->vhdg += (randy()%600)-300;
	}

	if((neg1.twist < 134) && (neg1.twist > 122)) neg1.twist = 128;
	else if(neg1.twist >= 134) neg1.twist -= 6;
	else if(neg1.twist <= 122) neg1.twist += 6;

	if(nCon.twistFactor == 0)
		printf("WARNING DIVISION BY ZERO : Control.c\n");

	if(playerShip->combatAttr & REVCONNED)
		targetVhdg = ((neg1.twist - 128)*(playerShip->maxHeading+400)<<1)/nCon.twistFactor ;
	else
		targetVhdg = (-(neg1.twist - 128)*(playerShip->maxHeading+400)<<1)/nCon.twistFactor ;

	if( targetVhdg > playerShip->vhdg)
	{
		if((targetVhdg - playerShip->vhdg) <= playerShip->headingInc) playerShip->vhdg += targetVhdg - playerShip->vhdg;
		else if(playerShip->vhdg >= 0) playerShip->vhdg += playerShip->headingInc;
		else if(playerShip->vhdg < 0) playerShip->vhdg += (playerShip->headingInc<<1);
	}
	else if( targetVhdg < playerShip->vhdg	)
	{
		if((targetVhdg - playerShip->vhdg) >= -playerShip->headingInc) playerShip->vhdg += targetVhdg - playerShip->vhdg;
		if(playerShip->vhdg <= 0) playerShip->vhdg -= playerShip->headingInc;
		else if(playerShip->vhdg > 0) playerShip->vhdg -= (playerShip->headingInc<<1);
	}
	if(playerShip->vhdg < -playerShip->maxHeading) playerShip->vhdg = -playerShip->maxHeading;
	if(playerShip->vhdg > playerShip->maxHeading) playerShip->vhdg = playerShip->maxHeading;

	/**** Check for Stall and set thrust ***/
	if(playerShip->updateCount == STALL_DELAY)
	{
		if((playerShip->thrust_mag >= 680) &&
			(playerShip->thrust_mag <= 700))
		{
			playerShip->thrust_mag = 1800;
			maxThrust = 1800;
		}
		else if(playerShip->thrust_mag < 680)
			maxThrust = playerShip->max_thrust;

		else 	maxThrust = 200;
	}

	if(playerShip->updateCount > 0)
	{
		playerShip->updateCount--;
	}
	else
		maxThrust = playerShip->max_thrust;

	if(maxThrust < 500) maxThrust += (randy()%11)>>1;


	if((nCon.type & A_THRUST_I) || (nCon.type & A_THRUST_II))
	{
		if(nCon.type & A_THRUST_I)
			thrustVal = neg1.i;
		else if(nCon.type & A_THRUST_II)
			thrustVal = neg1.ii;

		targetThrust = (maxThrust * thrustVal) / 200;
		if(targetThrust > maxThrust)
			targetThrust = maxThrust;

		if(thrustVal > 0)
		{
			if (targetThrust >= playerShip->thrust_mag)
			{
				if((targetThrust - playerShip->thrust_mag) < (FR15U+1))
					thrust = targetThrust - playerShip->thrust_mag;
				else
					thrust = (FR15U+1);

				playerShip->thrust_mag += thrust;
			}
			else
			{
				playerShip->thrust_mag -= (FR15U+1);
			}

		}
		else if (playerShip->thrust_mag > FR15U) playerShip->thrust_mag -= (FR15U+1);
	}

	else
	{
		if (pad & nCon.thrust)
		{
			playerShip->thrust_mag += (FR15U+1);
		}
		else if (playerShip->thrust_mag > FR15U) playerShip->thrust_mag -= (FR15U+1);
	}

	if(playerShip->thrust_mag > maxThrust)
		playerShip->thrust_mag = maxThrust;

	if((playerShip->attr & ELECTROED) && ((randy() % 80) == 0))
	{
		playerShip->thrust_mag -= playerShip->thrust_mag >> 2;
	}


	/*** Pitch ***/
	if (neg1.bits & NEG_UP) playerShip->vpitch += 30;
	if (neg1.bits & NEG_DOWN) playerShip->vpitch -= 30;

	/*** BRAKE ***/
	if (neg1.lShift > 200)	playerShip->r_brake_rot += 32;
	else if(playerShip->r_brake_rot > 0) playerShip->r_brake_rot -= 32;

	if (playerShip->r_brake_rot > 256)
		playerShip->r_brake_rot = 256;

	if ((neg1.bits & NEG_R_SHIFT) == 0)	playerShip->l_brake_rot += 32;
	else if(playerShip->l_brake_rot > 0) playerShip->l_brake_rot -= 32;

	if (playerShip->l_brake_rot > 256)
		playerShip->l_brake_rot = 256;


	if(nCon.type & D_VIEW)
	{
		if ((xpad & nCon.view))
		{
			if((playerShip->attr & VIEW_IN) == 0)
			{
				CameraUpdate = UpdateCameraRaceInternal;
				playerShip->attr |= VIEW_IN;
			}
			else
			{
				CameraUpdate = UpdateCameraRaceExternal;
				playerShip->attr &= ~VIEW_IN;
			}
		}
	}
	else
	{
		if((neg1.ii > 100) && (negView <= 100))
		{
			negView = neg1.ii;
			if((playerShip->attr & VIEW_IN) == 0)
			{
				CameraUpdate = UpdateCameraRaceInternal;
				playerShip->attr |= VIEW_IN;
			}
			else
			{
				CameraUpdate = UpdateCameraRaceExternal;
				playerShip->attr &= ~VIEW_IN;
			}
		}
		else
			negView = neg1.ii;
	}

	if(nCon.type & D_FIRE)
	{
		if ((xpad & nCon.fire) && (playerShip->weaponType != NO_WEAPON))
		{
			if((playerShip->attr & SHIELDED) == 0)
				FireWeapon(playerShip, weaponIndex, shipNo, playerShip->weaponType);
			else
			{
				PlayNote(MENU_MOVE, 1, 20);
			}
		}
	}
	else if(nCon.type & A_FIRE_II)
	{
		if((neg1.ii > 100) && (negFireII <= 100) && (playerShip->weaponType != NO_WEAPON))
		{
			negFireII = neg1.ii;
			if((playerShip->attr & SHIELDED) == 0)
			{
				FireWeapon(playerShip, weaponIndex, shipNo, playerShip->weaponType);
			}
			else
			{
				PlayNote(MENU_MOVE, 1, 20);
			}
		}
		else
			negFireII = neg1.ii;
	}
	else if(nCon.type & A_FIRE_I)
	{
		if((neg1.i > 100) && (negFireI <= 100) && (playerShip->weaponType != NO_WEAPON))
		{
			negFireI = neg1.i;
			if((playerShip->attr & SHIELDED) == 0)
			{
				FireWeapon(playerShip, weaponIndex, shipNo, playerShip->weaponType);
			}
			else
			{
				PlayNote(MENU_MOVE, 1, 20);
			}
		}
		else
			negFireI = neg1.i;
	}

	shipTrkReaction(playerShip, shipShapes);
}






/**************************************************************************/
/****					Race control update routine for mouse		   ****/
/**************************************************************************/
void UpdatePlayerShipMouse(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData *weaponIndex)
{
	ShipData 		*playerShip;
	static int32_t 		weaponPad;
	static int32_t 		viewPad;
	int16_t			targetVhdg;
	static char 	negView = 0;
	static char 	negFireI = 0;
	static char 	negFireII = 0;
	static int32_t 		maxThrust;


	playerShip = &(shipIndex[shipNo]);

	if(playerShip->electroCount > 0)
		playerShip->electroCount--;

	if(playerShip->electroCount == 0)
		playerShip->attr &= ~ELECTROED;

	if(playerShip->revConCount > 0)
		playerShip->revConCount--;

	if(playerShip->revConCount == 0)
		playerShip->combatAttr &= ~REVCONNED;

	if(playerShip->specialCount > 0)
		playerShip->specialCount--;

	if(playerShip->specialCount == 0)
		playerShip->combatAttr &= ~SPECIALED;

	if(playerShip->attr & ELECTROED)
	{
		if(playerShip->attr & VIEW_IN)
			SetShake(2);

		playerShip->vhdg += (randy()%600)-300;
	}


	// ncstuff

	Mouse2Neg();		// DLS

	if((neg1.twist < 134) && (neg1.twist > 122))
		neg1.twist = 128;
	//	else if(neg1.twist >= 134)
	//		neg1.twist -= 6;
	//	else if(neg1.twist <= 122)
	//		neg1.twist += 6;

	if(playerShip->combatAttr & REVCONNED)
		targetVhdg = ((neg1.twist - 128)*(playerShip->maxHeading+400)<<1)/nCon.twistFactor ;
	else
		targetVhdg = (-(neg1.twist - 128)*(playerShip->maxHeading+400)<<1)/nCon.twistFactor ;

	if( targetVhdg > playerShip->vhdg)
	{
		if((targetVhdg - playerShip->vhdg) <= playerShip->headingInc) playerShip->vhdg += targetVhdg - playerShip->vhdg;
		else if(playerShip->vhdg >= 0) playerShip->vhdg += playerShip->headingInc;
		else if(playerShip->vhdg < 0) playerShip->vhdg += (playerShip->headingInc<<1);
	}
	else if( targetVhdg < playerShip->vhdg	)
	{
		if((targetVhdg - playerShip->vhdg) >= -playerShip->headingInc) playerShip->vhdg += targetVhdg - playerShip->vhdg;
		if(playerShip->vhdg <= 0) playerShip->vhdg -= playerShip->headingInc;
		else if(playerShip->vhdg > 0) playerShip->vhdg -= (playerShip->headingInc<<1);
	}

	if(playerShip->vhdg < -playerShip->maxHeading)
		playerShip->vhdg = -playerShip->maxHeading;

	if(playerShip->vhdg > playerShip->maxHeading)
		playerShip->vhdg = playerShip->maxHeading;

	/**** Check for Stall and set thrust ***/
	if(playerShip->updateCount == STALL_DELAY)
	{
		if((playerShip->thrust_mag >= 680) &&
			(playerShip->thrust_mag <= 700))
		{
			playerShip->thrust_mag = 1800;
			maxThrust = 1800;
		}
		else if(playerShip->thrust_mag < 680)
			maxThrust = playerShip->max_thrust;

		else 	maxThrust = 200;
	}

	if(playerShip->updateCount > 0)
		playerShip->updateCount--;
	else
		maxThrust = playerShip->max_thrust;

	if(maxThrust < 500) maxThrust += (randy()%11)>>1;

	if (pad & nCon.thrust)
	{
		playerShip->thrust_mag += 16;
	}
	else if (playerShip->thrust_mag > 7) playerShip->thrust_mag -= 8;

	if(playerShip->thrust_mag > maxThrust)
		playerShip->thrust_mag = maxThrust;

	DoDetailKeys();			// DLS

	/*** Pitch ***/
	if (neg1.bits & NEG_UP) playerShip->vpitch += 30;
	if (neg1.bits & NEG_DOWN) playerShip->vpitch -= 30;

	if ((xpad & PadCircle0) && (playerShip->weaponType != NO_WEAPON))
	{
		if((playerShip->attr & SHIELDED) == 0)
			FireWeapon(playerShip, weaponIndex, shipNo, playerShip->weaponType);
		else
			PlayNote(MENU_MOVE, 1, 20);
	}

	SetBrake(playerShip, pCon.lBrake, pCon.rBrake);
	SetView(playerShip, pCon.view);
	shipTrkReaction(playerShip, shipShapes);
}



void UpdatePlayerShipJoystick(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData *weaponIndex)
{
	ShipData 		*playerShip;
	static int32_t 		weaponPad;
	static int32_t 		viewPad;
	int16_t			targetVhdg;
	static char 	negView = 0;
	static char 	negFireI = 0;
	static char 	negFireII = 0;
	static int32_t 		maxThrust;


	playerShip = &(shipIndex[shipNo]);

	if(playerShip->electroCount > 0)
		playerShip->electroCount--;

	if(playerShip->electroCount == 0)
		playerShip->attr &= ~ELECTROED;

	if(playerShip->revConCount > 0)
		playerShip->revConCount--;

	if(playerShip->revConCount == 0)
		playerShip->combatAttr &= ~REVCONNED;

	if(playerShip->specialCount > 0)
		playerShip->specialCount--;

	if(playerShip->specialCount == 0)
		playerShip->combatAttr &= ~SPECIALED;

	if(playerShip->attr & ELECTROED)
	{
		if(playerShip->attr & VIEW_IN)
			SetShake(2);

		playerShip->vhdg += (randy()%600)-300;
	}

	// ncstuff

	Joy2Neg();		// DLS

	if((neg1.twist < 134) && (neg1.twist > 122))
		neg1.twist = 128;
	//	else if(neg1.twist >= 134)
	//		neg1.twist -= 6;
	//	else if(neg1.twist <= 122)
	//		neg1.twist += 6;

	if(playerShip->combatAttr & REVCONNED)
		targetVhdg = ((neg1.twist - 128)*(playerShip->maxHeading+400)<<1)/nCon.twistFactor ;
	else
		targetVhdg = (-(neg1.twist - 128)*(playerShip->maxHeading+400)<<1)/nCon.twistFactor ;

	if( targetVhdg > playerShip->vhdg)
	{
		if((targetVhdg - playerShip->vhdg) <= playerShip->headingInc) playerShip->vhdg += targetVhdg - playerShip->vhdg;
		else if(playerShip->vhdg >= 0) playerShip->vhdg += playerShip->headingInc;
		else if(playerShip->vhdg < 0) playerShip->vhdg += (playerShip->headingInc<<1);
	}
	else if( targetVhdg < playerShip->vhdg	)
	{
		if((targetVhdg - playerShip->vhdg) >= -playerShip->headingInc) playerShip->vhdg += targetVhdg - playerShip->vhdg;
		if(playerShip->vhdg <= 0) playerShip->vhdg -= playerShip->headingInc;
		else if(playerShip->vhdg > 0) playerShip->vhdg -= (playerShip->headingInc<<1);
	}

	if(playerShip->vhdg < -playerShip->maxHeading)
		playerShip->vhdg = -playerShip->maxHeading;

	if(playerShip->vhdg > playerShip->maxHeading)
		playerShip->vhdg = playerShip->maxHeading;

	/**** Check for Stall and set thrust ***/
	if(playerShip->updateCount == STALL_DELAY)
	{
		if((playerShip->thrust_mag >= 680) &&
			(playerShip->thrust_mag <= 700))
		{
			playerShip->thrust_mag = 1800;
			maxThrust = 1800;
		}
		else if(playerShip->thrust_mag < 680)
			maxThrust = playerShip->max_thrust;

		else 	maxThrust = 200;
	}

	if(playerShip->updateCount > 0)
		playerShip->updateCount--;
	else
		maxThrust = playerShip->max_thrust;

	if(maxThrust < 500) maxThrust += (randy()%11)>>1;

	if (pad & nCon.thrust)
	{
		playerShip->thrust_mag += 16;
	}
	else if (playerShip->thrust_mag > 7) playerShip->thrust_mag -= 8;

	if(playerShip->thrust_mag > maxThrust)
		playerShip->thrust_mag = maxThrust;

	DoDetailKeys();			// DLS

	/*** Pitch ***/
	if (pad & pCon.down) playerShip->vpitch += FR30U;
	if (pad & pCon.up) playerShip->vpitch -= FR30U;

	if ((xpad & PadCircle0) && (playerShip->weaponType != NO_WEAPON))
	{
		if((playerShip->attr & SHIELDED) == 0)
			FireWeapon(playerShip, weaponIndex, shipNo, playerShip->weaponType);
		else
			PlayNote(MENU_MOVE, 1, 20);
	}

	SetBrake(playerShip, pCon.lBrake, pCon.rBrake);
	SetView(playerShip, pCon.view);
	shipTrkReaction(playerShip, shipShapes);
}





/***************************************************************/
/***************************************************************/
/******    				REMOTE CRAFT CONTROL						  *****/
/***************************************************************/
/***************************************************************/

void UpdateRemoteShipStart(ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData *weaponIndex)
{
	static int32_t startPos;

	shipIndex = &(shipIndex[shipNo]);

	shipIndex->prevShipSect = shipIndex->nearTrkSect;
	SectionSearch(shipIndex);

	if(shipIndex->updateCount == START_TIME)
	{
		startPos = shipIndex->ppivot.vy;
	}

	shipIndex->ppivot.vy = startPos + (rsin(shipIndex->updateCount*(80 + shipNo*3)) >> 7);

	shipIndex->updateCount--;
	if(shipIndex->updateCount == 0)
	{
		shipIndex->update = UpdateRemoteShip;
	}

}



void HoldLeft(VECTOR *result, ShipData *shipIndex, Face *facePtr, int32_t shipNo)
{
	VECTOR 		faceVert1, faceVert2;

	faceVert1.vx = track->vertices[(facePtr->vertex[1])].vx;
	faceVert1.vy = track->vertices[(facePtr->vertex[1])].vy;
	faceVert1.vz = track->vertices[(facePtr->vertex[1])].vz;

	faceVert2.vx = track->vertices[(facePtr->vertex[0])].vx;
	faceVert2.vy = track->vertices[(facePtr->vertex[0])].vy;
	faceVert2.vz = track->vertices[(facePtr->vertex[0])].vz;

	result->vx = (faceVert1.vx - faceVert2.vx) >> 1;
	result->vy = (faceVert1.vy - faceVert2.vy) >> 1;
	result->vz = (faceVert1.vz - faceVert2.vz) >> 1;
}



void HoldRight(VECTOR *result, ShipData *shipIndex, Face *facePtr, int32_t shipNo)
{
	VECTOR 		faceVert1, faceVert2;

	faceVert1.vx = track->vertices[(facePtr->vertex[0])].vx;
	faceVert1.vy = track->vertices[(facePtr->vertex[0])].vy;
	faceVert1.vz = track->vertices[(facePtr->vertex[0])].vz;

	faceVert2.vx = track->vertices[(facePtr->vertex[1])].vx;
	faceVert2.vy = track->vertices[(facePtr->vertex[1])].vy;
	faceVert2.vz = track->vertices[(facePtr->vertex[1])].vz;

	result->vx = (faceVert1.vx - faceVert2.vx) >> 1 ;
	result->vy = (faceVert1.vy - faceVert2.vy) >> 1 ;
	result->vz = (faceVert1.vz - faceVert2.vz) >> 1 ;
}



void HoldCentre(VECTOR *result, ShipData *shipIndex, Face *facePtr, int32_t shipNo)
{
	result->vx = 0;
	result->vy = 0;
	result->vz = 0;
}



void BlockShip(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t shipNo)
{
	if (shipIndex[ownShip].attr & LSIDE)
		HoldLeft (blockVector, shipIndex, facePtr, shipNo) ;
	else
		HoldRight  (blockVector, shipIndex, facePtr, shipNo) ;

}



void AvoidShip(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t shipNo)
{
	if (shipIndex[ownShip].attr & LSIDE)
		HoldRight (blockVector, shipIndex, facePtr, shipNo) ;
	else
		HoldLeft  (blockVector, shipIndex, facePtr, shipNo) ;

}



void AvoidOtherShips(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t shipNo)
{
	int32_t 					i;
	int16_t					DPASectDiff;			/** Change to run from sect not DPA sect **/
	int16_t					minDPASec = 100;
	int32_t					shipID;

	for(i = 0; i < NO_OF_SHIPS; i++)
	{
		if(i != shipNo)
		{

			DPASectDiff = shipIndex[i].DPASectDiff - shipIndex[shipNo].DPASectDiff;
			if(minDPASec < DPASectDiff)
			{
				minDPASec = DPASectDiff;
				shipID = i;
			}
		}
	}

	if((minDPASec < 10)	&& (minDPASec > -2 ))
	{
		if (shipIndex[shipID].attr & LSIDE)
			HoldRight (blockVector, shipIndex, facePtr, shipNo) ;
		else
			HoldLeft  (blockVector, shipIndex, facePtr, shipNo) ;
	}
}



void ZigZagShip(VECTOR *blockVector, ShipData *shipIndex, Face *facePtr, int32_t shipNo)
{
	if ((shipIndex->updateCount%50) == 0)
	{
		if (((shipIndex->updateCount/50)%2) == 0)
			HoldLeft (blockVector, shipIndex, facePtr, shipNo) ;
		else
			HoldRight  (blockVector, shipIndex, facePtr, shipNo) ;
	}
}



#define SLOW_DOWN 			1
#define SHIP_MESS 		   0

void UpdateRemoteShip(register ShipData *shipIndex, Object** shipShapes, int32_t shipNo, WeaponData *weaponIndex)
{
	ShipData				*myShip , *remoteShip;
	Face 					*facePtr ;
	TrackSection 		*nextSection, *section ;
	int32_t 					i;
	VECTOR 				targetVector, bestPath;
	VECTOR 				facePoint;
	int32_t 					temp, alpha;
	VECTOR 				offSetVector ;
	Vector				path1, path2;
	int32_t 					gap_mag, decide ;
	int16_t					DPASectDiff;

	offSetVector.vx = 0 ;
	offSetVector.vy = 0 ;
	offSetVector.vz = 0 ;

	myShip = &(shipIndex[ownShip]) ;
	remoteShip = &(shipIndex[shipNo]);

	if(remoteShip->electroCount > 0)
		remoteShip->electroCount--;
	if(remoteShip->electroCount == 0)
		remoteShip->attr &= ~ELECTROED;


	if((remoteShip->attr & FLYING) == 0)
	{
		/*** Find First track base section ***/
		facePtr = track->faces + remoteShip->nearTrkSect->faceStart ;
		while((facePtr->flags & TRACK_BASE) == 0)
		{
			facePtr++ ;
		}

		if(gameType == SINGLE)
			DPASectDiff = remoteShip->DPASectDiff - myShip->DPASectDiff;

		else/* if(gameType == MASTER)*/
		{
			if(shipIndex[ownShip].DPASectDiff > shipIndex[serialShip].DPASectDiff)
			{
				DPASectDiff = remoteShip->DPASectDiff - shipIndex[ownShip].DPASectDiff;
				myShip = &(shipIndex[ownShip]) ;
			}
			else
			{
				DPASectDiff = remoteShip->DPASectDiff - shipIndex[serialShip].DPASectDiff;
				myShip = &(shipIndex[serialShip]) ;
			}
		}
		remoteShip->attr &= ~JUST_IN_FRONT;

		if(remoteShip == myShip)
		{

			remoteShip->updateControl = AvoidOtherShips;
			if ( ((remoteShip->remoteMaxThrust) > remoteShip->speed) )
				remoteShip->speed += remoteShip->remoteThrustMag ;
		}
		else
		{

			/************************************************************************/
			/**** Make global DPA decisions , these will effect the craft in     ****/
			/**** relation to your race position											****/
			/************************************************************************/

			/************************************************************************/
			/**** Accelerate remote ships away at start, numtospeed set in	     	****/
			/**** InitShipData and is an exponential progression						****/
			/************************************************************************/

			if(remoteShip->numtospeed > 0)
			{
				remoteShip->numtospeed--;
				remoteShip->updateCount = 0;
				remoteShip->updateControl = AvoidShip;
				if ( (remoteShip->remoteMaxThrust+1200) > remoteShip->speed)
					remoteShip->speed += (remoteShip->remoteThrustMag+150) ;
			}

			/************************************************************************/
			/**** Ship has been left WELL BEHIND therefore set it to avoid	     	****/
			/**** other ships and update its speed as normal							****/
			/************************************************************************/
			else if( DPASectDiff < -10) 															/*** Ship behind, AVOID ***/
			{
				remoteShip->updateCount = 0;
				remoteShip->updateControl = AvoidShip;

				/***** If ship has been well passed, increase its speed to allow *****/
				/***** it to make a challenge when the player fouls up			  *****/

				if ( ((remoteShip->remoteMaxThrust + behindSpeed[trackNo]) > remoteShip->speed) )
				{
					remoteShip->speed += remoteShip->remoteThrustMag ;
				}
			}

			/************************************************************************/
			/**** Ship is JUST AHEAD therefore	 										  	****/
			/**** 																					****/
			/************************************************************************/

			else if ( ( DPASectDiff <= 4 ) && ( DPASectDiff > 0) )	 /***** Ship close by, beware does not account for lapped opponents yet *****/
			{
				remoteShip->attr |= JUST_IN_FRONT;

				if(remoteShip->updateCount == 0)	 /*** Make New Decision	***/
				{
					decide = randy()%64; /* 12 */

					remoteShip->updateCount = JUST_FRONT_DELAY;
					if(remoteShip->fightBack)	 /**** Ship wants to make life difficult ****/
					{
						if ((decide < 40) || (remoteShip->weaponType == NO_WEAPON))			 /*** Ship will try to block you ***/
						{
							remoteShip->updateControl = BlockShip;
						}
						else if ((decide >= 40) && (decide < 52))	/*** Ship will attempt to drop mines in your path ***/
						{
							remoteShip->updateControl = BlockShip;
							if(((remoteShip->attr & SHIELDED) == 0) && (remoteShip->attr & RACING))
							{
								if(gameType == MASTER)
								{
									if(shipIndex[ownShip].DPASectDiff > shipIndex[serialShip].DPASectDiff)
									{
										PlayNote(SP_MINES, 1, 70);
										SetPitch(SP_MINES, 0x1000);
									}
								}
								else
								{
									PlayNote(SP_MINES, 1, 70);
									SetPitch(SP_MINES, 0x1000);
								}
								remoteShip->weaponType = MINE;
								FireWeapon(remoteShip, weaponIndex, shipNo, remoteShip->weaponType);
							}
						}
						else if ((decide >= 52) && (decide < 64))	/*** Ship will raise its shield ***/
						{
							remoteShip->updateControl = BlockShip;
							if((remoteShip->attr & SHIELDED) == 0)
							{
								remoteShip->weaponType = SHIELD;
								FireWeapon(remoteShip, weaponIndex, shipNo, remoteShip->weaponType);
							}
						}
					}
					else		  /**** Let the first ships be easy to pass ****/
					{
						remoteShip->updateControl = AvoidShip;
					}
				}

				remoteShip->updateCount--;

				if(remoteShip->attr & OVERTAKE)
				{
					/*** If ship has just overtaken, slow it down to a reasonable speed ***/
					if ( (remoteShip->remoteMaxThrust + behindSpeed[trackNo]) > remoteShip->speed)
						remoteShip->speed += (remoteShip->remoteThrustMag) ;
				}
				else
				{
					/*** Increase the speed of any craft just in front slightly ***/
					if ( ((remoteShip->remoteMaxThrust + (behindSpeed[trackNo]>>1)) > remoteShip->speed) )
						remoteShip->speed += remoteShip->remoteThrustMag ;
				}

			}

			/************************************************************************/
			/**** Ship is JUST BEHIND therefore we must decided if and how many 	****/
			/**** times it 'should have a go back'											****/
			/************************************************************************/

			else if( ( DPASectDiff >= -10 ) && ( DPASectDiff <= 0) ) 				/*** Ship just behind, MAKE DECISION ***/
			{
				if(remoteShip->updateCount == 0)	 /*** Make New Decision	***/
				{
					remoteShip->updateCount = JUST_BEHIND_DELAY;

					if(remoteShip->fightBack)	 /**** Ship wants you to say "Outside Now!" ****/
					{
						if (remoteShip->weaponType == NO_WEAPON)
						{
							remoteShip->updateControl = AvoidShip;
							remoteShip->attr |= OVERTAKE;
						}
						else
						{
							decide = randy()%64;

							if (decide < 48)
							{
								remoteShip->updateControl = BlockShip;
							}
							else if ((decide >= 40) && (decide < 54))
							{
								remoteShip->updateControl = AvoidShip;
								remoteShip->attr &= ~OVERTAKE;
								if(((remoteShip->attr & SHIELDED) == 0) && (remoteShip->attr & RACING))
								{
									if(gameType == MASTER)
									{
										if(shipIndex[ownShip].DPASectDiff > shipIndex[serialShip].DPASectDiff)
										{
											PlayNote(SP_ROCKET, 1, 70);
											SetPitch(SP_ROCKET, 0x1000);
										}
									}
									else
									{
										PlayNote(SP_ROCKET, 1, 70);
										SetPitch(SP_ROCKET, 0x1000);
									}
									remoteShip->weaponType = ROCKET;
									FireWeapon(remoteShip, weaponIndex, shipNo, remoteShip->weaponType);
								}
							}
							else if ((decide >= 54) && (decide < 60))
							{
								remoteShip->updateControl = AvoidShip;
								remoteShip->attr &= ~OVERTAKE;
								if(((remoteShip->attr & SHIELDED) == 0) && (remoteShip->attr & RACING))
								{
									if(gameType == MASTER)
									{
										if(shipIndex[ownShip].DPASectDiff > shipIndex[serialShip].DPASectDiff)
										{
											PlayNote(SP_MISSILE, 1, 70);
											SetPitch(SP_MISSILE, 0x1000);
										}
									}
									else
									{
										PlayNote(SP_MISSILE, 1, 70);
										SetPitch(SP_MISSILE, 0x1000);
									}
									remoteShip->weaponType = MISSILE;
									remoteShip->targetShip = ownShip;
									FireWeapon(remoteShip, weaponIndex, shipNo, remoteShip->weaponType);
								}
							}
							else if ((decide >= 60) && (decide < 64))
							{
								remoteShip->updateControl = AvoidShip;
								remoteShip->attr &= ~OVERTAKE;
								if(((remoteShip->attr & SHIELDED) == 0) && (remoteShip->attr & RACING))
								{
									if(gameType == MASTER)
									{
										if(shipIndex[ownShip].DPASectDiff > shipIndex[serialShip].DPASectDiff)
										{
											PlayNote(SP_SHOCKWAVE, 1, 70);
											SetPitch(SP_SHOCKWAVE, 0x1000);
										}
									}
									else
									{
										PlayNote(SP_SHOCKWAVE, 1, 70);
										SetPitch(SP_SHOCKWAVE, 0x1000);
									}
									remoteShip->weaponType = ELEC_BOLT;
									remoteShip->targetShip = ownShip;
									FireWeapon(remoteShip, weaponIndex, shipNo, remoteShip->weaponType);
								}
							}
						}
					}
					else		  /**** If ship destined to be tail-ender then slow down ****/
					{
						remoteShip->remoteMaxThrust = 2100 ;
						remoteShip->remoteThrustMag = 25 ;
						remoteShip->speed = 2100 ;
						remoteShip->updateControl = AvoidShip;
						remoteShip->attr &= ~OVERTAKE;
					}
				}

				for(i = 0; i < NO_OF_SHIPS; i++)	 /**** If another ship is just in front pass fight on ****/
				{
					if(shipIndex[i].attr & JUST_IN_FRONT)
					{
						remoteShip->updateControl = AvoidShip;
						remoteShip->attr &= ~OVERTAKE;
					}
				}

				remoteShip->updateCount--;


				if(remoteShip->attr & OVERTAKE)
				{
					if ( (remoteShip->remoteMaxThrust+700) > remoteShip->speed)
						remoteShip->speed += (remoteShip->remoteThrustMag<<1) ;
				}
				else
				{
					if ( ((remoteShip->remoteMaxThrust + behindSpeed[trackNo]) > remoteShip->speed) )
						remoteShip->speed += remoteShip->remoteThrustMag ;
				}
		}

		/************************************************************************/
		/**** Ship is WELL AHEAD therefore we must slow the opponent to	  	****/
		/**** give the weaker player a chance to catch up							****/
		/************************************************************************/

		else if(( DPASectDiff > (shipOrder[shipNo]*15)) && (DPASectDiff < 150)) 	/*** Ship well ahead, SLOW DOWN ***/
		{
			remoteShip->speed += (remoteShip->remoteThrustMag>>1) ;
			if (remoteShip->speed > (remoteShip->remoteMaxThrust>>1))
				remoteShip->speed = (remoteShip->remoteMaxThrust>>1) ;

			remoteShip->updateCount = 0;
			remoteShip->updateControl = HoldCentre;
		}

		/************************************************************************/
		/**** Ship is TOO FAR AHEAD therefore 										  	****/
		/**** 																					****/
		/************************************************************************/

		else if (DPASectDiff >= 150)			  										/*** Ship too far ahead, let it continue ***/
		{
			remoteShip->updateCount = 0;
			remoteShip->updateControl = AvoidShip;

			if ( (remoteShip->remoteMaxThrust > remoteShip->speed) )
			{
				remoteShip->speed += remoteShip->remoteThrustMag ;
			}
		}


		/************************************************************************/
		/**** Ship is IN SIGHT therefore 										  	****/
		/**** 																					****/
		/************************************************************************/

		else if ( ( DPASectDiff <= 10 ) && ( DPASectDiff > 4) )	 /***** Ship close by, beware does not account for lapped opponents yet *****/
		{
			if(remoteShip->updateCount == 0)	 /*** Make New Decision	***/
			{
				decide = randy()%5;

				remoteShip->updateCount = IN_SIGHT_DELAY;
				switch (decide)
				{
				case 0: 	remoteShip->updateControl = HoldCentre;
					break;

				case 1: 	remoteShip->updateControl = HoldLeft;
					break;

				case 2: 	remoteShip->updateControl = HoldRight;
					break;

				case 3:	remoteShip->updateControl = BlockShip;
					break;

				case 4:	remoteShip->updateControl = ZigZagShip;
					break;
				}
			}

			remoteShip->updateCount--;

			if ( (remoteShip->remoteMaxThrust > remoteShip->speed) )
			{
				remoteShip->speed += remoteShip->remoteThrustMag ;
			}
		} /***** End of DPA control options *****/

		/************************************************************************/
		/**** Ship is JUST OUT OF SIGHT therefore 								  	****/
		/**** 																					****/
		/************************************************************************/

		else
		{
			remoteShip->updateCount = 0;
			remoteShip->updateControl = HoldCentre;
			if ( (remoteShip->remoteMaxThrust > remoteShip->speed) )
			{
				remoteShip->speed += remoteShip->remoteThrustMag ;
			}
		}
		}

		(remoteShip->updateControl)(&offSetVector, shipIndex, facePtr, shipNo);

		/***************************************************************************/
		/***************************************************************************/
		/***************************************************************************/
		/***************************************************************************/

		/************************************************************************/
		/**** Make decision as to which path the craft will take at a		  	****/
		/**** junction																			****/
		/************************************************************************/


		section = remoteShip->nearTrkSect->prevSection.ptr;

		for(i = 0; i < 4; i++)
		{
			section = section->nextSection.ptr;
		}

		if(section->junction.index != -1)
		{
			if(section->junction.ptr->flags & JunctionStart)
			{
				decide = randy()%2;
				if(decide == 0)
					remoteShip->attr |= JUNC_LEFT;
				else
					remoteShip->attr &= ~JUNC_LEFT;
			}
		}

		section = remoteShip->nearTrkSect->prevSection.ptr;

		for(i = 0; i < 4; i++)
		{
			if(section->junction.index != -1)
			{
				if(section->junction.ptr->flags & JunctionStart)
				{
					if(remoteShip->attr & JUNC_LEFT)
					{
						section = section->junction.ptr;
					}
					else
						section = section->nextSection.ptr;
				}
				else
					section = section->nextSection.ptr;
			}
			else
				section = section->nextSection.ptr;
		}
		nextSection = section->nextSection.ptr;
		section = remoteShip->nearTrkSect;


		/***************************************************************************/
		/**** 					General routines - Non decision based					****/
		/***************************************************************************/

		/***** Bleed off speed as orientation changes *****/

		remoteShip->speed -= (abs(remoteShip->speed * ((remoteShip->vhdg*FR60)/60)))>>14 ;	  /* 14 */
		remoteShip->speed -= (abs(remoteShip->speed * ((remoteShip->vpitch*FR60)/60)))>>14 ;

		if(remoteShip->nearTrkSect->flags & Jump)	/**** Cure for ships getting stuck on hump lip ****/
		{
			targetVector.vx = remoteShip->nearTrkSect->centre.vx - remoteShip->nearTrkSect->prevSection.ptr->centre.vx;
			targetVector.vy = remoteShip->nearTrkSect->centre.vy - remoteShip->nearTrkSect->prevSection.ptr->centre.vy;
			targetVector.vz = remoteShip->nearTrkSect->centre.vz - remoteShip->nearTrkSect->prevSection.ptr->centre.vz;
		}
		else
		{
			targetVector.vx = nextSection->centre.vx - section->centre.vx;
			targetVector.vy = nextSection->centre.vy - section->centre.vy;
			targetVector.vz = nextSection->centre.vz - section->centre.vz;
		}

		gap_mag = GetMagnitude(targetVector);

		targetVector.vx = (targetVector.vx*remoteShip->speed)/gap_mag;
		targetVector.vy = (targetVector.vy*remoteShip->speed)/gap_mag;
		targetVector.vz = (targetVector.vz*remoteShip->speed)/gap_mag;

		path1.vx = section->centre.vx + offSetVector.vx;
		path1.vy = section->centre.vy + offSetVector.vy;
		path1.vz = section->centre.vz + offSetVector.vz;

		path2.vx = nextSection->centre.vx + offSetVector.vx;
		path2.vy = nextSection->centre.vy + offSetVector.vy;
		path2.vz = nextSection->centre.vz + offSetVector.vz;

		GetTargetPos(&bestPath, remoteShip->ppivot, path2, path1);

		remoteShip->apivot.vx = (targetVector.vx + ((bestPath.vx - remoteShip->ppivot.vx)>>1));
		remoteShip->apivot.vy = (targetVector.vy + ((bestPath.vy - remoteShip->ppivot.vy)>>1));
		remoteShip->apivot.vz = (targetVector.vz + ((bestPath.vz - remoteShip->ppivot.vz)>>1));

		remoteShip->apivot.vx = (remoteShip->apivot.vx*60)/FR60;
		remoteShip->apivot.vy = (remoteShip->apivot.vy*60)/FR60;
		remoteShip->apivot.vz = (remoteShip->apivot.vz*60)/FR60;

#if COLLIDE_TEST
		remoteShip->vpivot.vx = 0;
		remoteShip->vpivot.vy = 0;
		remoteShip->vpivot.vz = 0;
#else
		remoteShip->vpivot.vx += remoteShip->apivot.vx;
		remoteShip->vpivot.vy += remoteShip->apivot.vy;
		remoteShip->vpivot.vz += remoteShip->apivot.vz;
#endif
		/***** If remote has gone over speed-up *****/

		/***** Can use control routine to determine position now *****/
		/***** should use actual position finally                *****/

		if((facePtr->flags & SPEED_UP) && ((remoteShip->updateControl == HoldLeft) || (remoteShip->updateControl == HoldCentre)))
		{
			remoteShip->speed += 200;
		}
		facePtr++;

		if((facePtr->flags & SPEED_UP) && ((remoteShip->updateControl == HoldRight) || (remoteShip->updateControl == HoldCentre)))
		{
			remoteShip->speed += 200;
		}

		/****** Speed Test, should not make much difference ******/
		/****** to overall lap times of opposition          ******/
		facePoint.vx = track->vertices[(facePtr->vertex[0])].vx;
		facePoint.vy = track->vertices[(facePtr->vertex[0])].vy;
		facePoint.vz = track->vertices[(facePtr->vertex[0])].vz;
		alpha = PlaneLineCollision (facePoint, remoteShip->ppivot, facePtr->normal);

		if(alpha < 50) alpha = 50;

		remoteShip->apivot.vx = (((facePtr->normal.vx*TARGETHEIGHT*TRACK_MAGNET)/alpha) -
			(facePtr->normal.vx*TRACK_MAGNET))>>8;
		remoteShip->apivot.vy = (((facePtr->normal.vy*TARGETHEIGHT*TRACK_MAGNET)/alpha) -
			(facePtr->normal.vy*TRACK_MAGNET))>>8;
		remoteShip->apivot.vz = (((facePtr->normal.vz*TARGETHEIGHT*TRACK_MAGNET)/alpha) -
			(facePtr->normal.vz*TRACK_MAGNET))>>8;

		remoteShip->vpivot.vx += remoteShip->apivot.vx;
		remoteShip->vpivot.vy += remoteShip->apivot.vy;
		remoteShip->vpivot.vz += remoteShip->apivot.vz;

		temp = SquareRoot0(targetVector.vx * targetVector.vx + targetVector.vz * targetVector.vz);

		remoteShip->vhdg = -ratan2(targetVector.vx, targetVector.vz) - remoteShip->hdg ;
		remoteShip->vhdg += remoteShip->ahdg ;	/*** Result of weapon collision ***/
		remoteShip->ahdg -= remoteShip->ahdg>>3;

		if(remoteShip->vhdg > 2048) remoteShip->vhdg = remoteShip->vhdg - 4096;
		else if(remoteShip->vhdg < -2048) remoteShip->vhdg = remoteShip->vhdg + 4096;


		remoteShip->vhdg = (remoteShip->vhdg * 60) / FR60;
		remoteShip->hdg += remoteShip->vhdg>>4;

		remoteShip->vpitch = -ratan2(targetVector.vy, temp) - remoteShip->pitch;
		remoteShip->vpitch = (remoteShip->vpitch * 60) / FR60;

		remoteShip->pitch += remoteShip->vpitch>>4;
		remoteShip->vroll += sar(remoteShip->vhdg,2);
		remoteShip->vroll -= remoteShip->vroll>>1;
		remoteShip->roll  += remoteShip->vroll;
		remoteShip->roll  -= remoteShip->roll>>3;
	}

	/************************************************************************/
	/**** Ship is FLYING therefore 												  	****/
	/**** 																					****/
	/************************************************************************/

	else
	{
		section = remoteShip->nearTrkSect;
		section = section->nextSection.ptr;
		section = section->nextSection.ptr;
		nextSection = section->nextSection.ptr;

		remoteShip->updateControl = HoldCentre;
		(remoteShip->updateControl)(&offSetVector, myShip, facePtr, shipNo);

		if ( remoteShip->remoteMaxThrust > remoteShip->speed)
		{
			remoteShip->speed += remoteShip->remoteThrustMag ;
		}

		remoteShip->speed -= (abs(remoteShip->speed * remoteShip->vhdg))>>14 ;


		targetVector.vx = nextSection->centre.vx - section->centre.vx;
		targetVector.vy = nextSection->centre.vy - section->centre.vy;
		targetVector.vz = nextSection->centre.vz - section->centre.vz;

		gap_mag = GetMagnitude(targetVector);

		targetVector.vx = (targetVector.vx*remoteShip->speed)/gap_mag;
		targetVector.vz = (targetVector.vz*remoteShip->speed)/gap_mag;

		targetVector.vy = 500;

		GetTargetPos(&bestPath, remoteShip->ppivot, nextSection->centre, remoteShip->nearTrkSect->centre);

		remoteShip->apivot.vx = (targetVector.vx + ((bestPath.vx - remoteShip->ppivot.vx)>>1));
		remoteShip->apivot.vy = targetVector.vy;
		remoteShip->apivot.vz = (targetVector.vz + ((bestPath.vz - remoteShip->ppivot.vz)>>1));

		remoteShip->apivot.vx = (remoteShip->apivot.vx*60)/FR60;
		remoteShip->apivot.vy = (remoteShip->apivot.vy*60)/FR60;
		remoteShip->apivot.vz = (remoteShip->apivot.vz*60)/FR60;


		remoteShip->vpivot.vx += remoteShip->apivot.vx;
		remoteShip->vpivot.vy += remoteShip->apivot.vy;
		remoteShip->vpivot.vz += remoteShip->apivot.vz;
		remoteShip->vpitch = -200 - remoteShip->pitch;
		remoteShip->pitch += remoteShip->vpitch>>4;
		remoteShip->vhdg = -ratan2(targetVector.vx, targetVector.vz) - remoteShip->hdg ;

		if(remoteShip->vhdg > 2048) remoteShip->vhdg = remoteShip->vhdg - 4096;
		else if(remoteShip->vhdg < -2048) remoteShip->vhdg = remoteShip->vhdg + 4096;

		remoteShip->hdg += remoteShip->vhdg>>4;
	}

	remoteShip->vpivot.vx -= remoteShip->vpivot.vx>>3;
	remoteShip->vpivot.vy -= remoteShip->vpivot.vy>>3;
	remoteShip->vpivot.vz -= remoteShip->vpivot.vz>>3;

	remoteShip->ppivot.vx += remoteShip->vpivot.vx>>6;
	remoteShip->ppivot.vy += remoteShip->vpivot.vy>>6;
	remoteShip->ppivot.vz += remoteShip->vpivot.vz>>6;

	if(remoteShip->attr & ELECTROED)
	{
		remoteShip->ppivot.vx += (randy()-16384)%20;
		remoteShip->ppivot.vy += (randy()-16384)%20;
		remoteShip->ppivot.vz += (randy()-16384)%20;

		if((randy()%50) == 0)
			remoteShip->speed -= remoteShip->speed>>1 ;
	}

	remoteShip->hdg   = ang(remoteShip->hdg);
	remoteShip->pitch = ang(remoteShip->pitch);
	remoteShip->roll  = ang(remoteShip->roll);
}



void SetSkeletonDirectionHPR( Skeleton* skeleton, int16_t hdg, int16_t pitch, int16_t roll  )
{
	int16_t    sx, sy, sz;
	int16_t    cx, cy, cz;


	sy = rsin( hdg );
	sx = rsin( pitch );
	sz = rsin( roll );

	cy = rcos( hdg );
	cx = rcos( pitch );
	cz = rcos( roll );

	skeleton->relative.m[0][0] = ((cy * cz) >> 12) + ((((sy * sz) >> 12) * sx) >> 12);
	skeleton->relative.m[0][1] = ((cy * sz) >> 12) - ((((sy * sx) >> 12) * cz) >> 12);
	skeleton->relative.m[0][2] = (-sy * cx) >> 12;

	skeleton->relative.m[1][0] = -(sz * cx) >> 12;
	skeleton->relative.m[1][1] = (cx * cz) >> 12;
	skeleton->relative.m[1][2] = -sx;

	skeleton->relative.m[2][0] = ((sy * cz) >> 12) - ((((cy * sx) >> 12) * sz) >> 12);
	skeleton->relative.m[2][1] = ((sy * sz) >> 12) + ((((cy * sx) >> 12) * cz) >> 12);
	skeleton->relative.m[2][2] = ((cx * cy) >> 12);


	/* Mark skeleton for update */

	skeleton->update = 1;
}



void SetSkeletonDirectionRPH( Skeleton* skeleton, int16_t roll, int16_t pitch, int16_t hdg  )
{
	int16_t    sx, sy, sz;
	int16_t    cx, cy, cz;


	sy = rsin( hdg );
	sx = rsin( pitch );
	sz = rsin( roll );

	cy = rcos( hdg );
	cx = rcos( pitch );
	cz = rcos( roll );

	skeleton->relative.m[0][0] = ((cy * cz) >> 12) - ((((sz * sx) >> 12) * sy) >> 12);
	skeleton->relative.m[0][1] = (sz * cx) >> 12;
	skeleton->relative.m[0][2] = (-(sy * cz) >> 12) - ((((sz * sx) >> 12) * cy) >> 12);

	skeleton->relative.m[1][0] = (-(sz * cy) >> 12) - ((((sx * cz) >> 12) * sy) >> 12);
	skeleton->relative.m[1][1] = (cx * cz) >> 12;
	skeleton->relative.m[1][2] = ((sz * sy) >> 12) - ((((sx * cz) >> 12) * cy) >> 12);

	skeleton->relative.m[2][0] = (cx * sy) >> 12;
	skeleton->relative.m[2][1] = sx;
	skeleton->relative.m[2][2] = (cx * cy) >> 12;

	/* Mark skeleton for update */

	skeleton->update = 1;
}




void Mouse2Neg(void)
{
	neg1.twist = etc_Mouse1.mX;
}



void Joy2Neg(void)
{

	if(etc_Joy1.jX > JoyMaxX)
		etc_Joy1.jX = JoyMaxX;
	else if(etc_Joy1.jX < JoyMinX)
		etc_Joy1.jX = JoyMinX;

	if(etc_Joy1.jY > JoyMaxY)
		etc_Joy1.jY = JoyMaxY;
	else if(etc_Joy1.jY < JoyMinY)
		etc_Joy1.jY = JoyMinY;

	//	if (etc_Joy1.jX < 128)
	//		neg1.twist = etc_Joy1.jX;
	//	else
	//		neg1.twist =

	//if(etc_Joy1.jX < (JoyCentreX - JoyCentreFudge))
	//neg1.twist = (etc_Joy1.jX - JoyMinX) * left_scale_factor;
	//else if(etc_Joy1.jX > (JoyCentreX + JoyCentreFudge))
	//neg1.twist = ((etc_Joy1.jX - JoyCentreX) * right_scale_factor) + 128;

	neg1.twist = etc_Joy1.jX;


	if (etc_Joy1.jY < 100)
		pad |= pCon.up;
	else if (etc_Joy1.jY > 156)
		pad |= pCon.down;
	//	if (etc_Joy1.jY < 155)
	//		pad |= pCon.up;
	//	else if (etc_Joy1.jY > 165)
	//		pad |= pCon.down;
	//if(etc_Joy1.jY < (JoyCentreY - JoyCentreFudge - 100))
	//pad |= pCon.up;
	//else if(etc_Joy1.jY > (JoyCentreY + JoyCentreFudge + 100))
	//pad |= pCon.down;

}




void DoDetailKeys(void)
{
	if (xpad & PadTopLeft1)			// DLS - Draw Distance
	{
		switch(DrawDistance)
		{
		case HIGH_DISTANCE:
			DrawDistance = MEDIUM_DISTANCE;
			break;

		case MEDIUM_DISTANCE:
			DrawDistance = LOW_DISTANCE;
			break;

		case LOW_DISTANCE:
			DrawDistance = HIGH_DISTANCE;
			break;
		}

		SetDrawDistance(DrawDistance);
	}

	if (xpad & PadSquare0)			// DLS - Screen Size
	{
		Sky ^= 1;
	}

	if (xpad & PadUnused1)			// DLS - Detail
	{
		switch(Resolution)
		{
		case SMALL:
			screenres=Resolution=LOW;
			changeScreen();
			break;
		case LOW:
			screenres=Resolution=MEDIUM;
			changeScreen();
			break;
		case MEDIUM:
			screenres=Resolution=HIGH;
			changeScreen();
			break;
		case HIGH:
			screenres=Resolution=SMALL;
			changeScreen();
			break;
		}
	}

	if (xpad & PadUnused2)
	{
		if(++ft>1)
			ft=0;
	}
}
