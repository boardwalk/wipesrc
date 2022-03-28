 
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
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"



#define TopLeftX 288
#define TopLeftY 10

#define SizeXY 32


#define  AbrMask  0x9f

#ifndef USE_ATI



void InitEditor( Editor *editor )
{
   Texture     *texture;
   short       i, j, k;

/* first icon has a pop out toolbar */
   editor->toolBar[0].type = -1;
   editor->toolBar[0].popOut[0] = fCamera;
   editor->toolBar[0].popOut[1] = fTexSelect;
   editor->toolBar[0].popOut[2] = fDpa;
   editor->toolBar[0].popOut[3] = fRadius;
   editor->toolBar[0].popOut[4] = fTrakView;
   editor->toolBar[0].currentPopout = fDynamics;
   editor->toolBar[0].poppedOut = 0;
   editor->toolBar[0].leftPos = 0;
   editor->toolBar[0].numPopouts = 5;

/* second is a toggle */
   editor->toolBar[1].type = fTrans;
   editor->toolBar[1].onOff = 0;
/*
   editor->toolBar[1].type = -1;
   editor->toolBar[1].popOut[0] = fRadius;
   editor->toolBar[1].popOut[1] = fTexSelect;
   editor->toolBar[1].popOut[2] = fDpa;
   editor->toolBar[1].currentPopout = fTrans;
   editor->toolBar[1].poppedOut = 0;
   editor->toolBar[1].leftPos = 0;
   editor->toolBar[1].numPopouts = 3;
*/


/* third is a toggle */
   editor->toolBar[2].type = fBug;
   editor->toolBar[2].onOff = 0;

/* fourth is a toggle */
   editor->toolBar[3].type = fSky;
   editor->toolBar[3].onOff = 0;

/* fifth is a toggle */
   editor->toolBar[4].type = fLand;
   editor->toolBar[4].onOff = 0;

/* sixth is a toggle  ( for now ) */
   editor->toolBar[5].type = fSave;
   editor->toolBar[5].onOff = 0;

/* sixth is a toggle  ( for now ) */
   editor->toolBar[6].type = fDumpVram;
   editor->toolBar[6].onOff = 0;


/* editor is initially off */
   editor->status = 0;

/* highlighter position is initially at the top */
   editor->downPos = 0;

/* flag to tell whether the .trs has just been saved or not */
   editor->justSaved = 0;
   editor->justDumped = 0;

   for ( i=0; i<numTools; i++ )
   {
      texture = TextureTable[ editor->iconTextures + i ];

      for ( j=0; j<2; j++ )
      {
      /* the actual icons */

         editor->icons[j][i].tag = 0;
         editor->icons[j][i].r0 = 230;	// PCwipeout
         editor->icons[j][i].g0 = 0;
         editor->icons[j][i].b0 = 0;
         editor->icons[j][i].code = 0;
         editor->icons[j][i].x0 = TopLeftX + 0;
         editor->icons[j][i].y0 = TopLeftY + i*SizeXY;
         editor->icons[j][i].u0 = texture->u0;
         editor->icons[j][i].v0 = texture->v0;
         editor->icons[j][i].clut = texture->cba;

         editor->icons[j][i].r1 = 255;
         editor->icons[j][i].g1 = 255;
         editor->icons[j][i].b1 = 255;
         editor->icons[j][i].p1 = 0;
         editor->icons[j][i].x1 = TopLeftX + SizeXY;
         editor->icons[j][i].y1 = TopLeftY + i*SizeXY;
         editor->icons[j][i].u1 = texture->u1;
         editor->icons[j][i].v1 = texture->v1;
         editor->icons[j][i].tpage = texture->tsb;

         editor->icons[j][i].r2 = 255;
         editor->icons[j][i].g2 = 255;
         editor->icons[j][i].b2 = 255;
         editor->icons[j][i].p2= 0;
         editor->icons[j][i].x2 = TopLeftX + 0;
         editor->icons[j][i].y2 = TopLeftY + i*SizeXY + SizeXY;
         editor->icons[j][i].u2 = texture->u2;
         editor->icons[j][i].v2 = texture->v2;
         editor->icons[j][i].pad2 = 0;

         editor->icons[j][i].r3 = 255;
         editor->icons[j][i].g3 = 255;
         editor->icons[j][i].b3 = 255;
         editor->icons[j][i].p3= 0;
         editor->icons[j][i].x3 = TopLeftX + SizeXY;
         editor->icons[j][i].y3 = TopLeftY + i*SizeXY + SizeXY;
         editor->icons[j][i].u3 = texture->u3;
         editor->icons[j][i].v3 = texture->v3;
         editor->icons[j][i].pad3 = 0;

         SetPolyGT4( &editor->icons[j][i] );


      /* the always selected icons */

         editor->selected[j][i].tag = 0;
         editor->selected[j][i].r0 = 230; // PCwipeout
         editor->selected[j][i].g0 = 0;
         editor->selected[j][i].b0 = 0;
         editor->selected[j][i].code = 0;
         editor->selected[j][i].x0 = TopLeftX + 0;
         editor->selected[j][i].y0 = TopLeftY + i*SizeXY;

         editor->selected[j][i].r1 = 0;
         editor->selected[j][i].g1 = 0;
         editor->selected[j][i].b1 = 255;
         editor->selected[j][i].pad1 = 0;
         editor->selected[j][i].x1 = TopLeftX + SizeXY;
         editor->selected[j][i].y1 = TopLeftY + i*SizeXY;
                            
         editor->selected[j][i].r2 = 0;
         editor->selected[j][i].g2 = 0;
         editor->selected[j][i].b2 = 255;
         editor->selected[j][i].pad2 = 0;
         editor->selected[j][i].x2 = TopLeftX + 0;
         editor->selected[j][i].y2 = TopLeftY + i*SizeXY + SizeXY;
                            
         editor->selected[j][i].r3 = 0;
         editor->selected[j][i].g3 = 0;
         editor->selected[j][i].b3 = 255;
         editor->selected[j][i].pad3 = 0;
         editor->selected[j][i].x3 = TopLeftX + SizeXY;
         editor->selected[j][i].y3 = TopLeftY + i*SizeXY + SizeXY;
                            
         SetPolyG4( &editor->selected[j][i] );
	      SetSemiTrans( ( POLY_G4* ) &editor->selected[j][i], 1);
      }

   }


   /* the rest of the popouts */

   for ( i=0; i<numTools; i++ )
   {
      for ( j=0; j<2; j++ )
      {
         for ( k=0; k<maxPopouts; k++ )
         {
            editor->toolBar[i].popIcons[j][k].tag = 0;
            editor->toolBar[i].popIcons[j][k].r0 = 230; // PCwipeout
            editor->toolBar[i].popIcons[j][k].g0 = 0;
            editor->toolBar[i].popIcons[j][k].b0 = 0;
            editor->toolBar[i].popIcons[j][k].code = 0;
            editor->toolBar[i].popIcons[j][k].x0 = TopLeftX + 0 - (k+1)*SizeXY; 
            editor->toolBar[i].popIcons[j][k].y0 = TopLeftY + i*SizeXY;
            editor->toolBar[i].popIcons[j][k].u0 = texture->u0;
            editor->toolBar[i].popIcons[j][k].v0 = texture->v0;
            editor->toolBar[i].popIcons[j][k].clut = texture->cba;

            editor->toolBar[i].popIcons[j][k].r1 = 255;
            editor->toolBar[i].popIcons[j][k].g1 = 255;
            editor->toolBar[i].popIcons[j][k].b1 = 255;
            editor->toolBar[i].popIcons[j][k].p1 = 0;
            editor->toolBar[i].popIcons[j][k].x1 = TopLeftX + SizeXY - (k+1)*SizeXY;
            editor->toolBar[i].popIcons[j][k].y1 = TopLeftY + i*SizeXY;
            editor->toolBar[i].popIcons[j][k].u1 = texture->u1;
            editor->toolBar[i].popIcons[j][k].v1 = texture->v1;
            editor->toolBar[i].popIcons[j][k].tpage = texture->tsb;

            editor->toolBar[i].popIcons[j][k].r2 = 255;
            editor->toolBar[i].popIcons[j][k].g2 = 255;
            editor->toolBar[i].popIcons[j][k].b2 = 255;
            editor->toolBar[i].popIcons[j][k].p2= 0;
            editor->toolBar[i].popIcons[j][k].x2 = TopLeftX + 0 - (k+1)*SizeXY;
            editor->toolBar[i].popIcons[j][k].y2 = TopLeftY + i*SizeXY + SizeXY;
            editor->toolBar[i].popIcons[j][k].u2 = texture->u2;
            editor->toolBar[i].popIcons[j][k].v2 = texture->v2;
            editor->toolBar[i].popIcons[j][k].pad2 = 0;

            editor->toolBar[i].popIcons[j][k].r3 = 255;
            editor->toolBar[i].popIcons[j][k].g3 = 255;
            editor->toolBar[i].popIcons[j][k].b3 = 255;
            editor->toolBar[i].popIcons[j][k].p3= 0;
            editor->toolBar[i].popIcons[j][k].x3 = TopLeftX + SizeXY - (k+1)*SizeXY;
            editor->toolBar[i].popIcons[j][k].y3 = TopLeftY + i*SizeXY + SizeXY;
            editor->toolBar[i].popIcons[j][k].u3 = texture->u3;
            editor->toolBar[i].popIcons[j][k].v3 = texture->v3;
            editor->toolBar[i].popIcons[j][k].pad3 = 0;

            SetPolyGT4( &editor->toolBar[i].popIcons[j][k] );
         }
      }
   }


   /* the popped out highlighters */
   for ( i=0; i<numTools; i++ )
   {
      for ( j=0; j<2; j++ )
      {
         editor->toolBar[i].poppedHighlight[j].tag = 0;
         editor->toolBar[i].poppedHighlight[j].r0 = 230; // PCwipeout
         editor->toolBar[i].poppedHighlight[j].g0 = 0;
         editor->toolBar[i].poppedHighlight[j].b0 = 0;
         editor->toolBar[i].poppedHighlight[j].code = 0;
         editor->toolBar[i].poppedHighlight[j].x0 = TopLeftX - SizeXY; 
         editor->toolBar[i].poppedHighlight[j].y0 = TopLeftY + i*SizeXY;

         editor->toolBar[i].poppedHighlight[j].r1 = 255;
         editor->toolBar[i].poppedHighlight[j].g1 = 0;
         editor->toolBar[i].poppedHighlight[j].b1 = 0;
         editor->toolBar[i].poppedHighlight[j].pad1 = 0;
         editor->toolBar[i].poppedHighlight[j].x1 = TopLeftX + SizeXY - SizeXY;
         editor->toolBar[i].poppedHighlight[j].y1 = TopLeftY + i*SizeXY;

         editor->toolBar[i].poppedHighlight[j].r2 = 255;
         editor->toolBar[i].poppedHighlight[j].g2 = 0;
         editor->toolBar[i].poppedHighlight[j].b2 = 0;
         editor->toolBar[i].poppedHighlight[j].pad2 = 0;
         editor->toolBar[i].poppedHighlight[j].x2 = TopLeftX  - SizeXY;
         editor->toolBar[i].poppedHighlight[j].y2 = TopLeftY + i*SizeXY + SizeXY;

         editor->toolBar[i].poppedHighlight[j].r3 = 255;
         editor->toolBar[i].poppedHighlight[j].g3 = 0;
         editor->toolBar[i].poppedHighlight[j].b3 = 0;
         editor->toolBar[i].poppedHighlight[j].pad3 = 0;
         editor->toolBar[i].poppedHighlight[j].x3 = TopLeftX + SizeXY - SizeXY;
         editor->toolBar[i].poppedHighlight[j].y3 = TopLeftY + i*SizeXY + SizeXY;

         SetPolyG4( &editor->toolBar[i].poppedHighlight[j] );
	      SetSemiTrans( ( POLY_G4* ) &editor->toolBar[i].poppedHighlight[j], 1);

      }
   }



   /* the one highlight icon */

   for ( i=0; i<2; i++ )
   {
      editor->highlight[i].tag = 0;
      editor->highlight[i].r0 = 230; // PCwipeout
      editor->highlight[i].g0 = 0;
      editor->highlight[i].b0 = 0;
      editor->highlight[i].code = 0;
      editor->highlight[i].x0 = TopLeftX;
      editor->highlight[i].y0 = TopLeftY;

      editor->highlight[i].r1 = 255;
      editor->highlight[i].g1 = 0;
      editor->highlight[i].b1 = 0;
      editor->highlight[i].pad1 = 0;
      editor->highlight[i].x1 = TopLeftX + SizeXY;
      editor->highlight[i].y1 = TopLeftY;

      editor->highlight[i].r2 = 255;
      editor->highlight[i].g2 = 0;
      editor->highlight[i].b2 = 0;
      editor->highlight[i].pad2 = 0;
      editor->highlight[i].x2 = TopLeftX;
      editor->highlight[i].y2 = TopLeftY + SizeXY;

      editor->highlight[i].r3 = 255;
      editor->highlight[i].g3 = 0;
      editor->highlight[i].b3 = 0;
      editor->highlight[i].pad3 = 0;
      editor->highlight[i].x3 = TopLeftX + SizeXY;
      editor->highlight[i].y3 = TopLeftY + SizeXY;

      SetPolyG4( &editor->highlight[i] );
	   SetSemiTrans( ( POLY_G4* ) &editor->highlight[i], 1);
   }
}

