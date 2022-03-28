
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
#include "scene.h"
#include "radscn.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"								  
#include "sound.h"
#include "sparks2.h"
#include "global.h"



   
/* reduce this later !!! */
#define ObjectListMax 128


void CalcNewRadius( TrackSection *section, Object* objects )
{

   TrackSection*  visSection;
   Object*        object;
   Object*        nextObject;
   MATRIX*        matrix;
	Vector*       	sectionCentre;
	VECTOR       	vector;
//   long long      distance; // PCwipeout
   long      distance;			 // PCwipeout
   short          i, j;
   short          objectCount;
   short          withinLocal;
   short          withinGlobal;
   short          allCount;
   short          pad;

   short          objectList[ ObjectListMax ];      


   DFree( heap, ( Ptr )section->objects );
   section->objectCount = 0;


   object = objects;
   objectCount = 0;
   allCount = 0;
   while( object )
   {
      withinLocal = 0;
      withinGlobal = 0;

      matrix = &object->skeleton->relative;
      sectionCentre = &section->centre;

      vector.vx = matrix->t[ 0 ] - sectionCentre->vx;
      vector.vy = matrix->t[ 1 ] - sectionCentre->vy;
      vector.vz = matrix->t[ 2 ] - sectionCentre->vz;


      if ( section->flags & Jump )
      {
         if ( ( abs( vector.vx ) < section->globalRadius * 2 ) &&
              ( abs( vector.vy ) < section->globalRadius * 2 ) &&
              ( abs( vector.vz ) < section->globalRadius * 2 ) )
         {
            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            distance = distance - object->extent;

            if ( distance < section->globalRadius )
            {
            /* within the global radius */
               withinGlobal = 1;
            }
         }
      }
      else
      {
         if ( ( abs( vector.vx ) < section->globalRadius ) &&
              ( abs( vector.vy ) < section->globalRadius ) &&
              ( abs( vector.vz ) < section->globalRadius ) )
         {
            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            distance = distance - object->extent;

            if ( distance < section->globalRadius )
            {
            /* within the global radius */
               withinGlobal = 1;
            }
         }
      }


      if ( withinGlobal )
      {
      /* Check distance forwards along the track */

         visSection = section;
         for ( j=0; ( !withinLocal ) && ( j < 30 ); j++ )
         {
            matrix = &object->skeleton->relative;
            sectionCentre = &visSection->centre;

            vector.vx = matrix->t[ 0 ] - sectionCentre->vx;
            vector.vy = matrix->t[ 1 ] - sectionCentre->vy;
            vector.vz = matrix->t[ 2 ] - sectionCentre->vz;

            if ( ( abs( vector.vx ) < section->localRadius ) &&
                 ( abs( vector.vy ) < section->localRadius ) &&
                 ( abs( vector.vz ) < section->localRadius ) )
            {
               distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                       ( vector.vy * vector.vy ) +
                                       ( vector.vz * vector.vz ) );

               distance = distance - object->extent;

               if ( distance < section->localRadius )
               {
               /* within the local radius */
                  withinLocal = 1;
               }
            }
            visSection = visSection->nextSection.ptr;
         }

      /* Check distance backwards along the track */

         visSection = section;
         for ( j=0; ( !withinLocal ) && ( j < 30 ); j++ )
         {
            matrix = &object->skeleton->relative;
            sectionCentre = &visSection->centre;

            vector.vx = matrix->t[ 0 ] - sectionCentre->vx;
            vector.vy = matrix->t[ 1 ] - sectionCentre->vy;
            vector.vz = matrix->t[ 2 ] - sectionCentre->vz;

            if ( ( abs( vector.vx ) < section->localRadius ) &&
                 ( abs( vector.vy ) < section->localRadius ) &&
                 ( abs( vector.vz ) < section->localRadius ) )
            {
               distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                       ( vector.vy * vector.vy ) +
                                       ( vector.vz * vector.vz ) );

               distance = distance - object->extent;

               if ( distance < section->localRadius )
               {
               /* within the local radius */
                  withinLocal = 1;
               }
            }
            visSection = visSection->prevSection.ptr;
         }

      }


      if ( withinLocal )
      {
         if ( objectCount < ObjectListMax )
         {
            objectList[ objectCount++ ] = allCount;            
         }
         else
         {
            Error( "Radius.c:SceneRadiusCheck: Maximum number of objects exceeded", Fatal );
         }
      }

      object = object->next;
      allCount++;
   }

   section->objects = ( short* )DAlloc(heap, objectCount * sizeof(short) );
   if (!section->objects)
   {
      Error( "Error:: radius.c: Not enough memory for section object list", Fatal );
   }          
   section->objectCount = objectCount;

   for ( j=0; j<objectCount; j++ )
   {
      section->objects[ j ] = objectList[ j ];   
   }

}



