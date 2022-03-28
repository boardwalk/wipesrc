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





void InitSprites( Sprites *sprites, Skeleton *camPos )
{
   short       i;
   short       tex = 370;


  	sprites->skeleton = NewSkeleton( );
   sprites->skeleton->super = camPos;


   for ( i=0; i<2; i++ )
   {
      sprites->sprite[ 0 ][ i ].r0 = 230; // PCwipeout
      sprites->sprite[ 0 ][ i ].g0 = 0;
      sprites->sprite[ 0 ][ i ].b0 = BLACK_NOT_DRAWN;

      sprites->sprite[ 0 ][ i ].u0 = TextureTable[ tex ]->u0;
      sprites->sprite[ 0 ][ i ].v0 = TextureTable[ tex ]->v0;

      sprites->sprite[ 0 ][ i ].u1 = TextureTable[ tex ]->u1;
      sprites->sprite[ 0 ][ i ].v1 = TextureTable[ tex ]->v1;

      sprites->sprite[ 0 ][ i ].u2 = TextureTable[ tex ]->u2;
      sprites->sprite[ 0 ][ i ].v2 = TextureTable[ tex ]->v2;

      sprites->sprite[ 0 ][ i ].u3 = TextureTable[ tex ]->u3;
      sprites->sprite[ 0 ][ i ].v3 = TextureTable[ tex ]->v3;

      sprites->sprite[ 0 ][ i ].tpage = TextureTable[ tex ]->tsb;
      sprites->sprite[ 0 ][ i ].clut = TextureTable[ tex ]->cba;
                            
      SetPolyFT4( &sprites->sprite[ 0 ][ i ] );
   }

}


void AddSprites( Sprites *sprites, TrackSection *section )
{


}




#define  HEIGHT 256
#define  WIDTH 256

void UpdateSprites( Sprites *sprites, TrackSection *section, short roll )
{

	SVECTOR 		   vertex;
	DVECTOR 		   sxy;
	long 			   p, flag;
	int 			   x, y;
	long 			   depth;
	short 		   xSize;
	short 		   ySize;
	short 		   xSize2;
	short 		   ySize2;

   short          tlX; 
   short          tlY; 
   short          trX; 
   short          trY; 
   short          blX; 
   short          blY; 
   short          brX; 
   short          brY; 

   short          offsetX; 
   short          offsetY; 
   short          offset2X; 
   short          offset2Y; 

   Face           *face;

   face = track->faces + section->faceStart + 1;

	SetSkeletonPosition( sprites->skeleton,
                        track->vertices[(face->vertex[2])].vx,
                        track->vertices[(face->vertex[2])].vy,
                        track->vertices[(face->vertex[2])].vz );

   SetSkeletonDirection( sprites->skeleton, 0, 0, 0 );

   sprites->skeleton->update = 1;

	SetSkeletonTransform( sprites->skeleton );

	vertex.vx = 0;
	vertex.vy = 0;
	vertex.vz = 0;

   depth = RotTransPers(&vertex,
		   					&sxy,
			   				&p,
								&flag);

	x = sxy.vx;
	y = sxy.vy;

   if( !( flag & ( CrtClipped | ZNegative | ZLarge | ZSmall ) ) )
	{
		xSize = ( 160 * HEIGHT ) / depth;
		ySize = ( 160 * WIDTH ) / depth;

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


		sprites->sprite[ 0 ][ CurrentScreen ].x0 = tlX;
		sprites->sprite[ 0 ][ CurrentScreen ].y0 = tlY;    
		sprites->sprite[ 0 ][ CurrentScreen ].x1 = trX;
		sprites->sprite[ 0 ][ CurrentScreen ].y1 = trY;
		sprites->sprite[ 0 ][ CurrentScreen ].x2 = blX;
		sprites->sprite[ 0 ][ CurrentScreen ].y2 = blY;
		sprites->sprite[ 0 ][ CurrentScreen ].x3 = brX;
		sprites->sprite[ 0 ][ CurrentScreen ].y3 = brY;

   	AddPrim( OT[ CurrentScreen ] + (depth>>1) , ( ulong* ) &(sprites->sprite[ 0 ][ CurrentScreen ]) );
	}


}
