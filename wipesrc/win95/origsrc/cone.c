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
#include "cone.h"





void InitCone ( Cone *cone )
{
   short       h, i, j, k;
   Prm         prm;

   short       seCount;
   short       similar;

   short       booster;

   short       allVert[ 64 ];
   short       allCount;

   short       shared[ 3 ];
   short       gotIt;


   for ( h=0; h<8; h++ )
   {
      seCount = 0;
      allCount = 0;
      prm.primitive = cone->object[ h ]->primitives;

      for ( i=0; i<cone->object[ h ]->primitiveCount; i++ )
      {
         switch ( prm.f3->type )
         {
            case TypeF3 :
               if ( prm.f3->flag & ShipEngine )
               {
                  Error ( "F3 ::SE marked polys should be ft3's", Warning );
               }
               prm.f3 += 1;
               break;

            case TypeF4 :
               if ( prm.f4->flag & ShipEngine )
               {
                  Error ( "F4 ::SE marked polys should be ft3's", Fatal );
               }
               prm.f4 += 1;
               break;

            case TypeFT3 :
               if ( prm.ft3->flag & ShipEngine )
               {
                  allVert[ allCount++ ] = prm.ft3->coords[ 0 ];
                  allVert[ allCount++ ] = prm.ft3->coords[ 1 ];
                  allVert[ allCount++ ] = prm.ft3->coords[ 2 ];

                  prm.ft3->flag |= Translucent;                   

                  prm.ft3->tsb =
                     GetTPage
                     (
                        0,
                        0,
                        TextureTable[ prm.ft3->texture ]->textureX,
                        TextureTable[ prm.ft3->texture ]->textureY
                     );

                  prm.ft3->colour.r = 230;
                  prm.ft3->colour.g = 0 ;
                 prm.ft3->colour.b = UNUSED1_TRANSL;
//                  prm.ft3->colour.r = 116;
//                  prm.ft3->colour.g = 97 ;
//                  prm.ft3->colour.b = 103;
                  seCount ++;
               }
               prm.ft3 += 1;
               break;

            case TypeFT4 :
               if ( prm.ft4->flag & ShipEngine )
               {
                  Error ( "FT4 ::SE marked polys should be ft3's", Fatal );
               }
               prm.ft4 += 1;
               break;

            case TypeG3 :
               if ( prm.g3->flag & ShipEngine )
               {
                  Error ( "G3 ::SE marked polys should be ft3's", Fatal );
               }
               prm.g3 += 1;
               break;

            case TypeG4 :
               if ( prm.g4->flag & ShipEngine )
               {
                  Error ( "G4 ::SE marked polys should be ft3's", Fatal );
               }
               prm.g4 += 1;
               break;

            case TypeGT3 :
               if ( prm.gt3->flag & ShipEngine )
               {
//						 printf("G");
                  allVert[ allCount++ ] = prm.gt3->coords[ 0 ];
                  allVert[ allCount++ ] = prm.gt3->coords[ 1 ];
                  allVert[ allCount++ ] = prm.gt3->coords[ 2 ];

                  prm.gt3->flag |= Translucent;

                  prm.gt3->tsb =
                     GetTPage
                     (
                        0,
                        0,
                        TextureTable[ prm.gt3->texture ]->textureX,
                        TextureTable[ prm.gt3->texture ]->textureY
                     );
                  for ( j=0; j<3; j++ )
                  {
                     prm.gt3->colour[ j ].r = 230;
                     prm.gt3->colour[ j ].g = 0 ;
                   prm.gt3->colour[ j ].b = UNUSED1_TRANSL;

//                     prm.gt3->colour[ j ].r = 116;
//                     prm.gt3->colour[ j ].g = 97 ;
//                     prm.gt3->colour[ j ].b = 103;
                  }

                  seCount ++;
               }
               prm.gt3 += 1;
               break;

            case TypeGT4 :
               if ( prm.gt4->flag & ShipEngine )
               {
                  Error ( "GT3 ::SE marked polys should be ft3's", Fatal );
               }
               prm.gt4 += 1;
               break;

            default :
               sprintf( errorString, "cone.c::InitCone:Bad primitive type %x\n", prm.f3->type );
               Error( errorString, Fatal );
               break;
         }
      }


   /* get out the centre vertex */

      cone->coneIndex[ 0 ][ h ] = -1;
      cone->coneIndex[ 1 ][ h ] = -1;
      cone->coneIndex[ 2 ][ h ] = -1;

      shared[ 0 ] = -1;
      shared[ 1 ] = -1;
      shared[ 2 ] = -1;


      booster = 0;
      for ( i=0; ( i<allCount ) && ( booster < 3 ); i++ )
      {
         similar = 0;
         for ( j=0; j<allCount; j++ )
         {
            if ( allVert[ i ] == allVert[ j ] )
            {
               similar++;
               if ( similar > 3 )
               {
                  gotIt = 0;
                  for ( k=0; k<3; k++ )
                  {
                     if ( shared[ k ] == allVert[ i ] )
                     {
                        gotIt = 1;
                     }
                  }

                  if ( !gotIt )
                  {
                     shared[ booster ] = allVert[ i ];
                     booster++;
                  }
               }
            }
         }
      }

      for ( j=0; j<3; j++ )
      {
         cone->coneIndex[ j ][ h ] = shared[ j ];

         if ( shared[ j ] != -1 )
         {
            cone->oldVertex[ j ][ h ] = cone->object[ h ]->vertices[ shared[ j ] ];
         }
      }
   }
}



void UpdateCone ( Cone *cone )
{
   short    disp;
   short    i, j;
   

   for ( i=0; i<8; i++ )
   {
      if ( i == ownShip )
      {
      /* get the z displacement related to speed or thrust */
         disp = cone->ship[ i ]->thrust_mag >> 4;
         disp += cone->ship[ i ]->speed >> 8;

      }
      else
      {
      /* for remote ships the z displacement is a constant */
         disp = 150;
      }


   /* move the centres */
      for ( j=0; j<3; j++ )
      {
         if ( cone->coneIndex[ j ][ i ] != -1 )
         {
            cone->object[ i ]->vertices[ cone->coneIndex[ j ][ i ] ].vz = cone->oldVertex[ j ][ i ].vz - disp + ( ( randy() - 16383 ) >> 9 );
            cone->object[ i ]->vertices[ cone->coneIndex[ j ][ i ] ].vx = cone->oldVertex[ j ][ i ].vx + ( ( randy() - 16383 ) >> 11 );
            cone->object[ i ]->vertices[ cone->coneIndex[ j ][ i ] ].vy = cone->oldVertex[ j ][ i ].vy + ( ( randy() - 16383 ) >> 11 );
         }
      }
   }
}




