#include "fileops.h"

TIMlist *LoadCompressedTextureSequence( char *fileName )
{
	short texNum;
	char	*fileBuffer;
	long	memOffset;
	long	fileSize, numSectors;
	long	fileHandle;
	static TIMlist timList;
	long	timSize, timTotalSize;
	int	CPURasters;

	printf("\nLoading: %s \n", fileName);

   fileSize = FileLength( fileName );
   if ( fileSize <= 0 )
   {
      sprintf( errorString, "Tim decompress: file %s not found\n", fileName );
      Error( errorString, Fatal );
   }

   fileBuffer = ( char* )DAlloc( heap, fileSize + 32 );

   if( !fileBuffer )
	{
		Error( "Decompress Load: Failed to allocate memory for track vertices", Fatal );
	}
   if ( LoadFile( fileName, (char *)fileBuffer ) != fileSize )
   {
		sprintf( errorString, "Decompress(): Failed to load file %s\n", fileName );
      Error( errorString, Fatal );
   }

   numTex = *(long *)fileBuffer;

   timTotalSize = 0;

   /***** Set offsets from timList.memBase & allocate memory for all tims *****/

   for ( texNum = 0 ; texNum < numTex ; texNum++ )
	{
		timList.memOffset[texNum] = timTotalSize;
		timSize = *((long *)(fileBuffer + 4 + texNum*4));
		timTotalSize += timSize;
	}

  	timList.memBase = (char *)DAlloc( heap, timTotalSize );

	for ( texNum = 0 ; texNum < numTex ; texNum++ )
	{
		timList.memOffset[texNum] += ( long )timList.memBase;
	}
	CPURasters=GsGetVcount();

	ExpandData( fileBuffer + 4 + numTex*4, timList.memBase );
	CPURasters=GsGetVcount();

	DFree( heap, fileBuffer );			/* free compressed file memory */

	return( &timList );
}
