/* (C) Psygnosis 1994. By Jason Carl Denton
*/


#define  Lib      0


/* Block Fill */

typedef struct
{
   ulong    tag;
   uchar    r0, g0, b0, code;
   ushort   x, y;
   ushort   w, h;
} BlkFill;

/* Flat Triangle */

typedef struct
{
   ulong    tag;
   CVECTOR  colour0;
   DVECTOR  xy0;
   DVECTOR  xy1;
   DVECTOR  xy2;
} PolyF3;

/* Flat Rectangle */

typedef struct
{
   ulong    tag;
   CVECTOR  colour0;
   DVECTOR  xy0;
   DVECTOR  xy1;
   DVECTOR  xy2;
   DVECTOR  xy3;
} PolyF4;            

/* Flat Textured Triangle */

typedef struct
{
   ulong    tag;
   CVECTOR  colour0;
   SVECTOR  xy0;
   uchar    u0, v0;
   ushort   cba;
   SVECTOR  xy1;
   uchar    u1, v1;
   ushort   tsb;
   SVECTOR  xy2;
   uchar    u2, v2;
   ushort   pad1;
} PolyFT3;            

/* Flat Textured Rectangle */

typedef struct 
{
   ulong    tag;
   CVECTOR  colour0;
   SVECTOR  xy0;
   uchar    u0, v0;   
   ushort   cba;
   SVECTOR  xy1;
   uchar    u1, v1;   
   ushort   tsb;
   SVECTOR  xy2;
   uchar    u2, v2;   
   ushort   pad1;
   SVECTOR  xy3;
   uchar    u3, v3;   
   ushort   pad2;
} PolyFT4;            

/* Gouraud Triangle */

typedef struct 
{
   ulong    tag;
   CVECTOR  colour0;
   DVECTOR  xy0;
   CVECTOR  colour1;
   DVECTOR  xy1;
   CVECTOR  colour2;
   DVECTOR  xy2;
} PolyG3;            

/* Gouraud Rectangle */

typedef struct 
{
   ulong    tag;
   CVECTOR  colour0;
   DVECTOR  xy0;
   CVECTOR  colour1;
   DVECTOR  xy1;
   CVECTOR  colour2;
   DVECTOR  xy2;
   CVECTOR  colour3;
   DVECTOR  xy3;
} PolyG4;            

/* Gouraud Textured Triangle */

typedef struct 
{
   ulong    tag;
   CVECTOR  colour0;
   SVECTOR  xy0;
   uchar    u0, v0;   
   ushort   cba;
   CVECTOR  colour1;
   SVECTOR  xy1;
   uchar    u1, v1;   
   ushort   tsb;
   CVECTOR  colour2;
   SVECTOR  xy2;
   uchar    u2, v2;   
   ushort   pad2;
} PolyGT3;            

/* Gouraud Textured Rectangle */

typedef struct 
{
   ulong    tag;
   CVECTOR  colour0;
   SVECTOR  xy0;
   uchar    u0, v0;   
   ushort   cba;
   CVECTOR  colour1;
   SVECTOR  xy1;
   uchar    u1, v1;   
   ushort   tsb;
   CVECTOR  colour2;
   SVECTOR  xy2;
   uchar    u2, v2;   
   ushort   pad2;
   CVECTOR  colour3;
   SVECTOR  xy3;
   uchar    u3, v3;   
   ushort   pad3;
} PolyGT4;            


/* Flat shaded line with 2 coords */

typedef struct
{
   ulong*   tag;
   CVECTOR  colour0;
   DVECTOR  xy0;
   DVECTOR  xy1;
} LineF2;


/* 2D Primitive Stubs 
*/

typedef struct Prim
{
   uint        next : 24;
   uint        size : 8;
   CVECTOR     colour0;
} Prim;

/* Sony PRIM structure
*/

typedef struct PRIM
{
   struct PRIM*   tag;
   CVECTOR        colour0;
} PRIM;

typedef struct Tag
{
   uint           next : 24;
   uint           size : 8;
} Tag;




/* Screen Double Buffering Structure
*/

typedef struct DoubleBuffer
{
   DRAWENV*    draw;
   DISPENV*    disp;
} DoubleBuffer;




/* Skeleton Structure ( Hierarchical Coordinate System )
*/

