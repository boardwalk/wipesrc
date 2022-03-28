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
#include "trakview.h"



#ifndef USE_ATI


void TrakViewControl( TrackCamera *camera, Object **prm )
{
   static ushort  pad;
   ushort         opad, xpad;


	opad=pad;
   pad = neg1.bits;
	pad = ~pad;
	xpad=(pad^opad)&pad;

#if 0
   if ( xpad & PadTriangle0 )
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
   else if ( pad & PadTopRight2 )      
   {
      camera->section->lookForward += 1;
      if ( camera->section->lookForward > MaxLo )
      {
         camera->section->lookForward = MaxLo;
      }
   }
   else if ( pad & PadTopRight1 )    
   {
      camera->section->lookForward -= 1;
      if ( camera->section->lookForward < 1 )
      {
         camera->section->lookForward = 1;
      }
   }
   else if ( pad & PadTopLeft2 )      
   {
      camera->section->lookBehind += 1;
      if ( camera->section->lookBehind > MaxLo )
      {
         camera->section->lookBehind = MaxLo;
      }
   }
   else if ( pad & PadTopLeft1 )    
   {
      camera->section->lookBehind -= 1;
      if ( camera->section->lookBehind < 1 )
      {
         camera->section->lookBehind = 1;
      }
   }
 
   
#endif

   if ( pad & PadLeft0 )
   {
      camera->hdg += 64;
   }

   if ( pad & PadRight0 )
   {
      camera->hdg -= 64;
   }

   if ( pad & PadDown0 )
   {
      camera->pitch += 64;
   }

   if ( pad & PadUp0 )
   {
      camera->pitch -= 64;
   }
}


#endif //USE_ATI
