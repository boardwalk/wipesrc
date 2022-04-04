/*
 *  (C) Copyright 1993/1994 Sony Computer Entertainment ,Tokyo,Japan.
 *				All Rights Reserved
 *
 * 		libgte.h: Geometry Basic Structures Database
 *
 *$Id: libgte.h,v 1.8 1994/10/17 07:31:22 ohba Exp $
 */


//
// Converted to WATCOM C v10.0 by
// Salim Siwani, Psygnosis Ltd
//       06/02/1995
//

#ifndef LIBGTE_H
#define LIBGTE_H



/*
 * Geometry Structures:
 */

//#define ONE		4096	/* GTE regards 4096 as 1.0 */
//#define SHIFT		12		/* number of bits to shift for 4096 */
//#define SHIFT2	24		/*  */

//#define ONE		65536	/* GTE now regards 65536 as 1.0 */
//#define SHIFT		16		/* number of bits to shift for 4096 */
//#define SHIFT2	32		/*  */

#define	ONE			4096
#define ONE_FLOAT	4096.0
#define SHIFT		12		/* number of bits to shift for 4096 */
#define SHIFT2		24		/*  */

#define TMPVTXNUM	16		/* Clip Function Header */
#define OUTVTXNUM	10


#define MULT( a, b )		(((int32_t)(a) * (int32_t)(b)) >> SHIFT)



#ifdef ASSEMBLER
/*
* GTE read macros
*/
#define	read_sz_fifo3(r1,r2,r3)		mfc2	r1,$17; \
					mfc2	r2,$18; \
					mfc2	r3,$19; \
					nop

#define	read_sz_fifo4(r1,r2,r3,r4) 	mfc2	r1,$16; \
					mfc2	r2,$17; \
					mfc2	r3,$18; \
					mfc2	r4,$19;	\
					nop

#define	read_szx(r1) 			mfc2	r1,$16;	\
					nop

#define	read_sz0(r1)			mfc2	r1,$17;	\
					nop

#define	read_sz1(r1)			mfc2	r1,$18;	\
					nop

#define	read_sz2(r1)			mfc2	r1,$19;	\
					nop

#define	read_sxsy_fifo3(r1,r2,r3) 	mfc2	r1,$12; \
					mfc2	r2,$13; \
					mfc2	r3,$14;	\
					nop

#define	read_sxsy0(r1)			mfc2	r1,$12;	\
					nop

#define	read_sxsy1(r1)			mfc2	r1,$13;	\
					nop

#define	read_sxsy2(r1)			mfc2	r1,$14;	\
					nop

#define	read_rgb_fifo(r1,r2,r3)		mfc2	r1,$20; \
					mfc2	r2,$21; \
					mfc2	r3,$22;	\
					nop

#define	read_rgb0(r1)			mfc2	r1,$20; \
					nop

#define	read_rgb1(r1)			mfc2	r1,$21; \
					nop

#define	read_rgb2(r1)			mfc2	r1,$22; \
					nop

#define	read_flag(r1)			cfc2	r1,$31;	\
					nop

#define	read_p(r1)			mfc2	r1,$8;	\
					nop

#define	read_otz(r1)			mfc2	r1,$7;	\
					nop

#define	read_opz(r1)			mfc2	r1,$24;	\
					nop

#define read_mt(r1,r2,r3)		mfc2	r1,$25; \
					mfc2	r2,$26; \
					mfc2	r3,$27; \
					nop
/*
* GTE set macros
*/
#define set_trans_matrix(r1,r2,r3)	ctc2	r1,$5;	\
					ctc2	r2,$6;	\
					ctc2	r3,$7;	\
					nop
#endif











#ifndef ASSEMBLER
typedef struct  {
	int16_t	m[3][3];	/* 3x3 rotation matrix */
    int32_t    t[3];		/* transfer vector */
} MATRIX;

typedef struct {		/* int32_t word type 3D vector */
	int32_t	vx, vy;
	int32_t	vz, pad;
} VECTOR;

