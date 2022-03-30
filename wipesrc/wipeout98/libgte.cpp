//
// LIBGTE.C for WATCOM C v10.0 by
//  Salim Siwani and Dave Smith,
//		     Psygnosis Ltd
//          07/02/1995
//

#include <math.h>
#include <string.h>
#define _SIZE_T
#include "wipesrc/psx26/include/sys/types.h"
#include "wipesrc/win95/daves/libgte.h"

VECTOR *ApplyMatrix(MATRIX *RTM, SVECTOR *v, VECTOR *r);

static float tmp;

extern int32_t					WinPitch;
extern int16_t				WinWidth;
extern int16_t				WinHeight;
extern int16_t				WinHeightX2;

// Global Variables for GTE...

MATRIX		LCM;			// local colour matrix
SVECTOR		LLV[3];			// local lighting vector
MATRIX		LLM;			// local lighting matrix

GTECVECTOR	BK;				// background colour
GTECVECTOR	FC;				// far colour

int32_t			FFAR;		// fog far
int32_t			FNEAR;		// fog near

int32_t			xOffset;	// geometry offset
int32_t			yOffset;
int32_t			screenZ;	// geometry screen Z
int32_t			SCR2;		// geometry screen Z / 2

MATRIX 		RTM;			// 1 current rotation matrix
GTECVECTOR 	grgb[3];		// 3 current colour values
int32_t			gsxy[3];	// 3 current screen coordinates
int32_t			gsz[4];		// 4 current screen z corrdinates

MATRIX		stack[20];		// 20 rotation matricies for stack
int32_t			stackPtr;

int32_t		MaxZValue;
int32_t		HalfZValue;

#define	TableSize	4096
double	SineTable[TableSize];





int32_t slow_rsin( int32_t a )
{
	return (int32_t)( sin( ((double)a / 4096.0) * 6.28318530718 ) * 4096.0);
}




int32_t ratan2(int32_t y, int32_t x)
{
	return((int32_t)((atan2((double) y, (double) x)) * 652));
}


void CreateSineTable(void)
{
	int32_t	angle;


	for(angle = 0; angle < TableSize; angle++)
		SineTable[angle] = slow_rsin(angle);
}



int32_t rsin(int32_t a)
{
	int32_t	index, result;


	index = a;

	if(abs(index) > TableSize)
		index %= TableSize;

	if(index < 0)
		index = -index;

	if(a < 0.0)
		result = -SineTable[index];
	else
		result = SineTable[index];

	return(result);
}



int32_t rcos(int32_t a)
{
	int32_t	index, result;


	index = a + 1024;

	if(abs(index) > TableSize)
		index %= TableSize;

	if(index < 0)
	{
		index = -index;
		result = -SineTable[index];
	}
	else
		result = SineTable[index];

	return(result);
}


int32_t AverageZ3( int32_t sz0, int32_t sz1, int32_t sz2 )
{
	int32_t a, b;

	a = (sz0+sz1+sz2) >> 2;
	b = a;					  //  1/4
	b >>= 2;
	a += b;					  // +1/16
	b >>= 2;
	a += b;					  // +1/64
	b >>= 2;
	a += b;					  // +1/256
	b >>= 2;
	a += b;					  // +1/1024
	return (a>>2);
}

int32_t AverageZ4( int32_t sz0, int32_t sz1, int32_t sz2, int32_t sz3 )
{
	return (sz0 + sz1 + sz2 + sz3)/16;
}


void InitGeom( void )
{
	// initalise stack pointer for rotation matricies
	stackPtr = 0;

	RTM.m[0][0] =  16652;
	RTM.m[0][1] = -31805;
	RTM.m[0][2] = -30657;
	RTM.m[1][0] =  17100;
	RTM.m[1][1] =  26345;
	RTM.m[1][2] =  25272;
	RTM.m[2][0] =   6882;
	RTM.m[2][1] = -11327;
	RTM.m[2][2] =  20672;

	RTM.t[0] = 0x40000401;
	RTM.t[1] = 0x40000401;
	RTM.t[2] = 0x40000401;

	LCM.m[0][0] = ONE;
	LCM.m[0][1] =   0;
	LCM.m[0][2] =   0;
	LCM.m[1][0] =   0;
	LCM.m[1][1] = ONE;
	LCM.m[1][2] =   0;
	LCM.m[2][0] =   0;
	LCM.m[2][1] =   0;
	LCM.m[2][2] = ONE;

	LLM.m[0][0] = -28320;
	LLM.m[0][1] = -29804;
	LLM.m[0][2] =   9801;
	LLM.m[1][0] =    782;
	LLM.m[1][1] = -23358;
	LLM.m[1][2] = -29799;
	LLM.m[2][0] =  21952;
	LLM.m[2][1] = -22357;
	LLM.m[2][2] =  31393;

	xOffset = 0;
	yOffset = 0;
	screenZ = 0x3e8;
	SCR2 = screenZ >> 1;

	BK.r = 0;
	BK.g = 0;
	BK.b = 255;

	FC.r = 0;
	FC.g = 255;
	FC.b = 0;

	MaxZValue = 65535;
	HalfZValue = MaxZValue >> 1;
}


