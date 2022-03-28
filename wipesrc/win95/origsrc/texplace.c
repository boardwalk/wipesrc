
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

#ifndef USE_ATI



void InitTexPlace( TexPlace *texPlace, TexTemplate* texTemplate )
{
   Texture     *texture;
   short       texIndex;
   short       i, j;


   texPlace->position = 0;
   texPlace->bank = 0;

   for ( i=0; i<8; i++ )
   {
      texIndex = i;

      if ( texIndex > texPlace->numLib - 1 )
      {
         texIndex = texPlace->emptyTexture;
         texture = TextureTable[ texIndex ];
      }
      else
      {
         texture = TextureTable[ texPlace->libraryTextures + texTemplate[texIndex].tex1[ 0 ]  ];
      }

      for ( j=0; j<2; j++ )
      {
      /* the actual icons */

         texPlace->textures[j][i].tag = 0;
		 if (TextureTrack)
			{
	         texPlace->textures[j][i].r0 = 230;	// PCwipeout
    	     texPlace->textures[j][i].g0 = BLUE_TRANSL;
        	 texPlace->textures[j][i].b0 = 0;
			}
 		else
		  {
	         texPlace->textures[j][i].r0 = BluePal;	// PCwipeout
    	     texPlace->textures[j][i].g0 = 0;
        	 texPlace->textures[j][i].b0 = 0;
		  }
         texPlace->textures[j][i].code = 0;
         texPlace->textures[j][i].x0 = TopLeftX + i*(SizeXY + 6);
         texPlace->textures[j][i].y0 = TopLeftY;
         texPlace->textures[j][i].u0 = texture->u0;
         texPlace->textures[j][i].v0 = texture->v0;
         texPlace->textures[j][i].clut = texture->cba;

         texPlace->textures[j][i].r1 = 190;
         texPlace->textures[j][i].g1 = 190;
         texPlace->textures[j][i].b1 = 190;
         texPlace->textures[j][i].p1 = 0;
         texPlace->textures[j][i].x1 = TopLeftX + i*(SizeXY+6) + SizeXY;
         texPlace->textures[j][i].y1 = TopLeftY;
         texPlace->textures[j][i].u1 = texture->u1;
         texPlace->textures[j][i].v1 = texture->v1;
         texPlace->textures[j][i].tpage = texture->tsb;

         texPlace->textures[j][i].r2 = 190;
         texPlace->textures[j][i].g2 = 190;
         texPlace->textures[j][i].b2 = 190;
         texPlace->textures[j][i].p2= 0;
         texPlace->textures[j][i].x2 = TopLeftX + i*(SizeXY + 6);
         texPlace->textures[j][i].y2 = TopLeftY + SizeXY;
         texPlace->textures[j][i].u2 = texture->u2;
         texPlace->textures[j][i].v2 = texture->v2;
         texPlace->textures[j][i].pad2 = 0;

         texPlace->textures[j][i].r3 = 190;
         texPlace->textures[j][i].g3 = 190;
         texPlace->textures[j][i].b3 = 190;
         texPlace->textures[j][i].p3= 0;
         texPlace->textures[j][i].x3 = TopLeftX + i*(SizeXY + 6) + SizeXY;
         texPlace->textures[j][i].y3 = TopLeftY + SizeXY;
         texPlace->textures[j][i].u3 = texture->u3;
         texPlace->textures[j][i].v3 = texture->v3;
         texPlace->textures[j][i].pad3 = 0;

         SetPolyGT4( &texPlace->textures[j][i] );
      }
   }


   for ( i=0; i<2; i++ )
   {
      /* the highlighter */

      texPlace->highlight[i].tag = 0;
      texPlace->highlight[i].r0 = 230; // PCwipeout
      texPlace->highlight[i].g0 = 0;
      texPlace->highlight[i].b0 = BLACK_NOT_DRAWN;
      texPlace->highlight[i].code = 0;
      texPlace->highlight[i].x0 = TopLeftX + 0*(SizeXY + 6);
      texPlace->highlight[i].y0 = TopLeftY;

      texPlace->highlight[i].r1 = 255;
      texPlace->highlight[i].g1 = 0;
      texPlace->highlight[i].b1 = 0;
      texPlace->highlight[i].pad1 = 0;
      texPlace->highlight[i].x1 = TopLeftX + 0*(SizeXY+6) + SizeXY;
      texPlace->highlight[i].y1 = TopLeftY;

      texPlace->highlight[i].r2 = 255;
      texPlace->highlight[i].g2 = 0;
      texPlace->highlight[i].b2 = 0;
      texPlace->highlight[i].pad2= 0;
      texPlace->highlight[i].x2 = TopLeftX + 0*(SizeXY + 6);
      texPlace->highlight[i].y2 = TopLeftY + SizeXY;

      texPlace->highlight[i].r3 = 255;
      texPlace->highlight[i].g3 = 0;
      texPlace->highlight[i].b3 = 0;
      texPlace->highlight[i].pad3= 0;
      texPlace->highlight[i].x3 = TopLeftX + 0*(SizeXY + 6) + SizeXY;
      texPlace->highlight[i].y3 = TopLeftY + SizeXY;

      SetPolyG4( &texPlace->highlight[i] );
	   SetSemiTrans( &texPlace->highlight[i], 1);
   }
}