typedef struct {
	int16_t	vx,vy;
	int16_t	vz,pad;
}SVECTOR;

typedef struct {		/* color type vector */
	uint8_t	r, g, b, cd;
} CVECTOR;

typedef struct {		/* 2D int16_t vector */
	int16_t vx, vy;
} DVECTOR;


typedef struct {
	SVECTOR v;		/* Object(Local) 3D Vertex 	*/
	VECTOR sxyz;		/* Screen 3D Vertex		*/
	DVECTOR sxy;		/* Screen 2D Vertex		*/
	CVECTOR rgb;		/* Vertex Color Data	 	*/
	int16_t txuv,pad;		/* Texture Mapping Data 	*/
	int32_t chx,chy;		/* Clip Window Data 		*/
} EVECTOR;

typedef struct {
	SVECTOR v;
	uint8_t uv[2]; uint16_t pad;	/*�@�@*/
	CVECTOR c;
	DVECTOR sxy;
	uint32_t  sz;		/*�@clip z-data�@*/
} RVECTOR;			/*�@�������_���x�N�^�@*/


typedef struct {
	RVECTOR r01,r12,r20;
	RVECTOR	*r0,*r1,*r2;
	uint32_t *rtn;
} CRVECTOR3;			/*�@�R�p�`�p�ċA�x�N�^�@*/

typedef struct {
	uint32_t 	ndiv;		/*�@�������@*/
	uint32_t 	pih,piv;	/*�@�N���b�v�G���A�@*/
	uint16_t clut,tpage;
	CVECTOR	rgbc;
	uint32_t	*ot;
	RVECTOR r0,r1,r2;
	CRVECTOR3 cr[5];
} DIVPOLYGON3;			/*�@�R�p�`�p�����o�b�t�@�@*/

typedef struct {
	RVECTOR r01,r02,r31,r32,rc;
	RVECTOR	*r0,*r1,*r2,*r3;
	uint32_t *rtn;
} CRVECTOR4;			/*�@�S�p�`�p�ċA�x�N�^�@*/

typedef struct {
	uint32_t 	ndiv;		/*�@�������@*/
	uint32_t 	pih,piv;	/*�@�N���b�v�G���A�@*/
	uint16_t clut,tpage;
	CVECTOR	rgbc;
	uint32_t	*ot;
	RVECTOR r0,r1,r2,r3;
	CRVECTOR4 cr[5];
} DIVPOLYGON4;			/*�@�S�p�`�p�����o�b�t�@�@*/

typedef struct {
        int16_t   xy[3];
        int16_t   uv[2];
        int16_t   rgb[3];
} SPOL;

                                        /*polygon: 41 bytes/1 polygon*/
typedef struct {
        int16_t   sxy[4][2];              /*0..7*/
        int16_t   sz[4][2];               /*8..15sz[][1] is dummy*/
        int16_t   uv[4][2];               /*16..23*/
        int16_t   rgb[4][3];              /*23..34*/
        int16_t   code;                   /*35...  F4:5,TF4:6,G4:7,TG4:8*/
} POL4;

typedef struct {
        int16_t   sxy[3][2];
        int16_t   sz[3][2];               /*sz[][1] is dummy*/
        int16_t   uv[3][2];
        int16_t   rgb[3][3];
        int16_t   code;                   /*F3:1,TF3:2,G3:3,TG3:4*/
} POL3;


typedef struct
{
	/* internal GTE color type vector */
	uint32_t	r, g, b, cd;
} GTECVECTOR;


#endif
#include "wipesrc/psx26/include/libgpu.h"

/*
 *      ProtoTypes
 */
#ifndef NO_PROTOTYPE

void InitGeom();

MATRIX *MulMatrix0(MATRIX *m0,MATRIX *m1,MATRIX *m2);
MATRIX *MulRotMatrix0(MATRIX *m0,MATRIX *m1);
MATRIX *MulMatrix(MATRIX *m0,MATRIX *m1);
MATRIX *MulMatrix2(MATRIX *m0,MATRIX *m1);
MATRIX *MulRotMatrix(MATRIX *m0);
MATRIX *SetMulMatrix(MATRIX *m0,MATRIX *m1);