MATRIX * MulMatrix0( MATRIX * m0, MATRIX * m1, MATRIX * m2 )
{
	RTM.m[0][0] = (((int32_t)m0->m[0][0] * (int32_t)m1->m[0][0]) + ((int32_t)m0->m[0][1] * (int32_t)m1->m[1][0]) + ((int32_t)m0->m[0][2] * (int32_t)m1->m[2][0])) >> SHIFT;
	RTM.m[0][1] = (((int32_t)m0->m[0][0] * (int32_t)m1->m[0][1]) + ((int32_t)m0->m[0][1] * (int32_t)m1->m[1][1]) + ((int32_t)m0->m[0][2] * (int32_t)m1->m[2][1])) >> SHIFT;
	RTM.m[0][2] = (((int32_t)m0->m[0][0] * (int32_t)m1->m[0][2]) + ((int32_t)m0->m[0][1] * (int32_t)m1->m[1][2]) + ((int32_t)m0->m[0][2] * (int32_t)m1->m[2][2])) >> SHIFT;

	RTM.m[1][0] = (((int32_t)m0->m[1][0] * (int32_t)m1->m[0][0]) + ((int32_t)m0->m[1][1] * (int32_t)m1->m[1][0]) + ((int32_t)m0->m[1][2] * (int32_t)m1->m[2][0])) >> SHIFT;
	RTM.m[1][1] = (((int32_t)m0->m[1][0] * (int32_t)m1->m[0][1]) + ((int32_t)m0->m[1][1] * (int32_t)m1->m[1][1]) + ((int32_t)m0->m[1][2] * (int32_t)m1->m[2][1])) >> SHIFT;
	RTM.m[1][2] = (((int32_t)m0->m[1][0] * (int32_t)m1->m[0][2]) + ((int32_t)m0->m[1][1] * (int32_t)m1->m[1][2]) + ((int32_t)m0->m[1][2] * (int32_t)m1->m[2][2])) >> SHIFT;

	RTM.m[2][0] = (((int32_t)m0->m[2][0] * (int32_t)m1->m[0][0]) + ((int32_t)m0->m[2][1] * (int32_t)m1->m[1][0]) + ((int32_t)m0->m[2][2] * (int32_t)m1->m[2][0])) >> SHIFT;
	RTM.m[2][1] = (((int32_t)m0->m[2][0] * (int32_t)m1->m[0][1]) + ((int32_t)m0->m[2][1] * (int32_t)m1->m[1][1]) + ((int32_t)m0->m[2][2] * (int32_t)m1->m[2][1])) >> SHIFT;
	RTM.m[2][2] = (((int32_t)m0->m[2][0] * (int32_t)m1->m[0][2]) + ((int32_t)m0->m[2][1] * (int32_t)m1->m[1][2]) + ((int32_t)m0->m[2][2] * (int32_t)m1->m[2][2])) >> SHIFT;

#ifdef WIPEOUTPC
	RTM.t[0] = m2->t[0];
	RTM.t[1] = m2->t[1];
	RTM.t[2] = m2->t[2];
#else
	RTM.t[0] = 0;
	RTM.t[1] = 0;
	RTM.t[2] = 0;
#endif

	(*m2) = RTM;

	return m2;
}


int16_t OnScreen3(int32_t xy0, int32_t xy1, int32_t xy2)
{
	DVECTOR *vxy0, *vxy1, *vxy2;

	//return 1;

	vxy0 = (DVECTOR *)&xy0;
	vxy1 = (DVECTOR *)&xy1;
	vxy2 = (DVECTOR *)&xy2;

	if (vxy0->vx > 320 || vxy0->vx < 0) return 0;
	if (vxy1->vx > 320 || vxy1->vx < 0) return 0;
	if (vxy2->vx > 320 || vxy2->vx < 0) return 0;

	if (vxy0->vy > 240 || vxy0->vy < 0) return 0;
	if (vxy1->vy > 240 || vxy1->vy < 0) return 0;
	if (vxy2->vy > 240 || vxy2->vy < 0) return 0;

	return 1;
}

int16_t OnScreen4(int32_t xy0, int32_t xy1, int32_t xy2, int32_t xy3)
{
	DVECTOR *vxy0, *vxy1, *vxy2, *vxy3;

	//return 1;

	vxy0 = (DVECTOR *)&xy0;
	vxy1 = (DVECTOR *)&xy1;
	vxy2 = (DVECTOR *)&xy2;
	vxy3 = (DVECTOR *)&xy3;

	if (vxy0->vx > 320 || vxy0->vx < 0) return 0;
	if (vxy1->vx > 320 || vxy1->vx < 0) return 0;
	if (vxy2->vx > 320 || vxy2->vx < 0) return 0;
	if (vxy3->vx > 320 || vxy3->vx < 0) return 0;

	if (vxy0->vy > 240 || vxy0->vy < 0) return 0;
	if (vxy1->vy > 240 || vxy1->vy < 0) return 0;
	if (vxy2->vy > 240 || vxy2->vy < 0) return 0;
	if (vxy3->vy > 240 || vxy3->vy < 0) return 0;

	return 1;
}

