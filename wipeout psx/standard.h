/* (C) Psygnosis 1994. By Jason Carl Denton
*/


#include <sys\types.h>
#include <libgte.h>
#include <libgpu.h>
#include <libsn.h>
#include <libetc.h>	 
#include <libcd.h>	 
#include <libspu.h>	

typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;

typedef char* Ptr;

#define PAL       0

#ifndef NULL
#define  NULL     0
#endif

extern 	ulong	FastRam;	// ajs (well ajy actually)

/* Functions in the library but not defined in the headers! */


/*
   rsin() and rcos()

   angle  : 4096 == 360 degrees
   result : 4096 == 1


long rsin( long angle );      
long rcos( long angle );      
*/
