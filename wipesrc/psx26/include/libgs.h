/*
 *	libgs.h: Graphic Library Header
 *
 *
 *		Version 1.**	Apr,  8, 1994
 *
 *		Copyright (C) 1993 by Sony Corporation
 *			All rights Reserved
 */

#ifndef NULL
#define NULL 0
#endif

/*** packet peripheral pointer ***/
typedef unsigned char   PACKET;

#define PSBANK 0x80000000
/*** --- Zsort resolution --- ***/
#define ZRESOLUTION     0x3fff

/*** --- coordinate keyword - ***/
#define WORLD NULL
#define SCREEN ((GsCOORDINATE *)0x0001)
#define SCREEN2 ((GsCOORDINATE2 *)0x0001)



typedef struct _GsCOORDINATE {
	long rx,ry,rz;
	short sx,sy,sz;
	long  tx,ty,tz;
	struct _GsCOORDINATE  *super;
	struct _GsCOORDINATE  *sub;
	} GsCOORDINATE;

typedef struct {	
	VECTOR scale;
	SVECTOR rotate;
	VECTOR trans;
	} GsCOORD2PARAM;

typedef struct _GsCOORDINATE2 {
        unsigned long flg;
        MATRIX coord;
	MATRIX workm;
	GsCOORD2PARAM  *param;
	struct _GsCOORDINATE2  *super;
	struct _GsCOORDINATE2  *sub;
	} GsCOORDINATE2;

typedef struct {
        long rx,ry,rz;
	short sx,sy,sz;
	long tx,ty,tz;
	GsCOORDINATE *super;
        } GsVIEW;

typedef struct {
        MATRIX view;
	GsCOORDINATE2 *super;
        } GsVIEW2;

typedef struct {
        long vpx,vpy,vpz;
	long vrx,vry,vrz;
	long rz;
	GsCOORDINATE *super;
	} GsRVIEW;

typedef struct {
        long vpx,vpy,vpz;
	long vrx,vry,vrz;
	long rz;
	GsCOORDINATE2 *super;
	} GsRVIEW2;

typedef struct {
  int vx,vy,vz;
	unsigned char r,g,b;
	} GsF_LIGHT;


typedef struct {
        unsigned p : 24;
        unsigned char num : 8;
	} GsOT_TAG;


typedef struct {
	unsigned long length;
	GsOT_TAG *org;
	unsigned long offset;
	unsigned long point;
	GsOT_TAG *tag;
	} GsOT;

/*** --- object structure --- ***/
typedef struct  {
        unsigned long attribute;	/* pers,trans,rotate,disp */
        MATRIX  lwmatrix;		/* local-world  matrix */
	MATRIX  lightm;
	GsCOORDINATE    coord;		/* local dmatrix */
	unsigned long  *tmd;
	unsigned long  id;
        } GsDOBJ;


typedef struct  {
        unsigned long attribute;	/* pers,trans,rotate,disp */
	GsCOORDINATE2  *coord2;	/* local dmatrix */
	unsigned long  *tmd;
	unsigned long  id;
       } GsDOBJ2;

typedef struct  {
        unsigned long attribute;	/* pers,trans,rotate,disp */
	GsCOORDINATE2  *coord2;	/* local dmatrix */
	unsigned long  *pmd;	/* pmd top address */
	unsigned long  *base;	/* object base address */
	unsigned long  *sv;	/* shared vertex base */
	unsigned long  id;
       } GsDOBJ3;

typedef struct  {
  unsigned long attribute;
  GsCOORDINATE2 *coord2;
  unsigned long *tmd;
  unsigned long *packet;
  unsigned long id;
} GsDOBJ5;

typedef struct {
	unsigned long attribute;
	short x,y;
	unsigned short w,h;
	unsigned short tpage;
	unsigned char u,v;
	short cx,cy;
	unsigned char r,g,b;
	short mx,my;
	short scalex,scaley;
	long rotate;
} GsSPRITE;

typedef struct {
	unsigned long attribute;
	short x,y;
	DR_MODE mode[2];	/* Draw mode primitive */
	SPRT packet[2];		/* Sprite primitive */
} GsSPARRAY;

