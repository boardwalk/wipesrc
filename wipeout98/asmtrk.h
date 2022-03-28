/* (C) Psygnosis 1994. By Jason Carl Denton
*/

TrackSection *AsmTransformTrackLo	// PCwipeout
( 
   TrackSection*     firstSection,
   Skeleton*         camera,
   short             loCount,
   short			 dir
);

TrackSection *AsmTransformTrackMed	// PCwipeout
( 
   TrackSection*     firstSection,
   Skeleton*         camera,
   short             medCount,
   short			 dir
);

TrackSection *AsmTransformTrackHi	// PCwipeout
( 
   TrackSection*     firstSection,
   Skeleton*         camera,
   AutoMeshData*     autoMesh,
   short             medCount,
   short			 dir
);
