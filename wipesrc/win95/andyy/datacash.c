#define	_SIZE_T
#include <sys\types.h>
#include "w:\h\stdio.h"

u_long *make_data_cache(void)
{
	u_long *cache_addr;

	cache_addr = (u_long*)malloc(1024);

	if (cache_addr == NULL)
		{
		fprintf(stderr,"failed to allocate data cache\n");
		exit(-1);
		}

	return(cache_addr);
	

}
