#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ttf.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "main.h"
#include "sound.h"
#include "dynam.h"
#include "combat.h"
#include "menus.h"
#include "hud.h"
#include "sparks2.h"
#include "global.h"
#include "editor.h"
#include "texplace.h"
#include "trakview.h"
#include "sprite.h"


#ifndef USE_ATI


void CreateSparks3D( Skeleton *camPos )
{
   short          i;



   for( i=0; i<Max3DLineSkels; i++ )
   {
 	   lineSkel[ i ].skeleton = NewSkeleton( );
      lineSkel[ i ].skeleton->super = camPos;

      lineSkel[ i ].count = 0;
      lineSkel[ i ].used = 0;
   }
   
   
   for( i=0; i<Max3DLines; i++ )
   {
      line3d[ i ].line[0].r0 = 230; // PCwipeout
      line3d[ i ].line[0].g0 = 0;
      line3d[ i ].line[0].b0 = 0;

      line3d[ i ].line[0].r1 = 0xff;
      line3d[ i ].line[0].g1 = 0x00;
      line3d[ i ].line[0].b1 = 0x00;

      SetLineG2( (LINE_G2*)( &line3d[ i ].line[0] ) );

      line3d[ i ].line[1].r0 = 230; // PCwipeout
      line3d[ i ].line[1].g0 = 0;
      line3d[ i ].line[1].b0 = 0;

      line3d[ i ].line[1].r1 = 0xff;
      line3d[ i ].line[1].g1 = 0x00;
      line3d[ i ].line[1].b1 = 0x00;

      SetLineG2( (LINE_G2*)( &line3d[ i ].line[1] ) );


      line3d[ i ].used = 0;
      line3d[ i ].skel = -1;
   }
}

#endif //USE_ATI


#define MaxSparksPerSkel 20

#ifndef USE_ATI



void StartSparks3D( VECTOR *start, short numSparks, VECTOR *velocity )
{
   short          randa;
   short          randx;
   short          randdy;
   short          randz;
   short          currentSkel;
   short          i;
   short          count = 0;

#if 0
   printf( "vx = %d\n", velocity->vx );
   printf( "vy = %d\n", velocity->vy );
   printf( "vz = %d\n", velocity->vz );
#endif

   /* maximum of 20 sparks per skeleton */
   if ( numSparks > MaxSparksPerSkel )
   {
      numSparks = MaxSparksPerSkel;
   }

   /* find a free skeleton */
   currentSkel = -1;
   for ( i=0; ( i<Max3DLineSkels ) && ( currentSkel == -1 ); i++ )
   {
      if ( !lineSkel[ i ].used )
      {
         currentSkel = i;
         lineSkel[ currentSkel ].used = 1;
         lineSkel[ currentSkel ].count = numSparks;
      }
   }

   if ( currentSkel == -1 )
   {
#if 0
      Error( "No more spark currentSkeletons", Warning ); 
#endif
      return;
   }

   /* start off that skeletons sparks */
   for ( i=currentSkel*MaxSparksPerSkel; i < (currentSkel*MaxSparksPerSkel)+numSparks; i++ )
   {
      randa = randy() - 16383;
      randx = ( randy() - 16383 );
      randdy = ( randy() - 16383 );
      randz = ( randy() - 16383 );

      line3d[ i ].pos.vx = start->vx;
      line3d[ i ].pos.vy = start->vy;
      line3d[ i ].pos.vz = start->vz;

      line3d[ i ].vel.vx = ( ( randx ) * 128 );
	   line3d[ i ].vel.vy = ( ( randdy ) * 128 );
	   line3d[ i ].vel.vz = ( ( randz ) * 128 );

      line3d[ i ].accl= 0 * randa * 10; 
	   line3d[ i ].friction=1;			
	   line3d[ i ].gravity=65536 * 2;
	   line3d[ i ].wallRes=1;				 

	   line3d[ i ].life = 10 + ( ( i - currentSkel*MaxSparksPerSkel ) * 1 );

      SetSkeletonPosition( lineSkel[ currentSkel ].skeleton, start->vx, start->vy, start->vz );
      SetSkeletonDirection( lineSkel[ currentSkel ].skeleton, 0, 0, 0 );

      line3d[ i ].start.vx = start->vx;
	   line3d[ i ].start.vy = start->vy;
	   line3d[ i ].start.vz = start->vz;



   /* set the initial colours */
      line3d[ i ].line[0].r0 = 230; // PCwipeout
      line3d[ i ].line[0].g0 = 0;
      line3d[ i ].line[0].b0 = 0;

      line3d[ i ].line[0].r1 = 0xff;
      line3d[ i ].line[0].g1 = 0x00;
      line3d[ i ].line[0].b1 = 0x00;


      line3d[ i ].line[1].r0 = 230; // PCwipeout
      line3d[ i ].line[1].g0 = 0;
      line3d[ i ].line[1].b0 = 0;

      line3d[ i ].line[1].r1 = 0xff;
      line3d[ i ].line[1].g1 = 0x00;
      line3d[ i ].line[1].b1 = 0x00;


   /* set the used and skeleton flags */
      line3d[ i ].used = 1;
      line3d[ i ].skel = currentSkel;
   }
}
#endif //USE_ATI


