/* (C) Psygnosis 1994. By Jason Carl Denton
*/

#include "standard.h"
#include <string.h>

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "track.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "iso9660.h"
#include "sound.h"
#include "sparks2.h"
#include "global.h"
#include "error.h"

#define _SIZE_T_DEFINED_			// PCwipeout
#include <stdio.h>		// PCwipeout

#if	CD_FS

#include <sys/file.h>
#include <kernel.h>
#include "iso9660.h"

void InitCDFS(DynamicHeap *heap)
{
	CdInit();
	iso_init(heap);
}

int32_t FileLength( char* name )
{
	FILE *file;
	int32_t length;

	char *cleanname;

	cleanname = (char *)strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}

	file = fopen(cleanname,"rb");
	fseek(file,0L,SEEK_END);
	length = ftell(file);
	fseek(file,0L,SEEK_SET);
	fclose(file);
	return(length);
}

int32_t LoadFile( char* name, char* buffer )
{
   int32_t   fd;
   int32_t  length;
   int32_t  to_read,i,left_over;
	uint32_t tmp_buf[2048/4];				/* Used for last sector of read */
	uint32_t *b, *t;

	char *cleanname;

	cleanname = (char *)strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}


/* Open file */

   fd = iso_open( cleanname, 0 );
   if ( fd == -1 )
   {
      printf( "File.c:LoadFile(): Bad file name %s\n", name );
      return( 0 );
   }

	/* length is the number of bytes to the nearest SECTOR size */

	length = iso_filesize(fd);
   to_read = length >> 11;		/* In sectors */

/* Read file into buffer */

   if ( iso_read( buffer, to_read ,fd) != to_read )
   {
      printf( "File.c:LoadFile(): Failed to load file %s\n", name );
      iso_close( fd );
      return( 0 );
   }
	/* So, we've read the whole sectors of the file. Now read the left overs at the end */
	if (iso_read(tmp_buf, 1, fd) != 1)
	{
      printf( "File.c:LoadFile(): Failed to load file %s\n", name );
      iso_close( fd );
      return( 0 );
   }
	left_over = (length - to_read*2048 + 3) >> 2 ;  /* Number of int32_t words left over */
	b = (uint32_t *)(buffer+to_read*2048);	/* copy the left overs here */
	t = tmp_buf;
	for (i = 0; i < left_over;i++)
	{
		*b++ = *t++;
	}
/* Close file */

   iso_close( fd );

/* Return file length */

   return( length );
}
void SaveFile( char* name, char* buffer, int32_t length )
{
	printf("Save file called: %s , but not implemented 'cause we are CD FS\n", name);
}

#else	/* PC FS */
#ifdef SN_PCFS	// PCwipeout

int32_t FileLength( char* name )
{
   int32_t   fd;
   int32_t  length;

/* Open file */

	char *cleanname;

	cleanname = (char *)strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}



   fd = PCopen( cleanname, 0, 0 );
   if ( fd == -1 )
   {
      return( NULL );
   }

/* Seek to end of file to get length */

   length = PClseek( fd, 0, 2 );

/* Close file */

   PCclose( fd );

/* Return file length */

   return( length );
}

int32_t LoadFile( char* name, char* buffer )
{
   int32_t   fd;
   int32_t  length;

/* Open file */
	char *cleanname;

	cleanname =(char *) strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}


   fd = PCopen( cleanname, 0, 0 );
   if ( fd == -1 )
   {
      printf( "File.c:LoadFile(): Bad file name %s\n", name );
      return( 0 );
   }

/* Seek to end of file to get length */

   length = PClseek( fd, 0, 2 );

/* Seek back to the start */

   PClseek( fd, 0, 0 );

/* Read file into buffer */

   if ( PCread( fd, buffer, length ) != length )
   {
      printf( "File.c:LoadFile(): Failed to load file %s\n", name );
      PCclose( fd );
      return( 0 );
   }

/* Close file */

   PCclose( fd );

/* Return file length */

   return( length );
}


void SaveFile( char* name, char* buffer, int32_t length )
{
   int32_t     fd;

/* Open file */
	char *cleanname;

	cleanname =(char *) strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}


   fd = PCopen( cleanname, 1, 0 );
   if ( fd == -1 )
   {
      sprintf( errorString, "File.c:SaveFile(): Cannot open file %s\n", name );
      Error( errorString, Fatal );
   }

/* Write file */

   if ( PCwrite( fd, buffer, length ) != length )
   {
      sprintf( errorString, "File.c:SaveFile(): Failed to save file %s\n", name );
      Error( errorString, Fatal );
   }

/* Close file */

   PCclose( fd );
}

#else	// PCwipeout - to end of file

int32_t FileLength( char* name )
{
   FILE   *fd;
   int32_t  length;

/* Open file */
	char *cleanname;

	cleanname = (char *)strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}


   fd = fopen( cleanname, "rb" );
   if ( fd == NULL )
   {
      return( NULL );
   }

/* Seek to end of file to get length */

		fseek(fd,0,SEEK_END);
   length = ftell( fd);
	 rewind(fd);

/* Close file */

   fclose( fd );

/* Return file length */

   return( length );
}

int32_t LoadFile( char* name, char* buffer )
{
   FILE *fd;
   int32_t  length;

/* Open file */
	char *cleanname;

	cleanname = (char *)strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}


   fd = fopen( cleanname, "rb" );
   if ( fd == NULL )
   {
      printf( "File.c:LoadFile(): Bad file name %s\n", name );
      return( 0 );
   }

/* Seek to end of file to get length */

		fseek(fd,0,SEEK_END);
   length = ftell( fd);
/* Seek back to the start */
	 rewind(fd);

/* Read file into buffer */

   if ( fread( buffer, 1, length, fd ) != length )
   {
      printf( "File.c:LoadFile(): Failed to load file %s\n", name );
      fclose( fd );
      return( 0 );
   }

/* Close file */

   fclose( fd );

/* Return file length */

   return( length );
}


void SaveFile( char* name, char* buffer, int32_t length )
{
   FILE     *fd;

/* Open file */

	char *cleanname;

	cleanname = (char *)strchr(name,':');
	if (cleanname != NULL)
		{
		cleanname++;
		}
	else
		{
		cleanname = name;
		}


   fd = fopen( cleanname, "wb" );
   if ( fd == NULL )
   {
      sprintf( errorString, "File.c:SaveFile(): Cannot open file %s\n", name );
      Error( errorString, Fatal );
   }

/* Write file */

   if ( fwrite(buffer, 1, length, fd) != length )
   {
      sprintf( errorString, "File.c:SaveFile(): Failed to save file %s\n", name );
      Error( errorString, Fatal );
   }

/* Close file */

   fclose( fd );
}


#endif	// PCwipeout
#endif
