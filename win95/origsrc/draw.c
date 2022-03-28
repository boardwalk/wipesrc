#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "file.h"
#include "scene.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "tilelib.h"
#include "ttf.h"
#include "texplace.h"
#include "trakview.h"
#include "trackasm.h"
#include "effects.h"
#include "fileops.h"
#include "menus.h"  
#include "hud.h"
#include "hiscores.h"
#include "fri.h"


extern TrackCamera		ctrlcamera;



char TrackDirection
( 
	TrackCamera*		camera 
)
{
	SVECTOR        reference;
	SVECTOR        toCentre;
	long           angle;
   long           forwardAngle;
   SVECTOR        forwards;
   TrackSection   *nextSection;
   TrackSection   *startSection;


   nextSection = camera->section->nextSection.ptr;

	reference.vx = (short)(nextSection->centre.vx - camera->section->centre.vx );
	reference.vy = (short)(nextSection->centre.vy - camera->section->centre.vy );
	reference.vz = (short)(nextSection->centre.vz - camera->section->centre.vz );

	angle = -ratan2(reference.vx, reference.vz) + camera->hdg ;
	angle = ang(angle);

	if((angle >= -512) && (angle < 512))
	{
      return (ShipForwards);
	}
	else if((angle >= 512) && (angle < 1536))
	{
      return (ShipRight);
	}
	else if((angle >= 1536) || (angle < -1536))
	{
      return (ShipBackwards);
	}
	else
	{
      return (ShipLeft);
	}
}





long  count = 0;


long  c = 0;

void DrawLandscape
( 
	ShipData			*playerShip, 
	TrackCamera		*camera
)
{
   short          i;
	VECTOR         result;
	long           flag;
	SVECTOR        toCentre;
   MATRIX*        matrix;
	TrackSection* 	section; 
	long				extent;
   SVECTOR        centre;

   for ( i=0; i<MaxUnique; i++ )
   {
      sharedPrimCount[ i ] = 0;
   }

	section = camera->section;

/*	
   FntPrint(" section->objectCount = %d \n", section->objectCount );
*/

	if ( section->objectCount > 0 )
	{
	   for ( i=0; i<section->objectCount; i++ )
 	  	{
         objectTable[ section->objects[ i ] ]->skeleton->update = 1;

         SetSkeletonTransform( objectTable[ section->objects[ i ] ]->skeleton );

			extent = objectTable[ section->objects[ i ] ]->extent;

         centre.vx = 0;
         centre.vy = 0;
         centre.vz = 0;

  			RotTrans( &centre, &result, &flag );

         result.vz += extent;

         if ( result.vz == 0 ) result.vz = 1;

         result.vx = ( result.vx * 160 ) / result.vz;
         result.vy = ( result.vy * 160 ) / result.vz;

         extent = ( extent * 160 ) / result.vz;

         if 
         (
            ( result.vx >= -( extent + 160 ) ) && 
            ( result.vx <=  ( extent + 160 ) ) &&
            ( result.vy >= -( extent + 120 ) ) &&
            ( result.vy <=  ( extent + 120 ) )
         )
         {

#if 1

            if ( playerShip->attr & VIEW_IN )
            {
               DrawObjectAsm( objectTable[ section->objects[ i ] ], ctrlcamera.camPos, playerShip->roll, 0 );
            }
            else
            {
               DrawObjectAsm( objectTable[ section->objects[ i ] ], ctrlcamera.camPos, 0, 0 );
            }

#else

            if ( playerShip->attr & VIEW_IN )
            {
      		   TransformLibraryObject( objectTable[ section->objects[ i ] ], &glo.camPos, playerShip->roll );
            }
            else
            {
      		   TransformLibraryObject( objectTable[ section->objects[ i ] ], &glo.camPos, 0 );
            }

#endif          
         }      
		}         
	}            
                
   count += 1;  
                
}               
                     
