/*
 *          'ds.h'
 *
 *          PSX 3d engine main include file
 *
 *          22 July 1994
 *          Marcus Goodey
 *          Psygnosis Liverpool
 *
 *				2/11/94	mhg	x1,y1,x2,y2 added to shape
 * 			2/2/95	mhg	cmask var3 var4 added to OBJECT
 **/

#include <sys\types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>
#include <libetc.h>    

typedef unsigned char byte;
typedef short int word;
typedef unsigned long ulong;


/*---------------------------------------

   DEFINES

----------------------------------------                                    */

#define  MAX_POINTS    	2000  					/* maximum points in one shape */
#define  MAX_OBJECTS  	400					 	/* maximum actors in the world at one time */
#define  MAX_SHAPES    	1000						/* maximum shapes (tmds) that can be loaded */
#define	MAX_TEXTURES	600						/* maximum tims that can be loaded */


#define  ZSHIFT        4
#define  MAX_DEPTH     (1<<(16-ZSHIFT))-1
#define  KAY           1024
#define  MEG           1024*1024
#define  MARCUS        0
#define  TRUE          1
#define  FALSE         0
#define  MAXDIST       120000
#define  NEAR_Z        100
#undef   ONE
#define  ONE           32767
#define  INF           32767
#define  INF32         0x7fffffff
#define  frac(x,y)     (((x)*(y))>>15)

/*--------------------------------------------------------------------------

   STRUCTURES

--------------------------------------------------------------------------  */

typedef struct
{
   ulong *shape;
   int 	extent;
   int 	height;
   int 	root;
	int	extent2;

	int	x1;
	int	y1;
	int	x2;
	int	y2;
	int	*outline;

} DsSHAPE;

typedef struct
{
   int 	class;

   int 	xft;
   int 	yft;
   int 	zft;
   int 	hdg;
   int 	pitch;
   int 	roll;

   void	(*vizsub)();
   int	(*drawsub)();
   void	(*ctrlsub)();
	void	(*collide)();
	void 	(*death)();

   int 	link;
   ulong *shape;                       /* pointer to the tmd                */
   int 	attribute;                      /* affects how the object is rendered*/
   int 	vid;                            /* visual IDentity                   */
   int 	xpos;
   int 	ypos;
   int 	zpos;
   int 	depth;
   int 	objmatrix[3][3];

   int 	xvel;
   int 	yvel;
   int	zvel;

   int 	xacc;
   int 	yacc;
   int 	zacc;

   int 	lastxft;
   int 	lastyft;
   int 	lastzft;
   int 	trigger;
	int	cmask;
   int 	var1;
   int 	var2;
	int	var3;
	int	var4;
	int	lxcrt;
	int	lycrt;

} DsOBJECT;

typedef struct
{
   int x;
   int y;
   int z;

} DsCOORD;

typedef struct
{
   int xeye;
   int yeye;
   int zeye;
   short xcrt;
   short ycrt;
   char r;
   char g;
   char b;
   char cpad;
   int outc;
   char u;
   char v;
   char pad1;
   char pad2;
   int pad3;
} DsPOINT;

typedef struct
{
   int m[3][3];
} MAT;

typedef struct
{
   int xft;
   int yft;
   int zft;
   int hdg;
   int pitch;
   int roll;
} VIEW;

typedef struct
{
   int xft;
   int yft;
   int zft;
   int xv;
   int yv;
   int zv;
   int roll;
} RVIEW;

typedef struct {
short	tx;
short	ty;
short	cx;
short	cy;
short	w;
short	h;
ulong	pmode;
} DsTEXT;



typedef struct
{
   int xft;
   int yft;
   int zft;
   int x1;
   int y1;
   int z1;
   int roll;
} PVIEW;

/*
 * PAD I/O (tentative)
 */


#define  fire1         PADi 
#define  fire2         PADj 
#define  left          PADRleft 
#define  right         PADRright 
#define  up            PADRup 
#define  down          PADRdown 
#define  left1         PADLleft 
#define  right1        PADLright
#define  up1           PADLup 
#define  down1         PADLdown 
#define  quit          PADk 
#define  p_select      PADk 


