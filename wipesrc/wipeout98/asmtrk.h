/* (C) Psygnosis 1994. By Jason Carl Denton
*/

TrackSection *AsmTransformTrackLo	// PCwipeout
(
   TrackSection*     firstSection,
   Skeleton*         camera,
   int16_t             loCount,
   int16_t			 dir
);

TrackSection *AsmTransformTrackMed	// PCwipeout
(
   TrackSection*     firstSection,
   Skeleton*         camera,
   int16_t             medCount,
   int16_t			 dir
);

TrackSection *AsmTransformTrackHi	// PCwipeout
(
   TrackSection*     firstSection,
   Skeleton*         camera,
   AutoMeshData*     autoMesh,
   int16_t             medCount,
   int16_t			 dir
);
