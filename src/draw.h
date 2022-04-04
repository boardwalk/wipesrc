#ifndef DRAW_H
#define DRAW_H

#include "ships.h"
#include "track.h"

#define ShipForwards    1
#define ShipBackwards   2
#define ShipRight       4
#define ShipLeft    	   8

#define North           1
#define South           2
#define East	         4
#define West	         8


char TrackDirection
(
	TrackCamera*		camera
);




void DrawLandscape
(
	ShipData			*playerShip,
	TrackCamera		*camera
);

#endif
