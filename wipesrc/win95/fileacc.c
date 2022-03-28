#include "d:\watcom\h\stdio.h"

typedef struct {
	FILE	*dosfp;
	unsigned long	size;
	char	name[/*12*/16];
} AYCdlFILE;

#ifndef USE_ATI



AYCdlFILE *AYCdSearchFile(AYCdlFILE *a, char *b)
{
	a->dosfp = fopen(b,"rb");

	if (a->dosfp != NULL)
	{
		fseek(a->dosfp,0,SEEK_END);
		a->size = ftell(a->dosfp);
		fseek(a->dosfp,0,SEEK_SET);
		strcpy (a->name, b);
	}
	else
		return(NULL);
		

	return(a);
}

#endif //USE_ATI
#ifndef USE_ATI


void AYGetFile(void *buf, int a, int b, FILE *c)
{
	fread(buf,a,b,c);
	fclose(c);
}

#endif //USE_ATI