int32_t NormalClip( DVECTOR sxy0, DVECTOR sxy1, DVECTOR sxy2 )
{
	int32_t		sx0, sy0,
				dx1, dy1,
				dx2, dy2;
	DVECTOR 	*xy0, *xy1, *xy2;


	xy0 = (DVECTOR *)&sxy0;			// AndyY mod for signed x
	xy1 = (DVECTOR *)&sxy1;
	xy2 = (DVECTOR *)&sxy2;

	dx1 = xy1->vx - xy0->vx;
	dy1 = xy1->vy - xy0->vy;

	dx2 = xy2->vx - xy0->vx;
	dy2 = xy2->vy - xy0->vy;

	return ((dx1*dy2) - (dy1*dx2));
}



void NormalColorCol( SVECTOR * v0, CVECTOR * v1, CVECTOR * v2 )
{
	int32_t xt, yt, zt;


	xt = (int32_t)(v0->vx);
	yt = (int32_t)(v0->vy);
	zt = (int32_t)(v0->vz);

	LLV[2].vx = ( ( xt * (int32_t)(LLM.m[0][0]) ) + ( yt * (int32_t)(LLM.m[0][1]) ) + ( zt * (int32_t)(LLM.m[0][2]) ) ) >> SHIFT;
	LLV[2].vy = ( ( xt * (int32_t)(LLM.m[1][0]) ) + ( yt * (int32_t)(LLM.m[1][1]) ) + ( zt * (int32_t)(LLM.m[1][2]) ) ) >> SHIFT;
	LLV[2].vz = ( ( xt * (int32_t)(LLM.m[2][0]) ) + ( yt * (int32_t)(LLM.m[2][1]) ) + ( zt * (int32_t)(LLM.m[2][2]) ) ) >> SHIFT;

	xt = (int32_t)(LLV[2].vx);
	yt = (int32_t)(LLV[2].vy);
	zt = (int32_t)(LLV[2].vz);

	grgb[2].r = ( ( ( xt * (int32_t)(LCM.m[0][0]) ) + ( yt * (int32_t)(LCM.m[0][1]) ) + ( zt * (int32_t)(LCM.m[0][2]) ) ) >> SHIFT) + ( BK.r );
	grgb[2].g = ( ( ( xt * (int32_t)(LCM.m[1][0]) ) + ( yt * (int32_t)(LCM.m[1][1]) ) + ( zt * (int32_t)(LCM.m[1][2]) ) ) >> SHIFT) + ( BK.g );
	grgb[2].b = ( ( ( xt * (int32_t)(LCM.m[2][0]) ) + ( yt * (int32_t)(LCM.m[2][1]) ) + ( zt * (int32_t)(LCM.m[2][2]) ) ) >> SHIFT) + ( BK.b );

	v2->r = grgb[2].r = MULT(v1->r, grgb[2].r);
	v2->g = grgb[2].g = MULT(v1->g, grgb[2].g);
 	v2->b = grgb[2].b = MULT(v1->b, grgb[2].b);
}



