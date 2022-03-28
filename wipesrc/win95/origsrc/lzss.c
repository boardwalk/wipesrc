/******************************************
*
* General Non-Lossy File Decompression.
*
* ( Optimized for speed )
*
*
*	
******************************************/
/*
#include "selibs\standard.i"
*/
/*
** Defines from Compression Routine
*/

#define INDEX_BIT_COUNT		13
#define LENGTH_BIT_COUNT 	4
#define WINDOW_SIZE			( 1 << INDEX_BIT_COUNT )
#define BREAK_EVEN			( ( 1 + INDEX_BIT_COUNT + LENGTH_BIT_COUNT ) /9 )
#define END_OF_STREAM		0
#define MOD_WINDOW( a )		( ( a ) & ( WINDOW_SIZE - 1 ) )


/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
/***************************************************************************/
void
ExpandData( unsigned char	*inData, unsigned char	*outData )
{
	short i;
	short current_position;
	unsigned char cc;
	short match_length;
	short match_position;
	unsigned long mask;
	unsigned long return_value;
	unsigned char inBfile_mask;
	short inBfile_rack;
	short	value;
	unsigned char window[ WINDOW_SIZE ];

	inBfile_rack = 0;
	inBfile_mask = 0x80;

	current_position = 1;
	for ( ; ; )
	{

		if ( inBfile_mask == 0x80 )
	 		inBfile_rack =  (short)*inData++;

		value = inBfile_rack & inBfile_mask;
		inBfile_mask >>= 1;
		if ( inBfile_mask == 0 )
			inBfile_mask = 0x80;

		if ( value )
		{
			mask = 1L << ( 8 - 1 );
			return_value = 0;
			while ( mask != 0)
			{
				if ( inBfile_mask == 0x80 )
		 			inBfile_rack =  (short)*inData++;
		
				if ( inBfile_rack & inBfile_mask )
					return_value |= mask;
				mask >>= 1;
				inBfile_mask >>= 1;

				if ( inBfile_mask == 0 )
					inBfile_mask = 0x80;
			}
			cc = (unsigned char) return_value;
			*outData++ = cc;
			window[ current_position ] = cc;
			current_position = MOD_WINDOW( current_position + 1 );
		}
		else
		{
			mask = 1L << ( INDEX_BIT_COUNT - 1 );
			return_value = 0;
			while ( mask != 0)
			{
				if ( inBfile_mask == 0x80 )
		 			inBfile_rack =  (short)*inData++;
		
				if ( inBfile_rack & inBfile_mask )
					return_value |= mask;
				mask >>= 1;
				inBfile_mask >>= 1;

				if ( inBfile_mask == 0 )
					inBfile_mask = 0x80;
			}
			match_position = (short) return_value;

			if ( match_position == END_OF_STREAM )
					break;

			mask = 1L << ( LENGTH_BIT_COUNT - 1 );
			return_value = 0;
			while ( mask != 0)
			{
				if ( inBfile_mask == 0x80 )
		 			inBfile_rack =  (short)*inData++;
		
				if ( inBfile_rack & inBfile_mask )
					return_value |= mask;
				mask >>= 1;
				inBfile_mask >>= 1;

				if ( inBfile_mask == 0 )
					inBfile_mask = 0x80;
			}
			match_length = (short) return_value;

			match_length += BREAK_EVEN;

			for ( i = 0 ; i <= match_length ; i++ )
			{
				cc = window[ MOD_WINDOW( match_position + i ) ];
				*outData++ = cc;
				window[ current_position ] = cc;
				current_position = MOD_WINDOW( current_position + 1 );
			}
		}
	}
}




/***************************************************************************/