typedef struct {
	unsigned char u, v;
	unsigned short cba;
	unsigned short flag;
	unsigned short tpage;
	} GsCELL;

typedef struct {
	unsigned char cellw,cellh;
	unsigned short ncellw,ncellh;
	GsCELL *base;
	unsigned short *index;
	} GsMAP;

typedef struct {
	unsigned long attribute;
	short x,y;
	short w,h;
	short scrollx,scrolly;
	unsigned char r,g,b;
	GsMAP *map;
	short mx,my;
	short scalex,scaley;
	long rotate;
	} GsBG;

typedef struct {
	unsigned long attribute;
	short x0,y0;
	short x1,y1;
	unsigned char r,g,b;
        } GsLINE;

typedef struct {
	unsigned long attribute;
	short x0,y0;
	short x1,y1;
	unsigned char r0,g0,b0;
	unsigned char r1,g1,b1;
        } GsGLINE;

typedef struct {
	unsigned long attribute;
	short x,y;
	unsigned short w,h;
	unsigned char r,g,b;
        } GsBOXF;

typedef struct {
        short dqa;
        long  dqb;
        unsigned char rfc,gfc,bfc;
        } GsFOGPARAM;


typedef struct {
  unsigned long pmode;
  short px,py;
  unsigned short pw,ph;
  unsigned long *pixel;
  short cx,cy;
  unsigned short cw,ch;
  unsigned long *clut;
} GsIMAGE;

typedef struct {
  short offx,offy;
} _GsPOSITION;

typedef struct {
  unsigned long farz;
  unsigned long nearz;
} GsZCLIP;

/* Object Management Table */
typedef struct {
	GsDOBJ *top;
	int nobj;
	int maxobj;
} GsOBJTABLE;

typedef struct {
	GsDOBJ2 *top;
	int nobj;
	int maxobj;
} GsOBJTABLE2;

typedef struct 
{
  SVECTOR  **diff_sp_vertex;
  unsigned long   *diff_off_vertex;
  unsigned long   *diff_n_vertex;
  unsigned long   *diff_obj_vertex;
  long     *mimepr;
} GsMIMEV;

typedef struct
{
  SVECTOR  **diff_sp_norm;
  unsigned long   *diff_off_norm;
  unsigned long   *diff_n_norm;
  unsigned long   *diff_obj_norm;
  long     *mimepr;
} GsMIMEN;

/*
 * libgs macro
 */
#define GsOFSGTE 0
#define GsOFSGPU 4
#define GsINTER  1
#define GsNONINTER 0

/*
 * object attribute set macro
 */
#define GsLDIM0 0
#define GsLDIM1 1
#define GsLDIM2 2
#define GsLDIM3 3
#define GsLDIM4 4
#define GsLDIM5 5
#define GsLDIM6 6
#define GsLDIM7 7
#define GsFOG   (1<<3)
#define GsMATE  (1<<4)
#define GsLLMOD (1<<5)
#define GsLOFF  (1<<6)
#define GsZIGNR (1<<7)
#define GsNBACKC (1<<8)
#define GsDIV1   (1<<9)
#define GsDIV2   (2<<9)
#define GsDIV3   (3<<9)
#define GsDIV4	 (4<<9)
#define GsDIV5	 (5<<9)
#define GsAZERO  (0<<28)
#define GsAONE   (1<<28)
#define GsATWO   (2<<28)
#define GsATHREE (3<<28)
#define GsALON   (1<<30)
#define GsDOFF   (1<<31)
/*
 * BG/sprite attribute set macro
 */
#define GsPERS   (1<<26)
#define GsROTOFF (1<<27)

/*
 * TOD animation 
 */
#define GsTOD_ATTR (0)
#define GsTOD_COORD (1)
#define GsTOD_TMDID (2)
#define GsTOD_PARENT (3)
#define GsTOD_MATRIX (4)
#define GsTOD_TMDDATA (5)
#define GsTOD_LIGHT (6)
#define GsTOD_CAMERA (7)
#define GsTOD_OBJCTL (8)
#define GsTOD_USER0 (9)
#define GsTOD_USER1 (10)
#define GsTOD_USER2 (11)
#define GsTOD_USER3 (12)
#define GsTOD_USER4 (13)
#define GsTOD_SYSFUNC (15)

