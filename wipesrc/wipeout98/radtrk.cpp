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
#include "libgte.h"
#include "error.h"
#include "utils.h"

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
}