#endif //USE_ATI
#ifndef USE_ATI



void EditorUpdate( Editor *editor, char level )
{         
	static ushort  pad;
   ushort         opad, xpad;

   short          i, k;
   Texture        *texture;
   short          temp;

   char           trackPath[ 255 ];
   char           No[ 2 ];


	opad=pad;
   pad = neg1.bits;
	pad = ~pad;
	xpad=(pad^opad)&pad;

   /* editor on or off */
   if ( ( xpad & PadSelect ) )
   {
      editor->status ^= 1;
   }

   if ( !editor->status )
   {
      return;
   }


/* the popped out highlighter */
   for ( i=0; i<numTools; i++ )
   {
      if ( ( editor->toolBar[i].poppedOut ) && ( editor->toolBar[i].type == -1 ) )
      {
         AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &editor->toolBar[i].poppedHighlight[CurrentScreen] );
      }
   } 

/* the highlighter ( if not popped out ) */
   if ( !( editor->toolBar[ editor->downPos ].poppedOut ) )
   {
      AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &editor->highlight[CurrentScreen] );
   }

/* the blue toggle */
   for (i=0; i<numTools; i++ )
   {
      if ( ( editor->toolBar[i].onOff ) && ( editor->toolBar[i].type != -1 ) )
      {
         AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &editor->selected[CurrentScreen][i] );
      }
   }

