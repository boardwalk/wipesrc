#include "standard.h"
#include <stdio.h>
#include <string.h>

#include "malloc.h"
#include "tim.h"
#include "object.h"
#include "file.h"
#include "skeleton.h"
#include "track.h"
#include "ttf.h"
#include "ships.h"
#include "weapon.h"
#include "control.h"
#include "droid.h"
#include "camera.h"
#include "draw.h"
#include "scene.h"
#include "main.h"
#include "sound.h"
#include "combat.h"
#include "menus.h"
#include "hiscores.h"
#include "hud.h"
#include "fileops.h"
#include "init.h"
#include "sparks2.h"
#include "global.h"
#include "error.h"
#include "radscn.h"
#include "ajs.h"
#include "lzss.h"

int32_t skyPos[15] = {-820, -2520, -1930, -5000, -5000, 0, -2260, -40, -2700, 0, -240, -2120, -2700, 0, 0};

#define	GsGetVcount()	0;

int16_t InitScene(Object** prm, int16_t *prmCount, Skeleton* camPos, ConfigData *gameData)
{
	int16_t    sceneTextures;
   int16_t    skyTextures;
	int16_t    i, j;
   Object*  obj;
   int16_t    sky;
	TIMlist  *timPtr, *skyTimPtr;

   sceneTextures = TextureTableCount;
	getPath(trackPath, gameData->actTrackNum, "/scene.cmp");
	timPtr = LoadCompressedTextureSequence( trackPath );
   LoadCmpFiles( timPtr );

   skyTextures = TextureTableCount;
	getPath(trackPath, gameData->actTrackNum, "/sky.cmp");
	skyTimPtr = LoadCompressedTextureSequence( trackPath );
   LoadCmpFiles( skyTimPtr );

   for ( i=0; i<MaxUnique; i++ )
   {
      for ( j=0; j<MaxObjects; j++ )
      {
         sharedPrims[ i ][ j ] = NULL;
      }
   }

	getPath(trackPath, gameData->actTrackNum, "/scene.prm");

/*
   objectTable = LoadPrm( trackPath , sceneTextures );
*/
   prm[ (*prmCount)++ ] = LoadPrm( trackPath , sceneTextures );

#if 0
   printf( "Heap free after scene load: %d\n", heap->free );
#endif

   for ( i=0; i<*prmCount; i++ )
   {
      obj = prm[ i ];
      while ( obj )
      {
         obj->skeleton->super = camPos;
         obj = obj->next;
      }
   }


   sky = *prmCount;
	getPath(trackPath, gameData->actTrackNum, "/sky.prm");
   prm[ (*prmCount)++ ] = LoadPrm( trackPath , skyTextures );
   obj = prm[ sky ];
/*   while ( obj )	*/
   {
   	obj->skeleton->super = camPos->super;
   /*   obj = obj->next; */
   }
	SetSkeletonPosition( prm[sky]->skeleton,
		0,
		skyPos[trackNo-1],
		0);
#if 0
   printf( "Heap free after sky load: %d\n", heap->free );
#endif

   SceneRadiusCheck( prm[0] );

#if 0
 	printf( "Heap free after Scene Radius Check: %d\n", heap->free );
#endif

	return(sky);
}

void getPath(char* trackPath, char trackNo, const char* name)
{
	char				No[ 4 ];

	strcpy( trackPath, "wipeout/track" );
	if (trackNo <= 9)
		sprintf( No, "%02d", trackNo );
	else
		sprintf( No, "%2d", trackNo );
	strcat( trackPath, No);
	strcat( trackPath, name);
}

void LoadCmpFiles( TIMlist *timPtr )
{
	int16_t       i;
	Texture*    texture;

	for (i = 0; i < numTex; i++)
	{
		texture = NewLoadTexture( timPtr->memOffset[i], 1 );
		if ( texture )
		{
			TextureTable[ TextureTableCount++ ] = texture;
		}

		if ( TextureTableCount >= TextureTableMax )
		{
			printf( "Wtl.c:LoadWtlFiles(): TextureTable is full\n" );
		}
	}
	DFree( heap, (char *)( timPtr->memBase ) );
#if 0
	printf( "Heap free after tex all free: %d\n", heap->free );
#endif
}


