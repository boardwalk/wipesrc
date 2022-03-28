#if 0

void TransformTrack
( 
   TrackSection*     firstSection,
   TileLib*          tileLib,
   Skeleton*         camera
)
{
/* 
   Variables with greatest need for optimisation 
   Listed in decreasing order of priority 
*/

   VECTOR*              vector;
   VECTOR*              FrVector = (( VECTOR* ) FastRam );
   SVECTOR*             FrSVector = (( SVECTOR* )( FastRam + ( 22 * sizeof( VECTOR ) ) ) );

   #define              campos   FrVector
   #define              svector  FrSVector

/* original 32 bit vertices */

   VECTOR*              tr32;
   VECTOR*              tl32;
   VECTOR*              bl32;
   VECTOR*              br32;

/* the meshed results */

   #define              l32   ( FrVector + 1 )
   #define              t32   ( FrVector + 2 )
   #define              r32   ( FrVector + 3 )
   #define              b32   ( FrVector + 4 )
   #define              c32   ( FrVector + 5 )


   #define              tw32    ( FrVector + 6 )
   #define              te32    ( FrVector + 7 )
   #define              rn32    ( FrVector + 8 )
   #define              rs32    ( FrVector + 9 )
   #define              bw32    ( FrVector + 10 )
   #define              be32    ( FrVector + 11 )
   #define              ln32    ( FrVector + 12 )
   #define              ls32    ( FrVector + 13 )

   #define              ctl32   ( FrVector + 14 )
   #define              ctr32   ( FrVector + 15 )
   #define              cbl32   ( FrVector + 16 )
   #define              cbr32   ( FrVector + 17 )

   #define              ct32    ( FrVector + 18 )
   #define              cr32    ( FrVector + 19 )
   #define              cb32    ( FrVector + 20 )
   #define              cl32    ( FrVector + 21 )



/* the 16 bit equivalents of the above */

   #define              l16   ( FrSVector + 1 )
   #define              t16   ( FrSVector + 2 )
   #define              r16   ( FrSVector + 3 )
   #define              b16   ( FrSVector + 4 )
   #define              c16   ( FrSVector + 5 )

   #define              tr16  ( FrSVector + 6 )
   #define              tl16  ( FrSVector + 7 )
   #define              bl16  ( FrSVector + 8 )
   #define              br16  ( FrSVector + 9 )


   #define              tw16    ( FrSVector + 10 )
   #define              te16    ( FrSVector + 11 )
   #define              rn16    ( FrSVector + 12 )
   #define              rs16    ( FrSVector + 13 )
   #define              bw16    ( FrSVector + 14 )
   #define              be16    ( FrSVector + 15 )
   #define              ln16    ( FrSVector + 16 )
   #define              ls16    ( FrSVector + 17 )

   #define              ctl16   ( FrSVector + 18 )
   #define              ctr16   ( FrSVector + 19 )
   #define              cbl16   ( FrSVector + 20 )
   #define              cbr16   ( FrSVector + 21 )

   #define              ct16    ( FrSVector + 22 )
   #define              cr16    ( FrSVector + 23 )
   #define              cb16    ( FrSVector + 24 )
   #define              cl16    ( FrSVector + 25 )


   FT4*                 p;

   short                x, y;
   short                i, j, k;
   
   Poly                 poly;
   PolyFT4*             quad;

   long                 interpolate;

   TrackSection*        section;
   Face*                face;

   VECTOR               *vertices;
   short                faceCount;

   short                offset;
   Object*              object;


/* number of sections to draw ahead of each resolution */

   short                loCount = 15;   /* 16 track sections - 1 */
   short                medCount = 3;   /* 4 track sections - 1 */
   short                hiCount = 3;    /* 4 track sections - 1 */



   /* Use a structure, then dereference it once! */

   long                 loScreen [ MaxLo + 1 + MaxJumps ][ MaxFaces + 1 ];
   long                 loDepth  [ MaxLo + 1 + MaxJumps ][ MaxFaces + 1 ];
   long                 loFlag   [ MaxLo + 1 + MaxJumps ][ MaxFaces + 1 ];

   long                 medScreen [ ( MaxMed * 2 ) + 1 + MaxJumps ][ ( MaxFaces * 2 ) + 1 ];
   long                 medDepth  [ ( MaxMed * 2 ) + 1 + MaxJumps ][ ( MaxFaces * 2 ) + 1 ];
   long                 medFlag   [ ( MaxMed * 2 ) + 1 + MaxJumps ][ ( MaxFaces * 2 ) + 1 ];

   long                 hiScreen [ ( MaxHi * 4 ) + 1 + MaxJumps ][ ( MaxFaces * 4 ) + 1 ];
   long                 hiDepth  [ ( MaxHi * 4 ) + 1 + MaxJumps ][ ( MaxFaces * 4 ) + 1 ];
   long                 hiFlag   [ ( MaxHi * 4 ) + 1 + MaxJumps ][ ( MaxFaces * 4 ) + 1 ];


   if ( edDebug )
   {
      if ( firstSection->flags & Jump )
      {
         FntPrint( "Jump\n" );
      }

      if ( firstSection->flags & Tunnel )
      {
         FntPrint( "Tunnel\n" );
      }

      if ( firstSection->flags & MoreFaces )
      {
         FntPrint( "More Faces\n" );
      }
   }

   firstSection = PrevSection( firstSection );

/* Set transformation matrix */

   SetSkeletonTransform( camera->super );

/* Copy camera position into fast ram */

   campos->vx = camera->relative.t[ 0 ];
   campos->vy = camera->relative.t[ 1 ];
   campos->vz = camera->relative.t[ 2 ];

/* Initialise arrays */

   for (i=0; i<25; i++)
   {
      for (j=0; j<9; j++)
      {
         loScreen[i][j] = 0;
         loDepth[i][j] = 0;
         loFlag[i][j] = CrtClipped | ZNegative | ZLarge | ZSmall;
      }
   }

/* 
   from the new face array we put together in NewInitAll we can now
   go through each face making sure we share points whilst doing
   RotTransPers.
   A 2D screen coord array can be filled in from this 
*/


/* Calculate Hi res meshed track sections */

   vertices = track->vertices;

   section = firstSection;

#if HiRes

   y = 0;
   for ( i=0; i <= hiCount; i++ )
   {
      offset = section->offset << 2;

      face = track->faces + section->faceStart;
      faceCount = section->faceCount - 1;

      x = 0;
   	for ( j=0; j <= faceCount; j++ )
      {
#if Jumps
         if ( !( section->flags & MoreFaces ) && !( section->flags & Jump ) && ( 0 ) /* ( i < hiCount ) */  )
#else
         if ( ( !( section->flags & MoreFaces ) ) && ( 0 ) /* ( i < hiCount ) */ )
#endif
         {
         }
         else
         {
         /* First sections */

            if ( 1 || j < faceCount )
            {
            /* First faces in first sections */

            /* get original 32 bit faces coords */

               tr32 = vertices + face->vertex[ 0 ];
               tl32 = vertices + face->vertex[ 1 ];
               bl32 = vertices + face->vertex[ 2 ];
               br32 = vertices + face->vertex[ 3 ];

            /* mesh */

               l32->vx = ( tl32->vx + bl32->vx ) >> 1;
               l32->vy = ( tl32->vy + bl32->vy ) >> 1;
               l32->vz = ( tl32->vz + bl32->vz ) >> 1;

               b32->vx = ( bl32->vx + br32->vx ) >> 1;
               b32->vy = ( bl32->vy + br32->vy ) >> 1;
               b32->vz = ( bl32->vz + br32->vz ) >> 1;

               r32->vx = ( tr32->vx + br32->vx ) >> 1;
               r32->vy = ( tr32->vy + br32->vy ) >> 1;
               r32->vz = ( tr32->vz + br32->vz ) >> 1;

               t32->vx = ( tl32->vx + tr32->vx ) >> 1;
               t32->vy = ( tl32->vy + tr32->vy ) >> 1;
               t32->vz = ( tl32->vz + tr32->vz ) >> 1;

               c32->vx = ( tr32->vx + tl32->vx + bl32->vx + br32->vx ) >> 2;
               c32->vy = ( tr32->vy + tl32->vy + bl32->vy + br32->vy ) >> 2;
               c32->vz = ( tr32->vz + tl32->vz + bl32->vz + br32->vz ) >> 2;

            /* 4*4 specific stuff */

               tw32->vx = ( tl32->vx + t32->vx ) >> 1;
               tw32->vy = ( tl32->vy + t32->vy ) >> 1;
               tw32->vz = ( tl32->vz + t32->vz ) >> 1;
               
               te32->vx = ( t32->vx + tr32->vx ) >> 1;
               te32->vy = ( t32->vy + tr32->vy ) >> 1;
               te32->vz = ( t32->vz + tr32->vz ) >> 1;

               ln32->vx = ( tl32->vx + l32->vx ) >> 1;
               ln32->vy = ( tl32->vy + l32->vy ) >> 1;
               ln32->vz = ( tl32->vz + l32->vz ) >> 1;

               ls32->vx = ( l32->vx + bl32->vx ) >> 1;
               ls32->vy = ( l32->vy + bl32->vy ) >> 1;
               ls32->vz = ( l32->vz + bl32->vz ) >> 1;

               bw32->vx = ( bl32->vx + b32->vx ) >> 1;
               bw32->vy = ( bl32->vy + b32->vy ) >> 1;
               bw32->vz = ( bl32->vz + b32->vz ) >> 1;

               be32->vx = ( b32->vx + br32->vx ) >> 1;
               be32->vy = ( b32->vy + br32->vy ) >> 1;
               be32->vz = ( b32->vz + br32->vz ) >> 1;

               rn32->vx = ( tr32->vx + r32->vx ) >> 1;
               rn32->vy = ( tr32->vy + r32->vy ) >> 1;
               rn32->vz = ( tr32->vz + r32->vz ) >> 1;

               rs32->vx = ( r32->vx + br32->vx ) >> 1;
               rs32->vy = ( r32->vy + br32->vy ) >> 1;
               rs32->vz = ( r32->vz + br32->vz ) >> 1;


               ct32->vx = ( c32->vx + t32->vx ) >> 1;
               ct32->vy = ( c32->vy + t32->vy ) >> 1;
               ct32->vz = ( c32->vz + t32->vz ) >> 1;
               
               cr32->vx = ( c32->vx + r32->vx ) >> 1;
               cr32->vy = ( c32->vy + r32->vy ) >> 1;
               cr32->vz = ( c32->vz + r32->vz ) >> 1;

               cb32->vx = ( c32->vx + b32->vx ) >> 1;
               cb32->vy = ( c32->vy + b32->vy ) >> 1;
               cb32->vz = ( c32->vz + b32->vz ) >> 1;

               cl32->vx = ( c32->vx + l32->vx ) >> 1;
               cl32->vy = ( c32->vy + l32->vy ) >> 1;
               cl32->vz = ( c32->vz + l32->vz ) >> 1;


               ctl32->vx = ( ln32->vx + ct32->vx ) >> 1;
               ctl32->vy = ( ln32->vy + ct32->vy ) >> 1;
               ctl32->vz = ( ln32->vz + ct32->vz ) >> 1;

               ctr32->vx = ( ct32->vx + rn32->vx ) >> 1;
               ctr32->vy = ( ct32->vy + rn32->vy ) >> 1;
               ctr32->vz = ( ct32->vz + rn32->vz ) >> 1;

               cbl32->vx = ( ls32->vx + cb32->vx ) >> 1;
               cbl32->vy = ( ls32->vy + cb32->vy ) >> 1;
               cbl32->vz = ( ls32->vz + cb32->vz ) >> 1;

               cbr32->vx = ( cb32->vx + rs32->vx ) >> 1;
               cbr32->vy = ( cb32->vy + rs32->vy ) >> 1;
               cbr32->vz = ( cb32->vz + rs32->vz ) >> 1;

            /* make SVECTORs */

               c16->vx = c32->vx + campos->vx;
               c16->vy = c32->vy + campos->vy;
               c16->vz = c32->vz + campos->vz;

               l16->vx = l32->vx + campos->vx;
               l16->vy = l32->vy + campos->vy;
               l16->vz = l32->vz + campos->vz;

               tl16->vx = tl32->vx + campos->vx;
               tl16->vy = tl32->vy + campos->vy;
               tl16->vz = tl32->vz + campos->vz;

               t16->vx = t32->vx + campos->vx;
               t16->vy = t32->vy + campos->vy;
               t16->vz = t32->vz + campos->vz;

               tr16->vx = tr32->vx + campos->vx;
               tr16->vy = tr32->vy + campos->vy;
               tr16->vz = tr32->vz + campos->vz;

               r16->vx = r32->vx + campos->vx;
               r16->vy = r32->vy + campos->vy;
               r16->vz = r32->vz + campos->vz;

               br16->vx = br32->vx + campos->vx;
               br16->vy = br32->vy + campos->vy;
               br16->vz = br32->vz + campos->vz;

               b16->vx = b32->vx + campos->vx;
               b16->vy = b32->vy + campos->vy;
               b16->vz = b32->vz + campos->vz;

               bl16->vx = bl32->vx + campos->vx;
               bl16->vy = bl32->vy + campos->vy;
               bl16->vz = bl32->vz + campos->vz;


            /* 4*4 */

               tw16->vx = tw32->vx + campos->vx;
               tw16->vy = tw32->vy + campos->vy;
               tw16->vz = tw32->vz + campos->vz;
               
               te16->vx = te32->vx + campos->vx;
               te16->vy = te32->vy + campos->vy;
               te16->vz = te32->vz + campos->vz;

               ln16->vx = ln32->vx + campos->vx;
               ln16->vy = ln32->vy + campos->vy;
               ln16->vz = ln32->vz + campos->vz;

               ls16->vx = ls32->vx + campos->vx;
               ls16->vy = ls32->vy + campos->vy;
               ls16->vz = ls32->vz + campos->vz;

               bw16->vx = bw32->vx + campos->vx;
               bw16->vy = bw32->vy + campos->vy;
               bw16->vz = bw32->vz + campos->vz;

               be16->vx = be32->vx + campos->vx;
               be16->vy = be32->vy + campos->vy;
               be16->vz = be32->vz + campos->vz;

               rn16->vx = rn32->vx + campos->vx;
               rn16->vy = rn32->vy + campos->vy;
               rn16->vz = rn32->vz + campos->vz;

               rs16->vx = rs32->vx + campos->vx;
               rs16->vy = rs32->vy + campos->vy;
               rs16->vz = rs32->vz + campos->vz;


               ct16->vx = ct32->vx + campos->vx;
               ct16->vy = ct32->vy + campos->vy;
               ct16->vz = ct32->vz + campos->vz;
               
               cr16->vx = cr32->vx + campos->vx;
               cr16->vy = cr32->vy + campos->vy;
               cr16->vz = cr32->vz + campos->vz;

               cb16->vx = cb32->vx + campos->vx;
               cb16->vy = cb32->vy + campos->vy;
               cb16->vz = cb32->vz + campos->vz;

               cl16->vx = cl32->vx + campos->vx;
               cl16->vy = cl32->vy + campos->vy;
               cl16->vz = cl32->vz + campos->vz;


               ctl16->vx = ctl32->vx + campos->vx;
               ctl16->vy = ctl32->vy + campos->vy;
               ctl16->vz = ctl32->vz + campos->vz;

               ctr16->vx = ctr32->vx + campos->vx;
               ctr16->vy = ctr32->vy + campos->vy;
               ctr16->vz = ctr32->vz + campos->vz;

               cbl16->vx = cbl32->vx + campos->vx;
               cbl16->vy = cbl32->vy + campos->vy;
               cbl16->vz = cbl32->vz + campos->vz;

               cbr16->vx = cbr32->vx + campos->vx;
               cbr16->vy = cbr32->vy + campos->vy;
               cbr16->vz = cbr32->vz + campos->vz;



               hiDepth[ y + 2 ][ x + offset + 2 ] = RotTransPers
               (
                  c16,
                  &( hiScreen[ y + 2 ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 2 ][ x + offset + 2 ] )
               );

               hiDepth[ y + 2 ][ x + offset ] = RotTransPers
               (
                  l16,
                  &( hiScreen[ y + 2 ][ x + offset ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 2 ][ x + offset ] )
               );

               hiDepth[ y + 4 ][ x + offset ] = RotTransPers
               (
                  tl16,
                  &( hiScreen[ y + 4 ][ x + offset ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 4 ][ x + offset ] )
               );

               hiDepth[ y + 4 ][ x + offset + 2 ] = RotTransPers
               (
                  t16,
                  &( hiScreen[ y + 4 ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 4 ][ x + offset + 2 ] )
               );

               hiDepth[ y + 4 ][ x + offset + 4 ] = RotTransPers
               (
                  tr16,
                  &( hiScreen[ y + 4 ][ x + offset + 4 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 4 ][ x + offset + 4 ] )
               );

               hiDepth[ y + 2 ][ x + offset + 4 ] = RotTransPers
               (
                  r16,
                  &( hiScreen[ y + 2 ][ x + offset + 4 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 2 ][ x + offset + 4 ] )
               );

               hiDepth[ y ][ x + offset + 4 ] = RotTransPers
               (
                  br16,
                  &( hiScreen[ y ][ x + offset + 4 ] ),
                  &( interpolate ),
                  &( hiFlag[ y ][ x + offset + 4 ] )
               );

               hiDepth[ y ][ x + offset + 2 ] = RotTransPers
               (
                  b16,
                  &( hiScreen[ y ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( hiFlag[ y ][ x + offset + 2 ] )
               );

               hiDepth[ y ][ x + offset ] = RotTransPers
               (
                  bl16,
                  &( hiScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( hiFlag[ y ][ x + offset ] )
               );


            /* hi res */
               hiDepth[ y + 1 ][ x + offset ] = RotTransPers
               (
                  ls16,
                  &( hiScreen[ y + 1 ][ x + offset ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 1 ][ x + offset ] )
               );

               hiDepth[ y + 3 ][ x + offset ] = RotTransPers
               (
                  ln16,
                  &( hiScreen[ y + 3 ][ x + offset ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 3 ][ x + offset ] )
               );

               hiDepth[ y + 4 ][ x + offset + 1 ] = RotTransPers
               (
                  tw16,
                  &( hiScreen[ y + 4 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 4 ][ x + offset + 1 ] )
               );

               hiDepth[ y + 4 ][ x + offset + 3 ] = RotTransPers
               (
                  te16,
                  &( hiScreen[ y + 4 ][ x + offset + 3 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 4 ][ x + offset + 3 ] )
               );

               hiDepth[ y + 3 ][ x + offset + 4 ] = RotTransPers
               (
                  rn16,
                  &( hiScreen[ y + 3 ][ x + offset + 4 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 3 ][ x + offset + 4 ] )
               );

               hiDepth[ y + 1 ][ x + offset + 4 ] = RotTransPers
               (
                  rs16,
                  &( hiScreen[ y + 1 ][ x + offset + 4 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 1 ][ x + offset + 4 ] )
               );

               hiDepth[ y ][ x + offset + 3 ] = RotTransPers
               (
                  be16,
                  &( hiScreen[ y ][ x + offset + 3 ] ),
                  &( interpolate ),
                  &( hiFlag[ y ][ x + offset + 3 ] )
               );

               hiDepth[ y ][ x + offset + 1 ] = RotTransPers
               (
                  bw16,
                  &( hiScreen[ y ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( hiFlag[ y ][ x + offset + 1 ] )
               );


               hiDepth[ y + 2 ][ x + offset + 1 ] = RotTransPers
               (
                  cl16,
                  &( hiScreen[ y + 2 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 2 ][ x + offset + 1 ] )
               );

               hiDepth[ y + 3 ][ x + offset + 1 ] = RotTransPers
               (
                  ctl16,
                  &( hiScreen[ y + 3 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 3 ][ x + offset + 1 ] )
               );

               hiDepth[ y + 3 ][ x + offset + 2 ] = RotTransPers
               (
                  ct16,
                  &( hiScreen[ y + 3 ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 3 ][ x + offset + 2 ] )
               );

               hiDepth[ y + 3 ][ x + offset + 3 ] = RotTransPers
               (
                  ctr16,
                  &( hiScreen[ y + 3 ][ x + offset + 3 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 3 ][ x + offset + 3 ] )
               );

               hiDepth[ y + 2 ][ x + offset + 3 ] = RotTransPers
               (
                  cr16,
                  &( hiScreen[ y + 2 ][ x + offset + 3 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 2 ][ x + offset + 3 ] )
               );

               hiDepth[ y + 1 ][ x + offset + 3 ] = RotTransPers
               (
                  cbr16,
                  &( hiScreen[ y + 1 ][ x + offset + 3 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 1 ][ x + offset + 3 ] )
               );

               hiDepth[ y + 1 ][ x + offset + 2 ] = RotTransPers
               (
                  cb16,
                  &( hiScreen[ y + 1 ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 1 ][ x + offset + 2 ] )
               );

               hiDepth[ y + 1 ][ x + offset + 1 ] = RotTransPers
               (
                  cbl16,
                  &( hiScreen[ y + 1 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( hiFlag[ y + 1 ][ x + offset + 1 ] )
               );

            }
         }
         face++;

         x += 4;
      }

#if Jumps
      if ( section->flags & Jump )
      {
         y += 1;
      }
#endif
      section = NextSection( section );

      y += 4;
   }
#endif






/* Calculate medium meshed track sections */

#if MedRes

   y = 0;
   for ( i=0; i <= medCount; i++ )
   {
      offset = section->offset << 1;

      face = track->faces + section->faceStart;
      faceCount = section->faceCount - 1;

      x = 0;
   	for ( j=0; j <= faceCount; j++ )
      {              
#if Jumps
         if ( !( section->flags & MoreFaces ) && !( section->flags & Jump ) && ( i < medCount ) )
#else
         if ( !( section->flags & MoreFaces ) && ( i < medCount ) )
#endif
         {
         /* First sections */

            if ( j < faceCount )
            {
            /* First faces in first sections */

            /* get original 32 bit faces coords */

               tr32 = vertices + face->vertex[ 0 ];
               tl32 = vertices + face->vertex[ 1 ];
               bl32 = vertices + face->vertex[ 2 ];
               br32 = vertices + face->vertex[ 3 ];
            
            /* mesh */

               l32->vx = ( tl32->vx + bl32->vx ) >> 1;
               l32->vy = ( tl32->vy + bl32->vy ) >> 1;
               l32->vz = ( tl32->vz + bl32->vz ) >> 1;

               b32->vx = ( bl32->vx + br32->vx ) >> 1;
               b32->vy = ( bl32->vy + br32->vy ) >> 1;
               b32->vz = ( bl32->vz + br32->vz ) >> 1;

               c32->vx = ( tr32->vx + tl32->vx + bl32->vx + br32->vx ) >> 2;
               c32->vy = ( tr32->vy + tl32->vy + bl32->vy + br32->vy ) >> 2;
               c32->vz = ( tr32->vz + tl32->vz + bl32->vz + br32->vz ) >> 2;


            /* make SVECTORs */

               c16->vx = c32->vx + campos->vx;
               c16->vy = c32->vy + campos->vy;
               c16->vz = c32->vz + campos->vz;

               l16->vx = l32->vx + campos->vx;
               l16->vy = l32->vy + campos->vy;
               l16->vz = l32->vz + campos->vz;

               b16->vx = b32->vx + campos->vx;
               b16->vy = b32->vy + campos->vy;
               b16->vz = b32->vz + campos->vz;

               bl16->vx = bl32->vx + campos->vx;
               bl16->vy = bl32->vy + campos->vy;
               bl16->vz = bl32->vz + campos->vz;



               medDepth[ y + 1 ][ x + offset + 1 ] = RotTransPers
               (
                  c16,
                  &( medScreen[ y + 1 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset + 1 ] )
               );

               medDepth[ y + 1 ][ x + offset ] = RotTransPers
               (
                  l16,
                  &( medScreen[ y + 1 ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset ] )
               );

               medDepth[ y ][ x + offset + 1 ] = RotTransPers
               (
                  b16,
                  &( medScreen[ y ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset + 1 ] )
               );

               medDepth[ y ][ x + offset ] = RotTransPers
               (
                  bl16,
                  &( medScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset ] )
               );

            }
            else
            {
            /* Last face in first sections */

            /* get original 32 bit faces coords */

               tr32 = vertices + face->vertex[ 0 ];
               tl32 = vertices + face->vertex[ 1 ];
               bl32 = vertices + face->vertex[ 2 ];
               br32 = vertices + face->vertex[ 3 ];
            
            /* mesh */

               l32->vx = ( tl32->vx + bl32->vx ) >> 1;
               l32->vy = ( tl32->vy + bl32->vy ) >> 1;
               l32->vz = ( tl32->vz + bl32->vz ) >> 1;

               b32->vx = ( bl32->vx + br32->vx ) >> 1;
               b32->vy = ( bl32->vy + br32->vy ) >> 1;
               b32->vz = ( bl32->vz + br32->vz ) >> 1;

               r32->vx = ( tr32->vx + br32->vx ) >> 1;
               r32->vy = ( tr32->vy + br32->vy ) >> 1;
               r32->vz = ( tr32->vz + br32->vz ) >> 1;

               c32->vx = ( tr32->vx + tl32->vx + bl32->vx + br32->vx ) >> 2;
               c32->vy = ( tr32->vy + tl32->vy + bl32->vy + br32->vy ) >> 2;
               c32->vz = ( tr32->vz + tl32->vz + bl32->vz + br32->vz ) >> 2;


            /* make SVECTORs */

               c16->vx = c32->vx + campos->vx;
               c16->vy = c32->vy + campos->vy;
               c16->vz = c32->vz + campos->vz;

               l16->vx = l32->vx + campos->vx;
               l16->vy = l32->vy + campos->vy;
               l16->vz = l32->vz + campos->vz;

               r16->vx = r32->vx + campos->vx;
               r16->vy = r32->vy + campos->vy;
               r16->vz = r32->vz + campos->vz;

               br16->vx = br32->vx + campos->vx;
               br16->vy = br32->vy + campos->vy;
               br16->vz = br32->vz + campos->vz;

               b16->vx = b32->vx + campos->vx;
               b16->vy = b32->vy + campos->vy;
               b16->vz = b32->vz + campos->vz;

               bl16->vx = bl32->vx + campos->vx;
               bl16->vy = bl32->vy + campos->vy;
               bl16->vz = bl32->vz + campos->vz;



               medDepth[ y + 1 ][ x + offset + 1 ] = RotTransPers
               (
                  c16,
                  &( medScreen[ y + 1 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset + 1 ] )
               );

               medDepth[ y + 1 ][ x + offset ] = RotTransPers
               (
                  l16,
                  &( medScreen[ y + 1 ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset ] )
               );

               medDepth[ y + 1 ][ x + offset + 2 ] = RotTransPers
               (
                  r16,
                  &( medScreen[ y + 1 ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset + 2 ] )
               );

               medDepth[ y ][ x + offset + 2 ] = RotTransPers
               (
                  br16,
                  &( medScreen[ y ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset + 2 ] )
               );

               medDepth[ y ][ x + offset + 1 ] = RotTransPers
               (
                  b16,
                  &( medScreen[ y ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset + 1 ] )
               );

               medDepth[ y ][ x + offset ] = RotTransPers
               (
                  bl16,
                  &( medScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset ] )
               );

            }         
         }
         else
         {
         /* Last section */

            if ( j < faceCount )
            {
            /* First faces in last section */

            /* get original 32 bit faces coords */

               tr32 = vertices + face->vertex[ 0 ];
               tl32 = vertices + face->vertex[ 1 ];
               bl32 = vertices + face->vertex[ 2 ];
               br32 = vertices + face->vertex[ 3 ];
            
            /* mesh */

               l32->vx = ( tl32->vx + bl32->vx ) >> 1;
               l32->vy = ( tl32->vy + bl32->vy ) >> 1;
               l32->vz = ( tl32->vz + bl32->vz ) >> 1;

               b32->vx = ( bl32->vx + br32->vx ) >> 1;
               b32->vy = ( bl32->vy + br32->vy ) >> 1;
               b32->vz = ( bl32->vz + br32->vz ) >> 1;

               t32->vx = ( tl32->vx + tr32->vx ) >> 1;
               t32->vy = ( tl32->vy + tr32->vy ) >> 1;
               t32->vz = ( tl32->vz + tr32->vz ) >> 1;

               c32->vx = ( tr32->vx + tl32->vx + bl32->vx + br32->vx ) >> 2;
               c32->vy = ( tr32->vy + tl32->vy + bl32->vy + br32->vy ) >> 2;
               c32->vz = ( tr32->vz + tl32->vz + bl32->vz + br32->vz ) >> 2;


            /* make SVECTORs */

               c16->vx = c32->vx + campos->vx;
               c16->vy = c32->vy + campos->vy;
               c16->vz = c32->vz + campos->vz;

               l16->vx = l32->vx + campos->vx;
               l16->vy = l32->vy + campos->vy;
               l16->vz = l32->vz + campos->vz;

               tl16->vx = tl32->vx + campos->vx;
               tl16->vy = tl32->vy + campos->vy;
               tl16->vz = tl32->vz + campos->vz;

               t16->vx = t32->vx + campos->vx;
               t16->vy = t32->vy + campos->vy;
               t16->vz = t32->vz + campos->vz;

               b16->vx = b32->vx + campos->vx;
               b16->vy = b32->vy + campos->vy;
               b16->vz = b32->vz + campos->vz;

               bl16->vx = bl32->vx + campos->vx;
               bl16->vy = bl32->vy + campos->vy;
               bl16->vz = bl32->vz + campos->vz;



               medDepth[ y + 1 ][ x + offset + 1 ] = RotTransPers
               (
                  c16,
                  &( medScreen[ y + 1 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset + 1 ] )
               );

               medDepth[ y + 1 ][ x + offset ] = RotTransPers
               (
                  l16,
                  &( medScreen[ y + 1 ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset ] )
               );

               medDepth[ y + 2 ][ x + offset ] = RotTransPers
               (
                  tl16,
                  &( medScreen[ y + 2 ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y + 2 ][ x + offset ] )
               );

               medDepth[ y + 2 ][ x + offset + 1 ] = RotTransPers
               (
                  t16,
                  &( medScreen[ y + 2 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 2 ][ x + offset + 1 ] )
               );

               medDepth[ y ][ x + offset + 1 ] = RotTransPers
               (
                  b16,
                  &( medScreen[ y ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset + 1 ] )
               );

               medDepth[ y ][ x + offset ] = RotTransPers
               (
                  bl16,
                  &( medScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset ] )
               );

            }
            else
            {
            /* Last faces in last section */

            /* get original 32 bit faces coords */

               tr32 = vertices + face->vertex[ 0 ];
               tl32 = vertices + face->vertex[ 1 ];
               bl32 = vertices + face->vertex[ 2 ];
               br32 = vertices + face->vertex[ 3 ];
            
            /* mesh */

               l32->vx = ( tl32->vx + bl32->vx ) >> 1;
               l32->vy = ( tl32->vy + bl32->vy ) >> 1;
               l32->vz = ( tl32->vz + bl32->vz ) >> 1;

               b32->vx = ( bl32->vx + br32->vx ) >> 1;
               b32->vy = ( bl32->vy + br32->vy ) >> 1;
               b32->vz = ( bl32->vz + br32->vz ) >> 1;

               r32->vx = ( tr32->vx + br32->vx ) >> 1;
               r32->vy = ( tr32->vy + br32->vy ) >> 1;
               r32->vz = ( tr32->vz + br32->vz ) >> 1;

               t32->vx = ( tl32->vx + tr32->vx ) >> 1;
               t32->vy = ( tl32->vy + tr32->vy ) >> 1;
               t32->vz = ( tl32->vz + tr32->vz ) >> 1;

               c32->vx = ( tr32->vx + tl32->vx + bl32->vx + br32->vx ) >> 2;
               c32->vy = ( tr32->vy + tl32->vy + bl32->vy + br32->vy ) >> 2;
               c32->vz = ( tr32->vz + tl32->vz + bl32->vz + br32->vz ) >> 2;


            /* make SVECTORs */

               c16->vx = c32->vx + campos->vx;
               c16->vy = c32->vy + campos->vy;
               c16->vz = c32->vz + campos->vz;

               l16->vx = l32->vx + campos->vx;
               l16->vy = l32->vy + campos->vy;
               l16->vz = l32->vz + campos->vz;

               tl16->vx = tl32->vx + campos->vx;
               tl16->vy = tl32->vy + campos->vy;
               tl16->vz = tl32->vz + campos->vz;

               t16->vx = t32->vx + campos->vx;
               t16->vy = t32->vy + campos->vy;
               t16->vz = t32->vz + campos->vz;

               tr16->vx = tr32->vx + campos->vx;
               tr16->vy = tr32->vy + campos->vy;
               tr16->vz = tr32->vz + campos->vz;

               r16->vx = r32->vx + campos->vx;
               r16->vy = r32->vy + campos->vy;
               r16->vz = r32->vz + campos->vz;

               br16->vx = br32->vx + campos->vx;
               br16->vy = br32->vy + campos->vy;
               br16->vz = br32->vz + campos->vz;

               b16->vx = b32->vx + campos->vx;
               b16->vy = b32->vy + campos->vy;
               b16->vz = b32->vz + campos->vz;

               bl16->vx = bl32->vx + campos->vx;
               bl16->vy = bl32->vy + campos->vy;
               bl16->vz = bl32->vz + campos->vz;



               medDepth[ y + 1 ][ x + offset + 1 ] = RotTransPers
               (
                  c16,
                  &( medScreen[ y + 1 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset + 1 ] )
               );

               medDepth[ y + 1 ][ x + offset ] = RotTransPers
               (
                  l16,
                  &( medScreen[ y + 1 ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset ] )
               );

               medDepth[ y + 2 ][ x + offset ] = RotTransPers
               (
                  tl16,
                  &( medScreen[ y + 2 ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y + 2 ][ x + offset ] )
               );

               medDepth[ y + 2 ][ x + offset + 1 ] = RotTransPers
               (
                  t16,
                  &( medScreen[ y + 2 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 2 ][ x + offset + 1 ] )
               );

               medDepth[ y + 2 ][ x + offset + 2 ] = RotTransPers
               (
                  tr16,
                  &( medScreen[ y + 2 ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 2 ][ x + offset + 2 ] )
               );

               medDepth[ y + 1 ][ x + offset + 2 ] = RotTransPers
               (
                  r16,
                  &( medScreen[ y + 1 ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( medFlag[ y + 1 ][ x + offset + 2 ] )
               );

               medDepth[ y ][ x + offset + 2 ] = RotTransPers
               (
                  br16,
                  &( medScreen[ y ][ x + offset + 2 ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset + 2 ] )
               );

               medDepth[ y ][ x + offset + 1 ] = RotTransPers
               (
                  b16,
                  &( medScreen[ y ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset + 1 ] )
               );

               medDepth[ y ][ x + offset ] = RotTransPers
               (
                  bl16,
                  &( medScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( medFlag[ y ][ x + offset ] )
               );
            }
         }

         face++;

         x += 2;
      }

#if Jumps
      if ( section->flags & Jump )
      {
         y += 1;
      }
#endif

      section = NextSection( section );

      y += 2;
   }
#endif




/* Calculate non-meshed track sections */
   

#if LoRes

   y = 0;
	for ( i=0; i <= loCount; i++ )
   {
      offset = section->offset;

      face = track->faces + section->faceStart;
      faceCount = section->faceCount - 1;

      x = 0;
   	for ( j=0; j <= faceCount; j++ )
      {
#if Jumps
         if ( !( section->flags & MoreFaces ) && !( section->flags & Jump ) && ( i < loCount ) )
#else
         if ( !( section->flags & MoreFaces ) && ( i < loCount ) )
#endif
         {
         /* First sections */

            if ( j < faceCount )
            {
            /* First faces in first sections */

               vector = vertices + face->vertex[ 2 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y ][ x + offset ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( loFlag[ y ][ x + offset ] )
               );
            }
            else
            {
            /* Last face in first sections */

               vector = vertices + face->vertex[ 2 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y ][ x + offset ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( loFlag[ y ][ x + offset ] )
               );

               vector = vertices + face->vertex[ 3 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y ][ x + offset + 1 ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( loFlag[ y ][ x + offset + 1 ] )
               );
            }
         }
         else
         {
         /* Last section */

            if ( j < faceCount )
            {
            /* First faces in last secion we do 2 points */

               vector = vertices + face->vertex[ 1 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y + 1 ][ x + offset ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y + 1 ][ x + offset ] ),
                  &( interpolate ),
                  &( loFlag[ y + 1 ][ x + offset ] )
               );

               vector = vertices + face->vertex[ 2 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y ][ x + offset ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( loFlag[ y ][ x + offset ] )
               );
            }
            else
            {
            /* Last face in last section we do all 4 points */

               vector = vertices + face->vertex[ 0 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y + 1 ][ x + offset + 1 ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y + 1 ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( loFlag[ y + 1 ][ x + offset + 1 ] )
               );

               vector = vertices + face->vertex[ 1 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y + 1 ][ x + offset ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y + 1 ][ x + offset ] ),
                  &( interpolate ),
                  &( loFlag[ y + 1 ][ x + offset ] )
               );

               vector = vertices + face->vertex[ 2 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y ][ x + offset ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y ][ x + offset ] ),
                  &( interpolate ),
                  &( loFlag[ y ][ x + offset ] )
               );

               vector = vertices + face->vertex[ 3 ];
               svector->vx = vector->vx + campos->vx;
               svector->vy = vector->vy + campos->vy;
               svector->vz = vector->vz + campos->vz;
               loDepth[ y ][ x + offset + 1 ] = RotTransPers
               (
                  svector,
                  &( loScreen[ y ][ x + offset + 1 ] ),
                  &( interpolate ),
                  &( loFlag[ y ][ x + offset + 1 ] )
               );
            }
         }

         face++;

         x += 1;
      }

#if Jumps
      if ( section->flags & Jump )
      {
         y += 1;
      }
#endif
      section = NextSection( section );

      y += 1;
   }

#endif




/* **DRAW** */


/* Draw Hi res meshed track sections */

   section = firstSection;

#if HiRes

   y = 0;
   for ( i=0; i <= hiCount; i++ )
   {
      offset = section->offset << 2;

      face = track->faces + section->faceStart;
      faceCount = section->faceCount - 1;

      x = 0;
   	for ( j=0; j <= faceCount; j++ )
      {
         if ( face->flags & FlipTexture )
         {
            object = GetTileObject( tileLib, face->texture, 2, 1 );
         }
         else
         {
            object = GetTileObject( tileLib, face->texture, 2, 0 );
         }

         /* set the light */
         p = (FT4*)object->primitives;

         for ( k=0; k<16; k++ )
         {
            p->quad[ 0 ].r0 = face->red;
            p->quad[ 0 ].g0 = face->green;
            p->quad[ 0 ].b0 = face->blue;

            p->quad[ 1 ].r0 = face->red;
            p->quad[ 1 ].g0 = face->green;
            p->quad[ 1 ].b0 = face->blue;

            p++;
         }

         poly.prim = object->primitives;

      /* 1st row */
         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 4 ][ x + offset ];
         quad->xy1 = hiScreen[ y + 4 ][ x + offset + 1 ];
         quad->xy2 = hiScreen[ y + 3 ][ x + offset ];
         quad->xy3 = hiScreen[ y + 3 ][ x + offset + 1 ];
         if ( !( ( hiFlag[ y + 4 ][ x + offset ] | 
                   hiFlag[ y + 4 ][ x + offset + 1 ] | 
                   hiFlag[ y + 3 ][ x + offset ] | 
                   hiFlag[ y + 3 ][ x + offset + 1 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
         /*
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 4 ][ x + offset ], ( ulong* ) quad );
         */
            AddPrim( OT[ CurrentScreen ] + 
                  ( ( hiDepth[ y + 4 ][ x + offset ] +
                      hiDepth[ y + 4 ][ x + offset + 1 ] +
                      hiDepth[ y + 3 ][ x + offset ] +
                      hiDepth[ y + 3 ][ x + offset + 1 ] ) >> 2 ),
               ( ulong* ) quad );


         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 4 ][ x + offset + 1 ];
         quad->xy1 = hiScreen[ y + 4 ][ x + offset + 2 ];
         quad->xy2 = hiScreen[ y + 3 ][ x + offset + 1 ];
         quad->xy3 = hiScreen[ y + 3 ][ x + offset + 2 ];
         if ( !( ( hiFlag[ y + 4 ][ x + offset + 1 ] | 
                   hiFlag[ y + 4 ][ x + offset + 2 ] | 
                   hiFlag[ y + 3 ][ x + offset + 1 ] | 
                   hiFlag[ y + 3 ][ x + offset + 2 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 4 ][ x + offset + 1 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 4 ][ x + offset + 2 ];
         quad->xy1 = hiScreen[ y + 4 ][ x + offset + 3 ];
         quad->xy2 = hiScreen[ y + 3 ][ x + offset + 2 ];
         quad->xy3 = hiScreen[ y + 3 ][ x + offset + 3 ];
         if ( !( ( hiFlag[ y + 4 ][ x + offset + 2 ] | 
                   hiFlag[ y + 4 ][ x + offset + 3 ] | 
                   hiFlag[ y + 3 ][ x + offset + 2 ] | 
                   hiFlag[ y + 3 ][ x + offset + 3 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 4 ][ x + offset + 2 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 4 ][ x + offset + 3 ];
         quad->xy1 = hiScreen[ y + 4 ][ x + offset + 4 ];
         quad->xy2 = hiScreen[ y + 3 ][ x + offset + 3 ];
         quad->xy3 = hiScreen[ y + 3 ][ x + offset + 4 ];
         if ( !( ( hiFlag[ y + 4 ][ x + offset + 3 ] | 
                   hiFlag[ y + 4 ][ x + offset + 4 ] | 
                   hiFlag[ y + 3 ][ x + offset + 3 ] | 
                   hiFlag[ y + 3 ][ x + offset + 4 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 4 ][ x + offset + 3 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


      /* 2nd row */
         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 3 ][ x + offset ];
         quad->xy1 = hiScreen[ y + 3 ][ x + offset + 1 ];
         quad->xy2 = hiScreen[ y + 2 ][ x + offset ];
         quad->xy3 = hiScreen[ y + 2 ][ x + offset + 1 ];
         if ( !( ( hiFlag[ y + 3 ][ x + offset ] | 
                   hiFlag[ y + 3 ][ x + offset + 1 ] | 
                   hiFlag[ y + 2 ][ x + offset ] | 
                   hiFlag[ y + 2 ][ x + offset + 1 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 3 ][ x + offset ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 3 ][ x + offset + 1 ];
         quad->xy1 = hiScreen[ y + 3 ][ x + offset + 2 ];
         quad->xy2 = hiScreen[ y + 2 ][ x + offset + 1 ];
         quad->xy3 = hiScreen[ y + 2 ][ x + offset + 2 ];
         if ( !( ( hiFlag[ y + 3 ][ x + offset + 1 ] | 
                   hiFlag[ y + 3 ][ x + offset + 2 ] | 
                   hiFlag[ y + 2 ][ x + offset + 1 ] | 
                   hiFlag[ y + 2 ][ x + offset + 2 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 3 ][ x + offset + 1 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 3 ][ x + offset + 2 ];
         quad->xy1 = hiScreen[ y + 3 ][ x + offset + 3 ];
         quad->xy2 = hiScreen[ y + 2 ][ x + offset + 2 ];
         quad->xy3 = hiScreen[ y + 2 ][ x + offset + 3 ];
         if ( !( ( hiFlag[ y + 3 ][ x + offset + 2 ] | 
                   hiFlag[ y + 3 ][ x + offset + 3 ] | 
                   hiFlag[ y + 2 ][ x + offset + 2 ] | 
                   hiFlag[ y + 2 ][ x + offset + 3 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 3 ][ x + offset + 2 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 3 ][ x + offset + 3 ];
         quad->xy1 = hiScreen[ y + 3 ][ x + offset + 4 ];
         quad->xy2 = hiScreen[ y + 2 ][ x + offset + 3 ];
         quad->xy3 = hiScreen[ y + 2 ][ x + offset + 4 ];
         if ( !( ( hiFlag[ y + 3 ][ x + offset + 3 ] | 
                   hiFlag[ y + 3 ][ x + offset + 4 ] | 
                   hiFlag[ y + 2 ][ x + offset + 3 ] | 
                   hiFlag[ y + 2 ][ x + offset + 4 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 3 ][ x + offset + 3 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;



      /* 3rd row */
         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 2 ][ x + offset ];
         quad->xy1 = hiScreen[ y + 2 ][ x + offset + 1 ];
         quad->xy2 = hiScreen[ y + 1 ][ x + offset ];
         quad->xy3 = hiScreen[ y + 1 ][ x + offset + 1 ];
         if ( !( ( hiFlag[ y + 2 ][ x + offset ] | 
                   hiFlag[ y + 2 ][ x + offset + 1 ] | 
                   hiFlag[ y + 1 ][ x + offset ] | 
                   hiFlag[ y + 1 ][ x + offset + 1 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 2 ][ x + offset ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 2 ][ x + offset + 1 ];
         quad->xy1 = hiScreen[ y + 2 ][ x + offset + 2 ];
         quad->xy2 = hiScreen[ y + 1 ][ x + offset + 1 ];
         quad->xy3 = hiScreen[ y + 1 ][ x + offset + 2 ];
         if ( !( ( hiFlag[ y + 2 ][ x + offset + 1 ] | 
                   hiFlag[ y + 2 ][ x + offset + 2 ] | 
                   hiFlag[ y + 1 ][ x + offset + 1 ] | 
                   hiFlag[ y + 1 ][ x + offset + 2 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 2 ][ x + offset + 1 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 2 ][ x + offset + 2 ];
         quad->xy1 = hiScreen[ y + 2 ][ x + offset + 3 ];
         quad->xy2 = hiScreen[ y + 1 ][ x + offset + 2 ];
         quad->xy3 = hiScreen[ y + 1 ][ x + offset + 3 ];
         if ( !( ( hiFlag[ y + 2 ][ x + offset + 2 ] | 
                   hiFlag[ y + 2 ][ x + offset + 3 ] | 
                   hiFlag[ y + 1 ][ x + offset + 2 ] | 
                   hiFlag[ y + 1 ][ x + offset + 3 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 2 ][ x + offset + 2 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 2 ][ x + offset + 3 ];
         quad->xy1 = hiScreen[ y + 2 ][ x + offset + 4 ];
         quad->xy2 = hiScreen[ y + 1 ][ x + offset + 3 ];
         quad->xy3 = hiScreen[ y + 1 ][ x + offset + 4 ];
         if ( !( ( hiFlag[ y + 2 ][ x + offset + 3 ] | 
                   hiFlag[ y + 2 ][ x + offset + 4 ] | 
                   hiFlag[ y + 1 ][ x + offset + 3 ] | 
                   hiFlag[ y + 1 ][ x + offset + 4 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 2 ][ x + offset + 3 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;



      /* 4th row */
         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 1 ][ x + offset ];
         quad->xy1 = hiScreen[ y + 1 ][ x + offset + 1 ];
         quad->xy2 = hiScreen[ y + 0 ][ x + offset ];
         quad->xy3 = hiScreen[ y + 0 ][ x + offset + 1 ];
         if ( !( ( hiFlag[ y + 1 ][ x + offset ] | 
                   hiFlag[ y + 1 ][ x + offset + 1 ] | 
                   hiFlag[ y + 0 ][ x + offset ] | 
                   hiFlag[ y + 0 ][ x + offset + 1 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 1 ][ x + offset ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 1 ][ x + offset + 1 ];
         quad->xy1 = hiScreen[ y + 1 ][ x + offset + 2 ];
         quad->xy2 = hiScreen[ y + 0 ][ x + offset + 1 ];
         quad->xy3 = hiScreen[ y + 0 ][ x + offset + 2 ];
         if ( !( ( hiFlag[ y + 1 ][ x + offset + 1 ] | 
                   hiFlag[ y + 1 ][ x + offset + 2 ] | 
                   hiFlag[ y + 0 ][ x + offset + 1 ] | 
                   hiFlag[ y + 0 ][ x + offset + 2 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 1 ][ x + offset + 1 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 1 ][ x + offset + 2 ];
         quad->xy1 = hiScreen[ y + 1 ][ x + offset + 3 ];
         quad->xy2 = hiScreen[ y + 0 ][ x + offset + 2 ];
         quad->xy3 = hiScreen[ y + 0 ][ x + offset + 3 ];
         if ( !( ( hiFlag[ y + 1 ][ x + offset + 2 ] | 
                   hiFlag[ y + 1 ][ x + offset + 3 ] | 
                   hiFlag[ y + 0 ][ x + offset + 2 ] | 
                   hiFlag[ y + 0 ][ x + offset + 3 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 1 ][ x + offset + 2 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = hiScreen[ y + 1 ][ x + offset + 3 ];
         quad->xy1 = hiScreen[ y + 1 ][ x + offset + 4 ];
         quad->xy2 = hiScreen[ y + 0 ][ x + offset + 3 ];
         quad->xy3 = hiScreen[ y + 0 ][ x + offset + 4 ];
         if ( !( ( hiFlag[ y + 1 ][ x + offset + 3 ] | 
                   hiFlag[ y + 1 ][ x + offset + 4 ] | 
                   hiFlag[ y + 0 ][ x + offset + 3 ] | 
                   hiFlag[ y + 0 ][ x + offset + 4 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) 
            )
         {
            AddPrim( OT[ CurrentScreen ] + hiDepth[ y + 1 ][ x + offset + 3 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;


         face++;

         x += 4;
      }

#if Jumps
      if ( section->flags & Jump )
      {
         y += 1;
      }
#endif

      section = NextSection( section );

      y += 4;
   }

#endif



   
   
/* Draw medium meshed track sections */

#if MedRes

   y = 0;
   for ( i=0; i <= medCount; i++ )
   {
      offset = section->offset << 1;

      face = track->faces + section->faceStart;
      faceCount = section->faceCount - 1;

      x = 0;
   	for ( j=0; j <= faceCount; j++ )
      {
         if ( face->flags & FlipTexture )
         {
            object = GetTileObject( tileLib, face->texture, 1, 1 );
         }
         else
         {
            object = GetTileObject( tileLib, face->texture, 1, 0 );
         }
         /* set the light */
         p = (FT4*)object->primitives;

         for ( k=0; k<4; k++ )
         {
            p->quad[ 0 ].r0 = face->red;
            p->quad[ 0 ].g0 = face->green;
            p->quad[ 0 ].b0 = face->blue;

            p->quad[ 1 ].r0 = face->red;
            p->quad[ 1 ].g0 = face->green;
            p->quad[ 1 ].b0 = face->blue;

            p++;
         }

         poly.prim = object->primitives;

         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = medScreen[ y + 1 ][ x + offset ];
         quad->xy1 = medScreen[ y + 1 ][ x + offset + 1 ];
         quad->xy2 = medScreen[ y ][ x + offset ];
         quad->xy3 = medScreen[ y ][ x + offset + 1 ];
         if ( !( ( medFlag[ y ][ x + offset ] | 
                   medFlag[ y ][ x + offset + 1 ] | 
                   medFlag[ y + 1 ][ x + offset ] | 
                   medFlag[ y + 1 ][ x + offset + 1 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) )
         {
            AddPrim( OT[ CurrentScreen ] + medDepth[ y ][ x + offset ], ( ulong* ) quad );
         }
         poly.ft4 += 1;
         
         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = medScreen[ y + 1 ][ x + offset + 1];
         quad->xy1 = medScreen[ y + 1 ][ x + offset + 2 ];
         quad->xy2 = medScreen[ y ][ x + offset + 1];
         quad->xy3 = medScreen[ y ][ x + offset + 2 ];
         if ( !( ( medFlag[ y ][ x + offset + 1] | 
                   medFlag[ y ][ x + offset + 2 ] | 
                   medFlag[ y + 1 ][ x + offset + 1 ] | 
                   medFlag[ y + 1 ][ x + offset + 2 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) )
         {
            AddPrim( OT[ CurrentScreen ] + medDepth[ y ][ x + offset + 1 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;
         
         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = medScreen[ y + 2 ][ x + offset ];
         quad->xy1 = medScreen[ y + 2 ][ x + offset + 1 ];
         quad->xy2 = medScreen[ y + 1 ][ x + offset ];
         quad->xy3 = medScreen[ y + 1 ][ x + offset + 1 ];
         if ( !( ( medFlag[ y + 1 ][ x + offset ] | 
                   medFlag[ y + 1 ][ x + offset + 1 ] | 
                   medFlag[ y + 2 ][ x + offset ] | 
                   medFlag[ y + 2 ][ x + offset + 1 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) )
         {
            AddPrim( OT[ CurrentScreen ] + medDepth[ y + 1 ][ x + offset ], ( ulong* ) quad );
         }
         poly.ft4 += 1;
         
         quad = &( poly.ft4->quad[ CurrentScreen ] );
         quad->xy0 = medScreen[ y + 2 ][ x + offset + 1 ];
         quad->xy1 = medScreen[ y + 2 ][ x + offset + 2 ];
         quad->xy2 = medScreen[ y + 1 ][ x + offset + 1 ];
         quad->xy3 = medScreen[ y + 1 ][ x + offset + 2 ];
         if ( !( ( medFlag[ y + 1 ][ x + offset + 1 ] | 
                   medFlag[ y + 1 ][ x + offset + 2 ] | 
                   medFlag[ y + 2 ][ x + offset + 1 ] | 
                   medFlag[ y + 2 ][ x + offset + 2 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) )
         {
            AddPrim( OT[ CurrentScreen ] + medDepth[ y + 1 ][ x + offset + 1 ], ( ulong* ) quad );
         }
         poly.ft4 += 1;
         
         face++;

         x += 2;
      }

#if Jumps
      if ( section->flags & Jump )
      {
         y += 1;
      }
#endif

      section = NextSection( section );

      y += 2;
   }

#endif


#if LoRes

/* Draw non-meshed track sections */

   y = 0;
	for ( i=0; i <= loCount; i++ )
   {
      offset = section->offset;

      face = track->faces + section->faceStart;
      faceCount = section->faceCount - 1;

      x = 0;
   	for ( j=0; j <= faceCount; j++ )
      {
         if ( face->flags & FlipTexture )
         {
            object = GetTileObject( tileLib, face->texture, 0, 1 );
         }
         else
         {
            object = GetTileObject( tileLib, face->texture, 0, 0 );
         }

         /* set the light */
         p = (FT4*)object->primitives;

         p->quad[ 0 ].r0 = face->red;
         p->quad[ 0 ].g0 = face->green;
         p->quad[ 0 ].b0 = face->blue;

         p->quad[ 1 ].r0 = face->red;
         p->quad[ 1 ].g0 = face->green;
         p->quad[ 1 ].b0 = face->blue;


         poly.prim = object->primitives;

         quad = &( poly.ft4->quad[ CurrentScreen ] );
         
         quad->xy0 = loScreen[ y + 1 ][ x + offset ];
         quad->xy1 = loScreen[ y + 1 ][ x + offset + 1 ];
         quad->xy2 = loScreen[ y ][ x + offset ];
         quad->xy3 = loScreen[ y ][ x + offset + 1 ];

         if ( !( ( loFlag[ y ][ x + offset ] | 
                   loFlag[ y ][ x + offset + 1 ] | 
                   loFlag[ y + 1 ][ x + offset ] | 
                   loFlag[ y + 1 ][ x + offset + 1 ] ) &
               ( CrtClipped | ZNegative | ZLarge | ZSmall) ) )
         {
#if AccurateDepth
            AddPrim( OT[ CurrentScreen ] + 
                  ( ( loDepth[ y ][ x + offset ] +
                      loDepth[ y ][ x + offset + 1 ] +
                      loDepth[ y + 1 ][ x + offset ] +
                      loDepth[ y + 1 ][ x + offset + 1 ] ) >> 2 ),
               ( ulong* ) quad );
#else
            AddPrim( OT[ CurrentScreen ] + loDepth[ y ][ x + offset ], ( ulong* ) quad );
#endif
         }

         face++;

         x += 1;
      }

#if Jumps
      if ( section->flags & Jump )
      {
         y += 1;
      }
#endif

      section = NextSection( section );

      y += 1;
   }

#endif


}


#endif








#define LoDistance ( 32767 - 4096 )
#define MedDistance 12276
#define HiDistance 4095

#define MaxLoList 256
#define MaxMedList 128
#define MaxHiList 64


/* ****************************************************
   TRACK RADIUS STUFF
   **************************************************** */






void sort ( short *numbers, short count )
{
   short       i, j;
   short       temp;


   for ( i=0; i<count-1; i++ )
   {
      if ( numbers[ i ] > numbers[ i+1 ] )
      {
         printf( "out of order ************\n" );
      }
   }


   printf( "before " );
   for ( i=0; i<count; i++ )
   {
      printf( "%3d ", numbers[ i ] );
   }
   printf( "\n" );

   for ( i=0; i<count; i++ )
   {
      for ( j=i+1; j<count; j++ )
      {
         if ( numbers[ i ] > numbers[ j ] )
         {
            temp = numbers[ j ];
            numbers[ j ] = numbers[ i ];
            numbers[ i ] = temp;
         }
      }
   }

   printf( "after  " );
   for ( i=0; i<count; i++ )
   {
      printf( "%3d ", numbers[ i ] );
   }
   printf( "\n" );
}




void TrackRadiusCheck( )
{
   TrackSection*  sectionA;
   TrackSection*  sectionB;
	Vector*       	sectionAcentre;
	Vector*       	sectionBcentre;
	VECTOR       	vector;
   short          i, j, k;

   short          loViewCount;
   short          loTrackList[ MaxLoList ];      

   short          medViewCount;
   short          medTrackList[ MaxMedList ];      

   short          hiViewCount;
   short          hiTrackList[ MaxHiList ];      

   short          tempTrackList[ MaxLoList ];      


   short          maxLo = 0;
   short          maxMed = 0;
   short          maxHi = 0;

   short          avLo = 0;
   short          avMed = 0;
   short          avHi = 0;

   sectionA = track->sections;
   sectionB = track->sections;

   for ( i=0; i<track->sectionCount; i++ )
   {

#if 1
   /* build up a lo res list */

      loViewCount = 0;
      sectionAcentre = &sectionA->centre;

      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < LoDistance ) &&
              ( abs( vector.vy ) < LoDistance ) &&
              ( abs( vector.vz ) < LoDistance ) )
         {
            if ( loViewCount >= MaxLoList )
            {
               Error( "radius.c::TrackRadiusCheck: too many lo tiles", Fatal );
            }

            loTrackList[ loViewCount ] = j;
            loViewCount += 1;
         }
         sectionB = NextIndexSection( sectionB );
      }
#endif


#if 1
   /* build up a med res list */

      medViewCount = 0;
      sectionAcentre = &sectionA->centre;

      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < MedDistance ) &&
              ( abs( vector.vy ) < MedDistance ) &&
              ( abs( vector.vz ) < MedDistance ) )
         {
            if ( medViewCount >= MaxMedList )
            {
               Error( "radius.c::TrackRadiusCheck: too many medium tiles", Fatal );
            }

            medTrackList[ medViewCount ] = j;
            medViewCount += 1;
         }
         sectionB = NextIndexSection( sectionB );
      }
#endif



#if 1
   /* build up a hi res list */

      hiViewCount = 0;
      sectionAcentre = &sectionA->centre;

      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < HiDistance ) &&
              ( abs( vector.vy ) < HiDistance ) &&
              ( abs( vector.vz ) < HiDistance ) )
         {
            if ( hiViewCount >= MaxHiList )
            {
               Error( "radius.c::TrackRadiusCheck: too many hi tiles", Fatal );
            }

            hiTrackList[ hiViewCount ] = j;
            hiViewCount += 1;
         }
         sectionB = NextIndexSection( sectionB );
      }
#endif

#if 1
   /* take medium tiles from the lo tiles */
      for ( j=0; j<loViewCount; j++ )
      {
         for ( k=0; k<medViewCount; k++ )
         {
            if ( loTrackList[ j ] == medTrackList[ k ] )
            {
               loTrackList[ j ] = -1;
            }
         }
      }
#endif


#if 1
   /* take hi tiles from the med tiles */
      for ( j=0; j<medViewCount; j++ )
      {
         for ( k=0; k<hiViewCount; k++ )
         {
            if ( medTrackList[ j ] == hiTrackList[ k ] )
            {
               medTrackList[ j ] = -1;
            }
         }
      }
#endif




#if 1
      k = 0;
      for ( j=0; j<loViewCount; j++ )
      {
         if ( loTrackList[ j ] != -1 )
         {
            tempTrackList[ k++ ] = loTrackList[ j ];   
         }
      }
      sectionA->loViewCount = k;
      if ( sectionA->loViewCount > maxLo )
      {
         maxLo = sectionA->loViewCount;
      }
      avLo += sectionA->loViewCount;
      sectionA->loViewSection = compress ( tempTrackList, &sectionA->loViewCount );
#endif


#if 1
      k = 0;
      for ( j=0; j<medViewCount; j++ )
      {
         if ( medTrackList[ j ] != -1 )
         {
            tempTrackList[ k++ ] = medTrackList[ j ];   
         }
      }
      sectionA->medViewCount = k;
      if ( sectionA->medViewCount > maxMed )
      {
         maxMed = sectionA->medViewCount;
      }
      avMed += sectionA->medViewCount;
      sectionA->medViewSection = compress ( tempTrackList, &sectionA->medViewCount );
#endif


#if 1
      for ( j=0; j<hiViewCount; j++ )
      {
          tempTrackList[ j ] = hiTrackList[ j ];   
      }
      sectionA->hiViewCount = hiViewCount;
      if ( sectionA->hiViewCount > maxHi )
      {
         maxHi = sectionA->hiViewCount;
      }
      avHi += sectionA->hiViewCount;
      sectionA->hiViewSection = compress ( tempTrackList, &sectionA->hiViewCount );
#endif


      sectionA = NextIndexSection( sectionA );
   }

   printf( "maxLo  %d\n", maxLo );
   printf( "maxMed %d\n", maxMed );
   printf( "maxHi  %d\n", maxHi );

   printf( "avLo  %d\n", avLo / track->sectionCount );
   printf( "avMed  %d\n", avMed / track->sectionCount );
   printf( "avHi  %d\n", avHi / track->sectionCount );

}






#if 1
void DrawTrack( TrackCamera *camera, AutoMeshData *autoMesh, TileLib *tileLib )
{
   long           beforeVcount;
   char           dir;
   short          i, j;

   TrackSection   *forwardSection;
   TrackSection   *junctionSection;
   TrackSection   *backSection;

   TrackSection   *tempSection;
   short          junctionForwardDone = 0;
   short          junctionBackwardDone = 0;

   dir = TrackDirection(camera);

   if ( camera->section->flags & JunctionStart )
   {
      FntPrint( "Junction Start\n" );
   }

   if ( camera->section->flags & JunctionEnd )
   {
      FntPrint( "Junction End\n" );
   }

   if ( camera->section->flags & JunctionSection )
   {
      FntPrint( "Junction Section\n" );
   }

   if ( edDebug )
   {
      FntPrint( "Automeshed tris %d\n", autoMesh->triCount );
      FntPrint( "Automeshed quads %d\n", autoMesh->quadCount );
      FntPrint( "Automesh called %d times\n", autoMesh->attemptedAutos );

      if ( !autoMesh->autoMeshedOn )
      {
         FntPrint( "AutoMesh Off\n" );
      }
      else
      {
         FntPrint( "AutoMesh On\n" );
      }
   }

   autoMesh->triCount = 0;
   autoMesh->quadCount = 0;
   autoMesh->attemptedAutos = 0;

	beforeVcount = GsGetVcount();   


   AsmNextFrame( );


   if ( dir == ShipForwards )
   {
   /* forwards along main track */
      FntPrint ("Forwards\n" );

   /* draw the normal track forwards */
      forwardSection = camera->section;
      tempSection = camera->section;

      forwardSection = AsmTransformTrackHi( forwardSection, camera->camPos, 4 );  
      forwardSection = AsmTransformTrackMed( forwardSection, camera->camPos, 4 ); 
      AsmTransformTrackLo( forwardSection, camera->camPos, 15 );
   }
   else if ( dir == ShipBackwards )
   {
   /* backwards along main track */
      FntPrint ("Backwards\n" );

   /* draw the normal track backwards */

      backSection = GoBack( camera->section, 15-1 + 4-1 + 3-1 + 2 );
      tempSection = backSection;

      backSection = AsmTransformTrackLo( backSection, camera->camPos, 15 );
      backSection = AsmTransformTrackMed( backSection, camera->camPos, 4 );
      AsmTransformTrackHi( backSection, camera->camPos, 3 );
   }
   else if ( ( dir ==  ) || ( dir == RIGHT1 ) )
   {
      FntPrint ("Left1 or Right1\n" );

      backSection = GoBack( camera->section, 1 );
      tempSection = backSection;

      backSection = AsmTransformTrackHi( backSection, camera->camPos, 4 );
      backSection = AsmTransformTrackMed( backSection, camera->camPos, 2 ); 
      AsmTransformTrackLo( backSection, camera->camPos, 15 );

   }
   else if ( ( dir == LEFT2 ) || ( dir == RIGHT2 ) )
   {
      FntPrint ("Left2 or Right2\n" );

      backSection = GoBack( camera->section, 3 );
      tempSection = backSection;

      backSection = AsmTransformTrackMed( backSection, camera->camPos, 1 ); 
      backSection = AsmTransformTrackHi( backSection, camera->camPos, 3 );
      backSection = AsmTransformTrackMed( backSection, camera->camPos, 1 ); 
      AsmTransformTrackLo( backSection, camera->camPos, 7 );
   }
   else if ( ( dir == LEFT3 ) || ( dir == RIGHT3 ) )
   {
      FntPrint ("Left3 or Right3\n" );

      backSection = GoBack( camera->section, 11 );
      tempSection = backSection;

      backSection = AsmTransformTrackLo( backSection, camera->camPos, 6);
      backSection = AsmTransformTrackMed( backSection, camera->camPos, 4 );
      AsmTransformTrackHi( backSection, camera->camPos, 3 );
   }
   else if ( ( dir == LEFT4 ) || ( dir == RIGHT4 ) )
   {
      FntPrint ("Left4 or Right4\n" );

      backSection = GoBack( camera->section, 11 );
      tempSection = backSection;

      backSection = AsmTransformTrackLo( backSection, camera->camPos, 6);
      backSection = AsmTransformTrackMed( backSection, camera->camPos, 4 );
      AsmTransformTrackHi( backSection, camera->camPos, 3 );
   }



#if JunctionDrawForwards
   if ( 1 )
   {
   /* draw any junction pieces forwards */

   /* check we are not on a junction section first */
      if ( !(tempSection->flags & JunctionSection) )
      {
      /* search along the normal track to find a junction */
         for ( i=0; i<24; i++ )
         {
         /* if we find one and it has not already been drawn */
            if ( ( tempSection->junction.index != -1 ) )
            {
            /* if it is a junctionStart section ( as opposed to a junctionEnd ) then draw forwards */ 
               if ( ( tempSection->junction.ptr->flags & JunctionStart ) && ( junctionForwardDone < 1 ) )
               {
               /* ship going forwards drawing from JunctionStart */
                  FntPrint( "Forward Dir - Forward Junc\n" );

                  junctionSection = tempSection->junction.ptr;
#if 0
                  junctionSection = TransformTrackHi( junctionSection, tileLib, camera->camPos, camera->section, 3,  autoMesh, -1 );
                  junctionSection = TransformTrackMed( junctionSection, tileLib, camera->camPos, 2 ); 
#endif
                  TransformTrackLo( junctionSection, tileLib, camera->camPos, 11 );

                  junctionForwardDone += 1;
               }
               else if ( ( tempSection->junction.ptr->flags & JunctionEnd ) && ( junctionBackwardDone < 1 ) )
               {
               /* ship going forwards drawing from JunctionEnd */
                  FntPrint( "Forward Dir - Backward Junc\n" );

                  junctionSection = tempSection->junction.ptr;
/*
                  for (j=0; j<10; j++)
                  {
                     junctionSection = junctionSection->prevSection.ptr;
                  }
*/
                  junctionSection = GoBack( junctionSection, 10 );

                  junctionSection = TransformTrackLo( junctionSection, tileLib, camera->camPos, 11 );
#if 0
                  junctionSection = TransformTrackMed( junctionSection, tileLib, camera->camPos, 4 ); 
                  TransformTrackHi( junctionSection, tileLib, camera->camPos, camera->section, 3,  autoMesh, FORWARDS );  
#endif

                  junctionBackwardDone += 1;
               }
            }

            tempSection = tempSection->nextSection.ptr;
         }
      }
      else
      {
         /* we are on a junction section */


      }
   }
#endif

   FntPrint( "Track vcount  : %d \n", GsGetVcount() - beforeVcount );   
   FntPrint( "Section %d\n", camera->section - track->sections );


   if ( edDebug )
   {
      FntPrint( "Track vcount  : %d \n", GsGetVcount() - beforeVcount );   

      if ( camera->section->flags & Jump )
      {
         FntPrint( "Jump\n" );
      }

      if ( camera->section->flags & Tunnel )
      {
         FntPrint( "Tunnel\n" );
      }

      if ( camera->section->flags & MoreFaces )
      {
         FntPrint( "More Faces\n" );
      }

      if ( camera->section->flags & JunctionStart )
      {
         FntPrint( "Junction Start\n" );
      }

      if ( camera->section->flags & JunctionEnd )
      {
         FntPrint( "Junction End\n" );
      }

      FntPrint( "Faces %d\n", camera->section->faceCount );

      FntPrint( "Section %d\n", camera->section - track->sections );
   }

}

#endif



#if 0
   for ( i=0; i<camera->section->loViewCount; i+=2 )
   {
      dir = GetDir( camera, track->sections + camera->section->loViewSection[ i ] );
      if ( dir == FORWARDS )
      {
         AsmTransformTrackLo( track->sections + camera->section->loViewSection[ i ], 
                                 camera->camPos, 
                                    camera->section->loViewSection[ i + 1 ] );
         loCount += camera->section->loViewSection[ i + 1 ];
      }
   }
#endif

#if 0
   for ( i=0; i<camera->section->medViewCount; i+=2 )
   {
      dir = GetDir( camera, track->sections + camera->section->medViewSection[ i ] );
      if ( dir == FORWARDS )
      {
         AsmTransformTrackMed( track->sections + camera->section->medViewSection[ i ], 
                                 camera->camPos, 
                                    camera->section->medViewSection[ i + 1 ] );
         medCount += camera->section->medViewSection[ i + 1 ];
      }
   }
#endif

#if 0
   for ( i=0; i<camera->section->hiViewCount; i+=2 )
   {
      AsmTransformTrackHi( track->sections + camera->section->hiViewSection[ i ], 
                              camera->camPos, 
                                 camera->section->hiViewSection[ i + 1 ] );
      hiCount += camera->section->hiViewSection[ i + 1 ];
   }
#endif



char GetDir
( 
	TrackCamera       *camera, 
	TrackSection      *section
)
{
	SVECTOR     toCentre;
	long        angle;

   return (FORWARDS);


   toCentre.vx = (short)( section->centre.vx + camera->camPos->relative.t[0] );
	toCentre.vy = (short)( section->centre.vy + camera->camPos->relative.t[1] );
	toCentre.vz = (short)( section->centre.vz + camera->camPos->relative.t[2] );

	angle = -ratan2(toCentre.vx, toCentre.vz) + camera->hdg ;
	angle = ang(angle);
 
  
   if( (angle >= -1024) && (angle < 1024) )
	{
      return (FORWARDS);
	}
	else 
	{
      return (BACKWARDS);
	}

}






xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx







#if 0 
      /* south */

      globalViewCount = 0;
      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < GlobalDistance ) &&
              ( abs( vector.vy ) < GlobalDistance ) &&
              ( abs( vector.vz ) < GlobalDistance ) )
         {
            vector.vx = vector.vx;
            vector.vy = vector.vy;
            vector.vz = vector.vz;

            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            if ( ( distance < 32767 ) && ( SectionPosition( sectionA, sectionB ) & South ) )
            {
               if ( globalViewCount >= MaxGlobalList )
               {
                  Error( "radius.c::TrackRadiusCheckNew: Global View List Full", Warning );
               }

               globalTrackList[ globalViewCount ][ 0 ] = j;
               globalTrackList[ globalViewCount ][ 1 ] = distance;
               globalViewCount += 1;
            }

         }
         sectionB = NextIndexSection( sectionB );
      }

      sortDistances( globalTrackList, globalViewCount );

#if 0
      printf( "From section %d there are %d visible south sections\n", i, globalViewCount );
#endif

      sectionA->southViewCount = globalViewCount;
      sectionA->southViewSection = ( short* )DAlloc( heap,  globalViewCount * sizeof(short) );
      for ( j=0; j<sectionA->southViewCount; j++ )         
      {
         sectionA->southViewSection[ j ] = globalTrackList[ j ][ 0 ];
      }
 


      /* east */

      globalViewCount = 0;
      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < GlobalDistance ) &&
              ( abs( vector.vy ) < GlobalDistance ) &&
              ( abs( vector.vz ) < GlobalDistance ) )
         {
            vector.vx = vector.vx;
            vector.vy = vector.vy;
            vector.vz = vector.vz;

            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            if ( ( distance < 32767 ) && ( SectionPosition( sectionA, sectionB ) & East ) )
            {
               if ( globalViewCount >= MaxGlobalList )
               {
                  Error( "radius.c::TrackRadiusCheckNew: Global View List Full", Warning );
               }

               globalTrackList[ globalViewCount ][ 0 ] = j;
               globalTrackList[ globalViewCount ][ 1 ] = distance;
               globalViewCount += 1;
            }

         }
         sectionB = NextIndexSection( sectionB );
      }

      sortDistances( globalTrackList, globalViewCount );

#if 0
      printf( "From section %d there are %d visible east sections\n", i, globalViewCount );
#endif

      sectionA->eastViewCount = globalViewCount;
      sectionA->eastViewSection = ( short* )DAlloc( heap,  globalViewCount * sizeof(short) );
      for ( j=0; j<sectionA->eastViewCount; j++ )         
      {
         sectionA->eastViewSection[ j ] = globalTrackList[ j ][ 0 ];
      }
 




      /* west */

      globalViewCount = 0;
      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < GlobalDistance ) &&
              ( abs( vector.vy ) < GlobalDistance ) &&
              ( abs( vector.vz ) < GlobalDistance ) )
         {
            vector.vx = vector.vx;
            vector.vy = vector.vy;
            vector.vz = vector.vz;

            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            if ( ( distance < 32767 ) && ( SectionPosition( sectionA, sectionB ) & West ) )
            {
               if ( globalViewCount >= MaxGlobalList )
               {
                  Error( "radius.c::TrackRadiusCheckNew: Global View List Full", Warning );
               }

               globalTrackList[ globalViewCount ][ 0 ] = j;
               globalTrackList[ globalViewCount ][ 1 ] = distance;
               globalViewCount += 1;
            }

         }
         sectionB = NextIndexSection( sectionB );
      }

      sortDistances( globalTrackList, globalViewCount );

#if 0
      printf( "From section %d there are %d visible west sections\n", i, globalViewCount );
#endif

      sectionA->westViewCount = globalViewCount;
      sectionA->westViewSection = ( short* )DAlloc( heap,  globalViewCount * sizeof(short) );
      for ( j=0; j<sectionA->westViewCount; j++ )         
      {
         sectionA->westViewSection[ j ] = globalTrackList[ j ][ 0 ];
      }
#endif 











#if 0
   short          test[ 32 ] = { 5, 6, 7, 8, 1, 2, 3, 4, 1 };
   short          testCount = 9;

   compress ( test, &testCount );
   printf("\ntestCount = %d\n", testCount );
   exit ( 0 );
#endif









#if 0
   if ( 1 || dir == ShipForwards )
   {
      if ( camera->section->northViewCount[ 0 ] >= 4 )
      {
         for ( i=0; i<4; i++ )
         {
            AsmTransformTrackHi( track->sections + camera->section->northViewSection[ 0 ][ i ], 
                                    camera->camPos, 
                                       1 );
            hiCount++;
         }
      }

      if ( camera->section->northViewCount[ 0 ] >= 8 )
      {
         for ( i=4; i<8; i++ )
         {
            AsmTransformTrackMed( track->sections + camera->section->northViewSection[ 0 ][ i ], 
                                    camera->camPos, 
                                       1 );
            medCount++;
         }
      }

      for ( i=8; i<camera->section->northViewCount[ 0 ] && i<30; i++ )
      {
         AsmTransformTrackLo( track->sections + camera->section->northViewSection[ 0 ][ i ], 
                                 camera->camPos, 
                                    1 );
         loCount++;
      }
   }
   else if ( dir == ShipBackwards )
   {
      for ( i=0; i<4; i++ )
      {
         AsmTransformTrackHi( track->sections + camera->section->southViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         hiCount++;
      }

      for ( i=4; i<8; i++ )
      {
         AsmTransformTrackMed( track->sections + camera->section->southViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         medCount++;
      }

      for ( i=8; i<camera->section->southViewCount && i<30; i++ )
      {
         AsmTransformTrackLo( track->sections + camera->section->southViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         loCount++;
      }
   }
   else if ( dir == ShipRight )
   {
      for ( i=0; i<4; i++ )
      {
         AsmTransformTrackHi( track->sections + camera->section->westViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         hiCount++;
      }

      for ( i=4; i<8; i++ )
      {
         AsmTransformTrackMed( track->sections + camera->section->westViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         medCount++;
      }

      for ( i=8; i<camera->section->westViewCount && i<30; i++ )
      {
         AsmTransformTrackLo( track->sections + camera->section->westViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         loCount++;
      }
   }
   else if ( dir == ShipLeft )
   {
      for ( i=0; i<4; i++ )
      {
         AsmTransformTrackHi( track->sections + camera->section->eastViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         hiCount++;
      }

      for ( i=4; i<8; i++ )
      {
         AsmTransformTrackMed( track->sections + camera->section->eastViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         medCount++;
      }

      for ( i=8; i<camera->section->eastViewCount && i<30; i++ )
      {
         AsmTransformTrackLo( track->sections + camera->section->eastViewSection[ i ], 
                                 camera->camPos, 
                                    1 );
         loCount++;
      }
   }
#endif