/* the popout icons */
   if ( ( editor->toolBar[editor->downPos].poppedOut ) && ( editor->toolBar[editor->downPos].type == -1 ) )
   {
      for ( k=0; k<editor->toolBar[editor->downPos].numPopouts; k++ )
      {
         texture = TextureTable[ editor->iconTextures + editor->toolBar[editor->downPos].popOut[k] ];

         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].u0 = texture->u0;
         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].v0 = texture->v0;
         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].clut = texture->cba;

         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].u1 = texture->u1;
         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].v1 = texture->v1;
         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].tpage = texture->tsb;
            
         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].u2 = texture->u2;
         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].v2 = texture->v2;

         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].u3 = texture->u3;
         editor->toolBar[editor->downPos].popIcons[CurrentScreen][k].v3 = texture->v3;
      
         AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &editor->toolBar[editor->downPos].popIcons[CurrentScreen][k] );
      }
   }

/* the icons */
   for (i=0; i<numTools; i++ )
   {
      if ( editor->toolBar[i].type != -1 ) 
      {
         AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &editor->icons[CurrentScreen][i] );
      }
      else
      {
         texture = TextureTable[ editor->iconTextures + editor->toolBar[ i ].currentPopout ];

         editor->icons[CurrentScreen][i].u0 = texture->u0;
         editor->icons[CurrentScreen][i].v0 = texture->v0;
         editor->icons[CurrentScreen][i].clut = texture->cba;
      
         editor->icons[CurrentScreen][i].u1 = texture->u1;
         editor->icons[CurrentScreen][i].v1 = texture->v1;
         editor->icons[CurrentScreen][i].tpage = texture->tsb;
         
         editor->icons[CurrentScreen][i].u2 = texture->u2;
         editor->icons[CurrentScreen][i].v2 = texture->v2;

         editor->icons[CurrentScreen][i].u3 = texture->u3;
         editor->icons[CurrentScreen][i].v3 = texture->v3;

         AddPrim( OT[ CurrentScreen ] + 0, ( ulong* ) &editor->icons[CurrentScreen][i] );
      }
   }




