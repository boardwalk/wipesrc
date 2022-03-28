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
	short			highResTableIndex;						 	/* Index into shipShapes for high-res model */
	short			lowResTableIndex;							 	/* Index into shipShapes for low-res model */

	/***** Dynamic Attributes *****/
	TrackSection *nearTrkSect, *prevShipSect;
	int 			attr;												/* Ship attributes */
	int 			combatAttr;											/* Ship attributes */
	VECTOR 		unitVecNose;		 /* SVECTOR  ? */  	/* Unit vector along ships nose */
	VECTOR 		unitVecWing;		 /* SVECTOR  ? */  	/* Unit vector along ships nose */
	VECTOR 		ppivot;										 	/* 32bit ship position */
	VECTOR 		vpivot, apivot;							 	/* Velocity, accn vectors for ship */
	short 		hdg, pitch, roll;							 	/* Orientation attributes */
	short 		vhdg, vpitch, vroll;	
	short 		ahdg;						
	short 		mass;											 	/* Mass of ship */
 	VECTOR 		thrust;										  	/* Thrust vector */
	short 		r_brake_rot, l_brake_rot;					/* Air brakes */
	u_short 		speed;										  	/* speed */
	short 		thrust_mag;				 					/* Thrust magnitude */
	short 		max_thrust;								  	/* Miximum thrust */
	short 		collideCount;
	short 		headingInc, maxHeading;
	short 		resistance;
	short 		skid;

	/***** Remote Ship Attributes *****/
	short			fightBack, numtospeed	;
	u_short 		remoteThrustMag; 		 					/* Thrust magnitude */
	u_short 		remoteMaxThrust; 						  	/* Miximum thrust */
	short			DPASectDiff;	

	/***** Weapon Attributes *****/
	u_char 		weaponType;
	u_char 		targetShip;
	short  		haveFired;
	short			electroCount;
	short			revConCount;
	short			specialCount;

	/***** Race Control Attributes *****/
	int 			lapTime;
	char 			lapNo;
    char			prevLapNo;
	char 			position;
	short 		nearSecNo, prevSecNo ;
	int 			lapTimes[5];
	int 			updateCount;

	/***** Control Routines *****/
	void 			(*updateControl)(VECTOR* , struct ShipData*, Face*, int);
	void 			(*update)(struct ShipData *, Object**, int, struct WeaponData*);

} ShipData;

struct ConfigData ;

void loadShipData(TrackSection* section, ShipData *shipIndex, Object** shipShapes, Skeleton* camPos);
void DrawShips(ShipData *shipIndex, Object** shipShapes, int camShip, TrackCamera camera);
void initShipData(TrackSection* section, ShipData *shipIndex, ConfigData *gameData);
