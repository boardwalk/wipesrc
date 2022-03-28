
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



/* maximum number of sections in section view list */
#define MaxGlobalList   255

/* sections must be within this distance */
#define GlobalDistance ( 32767 - 4096 )

/* this is the initial overestimate of the compressed data */
#define MaxCompressed ( 64 * 1024 )


/* this is part of the pre process to make up N, S, E, W lists */
char SectionPosition
( 
	TrackSection      *start, 
	TrackSection      *end
)
{
	VECTOR        toCentre;
	long           angle;
   long           forwardAngle;
   VECTOR        forwards;
   VECTOR        unit;
   TrackSection   *nextSection;
   char           flag = 0;
   long           length;

   nextSection = start->nextSection.ptr;

   forwards.vx = ( nextSection->centre.vx - start->centre.vx );
	forwards.vz = ( nextSection->centre.vz - start->centre.vz );

   length = SquareRoot0( ( forwards.vx * forwards.vx ) + ( forwards.vz * forwards.vz ) );

   unit.vx = forwards.vx * 1536 / length; 
   unit.vz = forwards.vz * 1536 / length; 

   toCentre.vx = ( end->centre.vx - start->centre.vx );
	toCentre.vz = ( end->centre.vz - start->centre.vz );

   forwardAngle = -ratan2(forwards.vx, forwards.vz);
	forwardAngle = ang(forwardAngle);
 


	angle = -ratan2(toCentre.vx + unit.vx, toCentre.vz + unit.vz );
	angle = ang(angle);

   angle = ang(angle - forwardAngle);
  
   if ( start->flags & ( JunctionStart | JunctionEnd ) )
   {
      if( (angle >= -1536) && (angle < 1536) )
	   {
         flag |= North;
	   }
   }
   else
   {
      if( (angle >= -1024) && (angle < 1024) )
	   {
         flag |= North;
	   }
   }

	angle = -ratan2( toCentre.vx - unit.vx, toCentre.vz - unit.vz );
	angle = ang(angle);

   angle = ang(angle - forwardAngle);

   
   if ( start->flags & ( JunctionStart | JunctionEnd ) )
   {
      /* note the || */
      if( (angle >= 512) || (angle < -512) )
	   {
         flag |= South;
	   }
   }
   else
   {
      /* note the || */
      if( (angle >= 1024) || (angle < -1024) )
	   {
         flag |= South;
	   }
   }

	angle = -ratan2( toCentre.vx - unit.vz, toCentre.vz + unit.vx );
	angle = ang(angle);

   angle = ang(angle - forwardAngle);

   if ( start->flags & ( JunctionStart | JunctionEnd ) )
   {
      if( (angle >= -512) || (angle < -1536) )
	   {
         flag |= East;
	   }
   }
   else
   {
      if( (angle >= 0) && (angle < 2048) )
	   {
         flag |= East;
	   }
   }

	angle = -ratan2( toCentre.vx + unit.vz, toCentre.vz - unit.vx );
	angle = ang(angle);

   angle = ang(angle - forwardAngle);

   if ( start->flags & ( JunctionStart | JunctionEnd ) )
   {
      if( (angle >= 1536) || (angle < 512) )
	   {
         flag |= West;
	   }
   }
   else
   {
      if( (angle >= -2047) && (angle < 0) )
	   {
         flag |= West;
	   }
   }

   return flag;
}



short *compress ( short *numbers, short *count, short *all, long *allCount )
{
   short       i, j;
   short       consecCount = 0;
   short       packed[ 255 ];
   short       *packed2;
   short       index = 0;
   short       consecutive;
   short       current;
   short       next;
   short       start;

   static short secNo = 0;
   
#if 0
   printf( "%d\n", secNo++ );
   printf( "before " );
   for ( i=0; i<*count; i++ )
   {
      printf( "%3d ", numbers[ i ] );
   }
   printf( "\n" );
#endif

   for ( i=0; i<*count; i++ )
   {
      current = numbers[ i ];
      start = current;
      consecCount = 1;
      consecutive = 1;

      for ( j=i+1; j<*count && consecutive; j++ )
      {
         next = numbers[ j ];
         if ( ( next - current ) == 1 )
         {
            consecCount += 1;
            current = next;
         }
         else
         {
            consecutive = 0;
         }
      }

      i += consecCount-1;
      packed[ index++ ] = start;
      packed[ index++ ] = consecCount;
   }

#if 0
   printf( "after  " );
   for ( i=0; i<index; i++ )
   {
      printf( "%3d ", packed[ i ] );
   }
   printf( "\n\n" );
#endif

   packed2 = all + *allCount;        /* start of the data */
   for ( i=0; i<index; i++ )
   {
      packed2[ i ] = packed[ i ];
      (*allCount)++;
      if ( *allCount >= MaxCompressed )
      {
         Error ( "radtrk.c::compress:No memory for temporay compressed data", Fatal );
      }
   }

   *count = index;
   return packed2;
}




