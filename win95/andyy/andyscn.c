#include "..\origsrc\standard.h"

#include "..\origsrc\malloc.h"
#include "..\origsrc\tim.h"
#include "..\origsrc\object.h"
#include "..\origsrc\skeleton.h"
#include "..\origsrc\track.h"
#include "..\origsrc\ships.h"
#include "..\origsrc\weapon.h"
#include "..\origsrc\control.h"
#include "..\origsrc\droid.h"
#include "..\origsrc\camera.h"
#include "..\origsrc\scene.h"
#include "..\origsrc\sound.h"
#include "..\origsrc\sparks2.h"
#include "..\origsrc\global.h"
#include "..\origsrc\combat.h"


extern SVECTOR     StaticVertex[ ];
extern DVECTOR     StaticScreen[ ];
extern ushort      StaticDepth[ ];
extern ushort      StaticFlags[ ];
extern ushort	   StaticZ[ ];

#define     ClipFlag    ( ( CrtClipped | ZLarge | ZNegative | ZSmall ) >> 12 )
#define		MaxLimit;


void DrawObjectAsm( Object* object, Skeleton* camera, int roll, short ShipObj)
{
   DVECTOR*    screenPtr;
   ushort*     depthPtr;
   ushort*     flagsPtr;
   ushort*	   ZPtr;

   StdPoly     prim;
   Poly        poly;

   short       i;
   
   short       coord0;
   short       coord1;
   short       coord2;
   short       coord3;

   long        depth;
   long        newdepth;

   SVECTOR*    vertex;

   short       primitiveCount;
   short       vertexCount;


   /* for sprites */
   int 			    x, y;

   short 		    xSize;
   short 		    ySize;
   short 		    xSize2;
   short 		    ySize2;

   short            tlX; 
   short            tlY; 
   short            trX; 
   short            trY; 
   short            blX; 
   short            blY; 
   short            brX; 
   short            brY; 

   short            offsetX; 
   short            offsetY; 
   short            offset2X; 
   short            offset2Y; 

   short            tex;

   short			ObjDepthMod;

   long				Test16bitX;
   long				Test16bitY;
   long				Test16bitZ;



	if(ShipObj)
		ObjDepthMod = 0;
	else
		ObjDepthMod = 600;

   SetSkeletonTransform( object->skeleton );

   vertex = object->vertices;
   vertexCount = object->vertexCount;

   poly.prim = object->primitives;
   primitiveCount = object->primitiveCount;

	primCount += primitiveCount;


   if ( vertexCount <= 128 )
   {
      screenPtr = ( DVECTOR* )( FastRam );
      depthPtr = ( ushort* )( FastRam + 512 );
      flagsPtr = ( ushort* )( FastRam + 768 );
   }
   else
   {
      screenPtr = StaticScreen;
      depthPtr = StaticDepth;
      flagsPtr = StaticFlags;
   }
   ZPtr = StaticZ;

   RotTransPersN( vertex, screenPtr, depthPtr, flagsPtr, vertexCount, ZPtr );


   for ( i=0; i<primitiveCount; i++ )
   {
      switch ( poly.prim->type )
      {
         case TypeGT3 :

            coord0 = poly.gt3->coords[ 0 ];
            coord1 = poly.gt3->coords[ 1 ];
            coord2 = poly.gt3->coords[ 2 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyGT3( ( POLY_GT3* )prims.ptr );

                  prims.gt3->cba = poly.gt3->cba;
                  prims.gt3->tsb = poly.gt3->tsb;

                  prims.gt3->u0 = poly.gt3->u0;
                  prims.gt3->v0 = poly.gt3->v0;
                  prims.gt3->u1 = poly.gt3->u1;
                  prims.gt3->v1 = poly.gt3->v1;
                  prims.gt3->u2 = poly.gt3->u2;
                  prims.gt3->v2 = poly.gt3->v2;

                  prims.gt3->colour0.r = poly.gt3->colour[ 0 ].r;
                  prims.gt3->colour0.g = poly.gt3->colour[ 0 ].g;
                  prims.gt3->colour0.b = poly.gt3->colour[ 0 ].b;
                  prims.gt3->colour1 = poly.gt3->colour[ 1 ];
                  prims.gt3->colour2 = poly.gt3->colour[ 2 ];

                  prims.gt3->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.gt3->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.gt3->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.gt3->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.gt3->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.gt3->xy2.vy = screenPtr[ coord2 ].vy;
				  prims.gt3->xy0.vz = ZPtr[ coord0 ];
				  prims.gt3->xy1.vz = ZPtr[ coord1 ];
				  prims.gt3->xy2.vz = ZPtr[ coord2 ];
						prims.gt3->xy0.pad = 0;
						prims.gt3->xy1.pad = 0;
						prims.gt3->xy2.pad = 0;
                  //prims.gt3->xy1 = screenPtr[ coord1 ];
                  //prims.gt3->xy2 = screenPtr[ coord2 ];

                  depth = AverageZ3( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ] );

					depth += ObjDepthMod;

					if(poly.ft3->flag & ShipEngine)
						depth -= 50;

					if(depth > 8191)
						depth = 8191;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyGT3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.gt3 += 1;
               }

	  }
            poly.gt3 += 1;
            break;

         case TypeGT4 :

            coord0 = poly.gt4->coords[ 0 ];
            coord1 = poly.gt4->coords[ 1 ];
            coord2 = poly.gt4->coords[ 2 ];
            coord3 = poly.gt4->coords[ 3 ];
            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyGT4( ( POLY_GT4* )prims.ptr );

                  prims.gt4->cba = poly.gt4->cba;
                  prims.gt4->tsb = poly.gt4->tsb;

                  prims.gt4->u0 = poly.gt4->u0;
                  prims.gt4->v0 = poly.gt4->v0;
                  prims.gt4->u1 = poly.gt4->u1;
                  prims.gt4->v1 = poly.gt4->v1;
                  prims.gt4->u2 = poly.gt4->u2;
                  prims.gt4->v2 = poly.gt4->v2;
                  prims.gt4->u3 = poly.gt4->u3;
                  prims.gt4->v3 = poly.gt4->v3;
                  prims.gt4->colour0.r = poly.gt4->colour[ 0 ].r;
                  prims.gt4->colour0.g = poly.gt4->colour[ 0 ].g;
#if 0 //USE_ATI
                  prims.gt4->colour0.b = poly.gt4->colour[ 0 ].b;
#else

		if(trackNo == 4 || trackNo == 5)
			prims.gt4->colour0.b = BLACK_NOT_DRAWN;
		else
			prims.gt4->colour0.b = poly.gt4->colour[ 0 ].b;
#endif
                  prims.gt4->colour1 = poly.gt4->colour[ 1 ];
                  prims.gt4->colour2 = poly.gt4->colour[ 2 ];
                  prims.gt4->colour3 = poly.gt4->colour[ 3 ];

                  //prims.gt4->xy0 = screenPtr[ coord0 ];
                  //prims.gt4->xy1 = screenPtr[ coord1 ];
                  //prims.gt4->xy2 = screenPtr[ coord2 ];
                  //prims.gt4->xy3 = screenPtr[ coord3 ];
                  prims.gt4->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.gt4->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.gt4->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.gt4->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.gt4->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.gt4->xy2.vy = screenPtr[ coord2 ].vy;
                  prims.gt4->xy3.vx = screenPtr[ coord3 ].vx;
                  prims.gt4->xy3.vy = screenPtr[ coord3 ].vy;
				  prims.gt4->xy0.vz = ZPtr[ coord0 ];
				  prims.gt4->xy1.vz = ZPtr[ coord1 ];
				  prims.gt4->xy2.vz = ZPtr[ coord2 ];
				  prims.gt4->xy3.vz = ZPtr[ coord3 ];
						prims.gt4->xy0.pad = 0;
						prims.gt4->xy1.pad = 0;
						prims.gt4->xy2.pad = 0;
						prims.gt4->xy3.pad = 0;

                  depth = AverageZ4( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ],
                                     depthPtr[ coord3 ] );

					depth += ObjDepthMod;
					if(depth > 8191)
						depth = 8191;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyGT4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.gt4 += 1;
               }

}
            poly.gt4 += 1;
            break;

         case TypeFT3 :

            coord0 = poly.ft3->coords[ 0 ];
            coord1 = poly.ft3->coords[ 1 ];
            coord2 = poly.ft3->coords[ 2 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyFT3( ( POLY_FT3* )prims.ptr );

                  prims.ft3->cba = poly.ft3->cba;
                  prims.ft3->tsb = poly.ft3->tsb;

                  prims.ft3->u0 = poly.ft3->u0;
                  prims.ft3->v0 = poly.ft3->v0;
                  prims.ft3->u1 = poly.ft3->u1;
                  prims.ft3->v1 = poly.ft3->v1;
                  prims.ft3->u2 = poly.ft3->u2;
                  prims.ft3->v2 = poly.ft3->v2;

                  prims.ft3->colour0.r = poly.ft3->colour.r;
                  prims.ft3->colour0.g = poly.ft3->colour.g;
                  prims.ft3->colour0.b = poly.ft3->colour.b;

                  //prims.ft3->xy0 = screenPtr[ coord0 ];
                  //prims.ft3->xy1 = screenPtr[ coord1 ];
                  //prims.ft3->xy2 = screenPtr[ coord2 ];
                  prims.ft3->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.ft3->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.ft3->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.ft3->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.ft3->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.ft3->xy2.vy = screenPtr[ coord2 ].vy;
				  prims.ft3->xy0.vz = ZPtr[ coord0 ];
				  prims.ft3->xy1.vz = ZPtr[ coord1 ];
				  prims.ft3->xy2.vz = ZPtr[ coord2 ];
						prims.ft3->xy0.pad = 0;
						prims.ft3->xy1.pad = 0;
						prims.ft3->xy2.pad = 0;

                  depth = AverageZ3( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ] );

					depth += ObjDepthMod;
					if(depth > 8191)
						depth = 8191;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyFT3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.ft3 += 1;
               }
            }
            poly.ft3 += 1;
            break;

         case TypeFT4 :

            coord0 = poly.ft4->coords[ 0 ];
            coord1 = poly.ft4->coords[ 1 ];
            coord2 = poly.ft4->coords[ 2 ];
            coord3 = poly.ft4->coords[ 3 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyFT4( ( POLY_FT4* )prims.ptr );

                  prims.ft4->cba = poly.ft4->cba;
                  prims.ft4->tsb = poly.ft4->tsb;

                  prims.ft4->u0 = poly.ft4->u0;
                  prims.ft4->v0 = poly.ft4->v0;
                  prims.ft4->u1 = poly.ft4->u1;
                  prims.ft4->v1 = poly.ft4->v1;
                  prims.ft4->u2 = poly.ft4->u2;
                  prims.ft4->v2 = poly.ft4->v2;
                  prims.ft4->u3 = poly.ft4->u3;
                  prims.ft4->v3 = poly.ft4->v3;

                  prims.ft4->colour0.r = poly.ft4->colour.r;
                  prims.ft4->colour0.g = poly.ft4->colour.g;
                  prims.ft4->colour0.b = poly.ft4->colour.b;

                  //prims.ft4->xy0 = screenPtr[ coord0 ];
                  //prims.ft4->xy1 = screenPtr[ coord1 ];
                  //prims.ft4->xy2 = screenPtr[ coord2 ];
                  //prims.ft4->xy3 = screenPtr[ coord3 ];
                  prims.ft4->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.ft4->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.ft4->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.ft4->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.ft4->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.ft4->xy2.vy = screenPtr[ coord2 ].vy;
                  prims.ft4->xy3.vx = screenPtr[ coord3 ].vx;
                  prims.ft4->xy3.vy = screenPtr[ coord3 ].vy;
				  prims.ft4->xy0.vz = ZPtr[ coord0 ];
				  prims.ft4->xy1.vz = ZPtr[ coord1 ];
				  prims.ft4->xy2.vz = ZPtr[ coord2 ];
				  prims.ft4->xy3.vz = ZPtr[ coord3 ];
						prims.ft4->xy0.pad = 0;
						prims.ft4->xy1.pad = 0;
						prims.ft4->xy2.pad = 0;
						prims.ft4->xy3.pad = 0;

                  depth = AverageZ4( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ],
                                     depthPtr[ coord3 ] );

					depth += ObjDepthMod;
					if(depth > 8191)
						depth = 8191;

 //                 AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyFT4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.ft4 += 1;
            }

            poly.ft4 += 1;
		}
            break;

         case TypeG3 :

            coord0 = poly.g3->coords[ 0 ];
            coord1 = poly.g3->coords[ 1 ];
            coord2 = poly.g3->coords[ 2 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyG3( ( POLY_G3* )prims.ptr );

                  prims.g3->colour0.r = poly.g3->colour[ 0 ].r;
                  prims.g3->colour0.g = poly.g3->colour[ 0 ].g;
                  prims.g3->colour0.b = poly.g3->colour[ 0 ].b;
                  prims.g3->colour1 = poly.g3->colour[ 1 ];
                  prims.g3->colour2 = poly.g3->colour[ 2 ];

                  prims.g3->xy0 = screenPtr[ coord0 ];
                  prims.g3->xy1 = screenPtr[ coord1 ];
                  prims.g3->xy2 = screenPtr[ coord2 ];

                  depth = AverageZ3( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ] );

					depth += ObjDepthMod;
					if(depth > 8191)
						depth = 8191;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyG3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.g3 += 1;
               }

		}
            poly.g3 += 1;
            break;

         case TypeG4 :

            coord0 = poly.g4->coords[ 0 ];
            coord1 = poly.g4->coords[ 1 ];
            coord2 = poly.g4->coords[ 2 ];
            coord3 = poly.g4->coords[ 3 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyG4( ( POLY_G4* )prims.ptr );

                  prims.g4->colour0.r = poly.g4->colour[ 0 ].r;
                  prims.g4->colour0.g = poly.g4->colour[ 0 ].g;
                  prims.g4->colour0.b = poly.g4->colour[ 0 ].b;
                  prims.g4->colour1 = poly.g4->colour[ 1 ];
                  prims.g4->colour2 = poly.g4->colour[ 2 ];
                  prims.g4->colour3 = poly.g4->colour[ 3 ];

                  prims.g4->xy0 = screenPtr[ coord0 ];
                  prims.g4->xy1 = screenPtr[ coord1 ];
                  prims.g4->xy2 = screenPtr[ coord2 ];
                  prims.g4->xy3 = screenPtr[ coord3 ];

                  depth = AverageZ4( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ],
                                     depthPtr[ coord3 ] );

					depth += ObjDepthMod;
					if(depth > 8191)
						depth = 8191;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyG4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.g4 += 1;
               }

		}
            poly.g4 += 1;
            break;

         case TypeF3 :

            coord0 = poly.f3->coords[ 0 ];
            coord1 = poly.f3->coords[ 1 ];
            coord2 = poly.f3->coords[ 2 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyF3( ( POLY_F3* )prims.ptr );

                  prims.f3->colour0.r = poly.f3->colour.r;
                  prims.f3->colour0.g = poly.f3->colour.g;
                  prims.f3->colour0.b = poly.f3->colour.b;

                  prims.f3->xy0 = screenPtr[ coord0 ];
                  prims.f3->xy1 = screenPtr[ coord1 ];
                  prims.f3->xy2 = screenPtr[ coord2 ];

                  depth = AverageZ3( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ] );

					depth += ObjDepthMod;
					if(depth > 8191)
						depth = 8191;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyF3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.f3 += 1;
               }

		}
            poly.f3 += 1;
            break;

         case TypeF4 :

            coord0 = poly.f4->coords[ 0 ];
            coord1 = poly.f4->coords[ 1 ];
            coord2 = poly.f4->coords[ 2 ];
            coord3 = poly.f4->coords[ 3 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ) & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyF4( ( POLY_F4* )prims.ptr );


                  prims.f4->colour0.r = poly.f4->colour.r;
                  prims.f4->colour0.g = poly.f4->colour.g;
                  prims.f4->colour0.b = poly.f4->colour.b;


                  prims.f4->xy0 = screenPtr[ coord0 ];
                  prims.f4->xy1 = screenPtr[ coord1 ];
                  prims.f4->xy2 = screenPtr[ coord2 ];
                  prims.f4->xy3 = screenPtr[ coord3 ];

                  depth = AverageZ4( depthPtr[ coord0 ],
                                     depthPtr[ coord1 ],
                                     depthPtr[ coord2 ],
                                     depthPtr[ coord3 ] );

					depth += ObjDepthMod;
					if(depth > 8191)
						depth = 8191;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyF4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
				  

                  //prims.f4 += 1;
               }
            }

            poly.f4 += 1;
            break;

		case TypeTSPR:

            coord0 = poly.spr->coord;

            depth = depthPtr[coord0];
            x = screenPtr[coord0].vx;
            y = screenPtr[coord0].vy;

            if( !( flagsPtr[coord0] & ClipFlag ) )
	         {
		       xSize = ( 160 * poly.spr->width ) / depth; 
		       ySize = ( 160 * poly.spr->height ) / depth;

		       xSize2 = xSize >> 1;
		       ySize2 = ySize >> 1; 

               offsetX = ( ( xSize2 * rcos( roll ) ) >> 12 );
               offsetY = ( ( xSize2 * rsin( roll ) ) >> 12 );

               offset2X = ( ( ySize * rcos( 1024 - roll ) ) >> 12 );
               offset2Y = ( ( ySize * rsin( 1024 - roll ) ) >> 12 );

			   tlX = x - offsetX;
			   tlY = y - offsetY;
			
			   brX = tlX - offset2X;
			   brY = tlY + offset2Y;

               trX = x + offsetX;
               trY = y + offsetY;

               blX = trX - offset2X;
               blY = trY + offset2Y;


		         prims.ft4->xy0.vx = tlX;
		         prims.ft4->xy0.vy = tlY;
		         prims.ft4->xy1.vx = trX;
		         prims.ft4->xy1.vy = trY;
		         prims.ft4->xy2.vx = blX;
		         prims.ft4->xy2.vy = blY;
		         prims.ft4->xy3.vx = brX;
		         prims.ft4->xy3.vy = brY;

				  prims.ft4->xy0.vz = ZPtr[ coord0 ];
				  prims.ft4->xy1.vz = ZPtr[ coord0 ];
				  prims.ft4->xy2.vz = ZPtr[ coord0 ];
				  prims.ft4->xy3.vz = ZPtr[ coord0 ];
						prims.ft4->xy0.pad = 0;
						prims.ft4->xy1.pad = 0;
						prims.ft4->xy2.pad = 0;
						prims.ft4->xy3.pad = 0;



               SetPolyFT4( ( POLY_FT4* )prims.ptr );

               prims.ft4->colour0.r = 230; // PCwipeout
               prims.ft4->colour0.g = 0;
               prims.ft4->colour0.b = BLACK_NOT_DRAWN;
    
               tex = poly.spr->texture;

               prims.ft4->u0 = TextureTable[ tex ]->u0;
               prims.ft4->v0 = TextureTable[ tex ]->v0;

               prims.ft4->u1 = TextureTable[ tex ]->u1;
               prims.ft4->v1 = TextureTable[ tex ]->v1;

               prims.ft4->u2 = TextureTable[ tex ]->u2;
               prims.ft4->v2 = TextureTable[ tex ]->v2;

               prims.ft4->u3 = TextureTable[ tex ]->u3;
               prims.ft4->v3 = TextureTable[ tex ]->v3;

               prims.ft4->tsb = TextureTable[ tex ]->tsb;
               prims.ft4->cba = TextureTable[ tex ]->cba;

				newdepth = (depth >> 2) + ObjDepthMod;
				if(newdepth > 8191)
					newdepth = 8191;

               //AddPrim( OT[ CurrentScreen ] + newdepth, ( ulong* ) prims.prim );
               ClipPolyFT4( OT[ CurrentScreen ] + newdepth, ( ulong* ) prims.prim );

		  	   //prims.ft4 += 1;
	        }

            poly.spr += 1;
            break;

		case TypeBSPR:		

            coord0 = poly.spr->coord;

            depth = depthPtr[ coord0 ];
            x = screenPtr[ coord0 ].vx;
            y = screenPtr[ coord0 ].vy;

			if(x < -590 || x > 909)			// Bodge to eliminate
				break;						// BSPR messiness
			else if(y < -590 || y > 789)
				break;

            if( !( flagsPtr[ coord0] & ClipFlag ) )
	        {
		       xSize = ( 160 * poly.spr->width ) / depth; 
		       ySize = ( 160 * poly.spr->height ) / depth;

		       xSize2 = xSize >> 1;
		       ySize2 = ySize >> 1; 

               offsetX = ( ( xSize2 * rcos( roll ) ) >> 12 );
               offsetY = ( ( xSize2 * rsin( roll ) ) >> 12 );

               offset2X = ( ( ySize * rcos( 1024 - roll ) ) >> 12 );
               offset2Y = ( ( ySize * rsin( 1024 - roll ) ) >> 12 );

               blX = x - offsetX;
               blY = y - offsetY;

               brX = x + offsetX;
               brY = y + offsetY;

               tlX = blX + offset2X;
               tlY = blY - offset2Y;

               trX = brX + offset2X;
               trY = brY - offset2Y;

		         prims.ft4->xy0.vx = tlX;
		         prims.ft4->xy0.vy = tlY;
		         prims.ft4->xy1.vx = trX;
		         prims.ft4->xy1.vy = trY;
		         prims.ft4->xy2.vx = blX;
		         prims.ft4->xy2.vy = blY;
		         prims.ft4->xy3.vx = brX;
		         prims.ft4->xy3.vy = brY;


				  prims.ft4->xy0.vz = ZPtr[ coord0 ];
				  prims.ft4->xy1.vz = ZPtr[ coord0 ];
				  prims.ft4->xy2.vz = ZPtr[ coord0 ];
				  prims.ft4->xy3.vz = ZPtr[ coord0 ];
						prims.ft4->xy0.pad = 0;
						prims.ft4->xy1.pad = 0;
						prims.ft4->xy2.pad = 0;
						prims.ft4->xy3.pad = 0;

               SetPolyFT4( ( POLY_FT4* )prims.ptr );

               prims.ft4->colour0.r = 230; // PCwipeout
               prims.ft4->colour0.g = 0;
               prims.ft4->colour0.b = BLACK_NOT_DRAWN;
    
               tex = poly.spr->texture;

               prims.ft4->u0 = TextureTable[ tex ]->u0;
               prims.ft4->v0 = TextureTable[ tex ]->v0;

               prims.ft4->u1 = TextureTable[ tex ]->u1;
               prims.ft4->v1 = TextureTable[ tex ]->v1;

               prims.ft4->u2 = TextureTable[ tex ]->u2;
               prims.ft4->v2 = TextureTable[ tex ]->v2;

               prims.ft4->u3 = TextureTable[ tex ]->u3;
               prims.ft4->v3 = TextureTable[ tex ]->v3;

               prims.ft4->tsb = TextureTable[ tex ]->tsb;
               prims.ft4->cba = TextureTable[ tex ]->cba;

				newdepth = (depth >> 2) + ObjDepthMod;
				if(newdepth > 8191)
					newdepth = 8191;

               //AddPrim( OT[ CurrentScreen ] + newdepth, ( ulong* ) prims.prim );
               ClipPolyFT4( OT[ CurrentScreen ] + newdepth, ( ulong* ) prims.prim );

		  	   //prims.ft4 += 1;
	        }

            poly.spr += 1;
            break;

		default:
	      FntPrint( "bad prim\n" );
  			break;

      }
   }
}