#define  bot0          	PADm 
#define  top0          	PADl 
#define  top1          	PADn 
#define  bot1          	PADo 

#define	p_rs_1		top0
#define	p_rs_2		bot0

#define	p_ls_1		top1
#define	p_ls_2		bot1
			
#define	p_start	 	 	PADh
#define  p_tri         0x10
#define  p_sq          0x80
#define  p_o           0x20
#define  p_x           0x40
#define  bit(a,b)      ((a&b)!=0)



typedef struct
{
   short r[3][3];                      /* 3x3 rotation matrix               */
   long t[3];                          /* transfer vector                   */
} SMATRIX;

#define  Ds_FOG          1               /* turns fog on                      */
#define  Ds_BACKC        2               /* removes back facing faces         */
#define  Ds_NOLIGHT      4
#define  Ds_FLOAT        8               /* gives an object 3 axis of rotation*/
#define  Ds_ZCLIP        0x10
#define  Ds_CHILD        0x20
#define  Ds_INVISIBLE    0x40 
#define  Ds_MESH         0x80            /* enables meshing                   */
#define  Ds_CLIP         0x100           /* enables clipping                  */
#define  Ds_BIG          0x200           /* removes extent based object      */   
#define  Ds_DEPTH        0x400           /* sets the object depth to a fixed */   
#define  Ds_REJECT       0x1000 
#define	Ds_TRANSPARENT  0x2000						
#define	Ds_SCALE			 0x4000			/* scales a large object down */
#define	Ds_VERYBIG		 0x8000		



#define  rot3          \
 zdeep0=RotTransPers3(v0,v1,v2,&sxy0,&sxy1,&sxy2,&que,&flag);               \
                       if (ZREJECT && ((flag&0x80000000)!=0)) break;       \
                       zdeep0=zdeep0>>ZSHIFT;                              \
                       if (zdeep0>MAX_DEPTH) zdeep0=MAX_DEPTH;             \
                       zdeep0=MAX_DEPTH-zdeep0;                            \
                       if ((BACKCLIP!=0) && (NormalClip(sxy0,sxy1,sxy2)>0)) \
break;
#define                                                                   \
                       rot4 \
 zdeep=RotTransPers4(v0,v1,v2,v3,&sxy0,&sxy1,&sxy2,&sxy3,&que,&flag);       \
                       if (ZREJECT && ((flag&0x80000000)!=0)) break;       \
                       zdeep=zdeep>>ZSHIFT;                                \
                       if (zdeep>MAX_DEPTH) zdeep=MAX_DEPTH;               \
                       zdeep=MAX_DEPTH-zdeep;                              \
                       if ((BACKCLIP!=0) && (NormalClip(sxy0,sxy1,sxy2)>0)) \
break;
#define                                                                   \
                       rot3clip \
 zdeep=RotTransPers3(v0,v1,v2,&sxy0,&sxy1,&sxy2,&que,&flag);                \
                       zdeep=zdeep>>ZSHIFT;                                \
                       if (zdeep>MAX_DEPTH) zdeep=MAX_DEPTH; 
#define                                                                    \
                       rot4clip \
 zdeep=RotTransPers4(v0,v1,v2,v3,&sxy0,&sxy1,&sxy2,&sxy3,&que,&flag);       \
                       zdeep=zdeep>>ZSHIFT;                                \
                       if (zdeep>MAX_DEPTH) zdeep=MAX_DEPTH;               \
                       zdeep=MAX_DEPTH-zdeep; 
#define                                                                   \
                       shift 0
#define  setTrans(p)   p->code|=0x02 
#define  setaddr11(p,    addr)\
 ((p)[0]=(((p)[0]&0xff000000)|((addr)&0x00ffffff)))
#define  getaddr11(p)    ((p)[0]&0x00ffffff)
#define  AddPrim3(ot,p) setaddr11(p, getaddr11(ot));                          \
                       setaddr11(ot, (u_int)p) 
#define  AddPrim2(a,b) AddPrim(a,b)

/*
 *       Prototypes
 *
                                                                            */

DsOBJECT *DsFindHole(void);
int *dat2long(word *p1,int *p2);
void DsSeeView(VIEW *v,int x1,int y1);
DsOBJECT *start_entity(int id,int x,int y,int z,int h,int p,int r);