Texture* NewLoadTexture( intptr_t timPtr, int16_t translucent )
{
	Tim*        tim;
	TimClut4*   tc4;
	TimClut8*   tc8;
	Texture*		texture;
	RECT        rect;
	CLUT			cluts[256];			// PCwipeout

	int32_t         x,y;

#if LoadMessages
	//   printf( "Loading Tim File : %s\n", name );
#endif

	tim = (Tim*) timPtr;

	if ( ClutType( tim ) == Clut4Bit )
	{
		tc4 = ( TimClut4* )( tim );
		texture = ( Texture* )DAlloc( heap, sizeof( Texture ) );

		texture->type = CLUT4;

		if ( ( !tc4->textureX ) && ( !tc4->textureY ) )
		{
			tc4->clutX = clutX;
			tc4->clutY = clutY;

			tc4->clutW = 16;
			tc4->clutH = 1;

			tc4->textureX = textureX;
			tc4->textureY = textureY;

			clutX += 16;
			if ( clutX >= 384 )
			{
				clutX = 320;
				clutY += 1;
			}

			textureY += 32;
			if ( textureY >= 256 )
			{
				textureX += 8;
				textureY = 0;
			}

		}

		texture->clutX = tc4->clutX;
		texture->clutY = tc4->clutY;
		texture->clutW = tc4->clutW;
		texture->clutH = tc4->clutH;

		texture->textureX = tc4->textureX;
		texture->textureY = tc4->textureY;
		texture->textureW = tc4->textureW;
		texture->textureH = tc4->textureH;

		x = tc4->textureX - TextureHOffset( tc4->textureX );
		y = tc4->textureY - TextureVOffset( tc4->textureY );

		texture->u0 = ( x << 2 );
		texture->v0 = ( y );
		texture->u1 = ( ( x + tc4->textureW ) << 2 ) - 1;
		texture->v1 = ( y );
		texture->u2 = ( x << 2 );
		texture->v2 = ( y + tc4->textureH ) - 1;
		texture->u3 = ( ( x + tc4->textureW ) << 2 ) - 1;
		texture->v3 = ( y + tc4->textureH ) - 1;

		texture->tsb = TSB( Clut4Bit, translucent, TPAGE( texture->textureX, texture->textureY ) );
		texture->cba = CBA( texture->clutX >> 4, texture->clutY );

		rect.x = tc4->clutX;
		rect.y = tc4->clutY;
		rect.w = tc4->clutW;
		rect.h = tc4->clutH;
		LoadClut4( &rect, ( uint16_t* )( &tc4->clut ) , cluts);  // PCwipeout
		//      LoadImage( &rect, ( uint32_t* )( &tc4->clut ) );
		DrawSync( 0 );

		rect.x = tc4->textureX;
		rect.y = tc4->textureY;
		rect.w = tc4->textureW;
		rect.h = tc4->textureH;
		LoadImage4( &rect, ( uint32_t* )( tc4+1 ), cluts );  // PCwipeout
		//      LoadImage( &rect, ( uint32_t* )( tc4+1 ) );
		DrawSync( 0 );
	}
	else if ( ClutType( tim ) == Clut8Bit )
	{
		tc8 = ( TimClut8* )( tim );
		texture = ( Texture* )DAlloc( heap, sizeof( Texture ) );

		texture->type = CLUT8;

		if ( ( !tc8->textureX ) && ( !tc8->textureY ) )
		{
			Error( "tim.c:LoadTexture(): Cannot auto place 8 bit textures", Fatal );
		}

		texture->clutX = tc8->clutX;
		texture->clutY = tc8->clutY;
		texture->clutW = tc8->clutW;
		texture->clutH = tc8->clutH;

		texture->textureX = tc8->textureX;
		texture->textureY = tc8->textureY;
		texture->textureW = tc8->textureW;
		texture->textureH = tc8->textureH;

		x = tc8->textureX - TextureHOffset( tc8->textureX );
		y = tc8->textureY - TextureVOffset( tc8->textureY );

		texture->u0 = ( x << 1 );
		texture->v0 = ( y );
		texture->u1 = ( ( x + tc8->textureW ) << 1 ) - 1;
		texture->v1 = ( y );
		texture->u2 = ( x << 1 );
		texture->v2 = ( y + tc8->textureH ) - 1;
		texture->u3 = ( ( x + tc8->textureW ) << 1 ) - 1;
		texture->v3 = ( y + tc8->textureH ) - 1;

		texture->tsb = TSB( Clut8Bit, translucent, TPAGE( texture->textureX, texture->textureY ) );
		texture->cba = CBA( texture->clutX >> 4, texture->clutY );

		rect.x = tc8->clutX;
		rect.y = tc8->clutY;
		rect.w = tc8->clutW;
		rect.h = tc8->clutH;
		LoadClut8( &rect, ( uint16_t* )( &tc8->clut ),cluts );
		//      LoadImage( &rect, ( uint32_t* )( &tc8->clut ) );
		DrawSync( 0 );

		rect.x = tc8->textureX;
		rect.y = tc8->textureY;
		rect.w = tc8->textureW;
		rect.h = tc8->textureH;
		LoadImage8( &rect, ( uint32_t* )( tc8+1 ),cluts );
		//      LoadImage( &rect, ( uint32_t* )( tc8+1 ) );
		DrawSync( 0 );
	}
	else
	{
		texture = NULL;
	}
	return( texture );
}