#define GsTOD_CREATE (0)
#define GsTOD_NOCREATE (1)
#define GsTOD_COORDONLY (2)

#define GsOBJ_UNDEF (0xffffffff)

/*
 * PROTOTYPE DIFINITIONS
 */
void GsInitGraph(unsigned short x,unsigned short y,unsigned short intmode,
	       unsigned short dith,unsigned short varmmode);
void GsInit_3D(void);
void GsMapModelingData(unsigned long *p);

void GsSetProjection(long h);
int  GsSetRefView(GsRVIEW *pv);
void GsSetFlatLight(int id,GsF_LIGHT *lt);
void GsSetLihtMode(int mode);
void GsSetFogParam(GsFOGPARAM *fogparm);
void GsSetAmbient(long r,long g,long b);
void GsInitCoordinate(GsCOORDINATE *super,GsCOORDINATE *base);
void GsGetObjLw(GsDOBJ *objp);
void GsGetObjLight(GsDOBJ *objp);
void GsGetObjMatrix(GsDOBJ *objp);
void GsDrawOt(GsOT *ot);
void GsSetWorkBase(PACKET *outpacketp);

void GsSortObject(GsDOBJ *objp,GsOT *ot,int shift);
void GsSortObject2(GsDOBJ2 *objp,GsOT *ot,int shift);
void GsSortObject3(GsDOBJ3 *objp,GsOT *ot,int shift);
void GsSortObject4(GsDOBJ2 *objp,GsOT *ot,int shift,u_long *scratch);
void GsSortObject5(GsDOBJ5 *objp,GsOT *ot,int shift,u_long *scratch);

void GsSortSprite(GsSPRITE *sp,GsOT *ot,unsigned short pri);
void GsSortSpriteA(GsSPRITE *sp,GsOT *ot,unsigned short pri);
void GsSortFastSprite(GsSPRITE *sp,GsOT *ot,unsigned short pri);
void GsSortFastSpriteA(GsSPRITE *sp,GsOT *ot,unsigned short pri);
void GsSortFlipSprite(GsSPRITE *sp,GsOT *ot,unsigned short pri);
void GsInitFastSpriteArray(GsSPARRAY *sp, short n);
void GsSetFastSpriteArray(GsSPARRAY *sp,unsigned short w, unsigned short h,
			unsigned short tpage, unsigned char u, unsigned char v,
			unsigned short cx, unsigned short cy,
			unsigned char r, unsigned char g, unsigned char b);
void GsSortFastSpriteArray(GsSPARRAY *sp,unsigned short n,
			GsOT *ot,unsigned short pri);
void GsSortBg(GsBG *bg,GsOT *ot,unsigned short pri);
void GsSortFastBg(GsBG *bg,GsOT *ot,unsigned short pri);
void GsInitFixBg16(GsBG *bg,u_long *work);
void GsSortFixBg16(GsBG *bg,u_long *work,GsOT *otp,unsigned short pri);
void GsInitFixBg32(GsBG *bg,u_long *work);
void GsSortFixBg32(GsBG *bg,u_long *work,GsOT *otp,unsigned short pri);
void GsSortLine(GsLINE *lp,GsOT *ot,unsigned short pri);
void GsSortGLine(GsGLINE *lp,GsOT *ot,unsigned short pri);
void GsSortBoxFill(GsBOXF *bp,GsOT *ot,unsigned short pri);
void GsSortPoly(void *pp,GsOT *ot,unsigned short pri);