void NormalColorCol3( SVECTOR * v0, SVECTOR * v1, SVECTOR * v2,
							 CVECTOR * v3,
							 CVECTOR * v4, CVECTOR * v5, CVECTOR * v6 )
{
	int32_t xt, yt, zt;


	xt = (int32_t)(v0->vx);
	yt = (int32_t)(v0->vy);
	zt = (int32_t)(v0->vz);

	LLV[0].vx = ( ( xt * (int32_t)(LLM.m[0][0]) ) + ( yt * (int32_t)(LLM.m[0][1]) ) + ( zt * (int32_t)(LLM.m[0][2]) ) ) >> SHIFT;
	LLV[0].vy = ( ( xt * (int32_t)(LLM.m[1][0]) ) + ( yt * (int32_t)(LLM.m[1][1]) ) + ( zt * (int32_t)(LLM.m[1][2]) ) ) >> SHIFT;
	LLV[0].vz = ( ( xt * (int32_t)(LLM.m[2][0]) ) + ( yt * (int32_t)(LLM.m[2][1]) ) + ( zt * (int32_t)(LLM.m[2][2]) ) ) >> SHIFT;

	xt = (int32_t)(v1->vx);
	yt = (int32_t)(v1->vy);
	zt = (int32_t)(v1->vz);

	LLV[1].vx = ( ( xt * (int32_t)(LLM.m[0][0]) ) + ( yt * (int32_t)(LLM.m[0][1]) ) + ( zt * (int32_t)(LLM.m[0][2]) ) ) >> SHIFT;
	LLV[1].vy = ( ( xt * (int32_t)(LLM.m[1][0]) ) + ( yt * (int32_t)(LLM.m[1][1]) ) + ( zt * (int32_t)(LLM.m[1][2]) ) ) >> SHIFT;
	LLV[1].vz = ( ( xt * (int32_t)(LLM.m[2][0]) ) + ( yt * (int32_t)(LLM.m[2][1]) ) + ( zt * (int32_t)(LLM.m[2][2]) ) ) >> SHIFT;

	xt = (int32_t)(v2->vx);
	yt = (int32_t)(v2->vy);
	zt = (int32_t)(v2->vz);

	LLV[2].vx = ( ( xt * (int32_t)(LLM.m[0][0]) ) + ( yt * (int32_t)(LLM.m[0][1]) ) + ( zt * (int32_t)(LLM.m[0][2]) ) ) >> SHIFT;
	LLV[2].vy = ( ( xt * (int32_t)(LLM.m[1][0]) ) + ( yt * (int32_t)(LLM.m[1][1]) ) + ( zt * (int32_t)(LLM.m[1][2]) ) ) >> SHIFT;
	LLV[2].vz = ( ( xt * (int32_t)(LLM.m[2][0]) ) + ( yt * (int32_t)(LLM.m[2][1]) ) + ( zt * (int32_t)(LLM.m[2][2]) ) ) >> SHIFT;

	xt = (int32_t)(LLV[0].vx);
	yt = (int32_t)(LLV[0].vy);
	zt = (int32_t)(LLV[0].vz);

	grgb[0].r = ( ( ( xt * (int32_t)(LCM.m[0][0]) ) + ( yt * (int32_t)(LCM.m[0][1]) ) + ( zt * (int32_t)(LCM.m[0][2]) ) ) >> SHIFT) + ( BK.r );
	grgb[0].g = ( ( ( xt * (int32_t)(LCM.m[1][0]) ) + ( yt * (int32_t)(LCM.m[1][1]) ) + ( zt * (int32_t)(LCM.m[1][2]) ) ) >> SHIFT) + ( BK.g );
	grgb[0].b = ( ( ( xt * (int32_t)(LCM.m[2][0]) ) + ( yt * (int32_t)(LCM.m[2][1]) ) + ( zt * (int32_t)(LCM.m[2][2]) ) ) >> SHIFT) + ( BK.b );

	xt = (int32_t)(LLV[1].vx);
	yt = (int32_t)(LLV[1].vy);
	zt = (int32_t)(LLV[1].vz);

	grgb[1].r = ( ( ( xt * (int32_t)(LCM.m[0][0]) ) + ( yt * (int32_t)(LCM.m[0][1]) ) + ( zt * (int32_t)(LCM.m[0][2]) ) ) >> SHIFT) + ( BK.r );
	grgb[1].g = ( ( ( xt * (int32_t)(LCM.m[1][0]) ) + ( yt * (int32_t)(LCM.m[1][1]) ) + ( zt * (int32_t)(LCM.m[1][2]) ) ) >> SHIFT) + ( BK.g );
	grgb[1].b = ( ( ( xt * (int32_t)(LCM.m[2][0]) ) + ( yt * (int32_t)(LCM.m[2][1]) ) + ( zt * (int32_t)(LCM.m[2][2]) ) ) >> SHIFT) + ( BK.b );

	xt = (int32_t)(LLV[2].vx);
	yt = (int32_t)(LLV[2].vy);
	zt = (int32_t)(LLV[2].vz);

	grgb[2].r = ( ( ( xt * (int32_t)(LCM.m[0][0]) ) + ( yt * (int32_t)(LCM.m[0][1]) ) + ( zt * (int32_t)(LCM.m[0][2]) ) ) >> SHIFT) + ( BK.r );
	grgb[2].g = ( ( ( xt * (int32_t)(LCM.m[1][0]) ) + ( yt * (int32_t)(LCM.m[1][1]) ) + ( zt * (int32_t)(LCM.m[1][2]) ) ) >> SHIFT) + ( BK.g );
	grgb[2].b = ( ( ( xt * (int32_t)(LCM.m[2][0]) ) + ( yt * (int32_t)(LCM.m[2][1]) ) + ( zt * (int32_t)(LCM.m[2][2]) ) ) >> SHIFT) + ( BK.b );

	v4->r = grgb[0].r = MULT(v3->r, grgb[0].r);
	v4->g = grgb[0].g = MULT(v3->g, grgb[0].g);
	v4->b = grgb[0].b = MULT(v3->b, grgb[0].b);

	v5->r = grgb[1].r = MULT(v3->r, grgb[1].r);
	v5->g = grgb[1].g = MULT(v3->g, grgb[1].g);
	v5->b = grgb[1].b = MULT(v3->b, grgb[1].b);

	v6->r = grgb[2].r = MULT(v3->r, grgb[2].r);
	v6->g = grgb[2].g = MULT(v3->g, grgb[2].g);
	v6->b = grgb[2].b = MULT(v3->b, grgb[2].b);
}