TIMlist *LoadCompressedTextureSequence( const char *fileName )
{
	int16_t texNum;
	char	*fileBuffer;
	int32_t	fileSize;
	static TIMlist timList;
	int32_t	timSize, timTotalSize;
	int32_t	CPURasters;

//	GsClearVcount();

//	printf("\nLoading: %s \n", fileName);

#if 0
 	printf( "Heap free before tex load: %d\n", heap->free );
#endif

   fileSize = FileLength( fileName );
   if ( fileSize <= 0 )
   {
      sprintf( errorString, "Tim decompress: file %s not found\n", fileName );
      Error( errorString, Fatal );
   }

   fileBuffer = ( char* )DAlloc( heap, fileSize + 32 );
#if 0
 	printf( "Heap free after compress load: %d\n", heap->free );
#endif
   if( !fileBuffer )
	{
		Error( "Decompress Load: Failed to allocate memory for track vertices", Fatal );
	}
   if ( LoadFile( fileName, (char *)fileBuffer ) != fileSize )
   {
		sprintf( errorString, "Decompress(): Failed to load file %s\n", fileName );
      Error( errorString, Fatal );
   }

   numTex = *(int32_t *)fileBuffer;

   timTotalSize = 0;

   /***** Set offsets from timList.memBase & allocate memory for all tims *****/

   for ( texNum = 0 ; texNum < numTex ; texNum++ )
	{
		timList.memOffset[texNum] = timTotalSize;
		timSize = *((int32_t *)(fileBuffer + 4 + texNum*4));
		timTotalSize += timSize;
	}

  	timList.memBase = (char *)DAlloc( heap, timTotalSize );

#if 0
 	printf( "Heap free after temp tex alloc: %d\n", heap->free );
#endif

	for ( texNum = 0 ; texNum < numTex ; texNum++ )
	{
		timList.memOffset[texNum] += ( intptr_t )timList.memBase;
	}
	CPURasters=GsGetVcount();

/*	printf("Decompressing: %s\n", fileName);*/

//	GsClearVcount();
	ExpandData( (uint8_t *)fileBuffer + 4 + numTex*4, (uint8_t *)timList.memBase );
	CPURasters=GsGetVcount();

	DFree( heap, fileBuffer );			/* free compressed file memory */

	return( &timList );

}