typedef struct Skeleton
{
   MATRIX            relative;   /* Relative rotation/translation matrix */
   MATRIX            absolute;   /* Absolute rotation/translation matrix */

   short             update;     /* Set if absolute matrix needs updating ( i.e. parent matrix has been changed ) */

   struct Skeleton*  super;      /* Parent Skeleton */ 
   struct Skeleton*  sub;        /* First Child Skeleton of this Skeleton */
   struct Skeleton*  next;       /* Next Child Skeleton of Parent Skeleton */
} Skeleton;




/* Primitive Structure Stub ( Structure varies with primitive type )
*/

typedef struct Primitive
{
   short             type;    /* Type of Primitive */
} Primitive;




/* Actual Primitive Structures
*/


/* FIXED COLOUR POLYGONS
*/

typedef struct F3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             pad1;
   CVECTOR           colour;
} F3;

typedef struct FT3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   short             pad1;
   CVECTOR           colour;
} FT3;

typedef struct F4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   CVECTOR           colour;
} F4;

typedef struct FT4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   char              u3;
   char              v3;
   short             pad1;
   CVECTOR           colour;
} FT4;

typedef struct G3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             pad1;
   CVECTOR           colour[ 3 ];
} G3;

typedef struct GT3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   short             pad1;
   CVECTOR           colour[ 3 ];
} GT3;

typedef struct G4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   CVECTOR           colour[ 4 ];
} G4;

typedef struct GT4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   char              u3;
   char              v3;
   short             pad1;
   CVECTOR           colour[ 4 ];
} GT4;




/* LIGHT SOURCED POLYGONS
*/

typedef struct LSF3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             normal;        /* Indices of the normals */
   CVECTOR           colour;
} LSF3;

typedef struct LSFT3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             normal;        /* Indices of the normals */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   CVECTOR           colour;
} LSFT3;

typedef struct LSF4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   short             normal;        /* Indices of the normals */
   short             pad1;
   CVECTOR           colour;
} LSF4;

typedef struct LSFT4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   short             normal;        /* Indices of the normals */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   char              u3;
   char              v3;
   CVECTOR           colour;
} LSFT4;

typedef struct LSG3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             normals[ 3 ];  /* Indices of the normals */
   CVECTOR           colour[ 3 ];
} LSG3;

typedef struct LSGT3
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 3 ];   /* Indices of the coords */
   short             normals[ 3 ];  /* Indices of the normals */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   CVECTOR           colour[ 3 ];
} LSGT3;

typedef struct LSG4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   short             normals[ 4 ];  /* Indices of the normals */
   CVECTOR           colour[ 4 ];
} LSG4;

typedef struct LSGT4
{
   short             type;          /* Type of primitive */
   short             flag;
   short             coords[ 4 ];   /* Indices of the coords */
   short             normals[ 4 ];  /* Indices of the normals */
   short             texture;
   short             cba;
   short             tsb;
   char              u0;
   char              v0;
   char              u1;
   char              v1;
   char              u2;
   char              v2;
   char              u3;
   char              v3;
   short             pad1;
   CVECTOR           colour[ 4 ];
} LSGT4;






/* OTHER PRIMITIVE TYPES
*/

typedef struct LF2
{
   short             type;          /* Type of primitive */
   short             flag;
   short             lines;         /* Number of lines */
   LineF2*           line[ 2 ];     /* 2 GPU Primitive arrays for double buffer */
} LF2;

typedef struct SPR
{
   short             type;
   short             flag;
   short             coord;
   short             width;
   short             height;
   short             texture;
   CVECTOR           colour;
} SPR;


typedef struct Spline
{
   short             type;          /* Type of primitive */
   short             flag;
   VECTOR            control1;
   VECTOR            position;
   VECTOR            control2;
   CVECTOR           colour;
} Spline;


typedef struct PointLight
{
   short             type;
   short             flag;
   VECTOR            position;
   CVECTOR           colour;
   short             startFalloff;
   short             endFalloff;
} PointLight;


typedef struct SpotLight
{
   short             type;
   short             flag;
   VECTOR            position;
   SVECTOR           direction;
   CVECTOR           colour;
   short             startFalloff;
   short             endFalloff;
   short             coneAngle;
   short             spreadAngle;
} SpotLight;


