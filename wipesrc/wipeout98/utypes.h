/****************************************************************************
 *
 * $Log:   P:\ridger\ridger\chrise\utypes.h_v  $
 * 
 *    Rev 1.2   07 Sep 1995 17:04:24   DaveS
 * Sound stuff works
 * 
 *    Rev 1.0   15 Aug 1995 16:53:26   ChrisE
 * Initial revision.
 * 
 *    Rev 1.7   07 Jul 1994 09:49:20   NeilD
 * No change.
 * 
 *    Rev 1.6   03 Jun 1994 09:14:16   MarkL
 * Added #defines for the limits of the int16, uint16, int32 and uint32 types.
 * 
 *    Rev 1.5   28 Apr 1994 11:50:06   NeilC
 * added Watcom tip for overcoming signed char error
 * 
 *    Rev 1.4   28 Apr 1994 09:28:54   MarkL
 * Moved int64 and uint64 types to math64.h.
 * 
 *    Rev 1.3   27 Apr 1994 12:03:02   NeilD
 * Transfered all frac references and definitions to frac.h
 * 
 *    Rev 1.2   18 Apr 1994 13:45:56   NeilD
 * Added frac and longFrac types.
 * also added MAX_FRAC_VAL and
 *            MAX_LONGFRAC_VAL definitions.
 * 
 *    Rev 1.1   08 Mar 1994 17:19:22   MarkL
 * Added int64 and uint64 types.
 * 
 *    Rev 1.0   07 Mar 1994 11:34:14   MarkL
 * Initial revision.
 * 
 ****************************************************************************/

#ifndef _UTYPES_H_INCLUDED
#define _UTYPES_H_INCLUDED

/*
 * Include Files
 */
#include <limits.h>


/*
 * #defines and macros.
 */

#define INT16_MIN   (-32767-1)
#define INT16_MAX   32767
#define UINT16_MAX  65535U

#define INT32_MIN   (-2147483647-1)
#define INT32_MAX   2147483647
#define UINT32_MAX  4294967295U


/*
 * User type definitions.
 */

/* Char type must be signed */
	typedef signed char int8;
	typedef unsigned char uint8;

typedef unsigned char byte;
typedef unsigned int word;


/* Int16 and uint16 type */
#if USHRT_MAX == 0xffff
	typedef short int int16;
	typedef unsigned short int uint16;
#elif UINT_MAX == 0xffff
	typedef short int int16;
	typedef unsigned short int uint16;
#else
	#error Cannot define types: int16, uint16
#endif

/* Int32 and uint32 type */
#if UINT_MAX == 0xffffffff
	typedef int int32;
	typedef unsigned int uint32;
#elif ULONG_MAX == 0xffffffff
	typedef long int int32;
	typedef unsigned long int uint32;
#else
	#error Cannot define types: int32, uint32
#endif


/*
 * Structure definitions.
 */


/*
 * Function prototypes.
 */


/*
 * External variables.
 */




#endif