VECTOR *ApplyMatrix(MATRIX *m,SVECTOR *v0,VECTOR *v1);

void DLSRotTransPers(int32_t *sxy, VECTOR *r0, int32_t *flag);


VECTOR *ApplyRotMatrix(SVECTOR *v0,VECTOR *v1);
VECTOR *ApplyMatrixLV(MATRIX *m,VECTOR *v0,VECTOR *v1);
MATRIX *RotMatrix(SVECTOR *r,MATRIX *m);
MATRIX *RotMatrixYXZ(SVECTOR *r,MATRIX *m);
MATRIX *RotMatrixX(int32_t r,MATRIX *m);
MATRIX *RotMatrixY(int32_t r,MATRIX *m);
MATRIX *RotMatrixZ(int32_t r,MATRIX *m);
MATRIX *RotMatrixC(SVECTOR *r,MATRIX *m);
MATRIX *TransMatrix(MATRIX *m,VECTOR *v);
MATRIX *ScaleMatrix(MATRIX *m,VECTOR *v);
MATRIX *ScaleMatrixL(MATRIX *m,VECTOR *v);
MATRIX *TransposeMatrix(MATRIX *m0,MATRIX *m1);
MATRIX *CompMatrix(MATRIX *m0,MATRIX *m1,MATRIX *m2);
void SetRotMatrix(MATRIX *m);
void SetLightMatrix(MATRIX *m);
void SetColorMatrix(MATRIX *m);
void SetTransMatrix(MATRIX *m);
void PushMatrix();
void PopMatrix();
void ReadRotMatrix(MATRIX *m);
void ReadLightMatrix(MATRIX *m);
void ReadColorMatrix(MATRIX *m);
void SetRGBcd(CVECTOR *v);
void SetBackColor(int32_t rbk,int32_t gbk,int32_t bbk);
void SetFarColor(int32_t rfc,int32_t gfc,int32_t bfc);
void SetGeomOffset(int32_t ofx,int32_t ofy);
void SetGeomScreen(int32_t h);
void ReadSZfifo3(int32_t *sz0,int32_t *sz1,int32_t *sz2);
void ReadSZfifo4(int32_t *szx,int32_t *sz0,int32_t *sz1,int32_t *sz2);
void ReadSXSYfifo(int32_t *sxy0,int32_t *sxy1,int32_t *sxy2);
void ReadRGBfifo(CVECTOR *v0,CVECTOR *v1,CVECTOR *v2);
void ReadGeomOffset(int32_t *ofx,int32_t *ofy);
int32_t ReadGeomScreen();

int32_t RotTransPers(SVECTOR *v0, DVECTOR *sxy, int32_t *p, int32_t *flag);
int32_t RotTransPersForN(SVECTOR *v0,int32_t *sxy,int32_t *p,int32_t *flag);
int32_t RotTransPers3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,int32_t *p,int32_t *flag);
void RotTrans(SVECTOR *v0,VECTOR *v1,int32_t *flag);
void LocalLight(SVECTOR *v0,VECTOR *v1);
void LightColor(VECTOR *v0,VECTOR *v1);
void DpqColorLight(VECTOR *v0,CVECTOR *v1,int32_t p,CVECTOR *v2);
void DpqColor(CVECTOR *v0,int32_t p,CVECTOR *v1);
void DpqColor3(CVECTOR *v0,CVECTOR *v1,CVECTOR *v2,
			int32_t p,CVECTOR *v3,CVECTOR *v4,CVECTOR *v5);