typedef struct InfiniteLight
{
   short             type;
   short             flag;
   SVECTOR           direction;
   CVECTOR           colour;
} InfiniteLight;






/* PRIMITIVE FLAGS */

#define  SingleSided    0x0001
#define  ShipEngine     0x0002
#define  Translucent    0x0004



#if Lib

/* Library Primitive Structures
*/

typedef struct LibF3
{
   CVECTOR           colour;
} LibF3;

typedef struct LibFT3
{
   CVECTOR           colour;
} LibFT3;

typedef struct LibF4
{
   CVECTOR           colour;
} LibF4;

typedef struct LibFT4
{
   CVECTOR           colour;
} LibFT4;

typedef struct LibG3
{
   CVECTOR           colour[ 3 ];
} LibG3;

typedef struct LibGT3
{
   CVECTOR           colour[ 3 ];
} LibGT3;

typedef struct LibG4
{
   CVECTOR           colour[ 4 ];
} LibG4;

typedef struct LibGT4
{
   CVECTOR           colour[ 4 ];
} LibGT4;


typedef struct LibSPR
{
   CVECTOR           colour;
} LibSPR;


#endif



typedef union Poly
{
   char*             ptr;
   Primitive*        prim;
   
   F3*               f3;
   FT3*              ft3;
   F4*               f4;
   FT4*              ft4;
   G3*               g3;
   GT3*              gt3;
   G4*               g4;
   GT4*              gt4;
   
   LSF3*             lsf3;
   LSFT3*            lsft3;
   LSF4*             lsf4;
   LSFT4*            lsft4;
   LSG3*             lsg3;
   LSGT3*            lsgt3;
   LSG4*             lsg4;
   LSGT4*            lsgt4;

   LF2*              lf2;
   SPR*              spr;

   Spline*           spline;

   InfiniteLight*    infiniteLight;
   PointLight*       pointLight;
   SpotLight*        spotLight;

} Poly;

typedef union StdPoly
{
   char*             ptr;
   Prim*             prim;
   PolyF3*           f3;
   PolyFT3*          ft3;
   PolyF4*           f4;
   PolyFT4*          ft4;
   PolyG3*           g3;
   PolyGT3*          gt3;
   PolyG4*           g4;
   PolyGT4*          gt4;
   LineF2*           lf2;
   SPR*              spr;
   DR_MODE*          drmode;
} StdPoly;



#define     TypeF3               1
#define     TypeFT3              2
#define     TypeF4               3
#define     TypeFT4              4
#define     TypeG3               5
#define     TypeGT3              6
#define     TypeG4               7
#define     TypeGT4              8

#define     TypeLF2              9  
#define     TypeTSPR             10
#define     TypeBSPR             11

#define     TypeLSF3             12
#define     TypeLSFT3            13
#define     TypeLSF4             14
#define     TypeLSFT4            15
#define     TypeLSG3             16
#define     TypeLSGT3            17
#define     TypeLSG4             18
#define     TypeLSGT4            19

#define     TypeSpline           20

#define     TypeInfiniteLight    21
#define     TypePointLight       22
#define     TypeSpotLight        23


/* BSP Tree Structure

   NOTE : The BSP Tree only gives the relative sorting
   order for the objects in the tree. They must then be
   linked into the Z sort table using the object's centre
   point Z value.

   Drawing order when polygons are visible :
      1  back branch polygons
      2  plane polygons
      3  front branch polygons

   Drawing order when polygons are invisible ( back face rejected ) :
      1  front branch polygons
      2  back branch polygons
*/

typedef struct BspTree
{
   short             primitiveCount;   /* Number of coplanar primitives */
   Primitive*        primitives;       /* Pointer to the primitives */

   struct BspTree*   front;            /* Bsp Tree for polygons in front */
   struct BspTree*   back;             /* Bsp Tree for polygons behind */
} BspTree;


#if Lib

typedef union LibraryPrimitive
{
   char*       ptr;
   LibF3*      f3;
   LibFT3*     ft3;
   LibF4*      f4;
   LibFT4*     ft4;
   LibG3*      g3;
   LibGT3*     gt3;
   LibG4*      g4;
   LibGT4*     gt4;
   LibSPR*     spr;
} LibraryPrimitive;