void NormalColorDpq3( SVECTOR * v0, SVECTOR * v1, SVECTOR * v2,
							 CVECTOR * v3, int32_t p,
							 CVECTOR * v4, CVECTOR * v5, CVECTOR * v6 )
{
	int32_t xt, yt, zt;
	int32_t p2 = 4096 - p;


	xt = (int32_t)(v0->vx);
	yt = (int32_t)(v0->vy);
	zt = (int32_t)(v0->vz);

	LLV[0].vx = ( ( xt * (int32_t)(LLM.m[0][0]) ) + ( yt * (int32_t)(LLM.m[0][1]) ) + ( zt * (int32_t)(LLM.m[0][2]) ) ) >> SHIFT;
	LLV[0].vy = ( ( xt * (int32_t)(LLM.m[1][0]) ) + ( yt * (int32_t)(LLM.m[1][1]) ) + ( zt * (int32_t)(LLM.m[1][2]) ) ) >> SHIFT;
	LLV[0].vz = ( ( xt * (int32_t)(LLM.m[2][0]) ) + ( yt * (int32_t)(LLM.m[2][1]) ) + ( zt * (int32_t)(LLM.m[2][2]) ) ) >> SHIFT;

	xt = (int32_t)(v1->vx);
	yt = (int32_t)(v1->vy);
	zt = (int32_t)(v1->vz);

	LLV[1].vx = ( ( xt * (int32_t)(LLM.m[0][0]) ) + ( yt * (int32_t)(LLM.m[0][1]) ) + ( zt * (int32_t)(LLM.m[0][2]) ) ) >> SHIFT;
	LLV[1].vy = ( ( xt * (int32_t)(LLM.m[1][0]) ) + ( yt * (int32_t)(LLM.m[1][1]) ) + ( zt * (int32_t)(LLM.m[1][2]) ) ) >> SHIFT;
	LLV[1].vz = ( ( xt * (int32_t)(LLM.m[2][0]) ) + ( yt * (int32_t)(LLM.m[2][1]) ) + ( zt * (int32_t)(LLM.m[2][2]) ) ) >> SHIFT;

	xt = (int32_t)(v2->vx);
	yt = (int32_t)(v2->vy);
	zt = (int32_t)(v2->vz);

	LLV[2].vx = ( ( xt * (int32_t)(LLM.m[0][0]) ) + ( yt * (int32_t)(LLM.m[0][1]) ) + ( zt * (int32_t)(LLM.m[0][2]) ) ) >> SHIFT;
	LLV[2].vy = ( ( xt * (int32_t)(LLM.m[1][0]) ) + ( yt * (int32_t)(LLM.m[1][1]) ) + ( zt * (int32_t)(LLM.m[1][2]) ) ) >> SHIFT;
	LLV[2].vz = ( ( xt * (int32_t)(LLM.m[2][0]) ) + ( yt * (int32_t)(LLM.m[2][1]) ) + ( zt * (int32_t)(LLM.m[2][2]) ) ) >> SHIFT;

	xt = (int32_t)(LLV[0].vx);
	yt = (int32_t)(LLV[0].vy);
	zt = (int32_t)(LLV[0].vz);

	grgb[0].r = ( ( ( xt * (int32_t)(LCM.m[0][0]) ) + ( yt * (int32_t)(LCM.m[0][1]) ) + ( zt * (int32_t)(LCM.m[0][2]) ) ) >> SHIFT) + ( BK.r );
	grgb[0].g = ( ( ( xt * (int32_t)(LCM.m[1][0]) ) + ( yt * (int32_t)(LCM.m[1][1]) ) + ( zt * (int32_t)(LCM.m[1][2]) ) ) >> SHIFT) + ( BK.g );
	grgb[0].b = ( ( ( xt * (int32_t)(LCM.m[2][0]) ) + ( yt * (int32_t)(LCM.m[2][1]) ) + ( zt * (int32_t)(LCM.m[2][2]) ) ) >> SHIFT) + ( BK.b );

	xt = (int32_t)(LLV[1].vx);
	yt = (int32_t)(LLV[1].vy);
	zt = (int32_t)(LLV[1].vz);

	grgb[1].r = ( ( ( xt * (int32_t)(LCM.m[0][0]) ) + ( yt * (int32_t)(LCM.m[0][1]) ) + ( zt * (int32_t)(LCM.m[0][2]) ) ) >> SHIFT) + ( BK.r );
	grgb[1].g = ( ( ( xt * (int32_t)(LCM.m[1][0]) ) + ( yt * (int32_t)(LCM.m[1][1]) ) + ( zt * (int32_t)(LCM.m[1][2]) ) ) >> SHIFT) + ( BK.g );
	grgb[1].b = ( ( ( xt * (int32_t)(LCM.m[2][0]) ) + ( yt * (int32_t)(LCM.m[2][1]) ) + ( zt * (int32_t)(LCM.m[2][2]) ) ) >> SHIFT) + ( BK.b );

	xt = (int32_t)(LLV[2].vx);
	yt = (int32_t)(LLV[2].vy);
	zt = (int32_t)(LLV[2].vz);

	grgb[2].r = ( ( ( xt * (int32_t)(LCM.m[0][0]) ) + ( yt * (int32_t)(LCM.m[0][1]) ) + ( zt * (int32_t)(LCM.m[0][2]) ) ) >> SHIFT) + ( BK.r );
	grgb[2].g = ( ( ( xt * (int32_t)(LCM.m[1][0]) ) + ( yt * (int32_t)(LCM.m[1][1]) ) + ( zt * (int32_t)(LCM.m[1][2]) ) ) >> SHIFT) + ( BK.g );
	grgb[2].b = ( ( ( xt * (int32_t)(LCM.m[2][0]) ) + ( yt * (int32_t)(LCM.m[2][1]) ) + ( zt * (int32_t)(LCM.m[2][2]) ) ) >> SHIFT) + ( BK.b );

	v4->r = grgb[0].r = ( (p2 * v3->r * grgb[0].r) + (p * FC.r) ) >> SHIFT;
	v4->g = grgb[0].g = ( (p2 * v3->g * grgb[0].g) + (p * FC.g) ) >> SHIFT;
	v4->b = grgb[0].b = ( (p2 * v3->b * grgb[0].b) + (p * FC.b) ) >> SHIFT;

	v5->r = grgb[1].r = ( (p2 * v3->r * grgb[1].r) + (p * FC.r) ) >> SHIFT;
	v5->g = grgb[1].g = ( (p2 * v3->g * grgb[1].g) + (p * FC.g) ) >> SHIFT;
	v5->b = grgb[1].b = ( (p2 * v3->b * grgb[1].b) + (p * FC.b) ) >> SHIFT;

	v6->r = grgb[2].r = ( (p2 * v3->r * grgb[2].r) + (p * FC.r) ) >> SHIFT;
	v6->g = grgb[2].g = ( (p2 * v3->g * grgb[2].g) + (p * FC.g) ) >> SHIFT;
	v6->b = grgb[2].b = ( (p2 * v3->b * grgb[2].b) + (p * FC.b) ) >> SHIFT;
}