void Intpl(VECTOR *v0,int32_t p,CVECTOR *v1);
VECTOR *Square12(VECTOR *v0,VECTOR *v1);
VECTOR *Square0(VECTOR *v0,VECTOR *v1);
void NormalColor(SVECTOR *v0,CVECTOR *v1);
void NormalColor3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			CVECTOR *v3,CVECTOR *v4,CVECTOR *v5);
void NormalColorDpq(SVECTOR *v0,CVECTOR *v1,int32_t p,CVECTOR *v2);
void NormalColorDpq3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,CVECTOR *v3,
			int32_t p,CVECTOR *v4,CVECTOR *v5,CVECTOR *v6);
void NormalColorCol(SVECTOR *v0,CVECTOR *v1,CVECTOR *v2);
void NormalColorCol3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,CVECTOR *v3,
			CVECTOR *v4,CVECTOR *v5,CVECTOR *v6);
void ColorDpq(VECTOR *v0,CVECTOR *v1,int32_t p,CVECTOR *v2);
void ColorCol(VECTOR *v0,CVECTOR *v1,CVECTOR *v2);
int32_t NormalClip(int32_t sxy0,int32_t sxy1,int32_t sxy2);


int32_t AverageZ3(int32_t sz0,int32_t sz1,int32_t sz2);


int32_t AverageZ4(int32_t sz0,int32_t sz1,int32_t sz2,int32_t sz3);


void OuterProduct12(VECTOR *v0,VECTOR *v1,VECTOR *v2);
void OuterProduct0(VECTOR *v0,VECTOR *v1,VECTOR *v2);
int32_t Lzc(int32_t data);


int32_t RotTransPers4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,int32_t *sxy3,
			int32_t *p,int32_t *flag);
void RotTransPersN(SVECTOR *v0,DVECTOR *v1,uint16_t *sz,uint16_t *flag,int32_t n);
int32_t RotAverage3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,int32_t *p,int32_t *flag);
int32_t RotAverage4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,int32_t *sxy3,
			int32_t *p,int32_t *flag);
int32_t RotNclip3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,int32_t *p,int32_t *otz,
			int32_t *flag);
int32_t RotNclip4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,int32_t *sxy3,
			int32_t *p,int32_t *otz,int32_t *flag);
int32_t RotAverageNclip3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,
			int32_t *p,int32_t *otz,int32_t *flag);
int32_t RotAverageNclip4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,int32_t *sxy3,
			int32_t *p,int32_t *otz,int32_t *flag);
int32_t RotColorDpq(SVECTOR *v0,SVECTOR *v1,CVECTOR *v2,
			int32_t *sxy,CVECTOR *v3,int32_t *flag);
int32_t RotColorDpq3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,
			CVECTOR *v7,CVECTOR *v8,CVECTOR *v9,int32_t *flag);
int32_t RotAverageNclipColorDpq3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,
			CVECTOR *v7,CVECTOR *v8,CVECTOR *v9,
			int32_t *otz,int32_t *flag);
int32_t RotAverageNclipColorCol3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6,
			int32_t *sxy0,int32_t *sxy1,int32_t *sxy2,CVECTOR *v7,CVECTOR *v8,CVECTOR *v9,
			int32_t *otz,int32_t *flag);
