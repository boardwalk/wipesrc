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


#if 0

void NewInitAll
( 
   void 
);


TrackSection *TransformTrackHi
( 
   TrackSection*     firstSection,
   TileLib*          tileLib,
   Skeleton*         camera,
   short             hiCount,
   AutoMeshData*     autoMesh,
   short             direction
);


TrackSection *TransformTrackMed
( 
   TrackSection*     firstSection,
   TileLib*          tileLib,
   Skeleton*         camera,
   short             medCount
);

TrackSection *TransformTrackLo
( 
   TrackSection*     firstSection,
   TileLib*          tileLib,
   Skeleton*         camera,
   short             loCount
);

short CompareVertices
( 
   VECTOR* vertices, 
   ushort a, 
   ushort b 
);


void AutoMesh
( 
   SVECTOR*       tl, 
   SVECTOR*       tr, 
   SVECTOR*       bl, 
   SVECTOR*       br, 
   PolyFT4*       quad, 
   AutoMeshData*  autoMesh
);



#endif