void GsClearOt(unsigned short offset,unsigned short point,GsOT *otp);
GsOT *GsSortOt(GsOT *ot_src,GsOT *ot_dest);
GsOT *GsCutOt(GsOT *ot_src,GsOT *ot_dest);
void GsDefDispBuff(unsigned short x0,unsigned short y0,unsigned short x1,unsigned short y1);
void GsClearDispBuff(unsigned char ,unsigned char ,unsigned char);
void GsSortClear(unsigned char ,unsigned char ,unsigned char ,GsOT *);
void GsGetTimInfo(unsigned long *im,GsIMAGE *tim);
void GsSwapDispBuff(void);
int GsGetActiveBuff(void);
void GsSetDrawBuffClip(void);
void GsSetDrawBuffOffset(void);
void GsSetClip(RECT *clip);
void GsSetOffset(long x,long y);
void GsSetNearClip(long);
void GsSetFarClip(long);
int GsInitObjTable(GsOBJTABLE *, GsDOBJ *, int);
GsDOBJ *GsSearchObjByID(GsOBJTABLE *, unsigned long);
GsDOBJ *GsCreateNewObj(GsOBJTABLE *, unsigned long);
GsDOBJ *GsRemoveObj(GsOBJTABLE *, unsigned long);
int GsInitObjTable2(GsOBJTABLE2 *, GsDOBJ2 *, GsCOORDINATE2 *, GsCOORD2PARAM *, int);
GsDOBJ2 *GsSearchObjByID2(GsOBJTABLE2 *, unsigned long);
GsDOBJ2 *GsCreateNewObj2(GsOBJTABLE2 *, unsigned long);
GsDOBJ2 *GsRemoveObj2(GsOBJTABLE2 *, unsigned long);
unsigned long *GsSearchTMDByID(unsigned long *, int *, int);

void gte_mul_matrix(MATRIX *m0,MATRIX *m1);
void gte_mul_matrix2(MATRIX *m0,MATRIX *m1);
void gte_apply_matrix(MATRIX *m,short x,short y,short z,short *xo,short *yo,short *zo);
void gte_apply_matrixl(MATRIX *m,long x,long y,long z,long *xo,long *yo,long *zo);
void gte_rotate_x_matrix(MATRIX *m,long rx);
void gte_rotate_y_matrix(MATRIX *m,long ry);
void gte_rotate_z_matrix(MATRIX *m,long rz);
void gte_trans_matrix(MATRIX *m,long tx,long ty,long tz);
int gte_scale_matrix(MATRIX *m,short sx,short sy,short sz);
void gte_rev_matrix(MATRIX *m0,MATRIX *m1);
void gte_set_lc(MATRIX *m1);
void gte_read_lc(MATRIX *m1);
void GsInitCoordinate2(GsCOORDINATE2 *super,GsCOORDINATE2 *base);
void GsInitCoord2param(GsCOORD2PARAM *);
void GsMulCoord2(MATRIX *m1,MATRIX *m2);
void GsMulCoord3(MATRIX *m1,MATRIX *m2);
void GsGetLw(GsCOORDINATE2 *m,MATRIX *out);
void GsGetLs(GsCOORDINATE2 *m,MATRIX *out);

void GsLinkObject(unsigned long tmd,GsDOBJ *obj_base,int n);
void GsLinkObject2(unsigned long tmd_base,GsDOBJ2 *objp,int n);
u_long GsLinkObject3(unsigned long pmd_base,GsDOBJ3 *objp);
void GsLinkObject5(unsigned long tmd_base,GsDOBJ5 *objp,int n);

void GsSetLightMatrix(MATRIX *mp);
int GsSetRefView2(GsRVIEW2 *pv);
int    GsSetView2(GsVIEW2 *pv);
void GsSetLwMatrix(MATRIX *mp);
void GsSetClip2D(RECT *rectp);
void GsInitVcount();
long GsGetVcount();
void GsClearVcount();
unsigned long *GsSetTodFrame();
unsigned long *GsSetTodPacket();
unsigned long *GsSetTodFrame2(int,unsigned long *,GsOBJTABLE2 *,int *,unsigned long *,int);
unsigned long *GsSetTodPacket2(unsigned long *,GsOBJTABLE2 *,int *,unsigned long *,int);
void GsClearDispArea(unsigned char r,unsigned char g,unsigned char b);

u_long *GsPresetObject(GsDOBJ5 *objp,u_long *base_addr);
