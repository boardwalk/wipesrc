/* (C) Psygnosis 1994. By Jason Carl Denton
*/


#include "wipesrc/psx26/include/sys/types.h"
#include "wipesrc/psx26/include/libgte.h"
#include "wipesrc/psx26/include/libgpu.h"
#include "wipesrc/psx26/include/libsn.h"
#include "wipesrc/psx26/include/libetc.h"
#include "wipesrc/psx26/include/libcd.h"
#include "wipesrc/psx26/include/libspu.h"

typedef char* Ptr;

#define PAL       0

#ifndef NULL
#define  NULL     0
#endif

extern 	uintptr_t	FastRam;	// ajs (well ajy actually)

/* Functions in the library but not defined in the headers! */


/*
   rsin() and rcos()

   angle  : 4096 == 360 degrees
   result : 4096 == 1


int32_t rsin( int32_t angle );
int32_t rcos( int32_t angle );
*/
