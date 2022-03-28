#define  PadLeft0       0x80
#define  PadDown0       0x40
#define  PadRight0      0x20
#define  PadUp0         0x10

#define  PadSquare0     0x8000
#define  PadCross0      0x4000
#define  PadCircle0     0x2000
#define  PadTriangle0   0x1000

#define  PadSelect      0x01
#define  PadStart       0x08
#define  PadUnused1     0x02
#define  PadUnused2     0x04

#define  PadTopLeft1    0x0400
#define  PadTopLeft2    0x0100
#define  PadTopRight1   0x0800
#define  PadTopRight2   0x0200

#define NEG_UP			0x0010
#define NEG_DOWN		0x0040
#define NEG_R_SHIFT		0x0800
#define NEG_B			0x1000
#define NEG_A			0x2000

#define NEG_START		0x0008
#define NEG_LEFT		0x0080
#define NEG_RIGHT		0x0020

#define M_UP		0
#define M_DOWN		1
#define M_LEFT		2
#define M_RIGHT	3
#define M_BACK		4
#define M_SELECT	5

#define M_START	6
#define M_QUIT		7

#define A_THRUST_I		0x01
#define A_THRUST_II		0x80
#define D_THRUST		0x02
#define A_VIEW			0x04
#define D_VIEW			0x08
#define A_FIRE_I	 	0x10
#define A_FIRE_II	 	0x20
#define D_FIRE			0x40


typedef struct PadInfo
{
	u_short		thrust;
	u_short		view;
	u_short		fire;
	u_short		lBrake;
	u_short		rBrake;
	u_short		up;
	u_short		down;
	u_short		left;
	u_short		right;
} PadInfo;

typedef struct NegInfo
{
	u_short			type;
	u_short			thrust;
	u_short			view;
	u_short			fire;
	u_short			lBrake;
	u_short			rBrake;
	int				twistFactor;
} NegInfo;


void 	InitPadSettings	();
void 	SetBrake				(ShipData *playerShip, short padLeft, short padRight);
void 	SetView				(ShipData *playerShip, short padView);
void 	SetFire				(ShipData *playerShip, short padFire, WeaponData *weaponIndex, int shipNo);
short GetMenuKey			(short key);
