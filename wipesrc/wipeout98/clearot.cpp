#include "wipesrc/psx26/include/sys/types.h"
#include "wipesrc/psx26/include/libgte.h"
#include "wipesrc/psx26/include/libgpu.h"
#include <stddef.h>


P_TAG *ClearOTagR(P_TAG *ot, int32_t n)
{
	P_TAG * i, * j;

	// clear object table in reverse direction
	for( i=(ot+n-1), j=(ot+n-2); 	j>=ot; 	i--, j-- )
	{
		((P_TAG *)i)->code = 0;
		i->addr = j;
	}

	ot->addr = NULL;
	return(i);
}