#endif //USE_ATI


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


#ifndef USE_ATI


void TexPlaceUpdate( TexPlace *texPlace, TrackSection *section, short faceNum, TexTemplate *texTemplate, short copy )
{
   static ushort     pad;
   ushort            opad;
   ushort            xpad;

   short             flag;
   short             i;

   TrackSection      *prevSection;
   TrackSection      *nextSection;

   Face              *prevFace;
   Face              *nextFace;
   Face              *face;

   short             flip;

   char*             prim;

	opad=pad;
   pad = neg1.bits;
	pad = ~pad;
	xpad=(pad^opad)&pad;


   face = track->faces + section->faceStart + faceNum;

   
   AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &texPlace->highlight[CurrentScreen] );

   for ( i=0; i<8; i++ )
   {
      AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &texPlace->textures[CurrentScreen][i] );
   } 


/* move texture highlighter right */
   if ( xpad & PadRight0 )       
   {
      texPlace->position += 1;

      if ( texPlace->position > 7 )
      {
         texPlace->position = 0;

         texPlace->bank += 1;
         if ( texPlace->bank > texPlace->numLib / 8 )
         {
            texPlace->bank = 0;
         }

         NewBank( texPlace, texTemplate );
      }
   }
   else if ( xpad & PadLeft0 )       
   {
      texPlace->position -= 1 ;

      if ( texPlace->position < 0 )
      {
         texPlace->position = 7;

         texPlace->bank -= 1;
         if ( texPlace->bank < 0 )
         {
            texPlace->bank = texPlace->numLib / 8;
         }

         NewBank( texPlace, texTemplate );
      }
   }


/* apply selected texture to polygon */
   if ( xpad & PadTopLeft1 )      
   {
      if ( texPlace->bank*8 + texPlace->position > texPlace->numLib - 1 )
      {
//         printf( "No texture to add here\n" );
         /* empty */
      }
      else
      {
         face->texture = texPlace->bank*8 + texPlace->position;
         face->flags = face->flags & ~FlipTexture;

//         printf( "adding texture %d\n", face->texture );
      }
   }
   else if ( xpad & PadTopRight1 )      
   {
      if ( texPlace->bank*8 + texPlace->position > texPlace->numLib - 1 )
      {
//         printf( "No texture to add here\n" );
         /* empty */
      }
      else
      {
         face->texture = texPlace->bank*8 + texPlace->position;
         face->flags = face->flags | FlipTexture;

//         printf( "adding flipped texture %d\n", face->texture );
      }
   }




