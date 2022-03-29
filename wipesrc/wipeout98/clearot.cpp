#include "wipesrc/psx26/include/sys/types.h"
#include "wipesrc/psx26/include/libgte.h"
#include "wipesrc/psx26/include/libgpu.h"


P_TAG *ClearOTagR(P_TAG *ot, int n)
{
	P_TAG * i, * j;

	// clear object table in reverse direction
	for( i=(ot+n-1), j=(ot+n-2); 	j>=ot; 	i--, j-- )
	{
		((P_TAG *)i)->code = 0;
		i->addr = ((u_long) j) & 0xffffffff;
	}

	ot->addr = 0xFFFFFFFF;
	return(i);
}
