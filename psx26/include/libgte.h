/*
 *  (C) Copyright 1993/1994 Sony Computer Entertainment ,Tokyo,Japan.
 *				All Rights Reserved
 *
 * 		libgte.h: Geometry Basic Structures Database
 *
 *$Id: libgte.h,v 1.8 1994/10/17 07:31:22 ohba Exp $
 */

/*
 * Geometry Structures:
 */
#if !defined(_LIBGTE_)

#define _LIBGTE_

#define	ONE		4096	/* GTE regards 4096 as 1.0 */
#define TMPVTXNUM	16	/* Clip Function Header */
#define OUTVTXNUM	10

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
	short	m[3][3];	/* 3x3 rotation matrix */
        long    t[3];		/* transfer vector */
} MATRIX;

typedef struct {		/* long word type 3D vector */
	long	vx, vy;
	long	vz, pad;
} VECTOR;
	
typedef struct {		/* short word type 3D vector */	
	short	vx, vy;
	short	vz, pad;
} SVECTOR;
	       
typedef struct {		/* color type vector */	
	u_char	r, g, b, cd;
} CVECTOR;
	       
typedef struct {		/* 2D short vector */
	short vx, vy;
} DVECTOR;


typedef struct {		
	SVECTOR v;		/* Object(Local) 3D Vertex 	*/
	VECTOR sxyz;		/* Screen 3D Vertex		*/
	DVECTOR sxy;		/* Screen 2D Vertex		*/
	CVECTOR rgb;		/* Vertex Color Data	 	*/
	short txuv,pad;		/* Texture Mapping Data 	*/
	long chx,chy;		/* Clip Window Data 		*/
} EVECTOR;

typedef struct {
	SVECTOR v;
	u_char uv[2]; u_short pad;	/*　　*/  
	CVECTOR c;
	DVECTOR sxy;		
	u_long  sz;		/*　clip z-data　*/		
} RVECTOR;			/*　分割頂点情報ベクタ　*/


typedef struct {
	RVECTOR r01,r12,r20;
	RVECTOR	*r0,*r1,*r2;
	u_long *rtn;
} CRVECTOR3;			/*　３角形用再帰ベクタ　*/

typedef struct {
	u_long 	ndiv;		/*　分割数　*/
	u_long 	pih,piv;	/*　クリップエリア　*/
	u_short clut,tpage;
	CVECTOR	rgbc;
	u_long	*ot;
	RVECTOR r0,r1,r2;
	CRVECTOR3 cr[5];	
} DIVPOLYGON3;			/*　３角形用分割バッファ　*/

typedef struct {
	RVECTOR r01,r02,r31,r32,rc;
	RVECTOR	*r0,*r1,*r2,*r3;
	u_long *rtn;
} CRVECTOR4;			/*　４角形用再帰ベクタ　*/

typedef struct {
	u_long 	ndiv;		/*　分割数　*/
	u_long 	pih,piv;	/*　クリップエリア　*/
	u_short clut,tpage;
	CVECTOR	rgbc;
	u_long	*ot;
	RVECTOR r0,r1,r2,r3;
	CRVECTOR4 cr[5];	
} DIVPOLYGON4;			/*　４角形用分割バッファ　*/

#endif
#endif