/* propogate selected texture down polygons */

   if ( xpad & PadTopLeft2 )           /* normal */
   {
      if ( texPlace->bank*8 + texPlace->position > texPlace->numLib - 1 )
      {
//         printf( "No texture to add here\n" );
         /* empty */
      }
      else
      {
//         printf( "adding multiple textures\n" );

         nextSection = section->nextSection.ptr;

         while ( section->faceCount == nextSection->faceCount )
         {
            face = track->faces + section->faceStart + faceNum;
            face->texture = texPlace->bank*8 + texPlace->position;
            face->flags = face->flags & ~FlipTexture;

            nextSection = nextSection->nextSection.ptr;
            section = section->nextSection.ptr;
         }

      /* fill in the last section */
         face = track->faces + section->faceStart + faceNum;
         face->texture = texPlace->bank*8 + texPlace->position;
         face->flags = face->flags & ~FlipTexture;

      }
   }  
   else if ( xpad & PadTopRight2 )           /* flipped */
   {
      if ( texPlace->bank*8 + texPlace->position > texPlace->numLib - 1 )
      {
//         printf( "No texture to add here\n" );
         /* empty */
      }
      else
      {
//         printf( "adding multiple flipped textures\n" );

         nextSection = section->nextSection.ptr;

         while ( section->faceCount == nextSection->faceCount )
         {
            face = track->faces + section->faceStart + faceNum;
            face->texture = texPlace->bank*8 + texPlace->position;
            face->flags = face->flags | FlipTexture;

            nextSection = nextSection->nextSection.ptr;
            section = section->nextSection.ptr;
         }

      /* fill in the last section */
         face = track->faces + section->faceStart + faceNum;
         face->texture = texPlace->bank*8 + texPlace->position;
         face->flags = face->flags | FlipTexture;

      }
   }




/* copy previous section textures to current section */
   
   if ( copy )
   {
      if ( xpad & PadTriangle0 )   
      {
//         printf( "copying previous section textures to current section \n" );

         prevSection = section->prevSection.ptr;

         if ( prevSection->faceCount != section->faceCount )
         {
//            printf( "Previous section must have same number of faces as section\n" );
         }
         else
         {
            face = track->faces + section->faceStart;
            prevFace = track->faces + prevSection->faceStart;

            for (i=0; i<section->faceCount; i++)
            {
               face->texture = prevFace->texture;
               if ( prevFace->flags & FlipTexture )
               {
                  face->flags |= FlipTexture;                                 
                  flip = 1;
               }
               else
               {
                  face->flags &= ~FlipTexture;                                 
                  flip = 0;
               }

               face++;
               prevFace++;
            }
         }
      }
   }


/* copy forward section textures to current section */
   
   if ( copy )
   {
      if ( xpad & PadCross0 )   
      {
//         printf( "copying forward section textures to current section \n" );

         nextSection = section->nextSection.ptr;

         if ( nextSection->faceCount != section->faceCount )
         {
//            printf( "Next section must have same number of faces as section\n" );
         }
         else
         {
            face = track->faces + section->faceStart;
            nextFace = track->faces + nextSection->faceStart;

            for (i=0; i<section->faceCount; i++)
            {
               face->texture = nextFace->texture;
               if ( nextFace->flags & FlipTexture )
               {
                  face->flags |= FlipTexture;                                 
                  flip = 1;
               }
               else
               {
                  face->flags &= ~FlipTexture;                                 
                  flip = 0;
               }

               face++;
               nextFace++;
            }
         }
      }
   }



/* calc new position for left - right selector */
   for ( i=0; i<2; i++ )
   {
      texPlace->highlight[i].x0 = TopLeftX + texPlace->position * (SizeXY + 6);
      texPlace->highlight[i].x1 = TopLeftX + texPlace->position * (SizeXY+6) + SizeXY;
      texPlace->highlight[i].x2 = TopLeftX + texPlace->position * (SizeXY + 6);
      texPlace->highlight[i].x3 = TopLeftX + texPlace->position * (SizeXY + 6) + SizeXY;
   }     
}


#endif //USE_ATI





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






#ifndef USE_ATI