void ReadRotMatrix( MATRIX * m )
{
	*m = RTM;
}


MATRIX * RotMatrix( SVECTOR * r, MATRIX * m )
{
	int32_t	cx, sx;
	int32_t	cy, sy;
	int32_t	cz, sz;

	sx = rsin( r->vx );	cx = rcos( r->vx );
	sy = rsin( r->vy );	cy = rcos( r->vy );
	sz = rsin( r->vz );	cz = rcos( r->vz );

	m->m[0][0] =  MULT(cy,cz);
	m->m[0][1] = -MULT(cy,sz);
	m->m[0][2] =  sy;
	m->m[1][0] =  ((sz*cx) + (MULT(sy,sx)*cz)) >> SHIFT;
	m->m[1][1] =  ((cx*cz) - (MULT(sy,sx)*sz)) >> SHIFT;
	m->m[1][2] = -MULT(cy,sx);
	m->m[2][0] =  ((sz*sx) - (MULT(cx,cz)*sy)) >> SHIFT;
	m->m[2][1] =  ((sx*cz) + (MULT(cx,sz)*sy)) >> SHIFT;
	m->m[2][2] =  MULT(cy,cx);

	return m;
}



void RotTrans( SVECTOR * v0, VECTOR * v1, int32_t * flag )
{
	register int32_t z;

	ApplyMatrix( &RTM, v0, v1 );

	z = v1->vz;
	if( z < 0 )							{	*flag |= 0x00040000;					}
	if( z > 32768 )						{	*flag |= 0x00400000;					}
	if( z < SCR2 )						{	*flag |= 0x00020000; z = SCR2;			}
	if( z > 65536 )						{						 z = 65536;			}
	//if( (z < FNEAR) || (z > FFAR) )		{	*flag |= 0x00001000;					}
	if( v1->vx > 32767 )				{	*flag |= 0x81000000; v1->vx = 32767;	}
	if( v1->vy > 32767 )				{	*flag |= 0x80800000; v1->vy = 32767;	}
	if( v1->vx < -32767 )				{	*flag |= 0x81000000; v1->vx = -32767;	}
	if( v1->vy < -32767 )				{	*flag |= 0x80800000; v1->vy = -32767;	}
}