#ifndef USE_ATI


void UpdateSparks3D( void )
{
   short          i, j;
	SVECTOR 		   vertex;
	DVECTOR 		   sxy;
   long           flag0, flag1;
	long 	   		depth0, depth1;
	long 			   p;
	long   			x0, y0;
	long   			x1, y1;
   long           xpos, ypos, zpos;
   short          calc=0;
   short          skel;



  	for( i=0; i<Max3DLineSkels; i++ )
   {
      if ( lineSkel[ i ].used )
      {
         skel = i;

         lineSkel[ skel ].skeleton->update = 1;

		   SetSkeletonTransform( lineSkel[ skel ].skeleton );

  	      for( j=skel*20; j < (skel*20)+MaxSparksPerSkel; j++ )
         {
            if ( ( line3d[ j ].used ) )
            {
               calc ++;

            /* calculate new velocity */
               line3d[ j ].vel.vy += line3d[ j ].accl;
               line3d[ j ].vel.vx += line3d[ j ].accl;
               line3d[ j ].vel.vz += line3d[ j ].accl;

            /* add gravity */
	            line3d[ j ].vel.vy += line3d[ j ].gravity;

            /* add velocities to the double coords */
	            line3d[ j ].pos.vx += ( line3d[ j ].vel.vx >> 15 );  /* 256*128 */
	            line3d[ j ].pos.vy += ( line3d[ j ].vel.vy >> 15 );
	            line3d[ j ].pos.vz += ( line3d[ j ].vel.vz >> 15 );

            /* get vertices relative to the centre */
		         vertex.vx = line3d[ j ].pos.vx - line3d[ j ].start.vx;
		         vertex.vy = line3d[ j ].pos.vy - line3d[ j ].start.vy;
		         vertex.vz = line3d[ j ].pos.vz - line3d[ j ].start.vz;

		         depth0 = RotTransPers( &vertex,
   						   		        &sxy,
	      							        &p,
			         				        &flag0 );

		         x0 = sxy.vx;
		         y0 = sxy.vy;

            /* get the trail coords */
	            xpos = line3d[ j ].pos.vx - ( line3d[ j ].vel.vx >> 15);
	            ypos = line3d[ j ].pos.vy - ( line3d[ j ].vel.vy >> 15);
	            zpos = line3d[ j ].pos.vz - ( line3d[ j ].vel.vz >> 15);

		         vertex.vx = xpos - line3d[ j ].start.vx;
		         vertex.vy = ypos - line3d[ j ].start.vy;
		         vertex.vz = zpos - line3d[ j ].start.vz;

		         depth1 = RotTransPers( &vertex,
   						  			        &sxy,
	      							        &p,
			         				        &flag1 );

		         x1 = sxy.vx;
		         y1 = sxy.vy;

               if( !( ( flag0 | flag1 ) & (CrtClipped | ZNegative | ZLarge | ZSmall)) )
		         {
			         line3d[ j ].line[ CurrentScreen ].x0 = x0;
			         line3d[ j ].line[ CurrentScreen ].y0 = y0;
                    
			         line3d[ j ].line[ CurrentScreen ].x1 = x1;
			         line3d[ j ].line[ CurrentScreen ].y1 = y1;

   		         AddPrim( OT[ CurrentScreen ] + (0) , ( ulong* ) &(line3d[ j ].line[ CurrentScreen ]) );
	            }


            /* change the colour */

            /* starts white */
               line3d[ j ].line[0].r0 = 230;	// PCwipeout
               line3d[ j ].line[0].g0 = 0;
               line3d[ j ].line[0].b0 = 0;

               line3d[ j ].line[0].r1 -= 0x3a;
               line3d[ j ].line[0].g1 -= 0x3a;
               line3d[ j ].line[0].b1 -= 0x3a;

            /* starts red */
               line3d[ j ].line[1].r0 = 230; // PCwipeout
               line3d[ j ].line[1].g0 = 0;
               line3d[ j ].line[1].b0 = 0;

               line3d[ j ].line[1].r1 += 0;
               line3d[ j ].line[1].g1 += 0x3a;
               line3d[ j ].line[1].b1 += 0x3a;



               line3d[ j ].life -= 1;
               if ( line3d[ j ].life <= 0 )
               {
                  line3d[ j ].used = 0;
                  line3d[ j ].skel = -1;
                  lineSkel[ skel ].count -= 1;

                  if ( lineSkel[ skel ].count <= 0 )
                  {
                     lineSkel[ skel ].used = 0;
                  }
               }

            }


         }

      }

   }

#if 0
   FntPrint( "calc = %d\n", calc );
#endif

}
#endif //USE_ATI


