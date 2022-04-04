#ifndef RACE_H
#define RACE_H

#include <stdint.h>
#include "combat.h"
#include "droid.h"
#include "effects.h"
#include "menus.h"
#include "object.h"
#include "ships.h"
#include "weapon.h"

void race		( combatData *packetData, ConfigData *gameData, int32_t mode, int32_t timeout );
void Restart	(	ShipData 		*shipIndex,
				 		ConfigData 		*gameData,
						WeaponData 		*weaponIndex,
						WeaponShape		*weaponShapes,
						Effect			*effects,
                        RescueData      *rescueDroid,
                        Object          **shipShapes);
void QuitGame	();

#endif