Object *CreateTexHighlight( )
{
   Object*        object;
   G4*            primitive;
   PolyG4*        g4;
   short          i;


   object = (Object*) DAlloc( heap, sizeof( Object ) );
   if ( !object )
   {
      Error( "Error::Main.c:CreateTexHighlight - No memory for object", Fatal );
   }
   
   primitive = ( G4* )DAlloc( heap, sizeof( G4 ) );
   if ( !primitive )
   {
      Error( "Error::Main.c:CreateTexHighlight - No memory for primitive", Fatal );
   }
 

   object->vertices = NULL;

   primitive->type = TypeG4;


#if 0
   SetPolyG4( (POLY_G4*)( &primitive->quad[0] ) );
   SetSemiTrans( ( POLY_G4* ) &primitive->quad[0], 1);

   SetPolyG4( (POLY_G4*)( &primitive->quad[1] ) );
   SetSemiTrans( ( POLY_G4* ) &primitive->quad[1], 1);
#endif

   object->vertexCount = 4 ;     

   object->normalCount = 0;
   object->normals = NULL;

   object->primitiveCount = 1;
   object->primitives = (Primitive*)primitive;

   object->bspTree = NULL;
   object->skeleton = NewSkeleton( );
   object->next = NULL;

   return ( object );
}

#endif //USE_ATI

#ifndef USE_ATI



void TexPlaceControl( TrackCamera *camera, short *face, Object *texHighlight, TrackSection *nextSection )
{
   SVECTOR           highlightVertices[4];
   short             i;
   


/* remember these are back to front */
   ((G4*)texHighlight->primitives)->coords[0] = 1;
   ((G4*)texHighlight->primitives)->coords[1] = 0;
   ((G4*)texHighlight->primitives)->coords[2] = 2;
   ((G4*)texHighlight->primitives)->coords[3] = 3;

   ((G4*)texHighlight->primitives)->flag |= Translucent;


   ((G4*)texHighlight->primitives)->colour[ 0 ].r = 255;
   ((G4*)texHighlight->primitives)->colour[ 0 ].g = 0;
   ((G4*)texHighlight->primitives)->colour[ 0 ].b = 0;

   ((G4*)texHighlight->primitives)->colour[ 1 ].r = 255;
   ((G4*)texHighlight->primitives)->colour[ 1 ].g = 0;
   ((G4*)texHighlight->primitives)->colour[ 1 ].b = 0;

   ((G4*)texHighlight->primitives)->colour[ 2 ].r = 255;
   ((G4*)texHighlight->primitives)->colour[ 2 ].g = 0;
   ((G4*)texHighlight->primitives)->colour[ 2 ].b = 0;

   ((G4*)texHighlight->primitives)->colour[ 3 ].r = 255;
   ((G4*)texHighlight->primitives)->colour[ 3 ].g = 0;
   ((G4*)texHighlight->primitives)->colour[ 3 ].b = 0;


   highlightVertices[0].vx = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 0 ]].vx - nextSection->centre.vx;
   highlightVertices[0].vy = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 0 ]].vy - nextSection->centre.vy;
   highlightVertices[0].vz = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 0 ]].vz - nextSection->centre.vz;

   highlightVertices[1].vx = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 1 ]].vx - nextSection->centre.vx;
   highlightVertices[1].vy = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 1 ]].vy - nextSection->centre.vy;
   highlightVertices[1].vz = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 1 ]].vz - nextSection->centre.vz;

   highlightVertices[2].vx = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 2 ]].vx - nextSection->centre.vx;
   highlightVertices[2].vy = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 2 ]].vy - nextSection->centre.vy;
   highlightVertices[2].vz = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 2 ]].vz - nextSection->centre.vz;

   highlightVertices[3].vx = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 3 ]].vx - nextSection->centre.vx;
   highlightVertices[3].vy = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 3 ]].vy - nextSection->centre.vy;
   highlightVertices[3].vz = track->vertices[(track->faces + nextSection->faceStart + *face)->vertex[ 3 ]].vz - nextSection->centre.vz;

   texHighlight->vertices = highlightVertices;
   SetSkeletonPosition( texHighlight->skeleton, nextSection->centre.vx, nextSection->centre.vy, nextSection->centre.vz );
   SetSkeletonDirection( texHighlight->skeleton, 0, 0, 0 );
   texHighlight->skeleton->update = 1;


   DrawObjectFixedAsm( texHighlight, camera->camPos, 0 );
}


#endif //USE_ATI






