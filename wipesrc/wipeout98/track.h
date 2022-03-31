#define NEW_TRACK 1
#include "small_ptr.h"

typedef struct Normal
{
   int8_t     vx;
   int8_t     vy;
   int8_t     vz;
} Normal;


typedef struct Vector
{
   int32_t           vx;
   int32_t           vy;
   int32_t           vz;
} Vector;


typedef struct SVector
{
   int16_t          vx;
   int16_t          vy;
   int16_t          vz;
} SVector;




/* Face flags */
#define  TRACK_BASE      	1
#define	WEAPON_GRID_LEFT  2
#define  FlipTexture       4
#define	WEAPON_GRID_RIGHT 8
#define	Start_Grid        16
#define	SPEED_UP          32

typedef struct Face
{
 	int16_t		      vertex[ 4 ];
	SVector	      normal;
	char		      texture;
   char           flags;
   char           red;
   char           green;
   char           blue;
} Face;



typedef union
{
   SmallPtr<struct TrackSection>   ptr;
   int32_t                  index;
} SectionPtr;


/* TrackSection Flags */
#define  Jump              1
#define  JunctionEnd       8
#define  JunctionStart     16
#define  JunctionSection   32

typedef struct TrackSection
{
   SectionPtr     junction;

   SectionPtr     prevSection;
	SectionPtr		nextSection;

	Vector	      centre;

   int16_t          version;
   int16_t          pad;

	int16_t          *objects;
	int16_t          objectCount;

   int16_t          *northViewSection[ 3 ];
   int16_t          *southViewSection[ 3 ];
   int16_t          *eastViewSection[ 3 ];
   int16_t          *westViewSection[ 3 ];
   int16_t          *allViewSection[ 3 ];
   int16_t          northViewCount[ 3 ];
   int16_t          southViewCount[ 3 ];
   int16_t          eastViewCount[ 3 ];
   int16_t          westViewCount[ 3 ];
   int16_t          allViewCount[ 3 ];
   int16_t          high[ 4 ];
   int16_t          med[ 4 ];

	int16_t		      faceStart;
 	int16_t		      faceCount;

   int16_t          globalRadius;
   int16_t          localRadius;

   int16_t          flags;
   int16_t          secNo;
} TrackSection;



typedef struct colors
{
   char           red;
   char           green;
   char           blue;
} Colors;


typedef struct TrackData
{
	int32_t           vertexCount;
	int32_t           faceCount;
	int32_t           sectionCount;

   VECTOR         *vertices;			/* pointer to our track vertex table (which is later deallocated) */
   Face				*faces;				/* pointer to our list of faces */
   TrackSection   *sections;			/* pointer to the first section */

   int16_t          *viewList;
   Colors   		*colCopy;			/* original colours */
} TrackData;


typedef struct TrackCamera
{
	Skeleton* camPos;
	TrackSection* section;
   int16_t hdg, pitch, roll;
   int16_t vhdg, vpitch, vroll;
	SVECTOR vel;
} TrackCamera;

#define  MaxAutos       256

typedef struct AutoMeshData
{
   PolyFT4                 quad[ MaxAutos ][ 2 ];
   PolyFT3                 tri[ MaxAutos ][ 2 ];
   int16_t                   quadCount;              /* no. of automeshed quads added */
   int16_t                   triCount;               /* no. of automeshed tris added */
   int16_t                   attemptedAutos;         /* num of times automesh is called */
   int16_t                   colourMeshed;           /* colours on/off */
   int16_t                   autoMeshedOn;           /* automesh on/off */
} AutoMeshData;

void IntelVertex( void );
void IntelFace( void );
void IntelSection( void );

void CheckVersion( void );

void IndexToPointers( void );
void PointersToIndex( void );

void InitVertices( char *file );
void InitFaces( char *file );
void InitViewList( char *file );
void InitSections( char *file );

TrackSection* NextIndexSection( TrackSection* section );
TrackSection* PrevIndexSection( TrackSection* section );

void DrawNewTrack( TrackCamera *camera, AutoMeshData *autoMesh );
void DrawAllTrack( TrackCamera *camera, AutoMeshData *autoMesh );
