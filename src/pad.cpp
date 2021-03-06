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
#include "input.h"

extern int32_t andyTimeout;
extern uint16_t OriginalKeyCodes[16];

void InitPadSettings() {
  nCon.type = 0;
  nCon.type |= A_THRUST_I;
  nCon.type |= D_VIEW;
  nCon.type |= A_FIRE_II;

  nCon.thrust = NEG_A;
  nCon.view = PadTriangle0;
  nCon.fire = PadCircle0;

  pCon.thrust = PadCross0;
  pCon.view = PadTriangle0;
  pCon.fire = PadCircle0;
  pCon.lBrake = PadTopRight2;
  pCon.rBrake = PadTopLeft2;
  pCon.up = PadUp0;
  pCon.down = PadDown0;
  pCon.left = PadLeft0;
  pCon.right = PadRight0;
}

void SetBrake(ShipData* playerShip, int16_t padLeft, int16_t padRight) {
  if (pad & pCon.rBrake)
    playerShip->r_brake_rot += FR32U;
  else if (playerShip->r_brake_rot > 0)
    playerShip->r_brake_rot -= FR32U;

  if (playerShip->r_brake_rot < 0)
    playerShip->r_brake_rot = 0;

  //	if (pad & pCon.rBrake)	playerShip->r_brake_rot += 32;
  //	else if(playerShip->r_brake_rot > 0) playerShip->r_brake_rot -= 32;

  if (playerShip->r_brake_rot > 256)
    playerShip->r_brake_rot = 256;

  if (pad & pCon.lBrake)
    playerShip->l_brake_rot += FR32U;
  else if (playerShip->l_brake_rot > 0)
    playerShip->l_brake_rot -= FR32U;

  if (playerShip->l_brake_rot < 0)
    playerShip->l_brake_rot = 0;

  if (playerShip->l_brake_rot > 256)
    playerShip->l_brake_rot = 256;
}

void SetView(ShipData* playerShip, int16_t padView) {
  if (xpad & pCon.view) {
    if ((playerShip->attr & VIEW_IN) == 0) {
      CameraUpdate = UpdateCameraRaceInternal;
      playerShip->attr |= VIEW_IN;
    } else {
      CameraUpdate = UpdateCameraRaceExternal;
      playerShip->attr &= ~VIEW_IN;
    }
  }
}

void SetFire(ShipData* playerShip, int16_t padFire, WeaponData* weaponIndex, int32_t shipNo) {
  if ((xpad & pCon.fire) && (playerShip->weaponType != NO_WEAPON)) {
    if ((playerShip->attr & SHIELDED) == 0)
      FireWeapon(playerShip, weaponIndex, shipNo, playerShip->weaponType);
    else
      PlayNote(MENU_MOVE, 1, 20);
  }
}

int16_t GetMenuKey(int16_t key) {
  if (neg1.head == 8960) {
    switch (key) {
    case M_UP:
      return xpad & NEG_UP;
    case M_DOWN:
      return xpad & NEG_DOWN;
    case M_LEFT:
      return xpad & NEG_LEFT;
    case M_RIGHT:
      return xpad & NEG_RIGHT;
    case M_BACK:
      if ((neg1.ii > 200) && (negBack <= 100)) {
        /*	negBack = neg1.ii;*/
        return 1;
      }
      /*	else
					negBack = neg1.ii;
			*/
      return xpad & NEG_B;
    case M_SELECT:
      if ((neg1.i > 100) && (negSelect <= 100)) {
        /*	negSelect = neg1.i; */
        return 1;
      }
      /*	else
					negSelect = neg1.i;
			 */

    case M_START:
      return xpad & NEG_START;

    case M_QUIT:
#if 0
				if((neg1.ii > 100) && (negQuit <= 100))
				{
				/*	negQuit = neg1.ii;*/
					return(1);
				}
			 /*	else
					negQuit = neg1.ii;
			 */
#endif
      return xpad & NEG_A;

    default:
      break;
    }
  } else {
    switch (key) {
    case M_UP:
      if (xpad & PadUp0)
        andyTimeout = 0;
      return xpad & PadUp0;
    case M_DOWN:
      if (xpad & PadDown0)
        andyTimeout = 0;
      return xpad & PadDown0;
    case M_LEFT:
      if (xpad & PadLeft0)
        andyTimeout = 0;
      return xpad & PadLeft0;
    case M_RIGHT:
      if (xpad & PadRight0)
        andyTimeout = 0;
      return xpad & PadRight0;
    case M_BACK:
      if (xpad & PadTopRight1)
        andyTimeout = 0;
      return xpad & PadTopRight1;
    case M_SELECT:
      if (xpad & PadSelect)
        andyTimeout = 0;
      return xpad & PadSelect;
    case M_START:
      if (xpad & PadStart)
        andyTimeout = 0;
      return xpad & PadStart;
    case M_QUIT:
      if (xpad & PadTopRight1)
        andyTimeout = 0;
      return xpad & PadTopRight1;
    default:
      break;
    }
  }
  return 1; // PCwipeout
}
