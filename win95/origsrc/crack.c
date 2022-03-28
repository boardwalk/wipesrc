#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "droid.h"
#include "camera.h"
#include "weapon.h"
#include "control.h"
#include "sound.h"
#include "draw.h"
#include "sparks2.h"
#include "global.h"
#include "ttf.h"
#include "trackasm.h"
#include "asmtrk.h"
#include "crack.h"






POLY_FT3    crack[ 8 ][ 2 ];
POLY_FT3    hiMedCrack[ 16 ][ 2 ];


extern Surface     TrackSurface[ 24 ][ 2 ];         /* 20k */

//#ifndef USE_ATI

#if 0
void FillMedLoCrack
( 
   TrackSection      *medSection,
   Skeleton          *camera
)
{
   Face           *medFace;
   short          i;
   VECTOR         camposData;
   VECTOR         *campos = &camposData;
   VECTOR         *vertices;
   VECTOR         vector[ 9 ];
   SVECTOR        svector[ 9 ];

   ushort         depthA, depthB, depthC;
   long           flagA, flagB, flagC;
	DVECTOR 		   sxyA, sxyB, sxyC ;
   long           p;


   PolyFT3        *ft3;

   Surface        *surface;
   Tile           *tile;


/* Set transformation matrix */

   SetSkeletonTransform( camera->super );

/* Copy camera position into fast ram */

   campos->vx = camera->relative.t[ 0 ];
   campos->vy = camera->relative.t[ 1 ];
   campos->vz = camera->relative.t[ 2 ];

/* Calculate Med res meshed track sections */

   vertices = track->vertices;
   medFace = track->faces + medSection->faceStart;


   for ( i=0; i<medSection->faceCount; i++ )
   {          
   /* get original 32 bit faces coords */

      vector[ 0 ] = *( vertices + medFace->vertex[ 1 ] );
      vector[ 2 ] = *( vertices + medFace->vertex[ 0 ] );
      vector[ 6 ] = *( vertices + medFace->vertex[ 2 ] );
      vector[ 8 ] = *( vertices + medFace->vertex[ 3 ] );


   /* mesh top */

      vector[ 1 ].vx = ( vector[ 0 ].vx + vector[ 2 ].vx ) >> 1;
      vector[ 1 ].vy = ( vector[ 0 ].vy + vector[ 2 ].vy ) >> 1;
      vector[ 1 ].vz = ( vector[ 0 ].vz + vector[ 2 ].vz ) >> 1;


   /* make SVECTORs */

      svector[ 0 ].vx = vector[ 0 ].vx + campos->vx;
      svector[ 0 ].vy = vector[ 0 ].vy + campos->vy;
      svector[ 0 ].vz = vector[ 0 ].vz + campos->vz;

      svector[ 1 ].vx = vector[ 1 ].vx + campos->vx;
      svector[ 1 ].vy = vector[ 1 ].vy + campos->vy;
      svector[ 1 ].vz = vector[ 1 ].vz + campos->vz;

      svector[ 2 ].vx = vector[ 2 ].vx + campos->vx;
      svector[ 2 ].vy = vector[ 2 ].vy + campos->vy;
      svector[ 2 ].vz = vector[ 2 ].vz + campos->vz;


		depthA = RotTransPers( &svector[ 0 ],
   			   		        &sxyA,
	   					        &p,
		   				        &flagA );


		depthB = RotTransPers( &svector[ 1 ],
   			   		        &sxyB,
	   					        &p,
		   				        &flagB );


		depthC = RotTransPers( &svector[ 2 ],
   			   		        &sxyC,
	   					        &p,
		   				        &flagC );



      if ( !( ( flagA | flagB | flagC ) & ( CrtClipped | ZNegative | ZLarge | ZSmall ) ) )
      {
         ft3 = (PolyFT3*) &( crack[ i ][ CurrentScreen ] );

         SetPolyFT3( ( POLY_FT3* ) ft3 );
//#if 1
         if ( medFace->flags & FlipTexture )
         {
            surface = &( TrackSurface[ medFace->texture ][ 1 ] );
         }
         else
         {
            surface = &( TrackSurface[ medFace->texture ][ 0 ] );
         }

         tile = &( surface->low[ 0 ] );


         ft3->cba = tile->cba;
         ft3->tsb = tile->tsb;

         ft3->u0 = tile->u0;
         ft3->v0 = tile->v0;

         ft3->u1 = ( tile->u0 + tile->u1 ) >> 1;
         ft3->v1 = ( tile->v0 + tile->v1 ) >> 1;
         
         ft3->u2 = tile->u1;
         ft3->v2 = tile->v1;

         ft3->colour0.r =  medFace->red;
         ft3->colour0.g =  medFace->green;
         ft3->colour0.b =  medFace->blue;
//#endif
         ft3->xy0 = sxyA;
         ft3->xy1 = sxyB;
         ft3->xy2 = sxyC;

         AddPrim( OT[ CurrentScreen ] + ( depthB ), ( ulong* ) ft3 );
      }


      medFace++;
   }


}