void sortDistances ( long numbers[ MaxGlobalList ][ 2 ], short count )
{
   short       i, j;
   long        temp;

#if 0
   for ( i=0; i<count-1; i++ )
   {
      if ( numbers[ i ][ 1 ] > numbers[ i+1 ][ 1 ] )
      {
         printf( "out of order ************\n" );
      }
   }

   printf( "before " );
   for ( i=0; i<count; i++ )
   {
      printf( "%3d ", numbers[ i ][ 1 ] );
   }
   printf( "\n" );
#endif


   for ( i=0; i<count; i++ )
   {
      for ( j=i+1; j<count; j++ )
      {
         if ( numbers[ i ][ 1 ] > numbers[ j ][ 1 ] )
         {
            temp = numbers[ j ][ 1 ];
            numbers[ j ][ 1 ] = numbers[ i ][ 1 ];
            numbers[ i ][ 1 ] = temp;

            temp = numbers[ j ][ 0 ];
            numbers[ j ][ 0 ] = numbers[ i ][ 0 ];
            numbers[ i ][ 0 ] = temp;
         }
      }
   }

#if 0
   printf( "after  " );
   for ( i=0; i<count; i++ )
   {                
      printf( "%3d ", numbers[ i ][ 1 ] );
   }
   printf( "\n" );
#endif
}




void sortSecNo ( short numbers[ MaxGlobalList ], short count )
{
   short       i, j;
   short       temp;

#if 0
   for ( i=0; i<count-1; i++ )
   {
      if ( numbers[ i ] > numbers[ i+1 ] )
      {
         printf( "out of order ************\n" );
      }
   }

   printf( "before " );
   for ( i=0; i<count; i++ )
   {
      printf( "%3d ", numbers[ i ] );
   }
   printf( "\n" );
#endif

   for ( i=0; i<count; i++ )
   {
      for ( j=i+1; j<count; j++ )
      {
         if ( numbers[ i ] > numbers[ j ] )
         {
            temp = numbers[ j ];
            numbers[ j ] = numbers[ i ];
            numbers[ i ] = temp;
         }
      }
   }

#if 0
   printf( "after  " );
   for ( i=0; i<count; i++ )
   {                
      printf( "%3d ", numbers[ i ] );
   }
   printf( "\n" );
#endif
}





#ifndef USE_ATI