void SceneRadiusCheck( Object* objects )
{
   TrackSection*  section;
   TrackSection*  visSection;
   short          i, j;
   Object*        object;
   Object*        nextObject;
   MATRIX*        matrix;
	Vector*       	sectionCentre;
   short          objectCount;
	VECTOR       	vector;
//   long long      distance;
   long      distance; // PCwipeout
   short          objectList[ ObjectListMax ];      
   short          withinLocal;
   short          withinGlobal;
   short          allCount;



   /* allocate and fill in a global object table */

   object = objects;
   objectCount = 0;
   while( object )
   {
      object = object->next;
      objectCount++;
   }
#if 0
   printf( "num of objects = %d\n", objectCount );
#endif

   objectTable = ( Object** )DAlloc ( heap, ( objectCount + 1 ) * sizeof( Object* ) );
   if ( !objectTable )
   {
      Error( "radscn.c::SceneRadiusCheck(): No memory for global object table", Fatal );
   }

   object = objects;
   for ( i=0; i<objectCount; i++ )
   {
      objectTable[ i ] = object; 
      object = object->next;
   }



   /* do the radius check */

   section = track->sections;

   for ( i=0; i<track->sectionCount; i++ )
   {
#if 0
      printf( "," );
#endif
      object = objects;
      objectCount = 0;
      allCount = 0;
      while( object )
      {
         withinLocal = 0;
         withinGlobal = 0;

         matrix = &object->skeleton->relative;
         sectionCentre = &section->centre;

         vector.vx = matrix->t[ 0 ] - sectionCentre->vx;
         vector.vy = matrix->t[ 1 ] - sectionCentre->vy;
         vector.vz = matrix->t[ 2 ] - sectionCentre->vz;

         if ( section->flags & Jump )
         {
            if ( ( abs( vector.vx ) < ( section->globalRadius * 2 ) ) &&
                 ( abs( vector.vy ) < ( section->globalRadius * 2 ) ) &&
                 ( abs( vector.vz ) < ( section->globalRadius * 2 ) ) )
            {
               distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                       ( vector.vy * vector.vy ) +
                                       ( vector.vz * vector.vz ) );

               distance = distance - object->extent;

               if ( distance < section->globalRadius )
               {
               /* within the global radius */
                  withinGlobal = 1;
               }
            }
         }
         else
         {
            if ( ( abs( vector.vx ) < section->globalRadius ) &&
                 ( abs( vector.vy ) < section->globalRadius ) &&
                 ( abs( vector.vz ) < section->globalRadius ) )
            {
               distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                       ( vector.vy * vector.vy ) +
                                       ( vector.vz * vector.vz ) );

               distance = distance - object->extent;

               if ( distance < section->globalRadius )
               {
               /* within the global radius */
                  withinGlobal = 1;
               }
            }
         }


         if ( withinGlobal )
         {
         /* Check distance forwards along the track */

            visSection = section;
            for ( j=0; ( !withinLocal ) && ( j < 30 ); j++ )
            {
               matrix = &object->skeleton->relative;
               sectionCentre = &visSection->centre;

               vector.vx = matrix->t[ 0 ] - sectionCentre->vx;
               vector.vy = matrix->t[ 1 ] - sectionCentre->vy;
               vector.vz = matrix->t[ 2 ] - sectionCentre->vz;

               if ( ( abs( vector.vx ) < section->localRadius ) &&
                  ( abs( vector.vy ) < section->localRadius ) &&
                  ( abs( vector.vz ) < section->localRadius ) )
               {
                  distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                          ( vector.vy * vector.vy ) +
                                          ( vector.vz * vector.vz ) );

                  distance = distance - object->extent;

                  if ( distance < section->localRadius )
                  {
                  /* within the local radius */
                     withinLocal = 1;
                  }
               }
               visSection = visSection->nextSection.ptr;
            }

         /* Check distance backwards along the track */

            visSection = section;
            for ( j=0; ( !withinLocal ) && ( j < 30 ); j++ )
            {
               matrix = &object->skeleton->relative;
               sectionCentre = &visSection->centre;

               vector.vx = matrix->t[ 0 ] - sectionCentre->vx;
               vector.vy = matrix->t[ 1 ] - sectionCentre->vy;
               vector.vz = matrix->t[ 2 ] - sectionCentre->vz;

               if ( ( abs( vector.vx ) < section->localRadius ) &&
                  ( abs( vector.vy ) < section->localRadius ) &&
                  ( abs( vector.vz ) < section->localRadius ) )
               {
                  distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                          ( vector.vy * vector.vy ) +
                                          ( vector.vz * vector.vz ) );

                  distance = distance - object->extent;

                  if ( distance < section->localRadius )
                  {
                  /* within the local radius */
                     withinLocal = 1;
                  }
               }
               visSection = visSection->prevSection.ptr;
            }
         }


         if ( withinLocal )
         {
            if ( objectCount < ObjectListMax )
            {
               objectList[ objectCount++ ] = allCount;            
            }
            else
            {
               Error( "Radius.c:SceneRadiusCheck: Maximum number of objects exceeded", Fatal );
            }
         }

         object = object->next;
         allCount++;
      }

      section->objects = ( short* )DAlloc(heap, objectCount * sizeof(short) );
      if (!section->objects)
      {
         Error( "Error:: radius.c: Not enough memory for section object list", Fatal );
      }          
      section->objectCount = objectCount;

      for ( j=0; j<objectCount; j++ )
      {
         section->objects[ j ] = objectList[ j ];   
      }