int32_t RotTransPersForN(SVECTOR *v0, int32_t *sxy, int32_t *p, int32_t *flag)
{
	VECTOR 	r0;


	ApplyMatrix(&RTM, v0, &r0);
	DLSRotTransPers(sxy, &r0, flag);
/*
	if (r0.vz < FNEAR)
		*p = 0;
	else if (r0.vz > FFAR)
		*p = 4095;
	else
		tmp = (((r0.vz - FNEAR) * 4095.0) / (float)(FFAR-FNEAR));
		*p = (int32_t)tmp;
*/
	return r0.vz;
}


void RotTransPersN(SVECTOR *v0,DVECTOR *v1,uint16_t *sz,uint16_t *flag,int32_t n)
{
	int32_t loop;
	SVECTOR *ip;
	//DVECTOR v2;
	DVECTOR *op;
	//SVECTOR *stmp;
	int32_t	crap;
	int32_t	long_flag=0;

// printf("\n\n!!!!!!!!! RotTransPersN !!!!!!!!!!!\n\n");


	ip = v0;
	//op = &v2;
	crap = 0;
	op = v1;

	for (loop =0; loop < n; loop++)
	{
		*sz = (uint16_t)RotTransPersForN(ip,(int32_t *)op,&crap,&long_flag);
		sz++;
 		*flag = (uint16_t)(long_flag >> 12);
		flag++;

		//stmp->vx = op->vx;
		//stmp->vy = op->vy;
		//stmp->vz = ip->vz;
		ip++;
		op++;
		//stmp++;

	}
}



int32_t RotTransPers(SVECTOR *v0, DVECTOR *sxy, int32_t *p, int32_t *flag)
{
	VECTOR 	r0;
	//DVECTOR sxy1;
	//int32_t v1[3];
	//int32_t *v2;

	//v2 = &v1[0];

	ApplyMatrix(&RTM, v0, &r0);
	DLSRotTransPers((int32_t *)sxy, &r0, flag);

/*
	if (r0.vz < FNEAR)
		*p = 0;
	else if (r0.vz > FFAR)
		*p = 4095;
	else
		*p = (int32_t)(((r0.vz - FNEAR) * 4095.0) / (float)(FFAR-FNEAR));
*/
	//sxy->vx = v1[0];
	//sxy->vy = v1[1];
	//sxy->vz = v1[2];

	return r0.vz >> 2;
}


void SetBackColor( int32_t rbk, int32_t gbk, int32_t bbk )
{
	BK.r = rbk;
	BK.g = gbk;
	BK.b = bbk;
}



void SetColorMatrix( MATRIX * m )
{
	LCM = *m;
}



void SetDrawDistance(int32_t Dist)
{
	MaxZValue = Dist;
	HalfZValue = Dist >> 1;
}

void SetGeomOffset( int32_t ofx, int32_t ofy)
{
	xOffset = ofx;
	yOffset = ofy;
}



void SetGeomScreen( int32_t h )
{
	screenZ = h;
	SCR2 = h >> 1;
}



void SetLightMatrix( MATRIX * m )
{
	LLM = *m;
}



void SetRotMatrix( MATRIX * m )
{
	memcpy( &(RTM.m), &(m->m), sizeof(int16_t)*9 );
}



void SetTransMatrix( MATRIX * m )
{
	memcpy( &(RTM.t), &(m->t), sizeof(int32_t)*3 );
}


int32_t SquareRoot0( int32_t a )
{
uint32_t b;
	b = (uint32_t)a ;
	return sqrt((uint32_t) b );
}



MATRIX * TransMatrix( MATRIX * m, VECTOR * v )
{
	m->t[0] = v->vx;
	m->t[1] = v->vy;
	m->t[2] = v->vz;

	return m;
}

int32_t xt,yt,zt;
//------------------------------------------------------
// ApplyMatrix
//
//	Multiplies RTM by v0 and puts the
//	result into r0 (or whatever)
//
//	Inputs:
//		eax = RTM
//		ebx = v0 (input vector)
//		ecx = r0 (output)
//------------------------------------------------------

/*__declspec(naked)*/ VECTOR *ApplyMatrix(MATRIX *RTM, SVECTOR *v, VECTOR *r)
{
	// TODO(boardwalk) Implement me without __declspace(naked)
	/*
	__asm
	{

		push 	edi
		push	ebx

		mov 	edi, 12[esp]		; store parameters
		mov		ebx, 16[esp]
		mov 	edx, 20[esp]

		mov	cx, [ebx]
		mov	ax, [ebx+2]
		mov	bx, [ebx+4]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		mov 	xt, ecx
		mov 	yt, eax
		mov 	zt, ebx


		mov  	cx, [edi]
		mov	ax, [edi+2]
		mov	bx, [edi+4]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		imul	ecx, xt
		imul	eax, yt
		imul	ebx, zt
		sar	ecx, SHIFT
		sar	eax, SHIFT
		sar	ebx, SHIFT
		add	ecx, eax
		add	ecx, ebx
		add	ecx, [edi+20]
		mov 	0[edx], ecx		; store value in vector->vx


		mov  	cx, [edi+6]
		mov	ax, [edi+8]
		mov	bx, [edi+10]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		imul	ecx, xt
		imul	eax, yt
		imul	ebx, zt
		sar	ecx, SHIFT
		sar	eax, SHIFT
		sar	ebx, SHIFT
		add	ecx, eax
		add	ecx, ebx
		add	ecx, [edi+24]
		mov 	4[edx], ecx		; store value in vector->vx

		mov  	cx, [edi+12]
		mov	ax, [edi+14]
		mov	bx, [edi+16]
		sal	ecx, 16
		sal	eax, 16
		sal	ebx, 16
		sar	ecx, 16
		sar	eax, 16
		sar	ebx, 16
		imul	ecx, xt
		imul	eax, yt
		imul	ebx, zt
		sar	ecx, SHIFT
		sar	eax, SHIFT
		sar	ebx, SHIFT
		add	ecx, eax
		add	ecx, ebx
		add	ecx, [edi+28]
		mov 	8[edx], ecx		; store value in vector->vx

		pop	ebx
		pop 	edi
		ret
	}
	*/
	return NULL;
}