void TrackRadiusCheckNew( )
{
   TrackSection   *sectionA;
   TrackSection   *sectionB;
	Vector         *sectionAcentre;
	Vector         *sectionBcentre;
	VECTOR       	vector;
   long      distance;		//PCWipeout long (very) long
   short          i, j, k;

   short          globalViewCount;
   long           globalTrackList[ MaxGlobalList ][ 2 ];       /* track no and distance  */
   short          hiList[ MaxGlobalList ];                     /* just the track numbers */
   short          medList[ MaxGlobalList ];            
   short          loList[ MaxGlobalList ];             

   short          dir;
   short          count;

   short          *allCompressed;
   long           allCompressedIndex;

   short          *finalCompressed;
   long           finalCompressedIndex;


//   printf( "\n\npre processing\n\n" ); 


   allCompressedIndex = 0;
   allCompressed = ( short* )DAlloc(heap, MaxCompressed * sizeof(short) );
   if ( !allCompressed )
   {
      Error( "radtrk.c::TrackRadiusCheckNew: No memory for initial compressed data", Fatal );
   }

   sectionA = track->sections;

   for ( i=0; i<track->sectionCount; i++ )
   {
//      printf( "." ); 
      sectionAcentre = &sectionA->centre;


      /* north */

      globalViewCount = 0;
      sectionB = track->sections;
      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < GlobalDistance ) &&
              ( abs( vector.vy ) < GlobalDistance ) &&
              ( abs( vector.vz ) < GlobalDistance ) )
         {
            vector.vx = vector.vx;
            vector.vy = vector.vy;
            vector.vz = vector.vz;

            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

			distance /=2;//SJR
            if ( ( distance < GlobalDistance ) && ( SectionPosition( sectionA, sectionB ) & North ) )
            {
               if ( globalViewCount >= MaxGlobalList )
               {
                  Error( "radius.c::TrackRadiusCheckNew: Global View List Full ( North )", Fatal );
               }

               globalTrackList[ globalViewCount ][ 0 ] = j;
               globalTrackList[ globalViewCount ][ 1 ] = distance;
               globalViewCount += 1;
            }
         }

         sectionB = NextIndexSection( sectionB );
      }

      sortDistances( globalTrackList, globalViewCount );


      if ( ( sectionA->flags & ( JunctionStart | JunctionEnd ) ) || ( sectionA->junction.index != -1 ) )
      {
         count = 0;
         for ( j=0; j<6; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );

         if ( count > 0 )
         {
            sectionA->high[ 0 ] = hiList[ count - 1 ];
         }
         else
         {
            sectionA->high[ 0 ] = -1;
         }

         sectionA->northViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->northViewCount[ 0 ] = count ;//SJR



         count = 0;
         for ( j=6; j<10; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );

         if ( count > 0 )
         {
            sectionA->med[ 0 ] = medList[ count - 1 ];
         }
         else
         {
            sectionA->med[ 0 ] = -1;
         }

         sectionA->northViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->northViewCount[ 1 ] = count ; //SJR



         count = 0;
         for ( j=10; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );
         sectionA->northViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->northViewCount[ 2 ] = count; //SJR
      }
      else
      {
         count = 0;
         for ( j=0; j<4; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );

         if ( count > 0 )
         {
            sectionA->high[ 0 ] = hiList[ count - 1 ];
         }
         else
         {
            sectionA->high[ 0 ] = -1;
         }

         sectionA->northViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->northViewCount[ 0 ] = count;//SJR


         count = 0;
         for ( j=4; j<8; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );

         if ( count > 0 )
         {
            sectionA->med[ 0 ] = medList[ count - 1 ];
         }
         else
         {
            sectionA->med[ 0 ] = -1;
         }

         sectionA->northViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->northViewCount[ 1 ] = count; //SJR


         count = 0;
         for ( j=8; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );
         sectionA->northViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->northViewCount[ 2 ] = count;//SJR
      }


      /* south */

      globalViewCount = 0;
      sectionB = track->sections;
      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < GlobalDistance ) &&
              ( abs( vector.vy ) < GlobalDistance ) &&
              ( abs( vector.vz ) < GlobalDistance ) )
         {
            vector.vx = vector.vx;
            vector.vy = vector.vy;
            vector.vz = vector.vz;

            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            if ( ( distance < GlobalDistance ) && ( SectionPosition( sectionA, sectionB ) & South ) )
            {
               if ( globalViewCount >= MaxGlobalList )
               {
                  Error( "radius.c::TrackRadiusCheckNew: Global View List Full ( South )", Fatal );
               }

               globalTrackList[ globalViewCount ][ 0 ] = j;
               globalTrackList[ globalViewCount ][ 1 ] = distance;
               globalViewCount += 1;
            }

         }
         sectionB = NextIndexSection( sectionB );
      }

      sortDistances( globalTrackList, globalViewCount );


      if ( ( sectionA->flags & ( JunctionStart | JunctionEnd ) ) || ( sectionA->junction.index != -1 ) )
      {
         count = 0;
         for ( j=0; j<6; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );

         sectionA->southViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->southViewCount[ 0 ] = count;//SJR

         count = 0;
         for ( j=6; j<10; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );

         if ( count > 0 )
         {
            sectionA->high[ 1 ] = medList[ count - 1 ];  /* last med */
         }
         else
         {
            sectionA->high[ 1 ] = -1;
         }

         sectionA->southViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->southViewCount[ 1 ] = count;//SJR

         count = 0;
         for ( j=10; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );

         if ( count > 0 )
         {
            sectionA->med[ 1 ] = loList[ count - 1 ];    /* last lo */
         }
         else
         {
            sectionA->med[ 1 ] = -1;
         }

         sectionA->southViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->southViewCount[ 2 ] = count;//SJR
      }
      else
      {
         count = 0;
         for ( j=0; j<4; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );

         sectionA->southViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->southViewCount[ 0 ] = count;//SJR

         count = 0;
         for ( j=4; j<8; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );

         if ( count > 0 )
         {
            sectionA->high[ 1 ] = medList[ count - 1 ];  /* last med */
         }
         else
         {
            sectionA->high[ 1 ] = -1;
         }

         sectionA->southViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->southViewCount[ 1 ] = count;//SJR

         count = 0;
         for ( j=8; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );

         if ( count > 0 )
         {
            sectionA->med[ 1 ] = loList[ count - 1 ];    /* last lo */
         }
         else
         {
            sectionA->med[ 1 ] = -1;
         }

         sectionA->southViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->southViewCount[ 2 ] = count;//SJR
      }


      /* west */

      globalViewCount = 0;
      sectionB = track->sections;
      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < GlobalDistance ) &&
              ( abs( vector.vy ) < GlobalDistance ) &&
              ( abs( vector.vz ) < GlobalDistance ) )
         {
            vector.vx = vector.vx;
            vector.vy = vector.vy;
            vector.vz = vector.vz;

            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            if ( ( distance < GlobalDistance ) && ( SectionPosition( sectionA, sectionB ) & West ) )
            {
               if ( globalViewCount >= MaxGlobalList )
               {
                  Error( "radius.c::TrackRadiusCheckNew: Global View List Full ( West )", Fatal );
               }

               globalTrackList[ globalViewCount ][ 0 ] = j;
               globalTrackList[ globalViewCount ][ 1 ] = distance;
               globalViewCount += 1;
            }

         }
         sectionB = NextIndexSection( sectionB );
      }

      sortDistances( globalTrackList, globalViewCount );

      if ( ( sectionA->flags & ( JunctionStart | JunctionEnd ) ) || ( sectionA->junction.index != -1 ) )
      {
         count = 0;
         for ( j=0; j<6; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );
         sectionA->westViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->westViewCount[ 0 ] = count;//SJR

         count = 0;
         for ( j=6; j<10; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );
         sectionA->westViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->westViewCount[ 1 ] = count;

         count = 0;
         for ( j=10; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );
         sectionA->westViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->westViewCount[ 2 ] = count;//SJR
      }
      else
      {
         count = 0;
         for ( j=0; j<4; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );
         sectionA->westViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->westViewCount[ 0 ] = count;//SJR

         count = 0;
         for ( j=4; j<8; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );
         sectionA->westViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->westViewCount[ 1 ] = count;//SJR

         count = 0;
         for ( j=8; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );
         sectionA->westViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->westViewCount[ 2 ] = count;//SJR
      }



      /* east */

      globalViewCount = 0;
      sectionB = track->sections;
      for ( j=0; j<track->sectionCount; j++ )
      {
         sectionBcentre = &sectionB->centre;

         vector.vx = sectionAcentre->vx - sectionBcentre->vx;
         vector.vy = sectionAcentre->vy - sectionBcentre->vy;
         vector.vz = sectionAcentre->vz - sectionBcentre->vz;

         if ( ( abs( vector.vx ) < GlobalDistance ) &&
              ( abs( vector.vy ) < GlobalDistance ) &&
              ( abs( vector.vz ) < GlobalDistance ) )
         {
            vector.vx = vector.vx;
            vector.vy = vector.vy;
            vector.vz = vector.vz;

            distance = SquareRoot0( ( vector.vx * vector.vx ) +
                                    ( vector.vy * vector.vy ) +
                                    ( vector.vz * vector.vz ) );

            if ( ( distance < GlobalDistance ) && ( SectionPosition( sectionA, sectionB ) & East ) )
            {
               if ( globalViewCount >= MaxGlobalList )
               {
                  Error( "radius.c::TrackRadiusCheckNew: Global View List Full ( East )", Fatal );
               }

               globalTrackList[ globalViewCount ][ 0 ] = j;
               globalTrackList[ globalViewCount ][ 1 ] = distance;
               globalViewCount += 1;
            }

         }
         sectionB = NextIndexSection( sectionB );
      }

      sortDistances( globalTrackList, globalViewCount );


      if ( ( sectionA->flags & ( JunctionStart | JunctionEnd ) ) || ( sectionA->junction.index != -1 ) )
      {
         count = 0;
         for ( j=0; j<6; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );
         sectionA->eastViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->eastViewCount[ 0 ] = count;

         count = 0;
         for ( j=6; j<10; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );
         sectionA->eastViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->eastViewCount[ 1 ] = count;

         count = 0;
         for ( j=10; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );
         sectionA->eastViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->eastViewCount[ 2 ] = count;
      }
      else
      {
         count = 0;
         for ( j=0; j<4; j++ )         
         {
            hiList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( hiList, count );
         sectionA->eastViewSection[ 0 ] = compress( hiList, &count, allCompressed, &allCompressedIndex );
         sectionA->eastViewCount[ 0 ] = count;

         count = 0;
         for ( j=4; j<8; j++ )         
         {
            medList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( medList, count );
         sectionA->eastViewSection[ 1 ] = compress( medList, &count, allCompressed, &allCompressedIndex );
         sectionA->eastViewCount[ 1 ] = count;

         count = 0;
         for ( j=8; j<globalViewCount; j++ )         
         {
            loList[ count ] = globalTrackList[ j ][ 0 ];
            count++;
         }
         sortSecNo( loList, count );
         sectionA->eastViewSection[ 2 ] = compress( loList, &count, allCompressed, &allCompressedIndex );
         sectionA->eastViewCount[ 2 ] = count;
      }





      sectionA = NextIndexSection( sectionA );
   }

   finalCompressedIndex = allCompressedIndex;
   finalCompressed = ( short* )DAlloc(heap, finalCompressedIndex * sizeof(short) );
   if ( !finalCompressed )
   {
      Error( "radtrk.c::TrackRadiusCheckNew: No memory for final compressed data", Fatal );
   }

   for ( i=0; i<finalCompressedIndex; i++ )
   {
      finalCompressed[ i ] = allCompressed[ i ]; 
   }

   sectionA = track->sections;
   for ( i=0; i<track->sectionCount; i++ )
   {
      sectionA->northViewSection[ 0 ] = finalCompressed;
      finalCompressed += sectionA->northViewCount[ 0 ];

      sectionA->northViewSection[ 1 ] = finalCompressed; 
      finalCompressed += sectionA->northViewCount[ 1 ];

      sectionA->northViewSection[ 2 ] = finalCompressed; 
      finalCompressed += sectionA->northViewCount[ 2 ];


      sectionA->southViewSection[ 0 ] = finalCompressed;
      finalCompressed += sectionA->southViewCount[ 0 ];

      sectionA->southViewSection[ 1 ] = finalCompressed; 
      finalCompressed += sectionA->southViewCount[ 1 ];

      sectionA->southViewSection[ 2 ] = finalCompressed; 
      finalCompressed += sectionA->southViewCount[ 2 ];


      sectionA->westViewSection[ 0 ] = finalCompressed;
      finalCompressed += sectionA->westViewCount[ 0 ];

      sectionA->westViewSection[ 1 ] = finalCompressed; 
      finalCompressed += sectionA->westViewCount[ 1 ];

      sectionA->westViewSection[ 2 ] = finalCompressed; 
      finalCompressed += sectionA->westViewCount[ 2 ];


      sectionA->eastViewSection[ 0 ] = finalCompressed;
      finalCompressed += sectionA->eastViewCount[ 0 ];

      sectionA->eastViewSection[ 1 ] = finalCompressed; 
      finalCompressed += sectionA->eastViewCount[ 1 ];

      sectionA->eastViewSection[ 2 ] = finalCompressed; 
      finalCompressed += sectionA->eastViewCount[ 2 ];


      sectionA = NextIndexSection( sectionA );
   }


   DFree ( heap, (char *)allCompressed );

}


#endif //USE_ATI













