#ifndef SHIPS_H
#define SHIPS_H

#define NO_OF_SHIPS 8

#define IN_TOW			 	0x1
#define REMOTE_VIEW	 	0x2
#define VIEW_IN			0x4
#define DIRECTION			0x8

#define FLYING				0x10
#define LSIDE				0x20
#define RACING				0x40
#define SHIP_COLL			0x80

#define ON_JUNCTION		0x100
#define VISIBLE			0x200
#define RESCUE 			0x400
#define OVERTAKE 			0x800

#define JUST_IN_FRONT	0x1000
#define JUNC_LEFT			0x2000
#define SHIELDED			0x4000
#define ELECTROED			0x8000

#define REVCONNED			0x1
#define SPECIALED			0x2


#define NEW_SHIP 0
#define RESCUE_DROID 16

struct WeaponData;
struct TrackCamera;

typedef struct ShipData
{
	/***** Graphics Attributes *****/
	int16_t			highResTableIndex;						 	/* Index into shipShapes for high-res model */
	int16_t			lowResTableIndex;							 	/* Index into shipShapes for low-res model */

	/***** Dynamic Attributes *****/
	TrackSection *nearTrkSect, *prevShipSect;
	int32_t 			attr;												/* Ship attributes */
	int32_t 			combatAttr;											/* Ship attributes */
	VECTOR 		unitVecNose;		 /* SVECTOR  ? */  	/* Unit vector along ships nose */
	VECTOR 		unitVecWing;		 /* SVECTOR  ? */  	/* Unit vector along ships nose */
	VECTOR 		ppivot;										 	/* 32bit ship position */
	VECTOR 		vpivot, apivot;							 	/* Velocity, accn vectors for ship */
	int16_t 		hdg, pitch, roll;							 	/* Orientation attributes */
	int16_t 		vhdg, vpitch, vroll;
	int16_t 		ahdg;
	int16_t 		mass;											 	/* Mass of ship */
 	VECTOR 		thrust;										  	/* Thrust vector */
	int16_t 		r_brake_rot, l_brake_rot;					/* Air brakes */
	uint16_t 		speed;										  	/* speed */
	int16_t 		thrust_mag;				 					/* Thrust magnitude */
	int16_t 		max_thrust;								  	/* Miximum thrust */
	int16_t 		collideCount;
	int16_t 		headingInc, maxHeading;
	int16_t 		resistance;
	int16_t 		skid;

	/***** Remote Ship Attributes *****/
	int16_t			fightBack, numtospeed	;
	uint16_t 		remoteThrustMag; 		 					/* Thrust magnitude */
	uint16_t 		remoteMaxThrust; 						  	/* Miximum thrust */
	int16_t			DPASectDiff;

	/***** Weapon Attributes *****/
	uint8_t 		weaponType;
	uint8_t 		targetShip;
	int16_t  		haveFired;
	int16_t			electroCount;
	int16_t			revConCount;
	int16_t			specialCount;

	/***** Race Control Attributes *****/
	int32_t 			lapTime;
	char 			lapNo;
    char			prevLapNo;
	char 			position;
	int16_t 		nearSecNo, prevSecNo ;
	int32_t 			lapTimes[5];
	int32_t 			updateCount;

	/***** Control Routines *****/
	void 			(*updateControl)(VECTOR* , struct ShipData*, Face*, int32_t);
	void 			(*update)(struct ShipData *, Object**, int32_t, struct WeaponData*);

} ShipData;

struct ConfigData ;

void loadShipData(TrackSection* section, ShipData *shipIndex, Object** shipShapes, Skeleton* camPos);
void DrawShips(ShipData *shipIndex, Object** shipShapes, int32_t camShip, TrackCamera camera);
void initShipData(TrackSection* section, ShipData *shipIndex, ConfigData *gameData);

#endif
