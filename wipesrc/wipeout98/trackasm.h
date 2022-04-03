/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/
#ifndef TRACKASM_H
#define TRACKASM_H




typedef struct Tile
{
   int16_t       tsb;
   int16_t       cba;
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
   int16_t          index,
   int16_t          reversed
);

void AsmMediumResTile
(
   TexTemplate*	Template,
   int16_t          index,
   int16_t          reversed
);

void AsmHighResTile
(
   TexTemplate*	Template,
   int16_t          index,
   int16_t          reversed
);

void AsmGenerateTiles
(
	TexTemplate*	texTemplate,
   int16_t          numLib
);

void AsmNextFrame( );

TrackSection *AsmTransformTrackLo
(
   TrackSection*    firstSection,
   Skeleton*        camera,
   int16_t            loCount,
   int16_t			dir
);

TrackSection *AsmTransformTrackMed
(
   TrackSection*     firstSection,
   Skeleton*         camera,
   int16_t             loCount,
   int16_t		     dir
);

TrackSection *AsmTransformTrackHi
(
   TrackSection*     firstSection,
   Skeleton*         camera,
   AutoMeshData*     autoMesh,
   int16_t             loCount,
   int16_t			 dir
);


int16_t CompareVertices( VECTOR* vertices, uint16_t a, uint16_t b );

void NewInitAll();

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

#endif