int32_t RotColorMatDpq(SVECTOR *v0,SVECTOR *v1,CVECTOR *v2,int32_t *sxy,CVECTOR *v3,int32_t matc,int32_t *flag);
void ColorMatDpq(SVECTOR *v0,CVECTOR *v1,int32_t p,CVECTOR *v2,int32_t matc);
void ColorMatCol(SVECTOR *v0,CVECTOR *v1,CVECTOR *v2,int32_t matc);
void InterpolByte(uint8_t *in1, uint8_t *in2, int32_t p0, uint8_t *rtn);
void InterpolShort(SVECTOR *vi0, SVECTOR *vi1, int32_t p0, SVECTOR *vt1);
void LoadAverage12(VECTOR *v0,VECTOR *v1,int32_t p0,int32_t p1,VECTOR *v2);
void LoadAverageShort12(SVECTOR *v0,SVECTOR *v1,int32_t p0,int32_t p1,SVECTOR *v2);
void LoadAverage0(VECTOR *v0,VECTOR *v1,int32_t p0,int32_t p1,VECTOR *v2);
void LoadAverageShort0(SVECTOR *v0,SVECTOR *v1,int32_t p0,int32_t p1,SVECTOR *v2);
void LoadAverageByte(uint8_t *v0,uint8_t *v1,int32_t p0,int32_t p1,uint8_t *v2);
void LoadAverageCol(uint8_t *v0,uint8_t *v1,int32_t p0,int32_t p1,uint8_t *v2);
void VectorNormal(VECTOR *v0, VECTOR *v1);
void VectorNormalS(VECTOR *v0, SVECTOR *v1);
int32_t SquareRoot0(int32_t a);
int32_t SquareRoot12(int32_t a);
void InvSquareRoot(int32_t a, int32_t *b, int32_t *c);
void gteMIMefunc(SVECTOR *otp, SVECTOR *dfp, int32_t n, int32_t p);
void SetFogFar(int32_t a,int32_t h);
void SetFogNear(int32_t a,int32_t h);
void SubPol4(POL4 *p, SPOL *sp, int32_t ndiv);
void SubPol3(POL3 *p, SPOL *sp, int32_t ndiv);

int32_t slow_rsin(int32_t a);
int32_t slow_rcos(int32_t a);
int32_t rcos(int32_t a);
int32_t rsin(int32_t a);
int32_t ccos(int32_t a);
int32_t csin(int32_t a);
int32_t csqrt(int32_t a);
int32_t cln(int32_t a);
int32_t catan(int32_t a);
int32_t ratan2(int32_t y, int32_t x);


void RotPMD_F3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_G3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_FT3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_GT3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_F4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_G4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_FT4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_GT4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);

void RotPMD_SV_F3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_SV_G3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_SV_FT3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_SV_GT3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_SV_F4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_SV_G4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_SV_FT4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);
void RotPMD_SV_GT4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id,int32_t backc);


void InitClip(EVECTOR *evbfad,int32_t hw,int32_t vw,int32_t h,int32_t Near,int32_t Far);
int32_t Clip3F(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,EVECTOR **evmx);
int32_t Clip3FP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,EVECTOR **evmx);
int32_t Clip4F(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,EVECTOR **evmx);
int32_t Clip4FP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,EVECTOR **evmx);
int32_t Clip3FT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,EVECTOR **evmx);
int32_t Clip3FTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,EVECTOR **evmx);
int32_t Clip4FT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,int16_t *uv3,EVECTOR **evmx);
int32_t Clip4FTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,int16_t *uv3,EVECTOR **evmx);
int32_t Clip3G(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,EVECTOR **evmx);
int32_t Clip3GP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,EVECTOR **evmx);
int32_t Clip4G(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);
int32_t Clip4GP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);
int32_t Clip3GT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,
		EVECTOR **evmx);
int32_t Clip3GTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,
		EVECTOR **evmx);
int32_t Clip4GT(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,int16_t *uv3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);
int32_t Clip4GTP(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		int16_t *uv0,int16_t *uv1,int16_t *uv2,int16_t *uv3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		EVECTOR **evmx);

void RotTransPers_nom(SVECTOR *v0);
void RotTransPers3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
void RotTransPers4_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3);
void RotTrans_nom(SVECTOR *v0);
void RotAverage3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
void RotNclip3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
void RotAverageNclip3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
void RotAverageNclipColorDpq3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6);
void RotAverageNclipColorCol3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6);
void RotColorDpq_nom(SVECTOR *v0,SVECTOR *v1,CVECTOR *v2);
int32_t RotColorDpq3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			SVECTOR *v3,SVECTOR *v4,SVECTOR *v5,CVECTOR *v6);
void NormalColor_nom(SVECTOR *v0);
void NormalColor3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2);
void NormalColorDpq_nom(SVECTOR *v0,CVECTOR *v1,int32_t p);
void NormalColorDpq3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
			CVECTOR *v3,int32_t p);
