#include "wipesrc/psx26/include/sys/types.h"
#include <stdio.h>
#include <stdlib.h>

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
