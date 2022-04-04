/* (C) Psygnosis 1994. By Jason Carl Denton
*/
#ifndef STANDARD_H
#define STANDARD_H

#include "libgte.h"
#include "libgpu.h"
#include "libsn.h"
#include "libetc.h"
#include "libcd.h"
#include "libspu.h"

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

#endif