void NormalColorCol_nom(SVECTOR *v0,CVECTOR *v1);
void NormalColorCol3_nom(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,CVECTOR *v3);

uint32_t *DivideF3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,CVECTOR *rgbc,
		POLY_F3 *s,uint32_t *ot,DIVPOLYGON3 *divp);

uint32_t *DivideF4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		CVECTOR *rgbc,POLY_F4 *s,uint32_t *ot,DIVPOLYGON4 *divp);
uint32_t *DivideFT3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		uint32_t *uv0,uint32_t *uv1,uint32_t *uv2,CVECTOR *rgbc,
		POLY_FT3 *s,uint32_t *ot,DIVPOLYGON3 *divp);
uint32_t *DivideFT4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		uint32_t *uv0,uint32_t *uv1,uint32_t *uv2,uint32_t *uv3,
		CVECTOR *rgbc,POLY_FT4 *s,uint32_t *ot,DIVPOLYGON4 *divp);
uint32_t *DivideG3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,
		POLY_G3 *s,uint32_t *ot,DIVPOLYGON3 *divp);
uint32_t *DivideG4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		POLY_G4 *s,uint32_t *ot,DIVPOLYGON4 *divp);
uint32_t *DivideGT3(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,
		uint32_t *uv0,uint32_t *uv1,uint32_t *uv2,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,
		POLY_GT3 *s,uint32_t *ot,DIVPOLYGON3 *divp);
uint32_t *DivideGT4(SVECTOR *v0,SVECTOR *v1,SVECTOR *v2,SVECTOR *v3,
		uint32_t *uv0,uint32_t *uv1,uint32_t *uv2,uint32_t *uv3,
		CVECTOR *rgb0,CVECTOR *rgb1,CVECTOR *rgb2,CVECTOR *rgb3,
		POLY_GT4 *s,uint32_t *ot,DIVPOLYGON4 *divp);

uint32_t *RCpolyF3(POLY_F3 *s,DIVPOLYGON3 *divp);
uint32_t *RCpolyF4(POLY_F4 *s,DIVPOLYGON4 *divp);
uint32_t *RCpolyFT3(POLY_FT3 *s,DIVPOLYGON3 *divp);
uint32_t *RCpolyFT4(POLY_FT4 *s,DIVPOLYGON4 *divp);
uint32_t *RCpolyG3(POLY_G3 *s,DIVPOLYGON3 *divp);
uint32_t *RCpolyG4(POLY_G4 *s,DIVPOLYGON4 *divp);
uint32_t *RCpolyGT3(POLY_GT3 *s,DIVPOLYGON3 *divp);
uint32_t *RCpolyGT4(POLY_GT4 *s,DIVPOLYGON4 *divp);