//#endif //USE_ATI




//#ifndef USE_ATI



void FillHiMedCrack
( 
   TrackSection      *hiSection,
   Skeleton          *camera
)
{
   Face           *hiFace;
   short          i;

   VECTOR         camposData;
   VECTOR         *campos = &camposData;
   VECTOR         *vertices;
   VECTOR         vector[ 25 ];
   SVECTOR        svector[ 25 ];

   ushort         depthA, depthB, depthC, depthD, depthE;
   long           flagA, flagB, flagC, flagD, flagE;
	DVECTOR 		   sxyA, sxyB, sxyC, sxyD, sxyE;
   long           p;


   PolyFT3        *ft3;

   Surface        *surface;
   Tile           *tile;


/* Set transformation matrix */

   SetSkeletonTransform( camera->super );

/* Copy camera position into fast ram */

   campos->vx = camera->relative.t[ 0 ];
   campos->vy = camera->relative.t[ 1 ];
   campos->vz = camera->relative.t[ 2 ];

/* Calculate Med res meshed track sections */

   vertices = track->vertices;
   hiFace = track->faces + hiSection->faceStart;



   for ( i=0; i<hiSection->faceCount; i++ )
   { 
   /* get original 32 bit faces coords */

      vector[ 0 ] = *( vertices + hiFace->vertex[ 1 ] );
      vector[ 4 ] = *( vertices + hiFace->vertex[ 0 ] );

   /* mesh top */

      vector[ 2 ].vx = ( vector[ 0 ].vx + vector[ 4 ].vx ) >> 1;
      vector[ 2 ].vy = ( vector[ 0 ].vy + vector[ 4 ].vy ) >> 1;
      vector[ 2 ].vz = ( vector[ 0 ].vz + vector[ 4 ].vz ) >> 1;

      vector[ 1 ].vx = ( vector[ 0 ].vx + vector[ 2 ].vx ) >> 1;
      vector[ 1 ].vy = ( vector[ 0 ].vy + vector[ 2 ].vy ) >> 1;
      vector[ 1 ].vz = ( vector[ 0 ].vz + vector[ 2 ].vz ) >> 1;

      vector[ 3 ].vx = ( vector[ 2 ].vx + vector[ 4 ].vx ) >> 1;
      vector[ 3 ].vy = ( vector[ 2 ].vy + vector[ 4 ].vy ) >> 1;
      vector[ 3 ].vz = ( vector[ 2 ].vz + vector[ 4 ].vz ) >> 1;

   
   /* make SVECTORs */

      svector[ 0 ].vx = vector[ 0 ].vx + campos->vx;
      svector[ 0 ].vy = vector[ 0 ].vy + campos->vy;
      svector[ 0 ].vz = vector[ 0 ].vz + campos->vz;

      svector[ 1 ].vx = vector[ 1 ].vx + campos->vx;
      svector[ 1 ].vy = vector[ 1 ].vy + campos->vy;
      svector[ 1 ].vz = vector[ 1 ].vz + campos->vz;

      svector[ 2 ].vx = vector[ 2 ].vx + campos->vx;
      svector[ 2 ].vy = vector[ 2 ].vy + campos->vy;
      svector[ 2 ].vz = vector[ 2 ].vz + campos->vz;

      svector[ 3 ].vx = vector[ 3 ].vx + campos->vx;
      svector[ 3 ].vy = vector[ 3 ].vy + campos->vy;
      svector[ 3 ].vz = vector[ 3 ].vz + campos->vz;

      svector[ 4 ].vx = vector[ 4 ].vx + campos->vx;
      svector[ 4 ].vy = vector[ 4 ].vy + campos->vy;
      svector[ 4 ].vz = vector[ 4 ].vz + campos->vz;



		depthA = RotTransPers( &svector[ 0 ],
   		     		           &sxyA,
	   	  			           &p,
		     			           &flagA );

		depthB = RotTransPers( &svector[ 1 ],
   		     		           &sxyB,
	   	  			           &p,
		     			           &flagB );

		depthC = RotTransPers( &svector[ 2 ],
   		     		           &sxyC,
	   	  			           &p,
		     			           &flagC );

		depthD = RotTransPers( &svector[ 3 ],
   		     		           &sxyD,
	   	  			           &p,
		     			           &flagD );

		depthE = RotTransPers( &svector[ 4 ],
   		     		           &sxyE,
	   	  			           &p,
		   				        &flagE );




      if ( !( ( flagA | flagB | flagC ) & ( CrtClipped | ZNegative | ZLarge | ZSmall ) ) )
      {
         ft3 = (PolyFT3*) &( hiMedCrack[ i*2 ][ CurrentScreen ] );

         SetPolyFT3( ( POLY_FT3* ) ft3 );

         if ( hiFace->flags & FlipTexture )
         {
            surface = &( TrackSurface[ hiFace->texture ][ 1 ] );
         }
         else
         {
            surface = &( TrackSurface[ hiFace->texture ][ 0 ] );
         }

         tile = &( surface->med[ 0 ] );


         ft3->cba = tile->cba;
         ft3->tsb = tile->tsb;

         ft3->u0 = tile->u0;
         ft3->v0 = tile->v0;

         ft3->u1 = ( tile->u0 + tile->u1 ) >> 1;
         ft3->v1 = ( tile->v0 + tile->v1 ) >> 1;
         
         ft3->u2 = tile->u1;
         ft3->v2 = tile->v1;
 
         ft3->colour0.r = hiFace->red;
         ft3->colour0.g = hiFace->green;
         ft3->colour0.b = hiFace->blue;


         ft3->xy0 = sxyA;
         ft3->xy1 = sxyB;
         ft3->xy2 = sxyC;

         AddPrim( OT[ CurrentScreen ] + ( depthB ), ( ulong* ) ft3 );
      }



      if ( !( ( flagC | flagD | flagE ) & ( CrtClipped | ZNegative | ZLarge | ZSmall ) ) )
      {
         ft3 = (PolyFT3*) &( hiMedCrack[ i*2 + 1 ][ CurrentScreen ] );

         SetPolyFT3( ( POLY_FT3* ) ft3 );

         if ( hiFace->flags & FlipTexture )
         {
            surface = &( TrackSurface[ hiFace->texture ][ 1 ] );
         }
         else
         {
            surface = &( TrackSurface[ hiFace->texture ][ 0 ] );
         }

         tile = &( surface->med[ 1 ] );


         ft3->cba = tile->cba;
         ft3->tsb = tile->tsb;

         ft3->u0 = tile->u0;
         ft3->v0 = tile->v0;

         ft3->u1 = ( tile->u0 + tile->u1 ) >> 1;
         ft3->v1 = ( tile->v0 + tile->v1 ) >> 1;
         
         ft3->u2 = tile->u1;
         ft3->v2 = tile->v1;

         ft3->colour0.r = hiFace->red;
         ft3->colour0.g = hiFace->green;
         ft3->colour0.b = hiFace->blue;


         ft3->xy0 = sxyC;
         ft3->xy1 = sxyD;
         ft3->xy2 = sxyE;

         AddPrim( OT[ CurrentScreen ] + ( depthD ), ( ulong* ) ft3 );
      }


      hiFace++;
   }


}


//#endif //USE_ATI











#if 0

tile->u0;
tile->v0;

( tile->u0 + tile->u2 ) >> 1;
( tile->v0 + tile->v2 ) >> 1;

tile->u2;
tile->v2;




tile->u0;
tile->v0;

( tile->u0 + tile->u2 ) >> 1;
( tile->v0 + tile->v2 ) >> 1;

tile->u2;
tile->v2;

#endif

#endif