/* moved highlighter up */
   if ( xpad & PadUp0 )
   {
      editor->downPos -= 1;
      if ( editor->downPos < 0 )
      {
         editor->downPos = 0;
      }

   /* turn all popouts off */
      for (i=0; i<numTools; i++ )
      {
         editor->toolBar[i].poppedOut = 0;   /* not popped out anymore */
         editor->toolBar[i].leftPos = 0;     /* reset to 0 */
      }
   }


/* moved highlighter down */
   if ( xpad & PadDown0 )
   {
      editor->downPos += 1;
      if ( editor->downPos > numTools-1 )
      {
         editor->downPos = numTools-1;
      }

   /* turn all popouts off */
      for (i=0; i<numTools; i++ )
      {
         editor->toolBar[i].poppedOut = 0;   /* not popped out anymore */
         editor->toolBar[i].leftPos = 0;     /* reset to 0 */
      }
   }


/* moved popout highlighter left */
   if ( ( xpad & PadLeft0 ) && ( editor->toolBar[editor->downPos].poppedOut ) && ( editor->toolBar[editor->downPos].type == -1 ) )
   {
      editor->toolBar[editor->downPos].leftPos += 1;
      if ( editor->toolBar[editor->downPos].leftPos > editor->toolBar[editor->downPos].numPopouts )
      {
         editor->toolBar[editor->downPos].leftPos = editor->toolBar[editor->downPos].numPopouts ;
      }
   }