/*
uint32_t *GsTMDfastF3L(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastF3LFG(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastF3NL(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastNF3(TMD_P_NF3 *primtop,SVECTOR *vertop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDfastF4L(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastF4LFG(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastF4NL(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastNF4(TMD_P_NF4 *primtop,SVECTOR *vertop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDfastTF3L(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTF3LFG(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTF3NL(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTNF3(TMD_P_TNF3 *primtop,SVECTOR *vertop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDfastTF4L(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTF4LFG(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTF4NL(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTNF4(TMD_P_TNF4 *primtop,SVECTOR *vertop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDfastG3L(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastG3LFG(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastG3NL(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastNG3(TMD_P_NG3 *primtop,SVECTOR *vertop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDfastG4L(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastG4LFG(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastG4NL(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastNG4(TMD_P_NG4 *primtop,SVECTOR *vertop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDfastTG3L(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTG3LFG(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTG3NL(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTNG3(TMD_P_TNG3 *primtop,SVECTOR *vertop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDfastTG4L(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTG4LFG(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTG4NL(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsTMDfastTNG4(TMD_P_TNG4 *primtop,SVECTOR *vertop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstF3L(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstF3LFG(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstF3NL(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstNF3(TMD_P_NF3 *primtop,SVECTOR *vertop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstF4L(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstF4LFG(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstF4NL(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstNF4(TMD_P_NF4 *primtop,SVECTOR *vertop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstTF3L(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTF3LFG(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTF3NL(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTNF3(TMD_P_TNF3 *primtop,SVECTOR *vertop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstTF4L(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTF4LFG(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTF4NL(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTNF4(TMD_P_TNF4 *primtop,SVECTOR *vertop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstG3L(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstG3LFG(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstG3NL(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstNG3(TMD_P_NG3 *primtop,SVECTOR *vertop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstG4L(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstG4LFG(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstG4NL(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstNG4(TMD_P_NG4 *primtop,SVECTOR *vertop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstTG3L(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTG3LFG(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTG3NL(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTNG3(TMD_P_TNG3 *primtop,SVECTOR *vertop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsPrstTG4L(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTG4LFG(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTG4NL(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);
uint32_t *GsPrstTNG4(TMD_P_TNG4 *primtop,SVECTOR *vertop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp);

uint32_t *GsTMDdivF3L(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivF3LFG(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivF3NL(TMD_P_F3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivNF3(TMD_P_NF3 *primtop,SVECTOR *vertop,
		POLY_F3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);

uint32_t *GsTMDdivF4L(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivF4LFG(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivF4NL(TMD_P_F4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivNF4(TMD_P_NF4 *primtop,SVECTOR *vertop,
		POLY_F4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);

uint32_t *GsTMDdivTF3L(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivTF3LFG(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivTF3NL(TMD_P_TF3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivTNF3(TMD_P_TNF3 *primtop,SVECTOR *vertop,
		POLY_FT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);

uint32_t *GsTMDdivTF4L(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivTF4LFG(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivTF4NL(TMD_P_TF4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivTNF4(TMD_P_TNF4 *primtop,SVECTOR *vertop,
		POLY_FT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);

uint32_t *GsTMDdivG3L(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivG3LFG(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivG3NL(TMD_P_G3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivNG3(TMD_P_NG3 *primtop,SVECTOR *vertop,
		POLY_G3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);

uint32_t *GsTMDdivG4L(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivG4LFG(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivG4NL(TMD_P_G4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivNG4(TMD_P_NG4 *primtop,SVECTOR *vertop,
		POLY_G4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);

uint32_t *GsTMDdivTG3L(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivTG3LFG(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivTG3NL(TMD_P_TG3 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);
uint32_t *GsTMDdivTNG3(TMD_P_TNG3 *primtop,SVECTOR *vertop,
		POLY_GT3 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON3 *divp);

uint32_t *GsTMDdivTG4L(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivTG4LFG(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivTG4NL(TMD_P_TG4 *primtop,SVECTOR *vertop,SVECTOR *nortop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);
uint32_t *GsTMDdivTNG4(TMD_P_TNG4 *primtop,SVECTOR *vertop,
		POLY_GT4 *s,uint32_t n,uint32_t shift,GsOT *otp,DIVPOLYGON4 *divp);

*/

void RotSMD_F3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_G3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_FT3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_GT3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_F4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_G4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_FT4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_GT4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);

void RotSMD_SV_F3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_SV_G3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_SV_FT3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_SV_GT3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_SV_F4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_SV_G4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_SV_FT4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotSMD_SV_GT4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);



void RotRMD_F3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_G3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_FT3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_GT3(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_F4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_G4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_FT4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_GT4(int32_t *pa,uint32_t *ot,int32_t otlen,int32_t id);

void RotRMD_SV_F3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_SV_G3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_SV_FT3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_SV_GT3(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_SV_F4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_SV_G4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_SV_FT4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);
void RotRMD_SV_GT4(int32_t *pa,int32_t *va,uint32_t *ot,int32_t otlen,int32_t id);

int32_t NormalClip(DVECTOR sxy0, DVECTOR sxy1, DVECTOR sxy2);
void SetDrawDistance(int32_t Dist);
void CreateSineTable();

#endif
#endif
