void race		( combatData *packetData, ConfigData *gameData, int mode, int timeout );
void Restart	(	ShipData 		*shipIndex,
				 		ConfigData 		*gameData,
						WeaponData 		*weaponIndex,
						WeaponShape		*weaponShapes,
						Effect			*effects,
                        RescueData      *rescueDroid,
                        Object          **shipShapes);
void QuitGame	();