/* moved popout highlighter right */
   if ( ( xpad & PadRight0 ) && ( editor->toolBar[editor->downPos].poppedOut ) && ( editor->toolBar[editor->downPos].type == -1 ) )
   {
      editor->toolBar[editor->downPos].leftPos -= 1;
      if ( editor->toolBar[editor->downPos].leftPos < 0 )
      {
         editor->toolBar[editor->downPos].leftPos = 0 ;
      }
   }



/* made a selection */
   if ( xpad & PadCross0 )
   {
      if ( editor->toolBar[editor->downPos].type == -1 )
      {
      /* we have a popout */
         editor->toolBar[editor->downPos].poppedOut ^= 1;

         /* if selection has been been turned off then reset it and swap icons etc */
         if ( !editor->toolBar[editor->downPos].poppedOut )
         {
            if ( editor->toolBar[editor->downPos].leftPos > 0 )   /* if not the right hand one */
            {
               /* do the stuff here */
               temp = editor->toolBar[editor->downPos].popOut[ editor->toolBar[editor->downPos].leftPos - 1 ];

               editor->toolBar[editor->downPos].popOut[ editor->toolBar[editor->downPos].leftPos - 1 ] =
                              editor->toolBar[editor->downPos].currentPopout;

               editor->toolBar[editor->downPos].currentPopout = temp;
            }

            for (i=0; i<numTools; i++ )
            {
               editor->toolBar[i].poppedOut = 0;   /* not popped out anymore */
               editor->toolBar[i].leftPos = 0;     /* reset to 0 */
            }
         }

         /* set the camera update */
         if ( editor->toolBar[ 0 ].currentPopout == fDynamics )
         {
				CameraUpdate = UpdateCameraRaceExternal;	 
         }
         else if ( editor->toolBar[ 0 ].currentPopout == fRadius )
         {
				CameraUpdate = UpdateCameraEditorStep;	 
         }
         else if ( editor->toolBar[ 0 ].currentPopout == fTexSelect )
         {
				CameraUpdate = UpdateCameraEditorStep;	 
         }
         else if ( editor->toolBar[ 0 ].currentPopout == fTrakView )
         {
				CameraUpdate = UpdateCameraEditorStep;	 
         }
         else if ( editor->toolBar[ 0 ].currentPopout == fDpa )
         {
				CameraUpdate = UpdateCameraEditorStep;	 
         }

      }
      else
      {
      /* we have a toggle */
         editor->toolBar[editor->downPos].onOff ^= 1;

         if ( editor->toolBar[editor->downPos].type == fTrans )
         {
            if ( editor->toolBar[editor->downPos].onOff )
            {
               SetDefaultTrackTexture( );
            }
            else
            {
               SetDefaultTrackTexture( );
            }
         }

         /* pressed the save button */
         if ( editor->toolBar[editor->downPos].type == fSave )
         {
         	strcpy( trackPath, "wipeout\\track" );
//	         sprintf( No, "%02d", level );	
	         strcat( trackPath, No);
	         strcat( trackPath, "\\track.trs" );
          
            PointersToIndex( );
            IntelSection( );  
            SaveFile( trackPath, (char*)track->sections, track->sectionCount * sizeof( TrackSection ) );
            IntelSection( );  
            IndexToPointers( );

         	strcpy( trackPath, "wipeout\\track" );
//	         sprintf( No, "%02d", level );	
	         strcat( trackPath, No);
	         strcat( trackPath, "\\track.trf" );



            for ( i=0; i<track->faceCount; i++ )
            {
               track->faces[ i ].red = track->colCopy[ i ].red;
               track->faces[ i ].green = track->colCopy[ i ].green;
               track->faces[ i ].blue = track->colCopy[ i ].blue;
            }

            IntelFace( );
            SaveFile( trackPath, (char*)track->faces, track->faceCount * sizeof( Face ) );
            IntelFace( );

            for ( i=0; i<track->faceCount; i++ )
            {
               if ( track->faces[ i ].flags & SPEED_UP )
               {
                  track->faces[ i ].red = 0;
                  track->faces[ i ].green = 0;
                  track->faces[ i ].blue = 255;
               }
               else if ( track->faces[ i ].flags & ( WEAPON_GRID_LEFT | WEAPON_GRID_RIGHT ) )
               {
                  track->faces[ i ].red = 255;
                  track->faces[ i ].green = 0;
                  track->faces[ i ].blue = 0;
               }
               else
               {
                  track->faces[ i ].red = track->colCopy[ i ].red;
                  track->faces[ i ].green = track->colCopy[ i ].green;
                  track->faces[ i ].blue = track->colCopy[ i ].blue;
               }
            }


            editor->toolBar[editor->downPos].onOff = 0;

            editor->justSaved = 120;
         }

         /* pressed the dump vram button */
         if ( editor->toolBar[editor->downPos].type == fDumpVram )
         {
            DumpVram( 1 );

            editor->toolBar[editor->downPos].onOff = 0;

            editor->justDumped = 60;
         }

      }
   }


