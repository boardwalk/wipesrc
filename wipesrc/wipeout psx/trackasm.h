/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/




#define  MaxAutos       256


typedef struct AutoMeshData
{
   PolyFT4                 quad[ MaxAutos ][ 2 ];
   PolyFT3                 tri[ MaxAutos ][ 2 ];
   short                   quadCount;              /* no. of automeshed quads added */
   short                   triCount;               /* no. of automeshed tris added */
   short                   attemptedAutos;         /* num of times automesh is called */
   short                   colourMeshed;           /* colours on/off */
   short                   autoMeshedOn;           /* automesh on/off */
} AutoMeshData;




typedef struct Tile
{
   short       tsb;
   short       cba;
	char 			u0;
	char 			v0;
	char 			u1;
	char 			v1;
	char 			u2;
	char 			v2;
	char 			u3;
	char 			v3;
} Tile;


typedef struct Surface
{
   Tile        low[ 1 ];
   Tile        med[ 4  ];
   Tile        high[ 16 ];
} Surface;



void AsmLowResTile
( 
   TexTemplate*	template,
   short          index,
   short          reversed
);

void AsmMediumResTile
( 
   TexTemplate*	template,
   short          index,
   short          reversed
);

void AsmHighResTile
( 
   TexTemplate*	template,
   short          index,
   short          reversed
);

void AsmGenerateTiles
(  
	TexTemplate*	texTemplate,
   short          numLib
);

void AsmNextFrame( );

TrackSection *AsmTransformTrackLo
( 
   TrackSection*    firstSection,
   Skeleton*        camera,
   short            loCount,
   short			dir
);

TrackSection *AsmTransformTrackMed
( 
   TrackSection*     firstSection,
   Skeleton*         camera,
   short             loCount,
   short		     dir
);

TrackSection *AsmTransformTrackHi
( 
   TrackSection*     firstSection,
   Skeleton*         camera,
   AutoMeshData*     autoMesh,
   short             loCount,
   short			 dir
);


short CompareVertices( VECTOR* vertices, ushort a, ushort b );

void NewInitAll( void );

void AsmAutoMesh
( 
   SVECTOR*       tlVector, 
   SVECTOR*       trVector, 
   SVECTOR*       blVector, 
   SVECTOR*       brVector, 
   Face*          face,
   Tile*          tile,
   AutoMeshData*  autoMesh
);
