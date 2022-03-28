
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
#include "dpa.h"


#ifndef USE_ATI


void DpaControl( TrackCamera *camera, short *face, Object *texHighlight, TrackSection *nextSection )
{
   SVECTOR           highlightVertices[4];
   short             i;



/* remember these are back to front */
   ((G4*)texHighlight->primitives)->coords[0] = 1;
   ((G4*)texHighlight->primitives)->coords[1] = 0;
   ((G4*)texHighlight->primitives)->coords[2] = 2;
   ((G4*)texHighlight->primitives)->coords[3] = 3;

   ((G4*)texHighlight->primitives)->colour[ 0 ].r = 0;
   ((G4*)texHighlight->primitives)->colour[ 0 ].g = 255;
   ((G4*)texHighlight->primitives)->colour[ 0 ].b = 0;

   ((G4*)texHighlight->primitives)->colour[ 1 ].r = 0;
   ((G4*)texHighlight->primitives)->colour[ 1 ].g = 255;
   ((G4*)texHighlight->primitives)->colour[ 1 ].b = 0;

   ((G4*)texHighlight->primitives)->colour[ 2 ].r = 0;
   ((G4*)texHighlight->primitives)->colour[ 2 ].g = 255;
   ((G4*)texHighlight->primitives)->colour[ 2 ].b = 0;

   ((G4*)texHighlight->primitives)->colour[ 3 ].r = 0;
   ((G4*)texHighlight->primitives)->colour[ 3 ].g = 255;
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