#if 0
      printf( "List %d ::", i );
      for ( j=0; j<objectCount; j++ )
      {
         printf( "%6d", section->objects[ j ] );
      }
      printf( "\n" );
#endif

#if 0
      printf( "Track section %d object count %d\n", i, objectCount );
#endif
      section = NextIndexSection(section);
   }
}

                                                  


#ifndef USE_ATI


void RadiusControl( TrackCamera *camera, Object **prm )
{
   static ushort  pad;
   ushort         opad, xpad;


	opad=pad;
   pad = neg1.bits;
	pad = ~pad;
	xpad=(pad^opad)&pad;


//   if ( camera->section->junction.index != -1 )
//   {
//      FntPrint ( "Press start to enter junction\n" );
//   }


   if ( ( xpad & PadStart ) && ( camera->section->junction.index != -1 ) )
   {
      camera->section = camera->section->junction.ptr;
   }
   else if ( xpad & PadTriangle0 )
   {
      camera->section = camera->section->nextSection.ptr;
   }
   else if ( xpad & PadCross0 )
   {
      camera->section = camera->section->prevSection.ptr;
   }
   else if ( pad & PadCircle0 )
   {
      camera->section = camera->section->nextSection.ptr;
   }
   else if ( pad & PadSquare0 )
   {
      camera->section = camera->section->prevSection.ptr;
   }

   if ( 0 && camera->section->flags & Jump )
   {
      FntPrint( "\nCant change radius on jumps\n" );
   }
   else
   {
      if ( pad & PadTopLeft1 )             /* decrease local radius by 100 */
      {
         camera->section->localRadius-=1024;
         if ( camera->section->localRadius < 0 )
         {
            camera->section->localRadius = 0;
         }
         CalcNewRadius( camera->section, prm[0] );
      }
      else if ( pad & PadTopRight1 )       /* increase local radius by 100 */
      {
         camera->section->localRadius+=1024;
         if ( camera->section->localRadius < 0 )
         {
            camera->section->localRadius = 32767;
         }
         CalcNewRadius( camera->section, prm[0] );
      }
      else if ( pad & PadTopLeft2 )        /* decrease global radius by 1000 */
      {
         camera->section->globalRadius-=1024;
         if ( camera->section->globalRadius < 0 )
         {
            camera->section->globalRadius = 0;
         }
         CalcNewRadius( camera->section, prm[0] );
      }
      else if ( pad & PadTopRight2 )       /* increase global radius by 1000 */
      {
         camera->section->globalRadius+=1024;
         if ( camera->section->globalRadius < 0 )
         {
            camera->section->globalRadius = 32767;
         }
         CalcNewRadius( camera->section, prm[0] );
      }

      FntPrint( "Global Radius %d\n", camera->section->globalRadius );
      FntPrint( "Local  Radius %d\n", camera->section->localRadius );
   }

   FntPrint( "ObjectCount   %d\n", camera->section->objectCount );

   if ( pad & PadLeft0 )
   {
      camera->hdg -= 64;
   }

   if ( pad & PadRight0 )
   {
      camera->hdg += 64;
   }

   if ( pad & PadDown0 )
   {
      camera->pitch -= 64;
   }

   if ( pad & PadUp0 )
   {
      camera->pitch += 64;
   }
}

#endif //USE_ATI