/* 
 * exec.c 
                                                                            */

void DsClearObjs(void);
DsOBJECT *DsFindHole(void);
void DsSeeObjs(void);
void DsMoveObjs(void);
void DsScanObjs(void *sub());
void DsKillObj(DsOBJECT *p);
DsOBJECT *DsStartObj(void);
DsOBJECT *DsPlaceObj(int i,int x,int y,int z);
DsOBJECT	*DsGetObjStart(int i);
DsOBJECT	*DsGetObjEnd(int i);
void DsInitShape(int i);
void DsHrot(DsOBJECT *p);
void DsTumble(DsOBJECT *p);
void DsPrint(char c);

/* 
 *    tmdconv.c 
                                                                            */

ulong *DsFormatTmd(ulong *tmdp);
void DsColourTmd(ulong *p,u_char r,u_char g,u_char b);
void DsScaleTmd(ulong *p,int sc);
void scale_axis_tmd(ulong *p,int axis,int sc);
void DsSwapAxisTmd(ulong *p);
void DsCentreTmd(ulong *p);
int  DsGetExtentTmd(ulong *p);
void DsZCentreTmd(ulong *p);
void DsTextureTmd(ulong *p,ulong *text);
void DsCylinderTmd(ulong *p);
void DsSphereTmd(ulong *p);

/* 
 *    light.c 
                                                                            */

void DsPointLight(int id,int xv,int yv,int zv);
void DsSetLight(int id,int fr,int fg,int fb,int h,int p);
void DsPositionLight(int id,int xv,int yv,int zv);
void DsAngleLight(int id,int h,int p);
void DsColourLight(int id,int fr,int fg,int fb);
void DsSetFogDepth(int a);
void DsSetFogColour(char r,char g,char b);
void DsSetAmbientColour(char r,char g,char b);

/*
 *    trig.c         
                                                                            */

void DsCalcWorldMatrix(void);
void DsSetView(int x,int y,int z,int hdg,int pitch,int roll);
void DsSetRview(int x0,int y0,int z0,int xv,int YV,int ZV,int roll);
void DsSetPview(int x0,int y0,int z0,int x1,int y1,int z1,int roll);
void DsSetWorldMatrixToView(VIEW *v);
void DsTmdClip3d(int *p);
extern int DsDrawObj(DsOBJECT *p);
extern void DsVizObj(int *p);
extern void DsMarcusViz(int *p);

/*
 *    psx.c
                                                                            */

void DsAddVector(int x1,int y1,int z1,int x2,int y2,int z2);
void DsClipWindow(int x1,int y1,int x2,int y2);



/* Get thee behind me, heinous variable of GlO-BAAL

/*psx.c*/
extern int first;
extern int topdepth;
extern int XREZ;
extern int YREZ;
extern int size;
extern u_long *mem;
extern u_long *membase;
extern int XMIN;
extern int XMAX;
extern int XCEN;
extern int YMIN;
extern int YMAX;
extern int YCEN;
extern byte red;
extern byte green;
extern byte blue;
extern ulong *ot;
extern int XCEN;
extern int DEFAULT_Z;
extern u_short textTSB;
extern u_short textCBA;

/*tmdclip*/
extern int z0,z1,z2;
extern	int	BACKC;

/*vec1*/
extern int xtab;
extern int ytab;

/*clip3d*/
extern DsPOINT eyebuff[];
extern DsSHAPE tab[];
extern int OTZ;
extern DsPOINT *ptlist[];
extern DsPOINT *r;
extern CVECTOR col80;
extern ulong textCOL;

/*trig*/
extern int lightx[3][3];
extern int xpos;
extern int ypos;
extern int zpos;
extern VIEW *view;
extern int DEPTH;
extern int ZMIN;
extern int H_DISTANCE;
extern SMATRIX world;
extern int zdeep;
extern int FOGZ;

/* exec */
extern DsSHAPE tab[];

/*tmdfast*/
extern unsigned int gflag111;

/*tmdclip*/
extern DsPOINT **p1;
extern DsPOINT **p2;


#include	"tmd.h"
#include	"swap.h"
