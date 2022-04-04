#ifndef RACE_H
#define RACE_H

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
