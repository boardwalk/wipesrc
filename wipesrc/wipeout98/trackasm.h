/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/





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
   TexTemplate*	Template,
   short          index,
   short          reversed
);

void AsmMediumResTile
( 
   TexTemplate*	Template,
   short          index,
   short          reversed
);

void AsmHighResTile
( 
   TexTemplate*	Template,
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