int32_t x,y,z,flagaddr;

//------------------------------------------------------
// DLSRotTransPers
//
//	Moves the body of RotTransPers into
//	an assembly function
//
//	Inputs:
//		eax = address of screen coords
//		ebx = address of input coords
//		ecx = address of flag variable
//------------------------------------------------------

/*__declspec(naked)*/ int32_t *DLSRotTransPers(int32_t *sxy, VECTOR *r0, int32_t *flag)
{
	// TODO(boardwalk) Implement me without __declspace(naked)
	/*
	__asm
	{
		push ebx
		mov	eax,8[esp]
		mov	sxy, eax
		mov	ebx,12[esp]
		mov	r0, ebx
		mov	ecx,16[esp]
		mov	flagaddr, ecx
		mov	[flag], 0			; clear flag

		mov	eax, [ebx]		; move r0.vx into x
		mov	x, eax
		mov	eax, [ebx+4]		; move r0.vy into y
		mov	y, eax
		mov	eax, [ebx+8]		; move r0.vz into eax
		mov	z, eax

		cmp	z, 0
		jge	nonegz
		or	[flag], 00040000h

nonegz:
		mov	ebx, [HalfZValue]
		cmp	z, ebx
		jle	nobigz
		or	[flag], 00400000h

nobigz:
		mov	ebx, [SCR2]
		cmp	z, ebx
		jge	noscr2clip
		mov	z, ebx
		or	[flag], 00020000h

noscr2clip:
		mov	ebx, [MaxZValue]
		cmp	z, ebx
		jle	nomaxzclip
		mov	z, ebx

nomaxzclip:
		mov	ebx, [FNEAR]
		cmp	z, ebx
		jge	nozfnear
		or	[flag], 00001000h
		jmp	nozffar

nozfnear:
		mov	ebx, [FFAR]
		cmp	z, ebx
		jle	nozffar
		or	[flag], 00001000h

nozffar:
		cmp	x, 32767
		jle	nomaxxclip
		mov	x, 32767
		or	[flag], 81000000h

nomaxxclip:
		cmp	y, 32767
		jle	nomaxyclip
		mov	y, 32767
		or	[flag], 80800000h

nomaxyclip:
		cmp	x, -32767
		jge	nominxclip
		mov	x, -32767
		or	[flag], 81000000h

nominxclip:
		cmp	y, -32767
		jge	nominyclip
		mov	y, -32767
		or	[flag], 80800000h

nominyclip:
		mov	eax,[screenZ]		; Finished checks, do mul
		imul	x
		idiv	z
		add	eax,[xOffset]
		mov	ebx, eax		; xt0 in ebx

		mov	eax,[screenZ]		; Finished checks, do mul
		imul	y
		idiv	z
		add	eax,[yOffset]
		mov	ecx, eax		; yt0 in ecx

		cmp	ebx, -1024		; clip the screen coords
		jge	nominxt0clip
		mov	ebx, -1024
		or	[flag], 80004000h

nominxt0clip:
		cmp	ebx, 1024
		jle	nomaxxt0clip
		mov	ebx, 1024
		or	[flag], 80004000h

nomaxxt0clip:
		cmp	ecx, -1024
		jge	nominyt0clip
		mov	ecx, -1024
		or	[flag], 80002000h

nominyt0clip:
		cmp	ecx, 1024
		jle	nomaxyt0clip
		mov	ecx, 1024
		or	[flag], 80002000h

nomaxyt0clip:
		and	ebx, 0000FFFFh
		sal	ecx, 16			; equivalent to
		or	ebx, ecx		; (yt0 << 16) | (xt0 & 0x0000FFFF);
		mov	edx, sxy
		mov	[edx], ebx		; result stored in sxy
		mov	ebx, [flag]
		mov	edx, flagaddr
		mov	[edx], ebx		; flag result stored in flag

		pop	ebx
		ret
	}
	*/
	return NULL;
}
