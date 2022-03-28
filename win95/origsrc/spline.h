/*    */

TrackSection* FindAllNearestSection( VECTOR pos );

void UpdateShipRemote(register ShipData *shipIndex, Object** shipShapes, int shipNo, WeaponData *weaponIndex);
void UpdateShipLinearInterpolateSpline( register ShipData *shipIndex, Object** shipShapes, int shipNo, WeaponData *weaponIndex );

void UpdateCameraLinearInterpolateSpline(TrackCamera *camera, ShipData *ship, RescueData *rescueDroid);
void UpdateCameraHighUp( TrackCamera *camera, ShipData *ship, RescueData *rescueDroid );

void UpdateCameraExtroAttract(TrackCamera *camera, ShipData *camShip, RescueData *rescueDroid);


void InitFireCongratulations();
void InitIceCongratulations();

void ShowFireCongratulations( void );
void ShowIceCongratulations( void );
