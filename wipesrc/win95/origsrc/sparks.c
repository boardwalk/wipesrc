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





Object* CreateSparks3D( short numSparks )
{
   Object*        object;
   LF2*           primitive;
   LineF2*        lf2;
   short          i;


   object = (Object*) DAlloc( heap, sizeof( Object ) );
   if ( !object )
   {
      Error( "Error::Main.c:CreateSparks - No memory for object", Fatal );
   }
   
   primitive = (LF2*) DAlloc( heap, sizeof( LF2 ) );
   if ( !primitive )
   {
      Error( "Error::Main.c:CreateSparks - No memory for primitive", Fatal );
   }
 
   primitive->line[0] = (LineF2*)DAlloc( heap, numSparks * sizeof (LineF2) );
   if ( !primitive->line[0] )
   {
      Error( "Error::Main.c:CreateSparks - No memory for primitives", Fatal );
   }

   primitive->line[1] = (LineF2*)DAlloc( heap, numSparks * sizeof (LineF2) );
   if ( !primitive->line[1] )
   {
      Error( "Error::Main.c:CreateSparks - No memory for primitives", Fatal );
   }

   object->vertices = ( SVECTOR* )DAlloc( heap, numSparks * 2 * sizeof( SVECTOR ) );
   if ( !object->vertices )
   {
      Error( "Error::Main.c:CreateSparks - No memory for vertices", Fatal );
   }

   primitive->type = TypeLF2;
   primitive->lines = numSparks;

   for( i=0; i<numSparks; i++ )
   {
      primitive->line[0][i].colour0.r = 0xdb;
      primitive->line[0][i].colour0.g = 0xa8;
      primitive->line[0][i].colour0.b = 0x18;

      SetLineF2( (LINE_F2*)( primitive->line[0] + i ) );

      primitive->line[1][i].colour0.r = 0xdb;
      primitive->line[1][i].colour0.g = 0xa8;
      primitive->line[1][i].colour0.b = 0x18;

      SetLineF2( (LINE_F2*)( primitive->line[1] + i ) );
   }


   object->vertexCount = numSparks * 2;       /* for now */

   object->normalCount = 0;
   object->normals = NULL;

   object->primitiveCount = 1;
   object->primitives = (Primitive*)primitive;

   object->bspTree = NULL;
   object->skeleton = NewSkeleton( );
   object->next = NULL;

   return ( object );
}



void StartSparks3D( Object* object, Line3D *data, VECTOR *start )
{
   short          randa;
   short          randx;
   short          randy;
   short          randz;
   short          i;

   for ( i=0; i<object->vertexCount; i+=2 )
   {
      object->vertices[i].vx = ( randy() >> 6 ) - 512;
      object->vertices[i].vy = ( randy() >> 6 ) - 512;
      object->vertices[i].vz = ( randy() >> 6 ) - 512;

      object->vertices[i+1].vx = object->vertices[i].vx;
      object->vertices[i+1].vy = object->vertices[i].vy;
      object->vertices[i+1].vz = object->vertices[i].vz;

      randa = randy();
      randx = randy();
      randy = randy();
      randz = randy();

      data[i].used = 1;
	   data[i].accl= (randa-16383)*1;  /*0x000a0000;*/
	   data[i].xvel= (randx-16383)*100; /* 0x0005f000;*/
	   data[i].yvel= (randy-16383)*100; /* 0x0005f000;*/
	   data[i].zvel= (randz-16383)*100; /* 0x0005f000;*/
	   data[i].friction=1;					/* divide by friction */
	   data[i].wallRes=1;					/* divide by */
	   data[i].gravity=0x00016000;

	   data[i].life=30;

      /* copy the x,y locations of the object into a  double */
#if 0
      data[i].xpos = object->vertices[i].vx;
	   data[i].ypos = object->vertices[i].vy;
      data[i].zpos = object->vertices[i].vz;
#else
      data[i].xpos = 0;
	   data[i].ypos = 0;
      data[i].zpos = 0;
#endif
      data[i].xpos*=(256*256);
	   data[i].ypos*=(256*256);
	   data[i].zpos*=(256*256);
   }

   SetSkeletonPosition( sparks->skeleton, start->vx, start->vy, start->vz );
   SetSkeletonDirection( sparks->skeleton, 0, 0, 0 );

}


void UpdateSparks3D( Object *sparks, Line3D *data )
{
   short          i;


   for (i=0; i<sparks->vertexCount; i+=2)
   {
      data[i].yvel+=data[i].accl;
      data[i].xvel+=data[i].accl;
      data[i].zvel+=data[i].accl;

#if 1
   /* add gravity */
	   data[i].yvel+=data[i].gravity;
#endif

   /* add velocities to the double coords */
	   data[i].xpos+=data[i].xvel;
	   data[i].ypos+=data[i].yvel;
	   data[i].zpos+=data[i].zvel;

   /* copy coords back again from the doubles */

	   sparks->vertices[i].vx = data[i].xpos/ (256*256);
	   sparks->vertices[i+1].vx = sparks->vertices[i].vx - ( data[i].xvel * 1 / (256*256) );

	   sparks->vertices[i].vy = data[i].ypos/ (256*256);
	   sparks->vertices[i+1].vy = sparks->vertices[i].vy - ( data[i].yvel * 1 / (256*256) );
                                                        
	   sparks->vertices[i].vz = data[i].zpos/ (256*256);
	   sparks->vertices[i+1].vz = sparks->vertices[i].vz - ( data[i].zvel * 1 / (256*256) );

	   data[i].life -= 1;
      if ( data[i].life == 0 )
      {
		   data->used = 0;
      }
   }

   sparks->skeleton->update = 1;
   TransformObjects( sparks, 0, 0 );
}






