/* (C) Psygnosis 1994. By Jason Carl Denton & Rob Smith
*/


#include "standard.h"

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ttf.h"
#include "scene.h"
#include "main.h"
#include "editor.h"
#include "ships.h"
#include "droid.h"
#include "camera.h"
#include "texplace.h"
#include "weapon.h"
#include "control.h"
#include "draw.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"


#define TopLeftX 12
#define TopLeftY 15

#define SizeXY 32

extern	char	BluePal;


void NewBank( TexPlace *texPlace, TexTemplate* texTemplate )
{
   Texture     *texture;
   short       texIndex;
   short       i, j;

   for ( i=0; i<8; i++ )
   {
      texIndex = (texPlace->bank * 8) + i;

      if ( texIndex > texPlace->numLib - 1 )
      {
         texIndex = texPlace->emptyTexture;
         texture = TextureTable[ texIndex ];
      }
      else
      {
         texture = TextureTable[ texPlace->libraryTextures + texTemplate[ texIndex ].tex1[ 0 ]  ];
      }

      for ( j=0; j<2; j++ )
      {
         texPlace->textures[j][i].u0 = texture->u0;
         texPlace->textures[j][i].v0 = texture->v0;
         texPlace->textures[j][i].clut = texture->cba;

         texPlace->textures[j][i].u1 = texture->u1;
         texPlace->textures[j][i].v1 = texture->v1;
         texPlace->textures[j][i].tpage = texture->tsb;

         texPlace->textures[j][i].u2 = texture->u2;
         texPlace->textures[j][i].v2 = texture->v2;

         texPlace->textures[j][i].u3 = texture->u3;
         texPlace->textures[j][i].v3 = texture->v3;
      }
   }
}




void SetDefaultTrackTexture( )
{
   TrackSection      *section;
   Face              *face;
   short             i, j;

/* apply selected texture to polygon */

   section = track->sections;
   for ( i=0; i<track->sectionCount; i++ )
   {
      face = track->faces + section->faceStart;
      if ( section->faceCount == 4 )
      {
         for ( j=0; j<section->faceCount; j++ )
         {
            switch( j )
            {
               case 0:   
                  face->texture = 0;
                  break;
               case 1:   
                  face->texture = 1;
                  break;
               case 2:   
                  face->texture = 1;
                  face->flags = face->flags | FlipTexture;
                  break;
               case 3:   
                  face->texture = 0;
                  face->flags = face->flags | FlipTexture;
                  break;
            }
            face += 1;
         }
      }
      section += 1;
   }
}