void DrawObjectFixedAsm( Object* object, Skeleton* camera, int depth )
{
   DVECTOR*    screenPtr;
   ushort*     depthPtr;
   ushort*     flagsPtr;
   ushort*	   ZPtr;

   StdPoly     prim;
   Poly        poly;

   short       i;
   
   short       coord0;
   short       coord1;
   short       coord2;
   short       coord3;


   SVECTOR*    vertex;

   short       primitiveCount;
   short       vertexCount;


   /* for sprites */
   int 			     x, y;

   short 		     xSize;
   short 		     ySize;
   short 		     xSize2;
   short 		     ySize2;

   short             tlX; 
   short             tlY; 
   short             trX; 
   short             trY; 
   short             blX; 
   short             blY; 
   short             brX; 
   short             brY; 

   short             offsetX; 
   short             offsetY; 
   short             offset2X; 
   short             offset2Y; 

   short             tex;

   SetSkeletonTransform( object->skeleton );


   vertex = object->vertices;
   vertexCount = object->vertexCount;

   poly.prim = object->primitives;
   primitiveCount = object->primitiveCount;

	primCount += primitiveCount;


   if ( vertexCount <= 128 )
   {
      screenPtr = ( DVECTOR* )( FastRam );
      depthPtr = ( ushort* )( FastRam + 512 );
      flagsPtr = ( ushort* )( FastRam + 768 );
   }
   else
   {
      screenPtr = StaticScreen;
      depthPtr = StaticDepth;
      flagsPtr = StaticFlags;
   }
   ZPtr = StaticZ;


   RotTransPersN( vertex, screenPtr, depthPtr, flagsPtr, vertexCount , ZPtr);

		
   for ( i=0; i<primitiveCount; i++ )
   {
      switch ( poly.prim->type )
      {
         case TypeGT3 :

            coord0 = poly.gt3->coords[ 0 ];
            coord1 = poly.gt3->coords[ 1 ];
            coord2 = poly.gt3->coords[ 2 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyGT3( ( POLY_GT3* )prims.ptr );

                  prims.gt3->cba = poly.gt3->cba;
                  prims.gt3->tsb = poly.gt3->tsb;

                  prims.gt3->u0 = poly.gt3->u0;
                  prims.gt3->v0 = poly.gt3->v0;
                  prims.gt3->u1 = poly.gt3->u1;
                  prims.gt3->v1 = poly.gt3->v1;
                  prims.gt3->u2 = poly.gt3->u2;
                  prims.gt3->v2 = poly.gt3->v2;

                  prims.gt3->colour0.r = 230; // PCwipeout
                  prims.gt3->colour0.g = 0;
                  prims.gt3->colour0.b = 0;
                  prims.gt3->colour1 = poly.gt3->colour[ 1 ];
                  prims.gt3->colour2 = poly.gt3->colour[ 2 ];

                  //prims.gt3->xy0 = screenPtr[ coord0 ];
                  //prims.gt3->xy1 = screenPtr[ coord1 ];
                  //prims.gt3->xy2 = screenPtr[ coord2 ];
                  prims.gt3->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.gt3->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.gt3->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.gt3->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.gt3->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.gt3->xy2.vy = screenPtr[ coord2 ].vy;
				  prims.gt3->xy0.vz = ZPtr[coord0];
				  prims.gt3->xy1.vz = ZPtr[coord1];
				  prims.gt3->xy2.vz = ZPtr[coord2];
						prims.gt3->xy0.pad = 0;
						prims.gt3->xy1.pad = 0;
						prims.gt3->xy2.pad = 0;


                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyGT3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.gt3 += 1;
            }

		}
            poly.gt3 += 1;
            break;

         case TypeGT4 :

            coord0 = poly.gt4->coords[ 0 ];
            coord1 = poly.gt4->coords[ 1 ];
            coord2 = poly.gt4->coords[ 2 ];
            coord3 = poly.gt4->coords[ 3 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyGT4( ( POLY_GT4* )prims.ptr );

                  prims.gt4->cba = poly.gt4->cba;
                  prims.gt4->tsb = poly.gt4->tsb;

                  prims.gt4->u0 = poly.gt4->u0;
                  prims.gt4->v0 = poly.gt4->v0;
                  prims.gt4->u1 = poly.gt4->u1;
                  prims.gt4->v1 = poly.gt4->v1;
                  prims.gt4->u2 = poly.gt4->u2;
                  prims.gt4->v2 = poly.gt4->v2;
                  prims.gt4->u3 = poly.gt4->u3;
                  prims.gt4->v3 = poly.gt4->v3;

                  prims.gt4->colour0.r = poly.gt4->colour[ 0 ].r;
                  prims.gt4->colour0.g = poly.gt4->colour[ 0 ].g;
#if 0 //USE_ATI
                  prims.gt4->colour0.b = poly.gt4->colour[ 0 ].b;
#else

		if(trackNo == 4 || trackNo == 5)
			prims.gt4->colour0.b = BLACK_NOT_DRAWN;
		else
			prims.gt4->colour0.b = poly.gt4->colour[ 0 ].b;
#endif
#if 0
                  prims.gt4->colour0.r = 230; // PCwipeout
                  prims.gt4->colour0.g = 0;
                  prims.gt4->colour0.b = 0;
#endif
                  prims.gt4->colour1 = poly.gt4->colour[ 1 ];
                  prims.gt4->colour2 = poly.gt4->colour[ 2 ];
                  prims.gt4->colour3 = poly.gt4->colour[ 3 ];

                  //prims.gt4->xy0 = screenPtr[ coord0 ];
                  //prims.gt4->xy1 = screenPtr[ coord1 ];
                  //prims.gt4->xy2 = screenPtr[ coord2 ];
                  //prims.gt4->xy3 = screenPtr[ coord3 ];
                  prims.gt4->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.gt4->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.gt4->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.gt4->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.gt4->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.gt4->xy2.vy = screenPtr[ coord2 ].vy;
                  prims.gt4->xy3.vx = screenPtr[ coord3 ].vx;
                  prims.gt4->xy3.vy = screenPtr[ coord3 ].vy;
				  prims.gt4->xy0.vz = ZPtr[coord0];
				  prims.gt4->xy1.vz = ZPtr[coord1];
				  prims.gt4->xy2.vz = ZPtr[coord2];
				  prims.gt4->xy3.vz = ZPtr[coord3];
						prims.gt4->xy0.pad = 0;
						prims.gt4->xy1.pad = 0;
						prims.gt4->xy2.pad = 0;
						prims.gt4->xy3.pad = 0;


                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyGT4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.gt4 += 1;
               }

		}
            poly.gt4 += 1;
            break;

         case TypeFT3 :

            coord0 = poly.ft3->coords[ 0 ];
            coord1 = poly.ft3->coords[ 1 ];
            coord2 = poly.ft3->coords[ 2 ];


            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyFT3( ( POLY_FT3* )prims.ptr );

                  prims.ft3->cba = poly.ft3->cba;
                  prims.ft3->tsb = poly.ft3->tsb;

                  prims.ft3->u0 = poly.ft3->u0;
                  prims.ft3->v0 = poly.ft3->v0;
                  prims.ft3->u1 = poly.ft3->u1;
                  prims.ft3->v1 = poly.ft3->v1;
                  prims.ft3->u2 = poly.ft3->u2;
                  prims.ft3->v2 = poly.ft3->v2;

                  prims.ft3->colour0.r = 230; // PCwipeout
                  prims.ft3->colour0.g = 0;
                  prims.ft3->colour0.b = 0;

                  //prims.ft3->xy0 = screenPtr[ coord0 ];
                  //prims.ft3->xy1 = screenPtr[ coord1 ];
                  //prims.ft3->xy2 = screenPtr[ coord2 ];
                  prims.ft3->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.ft3->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.ft3->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.ft3->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.ft3->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.ft3->xy2.vy = screenPtr[ coord2 ].vy;
				  prims.ft3->xy0.vz = ZPtr[coord0];
				  prims.ft3->xy1.vz = ZPtr[coord1];
				  prims.ft3->xy2.vz = ZPtr[coord2];
						prims.ft3->xy0.pad = 0;
						prims.ft3->xy1.pad = 0;
						prims.ft3->xy2.pad = 0;


                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyFT3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.ft3 += 1;
               }
            }

            poly.ft3 += 1;
            break;

         case TypeFT4 :

            coord0 = poly.ft4->coords[ 0 ];
            coord1 = poly.ft4->coords[ 1 ];
            coord2 = poly.ft4->coords[ 2 ];
            coord3 = poly.ft4->coords[ 3 ];


            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyFT4( ( POLY_FT4* )prims.ptr );

                  prims.ft4->cba = poly.ft4->cba;
                  prims.ft4->tsb = poly.ft4->tsb;

                  prims.ft4->u0 = poly.ft4->u0;
                  prims.ft4->v0 = poly.ft4->v0;
                  prims.ft4->u1 = poly.ft4->u1;
                  prims.ft4->v1 = poly.ft4->v1;
                  prims.ft4->u2 = poly.ft4->u2;
                  prims.ft4->v2 = poly.ft4->v2;
                  prims.ft4->u3 = poly.ft4->u3;
                  prims.ft4->v3 = poly.ft4->v3;

                  prims.ft4->colour0.r = 230; // PCwipeout
                  prims.ft4->colour0.g = 0;
                  prims.ft4->colour0.b = 0;

                  //prims.ft4->xy0 = screenPtr[ coord0 ];
                  //prims.ft4->xy1 = screenPtr[ coord1 ];
                  //prims.ft4->xy2 = screenPtr[ coord2 ];
                  //prims.ft4->xy3 = screenPtr[ coord3 ];
                  prims.ft4->xy0.vx = screenPtr[ coord0 ].vx;
                  prims.ft4->xy0.vy = screenPtr[ coord0 ].vy;
                  prims.ft4->xy1.vx = screenPtr[ coord1 ].vx;
                  prims.ft4->xy1.vy = screenPtr[ coord1 ].vy;
                  prims.ft4->xy2.vx = screenPtr[ coord2 ].vx;
                  prims.ft4->xy2.vy = screenPtr[ coord2 ].vy;
                  prims.ft4->xy3.vx = screenPtr[ coord3 ].vx;
                  prims.ft4->xy3.vy = screenPtr[ coord3 ].vy;
				  prims.ft4->xy0.vz = ZPtr[coord0];
				  prims.ft4->xy1.vz = ZPtr[coord1];
				  prims.ft4->xy2.vz = ZPtr[coord2];
				  prims.ft4->xy3.vz = ZPtr[coord3];
						prims.ft4->xy0.pad = 0;
						prims.ft4->xy1.pad = 0;
						prims.ft4->xy2.pad = 0;
						prims.ft4->xy3.pad = 0;

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyFT4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.ft4 += 1;
            }

		}
            poly.ft4 += 1;
            break;

         case TypeG3 :

            coord0 = poly.g3->coords[ 0 ];
            coord1 = poly.g3->coords[ 1 ];
            coord2 = poly.g3->coords[ 2 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyG3( ( POLY_G3* )prims.ptr );

                  prims.g3->colour0.r = 230; // PCwipeout
                  prims.g3->colour0.g = 0;
                  prims.g3->colour0.b = 0;
                  prims.g3->colour1 = poly.g3->colour[ 1 ];
                  prims.g3->colour2 = poly.g3->colour[ 2 ];

                  prims.g3->xy0 = screenPtr[ coord0 ];
                  prims.g3->xy1 = screenPtr[ coord1 ];
                  prims.g3->xy2 = screenPtr[ coord2 ];

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyG3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.g3 += 1;
               }

			}
            poly.g3 += 1;
            break;

         case TypeG4 :

            coord0 = poly.g4->coords[ 0 ];
            coord1 = poly.g4->coords[ 1 ];
            coord2 = poly.g4->coords[ 2 ];
            coord3 = poly.g4->coords[ 3 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyG4( ( POLY_G4* )prims.ptr );

                  prims.g4->colour0.r = 230; // PCwipeout
                  prims.g4->colour0.g = 0;
                  prims.g4->colour0.b = 0;
                  prims.g4->colour1 = poly.g4->colour[ 1 ];
                  prims.g4->colour2 = poly.g4->colour[ 2 ];
                  prims.g4->colour3 = poly.g4->colour[ 3 ];

                  prims.g4->xy0 = screenPtr[ coord0 ];
                  prims.g4->xy1 = screenPtr[ coord1 ];
                  prims.g4->xy2 = screenPtr[ coord2 ];
                  prims.g4->xy3 = screenPtr[ coord3 ];

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyG4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.g4 += 1;
            }

			}
            poly.g4 += 1;
            break;

         case TypeF3 :

            coord0 = poly.f3->coords[ 0 ];
            coord1 = poly.f3->coords[ 1 ];
            coord2 = poly.f3->coords[ 2 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyF3( ( POLY_F3* )prims.ptr );

                  prims.f3->colour0.r = 230; // PCwipeout
                  prims.f3->colour0.g = 0;
                  prims.f3->colour0.b = 0;

                  prims.f3->xy0 = screenPtr[ coord0 ];
                  prims.f3->xy1 = screenPtr[ coord1 ];
                  prims.f3->xy2 = screenPtr[ coord2 ];

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyF3( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.f3 += 1;
               }

		}
            poly.f3 += 1;
            break;

         case TypeF4 :

            coord0 = poly.f4->coords[ 0 ];
            coord1 = poly.f4->coords[ 1 ];
            coord2 = poly.f4->coords[ 2 ];
            coord3 = poly.f4->coords[ 3 ];

            if ( !( ( flagsPtr[ coord0 ] | 
                      flagsPtr[ coord1 ] | 
                      flagsPtr[ coord2 ] | 
                      flagsPtr[ coord3 ] ))); // & ClipFlag ) )
            {
               if ( NormalClip( screenPtr[ coord0 ], 
                                screenPtr[ coord1 ], 
                                screenPtr[ coord2 ] ) > 0 )
               {
                  SetPolyF4( ( POLY_F4* )prims.ptr );

                  prims.f4->colour0.r = 230; // PCwipeout
                  prims.f4->colour0.g = 0;
                  prims.f4->colour0.b = 0;

                  prims.f4->xy0 = screenPtr[ coord0 ];
                  prims.f4->xy1 = screenPtr[ coord1 ];
                  prims.f4->xy2 = screenPtr[ coord2 ];
                  prims.f4->xy3 = screenPtr[ coord3 ];

                  //AddPrim( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );
                  ClipPolyF4( OT[ CurrentScreen ] + depth, ( ulong* ) prims.prim );

                  //prims.f4 += 1;
               }
            }

            poly.f4 += 1;
            break;

		default:
	      FntPrint( "bad prim\n" );
  			break;

      }
   }
}

