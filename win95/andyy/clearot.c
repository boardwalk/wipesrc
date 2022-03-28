#include <sys\types.h>




#include <libgte.h>
#include <libgpu.h>

#ifndef USE_ATI



P_TAG *ClearOTag(P_TAG *ot, int n)
{
	// clear object table in forwards direction
	P_TAG *i, *j;
	int count;

	for (i=(ot), j=(ot+1), count = 1; count < n; i++, j++, count++)
	  {
		i->addr =  ((u_long) j) & 0xffffffff;
		((P_TAG *)i)->code = 0;
	  }
	  i->addr = 0xffffffff;
	return(i); // PCwipeout
}

#endif //USE_ATI


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