typedef struct LibraryObject
{
   short                index;
   short                primitiveCount;
   LibraryPrimitive     primitives;
} LibraryObject;

#endif



/* Main Object Structure
   
   NOTE : Two copies of each primitive are required for double buffering.
*/

typedef struct Object
{
   char              name[16];

   short             vertexCount;      /* Number of Vertices */
   SVECTOR*          vertices;         /* Pointer to 3D Points */

   short             normalCount;      /* Number of Normals */
   SVECTOR*          normals;          /* Pointer to 3D Normals */

   short             primitiveCount;   /* Number of Primitives */
   Primitive*        primitives;       /* Pointer to Z Sort Primitives */


#if Lib
   LibraryObject*    libraryObject;
#else
   long              pad;
#endif

   BspTree*          bspTree;          /* Pointer to BSP Tree Primitives */

   Skeleton*         skeleton;         /* Pointer to Local Coordinates */

   long              extent;           /* Flags for object characteristics */

   short             flags;            /* Next object in list */

   struct Object*    next;             /* Next object in list */
} Object;



typedef union Prm
{
   char*             ptr;
   short*            sptr;
   long*             lptr;
   Object*           object;
   Skeleton*         skeleton;
   SVECTOR*          svector;
   Primitive*        primitive;

#if Lib
   LibraryObject*    libObj;
#endif

   F3*               f3;
   FT3*              ft3;
   F4*               f4;
   FT4*              ft4;
   G3*               g3;
   GT3*              gt3;
   G4*               g4;
   GT4*              gt4;
   SPR*              spr;
   Spline*           spline;
   PointLight*       pointLight;
   SpotLight*        spotLight;
   InfiniteLight*    infiniteLight;

   LSF3*             lsf3;
   LSFT3*            lsft3;
   LSF4*             lsf4;
   LSFT4*            lsft4;
   LSG3*             lsg3;
   LSGT3*            lsgt3;
   LSG4*             lsg4;
   LSGT4*            lsgt4;

#if Lib

   LibF3*            libf3;
   LibFT3*           libft3;
   LibF4*            libf4;
   LibFT4*           libft4;
   LibG3*            libg3;
   LibGT3*           libgt3;
   LibG4*            libg4;
   LibGT4*           libgt4;
   LibSPR*           libspr;

#endif

} Prm;




extern ulong*        ObjectTable[ ];
extern DoubleBuffer  Screen[ ];
extern short         CurrentScreen;


extern DRAWENV       DrawEnv0;
extern DRAWENV       DrawEnv1;
extern DISPENV       DispEnv0;
extern DISPENV       DispEnv1;

extern VECTOR        CameraPosition;
extern SVECTOR       CameraDirection;
extern MATRIX        Camera;


#define CrtClipped   0x80000000
#define XLarge       0x01000000
#define YLarge       0x00800000
#define ZLarge       0x00400000
#define ZNegative    0x00040000
#define ZSmall       0x00020000
#define XClipped     0x00004000
#define YClipped     0x00002000
#define OutFog       0x00001000





PolyF4* GenerateQuad( );

PolyF4* ClearScreen
(
   short          red,
   short          green,
   short          blue
);

void InitSystem( void );
void Swap( );

void SetCameraPosition( long x, long y, long z );
void SetCameraDirection( short x, short y, short z );


void TransformStaticObject( Object* object, Skeleton* camera );
void TransformObjects( Object* object, short minDepth, short follow );
void MatrixMultiply( MATRIX* m1, MATRIX* m2, MATRIX* m3 );

void InitialiseAmbientColour( void );
void InitialiseColourMatrix( void );
void InitialiseLightMatrix( void );

void SetupLights( Object* lights );

void InitFonts( );
void InitGameDisplay( void );
void InitMenuDisplay( int geomOffset );


PolyF4* Quad
(
   short          red,
   short          green,
   short          blue,
   short          border
);

BlkFill* BlockFill
(
   short          x,
   short          y,
   short          w,
   short          h,
   short          red,
   short          green,
   short          blue
);



void	    InitSonyDisplay(void);

void	    TransformStaticMenuObject(Object * object, Skeleton * camera);
void InitGeom( void );
void	    TransformStaticObjectBodge(Object * object, Skeleton * camera);