/* calc new position for up - down selector */
   for ( i=0; i<2; i++ )
   {
      editor->highlight[i].y0 = TopLeftY + editor->downPos*SizeXY;
      editor->highlight[i].y1 = TopLeftY + editor->downPos*SizeXY;
      editor->highlight[i].y2 = TopLeftY + editor->downPos*SizeXY + SizeXY;
      editor->highlight[i].y3 = TopLeftY + editor->downPos*SizeXY + SizeXY;
   }     


/* calc new position for left - right selector */
   for ( i=0; i<2; i++ )
   {
      editor->toolBar[ editor->downPos ].poppedHighlight[i].x0 = TopLeftX - SizeXY * editor->toolBar[editor->downPos].leftPos; 
      editor->toolBar[ editor->downPos ].poppedHighlight[i].x1 = TopLeftX - SizeXY * editor->toolBar[editor->downPos].leftPos + SizeXY; 
      editor->toolBar[ editor->downPos ].poppedHighlight[i].x2 = TopLeftX - SizeXY * editor->toolBar[editor->downPos].leftPos; 
      editor->toolBar[ editor->downPos ].poppedHighlight[i].x3 = TopLeftX - SizeXY * editor->toolBar[editor->downPos].leftPos + SizeXY; 
   }

}


#endif //USE_ATI
#ifndef USE_ATI



short EditorFlyMode( Editor *editor )
{

   if ( ( editor->toolBar[ 0 ].currentPopout == fCamera    ) ||
        ( editor->toolBar[ 0 ].currentPopout == fTexSelect ) ||
        ( editor->toolBar[ 0 ].currentPopout == fDpa       ) ||
        ( editor->toolBar[ 0 ].currentPopout == fRadius    ) ||
        ( editor->toolBar[ 0 ].currentPopout == fTrakView  ) 
      )
   {
      return 1;
   }
   else
   {
      return 0;
   }
}

#endif //USE_ATI




