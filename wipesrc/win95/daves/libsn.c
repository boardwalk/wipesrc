#include <stdio.h>
#include "libsn.h"


int	PCinit (void)
{
	return 0;	// return no error
}



int	PCopen (char *name, int flags, int perms)
{
	FILE * f;

	switch( flags )
	{
		case 0:	// read binary
			f = fopen( name, "rb" );
			break;
		case 1:	// write binary
			f = fopen( name, "wb" );
			break;
		case 2:	// read/write binary
			f = fopen( name, "wb+" );
			break;
	}

	if( f == NULL )
		return -1;
	else
		return (int)f;
}



int	PClseek (int fd, int offset, int mode)
{
	fseek( (FILE *)fd, offset, mode );
	return ftell( (FILE *)fd );
}



int	PCread (int fd, char *buff, int len)
{
	return fread( buff, 1, len, (FILE *)fd );
}



int	PCwrite (int fd, char *buff, int len)
{
	return fwrite( buff, 1, len, (FILE *)fd );
}



int	PCclose (int fd)
{
	fclose( (FILE*)fd );
	return 0;
}



